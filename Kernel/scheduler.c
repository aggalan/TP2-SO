#include <stdint.h>
#include "collections.h"
#include "processManager.h"
#include <sys/types.h>
#include "interrupts.h"
#include "scheduler.h"
#include "lib.h"
#include "memoryManager.h"
#include "videoDriver.h"

#define KERNEL 0
#define IDLE 1
#define PROCESS 2

linked_list_ADT processes;
PCB * idle_p;
int idle_has_run = 1;
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

void remove_process(pid_t pid_to_remove) { //ver bien y tema idle
    remove(pid_to_remove, processes);
}
void remove_process_for_nice(pid_t pid_to_remove) { //ver bien y tema idle
    remove_nice(pid_to_remove, processes);
}

PCB * find_process(pid_t pid_find) {
    return find(pid_find, processes);
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

    if (!scheduler_initialized) {
        return current_stack_ptr;
    }


    if (process_has_run == KERNEL) {
        if(processes->size == 0) {
            idle_p->process->state = RUNNING;
            process_has_run = IDLE;
//            idle_p->process->stack->current = current_stack_ptr;
            return idle_p->process->stack->current;
        } else {
            if (processes->current->data->process->state == READY){
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
        if (processes->current->data->process->state != KILLED) {
            processes->current->data->process->state = READY;
        }
        processes->current->data->process->stack->current = current_stack_ptr;
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
//            processes->current = processes->current->next;
            remove_process(aux->data->process->pid);
//            free_node(aux);
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


//    if (!scheduler_initialized) {
//        return current_stack_ptr;
//    }
//
//    if (processes->size == 0) {
//        idle_has_run = 1;
//        idle_p->process->state = RUNNING;
//        idle_p->process->stack->current = current_stack_ptr;
//        return idle_p->process->stack->current;
//    }
//
//
//
//    if (!idle_has_run) {
////        drawWord(" PID: ");
////        drawNumber(processes->current->data->process->pid);
////        drawWord(" STATE: ");
////        drawNumber(processes->current->data->process->state);
//        if (processes->current->data->process->state == RUNNING) {
//            processes->current->data->process->state = READY;
//            processes->current->data->process->stack->current = current_stack_ptr;
//        }
//    } else {
//        idle_p->process->state = READY;
//        idle_p->process->stack->current = current_stack_ptr;
//    }
//
//    node_t * aux = processes->current;
//
////    newline();
////    drawWord(" PID: ");
////    drawNumber(aux->data->process->pid);
////    newline();
//
//    processes->current = processes->current->next;
//
//
//    while(processes->current->data->process->state != READY && processes->current->data->process->pid != 1) {
//
//
//        if (processes->current->data->process->state == KILLED && processes->current->data->process->pid != 1) {
//            node_t * aux2 = processes->current;
////            int auxiliary_pid = processes->current->data->process->pid;
//            remove_process(processes->current->data->process->pid);
//
////            drawWord(" i killed ");
////            drawNumber(auxiliary_pid);
////            newline();
////            drawWord(" STATE: ");
////            drawNumber(processes->current->data->process->state);
////            newline();
////            print_processes();
//
//            continue;
//        }
//
//
//
//        if (processes->current == aux && processes->current->data->process->state != READY) {
//            drawWord("entre");
//            idle_has_run = 1;
//            idle_p->process->state = RUNNING;
//            return idle_p->process->stack->current;
//        }
//
//        processes->current = processes->current->next;
//    }
//
//    idle_has_run = 0;
//    processes->current->data->process->state = RUNNING;
//
////    drawWord("  do i finish?  ");
//
//    return processes->current->data->process->stack->current;
}

pid_t get_active_pid() {
    return processes->current->data->process->pid;
}

void my_nice(pid_t pid_to_nice, uint8_t priority) {
    PCB * pcb = find_process(pid_to_nice);
    if (pcb == NULL) {
        return;
    }
    remove_process_for_nice(pid_to_nice);
    pcb->priority = priority;
//    drawWord(" I am in nice between remove and add for pid: ");
//    drawNumber(pid_to_nice);
//    newline();
//    print_processes();
//    newline();
    add_process(pcb, priority);

}


void print_processes() {
    node_t *aux = processes->first;
    drawWord("i am going to print the processes now: ");
    newline();
    do  {
        drawWord("PID: ");
        drawNumber(aux->data->process->pid);
        drawWord(" PRIORITY: ");
        drawNumber(aux->data->priority);
        newline();
        aux = aux->next;
    } while(aux != processes->last);
    drawWord("PID: ");
    drawNumber(aux->data->process->pid);
    drawWord(" PRIORITY: ");
    drawNumber(aux->data->priority);
    newline();
    drawWord(" list last PID: ");
    drawNumber(processes->last->data->process->pid);
    newline();

}



