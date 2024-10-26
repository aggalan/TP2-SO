#include "semaphores.h"
#include "./include/syscalls.h"
#include "scheduler.h"
#include "./memory_manager/include/memory_manager.h"
#include "../Drivers/include/video_driver.h"
#include "./collections/include/collections.h"
#include "process_manager.h"


#define MAX_SEMAPHORES 10

int created = 0;

int wait(sem * semaphore);
int post(sem * semaphore);
sem * my_sem_create(int n);

typedef struct sem_manager_cdt{
    sem * semaphores[MAX_SEMAPHORES];
}sem_manager_cdt;


sem_manager_adt manager = NULL;


void sem_manager(){
    manager = (sem_manager_adt) mm_malloc(sizeof(sem_manager_cdt));
    for(int i = 0; i < MAX_SEMAPHORES; i++)
        manager->semaphores[i] = NULL;
}

sem * my_sem_create(int n){
    sem * semaphore = (sem *) mm_malloc(sizeof(sem));
    semaphore->n = n;
    semaphore->blocked = ll_init();
    semaphore->lock = 0;
    return semaphore;
}


int my_sem_init(int id, int n){
    if(manager->semaphores[id] != NULL){
        drawWord1("INVALID SEMAPHORE TO INIT ");
        return -1;
    }

    drawWord1("INIT ");
    manager->semaphores[id] = my_sem_create(n);
    return 0;
}

int my_sem_open(int id){
    if(manager->semaphores[id] == NULL){
        drawWord1("INVALID SEMAPHORE TO OPEN ");
        return -1;
    }
    drawWord1("OPENED ");
    return 0;
}


int my_sem_close(int id){
    if(manager->semaphores[id] == NULL){
        drawWord1("INVALID SEMAPHORE TO CLOSE ");
        return -1;
    }
    drawWord1("CLOSED ");
    //my_sem_free(id);
    manager->semaphores[id] = NULL;
    return 0;
}

int my_sem_wait(int id){
    if(manager->semaphores[id] == NULL){
        drawWord1("INVALID SEMAPHORE TO WAIT ");
        return -1;
    }
    drawWord1("WAITED ");
    return wait(manager->semaphores[id]);
};

int my_sem_post(int id){
    if(manager->semaphores[id] == NULL){
        drawWord1("INVALID SEMAPHORE TO POST ");
        return -1;
    }
    drawWord1("POSTED ");
    return post(manager->semaphores[id]);
};

void my_sem_free(int id){
     if(manager->semaphores[id] != NULL){
        drawWord1("INVALID SEMAPHORE TO FREE ");
        return;
    }

    //free lista
}


int wait(sem *semaphore) {
    drawNumber(semaphore->lock);
    acquire(&semaphore->lock);
    
    if (semaphore->n <= 0) {
        sem_insert(find_pcb(running_process()), semaphore->blocked);
        
        do {
            release(&semaphore->lock);
            block_process(running_process());
            
            acquire(&semaphore->lock);
        } while (semaphore->n <= 0);
    }
    
    semaphore->n--;
    release(&semaphore->lock);
    return 0;
}


int post(sem * semaphore) {
    acquire(&semaphore->lock);
    
    semaphore->n++;
    
    // Intentar despertar un proceso bloqueado, si hay alguno
    PCB * pcb = sem_remove(semaphore->blocked);
    if (pcb != NULL) {
        unblock_process(pcb->pid);
    }
    
    release(&semaphore->lock);
    return 0;
}