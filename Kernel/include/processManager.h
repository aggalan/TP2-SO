#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdint.h>
#include "registers.h"

typedef struct PCB{
    uint64_t ticks
    void * process;
    uint8_t priority;
    registerStruct regstate;
}PCB;

typedef struct p_memory_block {
    void * base_ptr;
    uint64_t size;
    void * current;
}p_memory_block;

typedef struct process {
    p_memory_block * heap;
    p_memory_block * stack;
    char * name;
    char is_foreground
    uint8_t state;
    uint64_t pid;
    uint64_t parent_pid;
}process;


#endif