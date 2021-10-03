org 0x7c00
jmp 0x0000: _start

data:
    str1 db 'Parte inteira: ', 0
    str2 db 'Resto: ', 0
    str3 db ' (Impar)', 13, 10, 0
    str4 db ' (Par)', 13, 10, 0

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
        mov ah, 0x0e
        int 10h
        jmp .loop
    .endloop:
    ret

_start:
    mov bx, 0
    mov cx, 0

inputLoop:
    mov ah, 00h
    int 16h
    putchar_nostack ax
    cmp al, 13
    jne stringToInt
    jmp push_num

stringToInt:
    sub al, 48
    mov ah, 0
    mov dx, ax
    mov ax, bx
    imul ax, 10
    add ax, dx
    mov bx, ax
    jmp inputLoop

push_num:
    call endl
    mov ax, bx
    push ax
    mov bx, 0
    inc cx
    cmp cx, 4
    jne inputLoop
    jmp calc


calc:
    mov ax, 0
    mov bx, 0
    mov cx, 0

    ;w
    pop ax
    sub bx, ax
    add cx, ax

    ;z
    pop ax
    add bx, ax
    sub cx, ax

    ;y
    pop ax
    add bx, ax
    add cx, ax

    ;x
    pop ax
    add bx, ax
    add cx, ax

    mov ax, bx

    div cl
    

    mov cx, 0
    mov dx, 1

    mov cl, ah
    mov ah, 0
    ; al: quociente
    ; cl: resto
    pusha
    mov si, str1
    call prints
    popa

    pusha
    cmp al, 0
    jne intToString
    putchar_nostack '0'
    popa

intToString:
    cmp al, 0
    je printResult
    mov bl, 10
    div bl                  
    mov bx, 0
    mov bl, ah 
    mov ah, 0

    push bx
    inc dx

    jmp intToString

printResult:
    dec dx
    cmp dx, 0
    jne funcpop
    
    jmp parte2
    
funcpop:
    pop ax
    add al, 48
    mov ah, 0eh
    int 10h
    jmp printResult

parte2:
    mov ah, 0
    sub al, 48

    mov bl, 2
    div bl

    cmp ah, 0
    je .par
    .impar:
        pusha
        mov si, str3
        call prints
        popa
    .endimpar:
    cmp ah, 0
    jne .endpar
    .par:
        pusha
        mov si, str4
        call prints
        popa
    .endpar:

    mov ah, 0
    mov al, cl
    mov dx, 1

    pusha
    mov si, str2
    call prints
    popa

    pusha
    cmp al, 0
    jne intToString2
    putchar_nostack '0'
    popa

intToString2:
    cmp al, 0
    je printResult2
    mov bl, 10
    div bl
    mov bx, 0
    mov bl, ah
    mov ah, 0

    push bx
    inc dx

    jmp intToString2
    
printResult2:
    dec dx
    cmp dx, 0
    jne pop2
    jmp end

pop2:
    pop ax
    add al, 48
    mov ah, 0eh
    int 10h
    jmp printResult2

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