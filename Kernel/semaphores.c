#include "semaphores.h"
#include "./memory_manager/include/memory_manager.h"
#include "./include/syscalls.h"
#include "./include/scheduler.h"
#define MAX_SEMAPHORES 10

int created = 0;

typedef struct sem_manager_cdt{
    sem_t * semaphores[MAX_SEMAPHORES];
}sem_manager_cdt;


sem_manager_adt manager = NULL;


void sem_init(){
    manager = (sem_manager_adt) mm_malloc(sizeof(sem_manager_cdt));
    for(int i = 0; i < MAX_SEMAPHORES; i++)
        manager->semaphores[i] = NULL;
}


int sem_open(int id, int n){
    if(manager->semaphores[id] != NULL){
        return -1;
    }
    manager->semaphores[id] = sem_create(n);
    return 0;
}

sem_t * sem_create(int n){
    sem_t * sem = (sem_t *) mm_malloc(sizeof(sem_t));
    sem->n = n;
    sem->blocked = ll_init();
    return sem;
}

int sem_close(int id){
    if(manager->semaphores[id] == NULL){
        return -1;
    }
    sem_free(manager->semaphores[id]);
    manager->semaphores[id] = NULL;
    return 0;
}

int sem_wait(int id){
    if(manager->semaphores[id] != NULL){
        return -1;
    }
    return wait(manager->semaphores[id]);
};

int sem_post(int id){
    if(manager->semaphores[id] != NULL){
        return -1;
    }
    return post(manager->semaphores[id]);
};


int wait(sem_t * sem){
    acquire(sem->lock);

    while (sem->n <= 0) {
        // Add current process to blocked list
        ll_add(sem->blocked, get_current_process());
        // Release lock and block the process
        release(sem->lock);
        //block_process();
        // When unblocked, reacquire the lock
        acquire(sem->lock);
    }

    sem->n--;
    release(sem->lock);
    return 0;
}

int post(sem_t * sem){
    acquire(sem->lock);
    sem->n++;

    // Unblock next process if exists
    // if (sem->blocked->size > 0) {
    //     process_t* process_to_unblock = ll_remove_first(sem->blocked);
    //     unblock_process(process_to_unblock);
    // }

    release(sem->lock);
    return 0;
}



