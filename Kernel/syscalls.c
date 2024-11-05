// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Drivers/include/video_driver.h"
#include "include/syscalls.h"
#include "include/keyboard_buffer.h"
#include "include/libasm.h"
#include <naive_console.h>
#include "include/fd_manager.h"
#include "include/pipe_manager.h"
#include "include/scheduler.h"
#define EOF -1

#define MIN(x, y) x < y ? x : y

int shell_read(char *save, int len);

ssize_t sys_write(int descriptor, const char *str, int len, uint32_t hexColor)
{
    fd_entry *entry = fd_get_entry(descriptor); // agregar chequeo de error, y ver porq aca misticamente no anda
    switch (entry->fd_type)
    {
    case STDOUT:
        PCB *pcb = get_current();
        if (pcb->fds[STDOUT] == STDOUT)
        {
            draw_word_len(hexColor, str, len);
            return len;
        }
        else
        {
            return sys_write(pcb->fds[STDOUT], str, len, hexColor);
        }
    case ERROUT:
        draw_word_len(0x00ff0000, str, len);
        return len;
    case FD_TYPE_PIPE:
        return pipe_write(descriptor, str, len);
    case FD_TYPE_ANON_PIPE:
        return anon_pipe_write(descriptor, str, len);
    default:
        draw_word(0x00ff0000, "no such descriptor");
        return -1;
    }
}

ssize_t sys_read(int descriptor, char *save, int len)
{
    fd_entry *entry = fd_get_entry(descriptor);
    switch (entry->fd_type)
    { // aca deberia switcher en algo como fd_table[fd]->type. pafa ver si es pipe u otra cosa
    case STDIN:
        //            return shell_read(save, len);
        PCB *pcb = get_current();
        if (pcb->fds[STDIN] == STDIN)
        {
            return shell_read(save, len);
        }
        else
        {
            return sys_read(pcb->fds[STDIN], save, len);
        }
    case FD_TYPE_PIPE:
        return pipe_read(descriptor, save, len);
    case FD_TYPE_ANON_PIPE:
        return anon_pipe_read(descriptor, save, len);
    default:
        draw_word(0x00ff0000, "no such descriptor");
        break;
    }

    //    if (descriptor != STDIN)
    //    {
    //        draw_word(0x00ff0000, "no such descriptor");
    //    }

    return -1;
}


int shell_read(char *save, int len) {
//    PCB * pcb = get_current();

//    if(pcb->ground == 0 && pcb->pid != 1 && pcb->pid != 0){ //to mimic the behaviour of cat when running in background like linux
//        kill_process_pid(pcb->pid);
//        return 0;
//    }


    block_shell_read();

    int n = get_buffer_position();

    if (get_char_at(n) == 0)
    {
        *save = 0;
        return 0;
    }

    if (get_char_at(n) == EOF)
    {
        *save = EOF;
        consume_buffer_at(n);
        return EOF;
    }

    int length = MIN(len, get_buffer_len());

    for (int i = 0; i < length; i++)
    {
        n = get_buffer_position();
        save[i] = get_char_at(n);
        consume_buffer_at(n);
    }
    return length;
}

void two_chars(char *first, int j, char *app)
{
    for (int i = 0; i < 2; i++)
    {
        first[j + i] = app[i];
    }
}

void irq_write(uint64_t rsi, char *rdx, uint64_t rcx, uint64_t r8)
{
    sys_write(rsi, rdx, rcx, r8);
}

void irq_read(uint64_t rsi, char *rdx, uint64_t rcx)
{
    sys_read(rsi, rdx, rcx);
}

void irq_clock(char *rsi)
{
    clock(rsi);
}

int irq_print_registers(uint32_t rsi)
{
    if (get_flag() || rsi == 1)
    {
        if (get_flag() == 0)
        {
            draw_word(0x00FF0000, "You must take a screenshot first, press : and try again.\n");
            return 0;
        }
        print_registers(get_registers(), 0x00ffffff);
    }
    print_registers(get_registers(), rsi);
    return 0;
}

int irq_clear()
{
    clear();
    return 0;
}

int irq_anon_pipe_create()
{
    return anon_pipe_create();
}

int irq_ticks_elapsed()
{
    return ticks_elapsed();
}

uint16_t irq_get_height()
{
    return get_height();
}

uint16_t irq_get_width()
{
    return get_width();
}

int irq_move_cursor_x(uint16_t rsi)
{
    move_cursor_x(rsi);
    return 0;
}

int irq_move_cursor_y(uint16_t rsi)
{
    move_cursor_y(rsi);
    return 0;
}

int irq_draw_rectangle(uint32_t rsi, uint32_t rdx, uint32_t rcx, uint32_t r8, uint32_t r9)
{
    draw_rectangle(rsi, rdx, rcx, r8, r9);
    return 0;
}

int irq_sleepms(uint32_t rsi)
{
    sleepms(rsi);
    return 0;
}

int irq_set_font_size(uint32_t rsi)
{
    return (int)set_font_size(rsi);
}

int irq_beep()
{
    beep();
    return 0;
}

int irq_mm_status()
{
    mm_status();
    return 0;
}

int case_17()
{
    return 0;
}

int case_18()
{
    return 0;
}

int irq_annihilate()
{
    annihilate();
    return 0;
}

pid_t irq_block_process(pid_t rsi)
{
    return block_process(rsi);
}

pid_t irq_unblock_process(pid_t rsi)
{
    return unblock_process(rsi);
}

pid_t irq_kill_process_pid(pid_t rsi)
{
    return kill_process_pid(rsi);
}

int case_22()
{
    return 0;
}

pid_t irq_wait_pid(pid_t rsi)
{
    return wait_pid(rsi);
}

int irq_change_priority(pid_t rsi, uint8_t rdx)
{
    return change_priority(rsi, rdx);
}

uint64_t irq_mm_malloc(uint32_t rsi)
{
    return (uint64_t)mm_malloc(rsi);
}

int irq_mm_free(void *rsi)
{
    mm_free(rsi);
    return 0;
}

pid_t irq_create_process(uint64_t rsi, int *rdx, uint64_t rcx, char **r8, int r9)
{
    return create_process(rsi, rdx, rcx, r8, r9);
}

int irq_print_processes()
{
    print_processes();
    return 0;
}

pid_t irq_get_current_pid()
{
    return get_current_pid();
}

pid_t irq_kill_process()
{
    return kill_process();
}

void irq_nice()
{
    nice();
}

int irq_sem_open(int rsi)
{
    return my_sem_open(rsi);
}

int irq_sem_close(int rsi)
{
    return my_sem_close(rsi);
}

int irq_sem_wait(int rsi)
{
    return my_sem_wait(rsi);
}

int irq_sem_post(int rsi, int rdx)
{
    return my_sem_post(rsi);
}

int irq_sem_init(int rsi)
{
    return my_sem_init(rsi);
}
int irq_named_pipe_create(char *rsi)
{
    return named_pipe_create(rsi);
}
int irq_named_pipe_open(char *rsi, int rdx)
{
    return named_pipe_open(rsi, rdx);
}
void irq_named_pipe_close(int rsi)
{
    named_pipe_close(rsi);
}
ssize_t irq_pipe_read(int rsi, char *rdx, size_t rcx)
{
    return pipe_read(rsi, rdx, rcx);
}
ssize_t irq_pipe_write(int rsi, char *rdx, size_t rcx)
{
    return pipe_write(rsi, rdx, rcx);
}
char irq_get_char_at(int rsi)
{
    return get_char_at(rsi);
}
int irq_get_buffer_position()
{
    return get_buffer_position();
}