#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sys/types.h>
#include "processManager.h"

void my_nice(pid_t pid, int priority);

void scheduler_init();

void schedule(void * current_stack_ptr); // no hace falta definirla creo porq solo se llama desde asm

void add_process(PCB * pcb);

pid_t running_process();


#endif