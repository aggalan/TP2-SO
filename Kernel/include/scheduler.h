#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sys/types.h>
#include "processManager.h"

void my_nice(pid_t pid, int priority);

void  scheduler_init();

void add_process(PCB * pcb);

void remove_process(pid_t pid);

pid_t running_process();

PCB * find_process(pid_t pid);

PCB * get_current();

#endif