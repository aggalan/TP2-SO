// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdint.h>

uint32_t get_uint();
uint32_t get_uniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
uint64_t test_mm(uint64_t argc, char *argv[]);
void test_prio();
int main_test_pipes();
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t argc, char **argv);

#endif
