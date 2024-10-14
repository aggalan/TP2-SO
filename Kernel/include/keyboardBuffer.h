#ifndef keyboardBuffer_h
#define keyboardBuffer_h
#include <stdint.h>
#define BUFF_SIZE 4096
typedef struct keyboard_buffer* buff_ptr; 

struct keyboard_buffer { 
    int bufferPos;
    int bufferLen;
    char buffer[BUFF_SIZE];
};

int getBufferPosition();            //Returns the current position of the buffer
void setPos(int newPosition);       //Set the buffer position to a new position 
char * getBufferAddress();          //Get the buffer address
char getCharAt(int position);       //Return the char located at buffer[position]
void consumeBufferAt(int position); //Erases the char located at position
void cleanBuffer();                 //cleans the buffer
int getBufferLen();                 //Returns the current size of the buffer
void incBufferLen(int n);           //increments the current size of the buffer

#endif