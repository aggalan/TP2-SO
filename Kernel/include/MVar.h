#include <sys/types.h>
#include "semaphores.h"


typedef struct mvar_t{
    pid_t * value;
    int full_sem;
    int empty_sem;
}mvar_t;


void openMVar(mvar_t * mvar);

void putMVar(mvar_t * mvar, int value);

int takeMVar(mvar_t * mvar);

