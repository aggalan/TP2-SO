
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6
#include "Drivers/include/video_driver.h"
#include "include/register_handling.h"
#include "include/syscalls.h"
#include "include/interrupts.h"
static void zero_division();
static void invalid_opcode();

void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID){
		zero_division();
	}else{
		invalid_opcode();
		}
    printRegisters(getRegisters(), 0x00ff0000);
    drawWord(0x00ff0000,"Press any key to continue");
    while(1) {
        char c;
        _hlt();
        sys_read(STDIN, &c, 1);
        if (c != 0) {
            clear();
            return;
        }
    }

}

static void zero_division() {
	drawWord(0x00FF0000, "Cannot divide by 0");
	newLine();
    return;
}

static void invalid_opcode(){

	drawWord(0x00FF0000, "Error:Invalid opcode");
	newLine();

}