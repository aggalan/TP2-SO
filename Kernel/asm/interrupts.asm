GLOBAL _cli
GLOBAL _sti
GLOBAL pic_master_mask
GLOBAL pic_slave_mask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq80Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler
GLOBAL get_registers
GLOBAL print_registers_asm
GLOBAL get_flag
GLOBAL save_registers_state
GLOBAL nice

EXTERN schedule
EXTERN irq_dispatcher
EXTERN exception_dispatcher
EXTERN ret_userland
EXTERN get_stack_base
EXTERN clear
EXTERN print_registers
SECTION .text

%macro save_registers_state 0
    mov rax, [rsp]
	mov [registers],rax ;r15
	mov rax , [rsp + 8]
	mov [registers+8], rax ;r14
	mov rax, [rsp+16]
	mov [registers+16],rax ;r13
	mov rax, [rsp+24]
	mov [registers+24],rax ;r12
	mov rax, [rsp+32]
	mov [registers+32],rax ;r11
	mov rax, [rsp + 40]
	mov [registers+40],rax ;r10
	mov rax, [rsp + 48]
	mov [registers+48],rax ;r9
	mov rax, [rsp + 56]
	mov [registers+56],rax ;r8
	mov rax, [rsp + 64]
	mov [registers+64],rax ; rsi
	mov rax, [rsp + 72]
	mov [registers+72],rax ;rdi
	mov rax, [rsp + 80]
	mov [registers+80],rax ;rdx
	mov rax, [rsp + 88]
	mov [registers+88],rax ;rcx
	mov rax, [rsp + 96]
	mov [registers+ 96],rax;rbx
	mov rax, [rsp + 104]
	mov [registers+ 104],rax ;rax
	mov rax, [rsp + 112]
	mov [registers+112],rax ;rbp

	mov rax, [rsp + 120]
	mov [registers+120],rax ;rip
	mov rax, [rsp + 128]
	mov [registers+128],rax ;rsp

	mov rax, [rsp + 152]
	mov [registers + 136], rax ; rflags

%endmacro

%macro push_state 0
    push rbp
	push rax
	push rbx
	push rcx
	push rdx
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro pop_state 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	pop rbp
%endmacro

%macro irqHandlerMaster 1
	push_state

	mov rdi, %1 ; pasaje de parametro
	call irq_dispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	pop_state
	iretq
%endmacro


get_registers:
    mov rax, registers
    mov byte[flag],0

    ret

nice:
	int 0x20
	ret

get_flag:
	movzx rax, byte[flag]
	ret

%macro exception_handler 1
    push rsp
    push qword[rsp + 8]
	push_state
	;Guardo el estado del registros
    save_registers_state
    mov byte[flag],0

	;llamo a funcion en c que imprime los registros
	mov rdi, %1 ; pasaje de parametro
	call exception_dispatcher
	pop_state
	add rsp, 16

	call get_stack_base
	mov [rsp+24], rax
    mov rax, userland
    mov [rsp], rax

   	iretq
%endmacro

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

pic_master_mask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

pic_slave_mask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

;8254 Timer (Timer Tick)
_irq00Handler:

	push_state
	mov rdi, 0
	call irq_dispatcher

	mov rdi, rsp
	call schedule
	mov rsp, rax

	mov al, 20h
	out 20h, al

	pop_state 
	iretq

;Keyboard
_irq01Handler:
    push rsp
    push qword[rsp + 8]
    push_state
    mov rax, 0
    in al, 60h
    cmp al,27h
    jne handle
    save_registers_state
    mov byte[flag],1
    mov al, 20h
    out 20h, al
    pop_state
    add rsp, 16
    iretq
handle:
    pop_state
    add rsp, 16
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

_irq80Handler:

 	push rbx
	push r12
	push r13
	push r14
	push r15
    push rbp
    mov rbp,rsp

	push r9

    mov r9, r8
    mov r8, rcx
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, 0x80

    call irq_dispatcher

    pop r9
    mov rsp, rbp
    pop rbp
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx

    iretq


;Zero Division Exception
_exception0Handler:
	exception_handler 0

_exception6Handler:
	exception_handler 6



SECTION .data
userland equ 0x400000

SECTION .bss
   	aux resq 1
	registers resq 20
	flag resq 1

