#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

uint32_t u_int_to_base(uint64_t value, char *buffer, uint32_t base);

// returns current time (GMT-3)
void clock(char *buffer);
#endif