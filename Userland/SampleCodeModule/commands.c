// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "include/commands.h"
#include "include/usr_sys_calls.h"
#include "tests/include/test_processes.h"
#include "tests/include/test_util.h"
#include "tests/include/test_sync.h"
#include "include/lib.h"
#include "include/eliminator.h"
#include "include/phylo.h"

#define MAX_LENGTH 1024

static int ampersen_searcher(char *str);

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

    static char *argv[2] = {"mem test", "266240"};

    int ampersen = ampersen_searcher(args);

    if (ampersen == -1)
    {
        print(WHITE, "Syntax error on or near '&'\n");
        return;
    }
    else if (ampersen)
    {
        call_create_process(test_mm, 0, 2, argv, 0);
    }
    else
    {
        call_create_process(test_mm, 0, 2, argv, 1);
    }
    return;
}
void process_test(char *args)
{
    static char *argv[2] = {"process test", "7"};

    int ampersen = ampersen_searcher(args);

    if (ampersen == -1)
    {
        print(WHITE, "Syntax error on or near '&'\n");
        return;
    }
    else if (ampersen)
    {
        call_create_process(test_processes, 0, 2, argv, 0);
    }
    else
    {
        call_create_process(test_processes, 0, 2, argv, 1);
    }
}
void prio_test(char *args)
{
    static char *argv[1] = {"priority test"};

    int ampersen = ampersen_searcher(args);

    if (ampersen == -1)
    {
        print(WHITE, "Syntax error on or near '&'\n");
        return;
    }
    else if (ampersen)
    {
        call_create_process(test_prio, 0, 1, argv, 0);
    }
    else
    {
        call_create_process(test_prio, 0, 1, argv, 1);
    }
}

void sync_test(char *args)
{
    // Malloc must be used here becauas eotherwise the reference to the argvs are lost
    char **argv_sync = (char **)(uintptr_t)call_malloc(4 * sizeof(char *));

    argv_sync[0] = (char *)call_malloc(sizeof(char) * (str_len("sync test") + 1));
    argv_sync[1] = (char *)call_malloc(sizeof(char));
    argv_sync[2] = (char *)call_malloc(sizeof(char));
    argv_sync[3] = (char *)call_malloc(sizeof(char));

    str_cpy(argv_sync[0], "sync test");
    str_cpy(argv_sync[3], "0");

    char *str = args + str_len("sync ");
    int aux = str_len(cut_string(str));
    char *n = cut_string(str);

    if (str_to_int(n) < 1)
    {
        print(WHITE, "INVALID N VALUE\n");
        for (int i = 0; i < 4; i++)
        {
            call_free(argv_sync[i]);
        }
        call_free(argv_sync);
        return;
    }

    str_cpy(argv_sync[1], n);

    str += aux + 1;

    char *aux2 = str + str_len(cut_string(str)) + 1;

    if (str_cmp(cut_string(str), "-no-sem") == 0)
    {
        str_cpy(argv_sync[2], "0");

        if (ampersen_searcher(aux2))
        {
            call_create_process(test_sync, 0, 4, argv_sync, 0);
        }
        else
        {
            call_create_process(test_sync, 0, 4, argv_sync, 1);
        }
    }
    else if (str_cmp(cut_string(str), "&") == 0)
    {
        str_cpy(argv_sync[2], "1");

        call_create_process(test_sync, 0, 4, argv_sync, 0);
    }
    else if (*str == 0)
    {
        str_cpy(argv_sync[2], "1");
        call_create_process(test_sync, 0, 4, argv_sync, 1);
    }
    else
    {
        print(WHITE, "Invalid flag\n");
    }
}

void pipe_test(char *args)
{

    char *argv[1] = {"pipes test"};

    int ampersen = ampersen_searcher(args);

    if (ampersen == -1)
    {
        print(WHITE, "Syntax error on or near '&'\n");
        return;
    }
    else if (ampersen)
    {
        call_create_process(main_test_pipes, 0, 1, argv, 0);
    }
    else
    {
        call_create_process(main_test_pipes, 0, 1, argv, 1);
    }
}

void busy_wait()
{
    char *argv[1] = {"loop"};
    call_create_process(loop, 0, 1, argv, 0);
}

void cmd_time(char *args)
{
    char time[9];
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

void cat()
{
    char c;
    char command[MAX_LENGTH] = {0};
    int i = 0;
    ssize_t status = -1;

    do
    {
        status = getC(&c);
        if (c != 0 && c != -1)
        {
            putC(c, WHITE);
            command[i++] = c;
            if (c == '\n')
            {
                command[i] = '\0';
                i = 0;
                print(WHITE, "%s", command);
                command[0] = '\0';
            }
        }
    } while (status != EOF && status != 0);
    putC('\n', WHITE);
    return;
}

int is_vowel(char c)
{
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
           c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}

void filter()
{
    char c;
    ssize_t status = -1;
    char command[MAX_LENGTH] = {0};
    int i = 0;
    do
    {
        status = getC(&c);
        if (c != 0 && c != -1)
        {
            putC(c, WHITE);
            if (!is_vowel(c))
            {
                command[i++] = c;
            }
        }

    } while (status != EOF && status != 0);
    print(WHITE, "\n");
    command[i] = '\0';
    print(WHITE, "%s", command);
    putC('\n', WHITE);
}

void wc()
{
    int lines = 1;
    char c;
    ssize_t status = -1;

    do
    {
        status = getC(&c);
        if (c == '\n')
        {
            lines++;
        }
        if (c != 0 && c != -1)
        {
            putC(c, WHITE);
        }
    } while (status != EOF && status != 0);
    putC('\n', WHITE);
    put_string("Lines: ", WHITE);
    put_int(lines, WHITE);
    put_string("\n", WHITE);
}

static int ampersen_searcher(char *input)
{
    int len = str_len(input);
    int ampersand_count = 0;

    for (int i = 0; i < len; i++)
    {
        if (input[i] == '&')
        {
            ampersand_count++;
            if (i != len - 1)
            { // If '&' is not at the end, it's invalid
                return -1;
            }
        }
    }

    // Return true if there's exactly one '&' at the end
    return ampersand_count == 1;
}

void cat_process(char *args)
{

    char *argv[1] = {"cat"};
    int ampersen = ampersen_searcher(args);
    if (ampersen == -1)
    {
        print(WHITE, "Syntax error on or near '&'\n");
        return;
    }
    else if (ampersen)
    {
        call_create_process(cat, 0, 1, argv, 0);
    }
    else
    {
        call_create_process(cat, 0, 1, argv, 1);
    }
}

void filter_process(char *args)
{
    char *argv[1] = {"filter"};
    int ampersen = ampersen_searcher(args);
    if (ampersen == -1)
    {
        print(WHITE, "Syntax error on or near '&'\n");
        return;
    }
    else if (ampersen)
    {
        call_create_process(filter, 0, 1, argv, 0);
    }
    else
    {
        call_create_process(filter, 0, 1, argv, 1);
    }
}

void wc_process(char *args)
{
    char *argv[1] = {"wc"};
    int ampersen = ampersen_searcher(args);
    if (ampersen == -1)
    {
        print(WHITE, "Syntax error on or near '&'\n");
        return;
    }
    else if (ampersen)
    {
        call_create_process(wc, 0, 1, argv, 0);
    }
    else
    {
        call_create_process(wc, 0, 1, argv, 1);
    }
}

void phylo_init_process(char *args)
{
    char *argv[1] = {"phylo"};
    int ampersen = ampersen_searcher(args);
    if (ampersen == -1)
    {
        print(WHITE, "Syntax error on or near '&'\n");
        return;
    }
    else if (ampersen)
    {
        print(WHITE, "Philosopher's problem can all be run in foreground\n");
    }
    else
    {
        call_create_process(phylo_init, 0, 1, argv, 1);
    }
}
