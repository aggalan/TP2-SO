// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sys/types.h>
#include "process_manager.h"

int change_priority(pid_t pid_to_nice, uint8_t priority);

void scheduler_init();

int add_process(PCB *pcb, uint8_t priority);

int remove_process(pid_t pid_to_remove);

pid_t running_process();

PCB *get_current();

pid_t get_current_pid();

pid_t get_active_pid();

void killed();

#endif