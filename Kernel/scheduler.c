#include <stdint.h>
#include "collections.h"
#include "processManager.h"
#include <sys/types.h>
#include "interrupts.h"
#include "scheduler.h"


linked_list_ADT processes;

void idle() {
    while(1) {
        _hlt();
    }
}

void scheduler_init() {

    processes = ll_init();

    char * argv[] = {"idle"};
    create_process(idle, 0, argv);
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

void update_all_stack_base(PCB * pcb, void * new_stack_ptr) { //para prioridad por ahora estan todas en uno y no cambia pero a futuro
    uint8_t priority = 0;
    node_t * aux = processes->current;
    while(priority < pcb->priority) {
        if (aux->data == pcb) { //puede q el proceso no exista? considerar, creo que no
            aux->data->process->stack->current = new_stack_ptr;
            priority++;
        }
        aux = aux->next;
    }
}

void * schedule(void * current_stack_ptr) {
    //depende como manejemos lo de idle aca faltan cosas o no, decidir
    update_all_stack_base(processes->current->data, current_stack_ptr);
    processes->current = processes->current->next;
    return processes->current->data->process->stack->current;
}





