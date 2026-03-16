;
; Arquivo: font32.inc
;
; Descrição:
;    Obs: Espaço reservado para uma font para 32 bit.
;    
; @todo: 
;    Usar essa fonte em 32 bits.
;
; Versão 1.0, 2015.
;


align 2
    db 'DEBUG: FONT 8x16 '
align 2


;
; Font info.
font_height: db 8
font_width:  db 16

;
; Font Base.
font_data:
;Font32_Base:
.z: dd 0	
	; @todo

	
; =======================
; EOF
