// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6
#include "Drivers/include/video_driver.h"
#include "include/register_handling.h"
#include "include/syscalls.h"
#include "include/interrupts.h"
static void zero_division();
static void invalid_opcode();

void exception_dispatcher(int exception)
{
    if (exception == ZERO_EXCEPTION_ID)
    {
        zero_division();
    }
    else
    {
        invalid_opcode();
    }
    print_registers(get_registers(), 0x00ff0000);
    draw_word(0x00ff0000, "Press any key to continue");
    while (1)
    {
        char c;
        _hlt();
        sys_read(STDIN, &c, 1);
        if (c != 0)
        {
            clear();
            return;
        }
    }
}

static void zero_division()
{
    draw_word(0x00FF0000, "Cannot divide by 0");
    newline();
    return;
}

static void invalid_opcode()
{

    draw_word(0x00FF0000, "Error:Invalid opcode");
    newline();
}