org 0x7e00
jmp 0x0000:start

data:
	; Strings menu
	title db 'Whack-A-Mongus', 0
    jogar db '[1] Jogar!', 0
    tutorial db '[2] Tutorial', 0
    sair db '[3] Sair', 0
    creditos db 'By: mjbo, mbn2, rnfo, vko. 2021', 0

prints:
    mov bh, 00h
    mov cx, 01h
    mov ah, 02h
    int 10h
    .loop:
        lodsb
        cmp al, 0
        je .endloop
        mov ah, 09h ; printar o caractere
        mov bl, 15  ; cor: branco
        int 10h

        mov ah, 02h ; move o Cursor
        inc dl
        int 10h

        jmp .loop
    .endloop:
    ret

clear_screen:
    mov ah, 00h
    mov al, 13h
    int 10h
    ret

start:
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; Código do projeto...
    ; Modo de vídeo
    mov ah, 00h
    mov al, 13h
    int 10h
    jmp print_menu

input_menu:
    mov ah, 1
    int 16h

    ; cmp ah, 4dh
    ; je leftarrow

    ; cmp ah, 4bh
    ; je rightarrow

    cmp ah, 50h
    je uparrow_menu

    cmp ah, 48h
    je downarrow_menu

    jmp input_menu

uparrow_menu:
    jmp print_menu

downarrow_menu:
    jmp print_menu

print_menu:
    call clear_screen

    mov dx, 050Ch
    mov si, title
    call prints

    mov dx, 0C0Eh
    mov si, jogar
    call prints

    mov dx, 0E0Dh
    mov si, tutorial
    call prints

    mov dx, 100Fh
    mov si, sair
    call prints

    mov dx, 1809h
    mov si, creditos
    call prints

    call input_menu

end:

jmp $