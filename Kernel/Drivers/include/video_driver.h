
#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>
#define FONT_SIZE 1
#define FONT_BIG 2

// draws a character in the current cursor position, and sets the cursor in the following character position
void draw_char(uint32_t hexColor, char character);

// draws a rectangle of color passed as parameter, which starts in (x,y) and has the width and height received
void draw_rectangle(uint32_t color, uint32_t x, uint32_t y, uint32_t height, uint32_t width);

// paints the pixel in the (x,y) position of the screen with the color hexColor (format 0x00RRGGBB)
void put_pixel(uint32_t hexColor, uint32_t x, uint32_t y);

// draws a string starting in the current cursor position, and sets the cursor in the following character position
void draw_word(uint32_t hexColor, const char *str);

// draws a string starting in the current cursor position, and sets the cursor in the following character position, with max length len
void draw_word_len(uint32_t hexColor, const char *str, int len);

// returns 1 if the size was changed, 0 if it was not. Only accepts 2 sizes, 1 or 2. It changes the character size.
char set_font_size(uint32_t size);

// empties the screen
void clear();

// moves the cursor to a new line
void newline();

// returns the screen width
uint16_t get_width();

// returns the screen height
uint16_t get_height();

// draws a word at the position passed by parameter, and sets the cursor in the following character position
void draw_word_at(uint32_t hexColor, const char *str, uint32_t posX, uint32_t posY);

// sets cursorX at the position received
void move_cursor_x(uint16_t posX);

// sets cursorY at the position received
void move_cursor_y(uint16_t posY);

// draws a register with its format and jumps to a newline
void draw_register(int reg, uint32_t colour);

void draw_word_white(char *string);
void address_to_string(void *ptr, char * buffer);
void draw_number(int value);
void draw_number_color(int value, uint64_t fcolor, uint64_t bcolor);
void int_to_str(int value, char *str, int base);

#endif