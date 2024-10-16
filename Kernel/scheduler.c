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

    create_process((uint64_t)idle, 1, 0, NULL);

    idle_p = get_idle();

    scheduler_initialized = 1;
}

void add_process(PCB * pcb, uint8_t priority) {
    insert(pcb, priority, processes);
}

void remove_process(pid_t pid_to_remove, int nice) { 
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
    return pcb->pid;
}

pid_t running_process() {
    return processes->current->data->pid; 
}

uint64_t schedule(uint64_t rsp) {

    if (!scheduler_initialized) {
        return rsp;
    }

    if (process_has_run == KERNEL) {
        if(processes->size == 0) {
            idle_p->state = RUNNING;
            process_has_run = IDLE;
            return idle_p->rsp;
        } else {
            if (processes->current->data->state == READY) {
                processes->current->data->state = RUNNING;
                process_has_run = PROCESS;
                return processes->current->data->rsp;
            }
        }
    }

    if (process_has_run == IDLE) { // ESTE ORDEN DE COASAS SOLO ES VALIDO PORQ LA SHELL AUNQUE ESTE BLOQUEADA QUEDA EN LA LISTA, SI CAMBIA ESO ESTO EXPLOTA MAL, PENSA Y HACE MEMORIA JOSE DEL FUTURO A VER A QUE ME REFIERO !!!
        idle_p->rsp = rsp;
        idle_p->state = READY;
    } else {
        if (processes->current->data->state == RUNNING) {
            processes->current->data->state = READY;
        }
        if (!was_killed) {
            processes->current->data->rsp = rsp;
        }
        was_killed = 0;
    }

    node_t * aux = processes->current;
    if (aux == NULL) { // BIEN A FUTURO ESTA ESTO, PERO MAL LA POSICION VA MAS ARRIBA, RELACIONADO CON EL COMENTARIO DE ARRIBA JOSE DEL FUTURO !
        process_has_run = IDLE;
        idle_p->state = RUNNING; //el timer tick se llama cada 20 ms o algo asi, si queremos ahorrar lineas para eficiencia es al pedo actualizar estado de idle
        return idle_p->rsp;
    }
    processes->current = processes->current->next;

    while(processes->current->data->state != READY) {

//        if (processes->current->data->process->state == KILLED) {
//            node_t * aux = processes->current;
//            remove_process(aux->data->process->pid, 0);
//        }

        if (processes->current == aux && processes->current->data->state != READY) {
            process_has_run = IDLE;
            idle_p->state = RUNNING;
            return idle_p->rsp;
        }

        processes->current = processes->current->next;
    }


    process_has_run = PROCESS;
    processes->current->data->state = RUNNING;
    return processes->current->data->rsp;
}

pid_t get_active_pid() {
    return processes->current->data->pid;
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
    drawWord1("There are ");
    drawNumber(processes->size);
    drawWord1(" processes in the system");
    newLine();
    node_t *aux = processes->first;
    do  {
        drawWord1("PID: ");
        drawNumber(aux->data->pid);
        drawWord1(" PRIORITY: ");
        drawNumber(aux->data->priority);
        newLine();
        aux = aux->next;
    } while(aux != processes->first);


}

void killed() {
    was_killed = 1;
}



