#include "include/Shell.h"
#include "include/Buffer.h"
#include "include/lib.h"
#include "include/usrSysCall.h"
#include "include/eliminator.h"
#include <stdio.h>
#include "tests/test_processes.h"
#include "tests/test_util.h"
#define WHITE 0xFFFFFFFF
static char buffer[BUFFER_SIZE] = {0};
int exitFlag = 0;
int registerFlag = 0;

void lineRead(char *buffer);
void call_InvalidOp();
char reSize(char *buffer);
void call_div0();

void startingLine()
{
    char *startingLine = "$>";
    putString(startingLine, GREEN);
    clearBuffer(buffer);
}

void bufferControl()
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
                    clearBuffer(buffer);
                    return;
                }
                buffer[i] = 0;
                lineRead(buffer);
                clearBuffer(buffer); // para limpiarlo ahora q lineRead no lo uso
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
};

void lineRead(char *buffer)
{
    if (strcmp(buffer, "help") == 0)
    {
        putString("The following commands may be used: \n", WHITE);
        for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
        {
            putString(commands[i], WHITE);
            putString("\n", WHITE);
        }
    }
    else if (strcmp(buffer, "eliminator") == 0)
    {
        startEliminator();
    }
    else if (strcmp(buffer, "time") == 0)
    {
        char time[9]; // Viene dada por el formato hh:mm:ss por eso son 8 mas la terminacion en cero
        call_timeClock(time);
        putString(time, WHITE);
        putString("\n", WHITE);
    }
    else if (strcmp(cutString(buffer), "setFont") == 0)
    {
        if (reSize(buffer))
        {
            call_clear();
        }
        else
        {
            putString("Enter a valid size (1 or 2) \n", RED);
        }
        clearBuffer(buffer);
    }
    else if (strcmp(buffer, "getRegisters") == 0)
    {
        call_printRegisters(1);
    }
    else if (strcmp(buffer, "clear") == 0)
    {
        call_clear();
        clearBuffer(buffer);
    }
    else if (strcmp(buffer, "exit") == 0)
    {
        exitFlag = 1;
        call_clear();
        clearBuffer(buffer);
        return;
    }
    else if (strcmp(buffer, "div0") == 0)
    {
        call_div0();
        return;
    }
    else if (strcmp(buffer, "invalidOp") == 0)
    {
        call_InvalidOp();
        return;
    }
    else if (strcmp(buffer, "status") == 0)
    {
        call_status();
        return;
    }
    else if (strcmp(buffer, "testmm") == 0)
    {
        char **argv_mm = (char **)(uintptr_t)call_malloc(2 * sizeof(char *));
        argv_mm[0] = "mem test";
        argv_mm[1] = "266240";
        call_create_process(test_mm, 1, 1, argv_mm);
        call_free(argv_mm);
        return;
    }
    else if (strcmp(buffer, "testprocess") == 0)
    {
        char **argv_process = (char **)(uintptr_t)call_malloc(2 * sizeof(char *));
        argv_process[0] = "process test";
        argv_process[1] = "10";
        call_create_process(test_processes, 1, 1, argv_process);
        call_free(argv_process);
        return;
    }
    else if (strcmp(buffer, "testprio") == 0)
    {
        char **argv_priority = (char **)(uintptr_t)call_malloc(sizeof(char *));
        argv_priority[0] = "process test";
        call_create_process(test_prio, 1, 1, argv_priority);
        call_free(argv_priority);
        return;
    }
    else if (strcmp(cutString(buffer), "kill") == 0)
    {
        char *init = buffer + strlen("kill ");
        if (!strlen(init))
        {
            return;
        }
        call_kill(strToInt(init));
        return;
    }
    else if (strcmp(buffer, "ps") == 0)
    {
        call_ps();
        return;
    }
    else
    {
        putString(buffer, WHITE);
        putString(":command not found", WHITE);
        putString("\n", WHITE);
    }
    call_printRegisters(0);
}

char reSize(char *buffer)
{
    char *init = buffer + strlen("setFont ");
    if (!strlen(init))
        return 0;
    return (char)call_setFontSize(strToInt(init));
}

int shellInit()
{
    char *start = "Welcome to IncFont OS, type help to get a list of commands.\n";
    putString(start, GREEN);
    clearBuffer(buffer);
    while (!exitFlag)
    {
        startingLine();
        bufferControl();
    }
    return 0;
}
void call_InvalidOp()
{
    InvalidOpasm();
}
void call_div0()
{
    int a = 7;
    int b = 0;
    a = a / b;
}