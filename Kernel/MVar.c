#include "include/MVar.h"

pid_t global_value = -1;

void openMVar(mvar_t * mvar) {
    mvar->full_sem = my_sem_init(0);
    mvar->empty_sem = my_sem_init(1);
    mvar->value = &global_value;
}

void putMVar(mvar_t * mvar, int value) {
    my_sem_wait(mvar->empty_sem);
    *(mvar->value) = value;
    my_sem_post(mvar->full_sem);
}

int takeMVar(mvar_t * mvar) {
    my_sem_wait(mvar->full_sem);
    int to_return = *(mvar->value);
    *(mvar->value) = -1;
    my_sem_post(mvar->empty_sem);
    return to_return;
}

