;
; File: input32.inc
; Descriçao:
;     Input Functions.
;     Tratamento de entrada de digiitações.
;     Usada pelo mini-shell.
; ==================================================
; Credits: 
; Autor:
; BareMetal -- a 64-bit OS written in Assembly for x86-64 systems.
; Copyright (C) 2008-2013 Return Infinity -- see LICENSE.
; ==================================================
; History:
;     2015 - Ported.
;     Adapted by Fred Nora.
;

align 2
    db 'debug: input32.inc'
align 2

; =======================================
; os_input:
;     Take string from keyboard entry.
;  IN:
;       EDI = Location where string will be stored.
;       ECX = Maximum number of characters to accept.
; OUT:
;       ECX = Length of string that was inputed (NULL not counted).
;             All other registers preserved.
;

; pause sequence
; 0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5

; NUM lock sequence:
; 0x45 0xC5
x45_found: db 0
xC5_found: db 0

; Capslock sequence:
;'CAPS':    [[0x3A], [0xBA]],
x3A_found: db 0
xBA_found: db 0


os_input:
; Get a string.

; #bugbug
; Podemos ter um problemas com sequencias na virtualbox.
; 0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5
; https://wiki.osdev.org/PS/2_Keyboard
; alguma sequencia esta sendo enviada no momento 
; em que pressionamos as primeiras doas teclas.
; https://pythonhosted.org/pyvbox/_modules/virtualbox/library_ext/keyboard.html

    push edi
    push edx    ;Counter to keep track of max accepted characters.
    push eax

    mov edx, ecx    ; Max chars to accept.
    xor ecx, ecx    ; Offset from start.

    ; Clean keyboard buffer. Single byte.
    mov byte [key], 0
    ; NUM sequence
    ; Clean flag.
    mov byte [x45_found], 0
    mov byte [xC5_found], 0
    ; CAPS sequence
    ; Clean flag.
    mov byte [x3A_found], 0
    mov byte [xBA_found], 0

;++
; Loop
os_input_more:

; #bugbug
; Extra 'E' byte on Virtualbox.
; hex=45 | dec=69
; Only when we type the first two chars.

; Prompt
    mov al, '_'
    call os_output_char
    call os_dec_cursor

; Get key
; Pega o char gerado pela irq1 e colocado no buffer [key].
    call os_input_key 
    jnc os_halt_and_more  ; Carry flag is '0'. Restart the loop.
    ;jnc os_input_halt    ; No key entered, halt until an interrupt is received.

; Carry flag was set.
; Char in 'al'.

; --------------------
; sequence?
; 'NUM':     [[0x45], [0xC5]],
; see:
; https://pythonhosted.org/pyvbox/_modules/virtualbox/library_ext/keyboard.html
; #bugbug
; This way we can't type capital e. 'E'.
; Because it is 0x45.
; No prob, we only use lowercase in this shell. :)

    ; Testing for numlock sequence.
    cmp al, 0x45
    je seq0x45
    cmp al, 0xC5
    je seq0xC5
    ; Testing for caps sequence.
    cmp al, 0x3A
    je seq0x3A
    cmp al, 0xBA
    je seq0xBA

; --------------------
; [Enter]
; If 'Enter' key pressed, finish.
    cmp al, 0x1C
    je os_input_done

; --------------------
; [Backspace]
    cmp al, 0x0E
    je os_input_backspace

; --------------------
; Printable.
; In ASCII range (32 - 126)?
    cmp al, 32
    jl os_input_more
    cmp al, 126
    jg os_input_more

; Max
; Check if we have reached the max number of chars.
; Jump if we have (should beep as well).
    cmp ecx, edx	
    je os_input_more	

; Coloca na string pra depois ser comparada
; Store AL at EDI and increment EDI by 1. ?
; Increment the couter.
    stosb
    inc ecx

; Display char that is in 'al'.
; see: screen32.inc
    call os_output_char 

; Loop:
; Get more chars.
    jmp os_input_more
;--	

;-------------------------------------
;Backspace.
os_input_backspace:
	cmp ecx, 0			;Backspace at the beginning? get a new char.
	je os_input_more    
	
	mov al, ' '			   ;0x20 is the character for a space.
	call os_output_char    ;Write over the last typed character with the space.
	
	call os_dec_cursor		;Decrement the cursor again.
	call os_dec_cursor		;Decrement the cursor.
	dec edi				    ;Go back one in the string.
	mov byte [edi], 0x00	;NULL out the char.
	dec ecx				    ;Decrement the counter by one.
    jmp os_input_more

os_input_halt:
os_halt_and_more:
    hlt    ;Halt until another keystroke is received.
    jmp os_input_more

; -----------------------------------------------
; NUM lock sequence:  0x45 0xC5
; If the NUM lock is pressed, the virtualbox sends
; a sequence at the first 2 keystrokes.
seq0x45:
    cmp byte [x45_found], 1
    je os_input_more
seq0xC5:
    cmp byte [xC5_found], 1
    je os_input_more
; CAPS lock sequence:  0x3A 0xBA
; If the NUM lock is pressed, the virtualbox sends
; a sequence at the first 2 keystrokes.
seq0x3A:
    cmp byte [x3A_found], 1
    je os_input_more
seq0xBA:
    cmp byte [xBA_found], 1
    je os_input_more

    jmp os_input_more

;----------------------------------------
os_input_done:

; We NULL terminate the string.
; Buffer in edi.
    mov al, 0x00
    stosb

; Print space.
    mov al, ' '
    call os_output_char

    pop eax
    pop edx
    pop edi
    RET
;--------------------

;=====================================
; os_input_key:
;     Scans keyboard for input.
;  IN: Nothing
; OUT: AL = 0 if no key pressed, 
;       otherwise ASCII code, other regs preserved
;       Carry flag is set if there was a keystoke, 
;       clear if there was not.
;       All other registers preserved.
;

os_input_key:

; Get from a small buffer.
; #bugbug
; And if the buffer is dirt at the first typed chars.
; see: keyboard.inc

    xor eax, eax
    mov al, byte [key]

; Clear the variable as the keystroke is in AL now.
    mov byte [key], 0x00 

    cmp al, 0
    je os_input_key_no_key

    stc                     ;Set the carry flag.
    ret

os_input_key_no_key:
    clc                     ;Clear the carry flag.
    ret
;--------------------

;
; End.
;


