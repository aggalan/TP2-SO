// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

uint64_t create_context(uint64_t rsp, uint64_t fn, int argc, char **argv);

#endif