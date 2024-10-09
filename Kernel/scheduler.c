#include <stdint.h>
#include "collections.h"
#include "processManager.h"
#include <sys/types.h>
#include "interrupts.h"
#include "scheduler.h"
#include "lib.h"
#include "memoryManager.h"


linked_list_ADT processes;
PCB * idle_p;
int idle_has_run = 1;

void free_node(node_t * node);

void scheduler_init() {

    processes = ll_init();

    idle_p = create_idle_process();
}

void add_process(PCB * pcb) { //ver bien y tema idle
    insert(pcb, processes);
}

void remove_process(pid_t pid) { //ver bien y tema idle
    remove(pid, processes);
}

PCB * find_process(pid_t pid) {
    return find(pid, processes);
}

PCB * get_current() {
    if (processes->size == 0) {
        return NULL;
    }
    return processes->current->data;
}

pid_t running_process() {
    return processes->current->data->process->pid; // ver si pasamos el parent_pid como argumento o hacemos esto. ver si no se caga en ningun caso
}

void * schedule(void * current_stack_ptr) {

    if (processes->size == 0) {
        idle_has_run = 1;
        idle_p->process->state = RUNNING;
        idle_p->process->stack->current = current_stack_ptr;
        return idle_p->process->stack->current;
    }

    if (!idle_has_run) {
        processes->current->data->process->state = READY;
        processes->current->data->process->stack->current = current_stack_ptr;
    } else {
        idle_p->process->state = READY;
        idle_p->process->stack->current = current_stack_ptr;
    }

    node_t * aux = processes->current;
    do {
        processes->current = processes->current->next;
        if (processes->current->data->process->state == KILLED) {
            node_t * aux = processes->current;
            processes->current = processes->current->next;
            remove_process(aux->data->process->pid);
            free_node(aux);
        }
    }while(processes->current->next->data->process->state != READY && processes->current != aux);
    if (processes->current == aux) {
        idle_has_run = 1;
        idle_p->process->state = RUNNING;
        return idle_p->process->stack->current;
    }

    idle_has_run = 0;
    processes->current->data->process->state = RUNNING;
    return processes->current->data->process->stack->current;
}

void free_node(node_t * node) { //revisar
    mm_free(node->data->process->stack->base_ptr);
    mm_free(node->data->process->stack->current);
    mm_free(node->data->process->heap->base_ptr);
    mm_free(node->data->process->heap->current);
    mm_free(node->data->process->stack);
    mm_free(node->data->process->heap);
    mm_free(node->data->process->name);
    mm_free(node->data->process);
    mm_free(node->data);
    mm_free(node);
}





