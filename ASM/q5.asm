org 0x7c00
jmp 0x0000: start

data:
    str1 db 'Malu eh uma otima monitora', 0

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

prints:
    .loop:
        lodsb           
        cmp al, 0
        je .endloop
        mov ah, 09h             
        mov bh, 0x00
        mov cx, 01h
        mov ah, 09h
        int 10h
        ;Move o Cursor
        mov bh, 00h
        mov ah, 03h
        int 10h
        mov ah, 02h
        mov bh, 00h
        inc dl
        int 10h
        ;termina de mover
        jmp .loop
    .endloop:
    ret

start:
    xor ax,ax
    xor bx,bx
    xor cx,cx
    xor dx,dx
    xor dl,dl

inputLoop:
    mov ah, 00h
    int 16h
    putchar_nostack ax
    cmp al, 13
    jne stringToint
    jmp printString

stringToint:
    sub al, 48
    mov ah, 0
    mov dx, ax
    mov ax, bx
    imul ax, 10
    add ax, dx
    mov bx, ax
    jmp inputLoop

printString:
    call endl

    pusha
    mov si, str1
    call prints
    popa

    jmp end

endl:
    mov al, 10
    putchar_nostack ax
    mov al, 13
    putchar_nostack ax                                                                                  
    ret

end:
    jmp $

times 510 - ($ - $$) db 0
dw 0xaa55