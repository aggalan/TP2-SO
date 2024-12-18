// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "stdint.h"
#include "sys/types.h"
#include "lib.h"

#ifndef USERSYSCALLS_H
#define USERSYSCALLS_H

// Reads data from a file descriptor into the kernels buffer
ssize_t call_sys_read(int fd, char *buf, int size);

// Writes data from the  kernels buffer to a file descriptor with a specified color
ssize_t call_sys_write(int fd, const char *buf, int size, uint32_t hexColor);

// Gets the current time and stores it in a buffer
void call_time_clock(char *buf);

// Prints registers based on a flag value
void call_print_registers(int flag);

// Draws a rectangle on the screen with specified color, position, and dimensions
void call_draw_rectangle(uint64_t color, uint32_t x, uint32_t y, uint32_t height, uint32_t width);

// Returns the current position of the kernels buffer
int call_get_buffer_position();

// Gets the character at a specified position on the kernels buffer
int call_get_char_at(int pos);

// Draws a squarewith specified position, size, and color
void call_put_square(uint32_t x, uint32_t y, uint32_t size, uint64_t hexColor);

// Pauses the program execution for a specified number of milliseconds
void call_sleepms(int mseconds);

// Sets the font size for text
int call_set_font_size(uint32_t size);

// Draws a word on a graphics screen with specified color at a given position
void call_draw_word_color_at(uint64_t hexColor, char *word, uint32_t x, uint32_t y);

// Draws a character  with specified color at a given position
void call_character_at(uint64_t hexColor, char c, uint32_t x, uint32_t y);

// Produces a beep
void call_beep();

// Clears the screen
void call_clear();

// Handles an invalid operation in assembly language
void invalid_op_asm();

// Gets the width of the screen or drawing area
uint16_t call_get_width();

// Gets the height of the screen or drawing area
uint16_t call_get_height();

// Moves the cursor to a specified vertical position
void call_move_cursor_y(uint16_t posY);

// Moves the cursor to a specified horizontal position
void call_move_cursor_x(uint16_t posX);

// Gets the elapsed time since some reference point in milliseconds
int call_get_ticks();

memory_status_t * call_status();

void call_annihilate();

void call_process_test();

void call_prio_test();

pid_t call_block(pid_t pid);

pid_t call_unblock(pid_t pid);

pid_t call_kill(pid_t pid);

pid_t call_waitpid(pid_t pid_to_wait);

int call_change_priority(int pid, int newPrio);

void *call_malloc(uint64_t size);

void call_free(void *ptr);

pid_t call_create_process(void *entryPoint, int *fds, int argc, char *argv[], int ground);

void call_ps();

pid_t call_get_current_pid();

pid_t call_kill_process();

void call_nice();

int call_sem_open(int id);

int call_sem_close(int id);

int call_sem_wait(int id);

int call_sem_post(int id);

int call_sem_init(int n);

int call_named_pipe_create(char *name);

int call_named_pipe_open(char *name, int mode);

void call_named_pipe_close(int fd);

ssize_t call_pipe_read(int fd, char *buff, size_t bytes_r);

ssize_t call_pipe_write(int fd, char *buff, size_t bytes_w);

int call_anon_pipe_create();

process_list_t* call_get_process_list();

void call_free_process_list(process_list_t* list);

void call_openMVar(MVar_t * mvar);

void call_putMVar(MVar_t * mvar, int value);

int call_takeMVar(MVar_t * mvar);

#endif /* USERSYSCALLS_H */