#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sys/types.h>
#include "processManager.h"

void my_nice(pid_t pid_to_nice, uint8_t priority);

void  scheduler_init();

void add_process(PCB * pcb, uint8_t priority);

void remove_process(pid_t pid_to_remove);

void remove_process_for_nice(pid_t pid_to_remove);

pid_t running_process();

PCB * find_process(pid_t pid_find);

PCB * get_current();

pid_t get_active_pid();

void print_processes();

#endif