// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <time.h>
#include <stdint.h>
#include "include/syscalls.h"

typedef uint64_t (*syscall_func_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

syscall_func_t syscalls[] = {
    [1] = (syscall_func_t)irq_write,
    [2] = (syscall_func_t)irq_read,
    [3] = (syscall_func_t)irq_clock,
    [4] = (syscall_func_t)irq_print_registers,
    [5] = (syscall_func_t)irq_clear,
    [6] = (syscall_func_t)irq_ticks_elapsed,
    [7] = (syscall_func_t)irq_get_height,
    [8] = (syscall_func_t)irq_get_width,
    [9] = (syscall_func_t)irq_move_cursor_x,
    [10] = (syscall_func_t)irq_move_cursor_y,
    [11] = (syscall_func_t)irq_draw_rectangle,
    [12] = (syscall_func_t)irq_sleepms,
    [13] = (syscall_func_t)irq_set_font_size,
    [14] = (syscall_func_t)irq_beep,
    [15] = (syscall_func_t)irq_mm_status,
    [16] = (syscall_func_t)irq_annihilate,
    [17] = (syscall_func_t)irq_get_buffer_position,
    [18] = (syscall_func_t)irq_pipes_status,
    [19] = (syscall_func_t)irq_block_process,
    [20] = (syscall_func_t)irq_unblock_process,
    [21] = (syscall_func_t)irq_kill_process_pid,
    [22] = (syscall_func_t)irq_get_char_at,
    [23] = (syscall_func_t)irq_wait_pid,
    [24] = (syscall_func_t)irq_change_priority,
    [25] = (syscall_func_t)irq_mm_malloc,
    [26] = (syscall_func_t)irq_mm_free,
    [27] = (syscall_func_t)irq_create_process,
    [28] = (syscall_func_t)irq_print_processes,
    [29] = (syscall_func_t)irq_get_current_pid,
    [30] = (syscall_func_t)irq_kill_process,
    [31] = (syscall_func_t)irq_nice,
    [32] = (syscall_func_t)irq_sem_open,
    [33] = (syscall_func_t)irq_sem_close,
    [34] = (syscall_func_t)irq_sem_wait,
    [35] = (syscall_func_t)irq_sem_post,
    [36] = (syscall_func_t)irq_sem_init,
    [37] = (syscall_func_t)irq_named_pipe_create,
    [38] = (syscall_func_t)irq_named_pipe_open,
    [39] = (syscall_func_t)irq_named_pipe_close,
    [40] = (syscall_func_t)irq_pipe_read,
    [41] = (syscall_func_t)irq_pipe_write,
    [42] = (syscall_func_t)irq_anon_pipe_create,
    [43] = (syscall_func_t)irq_get_process_list,
    [44] = (syscall_func_t) irq_free_process_list,
};

static void int_20();
static void int_21();
static uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
void irq_dispatcher(uint64_t irq, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
    switch (irq)
    {
    case 0:
        int_20();
        break;
    case 1:
        int_21();
        break;
    case 0x80:
        int_80(rdi, rsi, rdx, rcx, r8, r9);
        break;
    }
    return;
}

void int_20()
{
    timer_handler();
}
void int_21()
{
    keyboard_handler();
}

uint64_t int_80(uint64_t syscall_number, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
    if (syscall_number < sizeof(syscalls) / sizeof(syscalls[0]) && syscalls[syscall_number] != NULL)
    {
        return syscalls[syscall_number](rsi, rdx, rcx, r8, r9);
    }
    return -1;
}
