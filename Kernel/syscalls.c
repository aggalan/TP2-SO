#include "Drivers/include/video_driver.h"
#include "include/syscalls.h"
#include "include/keyboard_buffer.h"
#include "include/libasm.h"
#include <naive_console.h>
#include <stdio.h>
#define MIN(x,y) x < y ? x : y

void sys_write(int descriptor, const char * str, int len,uint32_t hexColor){
    switch(descriptor){
        case STDOUT:
            drawWordLen(hexColor, str, len);
            return;
        case ERROUT:
            drawWordLen(0x00ff0000, str, len);
        default:
            drawWord(0x00ff0000, "no such descriptor");
        return;
    }

}

int sys_read(int descriptor, char * save, int len){
    
    if(descriptor != STDIN){
        drawWord(0x00ff0000, "no such descriptor");
    }
   
    int n=getBufferPosition();
 
    if(getCharAt(n)==0){
        *save=0;
        return 0;
    }
    
    int length = MIN(len, getBufferLen());

    for(int i=0; i< length; i++){
        n=getBufferPosition();
        save[i] = getCharAt(n);
        consumeBufferAt(n);
    }
    return length;
}

void twoChars(char * first,int j, char * app){
    for(int i = 0; i < 2;i++){
        first[j+i] = app[i];
    }
}
