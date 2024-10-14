#include "include/Buffer.h"
#include "include/lib.h"
#include "include/usrSysCall.h"

void clearBuffer(char * buffer){
    for(int i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = 0;
    }
}

