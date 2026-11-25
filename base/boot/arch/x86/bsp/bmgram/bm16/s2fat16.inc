;
; File: lib16/s2fat16.inc
;
; Descrição:
; Suporte à leitura de arquivo no sistema FAT16.
; O ambiente é modo real 16 bit.
; *IMPORTANTE: É FUNDAMENTAL A POSSIBILIDADE DE CARREGAR ARQUIVOS USANDO BIOS 
; 16BIT. PORÉM TEMOS A LIMITAÇÃO DE 1MB.
; *Pois SATA e USB são coisas complicadas.
;
; In this file:
;     + s2fat16ReadFile
;     + s2fat16LOADIMAGE
;     + s2fat16ReadSectors
;
; 2015 - Created by Fred Nora.
;


;;
;; Salva o endereço do nome do arquivo a ser carregado.
;;
s2fat16_image_address: 
    dw 0
s2fat16_fat16_cluster:
    dw 0
s2fat16_fat16_datasector:
    dw 0
s2_fat16_Selected_Drive_Number:
    db 0
;; ...


;; ==================================================================
;; s2fat16ReadFile:
;;     Loads a file using fat16.    
;;     PS: We are in 16 bit real mode.
;;     PS: The readsector functions is implemented in this file. 
;;
;; IN:
;;     AX = ??
;;     We just need a pointer to the name of the file.
;;     "BL      BIN"
;;
;; @todo: Tentando portar do módulo em modo protegido para o modo real. 
;; @todo: refazer as mensagens para essa rotina.
;; @todo: substituir todas as chamadas à rotina de printstring 
;; pela rotina de printstring apropriada em 16bit.
;;
;; Status: Essa rotina esta funcionando parcialmente.
;; @todo: Fazer debug dessa rotina e corrigir as partes 
;; que não são eficazes.
;;

s2fat16ReadFile:

    pusha 

    ;Salva o endereço do nome do arquivo.
    mov word [s2fat16_image_address], ax 

	;;
	;; @todo:
	;; Imperativo. Estamos testando se é HD MASTER.
	;; Suportamos apenas o HD master por enquanto. 
	;; Talvez devessemos considerar as opções de disco.
	;;
.checkDiskNumber:
    ;Confere o número do disco.
    ;@todo: Trocar pelo bpb do stage 2.

    ;hd slave
    ;cmp byte [fat16_bpb.DriveNumber], byte 0x81
    ;jne .fail

    ;hd.master
    cmp byte [fat16_bpb.DriveNumber], byte 0x80
    jne .fail

    ;; ...

.setupRegisters:
    xor ax, ax  
    xor bx, bx
    xor cx, cx
    xor dx, dx

    ;Message.
    ;mov si, msg_s2fat16_Loading  
    ;call DisplayMessage

    ;1 - Calcula o início da área de dados.
.getDataAreaInit:

    ; Message.
    mov si, msg_s2fat16_step1  
    call DisplayMessage
    call s2fat16_get_dataarea_init

    ;2 - Carrega o rootdir.
.loadRoot:
	;Message.
    mov si, msg_s2fat16_step2
    call DisplayMessage
    call s2fat16LOADROOT

    ;3 - Procura um nome de arquivo no diretório raiz.	
.seachFileName:
	;Message.
    mov si, msg_s2fat16_step3  
    call DisplayMessage

    mov ax, word [s2fat16_image_address]
    call s2fat16SearchFileName
    jc  .fail                       ;@todo: Erro. 
	
    mov word [.cluster], ax          ;Salva cluster inicial.


    ;4 - Carrega a FAT. 
	

    ;Message.
    mov si, msg_s2fat16_step4  
    call DisplayMessage	
	
	;load FAT.
.loadFAT:
    call s2fat16LOADFAT

    ;5 - Carrega o arquivo.	
	
	;Message.
	;mov si, msg_fat16_step5
    ;call DisplayMessage	
	
    ;Message.
	;mov si, msg_s2fat16_step5  
    ;call DisplayMessage	
	
    ;Load Image.	
.loadImage:
    mov ax, word [.cluster]
    call s2fat16LOADIMAGE

	;*Se funcionou.
.fat16_read_DONE:

	;Message.
	;mov si, msg_s2fat16_Done
    ;call DisplayMessage	
    
    ;Message.
	;mov si, msg_s2fat16_Done   
    ;call DisplayMessage	

	;
    ; Olhar o nome do arquivo carregado!
    ;
	
	;Message.
	;mov si, S2_FAT16_IMAGE_ADDRESS
    ;call DisplayMessage	
    
	;Message.
	;mov si, S2_FAT16_IMAGE_ADDRESS ;@todo.  
    ;call DisplayMessage	

    ;popad
    ret    ;Retorna para quem chamou.
    ;; *FAIL !!
.fail:

    ;Message.
    mov si, msg_s2fat16_diskfail
    call DisplayMessage

	;#Debug
	;jmp $
	
    popa
    stc
    ret
.cluster: dw 0


;=================================================
; s2fat16_get_dataarea_init: 
;     Calcular o início da área de dados.
;     *IMPORTANTE.
;
s2fat16_get_dataarea_init:

    pusha
    xor ax, ax  
    xor bx, bx
    xor cx, cx
    xor dx, dx
	
	;Calcula o tamanho do diretório raiz. 
    ;( 32 * 512 ) / 512
    ; quantidade de bytes / bytes por setor = número de setores.    
	
    xor ax, ax 
    xor cx, cx
    mov ax, word 32                           ;32 bytes por entrada do diretório.         
    mul WORD [fat16_bpb.MaxRootEntries]       ;512 - Número máximo de entradas.
    div WORD [fat16_bpb.BytesPerSector]       ;512 - bytes por setor.
    xchg ax, cx                               ;Salva o tamanho do diretório.

	;32
	
	;calcula a localização do diretorio raiz. ;(2*254) + (setores reservados) = "ax".	
    xor ax, ax 
    mov al, BYTE [fat16_bpb.TotalFATs]        ;2   - numero de fats.
    mul WORD [fat16_bpb.SectorsPerFAT]        ;246 - setores por FATs.
    add ax, WORD [fat16_bpb.ReservedSectors]  ;3   - setores reservados.
    add ax, 1                                 ;Quantidade de setores do VBR. (1) 
    add ax, word FAT_LBA                ;63  - início da partição.

	;Construindo o valor de 'fat16_datasector', o inicio da area de dados. 
    mov WORD [fat16_datasector], ax           ;coloca 2 fats + setor reservado em 'ax'          
    add WORD [fat16_datasector], cx           ;adiciona o tamanho do diretorio raiz. 

    ; #obs:
    ; O valor de data sector esta salvo em 'fat16_datasector'.

    popa
    ret


;;==============================================================
;; Devemos portar a versão de 32bit para 16bit.
; IN: 
;     AX = Nome do arquivo.
; OUT: 
;     AX = Cluster.
s2fat16SearchFileName:

    pusha
    mov word [.name], ax    ;Save file name.
	
	;configurando a base.
    xor ax, ax
    mov ax, ROOT_SEGMENT  ;base para mbr,vbr,fat,root,data area. no stage 2.
    mov es, ax
	
	;Load Root Dir.
    call s2fat16LOADROOT

    xor cx, cx
    mov cx, WORD 512                ;16 ;[fat16_bpb.MaxRootEntries] contador de entradas  
	;@todo: isso é apenas o offset a base deve ser 7000h para teste.
    mov di, ROOT_OFFSET  ;inicio do diretorio   

;++
.loop_search_file:
    push cx
    mov cx, 0x000B            ;Eleven character name.
    
    mov si, word [.name]    
    push di
    rep cmpsb                  ;Test for entry match. 
    pop di
    je .exit_arquivo_encontrado

    pop cx
    add di, 0x0020            ;Queue next directory entry.	
    LOOP .loop_search_file
;--

    ;mov esi, msg_file_not_found
	;call os_output

	;Message.
	;mov esi, msg_file_not_found   
	;call lib32PrintString

	;jmp $
.fail:

    mov si, msg_s2fat16_Failure 
    call DisplayMessage

    popa
    stc
    ret

.sf_root_fail:

    mov si, msg_s2fat16_sf_root_fail
    call DisplayMessage

    popa
    stc
    ret

.exit_arquivo_encontrado:
    pop cx    ;Faltava o contador.

    mov si, msg_s2file_ok  
    call DisplayMessage

	;pega o valor do primeiro cluster do arquivo.
    xor dx, dx
    mov dx, WORD [di + 0x001A]   ;di= 200+20+1a.
    mov WORD [s2fat16_fat16_cluster], dx  ;file's first cluster.

    popa

	;retorna cluster.
    mov ax, WORD [s2fat16_fat16_cluster]
    clc
    ret

.name: dw 0


;;==============================================================
;; s2fat16LOADROOT:
;;     Carregar o diretório razi na memória.
;;     Na verdade estamos carregando apenas o primeiro setor.
;;

s2fat16LOADROOT:
    ;nop
    call s2_fat16_load_root_first_sector
    ret


;==================================================
; load_root_first_sector:
;     Carrega o primeiro setor do root no buffer.
;

s2_fat16_load_root_first_sector:

    ;cmp byte [s2_fat16_Selected_Drive_Number], 1
    ;jne .not_slave	
.is_slave:

	;Select and initialize slave.
    ;mov al, 1
    ;call select_hd
    
	;jmp .go
	
.not_slave:
    ;Select and initialize master.
    ;mov al, 0
    ;call select_hd
.go:	
	
    ;@todo: Chamar uma rotina do BIOS que inicialize o dispositivo em questão. 	
	;call init_ide0_master
	
    pusha
	
	;push ax
	;push bx
	;push cx
	
	
	;Quantidade
	mov cx, 1
	
	;buffer. (es:bx)
	xor ax, ax
	mov ax, ROOT_SEGMENT  ;base para mbr,vbr,fat,root,data area. no stage 2.
	mov es, ax
	mov bx, word ROOT_OFFSET	

	;LBA a se lido.
    mov ax, word ROOT_LBA        ;lba - (root dir) 

    ;Carregar apenas um setor na memória. O primeiro setor do diretório raiz.
    ; cx = Quantidade de setores a serem lidos.
    ; ax = Número do primeiro setor a ser lido.
    ; es:bx = Buffer. Localização na memória onde serão carregados os setores.	
    ;call hd0_read_sector
    call s2fat16ReadSectors

	;pop cx
	;pop bx
	;pop ax

    popa
    ret

;@todo: Checar se essa mensagem está sendo usada.	
;msg_s2fat16_load_root_fail db "load_root_first_sector fail: Dir.",13, 10,0	



;; ==============================================================
;; s2fat16LOADFAT:
;;     Carrega a fat na memória.
;;

s2fat16LOADFAT:
	
    ;@todo: chamar rotina do BIOS para inicializar o dispositivo.	
	;call init_ide0_master    ;reset ide.
	
    pusha

	push ax
	;buffer. (es:bx)
	xor ax, ax
	mov ax, FAT_SEGMENT  ;base para mbr,vbr,fat,root,data area. no stage 2.
	mov es, ax
	mov bx, word FAT_OFFSET
	mov word [.address], bx	
	mov ax, FAT_LBA 
	mov word [.lba], ax
	pop ax
                   
    ;Calcula o tamanho da FAT e coloca em "cx",(número de setores.).
    xor ax, ax
    xor cx, cx
    mov al, BYTE [fat16_bpb.TotalFATs] ;Número de FATs. 
    mul WORD [fat16_bpb.SectorsPerFAT] ;Setores por FAT. 
    
	;Quantidade de setores na fat. (contador)
    mov cx, ax      
.loop:

    push cx

    ;Quantidade
    mov cx, 1

	;buffer. (es:bx)
    xor ax, ax
    mov ax, 0x7000  ;base para mbr,vbr,fat,root,data area. no stage 2.
    mov es, ax
    mov bx, word [.address]  ;buffer (offset)	

	;LBA a se lido.
    mov ax, word  [.lba]      ;lba 
	
    ;Carregar apenas um setor na memória. O primeiro setor do diretório raiz.
	; cx = Quantidade de setores a serem lidos.
    ; ax = Número do primeiro setor a ser lido.
    ; es:bx = Buffer. Localização na memória onde serão carregados os setores.	
	;call hd0_read_sector
    call s2fat16ReadSectors

    pop cx

	dec cx
	cmp cx, word 0
	je .done_load_fat
	
	;address
	mov ax, word [.address]
	add ax, word 512
	mov word [.address], ax
	
	;lba
	mov ax, word [.lba]
	add ax, word 1
	mov word [.lba], ax

    jmp .loop

.fail:

    ;mov esi, msg_fat16_FatFail
    ;call os_output

	;mov esi, msg_fat16_FatFail   
	;call lib32PrintString

    popa
    ret

.done_load_fat:

	;; #bug bug... Isso tá errado tem que considerar es:bx
	;; Para pegar o valor que está no início da fat.

	;Ver se a FAT está no lugar certo.  
	;cmp word [FAT_OFFSET], word 0xFFF8              
    ;jne .fail 

    ;mov esi, msg_fat16_FatOk
    ;call os_output
    
    popa
    ret

.address:  dd 0
.lba:      dd 0


;;==============================================================
;; s2fat16LOADIMAGE:
;;      Carrega um arquivo na memória, setor por setor.
;;
;; Usaremos a função s2fat16ReadSectors para carregarmos um
;; um arquivo na memória.
; Esses são os argumentos da dunção s2fat16ReadSectors.
; cx = Quantidade de setores a serem lidos.
; ax = Número do primeiro setor (cluster) a ser lido. 
; es:bx = Buffer. 
; Localização na memória onde serão carregados os setores.
;

s2fat16LOADIMAGE:
	;pusha

    mov word [s2fat16_fat16_cluster], ax

	;Essa rotina de inicialização do dispositivo não é necessaria em modo real.
	;Tem uma rotina de interrupção que inicializa o dispositivo??
	;call init_ide0_master

    ;Buffer = Destino da imagem.
	;;@todo: rever isso. Em que header esta essa definição??
	;;devo colocar uma definição que esteja no header do stage 2.
    ;mov bx, word FAT16_IMAGE_ADDRESS      
    push bx
.next:
    pop bx                          ; buffer to read into

    ;converte cluster em lba.
    xor ax, ax
    mov ax, WORD [s2fat16_fat16_cluster]     ; cluster to read  	
    call s2fat16_fat16_ClusterLBA            ; convert cluster to LBA (eax)

    ;mov edx, eax                   ; coloca lba em edx
    mov word [.lba], ax             ;salva lba

	;quantidade de setores.
    xor cx, cx
    mov cl, BYTE 1 ;[fat16_bpb.SectorsPerCluster]  ;1 =  setores por cluster 
    ;buffer
    push bx      	
    mov ax, bx ; coloca o buffer em 'eax' 
    ;lba
    mov bx, word [.lba] ; coloca a lba em 'ebx'
    call s2fat16ReadSectors

    ;buffer
    pop bx             ;pega o buffer.
    add bx, word 512  ;incrementa o buffer.	
    push bx            ;coloca o buffer na pilha.
    
    ;cluster atual
    xor ax, ax
    mov ax, WORD [s2fat16_fat16_cluster]    ; identify current cluster
    
	;salva
	xor cx, cx
	mov cx, ax                ; copy current cluster	

	;salva novamente
	xor dx, dx
	mov dx, ax                ; copy current cluster
    
	;localização da fat.
	;mov bx, FAT16_FAT_ADDRESS  ; location of FAT in memory    
	add bx, cx                ;adiciona deslocamento                 
	add bx, cx                ;deslocamento dobrado para fat16 (vetores word)
 
    ;lê uma entrada da FAT.
    mov dx, WORD [bx]
    mov WORD [s2fat16_fat16_cluster], dx  ;salva	

    ;barra de progresso
	xor ax, ax
	mov ax, dx              ;mostra o numero do cluster.
	;call barra_de_progresso ;(nao altera os registradores)
	
	;testa
    cmp dx, word 0xFFFF 
    jne .next
;--
.DONE:
    ;ajustar a pilha.
    pop bx 
	
	;Message
	;mov esi, msg_load_file_eof
	;call os_output

	;Message.
	;mov esi, s2fat16_msg_load_file_eof   
	;call lib32PrintString

	
	;popa
	ret

.lba: 
    dw 0

s2fat16_msg_load_file_eof: 
    db "s2fat16LOADIMAGE: EOF.",13,10,0


;; =========================================
; s2fat16_fat16_ClusterLBA:
;     Convert FAT cluster into LBA addressing scheme
;     LBA = ((cluster - 2) * sectors per cluster) + dataarea.
;

s2fat16_fat16_ClusterLBA:
    
    sub ax, 0x0002  ;Zero base cluster number.
    xor cx, cx
    mov cl, BYTE 1   ;[fat16_bpb.SectorsPerCluster] ;setores por cluster.
    mul cx
    
	;debug
	;@todo: rever essa ocnstante, essa constante deve existir no header do stage  2.
	;cmp WORD [s2fat16_fat16_datasector], word  FAT16_DATAAREA_LBA ;591 ; Partição 0 8MB.
	jne .fail

    add ax, WORD [s2fat16_fat16_datasector]        ; Base data sector.
    ret
.fail:
    
	;Message.
	;mov esi, fail_datasector_s
    ;call os_output

	;Message.
	;mov esi, fail_datasector_s   
	;call lib32PrintString
	
	;Debug: Cancelada.
	;xor ax, ax
	;mov ax, WORD [fat16_datasector] 
	;call print_hex	

    JMP $

;mensagem cancelada.	
;fail_datasector_s db "fat16_ClusterLBA: Data sector errado!",0



;===============================================================================
; s2fat16ReadSectors:
;
; Reads "cx" sectors from disk starting at "ax" into memory location "es:bx".
; Lê 'cx' setores do 'disco' começando em 'ax' e coloca na memória em 'es:bx'.
;
; cx = Quantidade de setores a serem lidos.
; ax = Número do primeiro setor a ser lido.
; es:bx = Buffer. Localização na memória onde serão carregados os setores.
;

s2fat16ReadSectors:
s2_ReadSectors:
s2_MAIN:
    mov  di, 0x0005    ;Five retries for error.
.SECTORLOOP:
    push ax
    push bx
    push cx
    call s2_LBACHS
    mov ax, 0x0201                       ;BIOS read sector. Read one sector. 
    mov ch, BYTE [s2_absoluteTrack]      ;Track.
    mov cl, BYTE [s2_absoluteSector]     ;Sector.
    mov dh, BYTE [s2_absoluteHead]       ;Head.
    mov dl, BYTE [s2_bpb.DriveNumber]    ;Drive.
.rSec:
    int 0x13     
    jc .rSec     ;Test for read error.
.SUCCESS:
    
	;Message.
	;mov     si, msg_progress
    ;call    displayMessage
    
    pop cx
    pop bx
    pop ax
    add bx, WORD [s2_bpb.BytesPerSector]    ;Queue next buffer.
    inc ax                                  ;Queue next sector.                                          
    loop s2_MAIN                            ;Read next sector.
    RET
	
;---------------------------------------------------------------------------
; s2_LBACHS:
;     Convert "ax"; LBA addressing scheme to CHS addressing scheme.
;
;     absolute sector = (logical sector / sectors per track) + 1.
;     absolute head   = (logical sector / sectors per track) MOD number of heads.
;     absolute track  = logical sector / (sectors per track * number of heads).
;

s2_LBACHS:
    xor dx, dx                           ;Prepare dx:ax for operation.               
    div WORD [s2_bpb.SectorsPerTrack]    ;Divide por spt - 11h.
    inc dl                               ;Adjust for sector 0.  
    mov BYTE [s2_absoluteSector], dl
    xor dx, dx                           ;Prepare dx:ax for operation.                 
    div WORD [s2_bpb.NumHeads]               
    mov BYTE [s2_absoluteHead],  dl
    mov BYTE [s2_absoluteTrack], al
    RET

; Variáveis.
s2_absoluteSector db 0x00
s2_absoluteHead   db 0x00
s2_absoluteTrack  db 0x00

;
; End.
;

