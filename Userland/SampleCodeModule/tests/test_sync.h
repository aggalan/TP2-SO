// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <stdint.h>
#include <stdio.h>
#include "test_util.h"


void slowInc(int64_t *p, int64_t inc);

uint64_t my_process_inc(uint64_t argc, char *argv[]);

uint64_t test_sync(uint64_t argc, char *argv[]);