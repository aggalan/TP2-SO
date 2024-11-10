// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "include/lib.h"
#include "include/usr_sys_calls.h"
#include <stdarg.h>

#define TO_UPPER(c) (((c) >= 'a' && (c) <= 'z') ? ((c) - ('a' - 'A')) : (c))

int getC(char *c)
{
    return call_sys_read(STDIN, c, 1);
}
int get_buffer(char *buf)
{
    return call_sys_read(STDIN, buf, 100);
}
int putC(char c, uint32_t hex_color)
{
    return call_sys_write(STDOUT, &c, 1, hex_color);
}

void put_int(uint64_t num, uint32_t hex_color)
{

    int divisor = 1;
    while (num / divisor >= 10)
    {
        divisor *= 10;
    }

    while (divisor > 0)
    {
        int digit = num / divisor;
        putC('0' + digit, hex_color);
        num %= divisor;
        divisor /= 10;
    }
}
ssize_t put_string(const char *s, uint32_t hex_color)
{
    return call_sys_write(STDOUT, s, str_len(s), hex_color);
}

void print(int color, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    while (*format)
    {
        if (*format == '%' && *(format + 1) == 'd')
        {
            int num = va_arg(args, int);
            if (num < 0)
            {
                putC('-', color);
                num = -num;
            }
            put_int(num, color);
            format += 2;
        }
        else if (*format == '%' && *(format + 1) == 's')
        {
            const char *str = va_arg(args, const char *);
            put_string(str, color);
            format += 2;
        }
        else
        {
            putC(*format, color);
            format++;
        }
    }

    va_end(args);
}
int str_len(const char *str)
{
    int length = 0;
    while (str[length] != '\0')
    {
        length++;
    }
    return length;
}

void str_cpy(char *dest, const char *src)
{
    while (*src != '\0')
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

int str_cmp(char *str1, char *str2)
{
    while (*str1 && *str2 && *str1 == *str2)
    {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}
char *cut_string(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == ' ')
        {
            str[i] = '\0';
            return str;
        }
    }
    return str;
}

char *str_chr(const char *s, int c)
{
    while (*s != '\0')
    {
        if (*s == (char)c)
        {
            return (char *)s;
        }
        s++;
    }

    if (c == '\0')
    {
        return (char *)s;
    }

    return (void *)0;
}

int pow(int base, int pow)
{
    int toR = 1;
    for (int i = 0; i < pow; i++)
    {
        toR *= base;
    }
    return toR;
}
int str_to_int(char *str)
{
    int n = str_len(str);
    int rta = 0;

    for (int i = 0; i < n; i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return -1;
        }
        rta += (str[i] - '0') * pow(10, n - i - 1);
    }

    return rta;
}

void str_to_upper(char *str)
{
    while (*str)
    {
        *str = TO_UPPER(*str);
        str++;
    }
}

void loop()
{
    static int call_count = 0;
    const int print_interval = 150000000;
    pid_t pid = call_get_current_pid();

    while (1)
    {
        call_count++;

        if (call_count % print_interval == 0) // So all the process arent printing on the same moment (otherwise only one pid shows)
        {
            print(0xFFFFFF, "%d", pid);
        }
    }
}
int log(uint64_t n)
{
    int i = 0;
    while (n /= 10)
        i++;
    return i;
}

void int_to_str(uint64_t n, char *buffer)
{
    if (n == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    unsigned int len = 0;
    int i = 0;

    len += log(n) + 1;
    while (n != 0)
    {
        int r = n % 10;
        buffer[len - i++ - 1] = r + '0';
        n /= 10;
    }
    buffer[len] = '\0';
}

void print_spaces(int count)
{
    for (int i = 0; i < count; i++)
    {
        print(WHITE, " ");
    }
}

int count_digits(unsigned long num, int base)
{
    if (num == 0)
        return 1;

    int count = 0;
    while (num > 0)
    {
        num /= base;
        count++;
    }
    return count;
}

process_list_t *print_process_userland()
{
    process_list_t *list = call_get_process_list();
    if (!list)
        return NULL;

    print(WHITE, "There are %d processes in the system\n", list->count);
    print(WHITE, "PID    NAME          PRIORITY   STACK BASE   RSP        STATE    GROUND\n");

    for (size_t i = 0; i < list->count; i++)
    {
        const process_info_t *proc = &list->processes[i];

        print(WHITE, "%d", proc->pid);
        print_spaces(7 - count_digits(proc->pid, 10));

        print(WHITE, "%s", proc->name);
        print_spaces(14 - str_len(proc->name));

        print(WHITE, "%d", proc->priority);
        print_spaces(11 - count_digits(proc->priority, 10));

        print(WHITE, "%s", proc->stack_base);
        print_spaces(13 - str_len(proc->stack_base));

        print(WHITE, "%s", proc->rsp);
        print_spaces(11 - str_len(proc->rsp));

        const char *state_str;
        switch (proc->state)
        {
        case READY:
            state_str = "READY";
            break;
        case BLOCKED:
            state_str = "BLOCKED";
            break;
        case RUNNING:
            state_str = "RUNNING";
            break;
        case ZOMBIE:
            state_str = "ZOMBIE";
            break;
        case WAITING:
            state_str = "WAITING";
            break;
        case EXITED:
            state_str = "EXITED";
            break;
        case BLOCKED_IO:
            state_str = "BLOCKED IO";
            break;
        default:
            state_str = "UNKNOWN";
            break;
        }
        print(WHITE, "%s", state_str);
        print_spaces(9 - str_len(state_str));

        print(WHITE, "%s\n", proc->ground == 0 ? "BACKGROUND" : "FOREGROUND");
    }

    return list;
}

void memory_status_userland()
{
    memory_status_t *info = call_status();
    if (info == NULL)
        return;

    print(WHITE, "Running on %s\n", info->allocator_type);
    print(WHITE, "TOTAL: %d\n", info->total_memory);
    print(WHITE, "USED: %d\n", info->used_memory);
    print(WHITE, "FREE: %d\n", info->free_memory);

    call_free(info);
}