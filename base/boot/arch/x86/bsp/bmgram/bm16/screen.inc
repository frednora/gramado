
; Display ASCIIZ string at "ds:si" via BIOS.
os_print_string:
    call String.Print
    ret

; -----------------------------------------------
; os_draw_background:
; Clear screen with white top and bottom bars
; containing text, and a coloured middle section.
; IN: 
; AX/BX = top/bottom string locations, CX = colour.

os_draw_background:
	pusha

	push ax				; Store params to pop out later
	push bx
	push cx

	mov dl, 0
	mov dh, 0
	call Cursor.Move

	mov ah, 09h			; Draw white bar at top
	mov bh, 0
	mov cx, 80
	mov bl, 01110000b
	mov al, ' '
	int 10h

	mov dh, 1
	mov dl, 0
	call Cursor.Move

	mov ah, 09h			; Draw colour section
	mov cx, 1840
	pop bx				; Get colour param (originally in CX)
	mov bh, 0
	mov al, ' '
	int 10h

	mov dh, 24
	mov dl, 0
	call Cursor.Move

	mov ah, 09h			; Draw white bar at bottom
	mov bh, 0
	mov cx, 80
	mov bl, 01110000b
	mov al, ' '
	int 10h

	mov dh, 24
	mov dl, 1
	call Cursor.Move
	pop bx				; Get bottom string param
	mov si, bx
	call String.Print

	mov dh, 0
	mov dl, 1
	call Cursor.Move
	pop ax				; Get top string param
	mov si, ax
	call String.Print

	mov dh, 1			; Ready for app text
	mov dl, 0
	call Cursor.Move

	popa
	ret
