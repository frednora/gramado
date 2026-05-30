; file: command.inc
; Very simple command interpreter using INT 21h


MY_COMMAND:

    mov si, msg_next_line
    call DisplayMessageDollar

    ; Show banner
    mov si, msg_command_banner
    call DisplayMessageDollar

.cmd_loop:

    mov dx, msg_prompt
    mov ah, 09h
    int 21h

    mov di, LINBUF        ; buffer start
    mov cx, 0             ; char counter

    xor ax, ax
    xor bx, bx
    xor cx, cx
    xor dx, dx

.read_char:

; Get the char
    mov ah, 01h           ; CONIN
    int 21h

    ; Compare with?
    ;cmp al, 'q' ; test
    cmp al, 0Dh           ; Enter pressed?
    je .interpret

    ;mov dl, 'x' ;test
    mov dl, al
    mov ah, 02h           ; CONOUT
    int 21h

    mov [di], al          ; store char
    inc di
    inc cx
    jmp .read_char

.interpret:
    mov byte [di], 0      ; terminate string

    ; Compare with "help"
    mov si, LINBUF
    mov di, str_help
    call str_compare
    jz .do_help

    ; Compare with "about"
    mov si, LINBUF
    mov di, str_about
    call str_compare
    jz .do_about

    ; Compare with "exit"
    mov si, LINBUF
    mov di, str_exit
    call str_compare
    jz .do_exit

    ; Unknown command
    mov dx, msg_unknown
    mov ah, 09h           ; Print string ($ terminated)
    int 21h
    jmp .cmd_loop

;------------------------------

.do_help:
    mov dx, msg_next_line
    mov ah, 09h
    int 21h
    mov dx, msg_help
    mov ah, 09h
    int 21h
    jmp .cmd_loop

.do_about:
    mov dx, msg_next_line
    mov ah, 09h
    int 21h
    mov dx, msg_about
    mov ah, 09h
    int 21h
    jmp .cmd_loop

.do_exit:
    mov dx, msg_next_line
    mov ah, 09h
    int 21h
    ret

; ------------------------
; String compare routine
; IN: SI=buffer, DI=string
; OUT: ZF=1 if equal
str_compare:
.next:
    lodsb
    cmp al, [di]
    jne .not_equal
    cmp al, 0
    je .equal
    inc di
    jmp .next
.equal:
    xor ax, ax
    ret
.not_equal:
    mov ax, 1
    ret

; ------------------------
; Data
LINBUF:
    times 80 db 0
str_help:   db "help",0
str_about:  db "about",0
str_exit:   db "exit",0


msg_prompt: db "> ", "$"

msg_next_line:
    db 13, 10, "$"
msg_command_banner: 
    db "Command v1.0", 13, 10, "$"
msg_help:   db "Available commands: HELP, ABOUT, EXIT", 13, 10, "$"
msg_about:  db "My Hobby OS - Simple COMMAND demo", 13, 10, "$"
msg_unknown:db "Unknown command", 13, 10, "$"

