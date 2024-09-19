#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);

int get_seconds();
int get_minutes();
int get_hours();
int get_wday();
int get_mday();
int get_month();
int get_year();
char * TimeClock(char * buffer);
void sleepms(int ms);
void sleeps(int sec);
void nanosleep(int ns); 
#endif
