;
; Arquivo: tasks.inc
;
; Descrição:
;     Theres no tasks on boot manager.
;
; Versão 1.0, 2015.
;

;#
;# task1
task0:
 	mov [0b8014h], byte 't'
	mov [0b8015h], byte 7
	mov [0b8016h], byte '0'
	mov [0b8017h], byte 7	
	jmp task0

;#
;# task1
task1:
 	mov [0b8014h], byte 't'
	mov [0b8015h], byte 7
	mov [0b8016h], byte '1'
	mov [0b8017h], byte 7
	jmp task1

;
;fim.
;
