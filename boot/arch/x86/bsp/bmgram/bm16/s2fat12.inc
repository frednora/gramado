;
; File: lib16/s2fat12.inc
;
; Descri��o:
;     Suporte ao sistema de arquivos fat12 para 16bit real mode.
; Obs: fat12 tamb�m pode ser usado em HDs, VHDs e outras m�dias, como 
; emuladores de floppy.
;
; History:
;     2013 - Created by Fred Nora.
;


;---------------------------------------------------------------------
; ReadSectors: 
;   
; Reads "cx" sectors from disk starting at "ax" into 
; memory location "es:bx".
; ~L� 'cx' setores do 'disco' come�ando em 'ax' e coloca na 
; mem�ria em 'es:bx'.
;---------------------------------------------------------------------

;s2fat12ReadSectors:
ReadSectors:
rmain:
    mov di, word 0x0005    ;five retries for error
.SECTORLOOP:
    push ax
    push bx
    push cx
    call LBACHS
    mov ax, 0x0201                 ;BIOS read sector. Read one sector. 
    mov ch, BYTE [absoluteTrack]   ;track
    mov cl, BYTE [absoluteSector]  ;sector
    mov dh, BYTE [absoluteHead]    ;head
    mov dl, BYTE [DriveNumber]     ;drive
.rSec:
    int 0x13                           ;invoke BIOS
    jc .rSec                           ;test for read error
.SUCCESS:
    call Window.StatusBar
    mov si, msgProgress
    call PoeString
    pop cx
    pop bx
    pop ax
    add bx, WORD [BytesPerSector]      ;queue next buffer
    inc ax                             ;queue next sector                                             
    loop rmain                          ;read next sector
    ret


;----------------------------------------------------------------------
; ClusterLBA - convert FAT cluster into LBA addressing scheme
;
; LBA = (cluster - 2) * sectors per cluster
;----------------------------------------------------------------------

ClusterLBA:
    sub ax, 0x0002                   ;zero base cluster number
    xor cx, cx
    mov cl, BYTE [SectorsPerCluster] ;convert byte to word
    mul cx
    add ax, WORD [datasector]        ;base data sector
    ret

;----------------------------------------------------------------------
; LBACHS - Convert LBA addressing scheme to CHS addressing scheme.
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  =  logical sector / (sectors per track * number of heads)
;-----------------------------------------------------------------------

LBACHS:
    xor dx, dx                       ;prepare dx:ax for operation
    div WORD [SectorsPerTrack]       ;'18' calculate
    inc dl                           ;adjust for sector 0
    mov BYTE [absoluteSector], dl
    xor dx, dx                       ;prepare dx:ax for operation
    div WORD [NumHeads]              ;calculate... '2'
    mov BYTE [absoluteHead], dl
    mov BYTE [absoluteTrack], al
    ret

;------------------------------------
; carregar_begin:
;     @todo: renomear.
;
;load_begin:
carregar_begin:
    ;;@todo , rever esse salvamento de cs:ip
    pop ax  
    mov word [salva_carregar_ip], ax
    pop ax  
    mov word [salva_carregar_cs], ax

    cli
    ;registradores
    xor ax, ax
    xor bx, bx
    xor cx, cx
    xor dx, dx
    ;segmentos
    mov ax, word 0x0000    ;segmento 0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ;pilha
    mov ax, word 0x0000
    mov ss, ax
    mov sp, word 0xFFFF
    sti
    call Device.ResetFloppy    ;reseta FDC.
    ;Carrega o arquivo.
elfLoadKernelImage32_sys: 

    push ax
	mov ax, word [carregar_nome]
	mov word [ImageNamePos], ax     ;; Atualiza o endere�o do nome do arquivo a ser lido    
	mov ax, word [carregar_seg]
	mov word [destino_seg] , ax     ;; Atualiza o Segmento que ser� carregado o arquivo
	mov ax, word [carregar_off]
	mov word [destino_off] , ax     ;; Atualiza o Offset que ser� carregado o arquivo
	pop ax
    ;Load root. 
LOAD_ROOT:
    ; compute 'size' of root directory and store in "cx"
    xor     cx, cx
    xor     dx, dx
    mov     ax, 0x0020            ;; 32 byte directory entry
    mul     WORD [MaxRootEntries] ;; 0x00E0 224   
    div     WORD [BytesPerSector] ;; 0x0200  
    xchg    ax, cx

    ; compute 'location' of root directory and store in "ax"
    ;     2*9+1='19'     
    mov     al, BYTE [TotalFATs]            ;numero de fats
    mul     WORD [SectorsPerFAT]            ;'vezes' os setores por FATs
    add     ax, WORD [ReservedSectors];; 1  ;'setores reservados'
    mov     WORD [datasector],ax            ;base of root directory                                  
    add     WORD [datasector],cx            ;add o 'size' = 14 ...
                   
    ;; read root directory into memory (7C00:0200)
    mov     bx, buffer     
    call    ReadSectors

    ; browse root directory for binary image
    mov     cx, WORD [MaxRootEntries]    ;; load loop counter
    mov     di, buffer                   ; locate first root entry
.LOOP:
    push    cx
    mov     cx, 0x000B                  ;; eleven character name
    mov     si, word [ImageNamePos]     ;;'ImageNamePos' comtem o endere�o do nome do arquivo 
    push    di
    rep  cmpsb                     ; test for entry match
    pop     di
    je      LOAD_FAT
    pop     cx
    add     di, 0x0020      ;; queue next directory entry
    loop    .LOOP
    jmp     FAILURE
    ;Load fat.
LOAD_FAT:
    mov  dx, WORD [di + 0x001A]   ;; di= 200+20+1a
    mov  WORD [cluster], dx       ;; file's first cluster
                  
    ;compute size of  FAT and store in "cx" 
    xor     ax, ax
    mov     al, BYTE [TotalFATs];; 
    mul     WORD [SectorsPerFAT];; 
    mov     cx, ax      
 
    ; compute location of FAT and store in "ax"
    mov     ax, WORD [ReservedSectors]   ; adjust for bootsector
                                           
    ; read FAT into memory (7C00:0200)
    mov     bx, buffer      ; copy FAT above bootcode
    call    ReadSectors

    ; read image file into memory ;; 'es:bx' (0050:0000)

    ; #obs: 
    ; Os segmentos sar�o atualizados para carregar o arquivo, depois de 
    ; carregado o arquivo, os segmentos precisam voltar pra zero.
    
    mov   ax, word [destino_seg]    ;Destination for image.
    mov   es, ax                       
    mov   bx, word [destino_off]    ;Destination for image.
    push  bx
    ;Load image.
LOAD_IMAGE:
    mov  ax, WORD [cluster] ; cluster to read
    pop  bx                 ; buffer to read into
    call  ClusterLBA         ; convert cluster to LBA
	xor  cx, cx
    mov  cl, BYTE [SectorsPerCluster]  ; sectors to read
    call ReadSectors
    push  bx
    ;Compute next cluster.
    mov  ax, WORD [cluster]    ; identify current cluster
    mov  cx, ax                ; copy current cluster
    mov  dx, ax                ; copy current cluster
    shr  dx, 0x0001            ; divide by two
    add  cx, dx                ; sum for (3/2)
    mov  bx, buffer            ; location of FAT in memory
    add  bx, cx                ; index into FAT    
    mov  dx, WORD [bx]         ; read two bytes from FAT
    test  ax, 0x0001
    jnz  .ODD_CLUSTER
.EVEN_CLUSTER:
    and  dx, 0000111111111111b    ;Take low twelve bits.
    jmp  .DONE
.ODD_CLUSTER:
    shr  dx, 0x0004               ;Take high twelve bits.
.DONE:
   mov  WORD [cluster], dx        ;Store new cluster.
   cmp  dx, 0x0FF0                ;Test for end of file.
   jb  LOAD_IMAGE  
carregar_end:
    ;Post message.
    pusha
    call Window.ClearScreen          
    call Window.StatusBar 
    mov si, word [carregar_msg]  
    call PoeString
    popa

    ; Retornando de forma segura, usando cs:ip salvos.
    mov ax, word [salva_carregar_cs]
    push ax
    mov ax, word [salva_carregar_ip]
    push ax
    xor ax, ax
    retf

;----------------------------	
; FAILURE: 
;     Erro fatal, abortar.

FAILURE:
    call Window.StatusBar
    mov si, msgFailure
    call PoeString
    xor ax,ax
    int 0x16
    int 0x19

;Vari�veis.
carregar_nome: dw 0    ;Nome.
carregar_seg:  dw 0    ;Segmento.
carregar_off:  dw 0    ;Offset.
carregar_msg:  dw 0    ;Mensagem.
;Mensagens.
msg_debug_root db "fat12: 1 root",0
msg_debug_fat  db "fat12: 2 fat",0
msg_debug_img  db "fat12: 3 image",0
msg_debug_end  db "fat12: 4 Ok",0

;; ===================================================
; buffer: 
;     BUFFER, usado para carregar FAT e ROOTDIR.
;     @todo: Rever esse buffer, precisamos de espa�o no boot manager.

buffer:       
    times (512*2) db 0
;
; End.
;
