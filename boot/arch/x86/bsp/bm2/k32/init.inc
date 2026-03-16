;
; Arquivo: init.inc
; Descrição:
;     Variáveis de inicialização dentro do Boot Manager.
;     Não são salvas no disco como no caso do META$FILE.
;     2015 - Fred Nora

;
; Boot support.
;

BOOT:
.modo:           dd 1              ;+0   (step 1) modo de inicialização do boot manager.
.disco_atual:    dd 0              ;+4   (step 2) disco atual 
.particao_atual: dd 0              ;+8   (step 3) partição atual 
.bl_cluster:     dd 2              ;+12  (step 4) cluster inicial do bl.bin 
.bl_address:     dd 0x00020000     ;+16  (step 5) endereço de bl.bin
.bl_entry_point: dd 0x00021000     ;+20  (step 6) entrypoint de bl.bin 
;status
.status_bm16:    dd 0              ;+24  (step 7) 1 = ouve mudanças de configuração
                                   ;              0 = nao houve mudanças.          
.status_bl32:    dd 0              ;+28  (step 8) 1 = reservado
                                   ;              0 = reservado.

;
; System files support.
;

SYSTEMCNF:
   .address dd 0x00100000      ;+32 
   .cluster dd 0               ;+36
INITCNF:
   .address dd 0x00100000      ;+40
   .cluster dd 1               ;+44
BLBIN:
   .address dd 0x00200000      ;+48
   .cluster dd 2               ;+52
BOOTDIR:
   .address dd 0x00300000      ;+56
   .cluster dd 3               ;+60
SYSTEMDIR:
   .address dd 0x00400000      ;+64
   .cluster dd 4               ;+68
USERDIR:
   .address dd 0x00500000      ;+72
   .cluster dd 5               ;+76


;
; Disk support.
;

DiscoAtivo:	
    .disk_number       dd 0 ;número do disco selecionado.
    .particao_ativa    dd 0 ;número da partição ativa.
    .partition_number  dd 0 ;número da partição selecionada.
    .flag_p0           db 0
    .flag_p1           db 0
    .flag_p2           db 0
    .flag_p3           db 0
    .p0_name           db "/VOLUME0"
    .p1_name           db "/VOLUME1"
    .p2_name           db "/VOLUME2"
    .p3_name           db "/VOLUME3"

;
; Partition support.
;

ParticaoAtiva:
.flag:      db 0

.h_inicial: db 0      
.c_inicial: db 0      
.s_inicial: db 0

.os_type:   db 0

.h_final:   db 0
.c_final:   db 0
.s_final:   db 0

.lba_inicial:
    db 0x00, 0x00, 0x00, 0x00 

.tamanho_da_particao:
    db 0x00, 0x00, 0x00, 0x00 

;
; File System support.
;

FILESYSTEM:
    .fs_volume db 0    ;Número do volume.   
    .fs_type   db 0    ;Tipo de sistema de arquivos.
    .fs_res1   db 0
    .fs_res2   db 0
    .fs_res3   db 0
    ;...

;
; Vídeo support.
;

VIDEO:
   .lfb:        dd 0
   .buffer1:    dd 0
   .buffer2:    dd 0
   .buffer3:    dd 0
   .res1        dd 0
   .res2        dd 0
   .res3        dd 0
   .res4        dd 0
   .res5        dd 0
   .res6        dd 0
   .res7        dd 0
   .res8        dd 0     

;
; End
;   
