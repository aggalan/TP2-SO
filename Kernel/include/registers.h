#ifndef _REGISTERS_H_
#define _REGISTERS_H_
#include <stdint.h>



typedef struct registerStruct{
	uint64_t rax, rbx, rcx, rdx;
	uint64_t rsi, rdi, rbp, rsp;
	uint64_t  r8,  r9, r10, r11;
	uint64_t r12, r13, r14, r15;
	uint64_t ss, cs, rflags, rip;
} registerStruct;

extern int register_snapshot_taken; 
extern registerStruct registers;

void printRegisters(registerStruct * registers_dumped);

#endif