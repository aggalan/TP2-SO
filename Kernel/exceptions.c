#include "exceptions.h" 
#include "registers.h"
#include "syscalls.h"
#include "videoDriver.h"
#include "keyboardBuffer.h"
#define ZERO_EXCEPTION_ID 0
 



void exceptionDispatcher(int exception, registerStruct * registers) {
    setCursorFlag(0);
    if (exception == ZERO_EXCEPTION_ID)
		zeroDivision();
	else {
		invalidOp();
	}
	printRegisters(registers);
	drawWordColor("Press a key to continue",WHITE, RED);
	char c;
	sys_read(&c, 1, 0);
    bufferClear();
    setCursorFlag(1);
}

 void zeroDivision() {
	register_snapshot_taken = 1;
	drawWordColor("ERROR: CANNOT DIVIDE BY ZERO", WHITE, RED);
	newline();
}

 void invalidOp(){
	register_snapshot_taken = 1;
	drawWordColor("ERROR: INVALID OPCODE", WHITE, RED);
	newline();
}

