#include <stdint.h>
#include <sys/types.h>
#include "processManager.h"
#include "collections.h"
#include "memoryManager.h"
#include "scheduler.h"

static pid_t pid = 0;


pid_t create_process(void * fn, uint64_t argc, char ** argv) {

    PCB * pcb = (PCB *)mm_malloc(sizeof(PCB));
    if (pcb == NULL) {
        return -1;
    }
    pcb->priority = 1;
    process * p = (process *)mm_malloc(sizeof(process));
    if (p == NULL) {
        return -1;
    }
    p->name = argv[0];
    p->state = READY;
    p->pid = pid++;
    p->parent_pid = running_process();

    p_memory_block * heap = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
    if (heap == NULL) {
        return -1;
    }
    heap->base_ptr = fn;

    p->heap = heap;

    p_memory_block * stack = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
    if (stack == NULL) {
        return -1;

    return p->pid;
    }

    void * stack_base = mm_malloc(STACK);
    if (stack_base == NULL) {
        return -1;
    }
    stack_base += STACK - 1;
    stack->base_ptr = stack_base;
    stack->current = create_context(stack->base_ptr, heap->base_ptr); // aca se va a mover el rsp asi q me devuelve el current

    p->stack = stack;


    pcb->process = p;

    add_process(pcb);

    return p->pid;
}

void kill_process_pid(pid_t pid) {
    remove_process(pid); //aca faltaria todo
}

