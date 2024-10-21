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

static void irq_write(uint64_t rsi, char *rdx, uint64_t rcx, uint64_t r8);
static void irq_read(uint64_t rsi, char *rdx, uint64_t rcx);
static void irq_clock(char *rsi);
static int irq_print_registers(uint32_t rsi);
static int irq_clear();
static int irq_ticks_elapsed();
static uint16_t irq_getHeight();
static uint16_t irq_getWidth();
static int irq_moveCursorX(uint16_t rsi);
static int irq_moveCursorY(uint16_t rsi);
static int irq_drawRectangle(uint32_t rsi, uint32_t rdx, uint32_t rcx, uint32_t r8, uint32_t r9);
static int irq_sleepms(uint32_t rsi);
static int irq_setFontSize(uint32_t rsi);
static int irq_beep();
static int irq_mm_status();
static int case_17();
static int case_18();
static int irq_annihilate();
static pid_t irq_block_process(pid_t rsi);
static pid_t irq_unblock_process(pid_t rsi);
static pid_t irq_kill_process_pid(pid_t rsi);
static int case_22();
static pid_t irq_wait_pid(pid_t rsi);
static int irq_change_priority(pid_t rsi, uint8_t rdx);
static uint64_t irq_mm_malloc(uint32_t rsi);
static int irq_mm_free(void *rsi);
static pid_t irq_create_process(uint64_t rsi, uint8_t rdx, uint64_t rcx, char **r8);
static int irq_print_processes();
static pid_t irq_get_current_pid();
static pid_t irq_kill_process();


typedef uint64_t (*syscall_func_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

syscall_func_t syscalls[] = {
    [1] = (syscall_func_t) irq_write,
    [2] = (syscall_func_t) irq_read,
    [3] = (syscall_func_t) irq_clock,
    [4] = (syscall_func_t) irq_print_registers,
    [5] = (syscall_func_t) irq_clear,
    [6] = (syscall_func_t) irq_ticks_elapsed,
    [7] = (syscall_func_t) irq_getHeight,
    [8] = (syscall_func_t) irq_getWidth,
    [9] = (syscall_func_t) irq_moveCursorX,
    [10] = (syscall_func_t) irq_moveCursorY,
    [11] = (syscall_func_t) irq_drawRectangle,
    [12] = (syscall_func_t) irq_sleepms,
    [13] = (syscall_func_t) irq_setFontSize,
    [14] = (syscall_func_t) irq_beep,
    [15] = (syscall_func_t) irq_mm_status,
    [16] = (syscall_func_t) irq_annihilate,
    [17] = (syscall_func_t) case_17,
    [18] = (syscall_func_t) case_18,
    [19] = (syscall_func_t) irq_block_process,
    [20] = (syscall_func_t) irq_unblock_process,
    [21] = (syscall_func_t) irq_kill_process_pid,
    [22] = (syscall_func_t) case_22,
    [23] = (syscall_func_t) irq_wait_pid,
    [24] = (syscall_func_t) irq_change_priority,
    [25] = (syscall_func_t) irq_mm_malloc,
    [26] = (syscall_func_t) irq_mm_free,
    [27] = (syscall_func_t) irq_create_process,
    [28] = (syscall_func_t) irq_print_processes,
    [29] = (syscall_func_t) irq_get_current_pid,
    [30] = (syscall_func_t) irq_kill_process,
};

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

uint64_t int_80(uint64_t syscall_number, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
    if (syscall_number < sizeof(syscalls) / sizeof(syscalls[0]) && syscalls[syscall_number] != NULL) {
        return syscalls[syscall_number](rsi, rdx, rcx, r8, r9);
    }
    return -1; // Invalid syscall number
}


static void irq_write(uint64_t rsi, char *rdx, uint64_t rcx, uint64_t r8) {
    sys_write(rsi, rdx, rcx, r8);
}

static void irq_read(uint64_t rsi, char *rdx, uint64_t rcx) {
    sys_read(rsi, rdx, rcx);
}

static void irq_clock(char *rsi) {
    clock(rsi);
}

static int irq_print_registers(uint32_t rsi) {
    if(getFlag() || rsi == 1){
        if(getFlag()==0){
            drawWord(0x00FF0000,"You must take a screenshot first, press : and try again.\n");
            return 0;
        }
        printRegisters(getRegisters(), 0x00ffffff);
    }
    printRegisters(getRegisters(), rsi);
    return 0;
}

static int irq_clear() {
    clear();
    return 0;
}

static int irq_ticks_elapsed() {
    return ticks_elapsed();
}

static uint16_t irq_getHeight() {
    return getHeight();
}

static uint16_t irq_getWidth() {
    return getWidth();
}

static int irq_moveCursorX(uint16_t rsi) {
    moveCursorX(rsi);
    return 0;
}

static int irq_moveCursorY(uint16_t rsi) {
    moveCursorY(rsi);
    return 0;
}

static int irq_drawRectangle(uint32_t rsi, uint32_t rdx, uint32_t rcx, uint32_t r8, uint32_t r9) {
    drawRectangle(rsi, rdx, rcx, r8, r9);
    return 0;
}

static int irq_sleepms(uint32_t rsi) {
    sleepms(rsi);
    return 0;
}

static int irq_setFontSize(uint32_t rsi) {
    return (int) setFontSize(rsi);
}

static int irq_beep() {
    beep();
    return 0;
}

static int irq_mm_status() {
    mm_status();
    return 0;
}

static int case_17() {
    return 0;
}

static int case_18() {
    return 0;
}

static int irq_annihilate() {
    annihilate();
    return 0;
}

static pid_t irq_block_process(pid_t rsi) {
    return block_process(rsi);
}

static pid_t irq_unblock_process(pid_t rsi) {
    return unblock_process(rsi);
}

static pid_t irq_kill_process_pid(pid_t rsi) {
    return kill_process_pid(rsi);
}

static int case_22() {
    return 0;
}

static pid_t irq_wait_pid(pid_t rsi) {
    return wait_pid(rsi);
}

static int irq_change_priority(pid_t rsi, uint8_t rdx) {
    return change_priority(rsi, rdx);
}

static uint64_t irq_mm_malloc(uint32_t rsi) {
    return (uint64_t) mm_malloc(rsi);
}

static int irq_mm_free(void *rsi) {
    mm_free(rsi);
    return 0;
}

static pid_t irq_create_process(uint64_t rsi, uint8_t rdx, uint64_t rcx, char **r8) {
    return create_process(rsi, rdx, rcx, r8);
}

static int irq_print_processes() {
    print_processes();
    return 0;
}

static pid_t irq_get_current_pid() {
    return get_current_pid();
}

static pid_t irq_kill_process() {
    return kill_process();
}






// static uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9){
//     switch(rdi) {
//         case 1:
//             sys_write(rsi, (char *)rdx, rcx,r8);
//             return 0;
//         case 2:
//             return sys_read(rsi,(char *) rdx, rcx);
//         case 3:
//             clock((char *) rsi);
//             return 0;
//         case 4:
//             if(getFlag() || rsi == 1){
//                 if(getFlag()==0){
//                     drawWord(0x00FF0000,"You must take a screenshot first, press : and try again.\n");
//                     return 0;
//                 }
//                 printRegisters(getRegisters(), 0x00ffffff);
//             }

//             return 0;
//         case 5:
//             clear();
//             return 0;
//         case 6:
//             return ticks_elapsed();
//         case 7:
//            return (getHeight());
//         case 8:
//            return (getWidth());
//         case 9:
//             moveCursorX((uint16_t)rsi);
//             return 0;
//         case 10:
//             moveCursorY((uint16_t)rsi);
//             return 0;
//         case 11:
//             drawRectangle(rsi, rdx, rcx, r8, r9);
//             return 0;
//         case 12:
//             sleepms(rsi);
//             return 0;
//         case 13:
//             return (int) setFontSize(rsi);
//         case 14:
//             beep();
//             return 0;
//         case 15:
//             mm_status();
//             return 0;
//         case 16:
//             annihilate();
//             return 0;
//         case 17: //rellenar
//             return 0;
//         case 18: //rellenar
//             return 0;
//         case 19:
//             return block_process((pid_t)rsi);
//         case 20:
//             return unblock_process((pid_t)rsi);
//         case 21:
//             return kill_process_pid((pid_t)rsi);
//         case 22:
//             nice();
//             return 0;
//         case 23:
//             return wait_pid((pid_t)rsi);
//         case 24:
//             change_priority((pid_t)rsi,(int)rdx);
//             return 0;
//         case 25:
//             return (uint64_t) mm_malloc(rsi);
//         case 26:
//             mm_free((void *)rsi);
//             return 0;
//         case 27:
//             return create_process(rsi,rdx,rcx,(char **)r8);
//         case 28:
//             print_processes();
//             return 0;
//         case 29:
//             return get_current_pid();
//         case 30:
//             return kill_process();
//         default:
//             return 0;
//     }
// }