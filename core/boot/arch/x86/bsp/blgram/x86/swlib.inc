; swlib.inc
; Support to sw.inc
; 2015 - Created by Fred Nora.

; #bugbug
; Muda o nome do arquivo para o formato 8.3

;
; Imported functions.
;

extern _vsync


SOFTWARELIB_BACKBUFFER EQU   (0x1000000 - 0x800000)


;---------------------------------------------
; _my_buffer_load_bitmap_16x16 
;    Decodifica um bitmap 16x16 e coloca em um buffer.
;    Obs: Talvez seja agrad�vel carregar um bitmat na inicializa��o.
;         Mudar o nome para softwarelibLoadOnBufferBitmap16x16:.
; a = endre�o da imagem 
; b = x
; c = y 
; d = null
;

global _my_buffer_load_bitmap_16x16     
_my_buffer_load_bitmap_16x16:
;softwarelibLoadOnBufferBitmap16x16

    mov dword [.endereco], eax
    mov dword [.x],        ebx
    mov dword [.y],        ecx

;Origem.   

    ;eax contem o inicio do bmp.
    add eax, dword 036h    ;Adiciona o offset da area de dados do bmp.	
    mov esi, eax           ;In�cio da area de dados da imagem.

;Destino.

	;Calcula x.
	mov eax, ebx        ;Pega x.
	mov ebx, dword 3    ;4 ;3 bytes por pixel.
	mul ebx
	mov ebx, eax      ; eax contem o deslocamento de x.
	
	;Calcula y e adiciona x.
	mov eax, ecx            ;Pega y.
	mov edx, dword 800*3    ;640*3  ;1024*4 ;640*4
	mul edx                 ;(numero de linhas x bytes por linha)   
    add eax, ebx            ;Adiciona x.	
    
    ;'eax', Cont�m o deslocamento em rela��o ao inicio do buffer.
	
	;mov edx, dword [5000h] ;lfb
    mov edx, dword SOFTWARELIB_BACKBUFFER ;0x01000000    ;buffer
	add eax, edx                 ;adiciona o buffer
    mov edi, eax                 ;posi��o (esquerda/cima) da imagem na tela.
	
	;sabendo a altura da imagem ...
	mov eax, edi
	add eax, dword 800*3*16    ;640*3*16 ;1024*4*16 ;640*4*16 ;(bytes por linha x altura).
	
	;'eax', Cont�m a posi��o '|____' .... (baixo/esquerda) da imagem.
	
	mov edi, eax

	;A sincroniza��o � feita antes de chamar essa fun��o.
	;call _vsync

    mov ecx, dword 16    ;Quantidade de linhas.

;Draw
.pinta_bmp:
	;Pega uma linha.
    movq  mm0, [esi]        ; 2 primeiros pixels.      
	movq  mm1, [esi+8]    
	movq  mm2, [esi+16]
	movq  mm3, [esi+24]
	movq  mm4, [esi+32]
	movq  mm5, [esi+40]
	;movq  mm6, [esi+48]
	;movq  mm7, [esi+56]
	
    movq  [edi]   , mm0
	movq  [edi+8] , mm1
	movq  [edi+16], mm2
	movq  [edi+24], mm3
	movq  [edi+32], mm4
	movq  [edi+40], mm5
	;movq  [edi+48], mm6
	;movq  [edi+56], mm7
    
	;proxima linha da bmp
	mov eax, esi 
	add eax, dword 48    ;64 ;adiciona 64 bytes.
	mov esi, eax
	
	;Linha anterior do buffer.
	mov eax, edi 
	sub eax, dword 800*3    ;640*3 ;1024*4 ;640*4 ;adiciona pixels.
	mov edi, eax
	
	loop .pinta_bmp
	emms               ;Isso � necess�rio.
	ret
;Nothing.
.endereco: dd 0
.x: dd 0
.y: dd 0


;==========================================
; _background:
;     Pinta um background diretamente na mem�ria de video.  
;     ( #teste N�o � recomendado escrever na memoria de video 
;       sem sincronizar o retra�o vertical) 
; @todo:
;     Est� escrevendo diretamente na mem�ria de v�deo. No endere�o do LFB 
; passado pelo bootmanager via argumento.
;     � necess�rio criar uma um endre�o l�gico para o endere�o f�sico
;do LFB.
; IN:
;     EAX = COR
;; r_background:

global _background
_background:
    ;nop
.fade_screen: 
 
	pushad
	
	;eax (cor)
	mov byte [.a], al    ;a
	shr eax, 8
	mov byte [.r], al    ;r
	shr eax, 8
	mov byte [.g], al    ;g
	shr eax, 8
	mov byte [.b], al    ;b

; Direto.
    call _vsync    ;Vertical sync.

;LFB.
    mov edx, DWORD [_g_lbf_pa]    ;; Endere�o f�sico do LFB. Passado pelo BM. 
    mov edi, edx

	;Contador de linhas
	mov ebx, dword 600    ;n�mero de linhas.
.bg_newline:     	
	;Contador de pixel por linha.
    mov ecx, dword (800*3)    ;(800*3). (800 pixel por linha, 3 bytes por pixel.).	
.bg_newpixel:         
	mov al, byte [.r]
	stosb 
	mov al, byte [.g]
	stosb 
	mov al, byte [.b]
	stosb    
	loop .bg_newpixel    ;Pr�ximo pixel. 
    dec   ebx         
	
	cmp ebx, dword 0
	jne .bg_newline    ;Pr�xima linha.
	;Nothing.
.exit_bg:
    popad
    RET
;Nothing.
.r db 0
.g db 0
.b db 0
.a db 0

;----------------------------------------------
; _gui_buffer_putpixel:
;     Coloca um pixel no buffer. 

global _gui_buffer_putpixel
_gui_buffer_putpixel:

    push es
    push eax
    push ebx
    push ecx

    ;@todo: Isso � um ajuste provis�rio.
	mov dword [.cor], eax    ;Salva cor.
	mov dword [.x],   ebx    ;Salva x.
	mov dword [.y],   ecx    ;Salva y.
    mov eax, dword [.x]
	mov ebx, dword [.y]      ;Coloca y em ebx.
	mov ecx, dword [.cor]    ;Coloca a cor em ecx.

	;ecx (cor)
	mov byte [.a], cl    ;a
	shr ecx, 8
	mov byte [.r], cl    ;r
	shr ecx, 8
	mov byte [.g], cl    ;g
	shr ecx, 8
	mov byte [.b], cl    ;b

;Segmento ES, poderia ser 0x10.??
    push eax
    mov ax, 0x10
    mov es, ax
    pop eax

	imul ebx, (800*3)           
    
	;eax*3
	mov ecx, dword 3
	mul ecx
	;adiciona ebx.
	add eax, ebx
	
	;dword [ebx+eax*3]  ;[ebx+eax*4] ; edi = x*4+(y*y multiplier)
	mov  edi, eax 
    
	;mov eax, ecx       ;[MenColor] ; eax = color
    
	;@todo: Mudar o buffer para um endere�o l�gico conhecido. 
	;(base) buffer 1 
	mov ecx, dword SOFTWARELIB_BACKBUFFER ;0x01000000    ;buffer ;[ModeInfo_PhysBasePtr]
    add edi, ecx     ;(deslocamento).
   	
	;envia r
	mov al, byte [.r]
	stosb 
	;envia g
	mov al, byte [.g]
	stosb 
	;envia b
	mov al, byte [.b]
	stosb 
    ;Nothing.
    pop ecx
    pop ebx
    pop eax
    pop es
    ret
;Nothing
.cor:  dd 0
.x:    dd 0
.y:    dd 0
.r     db 0
.g     db 0
.b     db 0
.a     db 0


;;==================================================================
;  _refresh_screen:
;      Passa o conte�do do Backbuffer para o LFB.
;      @todo: _softwarelibRefreshScreen:       
;

    ;; #bugbug
    ;; Temos o limite de 2m, pois somente mapeamos esse tanto.
    ;; 800x600x24bpp da menos de 2mb.
    ;; 800x600x32bpp da menos de 2mb.

;; Esse rotina faz refresh de 800x600x32bpp

global _refresh_screen
_refresh_screen:

    call _vsync    ;Vertical sync.

    push esi
    push edi
    push ecx
    push edx

    ;Origem, buffer.
    mov esi, dword SOFTWARELIB_BACKBUFFER  ;0x01000000 
    ;Destino, LFB, (no in�cio o bootloader lida com endere�os fisicos).
    mov edx, DWORD [_g_lbf_pa]    ;[bl_lfb] 
    mov edi, edx

    ;; Nunca mais do que isso.
 
    ;; #bugbug
    ;; Temos o limite de 2m, pois somente mapeamos esse tanto.
    ;; 800x600x24bpp da menos de 2mb.
    ;; 800x600x32bpp da menos de 2mb.

    mov ecx, dword (800*600)  
    rep movsd

    pop edx
    pop ecx
    pop edi
    pop esi
    RET



;--------------------------------------
; write_char:
;     Escreve um caractere na tela em modo texto.
;
write_char:

	push gs
	push ebx
	push eax
	
	;#18 - seletor para memoria de video
	mov bx, word 0x18
	mov gs, bx
	
	;# movl scr_loc, %bx
	mov ebx, dword [scr_loc]
	
	shl ebx, 1
	
	;#manda um char pra memoria de video
	mov [ebx], byte al
	shr ebx, 1
	inc ebx

	cmp ebx, dword 2000
	jb ._1f
	mov ebx, dword 0
._1f:
    mov dword [scr_loc], ebx
	pop eax
	pop ebx
	pop gs
	ret


;Reboot.
global _softwarelibReboot
_softwarelibReboot:
    ; Wait for an empty Input Buffer
	in al, 0x64
	test al, 00000010b
	jne _asm_shut_down
	; Send the reboot call to the keyboard controller
	mov al, 0xFE
	out 0x64, al
	jmp _asm_shut_down
	jmp $


;--------------------------------------------
; _asm_shut_down:
;     Rotina de reboot.
;     Obs: Substituida por _softwarelibReboot. 
;     @todo: Deletar isso.

global _asm_shut_down
_asm_shut_down:
    jmp _softwarelibReboot
    jmp $

;
; End.
;

