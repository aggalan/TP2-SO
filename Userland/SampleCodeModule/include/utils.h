#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdarg.h>

void putCh(char c);
char getCh();
int nullOrSpace(char c);
int nullOrNewline(char c);
int strcmp(char * str1, char * str2);
int strcmpspace(char * str1, char * str2);
void print(char * fmt, ...);
int strlen(char * str);
int scan(char * fmt, ...);
void intToStr(int value, char* str, int base);
int readInt(int * num);
int readStr(char * buff);
#endif