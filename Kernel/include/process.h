#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

void * create_context(void * stack_ptr, void * fn, int argc, char ** argv);


#endif