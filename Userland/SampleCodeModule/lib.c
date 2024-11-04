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
            put_int(num, color); // Assuming put_int is a function that prints positive numbers
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

char * str_chr(const char *s, int c) {
    while (*s != '\0') {
        if (*s == (char)c) {
            return (char *)s;
        }
        s++;
    }
    
    if (c == '\0') {
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
int str_to_int(char *str) {
    int n = str_len(str);
    int rta = 0;

    for (int i = 0; i < n; i++) {
        // Check if the character is not a digit
        if (str[i] < '0' || str[i] > '9') {
            return -1;  // Return -1 if a non-numeric character is found
        }
        // Convert character to integer and add it to the result
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
    int flag = 1;
    int ticks = 0;
    while (1)
    {
        ticks = call_get_ticks();
        if (ticks % 100 == 0 && flag)
        {
            print(0xFFFFFF, "%d", call_get_current_pid());
            flag = 0;
        }
        else if (ticks % 18 == 1 && !flag)
        {
            flag = 1;
        }
        // ESTO QUEDA FEO, DESCOMENTARLO PARA ENTREGAR FINAL O PREGUNTar
        ;

    }
}