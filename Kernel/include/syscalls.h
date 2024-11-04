#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "../Drivers/include/keyboard_driver.h"
#include "../Drivers/include/video_driver.h"
#include "../Drivers/include/sound_driver.h"
#include "libasm.h"
#include <syscalls.h>
#include <naive_console.h>
#include "interrupts.h"
#include "lib.h"
#include "register_handling.h"
#include "../memory_manager/include/memory_manager.h"
#include "process_manager.h"
#include "scheduler.h"
#include "semaphores.h"
#include <sys/types.h>
#include "time.h"

#define STDIN 0
#define STDOUT 1
#define ERROUT 2

// writes in the file descriptor received the string, of length len in a color of HexColor
void sys_write(int descriptor, const char *str, int len, uint32_t hexColor);

// reads from the character buffer until reaching the amount of characters len or emptying the buffer and saves the chars at save
int sys_read(int descriptor, char *save, int len);

void irq_write(uint64_t rsi, char *rdx, uint64_t rcx, uint64_t r8);
void irq_read(uint64_t rsi, char *rdx, uint64_t rcx);
void irq_clock(char *rsi);
int irq_print_registers(uint32_t rsi);
int irq_clear();
int irq_ticks_elapsed();
uint16_t irq_getHeight();
uint16_t irq_getWidth();
int irq_moveCursorX(uint16_t rsi);
int irq_moveCursorY(uint16_t rsi);
int irq_drawRectangle(uint32_t rsi, uint32_t rdx, uint32_t rcx, uint32_t r8, uint32_t r9);
int irq_sleepms(uint32_t rsi);
int irq_setFontSize(uint32_t rsi);
int irq_beep();
int irq_mm_status();
int case_17();
int case_18();
int irq_annihilate();
pid_t irq_block_process(pid_t rsi);
pid_t irq_unblock_process(pid_t rsi);
pid_t irq_kill_process_pid(pid_t rsi);
int case_22();
pid_t irq_wait_pid(pid_t rsi);
int irq_change_priority(pid_t rsi, uint8_t rdx);
uint64_t irq_mm_malloc(uint32_t rsi);
int irq_mm_free(void *rsi);
pid_t irq_create_process(uint64_t rsi, uint8_t rdx, uint64_t rcx, char **r8, int r9);
int irq_print_processes();
pid_t irq_get_current_pid();
pid_t irq_kill_process();
void irq_nice();
int irq_sem_open(int rsi);
int irq_sem_close(int rsi);
int irq_sem_wait(int rsi);
int irq_sem_post(int rsi, int rdx);
int irq_sem_init(int rsi);
int irq_named_pipe_create(char * rsi);
int irq_named_pipe_open(char * rsi, int rdx);
void irq_named_pipe_close(int rsi);
ssize_t irq_pipe_read(int rsi, char * rdx, size_t rcx);
ssize_t irq_pipe_write(int rsi, char * rdx, size_t rcx);

#endif // TPE_SYSCALLS_H
