// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <naive_console.h>

static char buffer[64] = {'0'};
static uint8_t *const video = (uint8_t *)0xB8000;
static uint8_t *currentVideo = (uint8_t *)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25;

void ncPrint(const char *string)
{
	int i;

	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

void ncPrintChar(char character)
{
	*currentVideo = character;
	currentVideo += 2;
}

void ncNewline()
{
	do
	{
		ncPrintChar(' ');
	} while ((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)
{
	u_int_to_base(value, buffer, base);
	ncPrint(buffer);
}

void ncClear()
{
	int i;

	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
}

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