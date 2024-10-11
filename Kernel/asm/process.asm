GLOBAL create_context



call_wrapper:
    call rsi ;puntero a funcion que llamo (literalmente el puntero a donde corre el proceso) ver bien tema registros y orden
    mov rdi, 0x19 ;la proxima en irqdispatcher, hay que armarla (es este registro?)
    int 0x80

create_context:
    push rbp ;stack frame
    mov rbp, rsp

    mov rsp, rdi ; stack base

    and rsp, -16
    push 0x0 ; tiene que ir esta en presentacion
    push rdi
    push 0x202 ;esta en presentacion, la verdad ni idea
    push 0x8 ; misma
    push call_wrapper

    push rax  ;el orden de pushstate
    push rbx
    push rcx
    push rdx
    push rbp
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

    mov rax, rsp ; valor de retorno, la posicion en la que quedo en el stack
    mov rsp, rbp
    pop rbp
    ret


