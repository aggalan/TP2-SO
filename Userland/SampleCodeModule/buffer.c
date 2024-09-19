#include "buffer.h"
#include <utils.h>

void clearBuff(char * buff){
    for(int i = 0; i < BUFFERLIMIT; i++){
        buff[i] = '\0';
    }
    return;
}