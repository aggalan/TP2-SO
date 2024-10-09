#include <stdint.h>
#include "collections.h"
#include "processManager.h"
#include <sys/types.h>
#include "interrupts.h"
#include "scheduler.h"
#include "lib.h"


linked_list_ADT processes;
PCB * idle_p;
int idle_has_run = 1;

void scheduler_init() {

    processes = ll_init();

    idle_p = create_idle_process(idle);
}

void add_process(PCB * pcb) { //ver bien y tema idle
    insert(pcb, processes);
}

void remove_process(pid_t pid) { //ver bien y tema idle
    remove(pid, processes);
}

pid_t running_process() {
    return processes->current->data->process->pid; // ver si pasamos el parent_pid como argumento o hacemos esto. ver si no se caga en ningun caso
}

void * schedule(void * current_stack_ptr) {

    if (processes->size == 0) {
        idle_has_run = 1;
        idle_p->process->stack->current = current_stack_ptr;
        return idle_p->process->stack->current;
    }

    if (!idle_has_run) {
        processes->current->data->process->stack->current = current_stack_ptr;
    } else {
        idle_p->process->stack->current = current_stack_ptr;
    }

    node_t * aux = processes->current;
    do {
        processes->current = processes->current->next;
    }while(processes->current->next->data->process->state != READY && processes->current != aux);
    if (processes->current == aux) {
        idle_has_run = 1;
        return idle_p->process->stack->current;
    }

    idle_has_run = 0;
    return processes->current->data->process->stack->current;
}





