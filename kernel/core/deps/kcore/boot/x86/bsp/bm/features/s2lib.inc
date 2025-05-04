;
; File: lib16/s2lib.inc
;
;     Funções utilizadas pelo Stage 2.
;
; 2015 - Created by Fred Nora.
;


;-----------------------------------
; s2_FAILURE:
;     Alguma falha fatal no stage 2.
;

;s2libFailure:
s2_FAILURE:
    mov si, msg_s2_failure
    call DisplayMessage 
    xor ax, ax
    int 0x16        ;Await keypress.
    int 0x19        ;Reboot.


;; =========================
;M$ - Singularity	
;s2_InitializeVideo:

;        mov     ax, 1202h       ; LINES_400_CONFIGURATION
;        mov     bx, 0301h       ; SELECT_SCAN_LINE
;        int     10h

;        mov     ax, 3h          ; SET_80X25_16_COLOR_MODE
;        mov     bx, 0h          ; PAGE0
;        int     10h

;        mov     ax, 1112h       ; LOAD_8X8_CHARACTER_SET
;        mov     bx, 0h
;        int     10h

;        mov     ax, 1003h       ; Disable BLINK mode, enable background intensity.
;        mov     bx, 0h
;        int     10h

;        mov     ax, 0200h       ; Set Cursor position to 0, 0
;        mov     bx, 0h
;        mov     dx, 0h
;        int     10h

;       ret	


;; ==========================
; s2_turnoff_motor:
;     Desliga o motor do floppy disk.

s2_turnoff_motor:
    push ax
    push dx
    mov dx, 3F2h 
    mov al, 0 
    out dx, al 
    pop dx
    pop ax
    ret

;;==================================
; s2_setup_registers:
;     Configura os registradores.
;

s2_setup_registers: 
    cli
    ;; data
    mov  ax, DATA_SEGMENT
    mov  ds, ax
    mov  es, ax
    ;; stack
    mov ax, DATA_SEGMENT
    mov ss, ax
    mov sp, STACK_POINTER
    ;; general purpose
    xor ax, ax 
    xor bx, bx
    xor cx, cx
    xor dx, dx
    sti
    ret

;; ===========================
; DisplayMessage:
;     Mostra uma string.

;s2libDisplayMessage:

DisplayMessage:
    mov ah, 0x0E
    mov bh, 0x00
    mov bl, 0x07
.nextChar:
    lodsb
    or al, al
    jz .end
    int 0x10
    jmp .nextChar
.end:
    ret

;; alias.
PoeString:
    jmp DisplayMessage
    jmp $


;-----------------------------------
; String:
;     Rotinas de string.
;     Variável global.
;
;++
String:
;
; .Length - Return length of a string
; IN: AX = string location, OUT AX = length (other regs preserved)
;
.Length:               ;; comprimento da string
	pusha

	mov bx, ax		; Location of string now in BX
	mov cx, 0
.more:
	cmp byte [bx], 0	; Zero (end of string) yet?
	je .done
	inc bx			; If not, keep adding
	inc cx
	jmp .more	
.done:
	mov word [.tmp_counter], cx
	popa
	mov ax, [.tmp_counter]
	ret
	.tmp_counter:	dw 0
	
; 
; .Copy - Copy one string into another
; IN/OUT: SI = source, DI = destination
;
.Copy:
	pusha
.cmore:
	cmp byte [si], 0	; If source string is empty, quit out
	je .cdone
	mov al, [si]		; Transfer contents
	mov [di], al
	inc si
	inc di
	jmp .more
.cdone:
	mov byte [di], 0	; Write terminating zero
	popa
	ret
	
;
;.Printf - String em teletipo.
;
.Printf:	
    mov ah, 0x0E	
    mov bh, 0x00	
    mov bl, 0x07	
.proxCar:	
    lodsb		
    or al, al	
    jz .volta	
    int 0x10	
    jmp .proxCar	
.volta:
    ret	
	
;
; .TitleBar - Strings nas barras. in: bx
;
.TitleBar:
    push bx
	mov dh, 0
	mov dl, 1
	call Cursor.Move
	pop bx				; Get bottom string param
	mov si, bx
	pusha
	call Window.TitleBar
	popa
	pusha
	call String.Printf
	popa
    ret

;
; .MenuBar - Barra de menu. in: bx
;
.MenuBar:
    push bx
	mov dh, 1
	mov dl, 1
	call Cursor.Move
	pop bx				; Get bottom string param
	mov si, bx
	
	pusha
	call Window.MenuBar
	popa
	pusha
	call String.Printf
	popa
	ret
	
;
; .StatusBar - Barra de status. in: bx
;
.StatusBar:
    push bx
	mov dh, 23
	mov dl, 1
	call Cursor.Move
	pop bx				; Get bottom string param
	mov si, bx

	pusha
	call Window.StatusBar
	popa
	pusha
	call String.Printf
	popa
    ret
	
;
; .TaskBar - Barra de tarefas. in: bx
;
.TaskBar:
    push bx
	mov dh, 24
	mov dl, 1
	call Cursor.Move
	pop bx				; Get bottom string param
	mov si, bx
	
	pusha
	call Window.TaskBar
	popa
	pusha
	call String.Printf
	popa
    ret
;--


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Cursor - rotinas de cursor.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;++
Cursor:

; ------------------------------------------
; -- Moves cursor in text mode
; IN: DH, DL = row, column, OUT: Nothing (registers preserved)
.Move:
	pusha
	xor bh, bh
	mov ah, 2
	int 10h
	popa
	ret

; -----------------------------------------------------------------
; os_get_cursor_pos -- Return position of text cursor
; OUT: DH, DL = row, column
.GetPos:
	pusha
	xor bh, bh
	mov ah, 3
	int 10h
	mov [.tmp], dx
	popa
	
	mov dx, [.tmp]
	ret
    .tmp dw 0

; -----------------------------------------------------------------
; show -- Turns on cursor in text mode
; IN/OUT: Nothing!
.Show:
	pusha
	mov ch, 0			; Set cursor to solid block
	mov cl, 7
	mov ah, 1
	mov al, 3			; Must be video mode for buggy BIOSes!
	int 10h
	popa
	ret

; -----------------------------------------------------------------
; hide-- Turns off cursor in text mode
; IN/OUT: Nothing!
.Hide:
	pusha
	mov ch, 32
	mov ah, 1
	mov al, 3			; Must be video mode for buggy BIOSes!
	int 10h
	popa
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;--



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;; Window - rotinas de pintura de janelas.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;++
Window:

; -----------------------------------------------------------------
; clear_screen -- Clears the screen
; IN/OUT: Nothing (registers preserved)
.ClearScreen:
	pusha
	
	mov dx, 0			; Position cursor at top-left
	call Cursor.Move

	mov ah, 6			; Scroll full-screen
	mov al, 0			; Normal white on black
	mov bh, 7			; Upper-left corner of screen
	mov cx, 0			; Bottom-right
	mov dh, 24
	mov dl, 79
	int 10h

	popa
	ret


; -----------------------------------------------------------------
; draw_block -- Render block of specified colour
; IN: BL/DL/DH/SI/DI = colour/start X pos/start Y pos/width/finish Y pos
.DrawBlock:
	pusha
.more:
	call Cursor.Move	; Move to block starting position
	mov ah, 09h			; Draw colour section
	xor bh, bh
	mov cx, si
	mov al, ' '
	int 10h

	inc dh				; Get ready for next line

	xor ax, ax
	mov al, dh			; Get current Y position into DL
	cmp ax, di			; Reached finishing point (DI)?
	jne .more			; If not, keep drawing
	popa
	ret

;barra de título.
.TitleBar:
	mov dh, 0
	mov dl, 0
	call Cursor.Move
	mov ah, 09h			; Draw white bar at bottom
	xor bh, bh
	mov cx, 80
	mov bl, 01110000b
	mov al, ' '
	int 10h
    ret

;barra de menu.
.MenuBar:
	mov dh, 1
	mov dl, 0
	call Cursor.Move
	mov ah, 09h			; Draw white bar at bottom
	xor bh, bh
	mov cx, 80
	mov bl, 01110000b
	mov al, ' '
	int 10h
    ret

;barra de status.
.StatusBar:
	mov dh, 23
	mov dl, 0
	call Cursor.Move
	mov ah, 09h			; Draw white bar at bottom
	xor bh, bh
	mov cx, 80
	mov bl, 01110000b
	mov al, ' '
	int 10h
    ret

;barra de tarefas.
.TaskBar:
	mov dh, 24
	mov dl, 0
	call Cursor.Move
	mov ah, 09h			; Draw white bar at bottom
	xor bh, bh
	mov cx, 80
	mov bl, 01111111b
	mov al, ' '
	int 10h
    ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;--


;
;Draw_Line; 
;
;ah = color
;dh = coluna 
;dl = linha
;
Draw_Line:
	call Cursor.Move
	mov ah, 7h ;cor
	xor bh, bh
	mov cx, 80
	mov bl, 01110000b
	mov al, ' '
	int 10h    
	RET
	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;; Io - rotinas de input/output. 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;++
Io:

; -----------------------------------------------------------------
; .WaitKey - Waits for keypress and returns key
; IN: Nothing, OUT: AX = key pressed, other regs preserved
.WaitKey:
	pusha
	xor ax, ax
	mov ah, 00			; BIOS call to wait for key
	int 16h
	mov [.wk_tmp_buf], ax		; Store resulting keypress
	popa				; But restore all other regs
	mov ax, [.wk_tmp_buf]
	ret
	.wk_tmp_buf:	dw 0


; -----------------------------------------------------------------
;.PutCh - print character (int 21h, AH = 2h)
.PutCh:
	pusha
	mov al, dl
	mov ah, 0Eh		; int 10h teletype function
	int 10h
	mov [.pc_tmp], dl
	popa			; DOS expects AL = DL on exit
	mov al, [.pc_tmp]
	ret
    .pc_tmp: db 0

; -----------------------------------------------------------------
;.GetCh - get ch (int 21h, AH = 7h)
.GetCh:
	pusha
	call Io.WaitKey
	mov [.gc_tmp], al
	popa
	mov al, [.gc_tmp]
	ret
	.gc_tmp: db 0

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Device - rotinas relacionadas a dispositivos.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;++
Device:

;reboot system.
.Reboot:
    mov al, 0xFE
	out 0x64, al
	jmp $

; -----------------------------------------------------------------
; Reset floppy drive
.ResetFloppy:
	push ax
	push dx
	xor ax, ax
	mov dl, 0
	int 13h
	pop dx
	pop ax
	ret

; -----------------------------------------------------------------
; Reset floppy drive
.ResetHd:
	push ax
	push dx
	xor ax, ax
	mov dl, 80
	int 13h
	pop dx
	pop ax
	ret

; -----------------------------------------------------------------
; reset disk (int 21h, AH = 0Dh) , in: dl.
.ResetDisk:
	pusha
	xor ax,ax
	; mov dl, ?? (in)
	int 0x13
	popa
	ret
	
msg1_hd  db "", 0
msg2_hd  db "", 0

 
    
;
; End.
;	 
