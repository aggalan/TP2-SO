#ifndef VIDEO_DRIVER_H 
#define VIDEO_DRIVER_H 
#include <stdint.h>
void putPixel(uint64_t hexColor, uint64_t x, uint64_t y);
void drawCharTransparent(char c, uint64_t fcolor);
void drawCharColor(char c, uint64_t fcolor, uint64_t bcolor);
void drawWordColor(char * string, uint64_t fColor, uint64_t bColor);
void drawWord(char * string);
void fill(uint64_t x, uint64_t y, uint64_t color);
void fillPos(int size, uint64_t color);
void backspace();
void newline();
void clear();
void tab();
void drawWordLen(char * string, int len);
void drawWordColorLen(char * string, uint64_t fcolor, uint64_t bcolor, int len);
void invalidFD();
void drawRegister(int valueReg);
void clearColor(uint64_t color);
void move_screen();
void sizeUp();
void sizeDown();
void fontSize();
void intToStr(int value, char* str, int base);
void drawChar(char c);
void drawWordAt(char * string, uint32_t x, uint32_t y);
void drawWordColorAt(char * string, uint64_t color, uint32_t x, uint32_t y);
void putSquare(uint64_t x, uint64_t y, uint32_t size, uint64_t color);
void cursorOff();
void cursorOn();
uint64_t pixelColorAt(uint32_t x, uint32_t y);
void deleteLine();
void moveLeft();
void moveRight();
void moveScreenRight();
void moveScreen();
void setCursorFlag(int i);
void printImage();

#define RED 0xFF0000
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define MARGIN 10
#define DEFAULT_FONT_SIZE 1
#endif