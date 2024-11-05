// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "include/libasm.h"
#include <stdint.h>
#include "include/register_handling.h"
#include "Drivers/include/video_driver.h"
#define SIZE 18
char *register_names[SIZE] = {"rflags", "rsp", "rip", "rbp", "rax", "rbx", "rcx", "rdx", "rdi", "rsi", "r8 ", "r9 ", "r10", "r11", "r12", "r13", "r14", "r15"};

void print_registers(uint64_t *registers, uint32_t colour)
{
    for (int i = 0; i < SIZE; i++)
    {
        draw_word(colour, register_names[i]);
        draw_word(colour, ": 0x");
        draw_register(registers[SIZE - i - 1], colour);
    }
}