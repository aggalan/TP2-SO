#include "include/lib.h"
#include "include/usr_sys_calls.h"
#include "include/phylo.h"

#define MAX_PHYLO 20
#define MIN_PHYLO 2
#define INITIAL_PHYLO 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define WHITE 0xFFFFFF

int num_philosophers = 0;
volatile int last = 0;
volatile int eating_count = 0;

int mutex_id;
int add_remove;
int fork_states[MAX_PHYLO];
int fork_sem_ids[MAX_PHYLO];
int phylo_pid[MAX_PHYLO];
int handler_pid = 0; // Store handler process ID

void cleanup();
void handle(uint64_t id);
void add_philosopher();
void remove_phylo();
void philosopher(uint64_t id);
void take_forks(uint64_t id);
void put_forks(uint64_t id);
void test(uint64_t id);
void think(uint64_t id);
void eat(uint64_t id);
void print_phylo();
int left(int i);
int right(int i);
void initialize();
int phylo_init();

int left(int i)
{
    return (i + num_philosophers - 1) % num_philosophers;
}

int right(int i)
{
    return (i + 1) % num_philosophers;
}

void initialize()
{
    num_philosophers = 0;
    last = 0;
    eating_count = 0;
}

int phylo_init()
{
    initialize();

    mutex_id = call_sem_init(1);
    add_remove = call_sem_init(1);

    for (int i = 0; i < MAX_PHYLO; i++)
    {
        fork_sem_ids[i] = call_sem_init(0);
        fork_states[i] = THINKING;
    }

    print(WHITE, "Philosopher's problem initialized.\n Press 'a' to add a philosopher, 'r' to remove one, 'q' to quit.\n");

    char *argv[] = {"handler"};
    // Add initial philosophers
    for (int i = 0; i < INITIAL_PHYLO; i++)
    {
        add_philosopher();
        call_sleepms(50);
    }

    handler_pid = call_create_process(handle, 0, 1, argv, 1);

    // Main loop waiting for termination
    while (!last && call_get_parent_pid() != 0)
    {
        call_sleepms(100);
    }

    // Cleanup all processes and resources
    cleanup();
    
    // Ensure we return to shell
    return 0;
}

void cleanup()
{
    // First signal all philosophers to stop and unblock any waiting ones
    last = 1;
    
    // Force release all semaphores that might be blocking
    call_sem_post(mutex_id);
    call_sem_post(add_remove);
    for (int i = 0; i < num_philosophers; i++)
    {
        call_sem_post(fork_sem_ids[i]);
    }
    
    // Give processes time to notice last flag
    call_sleepms(100);
    
    // Kill handler first
    if (handler_pid > 0)
    {
        call_kill(handler_pid);
        call_sleepms(50);  // Give handler time to exit
    }

    // Kill all philosopher processes
    for (int i = 0; i < num_philosophers; i++)
    {
        if (phylo_pid[i] > 0)
        {
            call_kill(phylo_pid[i]);
        }
    }

    // Final cleanup of semaphores
    for (int i = 0; i < MAX_PHYLO; i++)
    {
        call_sem_close(fork_sem_ids[i]);
    }

    call_sem_close(mutex_id);
    call_sem_close(add_remove);
}

void handle(uint64_t id)
{
    char c;

    while (!last)
    {
        // Check if parent process is still alive
        if (call_get_parent_pid() == 0)
        {
            last = 1;
            return;  // Exit immediately
        }

        if (getC(&c) != EOF && c != 0)
        {
            if (c == 'q' || c == 'Q')
            {
                // Signal termination
                last = 1;
                
                // Force release all semaphores that might be blocking
                call_sem_post(mutex_id);
                call_sem_post(add_remove);
                for (int i = 0; i < num_philosophers; i++)
                {
                    call_sem_post(fork_sem_ids[i]);
                }
                
                // Exit handler immediately
                return;
            }
            
            // Only process other commands if not terminating
            if (!last)
            {
                if (c == 'a' || c == 'A')
                {
                    if (num_philosophers < MAX_PHYLO)
                    {
                        add_philosopher();
                    }
                    else
                    {
                        // print(WHITE, "Max philosophers reached (%d)\n", MAX_PHYLO);
                    }
                }
                else if (c == 'r' || c == 'R')
                {
                    if (num_philosophers > MIN_PHYLO)
                    {
                        remove_phylo();
                    }
                    else
                    {
                        // print(WHITE, "Min philosophers reached (%d)\n", MIN_PHYLO);
                    }
                }
            }
        }
    }
}
void philosopher(uint64_t id)
{
    while (!last)
    {
        // Check if parent process is still alive
        if (call_get_parent_pid() == 0)
        {
            break;
        }

        think(id);
        
        // Check last before taking forks
        if (last) break;
        
        take_forks(id);
        
        // Check again after getting forks
        if (last)
        {
            put_forks(id);
            break;
        }
        
        eat(id);
        put_forks(id);
    }
}


void add_philosopher()
{
    call_sem_wait(add_remove);

    if (num_philosophers < MAX_PHYLO)
    {
        int philosopher_num = num_philosophers;
        char *argv[] = {"phil"};

        // Create the philosopher process
        phylo_pid[philosopher_num] = call_create_process(philosopher, 0, philosopher_num, argv, 1);

        if (phylo_pid[philosopher_num] > 0)
        {
            fork_states[philosopher_num] = THINKING;
            num_philosophers++;
        }
        else
        {
            print(WHITE, "Failed to create philosopher process\n");
        }
    }

    call_sem_post(add_remove);
}

void remove_phylo()
{
    call_sem_wait(add_remove);

    if (num_philosophers > MIN_PHYLO)
    {
        int philosopher_num = num_philosophers - 1;

        // Wait for philosopher to finish eating
        call_sem_wait(mutex_id);

        if (fork_states[philosopher_num] == EATING)
        {
            // print(WHITE, "Philosopher is eating, try again later\n");
            call_sem_post(mutex_id);
            call_sem_post(add_remove);
            return;
        }

        if (call_kill(phylo_pid[philosopher_num]) == 0)
        {
            num_philosophers--;
            // print(WHITE, "Removed philosopher \n");
            // print_phylo();
        }

        call_sem_post(mutex_id);
    }

    call_sem_post(add_remove);
}

void take_forks(uint64_t id)
{
    // Check last before waiting on mutex
    if (last) return;
    
    call_sem_wait(mutex_id);
    fork_states[id] = HUNGRY;
    test(id);
    call_sem_post(mutex_id);

    if (!last && fork_states[id] != EATING)  // Check last before waiting
    {
        call_sem_wait(fork_sem_ids[id]);
    }
}

void put_forks(uint64_t id)
{
    call_sem_wait(mutex_id);
    fork_states[id] = THINKING;
    // print_phylo();
    test(left(id));
    test(right(id));
    call_sem_post(mutex_id);
}

void test(uint64_t id)
{
    if(last)
    return;
    if (fork_states[id] == HUNGRY &&
        fork_states[left(id)] != EATING &&
        fork_states[right(id)] != EATING)
    {
        fork_states[id] = EATING;
        print_phylo();
        call_sem_post(fork_sem_ids[id]);
    }
}

void think(uint64_t id)
{
    call_sleepms(100 + (call_get_ticks() % 200));
}

void eat(uint64_t id)
{
    call_sleepms(100 + (call_get_ticks() % 200));
}

void print_phylo()
{
    print(WHITE, "Table state (%d philosophers): ", num_philosophers);
    for (int i = 0; i < num_philosophers; i++)
    {
        switch (fork_states[i])
        {
        case THINKING:
            print(WHITE, "T ");
            break;
        case HUNGRY:
            print(WHITE, "H ");
            break;
        case EATING:
            print(WHITE, "E ");
            break;
        }
    }
    print(WHITE, "\n");
}