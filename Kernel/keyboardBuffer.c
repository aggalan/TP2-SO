#include <stdint.h>
#include <keyboardBuffer.h>
#include <videoDriver.h>
#include "keyboardBuffer.h"
#include "keyboardDriver.h"

static struct kbuff buff = {0, 0, {'\0'}};
static buffer_ptr ptr = &buff;

int buffer_pos = 0;

void bufferAppend(char c) {
    if (ptr->pos < BUFFER_LIMIT-1) {
        ptr->buffer[ptr->pos] = c;
        ptr->pos += 1;
        ptr->buffer[ptr->pos] = 0;
    }
    else{
        ptr->pos = 0;
        ptr->buffer[ptr->pos] = 0;
    }
}

int bufferLen(){
    return ptr->len;
}

void setPos(int newPos){
    ptr->pos = newPos;
}

void bufferClear() {
    ptr->len = 0;
}

void bufferClearAll(){
    ptr->pos = 0;
    ptr->len = 0;
    for (int i = 0; i < BUFFER_LIMIT; i++) {
        ptr->buffer[i] = 0;
    }

}

int getPos() {
    return ptr->pos;
}

uint16_t * getBuff() {
    return ptr->buffer;
}

char getBuffAtCurrent() {
    if (ptr->pos != 0) {
        return ptr->buffer[buff.pos - 1];
    }
    return ptr->buffer[BUFFER_LIMIT-1];
}

uint16_t getBuffCharAt(int pos){
    return ptr->buffer[pos];
}

void consume() {
    if (ptr->pos < BUFFER_LIMIT-1) {
        ptr->pos += 1;
    } else {
        ptr->pos = 0;
    }
}
