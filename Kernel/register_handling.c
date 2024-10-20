#include "include/libasm.h"
#include <stdint.h>
#include "include/register_handling.h"
#include "Drivers/include/video_driver.h"
#define SIZE 18
char* register_names[SIZE] = {"rflags",  "rsp",  "rip", "rbp", "rax", "rbx", "rcx", "rdx", "rdi", "rsi","r8 ", "r9 ", "r10", "r11", "r12", "r13", "r14", "r15"};
    

    
void printRegisters(uint64_t * registers,uint32_t colour){
    for (int i = 0; i < SIZE; i++) {
        drawWord(colour,register_names[i]);
        drawWord(colour,": 0x");
        drawRegister(registers[SIZE -i -1],colour);
    }
}