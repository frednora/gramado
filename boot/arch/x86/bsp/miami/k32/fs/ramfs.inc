;
; Arquivo: ramfs.inc
;
; Descrição:
;     Um pequeno suporte a um sistema de arquivos em ram. (virtual)
;     *** Isso é um teste.
;     Carrega uma partição
;
; Obs: 
;     Pode conter o núcleo e seus módulos. 
;     ( 128 setores cada modulo)
;
; partição 0 -  - lba 2048 até 4095. FAT16
; partição 1 - 4096 ...  RAMFS
;
; cada arquivo tem 64 k
; inicio do arquivo 0 =  (RAMFS_ADDRESS) + (0*128) /FILE 1
; inicio do arquivo 1 =  (RAMFS_ADDRESS) + (1*128) /FILE 2
; inicio do arquivo 2 =  (RAMFS_ADDRESS) + (2*128) /FILE 3
; inicio do arquivo 3 =  (RAMFS_ADDRESS) + (3*128) /FILE 4
; inicio do arquivo 4 =  (RAMFS_ADDRESS) + (4*128) /FILE 5
; ...
; version 1.0, 2015.
;


RAMFS_ADDRESS  EQU 0x00200000
RAMFS_LBA      EQU 4096
RAMFS_SIZE     EQU 2048


;------------------------------------
; ramfs_load_virtual_disk:
;     Carrega o disco virtual na memória em 0x8000.
;
ramfs_load_virtual_disk:
    
	;buffer
	mov eax, dword RAMFS_ADDRESS
	mov dword [.buffer], eax 
    
	;lba
	mov eax, dword RAMFS_LBA
	mov dword [.lba], eax 
	
	;prepara o contador
	mov ecx, dword RAMFS_SIZE   ; tamanho da partição
    ;coloca um setor no buffer.
.load_sector:		
	mov eax, dword [.buffer] ;buffer
	mov ebx, dword [.lba] ;lba
	call hd0_read_sector
    
	add dword [.buffer], 0200h
	add dword [.lba], 1
	
	loop .load_sector	
	
	mov esi, msg_disco_carregado
	call os_output
	ret
.buffer: dd 0	
.lba: dd 0



;ramfsInit:
;    nop
;	ret

;
; Mensagens.
;

msg_disco_carregado db "[ramfs:] Disco virtual carregado!", 13,10,0
	
;
; fim.
;	

	
	