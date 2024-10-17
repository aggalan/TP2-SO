#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>
#define FONT_SIZE 1
#define FONT_BIG 2

// draws a character in the current cursor position, and sets the cursor in the following character position
void drawChar(uint32_t hexColor, char character);

// draws a rectangle of color passed as parameter, which starts in (x,y) and has the width and height received
void drawRectangle(uint32_t color, uint32_t x, uint32_t y, uint32_t height, uint32_t width);

// paints the pixel in the (x,y) position of the screen with the color hexColor (format 0x00RRGGBB)
void putPixel(uint32_t hexColor, uint32_t x, uint32_t y);

// draws a string starting in the current cursor position, and sets the cursor in the following character position
void drawWord(uint32_t hexColor,const char * str);

// draws a string starting in the current cursor position, and sets the cursor in the following character position, with max length len
void drawWordLen(uint32_t hexColor,const char * str,int len);

// returns 1 if the size was changed, 0 if it was not. Only accepts 2 sizes, 1 or 2. It changes the character size.
char setFontSize(uint32_t size);

//empties the screen
void clear();

// moves the cursor to a new line
void newLine();

//returns the screen width
uint16_t getWidth();

//returns the screen height
uint16_t getHeight();

//draws a word at the position passed by parameter, and sets the cursor in the following character position
void drawWordAt(uint32_t hexColor,const char * str,uint32_t posX,uint32_t posY);

// sets cursorX at the position received
void moveCursorX(uint16_t posX);

// sets cursorY at the position received
void moveCursorY(uint16_t posY);

//draws a register with its format and jumps to a newline
void drawRegister(int reg,uint32_t colour);

void drawWord1(char * string);
void address_to_string(void *ptr);
void drawNumber(int value);
void drawNumberColor(int value, uint64_t fcolor, uint64_t bcolor);
void intToStr(int value, char* str, int base);

#endif