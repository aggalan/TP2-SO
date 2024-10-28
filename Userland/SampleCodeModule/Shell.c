#include "include/Shell.h"
#include "include/Buffer.h"
#include "include/lib.h"
#include "include/usr_sys_calls.h"
#include "include/eliminator.h"
#include <stdio.h>
#include "tests/test_processes.h"
#include "tests/test_util.h"
#include "tests/test_sync.h"
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

void cmd_help(char *args);
void cmd_tests();
void cmd_eliminator(char *args);
void cmd_time(char *args);
void cmd_setFont(char *args);
void cmd_kill(char *args);
void cmd_annihilate(char *args);
void cmd_block(char *args);
void cmd_unblock(char *args);
void cmd_changeprio(char *args);
void mm_test();
void process_test();
void prio_test();
void cmd_set_font(char *args);
void cmd_print_registers();
void cmd_exit();
void cmd_ps();
void sync_test();
void busy_wait();

command_t tests[] = {
    {"mem", mm_test, "Runs the memory test."},
    {"process", process_test, "Runs the process test."},
    {"prio", prio_test, "Runs the priority test."},
    {"sync", sync_test, "Runs the sync test. (usage: sync [-sem])"}};

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
    {"loop", busy_wait, "Creates a process whose purpose is to be monitored"}};

void line_read(char *buffer) {
    char *command = cut_string(buffer);
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if (str_cmp(command, commands[i].command) == 0) {
            commands[i].func(buffer);
            return;
        }
    }
    
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        if (str_cmp(command, tests[i].command) == 0) {
            tests[i].func(buffer);
            return;
        }
    }
    put_string(buffer, WHITE);
    put_string(": command not found", WHITE);
    put_string("\n", WHITE);
}

void cmd_ps()
{
    call_ps();
}

void cmd_exit()
{
    exit_flag = 1;
    call_clear();
    clear_buffer(buffer);
}

void cmd_print_registers()
{
    call_print_registers(1);
}

void mm_test()
{
    char **argv_mm = (char **)(uintptr_t)call_malloc(2 * sizeof(char *));
    argv_mm[0] = (char *)call_malloc(sizeof(char) * (str_len("mem test") + 1));
    str_cpy(argv_mm[0], "mem test");
    argv_mm[1] = (char *)call_malloc(sizeof(char) * (str_len("266240") + 1));
    str_cpy(argv_mm[1], "266240");
    char *str = buffer + str_len("mem ");
    if (*str == '&')
    {
        call_create_process(test_mm, 1, 2, argv_mm, 0);
    }
    else
    {
        call_create_process(test_mm, 1, 2, argv_mm, 1);
    }
    return;

}
void process_test()
{
    char **argv_process = (char **)(uintptr_t)call_malloc(2 * sizeof(char *));
    argv_process[0] = (char *)call_malloc(sizeof(char) * (str_len("process test") + 1));
    str_cpy(argv_process[0], "process test");
    argv_process[1] = (char *)call_malloc(sizeof(char) * (str_len("10") + 1));
    str_cpy(argv_process[1], "10");
    char *str = buffer + str_len("process ");
    if (*str == '&')
    {
        call_create_process(test_processes, 1, 2, argv_process, 0);
    }
    else
    {
        call_create_process(test_processes, 1, 2, argv_process, 1);
    }
}
void prio_test()
{
    char **argv_priority = (char **)(uintptr_t)call_malloc(sizeof(char *));
    argv_priority[0] = (char *)call_malloc(sizeof(char) * (str_len("priority test") + 1));
    str_cpy(argv_priority[0], "priority test");
    char *str = buffer + str_len("priority ");
    if (*str == '&')
    {
        call_create_process(test_prio, 1, 1, argv_priority, 0);
    }
    else
    {
        call_create_process(test_prio, 1, 1, argv_priority, 1);
    }
}

void sync_test()
{
    char **argv_sync = (char **)(uintptr_t)call_malloc(4 * sizeof(char *));
    argv_sync[0] = (char *)call_malloc(sizeof(char) * (str_len("sync test") + 1));
    argv_sync[1] = (char *)call_malloc(sizeof(char));
    argv_sync[2] = (char *)call_malloc(sizeof(char));
    argv_sync[3] = (char *)call_malloc(sizeof(char));
    str_cpy(argv_sync[0], "sync test");
    str_cpy(argv_sync[1], "6");
    str_cpy(argv_sync[3], "0");
    char *str = buffer + str_len("sync ");
    if (str_cmp(cut_string(str), "-sem") == 0)
    {
        print(WHITE, "USING SEMAPHORES\n");
        str_cpy(argv_sync[2], "1");
        if (str_cmp(str, "sem &") == 0)
            call_create_process(test_sync, 1, 4, argv_sync, 0);
        else
            call_create_process(test_sync, 1, 4, argv_sync, 1);
    }
    else
    {
        print(WHITE, "NOT USING SEMAPHORES\n");
        str_cpy(argv_sync[2], "0");
        if (str_cmp(str, "&") == 0)
            call_create_process(test_sync, 1, 4, argv_sync, 0);
        else
            call_create_process(test_sync, 1, 4, argv_sync, 1);
    }

}

void busy_wait()
{
    char **argv_loop = (char **)(uintptr_t)call_malloc(sizeof(char *));
    argv_loop[0] = (char *)call_malloc(sizeof(char) * (str_len("loop") + 1));
    str_cpy(argv_loop[0], "loop");
    call_create_process(loop, 1, 1, argv_loop, 0);
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

void cmd_eliminator(char *args)
{
    start_eliminator();
}

void cmd_time(char *args)
{
    char time[9]; // Viene dada por el formato hh:mm:ss por eso son 8 mas la terminacion en cero
    call_time_clock(time);
    put_string(time, WHITE);
    put_string("\n", WHITE);
}

void cmd_set_font(char *args)
{
    if (re_size(args))
    {
        call_clear();
    }
    else
    {
        put_string("Invalid font size.\n", WHITE);
    }
}

void cmd_kill(char *args)
{
    char *pid_str = buffer + str_len("kill ");
    if (!str_len(pid_str))
    {
        put_string("Usage: kill <pid>\n", WHITE);
        return;
    }
    if (str_to_int(pid_str) == 1)
    {
        put_string("You can't kill the shell.\nTo exit the shell, type 'exit'\n", WHITE);
    }
    else if (call_kill(str_to_int(pid_str)) == -1)
    {
        put_string("Invalid pid.\n", WHITE);
    }
    else
    {
        put_string("Process killed.\n", WHITE);
    }
}

void cmd_annihilate(char *args)
{
    call_annihilate();
}

void cmd_block(char *args)
{
    char *pid_str = buffer + str_len("block ");
    if (!str_len(pid_str))
    {
        put_string("Usage: block <pid>\n", WHITE);
        return;
    }
    if (call_block(str_to_int(pid_str)) == -1)
    {
        put_string("Invalid pid.\n", WHITE);
    }
    else
    {
        put_string("Process blocked.\n", WHITE);
    }
}

void cmd_unblock(char *args)
{
    char *pid_str = buffer + str_len("unblock ");
    if (!str_len(pid_str))
    {
        put_string("Usage: unblock <pid>\n", WHITE);
        return;
    }
    if (call_unblock(str_to_int(pid_str)) == -1)
    {
        put_string("Invalid pid.\n", WHITE);
    }
    else
    {
        put_string("Process unblocked.\n", WHITE);
    }
}

void cmd_changeprio(char *args)
{
    char *pid_str = cut_string(buffer + str_len("changeprio "));
    char *newPrio_str = buffer + str_len("changeprio ") + str_len(pid_str) + 1;
    if (!str_len(pid_str) || !str_len(newPrio_str))
    {
        put_string("Usage: changeprio <pid> <newPrio>\n", WHITE);
        return;
    }
    if (call_change_priority(str_to_int(pid_str), str_to_int(newPrio_str)) == -1)
    {
        put_string("Invalid pid or priority.\n", WHITE);
    }
    else
    {
        put_string("Priority changed.\n", WHITE);
    }
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