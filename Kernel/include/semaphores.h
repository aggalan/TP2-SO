#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <sys/types.h>
#include "process_manager.h"
#include "libasm.h"
#include "collections.h"


typedef struct{
    int id;
    linked_list_ADT blocked;
    int n;
    uint64_t * lock;
}sem_t;

typedef struct sem_manager_cdt * sem_manager_adt;

void sem_init();
int sem_open(int id, int n);
int sem_wait(int id);
int sem_post(int id);
int sem_close(int id);

#endif