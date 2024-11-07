// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "include/Shell.h"
#include "include/Buffer.h"
#include "include/lib.h"
#include "include/usr_sys_calls.h"
#include "include/eliminator.h"
#include "tests/include/test_processes.h"
#include "tests/include/test_util.h"
#include "tests/include/test_sync.h"
#include "include/commands.h"
#include <stddef.h>

typedef void (*command_func_t)(char *args);
#define false 0
#define true 1

typedef struct
{
    char *command;
    command_func_t func;
    const char *description;
    char is_pipeable;
} command_t;

static char buffer[BUFFER_SIZE] = {0};
int exit_flag = 0;
int register_flag = 0;

void normalize_whitespace(char *str);
void extract_commands(const char *buffer, char *command1, char *command2);
int is_pipe_valid(const char *buffer);
void piped_line_read(char *buffer);
int is_space(char c);
void line_read(char *buffer, int *fds);
void cmd_exit(char *args);
void turn_red();

static command_t commands[] = {
    {"help", cmd_help, "Displays this help message.", true},
    {"clear", call_clear, "Clears the screen.", false},
    {"status", call_status, "Displays the status of the memory in the system.", true},
    {"tests", cmd_tests, "Displays runnable tests", false},
    {"ps", cmd_ps, "Displays the processes in the system.", true},
    {"time", cmd_time, "Displays the current time.", true},
    {"set_font", cmd_set_font, "Sets the font size.", false},
    {"kill", cmd_kill, "Kills a process. (usage: kill <pid>)", false},
    {"annihilate", cmd_annihilate, "Kills all processes except for the Shell.", false},
    {"block", cmd_block, "Blocks a process. (usage: block <pid>)", false},
    {"unblock", cmd_unblock, "Unblocks a process. (usage: unblock <pid>)", false},
    {"changeprio", cmd_changeprio, "Changes the priority of a process. (usage: changeprio <pid> <newPrio>)", false},
    {"div_0", call_div0, "Generates a division by zero exception.", false},
    {"invalid_op", call_invalid_op, "Generates an invalid operation exception.", false},
    {"exit", cmd_exit, "Exits the shell.", false},
    {"loop", busy_wait, "Creates a process whose purpose is to be monitored", false},
    {"phylo", phylo_init_process, "Initializes interactive philosopher's dining problem.", false},
    {"cat", cat_process, "Prints the input received.", true},
    {"filter", filter_process, "Filters the input received.", true},
    {"wc", wc_process, "Counts the words in the input received.", true},
    {"red", turn_red, "To test the anonymous pipe. It reads from stdin and prints to the screen in red.", true},
    {"pipes_status", call_pipes_status, "Displays the status of the pipes in the system.", true}};

static command_t tests[] = {
    {"mem", mm_test, "Runs the memory test.", false},
    {"process", process_test, "Runs the process test.", false},
    {"prio", prio_test, "Runs the priority test.", false},
    {"sync", sync_test, "Runs the sync test. (usage: sync <N value> [-no-sem])", false},
    {"pipes", pipe_test, "Runs the pipes test.", false}};

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

        if (c != 0 && c != '\t' && c != (char)EOF)
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
                line_read(buffer, 0);
                clear_buffer(buffer);
                return;
            }
            else if (c == '\b')
            {
                if (i > 0)
                {
                    i--;
                    putC(c, WHITE);
                }
            }
            else
            {
                if (i < BUFFER_SIZE)
                {
                    buffer[i++] = c;
                    putC(c, WHITE);
                }
            }
        }
    }
}

void line_read(char *buffer, int *fds)
{
    normalize_whitespace(buffer);
    if (is_pipe_valid(buffer))
    {
        piped_line_read(buffer);
        return;
    }
    char *command_copy = (char *)call_malloc(sizeof(char) * (str_len(buffer) + 1));
    if (command_copy == NULL)
    {
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

void turn_red()
{
    char buff[3000] = {0};
    call_sys_read(STDIN, buff, 3000);
    call_sys_write(STDOUT, buff, str_len(buff), RED);
}

void piped_line_read(char *buffer)
{
    char command1[20];
    char command2[20];
    extract_commands(buffer, command1, command2);

    command_func_t func1 = NULL;
    command_func_t func2 = NULL;

    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
    {
        if (str_cmp(cut_string(command1), commands[i].command) == 0)
        {
            if (commands[i].is_pipeable == false)
            {
                print(RED, "Command %s is not pipeable\n", commands[i].command);
                return;
            }
            else if (str_cmp(commands[i].command, "filter") == 0)
            {
                func1 = filter;
            }
            else if (str_cmp(commands[i].command, "cat") == 0)
            {
                func1 = cat;
            }
            else if (str_cmp(commands[i].command, "wc") == 0)
            {
                func1 = wc;
            }
            else
            {
                func1 = commands[i].func;
            }
        }
        if (str_cmp(cut_string(command2), commands[i].command) == 0)
        {
            if (commands[i].is_pipeable == false)
            {
                print(RED, "Command %s is not pipeable\n", commands[i].command);
                return;
            }
            else if (str_cmp(commands[i].command, "filter") == 0)
            {
                func2 = filter;
            }
            else if (str_cmp(commands[i].command, "cat") == 0)
            {
                func2 = cat;
            }
            else if (str_cmp(commands[i].command, "wc") == 0)
            {
                func2 = wc;
            }

            else
            {
                func2 = commands[i].func;
            }
        }
    }

    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        if (str_cmp(cut_string(command1), tests[i].command) == 0)
        {
            if (commands[i].is_pipeable == false)
            {
                print(RED, "Command %s is not pipeable\n", tests[i].command);
                return;
            }
            func1 = tests[i].func;
        }
        if (str_cmp(cut_string(command2), tests[i].command) == 0)
        {
            if (commands[i].is_pipeable == false)
            {
                print(RED, "Command %s is not pipeable\n", tests[i].command);
                return;
            }
            func2 = tests[i].func;
        }
    }

    if (func1 == NULL || func2 == NULL)
    {
        print(RED, "Command not found\n");
        return;
    }

    int fd = call_anon_pipe_create();
    static char *argv[] = {"P1"};
    static char *argv2[] = {"P2"};
    int *fds = (int *)call_malloc(sizeof(int) * 3);
    fds[0] = 0;
    fds[1] = fd;
    fds[2] = 2;
    int *fds2 = (int *)call_malloc(sizeof(int) * 3);
    fds2[0] = fd;
    fds2[1] = 1;
    fds2[2] = 2;

    pid_t pid1 = call_create_process(func1, fds, 1, argv, 0);
    pid_t pid2 = call_create_process(func2, fds2, 1, argv2, 0);

    call_waitpid(pid1);
    call_waitpid(pid2);

    call_free(fds);
    call_free(fds2);
}

void extract_commands(const char *buffer, char *command1, char *command2)
{
    int len = str_len(buffer);
    int i = 0;

    command1[0] = '\0';
    command2[0] = '\0';

    while (i < len && buffer[i] != '|')
    {
        if (buffer[i] != ' ')
        {
            int j = 0;
            while (i < len && buffer[i] != ' ' && buffer[i] != '|')
            {
                command1[j++] = buffer[i++];
            }
            command1[j] = '\0';
        }
        i++;
    }

    while (i < len && (buffer[i] == ' ' || buffer[i] == '|'))
    {
        i++;
    }

    int j = 0;
    while (i < len)
    {
        if (buffer[i] != ' ')
        {
            while (i < len && buffer[i] != ' ')
            {
                command2[j++] = buffer[i++];
            }
            break;
        }
        i++;
    }
    command2[j] = '\0';
}

int is_pipe_valid(const char *buffer)
{
    int len = str_len(buffer);

    if (len == 0)
        return 0;

    int found_command = 0;
    int expecting_pipe = 0;
    int found_pipe = 0;
    int command_word_length = 0;

    for (int i = 0; i < len; i++)
    {
        if (buffer[i] == '|')
        {
            if (i == 0 || i == len - 1)
            {
                return 0;
            }
            if (!found_command)
            {
                return 0;
            }
            found_command = 0;
            expecting_pipe = 0;
            found_pipe = 1;
            command_word_length = 0;
        }
        else if (buffer[i] == ' ')
        {
            if (command_word_length > 0)
            {
                if (expecting_pipe)
                {
                    return 0;
                }
                expecting_pipe = 1;
            }
        }
        else
        {
            command_word_length++;
            found_command = 1;
        }
    }
    return found_command && found_pipe;
}

int is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

char to_lower(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c + ('a' - 'A');
    }
    return c;
}

void normalize_whitespace(char *str)
{
    int i = 0, j = 0;
    int space_found = 0;

    while (is_space((unsigned char)str[i]))
    {
        i++;
    }

    while (str[i] != '\0')
    {
        char current = to_lower(str[i]);
        if (!is_space(current))
        {
            str[j++] = current;
            space_found = 0;
        }
        else if (!space_found)
        {
            str[j++] = ' ';
            space_found = 1;
        }
        i++;
    }

    if (j > 0 && str[j - 1] == ' ')
    {
        j--;
    }

    str[j] = '\0';
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