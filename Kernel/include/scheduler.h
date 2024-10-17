#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sys/types.h>
#include "process_manager.h"

void change_priority(pid_t pid_to_nice, uint8_t priority);

void  scheduler_init();

int add_process(PCB * pcb, uint8_t priority, int nice);

int remove_process(pid_t pid_to_remove, int nice);

pid_t running_process();

PCB * find_process(pid_t pid_find);

PCB * get_current();

pid_t get_current_pid();

pid_t get_active_pid();

void print_processes();

void killed();

#endif