#include <stdint.h>
#include <sys/types.h>
#include "processManager.h"
#include "collections.h"
#include "memoryManager.h"
#include "scheduler.h"
#include "lib.h"

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

    //Por ahora todos hijos del proceso init hasta que implemetemos fork
    p->parent_pid = running_process();


    //Implementar strlen y strcpy

    p->name = mm_malloc((size_t)strlen(argv[0]) + 1);
    if (p->name == NULL) {
        free(p);
        return -1;
    }

    strcpy(p->name, argv[0]);

    p->state = READY;

    p->pid = pid++;

    p->parent_pid = running_process();

    p->heap = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
    if (p->heap == NULL) {
        free(p->name);
        free(p);
        return -1;
    }

    p->heap->base_ptr = mm_malloc(HEAP_SIZE);
    if (p->heap->base_ptr == NULL) {
        free(p->heap);
        free(p->name);
        free(p);
        return -1;
    }

    p->heap->base_ptr = fn;

    p->heap->current = (uintptr_t *) p->heap->base_ptr;
    if(p->heap->current == NULL) {
        free(p->heap->base_ptr);
        free(p->heap);
        free(p->name);
        free(p);
        return -1;
    }

    p->stack = (p_memory_block *)mm_malloc(sizeof(p_memory_block));
    if (p->stack == NULL) {
        free(p->heap->base_ptr);
        free(p->heap);
        free(p->name);
        free(p);
        return -1;

    return -1;
    }

    p->stack->base_ptr = mm_malloc(STACK);
    if (p->stack->base_ptr == NULL) {
        free(p->stack);
        free(p->heap->base_ptr);
        free(p->heap);
        free(p->name);
        free(p);
        return -1;
    }

    p->stack->base_ptr += STACK - 1;

    p->stack->current = create_context(p->stack->base_ptr, p->heap->base_ptr); // aca se va a mover el rsp asi q me devuelve el current

    pcb->process = p;

    add_process(pcb);

    return p->pid;
}

void kill_process_pid(pid_t pid) {
    remove_process(pid); //aca faltaria todo
}

