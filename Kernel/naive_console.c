// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <naive_console.h>

int get_hours();
int get_minutes();
int get_seconds();
int gmtM3(int hours)
{
	hours = (hours + 21) % 24;
	return hours;
}
void clock(char *buffer)
{
	int digits = u_int_to_base(gmtM3(get_hours()), buffer, 10);
	buffer[digits++] = ':';
	digits += u_int_to_base(get_minutes(), buffer + digits, 10);
	buffer[digits++] = ':';
	digits += u_int_to_base(get_seconds(), buffer + digits, 10);
	buffer[digits++] = '\0';
}

uint32_t u_int_to_base(uint64_t value, char *buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	*p = 0;

	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}