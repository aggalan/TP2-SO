#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sys/types.h>

void my_nice(pid_t pid, int priority);

void scheduler_init();

void schedule();

#endif