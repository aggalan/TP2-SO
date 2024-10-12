#include <stdint.h>
#include <sys/types.h>
#include "processManager.h"
#include "collections.h"
#include "memoryManager.h"
#include "scheduler.h"
#include "lib.h"
#include "interrupts.h"
#include "videoDriver.h"

static pid_t pid = 0;

PCB * create_pcb(void * fn, uint64_t argc, char ** argv);
pid_t create_process(void * fn, uint64_t argc, char ** argv);
PCB * create_idle_process();


pid_t create_process(void * fn, uint64_t argc, char ** argv) {

    PCB * pcb = create_pcb(fn, argc, argv);
    if (pcb == NULL) {
        return -1;
    }

    add_process(pcb);

    return pcb->process->pid;
}

PCB * create_idle_process(){

    char ** argv = (char **)mm_malloc(sizeof(char *));
    if (argv == NULL) {
        return NULL;
    }

    argv[0] = (char *)mm_malloc(5* sizeof(char));
    if (argv[0] == NULL) {
        mm_free(argv);
        return NULL;
    }

    str_cpy(argv[0], "idle");

    return create_pcb(idle, 0, argv);
}


PCB * create_pcb(void * fn, uint64_t argc, char ** argv) {

    PCB * pcb = (PCB *)mm_malloc(sizeof(PCB));
    if (pcb == NULL) {
        return NULL;
    }

    pcb->priority = 1;

    process * p = (process *)mm_malloc(sizeof(process));
    if (p == NULL) {
        mm_free(pcb);
        return NULL;
    }

    p->name = mm_malloc((size_t)str_len(argv[0]) + 1);
    if (p->name == NULL) {
        mm_free(p);
        mm_free(pcb);
        return NULL;
    }

    str_cpy(p->name, argv[0]);

    p->state = READY;

    p->pid = pid++;

    p->parent_pid = 0;

    p->heap = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
    if (p->heap == NULL) {
        mm_free(p->name);
        mm_free(p);
        mm_free(pcb);
        return NULL;
    }

    p->heap->base_ptr = fn;

    p->heap->current = p->heap->base_ptr;
    if(p->heap->current == NULL) {
        mm_free(p->heap);
        mm_free(p->name);
        mm_free(p);
        mm_free(pcb);
        return NULL;
    }

    p->stack = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
    if (p->stack == NULL) {
        mm_free(p->heap);
        mm_free(p->name);
        mm_free(p);
        mm_free(pcb);
        return NULL;
    }

    p->stack->base_ptr = mm_malloc(STACK);
    if (p->stack->base_ptr == NULL) {
        mm_free(p->stack);
        mm_free(p->heap);
        mm_free(p->name);
        mm_free(p);
        mm_free(pcb);
        return NULL;
    }

    p->stack->base_ptr += STACK - 1;

    p->stack->current = create_context(p->stack->base_ptr, p->heap->base_ptr); 

    pcb->process = p;

    return pcb;
}

pid_t kill_process() {
    int pid = running_process();
    return kill_process_pid(pid);
}

pid_t kill_process_pid(pid_t pid) {

    return pid;

    PCB * pcb = find_process(pid);
    if (pcb == NULL) {
        return -1;
    }
    int state = pcb->process->state;
    pcb->process->state = KILLED;
    if (state == RUNNING) {
        irq00Handler();
    }

    return pcb->process->pid;
}

pid_t block_process(pid_t pid){

    return pid;

    // PCB * pcb = find_process(pid);
    // if (pcb == NULL) {
    //     return -1;
    // }
    // if(pcb->process->state == RUNNING){
    //     pcb->process->state = BLOCKED;
    //      irq00Handler();
    // }else if(pcb->process->state == READY){
    //     pcb->process->state = BLOCKED;
    // }
    // return pcb->process->pid;
}

pid_t unblock_process(pid_t pid){

    return pid;
    // PCB * pcb = find_process(pid);
    // if (pcb == NULL) {
    //     return -1;
    // }
    // if(pcb->process->state == BLOCKED){
    //     pcb->process->state = READY;
    // }
    // return pcb->process->pid;
}

