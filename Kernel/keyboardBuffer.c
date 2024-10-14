#include <keyboardBuffer.h>
static struct keyboard_buffer buff = { 0, 0, {0} };

static buff_ptr ptr = &buff;

void incBufferLen(int n){
    ptr->bufferLen +=n;
}

int getBufferPosition(){
return ptr->bufferPos;
}
void setPos(int newPosition){
    ptr->bufferPos= newPosition % BUFF_SIZE;
}
char * getBufferAddress(){
    return ptr->buffer;
}
char getCharAt(int position){
    return ptr->buffer[position % BUFF_SIZE];
}
 void consumeBufferAt(int pos){
     ptr->buffer[pos] = 0;
    incBufferLen(-1);
    setPos(pos + 1);
 }
void cleanBuffer(){
    char * bufferAux=ptr->buffer; 
    for(int i=0;i<ptr->bufferLen;i++){
        bufferAux[i]='\0';
    }
}
int getBufferLen(){
    return ptr->bufferLen;
}

