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
void putC(char c, uint32_t hex_color)
{
    call_sys_write(STDOUT, &c, 1, hex_color);
}

void put_int(uint64_t num, uint32_t hex_color)
{
    if (num < 0)
    {
        putC('-', hex_color);
        num = -num;
    }

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
void put_string(const char *s, uint32_t hex_color)
{
    call_sys_write(STDOUT, s, str_len(s), hex_color);
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
                putC('-', 0xFFFFFF);
                num = -num;
            }
            put_int(num, 0xFFFFFF); // Assuming print_number is a function that prints positive numbers
            format += 2;
        }
        else
        {
            putC(*format, 0xFFFFFF);
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
    *dest = '\0'; // Add the null terminator
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
        rta += (str[i] - '0') * pow(10, n - i - 1);
    }
    return rta;
}

// Function to convert a string to uppercase
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
    while (1)
    {
        ;
    }
}