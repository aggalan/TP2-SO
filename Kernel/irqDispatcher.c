#include <time.h>
#include <stdint.h>
#include "Drivers/include/keyboardDriver.h"
#include "Drivers/include/videoDriver.h"
#include "Drivers/include/SoundDriver.h"
#include "include/libasm.h"
#include <syscalls.h>
#include <naiveConsole.h>
#include "include/interrupts.h"
#include "include/lib.h"
#include "include/registerHandling.h"
#include "include/memoryManager.h"
#include "include/processManager.h"
#include "include/test_util.h"
#include "include/test_processes.h"
#include "include/scheduler.h"

static void int_20();
static void int_21();
static int int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
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
static int int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
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
            //la idea faltaria que se prenda al pedir registros
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
            char ** argv=mm_malloc(2*sizeof(char));
            argv[1]="266240";
            argv[0]="mem test";
            create_process(test_mm,1,1,argv);
            return 0;
        case 17:
            char ** argv2=mm_malloc(2*sizeof(char));
            argv2[1]="10";
            argv2[0]="process test";
            create_process(test_processes,1,1,argv2);
            return 0;
        case 18:
            char ** argv3=mm_malloc(sizeof(char));
            argv3[0]="prio test";
            create_process(test_prio,1,1,argv3);
        case 19:
            return block_process((pid_t)rsi);
        case 20:
            return unblock_process((pid_t)rsi);
        case 21:
            return kill_process_pid((pid_t)rsi);
        case 22:
            yield();
            return 0;
        case 23:
            //return wait_pid((pid_t)rsi);
        case 24:
            my_nice((pid_t)rsi,(int)rdx);
            return 0;
        case 25:
            //return (uint64_t) mm_malloc(rsi);
        case 26:
            mm_free((void *)rsi);
            return 0;
        case 27:
            return create_process((void *)rsi,rdx,rcx,(char **)r8);
        case 28:
            //return processes_info();
        case 29:
            //return get_current_pid();
        case 30:
            return kill_process();
        default:
            return 0;
    }
}


