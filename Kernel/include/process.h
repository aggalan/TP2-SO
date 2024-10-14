#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

void * create_context(void * stack_ptr, void * fn, int argc, char ** argv);
//void * create_context(uint64_t argc, char ** argv, void * stack_ptr, void * fn);

#endif