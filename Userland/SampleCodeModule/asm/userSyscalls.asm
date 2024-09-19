GLOBAL call_sys_write
GLOBAL call_sys_read
GLOBAL call_reg_state
GLOBAL call_time_clock
GLOBAL call_size_up
GLOBAL call_size_down
GLOBAL call_clear
GLOBAL call_font
GLOBAL call_sound
GLOBAL call_paint_screen
GLOBAL call_drawWordColorAt
GLOBAL call_put_square
GLOBAL call_handler
GLOBAL call_pixelColorAt
GLOBAL call_get_pos
GLOBAL call_get_charAt
GLOBAL call_sleepms
GLOBAL call_sleeps
GLOBAL call_clear_buff
GLOBAL call_move_screen_right
GLOBAL call_eliminator_flag_on
GLOBAL call_eliminator_flag_off
GLOBAL call_print_image

section .text

%macro call_handler 1
    push rbp
    mov rbp, rsp
    
    mov rax, %1
    int 80h

    mov rsp, rbp
    pop rbp
    ret
%endmacro


call_sys_write:
    call_handler 1
call_sys_read:
    call_handler 2
call_time_clock:
    call_handler 3
call_reg_state:
    call_handler 4
call_size_up:
    call_handler 5
call_size_down:
    call_handler 6
call_clear:
    call_handler 7
call_font:
    call_handler 8
call_sound:
    call_handler 9
call_paint_screen:
    call_handler 10
call_drawWordColorAt:
    call_handler 11
call_put_square:
    call_handler 12
call_sleepms:
    call_handler 13
call_sleeps:
    call_handler 14
call_pixelColorAt:
    call_handler 15
call_get_pos:
    call_handler 16
call_get_charAt:
    call_handler 17
call_clear_buff:
    call_handler 18
call_move_screen_right:
    call_handler 19
call_eliminator_flag_on:
    call_handler 20
call_eliminator_flag_off:
    call_handler 21
call_print_image:
    call_handler 22