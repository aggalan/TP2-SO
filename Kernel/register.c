#include "registers.h"
#include "videoDriver.h"

int register_snapshot_taken = 0;
void printRegisters(registerStruct * dumpedRegisters){
    if(register_snapshot_taken == 0){
        drawWordColor("ERROR - No register snapshot detected\nPress ; to take one\n", WHITE, RED);
        return;
    }
    drawWord("RIP = 0x");
	drawRegister(dumpedRegisters->rip);
	drawWord("RAX = 0x");
	drawRegister(dumpedRegisters->rax);
	drawWord("RBX = 0x");
	drawRegister(dumpedRegisters->rbx);
	drawWord("RCX = 0x");
	drawRegister(dumpedRegisters->rcx);
	drawWord("RDX = 0x");
	drawRegister(dumpedRegisters->rdx);
	drawWord("RSP = 0x");
	drawRegister(dumpedRegisters->rsp);
	drawWord("RBP = 0x");
	drawRegister(dumpedRegisters->rbp);
	drawWord("RSI = 0x");
	drawRegister(dumpedRegisters->rsi);
	drawWord("RDI = 0x");
	drawRegister(dumpedRegisters->rdi);
	drawWord("R8  = 0x");
	drawRegister(dumpedRegisters->r8);
	drawWord("R9  = 0x");
	drawRegister(dumpedRegisters->r9);
	drawWord("R10 = 0x");
	drawRegister(dumpedRegisters->r10);
	drawWord("R11 = 0x");
	drawRegister(dumpedRegisters->r11);
	drawWord("R12 = 0x");
	drawRegister(dumpedRegisters->r12);
	drawWord("R13 = 0x");
	drawRegister(dumpedRegisters->r13);
	drawWord("R14 = 0x");
	drawRegister(dumpedRegisters->r14);
	drawWord("R15 = 0x");
	drawRegister(dumpedRegisters->r15);

}