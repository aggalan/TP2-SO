#ifndef _KEYBOARD_BUFFER_
#define _KEYBOARD_BUFFER_
#include <stdint.h>

#define BUFFER_LIMIT 256


struct kbuff {
    int pos;
    int len;
    uint16_t buffer[BUFFER_LIMIT];
};

typedef struct kbuff * buffer_ptr;

void bufferAppend(char c);
void bufferClear();
void bufferBackspace();
int getPos();
uint16_t * getBuff();
char getBuffAtCurrent();
void consumeChar();
int bufferLen();
void setPos(int newPos);
uint16_t getBuffCharAt(int pos);
void bufferClearAll();
void consume();
#endif