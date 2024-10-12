#include <time.h>
#include <stdint.h>
#include <naiveConsole.h>
#include "videoDriver.h"
#include "registers.h"
#include "interrupts.h"
#include "soundDriver.h"
#include "keyboardBuffer.h"
#include "syscalls.h"
#include "keyboardDriver.h"
#include <stdbool.h>
#include "test_util.h"
#include "memoryManager.h"
#include "processManager.h"
#include "test_processes.h"
#include <stddef.h>

static uint64_t int_80(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10);
typedef void (*InterruptHandler)(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10);

void irqDispatcher(uint64_t irq, uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10) {
	switch(irq){
		case 0:
			timer_handler();
			if(ticks_elapsed() % 21 <= 10){
				cursorOn();
			}else{
				cursorOff();
			}
			break;
		case 1:
			keyboardHandler();
			break;
		case 96:
			int_80(rax, rdi, rsi, rdx, r10);
			break;
	}
	return;

}

uint64_t int_80(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10){
	switch(rax){
		case 1: 
				sys_write((char *)rdi, rsi, rdx);
				break;
		case 2:
				sys_read((char *)rdi, rsi, rdx);
				break;
		case 3: 
				TimeClock((char *)rdi); 
				break;
		case 4:
				printRegistersASM(); 
				break;
		case 5: 
				sizeUp();
				break;
		case 6: 
				sizeDown();
				break;
		case 7: 
				clear();
				break;
		case 8:
				fontSize();
				break;
		case 9:
				beep(rdi, rsi);
				break;
		case 10:
				clearColor(rdi);
				break;
		case 11:
				drawWordColorAt((char *)rdi, rsi, rdx, r10);
				break;
		case 12:
				putSquare(rdi, rsi, rdx, r10);
				break;
		case 13:
				sleepms(rdi);
				break;
		case 14:
				sleeps(rdi);
				break;
		case 15:
				pixelColorAt(rdi, rsi);
				break;
		case 16:
				return getPos();
				break;
		case 17:
				return getBuffCharAt(rdi);
				break;
		case 18:
				bufferClearAll();
				break;
        case 19:
                moveScreenRight();
                break;
		case 20:
                setCursorFlag(1);
				break;
		case 21:
                setCursorFlag(0);
				break;
        case 22:
                printImage();
                break;
		case 23:
				char * argv[] = {"266240"};
				test_mm(1, argv);
				break;
		case 24:
				mm_status();
				break;
		case 25:
				kill_process();
				break;
		case 26:
				char * argv2[] = {"20"};
				test_processes(1, argv2);
				break;
		default:
				return 0;
				
	}
	return 0;
}
