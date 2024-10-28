// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void *memset(void *destination, int32_t character, uint64_t length);

void *memcpy(void *destination, const void *source, uint64_t length);

char *cpuVendor(char *result);

// freezes for the amount of ms indicated
void sleepms(int ms);
int str_len(char *str);
int str_cpy(char *dest, char *src);
void idle();
void exit_process(int status);
#endif