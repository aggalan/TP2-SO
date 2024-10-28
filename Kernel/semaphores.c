// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "semaphores.h"
#include "./include/syscalls.h"
#include "scheduler.h"
#include "./memory_manager/include/memory_manager.h"
#include "../Drivers/include/video_driver.h"
#include "./collections/include/collections.h"
#include "process_manager.h"
#define MAX_SEMAPHORES 10

int wait(sem *semaphore);
int post(sem *semaphore);
sem *my_sem_create(int n);

typedef struct sem_manager_cdt
{
    sem *semaphores[MAX_SEMAPHORES];
} sem_manager_cdt;

sem_manager_adt manager = NULL;

void sem_manager()
{
    manager = (sem_manager_adt)mm_malloc(sizeof(sem_manager_cdt));
    for (int i = 0; i < MAX_SEMAPHORES; i++)
        manager->semaphores[i] = NULL;
}

sem *my_sem_create(int n)
{
    sem *semaphore = (sem *)mm_malloc(sizeof(sem));
    semaphore->n = n;
    semaphore->blocked = qs_init();
    if (n > 0)
        semaphore->lock = 1;
    else
        semaphore->lock = 0;
    return semaphore;
}

int my_sem_init(int id, int n)
{
    if (manager->semaphores[id] != NULL)
    {
        return -1;
    }

    manager->semaphores[id] = my_sem_create(n);
    return 0;
}

int my_sem_open(int id)
{
    if (manager->semaphores[id] == NULL)
    {
        return -1;
    }
    return 0;
}

int my_sem_close(int id)
{
    if (manager->semaphores[id] == NULL)
    {
        return -1;
    }

    sem *semaphore = manager->semaphores[id];
    acquire(&semaphore->lock);

    pid_t pid;
    while ((pid = sem_remove(semaphore->blocked)) > 0)
    {
        unblock_process(pid);
    }

    my_sem_free(id);
    manager->semaphores[id] = NULL;
    release(&semaphore->lock);

    return 0;
}

int my_sem_wait(int id)
{
    if (manager->semaphores[id] == NULL)
    {
        return -1;
    }
    return wait(manager->semaphores[id]);
};

int my_sem_post(int id)
{
    if (manager->semaphores[id] == NULL)
    {
        return -1;
    }
    return post(manager->semaphores[id]);
};

void my_sem_free(int id)
{
    if (manager->semaphores[id] != NULL)
    {
        return;
    }

    mm_free(manager->semaphores[id]->blocked);
    mm_free(manager->semaphores[id]);
}

int wait(sem *semaphore)
{

    acquire(&semaphore->lock);
    if (semaphore->n > 0)
    {
        semaphore->n--;
        release(&semaphore->lock);
        return 0;
    }
    pid_t pid = running_process();
    sem_insert(pid, semaphore->blocked);
    release(&semaphore->lock);
    block_process(pid);
    return 0;
}

int post(sem *semaphore)
{
    acquire(&semaphore->lock);

    // Intentar despertar un proceso bloqueado, si hay alguno
    pid_t pid = sem_remove(semaphore->blocked);
    if (pid > 1)
    {
        unblock_process(pid);
        release(&semaphore->lock);
        return 0;
    }

    semaphore->n++;
    release(&semaphore->lock);
    return 0;
}
