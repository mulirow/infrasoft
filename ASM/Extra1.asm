org 0x7c00
jmp 0x0000: start

_putchar_nostack:
    mov si, sp
    mov al, [si+2]
    mov ah, 0eh
    int 10h
    ret

%macro putchar_nostack 1
    push %1
    call _putchar_nostack
    add sp, 2
%endmacro

start:
    xor ax,ax
    xor bx,bx
    xor cx,cx
    xor dx,dx

inputLoop:
    mov ah, 00h
    int 16h
    putchar_nostack ax
    cmp al, 13
    jne stringToint

stringToint:
    sub al, 48
    mov ah, 0
    mov dx, ax
    mov ax, bx
    imul ax, 10
    add ax, dx
    mov bx, ax
    jmp inputLoop


times 510 - ($ - $$) db 0
dw 0xaa55