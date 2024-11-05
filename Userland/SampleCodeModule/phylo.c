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