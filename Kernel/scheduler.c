#include <stdint.h>
#include "collections.h"
#include "processManager.h"
#include <sys/types.h>
#include "interrupts.h"


linked_list_ADT processes;

void idle() {
    while(1) {
        _hlt();
    }
}

void scheduler_init() {

    processes = ll_init();

    char * argv[] = {"idle"};
    create_process((uint64_t) &idle, 0, argv);
}

void add_process(PCB * pcb) {
    insert(pcb, processes);
}

void remove_process(pid_t pid) {
    remove(pid, processes);
}

pid_t running_process() {
    return 1;
}

void schedule(void * current_stack_ptr) {
    return;
}