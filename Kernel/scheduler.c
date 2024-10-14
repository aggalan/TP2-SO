#include <stdint.h>
#include "collections.h"
#include "processManager.h"
#include <sys/types.h>
#include "interrupts.h"
#include "scheduler.h"
#include "lib.h"
#include "memoryManager.h"
#include "../Drivers/include/videoDriver.h"

#define KERNEL 0
#define IDLE 1
#define PROCESS 2

linked_list_ADT processes;
PCB * idle_p;
int was_killed = 0;
int process_has_run = KERNEL;
int scheduler_initialized = 0;

void free_node(node_t * node);

void scheduler_init() {

    processes = ll_init();

    idle_p = create_idle_process();

    scheduler_initialized = 1;
}

void add_process(PCB * pcb, uint8_t priority) { //ver bien y tema idle
    insert(pcb, priority, processes);
}

void remove_process(pid_t pid_to_remove, int nice) { //ver bien y tema idle
    remove(pid_to_remove, processes, nice);
}

PCB * find_process(pid_t pid_find) {
    node_t * aux = find(pid_find, processes);
    return aux->next->data;
}

PCB * get_current() {
    if (processes->size == 0) {
        return NULL;
    }
    return processes->current->data;
}

pid_t get_current_pid() {
    PCB * pcb = get_current();
    if (pcb == NULL) {
        return -1;
    }
    return pcb->process->pid;
}

pid_t running_process() {
    return processes->current->data->process->pid; // ver si pasamos el parent_pid como argumento o hacemos esto. ver si no se caga en ningun caso
}

void * schedule(void * current_stack_ptr) {

    if (!scheduler_initialized) {
        return current_stack_ptr;
    }

    if (process_has_run == KERNEL) {
        if(processes->size == 0) {
            idle_p->process->state = RUNNING;
            process_has_run = IDLE;
            return idle_p->process->stack->current;
        } else {
            if (processes->current->data->process->state == READY) {
                processes->current->data->process->state = RUNNING;
                process_has_run = PROCESS;
                return processes->current->data->process->stack->current;
            }
        }
    }

    if (process_has_run == IDLE) {
        idle_p->process->stack->current = current_stack_ptr;
        idle_p->process->state = READY;
    } else {
        if (processes->current->data->process->state == RUNNING) {
            processes->current->data->process->state = READY;
        }
        if (!was_killed) {
            processes->current->data->process->stack->current = current_stack_ptr;
        }
        was_killed = 0;
    }

    node_t * aux = processes->current;
    if (aux == NULL) {
        process_has_run = IDLE;
        idle_p->process->state = RUNNING;
        return idle_p->process->stack->current;
    }
    processes->current = processes->current->next;

    while(processes->current->data->process->state != READY) {


        if (processes->current->data->process->state == KILLED) {
            node_t * aux = processes->current;
            remove_process(aux->data->process->pid, 0);
        }

        if (processes->current == aux && processes->current->data->process->state != READY) {
            process_has_run = IDLE;
            idle_p->process->state = RUNNING;
            return idle_p->process->stack->current;
        }

        processes->current = processes->current->next;
    }


    process_has_run = PROCESS;
    processes->current->data->process->state = RUNNING;
    return processes->current->data->process->stack->current;
}

pid_t get_active_pid() {
    return processes->current->data->process->pid;
}

void change_priority(pid_t pid_to_nice, uint8_t priority) {
    PCB * pcb = find_process(pid_to_nice);
    if (pcb == NULL) {
        return;
    }
    remove_process(pid_to_nice, 1);
    pcb->priority = priority;
    add_process(pcb, priority);

}


void print_processes() {
    drawWord1(" processes running: ");
    drawNumber(processes->size);
    newLine();
    drawWord1("i am going to print the processes now: ");
    newLine();
    node_t *aux = processes->first;
    do  {
        drawWord1("PID: ");
        drawNumber(aux->data->process->pid);
        drawWord1(" PRIORITY: ");
        drawNumber(aux->data->priority);
        newLine();
        aux = aux->next;
    } while(aux != processes->first);
    drawWord1(" list last PID: ");
    drawNumber(processes->last->data->process->pid);
    newLine();

}

void killed() {
    was_killed = 1;
}



