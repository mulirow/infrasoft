org 0x7c00
jmp 0x0000: start

_getchar:
    mov ah,00h
    int 16h
    ret

%macro getchar 0
    call _getchar
%endmacro

_putchar:
    mov si,sp
    mov al, [si+2]
    mov ah, 0eh
    int 10h
    ret

%macro putchar 1
    push %1
    call _putchar
    add sp,2
    ret
%endmacro 

start:
    xor ax,ax
    xor cx,cx

loop:
    getchar
    putchar ax
    cmp al, 13
    je endl
    inc cx
    jmp loop

endl:
    mov al 10
    putchar ax
    mov al, 13
    putchar al
    
    jmp Search

Search:
    getchar
    sub cx,al

    mov si,sp
    mov al,[si+cx]






times 510 - ($ - $$) db 0
dw 0xaa55