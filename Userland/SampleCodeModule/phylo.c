#include "include/lib.h"
#include "include/usr_sys_calls.h"
#include "include/phylo.h"
#include "tests/test_util.h"
#include <stddef.h>


#define MAX_PHYLO 10
#define MIN_PHYLO 3
#define WHITE 0xFFFFFF
#define QUIT 'q'
#define ADD 'a'
#define REMOVE 'r'
#define CLEAR 'c'
#define THINKING_TIME 500
#define EATING_TIME 500
#define MAX_BUFFER 32
#define LEFT(i) ((i + global_count - 1) % global_count)
#define RIGHT(i) ((i + 1) % global_count)

typedef enum { THINKING, HUNGRY, EATING } State;

int global_count;
int inner_count;

State philo_state[MAX_PHYLO];
int philo_pid[MAX_PHYLO];
int sem_philo[MAX_PHYLO];
int mutex;

int phylo_init();
int add_philo();
int remove_philo();
int philo(int argc, char ** argv);
void take_forks(int i);
void put_forks(int i);
void test(int i);
void render();
void print_phylo();

int phylo_init()
{
    global_count = 0;
    inner_count = 0;

    if((mutex = call_sem_init(1)) == -1){
        return -1;
    }

    for(int i = 0; i < MAX_PHYLO; i++){
        philo_pid[i] = -1;
    }

    for(int i = 0; i < MIN_PHYLO; i++){
        add_philo();
    }

    char cmd = '\0';
    do{
        getC(&cmd);
        switch (cmd)
        {
        case ADD:
            if(global_count <= MAX_PHYLO){
                add_philo();
            }else{
                print(WHITE, "Maximum of 10 philosophers reached\n");
            }
            break;
        case REMOVE:
            if(global_count > MIN_PHYLO){
                remove_philo();
            }else{
                print(WHITE, "Minimum of 3 philosophers is required\n");
            }
            break;
        }
    }while(cmd != QUIT);

    while(global_count > 0){
        remove_philo();
    }

    call_sem_close(mutex);
    return 0;
}

int add_philo(){
    call_sem_wait(mutex);
    int id = global_count;
    if((sem_philo[id] = call_sem_init(0)) == -1){
        call_sem_post(mutex);
        print(WHITE, "Error creating semaphore\n");
        return -1;
    }
    philo_state[id] = THINKING;
    char buffer[MAX_BUFFER] = {0};
    int_to_str(id, buffer);
    char *argv[2] = {"philosopher", buffer};
    philo_pid[id] = call_create_process(philo, 0, 2, argv, 1);
    global_count++;
    print(WHITE, "Philosopher %d added\n", global_count);
    putC('\n', WHITE);
    call_sem_post(mutex);
    return -1 * !(philo_pid[id] != -1);
}

int remove_philo(){
    call_sem_wait(mutex);
    int id = global_count - 1;
    while(philo_state[LEFT(id)] == EATING || philo_state[RIGHT(id)] == EATING){
        call_sem_post(mutex);
        call_sem_wait(sem_philo[id]);
        call_sem_wait(mutex);
    }
    call_kill(philo_pid[id]);
    call_waitpid(philo_pid[id]);
    call_sem_close(sem_philo[id]);
    print(WHITE, "Philosopher %d removed\n", global_count);
    putC('\n', WHITE);
    global_count--;
    philo_pid[id] = -1;
    call_sem_post(mutex);
    return 0;
}


int philo(int argc, char ** argv){
    // print(WHITE, " %s \n",argv[1]);
    // int i = satoi(argv[1]);
    int i = inner_count++;
    philo_state[i] = THINKING;
    while(1){
        call_sleepms(THINKING_TIME);
        take_forks(i);
        call_sleepms(EATING_TIME);
        put_forks(i);
    }

    return 0;
}

void take_forks(int i){
    call_sem_wait(mutex);
    philo_state[i] = HUNGRY;
    test(i);
    call_sem_post(mutex);
    call_sem_wait(sem_philo[i]);
}

void put_forks(int i){
    call_sem_wait(mutex);
    philo_state[i] = THINKING;
    test(LEFT(i));
    test(RIGHT(i));
    call_sem_post(mutex);
}

void test(int i){
    if(philo_state[i] == HUNGRY && philo_state[LEFT(i)] != EATING && philo_state[RIGHT(i)] != EATING){
        philo_state[i] = EATING;
        render();
        call_sem_post(sem_philo[i]);
    }
}

void render(){
    print_phylo();
    // if(line){
    //     call_clear();
    // }
    // const static char letters[] = {' ', 'E', '.', '.'};
    // int to_write = 0;
    // for(int i = 0; i < num_philosophers; i++){
    //     if(letters[philo_state[i]] != ' '){
    //          to_write = 1;
    //         print(WHITE, "%c", letters[philo_state[i]]);
    //     }
    
    // }
    // if(to_write){
    //     print(WHITE, "\n");
    // }
}

void print_phylo()
{
    print(WHITE, "Table state (%d philosophers): ", global_count);
    for (int i = 0; i < global_count; i++)
    {
        switch (philo_state[i])
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



// int mutex_id;
// int fork_states[MAX_PHYLO];
// int fork_sem_ids[MAX_PHYLO];
// int phylo_pid[MAX_PHYLO];
// int handler_pid = 0; // Store handler process ID

// void cleanup();
// void handle(uint64_t id);
// void add_philosopher();
// void remove_phylo();
// void philosopher(uint64_t id);
// void take_forks(uint64_t id);
// void put_forks(uint64_t id);
// void test(uint64_t id);
// void think(uint64_t id);
// void eat(uint64_t id);
// void print_phylo();
// int left(int i);
// int right(int i);
// void initialize();
// int phylo_init();

// int left(int i)
// {
//     return (i + num_philosophers - 1) % num_philosophers;
// }

// int right(int i)
// {
//     return (i + 1) % num_philosophers;
// }

// void initialize()
// {
//     num_philosophers = 0;
//     last = 0;
//     eating_count = 0;
// }

// int phylo_init()
// {
//     initialize();

//     mutex_id = call_sem_init(1);
//     add_remove = call_sem_init(1);

//     for (int i = 0; i < MAX_PHYLO; i++)
//     {
//         fork_sem_ids[i] = call_sem_init(0);
//         fork_states[i] = THINKING;
//     }

//     print(WHITE, "Philosopher's problem initialized.\n Press 'a' to add a philosopher, 'r' to remove one, 'q' to quit.\n");

//     char *argv[] = {"handler"};
//     // Add initial philosophers
//     for (int i = 0; i < INITIAL_PHYLO; i++)
//     {
//         add_philosopher();
//         call_sleepms(50);
//     }

//     handler_pid = call_create_process(handle, 0, 1, argv, 1);

//     // Main loop waiting for termination
//     while (!last && call_get_parent_pid() != 0)
//     {
//         call_sleepms(100);
//     }

//     // Cleanup all processes and resources
//     cleanup();
    
//     // Ensure we return to shell
//     return 0;
// }

// void cleanup()
// {
//     // First signal all philosophers to stop and unblock any waiting ones
//     last = 1;
    
//     // Force release all semaphores that might be blocking
//     call_sem_post(mutex_id);
//     call_sem_post(add_remove);
//     for (int i = 0; i < num_philosophers; i++)
//     {
//         call_sem_post(fork_sem_ids[i]);
//     }
    
//     // Give processes time to notice last flag
//     call_sleepms(100);
    
//     // Kill handler first
//     if (handler_pid > 0)
//     {
//         call_kill(handler_pid);
//         call_sleepms(50);  // Give handler time to exit
//     }

//     // Kill all philosopher processes
//     for (int i = 0; i < num_philosophers; i++)
//     {
//         if (phylo_pid[i] > 0)
//         {
//             call_kill(phylo_pid[i]);
//         }
//     }

//     // Final cleanup of semaphores
//     for (int i = 0; i < MAX_PHYLO; i++)
//     {
//         call_sem_close(fork_sem_ids[i]);
//     }

//     call_sem_close(mutex_id);
//     call_sem_close(add_remove);
// }

// void handle(uint64_t id)
// {
//     char c;

//     while (!last)
//     {
//         // Check if parent process is still alive
//         if (call_get_parent_pid() == 0)
//         {
//             last = 1;
//             return;  // Exit immediately
//         }

//         if (getC(&c) != EOF && c != 0)
//         {
//             if (c == 'q' || c == 'Q')
//             {
//                 // Signal termination
//                 last = 1;
                
//                 // Force release all semaphores that might be blocking
//                 call_sem_post(mutex_id);
//                 call_sem_post(add_remove);
//                 for (int i = 0; i < num_philosophers; i++)
//                 {
//                     call_sem_post(fork_sem_ids[i]);
//                 }
                
//                 // Exit handler immediately
//                 return;
//             }
            
//             // Only process other commands if not terminating
//             if (!last)
//             {
//                 if (c == 'a' || c == 'A')
//                 {
//                     if (num_philosophers < MAX_PHYLO)
//                     {
//                         add_philosopher();
//                     }
//                     else
//                     {
//                         // print(WHITE, "Max philosophers reached (%d)\n", MAX_PHYLO);
//                     }
//                 }
//                 else if (c == 'r' || c == 'R')
//                 {
//                     if (num_philosophers > MIN_PHYLO)
//                     {
//                         remove_phylo();
//                     }
//                     else
//                     {
//                         // print(WHITE, "Min philosophers reached (%d)\n", MIN_PHYLO);
//                     }
//                 }
//             }
//         }
//     }
// }
// void philosopher(uint64_t id)
// {
//     while (!last)
//     {
//         // Check if parent process is still alive
//         if (call_get_parent_pid() == 0)
//         {
//             break;
//         }

//         think(id);
        
//         // Check last before taking forks
//         if (last) break;
        
//         take_forks(id);
        
//         // Check again after getting forks
//         if (last)
//         {
//             put_forks(id);
//             break;
//         }
        
//         eat(id);
//         put_forks(id);
//     }
// }


// void add_philosopher()
// {
//     call_sem_wait(add_remove);

//     if (num_philosophers < MAX_PHYLO)
//     {
//         int philosopher_num = num_philosophers;
//         char *argv[] = {"phil"};

//         // Create the philosopher process
//         phylo_pid[philosopher_num] = call_create_process(philosopher, 0, philosopher_num, argv, 1);

//         if (phylo_pid[philosopher_num] > 0)
//         {
//             fork_states[philosopher_num] = THINKING;
//             num_philosophers++;
//         }
//         else
//         {
//             print(WHITE, "Failed to create philosopher process\n");
//         }
//     }

//     call_sem_post(add_remove);
// }

// void remove_phylo()
// {
//     call_sem_wait(add_remove);

//     if (num_philosophers > MIN_PHYLO)
//     {
//         int philosopher_num = num_philosophers - 1;

//         // Wait for philosopher to finish eating
//         call_sem_wait(mutex_id);

//         if (fork_states[philosopher_num] == EATING)
//         {
//             // print(WHITE, "Philosopher is eating, try again later\n");
//             call_sem_post(mutex_id);
//             call_sem_post(add_remove);
//             return;
//         }

//         if (call_kill(phylo_pid[philosopher_num]) == 0)
//         {
//             num_philosophers--;
//             // print(WHITE, "Removed philosopher \n");
//             // print_phylo();
//         }

//         call_sem_post(mutex_id);
//     }

//     call_sem_post(add_remove);
// }

// void take_forks(uint64_t id)
// {
//     // Check last before waiting on mutex
//     if (last) return;
    
//     call_sem_wait(mutex_id);
//     fork_states[id] = HUNGRY;
//     test(id);
//     call_sem_post(mutex_id);

//     if (!last && fork_states[id] != EATING)  // Check last before waiting
//     {
//         call_sem_wait(fork_sem_ids[id]);
//     }
// }

// void put_forks(uint64_t id)
// {
//     call_sem_wait(mutex_id);
//     fork_states[id] = THINKING;
//     // print_phylo();
//     test(left(id));
//     test(right(id));
//     call_sem_post(mutex_id);
// }

// void test(uint64_t id)
// {
//     if(last)
//     return;
//     if (fork_states[id] == HUNGRY &&
//         fork_states[left(id)] != EATING &&
//         fork_states[right(id)] != EATING)
//     {
//         fork_states[id] = EATING;
//         print_phylo();
//         call_sem_post(fork_sem_ids[id]);
//     }
// }

// void think(uint64_t id)
// {
//     call_sleepms(100 + (call_get_ticks() % 200));
// }

// void eat(uint64_t id)
// {
//     call_sleepms(100 + (call_get_ticks() % 200));
// }
