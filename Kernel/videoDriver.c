#include <videoDriver.h>
#include <stdint.h>
#include <keyboardBuffer.h>
#include <font.h>
#include "naiveConsole.h"


struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
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
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

uint32_t posX = MARGIN;
uint32_t posY = MARGIN;

int size = DEFAULT_FONT_SIZE;

static int cursor_flag = 1;

int cursor_pos = 0;

uint64_t defaultFColor = WHITE;
uint64_t defaultBColor = BLACK;

void putPixel(uint64_t hexColor, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}

void drawCharTransparent(char c, uint64_t fcolor) {
    drawCharColor(c, fcolor, BLACK);
}

void clear() {
    int i, j;
    for (i = 0; i < VBE_mode_info->height; i++) {
        for (j = 0; j < VBE_mode_info->width; j++) {
            putPixel(0,j, i);
        }
    }
    posX = MARGIN;
    posY = MARGIN;
}

void drawCharColor(char c, uint64_t fcolor, uint64_t bcolor) {
    cursorOff();
    if (posX >= VBE_mode_info->width-(16*size)-MARGIN && posY >= VBE_mode_info->height-(32*size)-MARGIN) {
        moveScreen();
    }

    if (c == ' ') {
        fcolor = bcolor;
    }
    if (cursor_pos != 0) {
        moveScreenRight();
    }


    int cx, cy;
    int pos = c - 33;

    for (cy = 0; cy < 16; cy++) {
        int maskCheck = 0x01;
        int maskCheck2 = 0x01;
        for (cx = 0; cx < 10; cx++) {
            if (((font[(pos*32) + (2*cy)] & (maskCheck << cx)) != 0) && cx < 8) {

                for (int z = 0; z < size; z++) {
                    for (int w = 0; w < size; w++) {
                        putPixel(fcolor, cx*size + posX + z, cy*size + posY + w);
                    }
                }

            }
            else {

                for (int z = 0; z < size; z++) {
                    for (int w = 0; w < size; w++) {
                        putPixel(bcolor, cx*size + posX + z, cy*size + posY + w);
                    }
                }

            }
        }
    }
    if (posX >= VBE_mode_info->width-MARGIN-(16*size)) {
        posY += 16*size;
        posX = MARGIN;
        return;
    }

    posX += 10*size;

}

void putSquare(uint64_t x, uint64_t y, uint32_t size, uint64_t color) {
    for (uint32_t i = 0; i < size; i++) {
        for (uint32_t j = 0; j < size; j++) {
            putPixel(color, x + j, y + i);
        }
    }
}


void character(char character, uint64_t fcolor, uint64_t bcolor){
    if(character == '\b'){
        backspace();
        return;
    }
    if(character == '\n'){
        newline();
        return;
    }
    if(character == '\t'){
        tab();
        return;
    }
    if (character == 0x7C) {
        moveLeft();
        return;
    }
    if (character == 0x7D) {
        moveRight();
        return;
    }
    if (character == 0x7F || character == 0x7B) {
        cursor_pos = 0;
        return;
    }

    drawCharColor(character, fcolor, bcolor);
    return;
}

void drawWord(char * string) {
    drawWordColor(string, defaultFColor, defaultBColor);
}

void drawWordColorAt(char * string, uint64_t color, uint32_t x, uint32_t y){
    uint32_t px = posX;
    uint32_t py = posY;
    posX = x;
    posY = y;
    drawWordColor(string, color, defaultBColor);
    posX = px;
    posY = py;
}

void drawWordAt(char * string, uint32_t x, uint32_t y){ 
    drawWordColorAt(string, defaultFColor, x, y);
}


void drawWordColor(char * string, uint64_t fColor, uint64_t bColor){
    for(int i = 0; string[i] != 0; i++){
        character(string[i], fColor, bColor);
    }

}

void transparent_space() {
    if (posX >= VBE_mode_info->width-(10*size)) {
        posY += 16*size;
        posX = MARGIN + 10*size;
    } else {
        posX += 10*size;
    }
}

void drawChar(char c){
    character(c, defaultFColor, defaultBColor);
}

void tab(){
    drawWord("  ");
 }

void backspace() {
    if (posX == MARGIN && posY == MARGIN) {
        return;
    }
    cursorOff();
    if (posX <= MARGIN) {
        posX = VBE_mode_info->width-MARGIN-4;
        posY -= 16*size;
    }
    int i, j;
    for (i = 0; i < 16*size; i++) {
        for (j = 0; j < 10*size; j++) {
            putPixel(0, posX - j - 1, posY + i);
        }
    }

    if (cursor_pos != 0) {
        backspaceMove();
    }
    posX -= 10*size;
}

void backspaceMove() {
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;

    for (int i = posY; i < VBE_mode_info->height && i < posY + 3*16*size; i++) {
        for (int j = MARGIN; j < VBE_mode_info->width-MARGIN; j++) {

            if (i <= posY+16*size && j<posX) {
                continue;
            }

            uint64_t offset = (j * ((VBE_mode_info->bpp)/8)) + (i * VBE_mode_info->pitch);

            uint8_t blue = framebuffer[offset];
            uint8_t green = framebuffer[offset + 1];
            uint8_t red = framebuffer[offset + 2];

            uint64_t hexColor = (red << 16) | (green << 8) | blue;

            if (j <= MARGIN + 10*size - 1) {
                putPixel(hexColor, VBE_mode_info->width-MARGIN-10*size + (j - MARGIN)-4, i-16*size);
            } else{
                putPixel(hexColor, j - (10*size), i);
            }

        }
    }
}


void newline() {
    cursorOff();
    cursor_pos = 0;
    if (posY >= VBE_mode_info->height-(32*size)-MARGIN) {
        moveScreen();
    }
    posX = MARGIN;
    posY += 16*size;
}

void drawWordColorLen(char * string, uint64_t fcolor, uint64_t bcolor, int len){
    for(int i = 0; i < len && string[i] != 0; i++){
        character(string[i], fcolor, bcolor );
    }
}

void drawWordLen(char * string, int len){
        for(int i = 0; i < len && string[i] != 0; i++){
        character(string[i], defaultFColor, defaultBColor );
    }
}

void invalidFD(){
    drawWordColor("Invalid File Descriptor entered", WHITE, RED);
}

void drawRegister(int valueReg){
    char buff[256] = {0};
    uintToBase(valueReg, buff, 16);
    drawWordColor(buff, WHITE, BLACK);
    newline();
}


void clearColor(uint64_t color){
        int i, j;
    for (i = 0; i < VBE_mode_info->height; i++) {
        for (j = 0; j < VBE_mode_info->width; j++) {
            putPixel(color,j, i);
        }
    }
    posX = MARGIN;
    posY = MARGIN;

}

void moveScreen() {
    posY -= 16*size;

    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;

    for (int i = MARGIN + (16*size); i < VBE_mode_info->height + 16; i++) {
        for (int j = MARGIN; j < VBE_mode_info->width; j++) {

            uint64_t offset = (j * ((VBE_mode_info->bpp)/8)) + (i * VBE_mode_info->pitch);

            uint8_t blue = framebuffer[offset];
            uint8_t green = framebuffer[offset + 1];
            uint8_t red = framebuffer[offset + 2];

            uint64_t hexColor = (red << 16) | (green << 8) | blue;

            putPixel(hexColor, j, i- (16*size) - size );


        }
    }
}

void sizeUp(){
    if(size == 2){
        return;
    }else{
        size++;
        clear();
    }

    return;
}

void sizeDown(){
    if(size == 1){
        return;
    }else{
        size--;
        clear();
    }
    return;
}
void fontSize() {
    char sizeStr[12];
    intToStr(size, sizeStr, 10);

    drawWordColor("Font size is ", WHITE, BLACK);
    drawWord(sizeStr);
    drawChar('\n');
    drawWordColor("Maximum font size is 2, minimum font size is 1\n", WHITE, BLACK);
}

void intToStr(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789"[tmp_value - value * base];
    } while (value);

    *ptr-- = '\0';

    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

void cursorOff() {
    if (cursor_flag == 0) {
        return;
    }
    for (int i = 0; i < 10; i++) {
        for (int z = 0; z < size; z++) {
            putPixel(0x00000000, i*size + posX + z, posY + 16*size - 1);
        }
    };
}

void cursorOn() {
    if (cursor_flag == 0) {
        return;
    }
    for (int i = 0; i < 10; i++) {
        for (int z = 0; z < size; z++) {
            putPixel(0x00FFFFFF, i*size + posX + z, posY + 16*size - 1);
        }
    }
}

void moveLeft() {
    cursorOff();
    if (posX <= MARGIN) {
        posX = VBE_mode_info->width - MARGIN-4;
        posY -= 16 * size;
    }
    cursor_pos--;
    posX -= 10*size;
}

void moveRight() {
    cursorOff();
    if (posX >= VBE_mode_info->width - MARGIN-4 -(10*size)) {
        posX = MARGIN;
        posY += 16 * size;
        return;
    }
    cursor_pos++;
    posX += 10*size;
}

void moveScreenRight() {
    cursorOff();
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    for (int i = (VBE_mode_info->height >posY + 3*16*size)? posY + 3*16*size:VBE_mode_info->height; i > posY; i--) {
        for (int j = VBE_mode_info->width-MARGIN-5; j >= MARGIN; j--) {

            if (i <= posY+16*size && j<posX) {
                continue;
            }

            uint64_t offset = (j * ((VBE_mode_info->bpp)/8)) + (i * VBE_mode_info->pitch);

            uint8_t blue = framebuffer[offset];
            uint8_t green = framebuffer[offset + 1];
            uint8_t red = framebuffer[offset + 2];

            uint64_t hexColor = (red << 16) | (green << 8) | blue;

            if (j >= VBE_mode_info->width - MARGIN - 4 - (size*10)) {
                putPixel(hexColor,MARGIN + (j - (VBE_mode_info->width - MARGIN - 4-(size*10))), i+16*size);
            } else{
                putPixel(hexColor, j + (10*size), i);
            }

        }
    }
}

uint64_t pixelColorAt(uint32_t x, uint32_t y) {

    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);

    uint8_t blue = framebuffer[offset];
    uint8_t green = framebuffer[offset + 1];
    uint8_t red = framebuffer[offset + 2];

    uint64_t hexColor = (red << 16) | (green << 8) | blue;

    return hexColor;
}

void setCursorFlag(int i) {
    cursor_flag = i;
}

void printImage() {
    int x = VBE_mode_info->width - NOSOTROS_WIDTH;
    int y = 0;
    for (int i =0; i < NOSOTROS_HEIGHT * NOSOTROS_WIDTH;i++) {
        putPixel(NOSOTROS[i], x++, y);
        if (x == NOSOTROS_WIDTH + VBE_mode_info->width - NOSOTROS_WIDTH) {
            x = VBE_mode_info->width - NOSOTROS_WIDTH;
            y++;
        }
    }
}

