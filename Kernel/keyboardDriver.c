#include <lib.h>
#include <naiveConsole.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <keyboardBuffer.h>
#include "registers.h"
#include "interrupts.h"



void keyboardHandler(){

	uint16_t key = get_key();

    if (key == 0) {
        return;
    }

    if (getShiftVal() == 0 && key == 0x2A) {
        setShift(1);
        return;
    } else if (getShiftVal() == 1 && key == 0xAA) {
        setShift(0);
        return;
    }

    if (getShiftVal() == 0 && key == 0x36) {
        setShift(2);
        return;
    } else if (getShiftVal() == 2 && key == 0xB6) {
        setShift(0);
        return;
    }


    if(ScanCodes[key].make == ';'){
        saveRegState();
        register_snapshot_taken = 1;
        return;
    }

    if (key != 0x2A && key != 0x36 ) {
        bufferAppend(key);
    }
    



}
