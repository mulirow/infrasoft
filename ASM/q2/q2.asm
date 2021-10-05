org 0x7c00
jmp 0x0000: start

_getchar:
    mov ah, 00h
    int 16h
    ret

%macro getchar 0
    call _getchar
%endmacro

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

_putchar:
    mov si, sp
    mov al, [si+2]
    mov ah, 0eh
    int 10h
    ret

%macro putchar 1
    push %1
    call _putchar
%endmacro

start:
    xor ax, ax
    xor cx, cx

loop:
    getchar
    putchar ax
    cmp al, 13
    je endl
    inc cx
    jmp loop

endl:
    mov al, 10
    putchar_nostack ax
    mov al, 13
    putchar_nostack ax
    jmp invertLoop

invertPrint:
    mov si, sp
    mov al, [si+4]
    mov ah, 0eh
    int 10h
    ret
    
invertLoop:
    cmp cx, 0
    je end

    call invertPrint
    add sp, 2

    dec cx
    jmp invertLoop

end:
    mov al, 0
    stosb

times 510 - ($ - $$) db 0
dw 0xaa55