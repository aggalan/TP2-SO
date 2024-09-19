#include <naiveConsole.h>
#include <syscalls.h>
#include <keyboardBuffer.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <interrupts.h>

static int shift = 0;

void setShift(int i) {
    shift = i;
}

int getShiftVal() {
    return shift;
}

void sys_write(char * buff, int len, int fd){

    if(fd == STDOUT){
        drawWordLen(buff, len);
        
    } else if(fd == STDERR){
        drawWordColorLen(buff, WHITE, RED, len);
    }else
    invalidFD();

}

void sys_read(char * c, int len, int fd){

    int i;

    if (fd == STDIN) {
        char aux = 0;
        for (i = 0; i < len;) {
            _hlt();
            aux = getBuffAtCurrent();
            if (aux > 0 && aux <= 256) {
                if ( shift != 0 && ScanCodes[(int)aux].make >= 'a' && ScanCodes[(int)aux].make <= 'z') {
                    c[i++] = ScanCodes[(int)aux].make - 32;
                } else {
                    c[i++] = ScanCodes[(int)aux].make;
                }


                consume();
            }

        }
        return;
    }
    else{
        invalidFD();
    }
}

