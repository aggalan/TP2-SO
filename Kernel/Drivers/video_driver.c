// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "include/video_driver.h"
#include "include/fonts.h"
#include "../include/naive_console.h"
#include <naive_console.h>
#include <lib.h>
#include "../include/keyboard_buffer.h"
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

uint64_t default_f_color = 0xFFFFFF;
uint64_t default_b_color = 0x000000;

typedef struct vbe_mode_info_structure *VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr)0x0000000000005C00;

uint32_t cursor_x = 0;
uint32_t cursor_y = 0;
uint32_t size_x = FONT_SIZE;
uint32_t size_y = FONT_SIZE * 2;
uint32_t bg_color = 0x00000000;
// uint8_t buff[256*16];

uint16_t get_height()
{
	return VBE_mode_info->height;
}
uint16_t get_width()
{
	return VBE_mode_info->width;
}

uint16_t get_cursor_x()
{
	return cursor_x;
}

uint16_t get_cursor_y()
{
	return cursor_y;
}

void move_upwards()
{
	char *dest = (char *)(uintptr_t)VBE_mode_info->framebuffer;
	char *src = dest + VBE_mode_info->pitch * size_y * 8;
	uint64_t size = (VBE_mode_info->height - size_y * 8) * VBE_mode_info->pitch;
	memcpy(dest, src, size);
	draw_rectangle(0x00000000, 0, VBE_mode_info->height - size_y * 8, size_y * 8, VBE_mode_info->pitch);
	cursor_y -= 8 * size_y;
}

void put_pixel(uint32_t hex_color, uint32_t x, uint32_t y)
{
	uint8_t *framebuffer = (uint8_t *)(uintptr_t)VBE_mode_info->framebuffer;
	uint64_t offset = (x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
	framebuffer[offset] = (hex_color) & 0xFF;
	framebuffer[offset + 1] = (hex_color >> 8) & 0xFF;
	framebuffer[offset + 2] = (hex_color >> 16) & 0xFF;
}

void draw_rectangle(uint32_t color, uint32_t x, uint32_t y, uint32_t height, uint32_t width)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			put_pixel(color, x + i, y + j);
		}
	}
}

void newline()
{
	cursor_x = 0;
	cursor_y += size_y * 8;
}

void draw_square(uint32_t hex_color, uint32_t side_length, uint32_t x, uint32_t y)
{
	draw_rectangle(hex_color, x, y, side_length, side_length);
}
void draw_char(uint32_t hex_color, char character)
{
	if (character == '\t')
		return;

	if (cursor_x >= (uint16_t)VBE_mode_info->width)
	{
		cursor_y += size_y * 8;
		cursor_x = 0;
	}
	if (cursor_y >= (uint16_t)VBE_mode_info->height)
	{
		cursor_x = 0;
		move_upwards();
	}
	int x = cursor_x;
	int y = cursor_y;
	int current = x;
	int index = character - 33;
	if (character == ' ')
	{
		cursor_x += size_x * 8;
		return;
	}
	if (character == '\n')
	{
		newline();
		return;
	}
	if (character == '\b')
	{
		if (cursor_x == 0)
		{
			if (cursor_y == 0)
			{
				return;
			}
			else
			{
				cursor_y -= size_y * 8;
				cursor_x = (uint16_t)VBE_mode_info->width;
			}
		}
		cursor_x -= size_x * 8;
		draw_rectangle(0x00000000, cursor_x, cursor_y, size_y * 8, size_x * 8);
		return;
	}

	for (int i = 0; i < 32; i++)
	{
		if (i % 2 == 0 && i != 0)
		{
			current = x;
			y += size_x; // Salto a la siguiente fila de píxeles
		}
		font[i + index * 32] & (char)0x01 ? draw_square(hex_color, size_x, current, y) : 0;
		current += size_x;
		uint8_t aux = 0x02;

		for (int j = 0; j < 8; j++)
		{
			// Comprueba cada bit de la fuente y dibuja un píxel si está activo
			((uint8_t)font[i + (index * 32)] & (uint8_t)aux) >> j ? draw_square(hex_color, size_x, current, y) : 0;
			current += size_x; // Avanza a la siguiente posición horizontal
			aux <<= 1;		   // Desplaza el bit auxiliar hacia la izquierda
		}
	}
	cursor_x += size_x * 8;
}

void draw_word_white(char *string)
{
	draw_word(default_f_color, string);
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

	draw_word_white(buffer); // Call draw_word_white to print the string directly
}

void draw_number(int value)
{
	draw_number_color(value, default_f_color, default_b_color);
}

void draw_number_color(int value, uint64_t fcolor, uint64_t bcolor)
{
	char buff[256] = {0};
	int_to_str(value, buff, 10);
	draw_word(fcolor, buff);
}

void draw_word(uint32_t hex_color, const char *str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		draw_char(hex_color, str[i]);
	}
}
void draw_word_len(uint32_t hex_color, const char *str, int len)
{
	for (int i = 0; i < len; i++)
	{
		draw_char(hex_color, str[i]);
	}
}

void int_to_str(int value, char *str, int base)
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

char set_font_size(uint32_t size)
{
	if (size > 2 || size <= 0)
	{
		return 0;
	}
	size_x = size;
	size_y = size * 2;
	return 1;
}
void clear_bg(uint64_t hex_color)
{
	for (int x = 0; x < VBE_mode_info->width; x++)
	{
		for (int y = 0; y < VBE_mode_info->height; y++)
		{
			put_pixel(hex_color, x, y);
		}
	}
	cursor_x = 0;
	cursor_y = 0;
	return;
}

void clear()
{
	clear_bg(bg_color);
	return;
}

void draw_register(int reg, uint32_t colour)
{
	char buff[256] = {0};
	u_int_to_base(reg, buff, 16);
	draw_word(colour, buff);
	newline();
}

void move_cursor_x(uint16_t posX)
{
	cursor_x = posX;
}
void move_cursor_y(uint16_t posY)
{
	cursor_y = posY;
}
void draw_wordAt(uint32_t hex_color, const char *str, uint32_t posX, uint32_t posY)
{
	cursor_x = posX;
	cursor_y = posY;
	draw_word(hex_color, str);
}
