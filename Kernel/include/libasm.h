
#ifndef LIBASM_H
#define LIBASM_H
#include <stdint.h>
// Returns minutes in gmt
char *get_minutes();
// Returns seconds in gmt
char *get_seconds();
// Returns hours in gmt
char *get_hours();
uint16_t get_key();
uint8_t inb(uint8_t);
void outb(uint8_t, uint8_t);
void acquire(uint64_t *lock);
void release(uint64_t *lock);
#endif