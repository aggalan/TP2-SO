#include "include/video_driver.h"
#include "include/fonts.h"
#include "../include/naive_console.h"
#include <naive_console.h>
#include <lib.h>
#include "../include/keyboard_buffer.h"
#include "include/scan_code.h"
#include "../include/lib.h"
struct vbe_mode_info_structure
{
	uint16_t attributes;  // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;	  // deprecated
	uint8_t window_b;	  // deprecated
	uint16_t granularity; // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;		   // number of bytes per horizontal line
	uint16_t width;		   // width in pixels
	uint16_t height;	   // height in pixels
	uint8_t w_char;		   // unused...
	uint8_t y_char;		   // ...
	uint8_t planes;
	uint8_t bpp;   // bits per pixel in this mode
	uint8_t banks; // deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer; // physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__((packed));

uint64_t defaultFColor = 0xFFFFFF;
uint64_t defaultBColor = 0x000000;

typedef struct vbe_mode_info_structure *VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr)0x0000000000005C00;

uint32_t cursorX = 0;
uint32_t cursorY = 0;
uint32_t sizeX = FONT_SIZE;
uint32_t sizeY = FONT_SIZE * 2;
uint32_t bg_color = 0x00000000;
// uint8_t buff[256*16];

uint16_t getHeight()
{
	return VBE_mode_info->height;
}
uint16_t getWidth()
{
	return VBE_mode_info->width;
}

uint16_t getCursorX()
{
	return cursorX;
}

uint16_t getCursorY()
{
	return cursorY;
}

void moveUpwards()
{
	char *dest = (char *)(uintptr_t)VBE_mode_info->framebuffer;
	char *src = dest + VBE_mode_info->pitch * sizeY * 8;
	uint64_t size = (VBE_mode_info->height - sizeY * 8) * VBE_mode_info->pitch;
	memcpy(dest, src, size);
	drawRectangle(0x00000000, 0, VBE_mode_info->height - sizeY * 8, sizeY * 8, VBE_mode_info->pitch);
	cursorY -= 8 * sizeY;
}

void putPixel(uint32_t hexColor, uint32_t x, uint32_t y)
{
	uint8_t *framebuffer = (uint8_t *)(uintptr_t)VBE_mode_info->framebuffer;
	uint64_t offset = (x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
	framebuffer[offset] = (hexColor) & 0xFF;
	framebuffer[offset + 1] = (hexColor >> 8) & 0xFF;
	framebuffer[offset + 2] = (hexColor >> 16) & 0xFF;
}

void drawRectangle(uint32_t color, uint32_t x, uint32_t y, uint32_t height, uint32_t width)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			putPixel(color, x + i, y + j);
		}
	}
}

void newLine()
{
	cursorX = 0;
	cursorY += sizeY * 8;
}

void drawSquare(uint32_t hexColor, uint32_t side_length, uint32_t x, uint32_t y)
{
	drawRectangle(hexColor, x, y, side_length, side_length);
}
void drawChar(uint32_t hexColor, char character)
{
	if (character == '\t')
		return;

	if (cursorX >= (uint16_t)VBE_mode_info->width)
	{
		cursorY += sizeY * 8;
		cursorX = 0;
	}
	if (cursorY >= (uint16_t)VBE_mode_info->height)
	{
		cursorX = 0;
		moveUpwards();
	}
	int x = cursorX;
	int y = cursorY;
	int current = x;
	int index = character - 33;
	if (character == ' ')
	{
		cursorX += sizeX * 8;
		return;
	}
	if (character == '\n')
	{
		newLine();
		return;
	}
	if (character == '\b')
	{
		if (cursorX == 0)
		{
			if (cursorY == 0)
			{
				return;
			}
			else
			{
				cursorY -= sizeY * 8;
				cursorX = (uint16_t)VBE_mode_info->width;
			}
		}
		cursorX -= sizeX * 8;
		drawRectangle(0x00000000, cursorX, cursorY, sizeY * 8, sizeX * 8);
		return;
	}

	for (int i = 0; i < 32; i++)
	{
		if (i % 2 == 0 && i != 0)
		{
			current = x;
			y += sizeX; // Salto a la siguiente fila de píxeles
		}
		font[i + index * 32] & (char)0x01 ? drawSquare(hexColor, sizeX, current, y) : 0;
		current += sizeX;
		uint8_t aux = 0x02;

		for (int j = 0; j < 8; j++)
		{
			// Comprueba cada bit de la fuente y dibuja un píxel si está activo
			((uint8_t)font[i + (index * 32)] & (uint8_t)aux) >> j ? drawSquare(hexColor, sizeX, current, y) : 0;
			current += sizeX; // Avanza a la siguiente posición horizontal
			aux <<= 1;		  // Desplaza el bit auxiliar hacia la izquierda
		}
	}
	cursorX += sizeX * 8;
}

void drawWord1(char *string)
{
	drawWord(defaultFColor, string);
}

void address_to_string(void *ptr)
{
	unsigned long address = (unsigned long)ptr;
	char hex_chars[] = "0123456789ABCDEF";
	char buffer[20]; // Local buffer for the address string
	int i = 0;
	int start = 2; // For "0x" prefix

	// Add "0x" prefix to indicate hexadecimal
	buffer[0] = '0';
	buffer[1] = 'x';

	// Fill the buffer from end to start with hex digits
	for (i = start; address > 0; i++)
	{
		buffer[i] = hex_chars[address % 16];
		address /= 16;
	}

	buffer[i] = '\0'; // Null-terminate the string

	// Reverse the portion of the string that contains the hexadecimal representation
	for (int j = start, k = i - 1; j < k; j++, k--)
	{
		char temp = buffer[j];
		buffer[j] = buffer[k];
		buffer[k] = temp;
	}

	drawWord1(buffer); // Call drawWord1 to print the string directly
}

void drawNumber(int value)
{
	drawNumberColor(value, defaultFColor, defaultBColor);
}

void drawNumberColor(int value, uint64_t fcolor, uint64_t bcolor)
{
	char buff[256] = {0};
	intToStr(value, buff, 10);
	drawWord(fcolor, buff);
}

void drawWord(uint32_t hexColor, const char *str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		drawChar(hexColor, str[i]);
	}
}
void drawWordLen(uint32_t hexColor, const char *str, int len)
{
	for (int i = 0; i < len; i++)
	{
		drawChar(hexColor, str[i]);
	}
}

void intToStr(int value, char *str, int base)
{
	char *ptr = str;
	char *ptr1 = str;
	char tmp_char;
	int tmp_value;

	do
	{
		tmp_value = value;
		value /= base;
		*ptr++ = "0123456789"[tmp_value - value * base];
	} while (value);

	*ptr-- = '\0';

	while (ptr1 < ptr)
	{
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
}

char setFontSize(uint32_t size)
{
	if (size > 2 || size <= 0)
	{
		return 0;
	}
	sizeX = size;
	sizeY = size * 2;
	return 1;
}
void clear_bg(uint64_t hexColor)
{
	for (int x = 0; x < VBE_mode_info->width; x++)
	{
		for (int y = 0; y < VBE_mode_info->height; y++)
		{
			putPixel(hexColor, x, y);
		}
	}
	cursorX = 0;
	cursorY = 0;
	return;
}

void clear()
{
	clear_bg(bg_color);
	return;
}

void drawRegister(int reg, uint32_t colour)
{
	char buff[256] = {0};
	uintToBase(reg, buff, 16);
	drawWord(colour, buff);
	newLine();
}

void moveCursorX(uint16_t posX)
{
	cursorX = posX;
}
void moveCursorY(uint16_t posY)
{
	cursorY = posY;
}
void drawWordAt(uint32_t hexColor, const char *str, uint32_t posX, uint32_t posY)
{
	cursorX = posX;
	cursorY = posY;
	drawWord(hexColor, str);
}
