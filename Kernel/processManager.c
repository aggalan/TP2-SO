#include <stdint.h>
#include <sys/types.h>
#include "processManager.h"
#include "collections.h"
#include "memoryManager.h"
#include "scheduler.h"
#include "lib.h"
#include "interrupts.h"

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

void kill_process_pid(pid_t pid) {
    remove_process(pid); //aca faltaria todo
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
        return NULL;
    }

    str_cpy(p->name, argv[0]);

    p->state = READY;

    p->pid = pid++;

    p->parent_pid = running_process();

    p->heap = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
    if (p->heap == NULL) {
        mm_free(p->name);
        mm_free(p);
        return NULL;
    }

    p->heap->base_ptr = mm_malloc(HEAP_SIZE);
    if (p->heap->base_ptr == NULL) {
        mm_free(p->heap);
        mm_free(p->name);
        mm_free(p);
        return NULL;
    }

    p->heap->base_ptr = fn;

    p->heap->current = (uintptr_t *) p->heap->base_ptr;
    if(p->heap->current == NULL) {
        mm_free(p->heap->base_ptr);
        mm_free(p->heap);
        mm_free(p->name);
        mm_free(p);
        return NULL;
    }

    p->stack = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
    if (p->stack == NULL) {
        mm_free(p->heap->base_ptr);
        mm_free(p->heap);
        mm_free(p->name);
        mm_free(p);
        return NULL;
    }

    p->stack->base_ptr = mm_malloc(STACK);
    if (p->stack->base_ptr == NULL) {
        mm_free(p->stack);
        mm_free(p->heap->base_ptr);
        mm_free(p->heap);
        mm_free(p->name);
        mm_free(p);
        return NULL;
    }

    p->stack->base_ptr += STACK - 1;

    p->stack->current = create_context(p->stack->base_ptr, p->heap->base_ptr); // aca se va a mover el rsp asi q me devuelve el current

    pcb->process = p;

    return pcb;
}

