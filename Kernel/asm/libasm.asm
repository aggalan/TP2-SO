GLOBAL cpu_vendor
GLOBAL get_key
GLOBAL get_hours
GLOBAL get_minutes
GLOBAL get_seconds
GLOBAL inb
GLOBAL outb
GLOBAL acquire
GLOBAL release
section .text
    
cpu_vendor:
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

get_key:
  push rbp
  mov rbp, rsp
  mov rax, 0
  in al, 0x60       ; lee la TECLA PRESIONADA desde el puerto 60h
_good:  
  mov rsp, rbp 
  pop rbp
  ret

 get_hours:
      push rbp
      mov rbp, rsp
      xor rax, rax
        xor rdi, rdi
        mov al, 0x0B
        out 70h, al
        in al, 71h
        or al, 0x04
        out 71h, al
      mov al, 4
      out 0x70, al
      in al, 0x71

      mov rsp, rbp
      pop rbp
      ret

  get_minutes:
      push rbp
      mov rbp, rsp
     xor rax, rax
       xor rdi, rdi
       mov al, 0x0B
       out 70h, al
       in al, 71h
       or al, 0x04
       out 71h, al
      mov al, 2
      out 0x70, al
      in al, 0x71

      mov rsp, rbp
      pop rbp
      ret

  get_seconds:
      push rbp
      mov rbp, rsp
    xor rax, rax
      xor rdi, rdi
      mov al, 0x0B
      out 70h, al
      in al, 71h
      or al, 0x04
      out 71h, al
      mov al, 0
      out 0x70, al
      in al, 0x71

      mov rsp, rbp
      pop rbp
      ret


inb:                ; Funciones para el correcto funcionamiento del soundDriver
    push rbp
    mov rbp, rsp

    mov rdx,rdi
    in al,dx        ; pasaje en 8 bits

    mov rsp, rbp
    pop rbp
    ret

outb:
    push rbp
    mov rbp, rsp

    mov rax, rsi    
    mov rdx, rdi
    out dx, al        ; pasaje en 8 bits

    mov rsp, rbp
    pop rbp
    ret


acquire:
    mov al, 0
.loop:
    xchg [rdi], al
    test al, al
    jz .loop
    ret

release:
    mov byte [rdi], 1
    ret

;revisar size