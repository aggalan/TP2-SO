#include <stdint.h>
#include "collections.h"
#include "process_manager.h"
#include <sys/types.h>
#include "interrupts.h"
#include "scheduler.h"
#include "lib.h"
#include "memory_manager_bitmap.h"
#include "../Drivers/include/video_driver.h"
#include "hash_map.h"

#define KERNEL 0
#define IDLE 1
#define PROCESS 2

linked_list_ADT processes;
hash_map_ADT map;
PCB * idle_p;
int was_killed = 0;
int process_has_run = KERNEL;
int scheduler_initialized = 0;

void free_node(node_t * node);

void scheduler_init() {

    processes = ll_init();
    map = hm_init();

    create_process((uint64_t)idle, 1, 0, NULL);

    idle_p = get_idle();

    scheduler_initialized = 1;
}

int add_process(PCB * pcb, uint8_t priority, int nice) {
    int status = insert(pcb, priority, processes);
    if (!nice && status) {
        status = insert_map(pcb->pid, pcb, map);
    }
    return status;
}

int remove_process(pid_t pid_to_remove, int nice) {
    int status = remove(pid_to_remove, processes);
    if (!nice && status) {
        status = remove_map(pid_to_remove, map);
    }
    return status;
}

PCB * find_process(pid_t pid_find) {
    return find_map(pid_find, map);
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
    int status = 0;
    if (priority > pcb->priority) {
        status = add_process(pcb, priority - pcb->priority, 1);
    } else if (priority < pcb->priority){
        status = remove_times(pid_to_nice, pcb->priority - priority, processes);
    }

    if (status) {
        pcb->priority = priority;
    }

}


void print_processes() {
    drawWord1("There are ");
    drawNumber(map->size);
    drawWord1(" processes in the system");
    newLine();
    drawWord1("PID    ");
    drawWord1("NAME          ");
    drawWord1("PRIORITY   ");
    drawWord1("STACK BASE   ");
    drawWord1("RSP        ");
    drawWord1("IS FOREGROUND");
    newLine();
    for (int i = 0, j = 0; j < map->size && i < MAX_MAP_SIZE; i++) {
        if (map->PCB_arr[i] != NULL) {
            map_node * aux = map->PCB_arr[i];
            while (aux != NULL) {
                drawNumber(aux->key);

                pid_t beaut = aux->key;
                int m = 0;
                while(beaut/10 > 0){
                    beaut = beaut/10;
                    m++;
                }
                for (int z = 0; z < (6-m); z++ ) {
                    drawWord1(" ");
                }
                //name
                drawWord1(aux->value->name);
                for (int z = 0; z < 14-str_len(aux->value->name); z++) {
                    drawWord1(" ");
                }

                //name
                drawNumber(aux->value->priority);
                drawWord1("          ");
                address_to_string(aux->value->base);

                unsigned long beaut2 = aux->value->base;
                m = 0;
                while(beaut2/16 > 0){
                    beaut2 = beaut2/16;
                    m++;
                }
                for (int z = 0; z < (10-m); z++ ) {
                    drawWord1(" ");
                }

                address_to_string(aux->value->rsp);

                beaut2 = aux->value->base;
                m = 0;
                while(beaut2/16 > 0){
                    beaut2 = beaut2/16;
                    m++;
                }
                for (int z = 0; z < (8-m); z++ ) {
                    drawWord1(" ");
                }
                drawWord1("FALSE");
                newLine();
                j++;
                aux = aux->next;
            }
        }
    }
}

void killed() {
    was_killed = 1;
}



