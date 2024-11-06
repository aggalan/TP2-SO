// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <stdint.h>
#include <time.h>
#include "include/interrupts.h"
#include "include/process_manager.h"
#include "include/scheduler.h"
#include <stdarg.h>
#include "../Drivers/include/video_driver.h"
#include "include/lib.h"
#include "include/syscalls.h"

void put_int_kernel(uint64_t num, uint32_t hex_color);
void put_string_kernel(const char *str, uint32_t hex_color);

void *memset(void *destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char *dst = (char *)destination;

	while (length--)
		dst[length] = chr;

	return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length)
{
	/*
	 * memcpy does not support overlapping buffers, so always do it
	 * forwards. (Don't change this without adjusting memmove.)
	 *
	 * For speedy copying, optimize the common case where both pointers
	 * and the length are word-aligned, and copy word-at-a-time instead
	 * of byte-at-a-time. Otherwise, copy by bytes.
	 *
	 * The alignment logic below should be portable. We rely on
	 * the compiler to be reasonably intelligent about optimizing
	 * the divides and modulos out. Fortunately, it is.
	 */
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *)destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t *d = (uint8_t *)destination;
		const uint8_t *s = (const uint8_t *)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

void sleepms(uint32_t ms)
{
	uint32_t start = ticks_elapsed();
	while (ms > 18 * (ticks_elapsed() - start))
	{
		_hlt();
	}
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

int str_cpy(char *dest, const char *src)
{
	int i = 0;
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
	return i;
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

void idle()
{
	while (1)
	{
		_hlt();
	}
}

void exit_process(int status)
{
	PCB *pcb = get_current();
	kill_process_pid(pcb->pid);
}

void print_kernel(int color, const char *format, ...)
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
				sys_write(STDOUT, "-", 1, color);
				num = -num;
			}
			put_int_kernel(num, color);
			format += 2;
		}
		else if (*format == '%' && *(format + 1) == 's')
		{
			const char *str = va_arg(args, const char *);
			put_string_kernel(str, color);
			format += 2;
		}
		else
		{
			sys_write(STDOUT, format, 1, color);
			format++;
		}
	}

	va_end(args);
}

void put_int_kernel(uint64_t num, uint32_t hex_color)
{
	char buffer[20];
	int i = 0;

	if (num == 0)
	{
		buffer[i++] = '0';
	}
	else
	{
		while (num > 0)
		{
			buffer[i++] = (num % 10) + '0';
			num /= 10;
		}
	}

	buffer[i] = '\0';

	for (int j = 0; j < i / 2; j++)
	{
		char temp = buffer[j];
		buffer[j] = buffer[i - j - 1];
		buffer[i - j - 1] = temp;
	}

	put_string_kernel(buffer, hex_color);
}
void put_string_kernel(const char *str, uint32_t hex_color)
{
	sys_write(STDOUT, str, str_len(str), hex_color);
}