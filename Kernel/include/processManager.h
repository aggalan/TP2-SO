#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdint.h>
#include "registers.h"
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
             KILLED };

typedef struct p_memory_block {
    void * base_ptr;
//    uint64_t size;
    void * current;
}p_memory_block;

typedef struct process {
    p_memory_block * heap;
    p_memory_block * stack;
    char * name;
//    char is_foreground;
    uint8_t state;
    pid_t pid;
    pid_t parent_pid;
}process;

typedef struct PCB{
    uint64_t ticks;
    process * process;
    uint8_t priority;
}PCB;





pid_t create_process(void * fn, uint64_t argc, char ** argv);
PCB * create_idle_process();

pid_t kill_process_pid(pid_t pid);

pid_t kill_process();

void list_processes();

pid_t block_process(pid_t pid);

pid_t unblock_process(pid_t pid);

int wait_pid(pid_t pid);

//void * create_context(void * stack_base_ptr, void * fn_ptr, uint64_t argc, char ** argv); //esto hace colombia creo y no se para que los otros 2 argumentos

#endif