;
; File: screen32.inc 
;
;     Screen
;
; ================================================================
; BareMetal -- a 64-bit OS written in Assembly for x86-64 systems
; Copyright (C) 2008-2013 Return Infinity -- see LICENSE.TXT
;
; Screen Output Functions
; =============================================
;
; History:
;     2015.
;


align 1
    db 'SCREEN'
align 1


;===================================================================
; os_inc_cursor:
;     Increment the cursor by one, scroll if needed.
;
;  IN:	Nothing
; OUT:	All registers preserved

os_inc_cursor:

	push eax

	;Incrementa coluna.
	add word [os_Screen_Cursor_Col], 1
	mov ax, [os_Screen_Cursor_Col]
	
	;Compara com a largura, se n�o for igual, n�o h� o que fazer.
	cmp ax, [os_Screen_Cols]
	jne os_inc_cursor_done
	
	;Caso o n�mero da coluna for igual � largura da tela
	;voltaremos para a coluna '0', e saltaremos para a pr�xima linha.
	mov word [os_Screen_Cursor_Col], 0
	add word [os_Screen_Cursor_Row], 1
	
	;Trataremos a linha em que o cursor se encontra.
	mov ax, [os_Screen_Cursor_Row]
	
	;Se a linha que cursor se encontra n�o for igual ao total de linhas
	;dispon�vel, ent�o n�o h� o que fazer.
	cmp ax, [os_Screen_Rows]
	jne os_inc_cursor_done
	
	;;Caso a linha que o cursor se encontra for igual ao total
	;;de linhas da tela, chamamos o scroll.
	call os_screen_scroll
	
	;Decrementamos o n�mero da linha que o cursor se encontra.
	sub word [os_Screen_Cursor_Row], 1
os_inc_cursor_done:
	pop eax
	ret
;-------------------------------------------------------


;===============================================================
; os_dec_cursor: 
;     Decrement the cursor by one.
;
;  IN:	Nothing
; OUT:	All registers preserved
os_dec_cursor:
	push eax
	cmp word [os_Screen_Cursor_Col], 0
	jne os_dec_cursor_done
	sub word [os_Screen_Cursor_Row], 1
	mov ax, [os_Screen_Cols]
	mov word [os_Screen_Cursor_Col], ax
os_dec_cursor_done:
	sub word [os_Screen_Cursor_Col], 1
	pop eax
	ret
;-----------------------------------------------------


;======================================================
; reset_cursor:
;     Posiciona o cursor no in�cio da tela.
;
reset_cursor:
    mov word [os_Screen_Cursor_Col], 0
    mov word [os_Screen_Cursor_Row], 0
    ret


;----------------------------------------------------
; os_print_newline:
;     Reset cursor to start of next line and scroll if needed.
;
;  IN:	Nothing
; OUT:	All registers perserved

os_print_newline:
    push eax

    mov word [os_Screen_Cursor_Col], 0
	mov ax, [os_Screen_Rows]
	sub ax, 1
	
	cmp ax, [os_Screen_Cursor_Row]
	je os_print_newline_scroll
	
	add word [os_Screen_Cursor_Row], 1
	jmp os_print_newline_done
	
os_print_newline_scroll:
    call os_screen_scroll
os_print_newline_done:
    pop eax
    ret
;------------------------------------------------------


;----------------------------------------------------
; os_output:
;     Displays text.
;  IN:	RSI = message location (zero-terminated string)
; OUT:	All registers perserved

os_output:
    ;nop
;screen32DisplayMessage:
	push ecx
	call os_string_length
	call os_output_chars
	pop ecx
	ret
; ----------------------------------------------------------------------


; ----------------------------------------------------------------------
; os_output_char -- Displays a char
;  IN: AL  = char to display
; OUT: All registers perserved

os_output_char:

	push edi
	push edx
	push ecx
	push ebx
	push eax

	cmp byte [os_VideoEnabled], 1
	je os_output_char_graphics

os_output_char_text:

; Store the attribute into AH so STOSW can be used later on
    mov ah, 0x07

	push eax
	mov ax, [os_Screen_Cursor_Row]
	and eax, 0x0000FFFF	; only keep the low 16 bits
	mov cl, 80			; 80 columns per row
	mul cl				; AX = AL * CL
	mov bx, [os_Screen_Cursor_Col]
	add ax, bx
	shl ax, 1			; multiply by 2

; Address of the screen buffer
; Base + offset.
    mov edi, 0xB8000
    add edi, eax

    pop eax

; Write the character and attribute with one call.
    stosw

    jmp os_output_char_done

os_output_char_graphics:
	mov ebx, [os_Font_Color]
	and eax, 0x000000FF
	call os_glyph_put

os_output_char_done:

	call os_inc_cursor

	pop eax
	pop ebx
	pop ecx
	pop edx
	pop edi
	ret
; ----------------------------------------------------------------------



;; ---------------------------------------------------------------------
;; os_pixel 
;;     Put a pixel on the screen
;;
;; IN:	
;;     EBX = Packed X & Y coordinates (YYYYXXXX)
;;	   EAX = Pixel Details (AARRGGBB)
;; OUT:	
;;     All registers preserved
;;

os_pixel:
	push edi
	push edx
	push ecx
	push ebx
	push eax

	push eax			; Save the pixel details
	mov eax, ebx
	shr eax, 16			; Isolate Y coord
	xor ecx, ecx
	mov cx, [os_VideoX]
	mul ecx				; Multiply Y by os_VideoX
	and ebx, 0x0000FFFF		; Isolate X coord
	add eax, ebx			; Add X
	mov edi, [os_VideoBase]

	cmp byte [os_VideoDepth], 32
	je os_pixel_32

os_pixel_24:
	mov ecx, 3
	mul ecx				; Multiply by 3 as each pixel is 3 bytes
	add edi, eax			; Add offset to pixel video memory
	pop eax				; Restore pixel details
	stosb
	shr eax, 8
	stosb
	shr eax, 8
	stosb
	jmp os_pixel_done

os_pixel_32:
	shl eax, 2			; Quickly multiply by 4
	add edi, eax			; Add offset to pixel video memory
	pop eax				; Restore pixel details
	stosd

os_pixel_done:
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop edi
	ret
; ----------------------------------------------------------------------


; ----------------------------------------------------------------------
; os_glyph_put -- Put a glyph on the screen at the cursor location
;  IN:	EAX = Glyph
;	EBX = Color (AARRGGBB)
; OUT:	All registers preserved
os_glyph_put:
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax

	and eax, 0x000000FF
	sub eax, 0x20
	mov ecx, 12		; Font height
	mul ecx
	mov esi, font_data
	add esi, eax		; add offset to correct glyph

; Calculate pixel co-ords for character
	xor ebx, ebx
	xor edx, edx
	xor eax, eax
	mov ax, [os_Screen_Cursor_Row]
	mov cx, 12		; Font height
	mul cx
	mov bx, ax
	shl ebx, 16
	xor edx, edx
	xor eax, eax
	mov ax, [os_Screen_Cursor_Col]
	mov cx, 6		; Font width
	mul cx
	mov bx, ax

	xor eax, eax
	xor ecx, ecx		; x counter
	xor edx, edx		; y counter

nextline1:
	lodsb			; Load a line

nextpixel:
	cmp ecx, 6		; Font width
	je bailout		; Glyph row complete
	rol al, 1
	bt ax, 0
	jc os_glyph_put_pixel
	push eax
	mov eax, 0x00000000
	call os_pixel
	pop eax
	jmp os_glyph_put_skip

os_glyph_put_pixel:
	push eax
	mov eax, [os_Font_Color]
	call os_pixel
	pop eax
os_glyph_put_skip:
	add ebx, 1
	add ecx, 1
	jmp nextpixel

bailout:
	xor ecx, ecx
	sub ebx, 6		; column start
	add ebx, 0x00010000	; next row
	add edx, 1
	cmp edx, 12		; Font height
	jne nextline1

glyph_done:
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	ret
; ---------------------------------------------------


;; -------------------------------------------------
;; os_output_chars
;;     Displays text
;;
;; IN:	
;;     ESI = message location (A string, not zero-terminated)
;;     ECX = number of chars to print
;; OUT:	
;;     All registers perserved
;;

os_output_chars:
	push edi
	push esi
	push ecx
	push eax

    ; Clear the direction flag.. we want to increment through the string
    ; Store the attribute into AH so STOSW can be used later on

    cld 
    mov ah, 0x07 

os_output_chars_nextchar:
	jecxz jmp_to_os_output_chars_done
	sub ecx, 1
	lodsb				; Get char from string and store in AL
	cmp al, 13			; Check if there was a newline character in the string
	je os_output_chars_newline	; If so then we print a new line
	cmp al, 10			; Check if there was a newline character in the string
	je os_output_chars_newline	; If so then we print a new line
	cmp al, 9
	je os_output_chars_tab
	call os_output_char
	jmp os_output_chars_nextchar
	
;como jump short nao tava funcionando
jmp_to_os_output_chars_done:
    jmp os_output_chars_done

os_output_chars_newline:
	mov al, [esi]
	cmp al, 10
	je os_output_chars_newline_skip_LF
	call os_print_newline
	jmp os_output_chars_nextchar

os_output_chars_newline_skip_LF:
	cmp ecx, 0
	je os_output_chars_newline_skip_LF_nosub
	sub ecx, 1
os_output_chars_newline_skip_LF_nosub:
	add esi, 1
	call os_print_newline
	jmp os_output_chars_nextchar	

os_output_chars_tab:
	push ecx
	mov ax, [os_Screen_Cursor_Col]	; Grab the current cursor X value (ex 7)
	mov cx, ax
	add ax, 8     ; Add 8 (ex 15)
	shr ax, 3     ; Clear lowest 3 bits (ex 8)
	shl ax, 3     ; Bug? 'xor al, 7' doesn't work...
	sub ax, cx    ; (ex 8 - 7 = 1)
	mov cx, ax
	mov al, ' '
os_output_chars_tab_next:
	call os_output_char
	sub cx, 1
	cmp cx, 0
	jne os_output_chars_tab_next
	pop ecx
	jmp os_output_chars_nextchar

os_output_chars_done:
	pop eax
	pop ecx
	pop esi
	pop edi
	ret
; ----------------------------------------------------------------


;=================================================================
; os_scroll_screen -- Scrolls the screen up by one line
;  IN:	Nothing
; OUT:	All registers perserved
os_screen_scroll:
	push esi
	push edi
	push ecx
	push eax

    ; Clear the direction flag as we want to increment through memory

    cld

    cmp byte [os_VideoEnabled], 1
    je os_screen_scroll_graphics

os_screen_scroll_text:
	mov esi, 0xB80A0 		; Start of video text memory for row 2
	mov edi, 0xB8000 		; Start of video text memory for row 1
	mov ecx, 1920			; 80 x 24
	rep movsw			; Copy the Character and Attribute
	; Clear the last line in video memory
	mov ax, 0x0720			; 0x07 for black background/white foreground, 0x20 for space (black) character
	mov edi, 0xB8F00
	mov ecx, 80
	rep stosw			; Store word in AX to RDI, RCX times
	jmp os_screen_scroll_done

os_screen_scroll_graphics:
	xor esi, esi
	xor ecx, ecx
	mov edi, [os_VideoBase]
	mov esi, [os_Screen_Row_2]
	add esi, edi
	mov ecx, [os_Screen_Bytes]
	rep movsb

os_screen_scroll_done:
	pop eax
	pop ecx
	pop edi
	pop esi
	ret
; ------------------------------------------


; ------------------------------------------
; os_screen_clear -- Clear the screen
;  IN:	Nothing
; OUT:	All registers perserved
os_screen_clear:
	push edi
	push ecx
	push eax

	xor ecx, ecx
	cmp byte [os_VideoEnabled], 1
	je os_screen_clear_graphics
os_screen_clear_text:
	mov ax, 0x0720		; 0x07 for black background/white foreground, 0x20 for space (black) character
	mov edi, 0xB8000	; Address for start of color video memory
	mov cx, 2000
	rep stosw		; Clear the screen. Store word in AX to RDI, RCX times
	jmp os_screen_clear_done
os_screen_clear_graphics:
	mov edi, [os_VideoBase]
	xor eax, eax
	mov ecx, [os_Screen_Bytes]
	rep stosb
os_screen_clear_done:
	pop eax
	pop ecx
	pop edi
	ret
; -------------


;-------------------------------------
; boot_set_cursor:
;     Set cursor position (text mode 80x25).
; @param BL The row on screen, starts from 0
; @param BH The column on screen, starts from 0
;=============================================
boot_set_cursor: 

    pushfd   
    push eax
    push ebx
    push ecx
    push edx
 
    ;unsigned short position = (row*80) + col;
    ;AX will contain 'position'
    mov ax, bx
    and ax, 0ffh             ;set AX to 'row'
    mov cl, 80   
    mul cl                  ;row*80
 
    mov cx, bx               
    shr cx, 8                ;set CX to 'col'
    add ax, cx               ;+ col
    mov cx, ax               ;store 'position' in CX
 
    ;cursor LOW port to vga INDEX register
    mov al, 0fh             
    mov dx, 3d4h             ;VGA port 3D4h
    out dx, al             
 
    mov ax, cx               ;restore 'postion' back to AX  
    mov dx, 3d5h             ;VGA port 3D5h
    out dx, al               ;send to VGA hardware
 
    ;cursor HIGH port to vga INDEX register
    mov al, 0eh
    mov dx, 3d4h             ;VGA port 3D4h
    out dx, al
 
    mov ax, cx               ;restore 'position' back to AX
    shr ax, 8                ;get high byte in 'position'
    mov dx, 3d5h             ;VGA port 3D5h
    out dx, al               ;send to VGA hardware
 
    pop edx
    pop ecx
    pop ebx
    pop eax
    popfd
    ret


;;===========================================================
; print_char:
;     Write a ASCII character to the console.
;
; Arguments:
;   AL = ASCII character.
;

print_char:
    ;nop
;screen32PrintChar:
    pushad
    call os_output_char
    popad
    ret


;------------------------------------------------------------------
; print_spaces:
;      Write a variable number of spaces to the console.
;
; Arguments:
;   AL = number to spaces.
;

print_spaces:
    ;nop
;screen32PrintSpaces:
    pushad
    xor    cx, cx
    mov    cl, al     ; use CX as the loop counter
    mov    al, ' '    ; character to print
.loop:
    jcxz    .exit
    call    print_char
    loop    .loop
.exit:
    popad
    ret


;;======================================================
; Write the byte value to the console in hex.
;
; Arguments:
;   AL = Value to be displayed in hex.
;

print_hex:
    ;nop
;screen32PrintHex:
    pushad
    mov     cx, WORD 4
    bswap   eax
.looph:
    push    ax
    ror     al, 4
    call    print_nibble    ; display upper nibble
    pop     ax
    call    print_nibble    ; display lower nibble
    ror     eax, 8
    loop    .looph

    ;mov     al, 10         ; carriage return
    ;call    print_char
    ;mov     al, 13
    ;call    print_char
    popad
    ret


print_nibble:
    and  al, 0x0f
    add  al, '0'
    cmp  al, '9'
    jna  .print_ascii
    add  al, 'A' - '9' - 1    ;; #bugbug: nasm only?
.print_ascii:
    call    print_char
    ret

; =============================================================================
; EOF

