extern isr_handler

%macro ISR_NOERR 1
global isr_call_%1
isr_call_%1:
    cli
    push 0
    push %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERR 1
global isr_call_%1
isr_call_%1:
    cli
    push %1
    jmp isr_common_stub
%endmacro

global isr_common_stub
isr_common_stub:
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rsi
    push rdi
    push rdx
    push rcx
    push rbx
    push rax

    mov rdi, rsp        ; pass pointer to frame as first argument
    call isr_handler    ; isr_handler(idt_frame_t* frame)

    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rdi
    pop rsi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    add rsp, 16         ; skip vector and error_code
    iretq
    
ISR_NOERR   0
ISR_NOERR   1
ISR_NOERR   2
ISR_NOERR   3
ISR_NOERR   4
ISR_NOERR   5
ISR_NOERR   6
ISR_NOERR   7
ISR_ERR     8
ISR_NOERR   9
ISR_ERR     10
ISR_ERR     11
ISR_ERR     12
ISR_ERR     13
ISR_ERR     14
ISR_NOERR   15
ISR_NOERR   16
ISR_ERR     17
ISR_NOERR   18
ISR_NOERR   19
ISR_NOERR   20
ISR_ERR     21
ISR_NOERR   22
ISR_NOERR   23
ISR_NOERR   24
ISR_NOERR   25
ISR_NOERR   26
ISR_NOERR   27
ISR_NOERR   28
ISR_NOERR   29
ISR_NOERR   30
ISR_NOERR   31

ISR_NOERR   32 ; APIC timer