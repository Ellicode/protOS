extern panic_handler
global panic

panic:
    pop r8 
    
    mov r9, ss
    push r9
    
    mov r9, rsp      
    add r9, 8
    push r9
    
    pushfq
    
    mov r9, cs
    push r9
    
    push r8
    
    push 0
    push 0xFE
    
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
    
    mov rsi, rsp
    
    cld
    call panic_handler
        
    halt:
        hlt
        jmp halt