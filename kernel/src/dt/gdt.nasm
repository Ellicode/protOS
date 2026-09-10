bits 64
global gdt_reload_segments

section .text

gdt_reload_segments:
    movzx rdi, di 
    push rdi

    lea rax, [rel .reload_cs]
    push rax
    
    retfq

.reload_cs:
    mov ax, si
    mov ds, ax
    mov es, ax
    mov ss, ax

    xor ax, ax
    mov fs, ax
    mov gs, ax

    mov ax, dx
    ltr ax

    ret