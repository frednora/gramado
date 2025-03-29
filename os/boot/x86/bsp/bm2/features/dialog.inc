
; dialog.inc

; ------------------------------------------------------------------
; os_dialog_box -- Print dialog box in middle of screen, with button(s)
; IN: AX, BX, CX = string locations (set registers to 0 for no display)
; IN: DX = 0 for single 'OK' dialog, 1 for two-button 'OK' and 'Cancel'
; OUT: If two-button mode, AX = 0 for OK and 1 for cancel
; NOTE: Each string is limited to 40 characters

os_dialog_box:

	pusha
	mov [.tmp], dx
	call Cursor.Hide  ; os_hide_cursor
	mov dh, 9         ; First, draw red background box
	mov dl, 19

.redbox:				; Loop to draw all lines of box
	call Cursor.Move  ;os_move_cursor

	pusha
	mov ah, 09h
	mov bh, 0
	mov cx, 42
	mov bl, 01001111b		; White on red
	mov al, ' '
	int 10h
	popa

	inc dh
	cmp dh, 16
	je .boxdone
	jmp .redbox

.boxdone:

    ; String 1
	cmp ax, 0  ; Skip string params if zero
	je .no_first_string
	mov dl, 20
	mov dh, 10
	call Cursor.Move  ;os_move_cursor

	mov si, ax			; First string
	call String.Print  ;os_print_string

.no_first_string:

    ; String 2
	cmp bx, 0
	je .no_second_string
	mov dl, 20
	mov dh, 11
	call Cursor.Move  ;os_move_cursor

	mov si, bx			; Second string
	call String.Print  ;os_print_string

.no_second_string:

    ; String 3
	cmp cx, 0
	je .no_third_string
	mov dl, 20
	mov dh, 12
	call Cursor.Move  ;os_move_cursor

	mov si, cx			; Third string
	call String.Print  ;os_print_string

.no_third_string:

; How many buttons we want?
; 0 = 1 button | 1 = 2 buttons

	mov dx, [.tmp]
	cmp dx, 0
	je .one_button
	cmp dx, 1
	je .two_button

.one_button:
	mov bl, 11110000b		; Black on white
	mov dh, 14
	mov dl, 35
	mov si, 8
	mov di, 15
	call Window.DrawBlock  ;os_draw_block

	mov dl, 38			; OK button, centred at bottom of box
	mov dh, 14
	call Cursor.Move  ;os_move_cursor
	mov si, .ok_button_string
	call String.Print  ;os_print_string

    ; Vamos esperar por input pois 
    ; nesse dialogo só tem um botão.
    jmp .one_button_wait

.two_button:
	mov bl, 11110000b  ; Black on white
	mov dh, 14
	mov dl, 27
	mov si, 8
	mov di, 15
	call Window.DrawBlock  ;os_draw_block

	mov dl, 30			; OK button
	mov dh, 14
	call Cursor.Move  ;os_move_cursor
	mov si, .ok_button_string
	call String.Print  ;os_print_string

	mov dl, 44			; Cancel button
	mov dh, 14
	call Cursor.Move  ;os_move_cursor
	mov si, .cancel_button_string
	call String.Print  ;os_print_string

    ; RESULT:
    ; Default button = 0
    mov cx, 0

	; Depois de desenharmos 2 botões,
	; agora vamos esperar por input.
    jmp .two_button_wait

; -----------
; Esperando input no caso do diálogo só ter 1 botão.
.one_button_wait:

	call Io.WaitKey  ;os_wait_for_key

	cmp al, 13			; Wait for enter key (13) to be pressed
	jne .one_button_wait

	call Cursor.Show  ;os_show_cursor

	popa
    ; [enter]
    ; Sim, foi enter.
    ret

; -----------
; Esperando input no caso do diálogo ter 2 botões.
.two_button_wait:

    ; Wait for input.
    call Io.WaitKey  ;os_wait_for_key

    ; Left cursor key pressed?
    ; Se não foi o 'left', então vamos ver quem foi.
    ; E se foi?
	cmp ah, 75
	jne .noleft  ; Step1: Não foi left.

	mov bl, 11110000b		; Black on white
	mov dh, 14
	mov dl, 27
	mov si, 8
	mov di, 15
	call Window.DrawBlock  ;os_draw_block

	mov dl, 30			; OK button
	mov dh, 14
	call Cursor.Move  ;os_move_cursor
	mov si, .ok_button_string
	call String.Print  ;os_print_string

	mov bl, 01001111b		; White on red for cancel button
	mov dh, 14
	mov dl, 42
	mov si, 9
	mov di, 15
	call Window.DrawBlock  ;os_draw_block

	mov dl, 44			; Cancel button
	mov dh, 14
	call Cursor.Move  ;os_move_cursor
	mov si, .cancel_button_string
	call String.Print  ;os_print_string

    ; [<]
    ; sim, foi left.
    ; RESULT: '0'
    ; And update result we'll return
    mov cx, 0
    jmp .two_button_wait

; Se não foi o 'left', então vamos ver quem foi.
; E se foi?
.noleft:

    ; Right cursor key pressed?
    ; Se não foi o right, então quem foi?
	cmp ah, 77
	jne .noright  ; Step2: Também não foi foi right.

	mov bl, 01001111b		; Black on white
	mov dh, 14
	mov dl, 27
	mov si, 8
	mov di, 15
	call Window.DrawBlock  ;os_draw_block

	mov dl, 30			; OK button
	mov dh, 14
	call Cursor.Move  ;os_move_cursor
	mov si, .ok_button_string
	call String.Print  ;os_print_string

	mov bl, 11110000b		; White on red for cancel button
	mov dh, 14
	mov dl, 43
	mov si, 8
	mov di, 15
	call Window.DrawBlock  ;os_draw_block

	mov dl, 44			; Cancel button
	mov dh, 14
	call Cursor.Move  ;os_move_cursor
	mov si, .cancel_button_string
	call String.Print  ;os_print_string

    ; [>]
    ; Sim, foi right.
    ; RESULT: '1'
	mov cx, 1			; And update result we'll return
	jmp .two_button_wait

; Se não foi o right, então quem foi?
.noright:

    ; Wait for enter key (13) to be pressed
    ; Se não foi o enter, então quem foi?
	cmp al, 13
	jne .two_button_wait   ; Step3: Também não foi 'enter'.

	call Cursor.Show  ;os_show_cursor

	mov [.tmp], cx			; Keep result after restoring all regs
	popa

    ; [enter]
    ; Sim, foi o 'enter'.
    ; Então 'ax' contem o resultado.
    ; Put the result into the 'ax'
    mov ax, [.tmp]
    ret

; Local strings.
.ok_button_string       db 'OK', 0
.cancel_button_string   db 'Cancel', 0
.ok_button_noselect     db '   OK   ', 0
.cancel_button_noselect db '   Cancel   ', 0
; Local data.
.tmp dw 0



