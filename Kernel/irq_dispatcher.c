#include <time.h>
#include <stdint.h>
#include "Drivers/include/keyboard_driver.h"
#include "Drivers/include/video_driver.h"
#include "Drivers/include/sound_driver.h"
#include "include/libasm.h"
#include <syscalls.h>
#include <naive_console.h>
#include "include/interrupts.h"
#include "include/lib.h"
#include "include/register_handling.h"
#include "./memory_manager/include/memory_manager.h"
#include "include/process_manager.h"
#include "include/scheduler.h"

static void int_20();
static void int_21();
static uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
void irqDispatcher(uint64_t irq,uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	switch (irq) {
		case 0:
			int_20();
			break;
		case 1:
			int_21();
			break;
        case 0x80:
            int_80(rdi, rsi,  rdx, rcx, r8, r9);
            break;
        }
	return;
}

void int_20() {
	timer_handler();
}
void int_21() {
	keyboard_handler();
}
static uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
    switch(rdi) {
        case 1:
            sys_write(rsi, (char *)rdx, rcx,r8);
            return 0;
        case 2:
            return sys_read(rsi,(char *) rdx, rcx);
        case 3:
            clock((char *) rsi);
            return 0;
        case 4:
            if(getFlag() || rsi == 1){
                if(getFlag()==0){
                    drawWord(0x00FF0000,"You must take a screenshot first, press : and try again.\n");
                    return 0;
                }
                printRegisters(getRegisters(), 0x00ffffff);
            }

            return 0;
        case 5:
            clear();
            return 0;
        case 6:
            return ticks_elapsed();
        case 7:
           return (getHeight());
        case 8:
           return (getWidth());
        case 9:
            moveCursorX((uint16_t)rsi);
            return 0;
        case 10:
            moveCursorY((uint16_t)rsi);
            return 0;
        case 11:
            drawRectangle(rsi, rdx, rcx, r8, r9);
            return 0;
        case 12:
            sleepms(rsi);
            return 0;
        case 13:
            return (int) setFontSize(rsi);
        case 14:
            beep();
            return 0;
        case 15:
            mm_status();
            return 0;
        case 16:
            annihilate();
            return 0;
        case 17: //rellenar
            return 0;
        case 18: //rellenar
            return 0;
        case 19:
            return block_process((pid_t)rsi);
        case 20:
            return unblock_process((pid_t)rsi);
        case 21:
            return kill_process_pid((pid_t)rsi);
        case 22:
            nice();
            return 0;
        case 23:
            //return wait_pid((pid_t)rsi);
        case 24:
            change_priority((pid_t)rsi,(int)rdx);
            return 0;
        case 25:
            return (uint64_t) mm_malloc(rsi);
        case 26:
            mm_free((void *)rsi);
            return 0;
        case 27:
            return create_process(rsi,rdx,rcx,(char **)r8);
        case 28:
            print_processes();
            return 0;
            //return processes_info();
        case 29:
            return get_current_pid();
        case 30:
            return kill_process();
        default:
            return 0;
    }
}


