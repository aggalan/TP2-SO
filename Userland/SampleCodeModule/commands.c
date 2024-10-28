#include "include/commands.h"
#include "include/usr_sys_calls.h"
#include "tests/test_processes.h"
#include "tests/test_util.h"
#include "tests/test_sync.h"
#include "include/lib.h"
#include "include/eliminator.h"

#define WHITE 0xFFFFFFFF

void cmd_ps()
{
    call_ps();
}

void cmd_print_registers()
{
    call_print_registers(1);
}

void mm_test(char *args)
{
    char **argv_mm = (char **)(uintptr_t)call_malloc(2 * sizeof(char *));
    argv_mm[0] = (char *)call_malloc(sizeof(char) * (str_len("mem test") + 1));
    str_cpy(argv_mm[0], "mem test");
    argv_mm[1] = (char *)call_malloc(sizeof(char) * (str_len("266240") + 1));
    str_cpy(argv_mm[1], "266240");
    char *str = args + str_len("mem ");
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
void process_test(char *args)
{
    char **argv_process = (char **)(uintptr_t)call_malloc(2 * sizeof(char *));
    argv_process[0] = (char *)call_malloc(sizeof(char) * (str_len("process test") + 1));
    str_cpy(argv_process[0], "process test");
    argv_process[1] = (char *)call_malloc(sizeof(char) * (str_len("10") + 1));
    str_cpy(argv_process[1], "10");
    char *str = args + str_len("process ");
    if (*str == '&')
    {
        call_create_process(test_processes, 1, 2, argv_process, 0);
    }
    else
    {
        call_create_process(test_processes, 1, 2, argv_process, 1);
    }
}
void prio_test(char *args)
{
    char **argv_priority = (char **)(uintptr_t)call_malloc(sizeof(char *));
    argv_priority[0] = (char *)call_malloc(sizeof(char) * (str_len("priority test") + 1));
    str_cpy(argv_priority[0], "priority test");
    char *str = args + str_len("priority ");
    if (*str == '&')
    {
        call_create_process(test_prio, 1, 1, argv_priority, 0);
    }
    else
    {
        call_create_process(test_prio, 1, 1, argv_priority, 1);
    }
}

void sync_test(char *args)
{
    char **argv_sync = (char **)(uintptr_t)call_malloc(4 * sizeof(char *));
    argv_sync[0] = (char *)call_malloc(sizeof(char) * (str_len("sync test") + 1));
    argv_sync[1] = (char *)call_malloc(sizeof(char));
    argv_sync[2] = (char *)call_malloc(sizeof(char));
    argv_sync[3] = (char *)call_malloc(sizeof(char));
    str_cpy(argv_sync[0], "sync test");
    str_cpy(argv_sync[1], "6");
    str_cpy(argv_sync[3], "0");
    char *str = args + str_len("sync ");
    if (str_cmp(cut_string(str), "") == 0)
    {
        print(WHITE, "USING SEMAPHORES\n");
        str_cpy(argv_sync[2], "1");
        if (str_cmp(str, "sem &") == 0)
            call_create_process(test_sync, 1, 4, argv_sync, 0);
        else
            call_create_process(test_sync, 1, 4, argv_sync, 1);
    }
    else if (str_cmp(str, "-nosem") == 0)
    {
        print(WHITE, "NOT USING SEMAPHORES\n");
        str_cpy(argv_sync[2], "0");
        if (str_cmp(str, "&") == 0)
            call_create_process(test_sync, 1, 4, argv_sync, 0);
        else
            call_create_process(test_sync, 1, 4, argv_sync, 1);
    }
    else
    {
        print(WHITE, "Invalid flag\n");
    }
}

void busy_wait()
{
    char **argv_loop = (char **)(uintptr_t)call_malloc(sizeof(char *));
    argv_loop[0] = (char *)call_malloc(sizeof(char) * (str_len("loop") + 1));
    str_cpy(argv_loop[0], "loop");
    call_create_process(loop, 1, 1, argv_loop, 0);
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
    if (re_size(cut_string(args)))
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
    char *pid_str = args + str_len("kill ");
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
    char *pid_str = args + str_len("block ");
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
    char *pid_str = args + str_len("unblock ");
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
    char *pid_str = cut_string(args + str_len("changeprio "));
    char *newPrio_str = args + str_len("changeprio ") + str_len(pid_str) + 1;
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
    char *init = buffer + str_len("set_font ");
    if (!str_len(init))
        return 0;
    return (char)call_set_font_size(str_to_int(init));
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