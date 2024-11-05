
#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void *memset(void *destination, int32_t character, uint64_t length);

void *memcpy(void *destination, const void *source, uint64_t length);

char *cpu_vendor(char *result);

// freezes for the amount of ms indicated
void sleepms(uint32_t ms);
int str_len(const char *str);
int str_cpy(char *dest, const char *src);
int str_cmp(char *str1, char *str2);
void idle();
void exit_process(int status);
void print_kernel(int color, const char *format, ...);
#endif