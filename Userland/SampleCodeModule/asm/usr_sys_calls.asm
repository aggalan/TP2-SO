GLOBAL call_sys_read
GLOBAL call_sys_write
GLOBAL call_time_clock
GLOBAL call_set_font_size
GLOBAL call_print_registers
GLOBAL call_clear
GLOBAL invalid_op_asm
GLOBAL call_get_height
GLOBAL call_get_width
GLOBAL call_move_cursor_x
GLOBAL call_move_cursor_y
GLOBAL call_draw_rectangle
GLOBAL call_sleepms
GLOBAL call_beep
GLOBAL call_get_ticks
GLOBAL call_status
GLOBAL call_test_mm
GLOBAL call_process_test
GLOBAL call_prio_test
GLOBAL call_block
GLOBAL call_unblock
GLOBAL call_kill
GLOBAL call_change_priority
GLOBAL call_waitpid
GLOBAL call_malloc
GLOBAL call_free
GLOBAL call_create_process
GLOBAL call_ps
GLOBAL call_kill_process
GLOBAL call_get_current_pid
GLOBAL call_annihilate
GLOBAL call_nice
GLOBAL call_sem_open
GLOBAL call_sem_close
GLOBAL call_sem_wait
GLOBAL call_sem_post
GLOBAL call_sem_init
GLOBAL call_named_pipe_create
GLOBAL call_named_pipe_open
GLOBAL call_named_pipe_close
GLOBAL call_pipe_read
GLOBAL call_pipe_write
GLOBAL call_get_char_at
GLOBAL call_get_buffer_position
GLOBAL call_anon_pipe_create
GLOBAL call_get_process_list
GLOBAL call_free_process_list


section .text

%macro call_to_handler 1
    push rbp
    mov rbp, rsp
    mov r9, r8      ;arg 5
    mov r8, rcx     ;arg 4
    mov rcx, rdx    ;arg 3
    mov rdx, rsi    ;arg 2
    mov rsi, rdi    ;arg 1
    mov rdi, %1     ;syscall deseada
    int 0x80      ;interrupcion 80
    mov rsp, rbp
	pop rbp
    ret
%endmacro

call_sys_write:
    call_to_handler 1
call_sys_read:
    call_to_handler 2
call_time_clock:
    call_to_handler 3
call_print_registers:
    call_to_handler 4
call_clear:
    call_to_handler 5
call_get_ticks:
    call_to_handler 6
call_get_height:
    call_to_handler 7
call_get_width:
    call_to_handler 8
call_move_cursor_x:
    call_to_handler 9
call_move_cursor_y:
    call_to_handler 10
call_draw_rectangle:
    call_to_handler 11
call_sleepms:
    call_to_handler 12
call_set_font_size:
    call_to_handler 13
call_beep:
    call_to_handler 14
call_status:
    call_to_handler 15
call_annihilate:
    call_to_handler 16
call_get_buffer_position:
    call_to_handler 17
call_get_process_list:
    call_to_handler 18
call_block:
    call_to_handler 19
call_unblock:
    call_to_handler 20
call_kill:
    call_to_handler 21
call_get_char_at:
    call_to_handler 22
call_waitpid:
    call_to_handler 23
call_change_priority:
    call_to_handler 24
call_malloc:
    call_to_handler 25
call_free:
    call_to_handler 26
call_create_process:
    call_to_handler 27
call_free_process_list:
    call_to_handler 28
call_get_current_pid:
    call_to_handler 29
call_kill_process:
    call_to_handler 30
call_nice:
    call_to_handler 31
call_sem_open:
    call_to_handler 32
call_sem_close:
    call_to_handler 33
call_sem_wait:
    call_to_handler 34
call_sem_post:
    call_to_handler 35
call_sem_init:
    call_to_handler 36
call_named_pipe_create:
    call_to_handler 37
call_named_pipe_open:
    call_to_handler 38
call_named_pipe_close:
    call_to_handler 39
call_pipe_read:
    call_to_handler 40
call_pipe_write:
    call_to_handler 41
call_anon_pipe_create:
    call_to_handler 42



invalid_op_asm:
    mov rax, 0x12345678
    rdpmc
    ret