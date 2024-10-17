#include "include/Shell.h"
#include "include/Buffer.h"
#include "include/lib.h"
#include "include/usr_sys_calls.h"
#include "include/eliminator.h"
#include <stdio.h>
#include "tests/test_processes.h"
#include "tests/test_util.h"
#define WHITE 0xFFFFFFFF
static char buffer[BUFFER_SIZE] = {0};
int exit_flag = 0;
int register_flag = 0;

void line_read(char *buffer);
void call_invalid_op();
char re_size(char *buffer);
void call_div0();

void starting_line()
{
    char *starting_line = "$>";
    put_string(starting_line, GREEN);
    clear_buffer(buffer);
}

void buffer_control()
{
    int i = 0;
    while (1)
    {
        char c;
        getC(&c);

        if (c != 0 && c != '\t')
        {
            if (c == '\n')
            {
                putC(c, WHITE);
                if (i == 0)
                {
                    clear_buffer(buffer);
                    return;
                }
                buffer[i] = 0;
                line_read(buffer);
                clear_buffer(buffer); // para limpiarlo ahora q line_read no lo uso
                return;
            }
            else if (c == '\b')
            {
                if (i > 0)
                {
                    i--; // borro el ultimo caracter
                    putC(c, WHITE);
                }
            }
            else
            {
                if (i < BUFFER_SIZE)
                {
                    buffer[i++] = c; // guardo el caracter en el buffer
                    putC(c, WHITE);
                }
            }
        }
    }
}

// Define an array to store command strings
const char *commands[] = {
    "eliminator:   Game similar to tron(the movie).",
    "time:         Shows the actual time.",
    "setFont:      Change the font size, receive an int from 1 to 2.",
    "getRegisters: Show the actual state of the registers.",
    "clear:        Empty the terminal.",
    "exit:         Kills the terminal.",
    "div0:         Test the exception of the cero division.",
    "invalidOp:    Test the exception of an invalid operand.",
    "status:       Shows the status of the memory.",
    "testmm:       Test the memory management.",
    "testprocess:  Test the process creation.",
    "testprio:     Test the priority of the process.",
    "ps:           Shows the live processes.",
    "annihilate:   Kills all processes except for the Shell. ",
};

void line_read(char *buffer)
{
    if (str_cmp(buffer, "help") == 0)
    {
        put_string("The following commands may be used: \n", WHITE);
        for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
        {
            put_string(commands[i], WHITE);
            put_string("\n", WHITE);
        }
    }
    else if (str_cmp(buffer, "eliminator") == 0)    
    {
        start_eliminator();
    }
    else if (str_cmp(buffer, "time") == 0)
    {
        char time[9]; // Viene dada por el formato hh:mm:ss por eso son 8 mas la terminacion en cero
        call_time_clock(time);
        put_string(time, WHITE);
        put_string("\n", WHITE);
    }
    else if (str_cmp(cut_string(buffer), "setFont") == 0)
    {
        if (re_size(buffer))
        {
            call_clear();
        }
        else
        {
            put_string("Enter a valid size (1 or 2) \n", RED);
        }
        clear_buffer(buffer);
    }
    else if (str_cmp(buffer, "getRegisters") == 0)
    {
        call_print_registers(1);
    }
    else if (str_cmp(buffer, "clear") == 0)
    {
        call_clear();
        clear_buffer(buffer);
    }
    else if (str_cmp(buffer, "exit") == 0)
    {
        exit_flag = 1;
        call_clear();
        clear_buffer(buffer);
        return;
    }
    else if (str_cmp(buffer, "div0") == 0)
    {
        call_div0();
        return;
    }
    else if (str_cmp(buffer, "invalidOp") == 0)
    {
        call_invalid_op();
        return;
    }
    else if (str_cmp(buffer, "status") == 0)
    {
        call_status();
        return;
    }
    else if (str_cmp(buffer, "testmm") == 0)
    {
        char **argv_mm = (char **)(uintptr_t)call_malloc(2 * sizeof(char *));
        argv_mm[0] = "mem test";
        argv_mm[1] = "266240";
        call_create_process(test_mm, 1, 2, argv_mm);
        return;
    }
    else if (str_cmp(buffer, "testprocess") == 0)
    {
        char **argv_process = (char **)(uintptr_t)call_malloc(2 * sizeof(char *));
        argv_process[0] = "process test";
        argv_process[1] = "10";
        call_create_process(test_processes, 1, 2, argv_process);
        return;
    }
    else if (str_cmp(buffer, "testprio") == 0)
    {
        char **argv_priority = (char **)(uintptr_t)call_malloc(sizeof(char *));
        argv_priority[0] = "prio";
        call_create_process(test_prio, 1, 1, argv_priority);
        return;
    }
    else if (str_cmp(cut_string(buffer), "kill") == 0)
    {
        char *init = buffer + str_len("kill ");
        if (!str_len(init))
        {
            return;
        }
        call_kill(str_to_int(init));
        return;
    }
    else if (str_cmp(buffer, "ps") == 0)
    {
        call_ps();
        return;
    }
    else if (str_cmp(buffer, "annihilate") == 0)
    {
        call_annihilate();
        return;
    }
    else
    {
        put_string(buffer, WHITE);
        put_string(":command not found", WHITE);
        put_string("\n", WHITE);
    }
    call_print_registers(0);
}

char re_size(char *buffer)
{
    char *init = buffer + str_len("setFont ");
    if (!str_len(init))
        return 0;
    return (char)call_set_font_size(str_to_int(init));
}

int shell_init()
{
    char *start = "Welcome to IncFont OS, type help to get a list of commands.\n";
    put_string(start, GREEN);
    clear_buffer(buffer);
    while (!exit_flag)
    {
        starting_line();
        buffer_control();
    }
    return 0;
}
void call_invalid_op()
{
    invalid_op_asm();
}
void call_div0()
{
    int a = 7;
    int b = 0;
    a = a / b;
}