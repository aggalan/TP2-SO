#ifndef TPE_LIB_H
#define TPE_LIB_H
#include <stdint.h>
#include "sys/types.h"

typedef struct p_memory_block {
    void * base_ptr;
//    uint64_t size;
    void * current;
    void * base;
}p_memory_block;

typedef struct process {
    p_memory_block * heap;
    p_memory_block * stack;
    char * name;
//    char is_foreground;
    uint8_t state;
    pid_t pid;
    pid_t parent_pid;
}process;

typedef struct PCB{
    uint64_t ticks;
    process * process;
    uint8_t priority;
}PCB;

typedef struct PCB * PCB_t;


// Reads a character from input and saves it in the pointer c. 
// Returns the number of characters saved.
int getC(char * c);

// Prints the character passed as the first argument in the color specified by the second argument.
void putC(char c, uint32_t hexColor);

// Prints the integer number passed as the first argument in the color specified by the second argument.
void put_int(uint64_t num, uint32_t hexColor);

// Prints the formatted string passed as the second argument in the color specified by the hexadecimal color code passed as the first argument. 
// Similar to c standarlibrary functino printf.
void print(uint32_t hexColor, const char * format, ...);

// Compares two strings. 
// Returns 0 if the strings are equal, a positive integer if the first string is greater, or a negative integer if the second string is greater.
int str_cmp(char * str1, char * str2);

void str_cpy(char *dest, const char *src);

//copy str on an auxiliar char * until first space 
char * cut_string(char * str);

// Returns the length of the string passed as the argument.
int str_len(const char *str);

// Prints the string passed as the first argument in the color specified by the second argument.
void put_string(const char * s, uint32_t hexColor);

// Converts the string passed as the argument to an integer and returns value.
int str_to_int(char * s);

// Reads input into  buf. 
// Returns the number of characters read.
int get_buffer(char * buf);

// Converts all lowercase characters in the string passed as the argument to uppercase.
void str_to_upper(char *str);

#endif // TPE_LIB_H
