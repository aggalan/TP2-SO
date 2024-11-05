// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "include/Shell.h"
#include "include/Buffer.h"
#include "include/lib.h"
#include "include/usr_sys_calls.h"
#include "include/eliminator.h"
#include "tests/test_processes.h"
#include "tests/test_util.h"
#include "tests/test_sync.h"
#include "include/commands.h"
#include <stddef.h>
#define WHITE 0xFFFFFFFF

typedef void (*command_func_t)(char *args);

typedef struct
{
    char *command;
    command_func_t func;
    const char *description;
} command_t;

static char buffer[BUFFER_SIZE] = {0};
int exit_flag = 0;
int register_flag = 0;

void normalize_whitespace(char * str);
int isspace(char c);
void line_read(char *buffer);
void cmd_exit(char *args);
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

        if (c != 0 && c != '\t' && c != EOF)
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

command_t tests[] = {
    {"mem", mm_test, "Runs the memory test."},
    {"process", process_test, "Runs the process test."},
    {"prio", prio_test, "Runs the priority test."},
    {"sync", sync_test, "Runs the sync test. (usage: sync <N value> [-no-sem])"},
    {"pipes", pipe_test, "Runs the pipes test."}};

command_t commands[] = {
    {"help", cmd_help, "Displays this help message."},
    {"eliminator", cmd_eliminator, "Starts the eliminator."},
    {"clear", call_clear, "Clears the screen."},
    {"set_font", cmd_set_font, "Sets the font size."},
    {"status", call_status, "Displays the status of the memory in the system."},
    {"tests", cmd_tests, "Displays runnable tests"},
    {"ps", cmd_ps, "Displays the processes in the system."},
    {"time", cmd_time, "Displays the current time."},
    {"set_font", cmd_set_font, "Sets the font size."},
    {"kill", cmd_kill, "Kills a process. (usage: kill <pid>)"},
    {"annihilate", cmd_annihilate, "Kills all processes except for the Shell."},
    {"block", cmd_block, "Blocks a process. (usage: block <pid>)"},
    {"unblock", cmd_unblock, "Unblocks a process. (usage: unblock <pid>)"},
    {"changeprio", cmd_changeprio, "Changes the priority of a process. (usage: changeprio <pid> <newPrio>)"},
    {"div_0", call_div0, "Generates a division by zero exception."},
    {"invalid_op", call_invalid_op, "Generates an invalid operation exception."},
    {"get_registers", cmd_print_registers, "Prints the registers of the current process."},
    {"exit", cmd_exit, "Exits the shell."},
    {"loop", busy_wait, "Creates a process whose purpose is to be monitored"},
    {"cat", cat_process, "Prints the input received."},
    {"filter", filter_process, "Filters the input received."},
    {"wc", wc_process, "Counts the words in the input received."},
    {"phylo", phylo_init_process, "Initializes the philosopher's problem."}};

void line_read(char *buffer)
{
    normalize_whitespace(buffer);
    char * command_copy = (char *)call_malloc(sizeof(char) * (str_len(buffer) + 1));
    if(command_copy == NULL){
        put_string("Error: malloc failed\n", WHITE);
        return;
    }
    str_cpy(command_copy, buffer);
    char *command = cut_string(command_copy);
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
    {
        if (str_cmp(command, commands[i].command) == 0)
        {
            commands[i].func(buffer);
            call_free(command_copy);
            return;
        }
    }

    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        if (str_cmp(command, tests[i].command) == 0)
        {
            tests[i].func(buffer);
            call_free(command_copy);
            return;
        }
    }

    put_string(buffer, WHITE);
    put_string(": command not found", WHITE);
    put_string("\n", WHITE);
    call_free(command_copy);
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

void cmd_help(char *args)
{
    put_string("The following commands may be used: \n", WHITE);
    int max_len = 0;
    int aux = 0;
    for (int j = 0; j < sizeof(commands) / sizeof(commands[0]); j++)
    {
        if ((aux = str_len(commands[j].command)) > max_len)
        {
            max_len = aux;
        }
    }
    max_len += 2;
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
    {
        put_string(commands[i].command, WHITE);
        put_string(": ", WHITE);
        for (int z = 0; z < (max_len - str_len(commands[i].command)); z++)
        {
            put_string(" ", WHITE);
        }
        put_string(commands[i].description, WHITE);
        put_string("\n", WHITE);
    }
}

void cmd_tests()
{
    call_clear();
    put_string("The following tests may be run: \n", WHITE);
    int max_len = 0;
    int aux = 0;
    for (int j = 0; j < sizeof(tests) / sizeof(tests[0]); j++)
    {
        if ((aux = str_len(tests[j].command)) > max_len)
        {
            max_len = aux;
        }
    }
    max_len += 2;
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        put_string(tests[i].command, WHITE);
        put_string(": ", WHITE);
        for (int z = 0; z < (max_len - str_len(tests[i].command)); z++)
        {
            put_string(" ", WHITE);
        }
        put_string(tests[i].description, WHITE);
        put_string("\n", WHITE);
    }
}

void cmd_exit(char *args)
{
    exit_flag = 1;
    call_clear();
    clear_buffer(args);
}

void normalize_whitespace(char *str) {
    int i = 0, j = 0;
    int space_found = 0;

    while (isspace((unsigned char)str[i])) {
        i++;
    }

    while (str[i] != '\0') {
        if (!isspace((unsigned char)str[i])) {
            str[j++] = str[i];
            space_found = 0;
        } else if (!space_found) {
            str[j++] = ' ';
            space_found = 1;
        }
        i++;
    }


    if (j > 0 && str[j - 1] == ' ') {
        j--;
    }


    str[j] = '\0';
}

int isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}