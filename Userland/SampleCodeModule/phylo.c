// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
int last = 0;

int mutex_id;
int rw_id;
int add_remove;
int fork_states[MAX_PHYLO];
int fork_sem_ids[MAX_PHYLO];
int phylo_qty = 0;
int phylo_pid[MAX_PHYLO];

void add_philosopher();
void remove_phylo();
void philosopher(uint64_t id);
void take_forks(uint64_t id);
void put_forks(uint64_t id);
void test(uint64_t id);
void think(uint64_t id);
void eat(uint64_t id);
void print_phylo();
void handle();
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
}
int eating = 0;

int phylo_init()
{
    initialize();
    mutex_id = call_sem_init(1);
    rw_id = call_sem_init(1);
    add_remove = call_sem_init(1);
    for (int i = 0; i < MAX_PHYLO; i++)
    {
        fork_sem_ids[i] = call_sem_init(0);
        fork_states[i] = THINKING;
    }
    print(WHITE, "Philosopher's problem initialized.\n Press 'a' to add a philosopher, 'r' to remove one, 'q' to quit.\n");

    for (int i = 0; i < INITIAL_PHYLO; i++)
    {
        add_philosopher();
        phylo_qty++;
    }
    char *argv[] = {"phylo"};

    call_create_process(handle, 0, 1, argv, 1);

    while (!last)
    {
        for (int i = 0; i < num_philosophers; i++)
        {
            phylo_qty--;
            call_kill(phylo_pid[i]);
            call_sem_close(fork_sem_ids[i]);
        }
    }
    call_sem_close(mutex_id);
    call_sem_close(rw_id);
    call_sem_close(add_remove);
    return 0;
}

void handle()
{
    char c;
    int times_eaten = 0;
    int pos = 0;
    pos = call_get_buffer_position();
    while (!last)
    {
        if (times_eaten < eating)
        {
            times_eaten = eating;
            print_phylo();
        }
        if (pos <= call_get_buffer_position())
        {
            c = call_get_char_at(pos++ - 1);
        }
        if (c == 'a' || c == 'A')
        {
            if (num_philosophers < MAX_PHYLO)
            {
                add_philosopher();
                phylo_qty++;
            }
        }
        else if (c == 'r' || c == 'R')
        {
            call_sem_wait(mutex_id);
            if (num_philosophers > MIN_PHYLO)
            {
                num_philosophers--;
                remove_phylo();
            }
            else
            {
                print(WHITE, "Min philosophers reached.\n");
            }
        }
        else if (c == 'q' || c == 'Q')
        {
            last = 1;
        }
    }
}

void add_philosopher()
{
    call_sem_wait(mutex_id);
    int philosopher_num = num_philosophers;
    fork_states[philosopher_num] = THINKING;

    if (num_philosophers < MAX_PHYLO)
    {
        num_philosophers++;
        print(WHITE, "Philosopher %d added.\n", num_philosophers);
    }
    else
    {
        print(WHITE, "Max philosophers reached.\n");
    }
    call_sem_post(add_remove);
}

void remove_phylo()
{
    call_sem_wait(add_remove);
    if (num_philosophers > MIN_PHYLO)
    {
        int philosopher_num = num_philosophers - 1;
        if (fork_states[num_philosophers] == EATING)
        {
            call_sem_post(add_remove);
            call_sleepms(100);
            call_sem_wait(add_remove);
        }
        int left_phylo = left(num_philosophers);
        int right_phylo = right(num_philosophers);
        test(left_phylo);
        test(right_phylo);
        call_kill(phylo_pid[num_philosophers]);
        fork_states[num_philosophers] = THINKING;
        call_sem_close(fork_sem_ids[num_philosophers]);
        num_philosophers--;
    }
    call_sem_post(add_remove);
}

void philosopher(uint64_t id)
{
    while (!last)
    {
        think(id);
        take_forks(id);
        eat(id);
        put_forks(id);
    }
}

void take_forks(uint64_t id)
{
    call_sem_wait(mutex_id);
    fork_states[id] = HUNGRY;
    call_sem_post(rw_id);
    test(id);
    if (fork_states[id] != EATING)
    {
        call_sem_wait(mutex_id);
        call_sem_wait(fork_sem_ids[id]);
    }
    else
    {
        call_sem_post(mutex_id);
    }
}

void put_forks(uint64_t id)
{
    call_sem_wait(mutex_id);
    fork_states[id] = THINKING;
    call_sem_post(rw_id);
    test(left(id));
    test(right(id));
    call_sem_post(mutex_id);
}

void test(uint64_t id)
{
    if (fork_states[id] == HUNGRY && fork_states[left(id)] != EATING && fork_states[right(id)] != EATING)
    {
        fork_states[id] = EATING;
        call_sem_post(fork_sem_ids[id]);
    }
}

void think(uint64_t id)
{
    call_sleepms(100);
}

void eat(uint64_t id)
{
    if (fork_states[id] == EATING)
    {
        eating++;
        call_sleepms(100);
    }
}

void print_phylo()
{
    call_sem_wait(mutex_id);
    print(WHITE, "State of the table with %d philosophers:\n", num_philosophers);
    for (int i = 0; i < num_philosophers; i++)
    {
        if (fork_states[i] == EATING)
        {
            print(WHITE, "E  ");
        }
        else
        {
            print(WHITE, "T  ");
        }
    }
    print(WHITE, "\n");
    call_sem_post(mutex_id);
}