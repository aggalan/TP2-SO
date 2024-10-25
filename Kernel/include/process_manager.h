#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdint.h>
// #include "registers.h"
#include <sys/types.h>
#include "process.h"

// #define BLOCKED 0
// #define RUNNING 2
// #define READY 1
// #define ZOMBIE 3
// #define KILLED 4

#define STACK 4096
#define MAX_PROCESSES 64

enum State { BLOCKED,
            READY,
            RUNNING,
            ZOMBIE,
            KILLED,
            WAITING,
            EXITED };

typedef struct child_node child_node;

typedef struct PCB{
    char * name;
    char ** argv;
    uint64_t argc;
    pid_t pid;
    pid_t ppid;
    uint64_t rsp;
    uint64_t base;
    uint64_t rip;
    int ground;
    int state;
    int priority;
    int is_waited;
    struct child_node * child;
}PCB;

typedef struct child_node{
    PCB * pcb;
    struct child_node * next;
}child_node;


pid_t create_process(uint64_t fn, int priority, uint64_t argc, char **argv, int ground);

pid_t kill_process();

void annihilate();

void add_child(PCB * child, PCB * parent);

pid_t kill_process_pid(pid_t pid);

pid_t block_process(pid_t pid);

pid_t unblock_process(pid_t pid);

pid_t wait_pid(pid_t pid_to_wait);

PCB * get_idle();

void hash_map_init();

int add_pcb(pid_t key, PCB * value);

int remove_pcb(pid_t key);

PCB * find_pcb(pid_t key);

void print_processes();



// #define STACK 4096
// #define MAX_PROCESSES 64

// enum State { BLOCKED,
//             READY,
//             RUNNING,
//             ZOMBIE,
//              KILLED };

// //State usado en test_processes, no deberia ser un enum

// typedef struct p_memory_block {
//     void * base_ptr;
// //    uint64_t size;
//     void * current;
//     void * base;
// }p_memory_block;

// typedef struct process {
//     p_memory_block * heap;
//     p_memory_block * stack;
//     char * name;
// //    char is_foreground;
//     uint8_t state;
//     pid_t pid;
//     pid_t parent_pid;
// }process;

// typedef struct PCB{
//     uint64_t ticks;
//     process * process;
//     uint8_t priority;
// }PCB;





// pid_t create_process(void * fn, uint8_t prio, uint64_t argc, char ** argv);
// PCB * create_idle_process();

// pid_t kill_process_pid(pid_t pid_to_free);

// pid_t kill_process();

// void list_processes();

// pid_t block_process(pid_t pid_to_block);

// pid_t unblock_process(pid_t pid_to_unblock);

// int wait_pid(pid_t pid_to_wait);

// //void * create_context(void * stack_base_ptr, void * fn_ptr, uint64_t argc, char ** argv); //esto hace colombia creo y no se para que los otros 2 argumentos

 #endif