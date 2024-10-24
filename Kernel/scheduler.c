#include <stdint.h>
#include "process_manager.h"
#include <sys/types.h>
#include "interrupts.h"
#include "scheduler.h"
#include "lib.h"
#include "./memory_manager/include/memory_manager.h"
#include "../Drivers/include/video_driver.h"
#include "./collections/include/collections.h"

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
    hash_map_init();

    create_process((uint64_t)idle, 1, 0, NULL);

    idle_p = get_idle();

    scheduler_initialized = 1;
}

int add_process(PCB * pcb, uint8_t priority) {
    return insert(pcb, priority, processes);
}

int remove_process(pid_t pid_to_remove) {
    return remove(pid_to_remove, processes);
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
            processes->current->data->rsp = rsp;
        }
//        if (!was_killed) {
//            processes->current->data->rsp = rsp;
//        }
//        was_killed = 0;
    }

    node_t * aux = processes->current;
    if (aux == NULL) { // BIEN A FUTURO ESTA ESTO, PERO MAL LA POSICION VA MAS ARRIBA, RELACIONADO CON EL COMENTARIO DE ARRIBA JOSE DEL FUTURO !
        process_has_run = IDLE;
        idle_p->state = RUNNING; //el timer tick se llama cada 20 ms o algo asi, si queremos ahorrar lineas para eficiencia es al pedo actualizar estado de idle
        return idle_p->rsp;
    }
    processes->current = processes->current->next;

    while(processes->current->data->state != READY) {

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

int change_priority(pid_t pid_to_nice, uint8_t priority) {
    if(pid_to_nice == 1) {
        return -1;
    }
    PCB * pcb = find_pcb(pid_to_nice);
    if (pcb == NULL) {
        return -1;
    }   
    int status = 0;
    if (priority > pcb->priority) {
        status = add_process(pcb, priority - pcb->priority);
    } else if (priority < pcb->priority){
        status = remove_times(pid_to_nice, pcb->priority - priority, processes);
    }

    if (status) {
        pcb->priority = priority;
    }
    return 0;

}

void killed() {
    was_killed = 1;
}



