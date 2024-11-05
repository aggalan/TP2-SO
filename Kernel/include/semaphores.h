
#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <sys/types.h>
#include "process_manager.h"
#include "libasm.h"
#include "../collections/include/collections.h"

typedef struct sem
{
    int id;
    queue_sem_ADT blocked;
    int n;
    uint64_t lock;
} sem;

typedef struct sem_manager_cdt *sem_manager_adt;

void sem_manager();
int my_sem_open(int id);
int my_sem_wait(int id);
int my_sem_post(int id);
int my_sem_close(int id);
void my_sem_free(int id);
int my_sem_init(int n);

#endif