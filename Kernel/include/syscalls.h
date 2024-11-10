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
#include "fd_manager.h"

ssize_t sys_write(int descriptor, const char *str, int len, uint32_t hexColor);

ssize_t sys_read(int descriptor, char *save, int len);

void irq_write(uint64_t rsi, char *rdx, uint64_t rcx, uint64_t r8);

void irq_read(uint64_t rsi, char *rdx, uint64_t rcx);

void irq_clock(char *rsi);

int irq_print_registers(uint32_t rsi);

int irq_clear();

int irq_ticks_elapsed();

uint16_t irq_get_height();

uint16_t irq_get_width();

int irq_move_cursor_x(uint16_t rsi);

int irq_move_cursor_y(uint16_t rsi);

int irq_draw_rectangle(uint32_t rsi, uint32_t rdx, uint32_t rcx, uint32_t r8, uint32_t r9);

int irq_sleepms(uint32_t rsi);

int irq_set_font_size(uint32_t rsi);

int irq_beep();

memory_status_t *irq_mm_status();

int irq_annihilate();

pid_t irq_block_process(pid_t rsi);

pid_t irq_unblock_process(pid_t rsi);

pid_t irq_kill_process_pid(pid_t rsi);

pid_t irq_wait_pid(pid_t rsi);

int irq_change_priority(pid_t rsi, uint8_t rdx);

uint64_t irq_mm_malloc(uint32_t rsi);

int irq_mm_free(void *rsi);

pid_t irq_create_process(uint64_t rsi, int *rdx, uint64_t rcx, char **r8, int r9);

pid_t irq_get_current_pid();

pid_t irq_kill_process();

void irq_nice();

int irq_sem_open(int rsi);

int irq_sem_close(int rsi);

int irq_sem_wait(int rsi);

int irq_sem_post(int rsi, int rdx);

int irq_sem_init(int rsi);

int irq_named_pipe_create(char *rsi);

int irq_named_pipe_open(char *rsi, int rdx);

void irq_named_pipe_close(int rsi);

ssize_t irq_pipe_read(int rsi, char *rdx, size_t rcx);

ssize_t irq_pipe_write(int rsi, char *rdx, size_t rcx);

char irq_get_char_at(int rsi);

int irq_get_buffer_position();

int irq_anon_pipe_create();

process_list_t *irq_get_process_list();

void irq_free_process_list(process_list_t *rsi);

#endif
