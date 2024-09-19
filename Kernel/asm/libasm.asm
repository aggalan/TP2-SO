GLOBAL cpuVendor
GLOBAL get_seconds
GLOBAL get_minutes
GLOBAL get_wday
GLOBAL get_mday
GLOBAL get_month
GLOBAL get_year
GLOBAL get_hours
GLOBAL get_key
GLOBAL inb
GLOBAL outb

section .text

%macro SFI 0
	push rbp
	mov rbp, rsp
%endmacro

%macro SFO 0
	mov rsp, rbp
	pop rbp
%endmacro

%macro RTC 1
	xor rax, rax
 	xor rdi, rdi
  	mov al, 0x0B
  	out 70h, al
  	in al, 71h 
  	or al, 0x04 
  	out 71h, al
  	mov al, %1
  	out 70h, al
  	in al, 71h
  	mov rdi, rax
%endmacro
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

get_seconds: 
	SFI
	RTC 0
	SFO
	ret

get_minutes:
        SFI
        RTC 2
        SFO
        ret

get_hours:
        SFI
        RTC 4
        SFO
        ret

get_wday:
        SFI
        RTC 6
        SFO
        ret

get_mday:
        SFI
        RTC 7
        SFO
        ret

get_month:
        SFI
        RTC 8
        SFO
        ret

get_year:
        SFI
        RTC 9
        SFO
        ret

get_key:
	SFI
	xor rax, rax

	mov al, 0x47
	out 0x64, al

	in al, 0x60
	SFO
	ret

inb:
    mov rdx, rdi
    in al, dx
    ret

outb:
    mov rax, rsi
    mov rdx, rdi
    out dx, al
    ret
