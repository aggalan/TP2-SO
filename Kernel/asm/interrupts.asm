
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
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
GLOBAL getRegisters
GLOBAL printRegistersAsm
GLOBAL getFlag
GLOBAL saveRegisters
GLOBAL nice

EXTERN schedule
EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN retUserland
EXTERN getStackBase
EXTERN clear
EXTERN printRegisters
SECTION .text


%macro saveRegistersState 0
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

%macro pushState 0
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

%macro popState 0
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
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro


getRegisters:
    mov rax, registers
    mov byte[flag],0

    ret

nice:
	int 0x20

getFlag:
	movzx rax, byte[flag]
	ret

%macro exceptionHandler 1
    push rsp
    push qword[rsp + 8]
	pushState
	;Guardo el estado del registros
    saveRegistersState
    mov byte[flag],0

	;llamo a funcion en c que imprime los registros
	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher
	popState
	add rsp, 16

	call getStackBase
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

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
		;irqHandlerMaster 0

	;esto va a decir push registros, cambio de rsp, pop registros
	pushState
	mov rdi, 0
	call irqDispatcher

    ; levantar procesos que esta esperando el timer

    ;call wakeUpProcesses

	; llamar al scheduler para que me devuelva un rsp
	mov rdi, rsp
	call schedule
	mov rsp, rax

	mov al, 20h
	out 20h, al

	popState ; (aca ya estoy en el stack del otro proceso)
	iretq

;Keyboard
_irq01Handler:
    push rsp
    push qword[rsp + 8]
    pushState
    mov rax, 0
    in al, 60h
    cmp al,27h
    jne handle
    saveRegistersState
    mov byte[flag],1
    mov al, 20h
    out 20h, al
    popState
    add rsp, 16
    iretq
handle:
    popState
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

    call irqDispatcher

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
	exceptionHandler 0

_exception6Handler:
	exceptionHandler 6



SECTION .data
userland equ 0x400000

SECTION .bss
   	aux resq 1
	registers resq 20
	flag resq 1

