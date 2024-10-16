#include "include/keyboard_driver.h"
#include "../include/keyboard_buffer.h"
#include "../include/interrupts.h"
#include <stdio.h>
#include "include/scan_code.h"
#include "include/video_driver.h"
    static char keyMapRow = 0;
void keyboard_handler()
{
	uint16_t code = getKey();
	if(code < 0x80){    // Key pressed
    char * buff = getBufferAddress();  
   int buff_pos = getBufferPosition();
    if(code == LEFT_SHIFT || code == RIGHT_SHIFT){
      keyMapRow=1;
    }
    else if(keyMap[keyMapRow][code]!=0){
          buff[buff_pos]=keyMap[keyMapRow][code];
          incBufferLen(1);
          setPos(buff_pos);

    }
	} else {               // Key released
    code-=0x80;
    if(code == LEFT_SHIFT || code == RIGHT_SHIFT){  
      keyMapRow&=0xFE;
    }
	}

  return;
}

