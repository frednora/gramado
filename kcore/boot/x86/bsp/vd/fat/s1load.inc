; s1load.inc
; Library for stage1.asm
; Load the rootdir, load the FAT and pass the command to KLDR.BIN.


; -----------------------------------------
; ClusterLBA:
; Convert FAT cluster into LBA addressing scheme
; LBA = (cluster - 2) * sectors per cluster
ClusterLBA:
    sub ax, 0x0002                      ; zero base cluster number
    xor cx, cx
    mov cl, BYTE [SectorsPerCluster]    ; convert byte to word
    mul cx
    add ax, WORD [datasector]           ; base data sector
    RET

; -----------------------------------------
; ReadSectors:
; IN:
; Reads "cx" sectors from disk starting at "ax" 
; into memory location "es:bx"
ReadSectors:
; #ps: 
; It goes to the next segment if it reaches the end of the current segment.

    mov WORD [DAPBuffer]   ,bx
    mov WORD [DAPBuffer+2] ,es
    mov WORD [DAPStart]    ,ax
; Tentativas.
.MAIN:
    mov di, 0x0005  
.SECTORLOOP:

    push  ax
    push  bx
    push  cx

    push si
    mov ah, 0x42
    mov dl, 0x80
    mov si, DAPSizeOfPacket
    int 0x13
    pop si
 
    jnc  .__SUCCESS    ; Test for read error.
    xor  ax, ax        ; BIOS reset disk.
    int  0x13          ; Invoke BIOS.
    
    dec  di            ; Decrement error counter.
    
    pop  cx
    pop  bx
    pop  ax

; Attempt to read again
    jnz  .SECTORLOOP    
;.fail:
    int  0x18
.__SUCCESS:

    ; Importante: 
    ; Mensagem de progresso.    
    ;mov  si, msgProgress
    ;call  DisplayMessage
    
    pop  cx
    pop  bx
    pop  ax

    ; Queue next buffer.
    ; Increment the segment if the offset reaches the end of the segment.
    ; Actually when it reaches the start of the next segment.
    add bx, WORD [BytesPerSector]  ;512
    cmp bx, 0x0000
    jne .NextSector

    ; Switching the segment.
    push ax
    mov  ax, es
    add  ax, 0x1000   ; Got to the next segment.
    mov  es, ax
    pop  ax

.NextSector:

    inc  ax                     ; Queue next sector.
    mov WORD [DAPBuffer], bx
    mov WORD [DAPStart], ax
    loop  .MAIN                 ; Read next sector.
    RET


; -----------------------------------------------
; Calculate roodir start
; Root location
; Compute location(LBA) of root directory and store in "ax".
calculate_roodir_start:
    xor ax, ax
    mov  al, BYTE [TotalFATs]    ; 2. Number of FATs.
    mul  WORD [SectorsPerFAT]    ; 246. Sectors used by FATs.
; +62. Adiciona os setores reservados. 
; +5.  Adiciona os setores escondidos. (1+1+3) 
; 5 = ( 1 MBR + 1 VBR + 3 hidden sectors )
    add  ax, WORD [ReservedSectors]  
    add  ax, WORD [HiddenSectors]    
; >>> Nesse momento ax contém o setor inicial do root dir.
; vamos adicionar o tamanho do diretório raiz dado em setores. 
; [ROOTDIRSIZE]
; Dessa forma ex contém o setor onde inicia a área de dados.
    mov word [ROOTDIRSTART], ax
    ret 

; -----------------------------------------------
; Calculate roodir size
; ## data area location ## 
; Calcula o início da área de dados.
; Compute size of root directory and store in "cx". In sectors.
; Coloca o resultado em cx. (Quantidade de setores no diretório raiz.)
calculate_roodir_size:
    mov  ax, 32                   ; 32 byte. Tamanho de uma entrada de diretório.
    mul  WORD [MaxRootEntries]    ; 512. Tamanho total do diretório dado em bytes. (32*512) bytes.
    div  WORD [BytesPerSector]    ; ((32*512)/512) O número total de bytes no diretório, dividido pela quantidade de bytes por setor.
    mov  WORD [ROOTDIRSIZE], ax
    ret 

;----------------------------------------------------
; Obs: 
; A variável 'datasector' precisa ser inicializada aqui, 
; pois é usada mais à frente na rotina de conversão.
; Obtivemos com o cálculo:
    ;; cx           = Tamanho do diretório raiz, dado em número de setores.
    ;; [datasector] = Início da área de dados.
;; >> Carregar o diretório raiz em es:bx 0x07C0:0x0200.
;; Obs: 
;; Me parece seguro permitirmos que carregue o diretório raiz inteiro,
;; Porém desnecessário ainda.
;; 559 (root)
;; ( MBR + Reserved Sectors + VBR + Hidden Sectors + TotalFATs * SectorsPerFAT )
;; (  1  +      62          +  1  +    3           +    2      *     246)
;;==============================
;;   1 - mbr (*hidden) 
;;  62 - Reserved sectors.
;;   1 - vbr (*hidden) (VBR da primeira partição)  	
;; 246 - fat1
;; 246 - fat2
;;   3 - Hidden sectors. (*hidden)
;;  32 - root. (512 entradas)
;; xxx - data area.
; -----------------------------------------------
calculate_datasector_start:
    mov ax, word [ROOTDIRSTART]
    add ax, word [ROOTDIRSIZE]
    mov  WORD [datasector], ax
    ; #test
    ; SIMULANDO o data area para testes.
    ; mov  WORD [datasector], 591
    ret 


; ------------------------------------------------
__load_rootdir:
; 559 - Início do diretório raiz.
; 32  - Quantos setores carregar. Tamanho do diretório raiz.
; 0x0200 - Offset para encontrarmos o endereço onde devemos 
; carregar o root na memória. (seg:off) 

; IN:
; Reads "cx" sectors from disk starting at "ax" 
; into memory location "es:bx"
    mov  bx, ROOTDIR_OFFSET
    mov  ax, word [ROOTDIRSTART]
    mov  cx, word [ROOTDIRSIZE]
    ; #ps: It goes to the next segment if it reaches the end of the current segment.
    call  ReadSectors
    ret 

; -------------------------------------------------------
; Load rootdir into the memory
s1load_load_rootdir:

; Calculate what is the first sector of the rootdir and 
; what is the size of rootdir in sectors.
    call calculate_roodir_start
    call calculate_roodir_size

; Calculate what is the first sector of the data sector.
    call calculate_datasector_start

; Load the rootdir into the memory
; given the values calculated before.
    call __load_rootdir

    ret

; ==================================================================
; Load root and fat and go
; Step7:
; Carregamentos...
; Load root, fat and image.
; Carregar o ROOT.
; #todo: 
; Cuidadosamente rever os cálculos feitos aqui para permitir que
; que carreguemos o sistema usando discos de vários tamanhos. 
; Por enquanto estamos predeterminando as diretrizes de carregamento. 
;  559 (root)
; ( MBR + Reserved Sectors + VBR + Hidden Sectors + TotalFATs * SectorsPerFAT )
; (  1  +      62          +  1  +    3           +    2      *     246)
; =================================================================
;   1 - mbr (*hidden) 
;  62 - Reserved sectors.
;   1 - vbr (*hidden) (VBR da primeira partição)
; 246 - fat1
; 246 - fat2
;   3 - Hidden sectors. (*hidden)
;  32 - root. (512 entradas)
; xxx - data area.

; IN: Nothing
; Search file in rootdir, load fat and go.
s1load_load_file_and_go:

; --------------------------------------
; Search
; Procurando o arquivo BM.BIN no diretório raiz.
; Browse root directory for binary image.
.searchFile:
    mov cx, WORD [MaxRootEntries]  ; Load loop counter.
    mov di, ROOTDIR_OFFSET         ; Determinando o offset do início do diretório.
.LOOP:
    push  cx
    mov  cx, 0x000B       ; Eleven character name.
    mov  si, ImageName    ; Image name to find.
    ; It prints the image name while comparing each entry.
    ; Debug purpose.
    pusha
    call  DisplayMessage
    popa
    push  di
    rep  cmpsb            ; Test for entry match.
    pop  di
    je  LOADFAT     ; Se o arquivo foi encontrado.
    pop  cx
    add  di, 0x0020       ; Queue next directory entry.
    loop  .LOOP
    jmp  FAILURE

; Load the FAT in es:bx 0x17C0:0200.
; #bugbug Size?

LOADFAT:

    ;pusha
    ;mov  si, msgFAT
    ;call  DisplayMessage
    ;popa

; dx = Cluster inicial do arquivo a ser carregado.
; Save starting cluster of boot image.
; Salvamos o número do cluster inicial da Imagem. 
; Isso foi obtido na entrada do diretório.
; File's first cluster.

    mov dx, WORD [di + 0x001A]
    mov WORD [cluster], dx 

; Efetuando o carregamento da fat no buffer es:bx. 
; 0x6000:0x0200.

;===================================
; #bugbug: 
; SE CARREGAR A FAT INTEIRA DÁ PROBLEMA. !!!! 
; (246/2)
; Estamos carregando apenas metade da fat.
; ?? Por que ?? Qual é o problema ??
;===================================
; Read FAT into memory (0x6000:0200).
; Obs: Confiar no cálculo do início da LBA é perigoso, apesar de necessário.
; Por enquanto vamos determiná-lo.
; Obs: É possível confiarmos no cálculo do tamanho da fat, mesmo assim, vamos
; por enquanto, determiná-lo.
; Obs: Não estamos carregando a FAT inteira. Isso pode ser arriscado, mas 
; por enquanto, como temos poucos arquivos, vamos carrega apenas metade da FAT.

;.loadFAT:

; Setup segment and offset for FAT.
    mov ax, FAT_SEGMENT
    mov es, ax   
    mov bx, FAT_OFFSET

; Compute location of FAT and store in "ax".
; Calculado qual é a LBA inicial da FAT e salvando em 'ax'.

    mov ax, WORD [HiddenSectors]      ; 5 setores escondidos.
    add ax, WORD [ReservedSectors]    ; 62 setores reservados.
    ; mov  ax, 67                     ; simulando para teste
    ;mov  cx,  (246/2) ; WORD [SectorsPerFAT]  ;  Metade da fat.

    mov cx, 128 ; 64KB (1 segment)

; #bugbug
; This is our limit for now, we can't load more than 128 sectors.
; It causes a lot of problems,
; 'cause this way can't read the FAT properly, and
; if the root dir has a lot o entries, maybe we will not be able
; to find the information about the file we want to load.



; #bugbug
; Estamos carregando apenas metade da fat.
; Falha se tentarmos carregar ela inteira.
; IN:
; Reads "cx" sectors from disk starting at "ax" 
; into memory location "es:bx"
; #ps: It goes to the next segment if it reaches the end of the current segment.
    call  ReadSectors

; Reafirme o segmento da fat.
; Nesse momento a FAT já está na memória, precisamos indicar 
; em gs:bx o endereço do buffer onde carregamos a fat.
; Indicando o segmento onde a FAT está carregada.
    mov ax, FAT_SEGMENT
    mov gs, ax

setup_buffer_for_image:

; Image address (es:bx), 0:8000h.
    mov ax, IMAGE_SEGMENT
    mov es, ax
    mov bx, IMAGE_OFFSET

; Salvando o offset do buffer do arquivo.
    push  bx

; >> Carrega o arquivo na memória 
;    em es:bx, 0:8000h.

__loop_LOAD_IMAGE:

; Em 'bx' está o offset do buffer do arquivo.

; Recuperando o offset do buffer do arquivo.
    pop  bx                                  

; Vamos converter Cluster em LBA.
    mov  ax, WORD [cluster]    ; Cluster inicial do arquivo, obtido na entrada no diretório.
    call  ClusterLBA           ; Convert cluster to LBA.

; Carregamos apenas um cluster de cada vez.
; No nosso caso, um cluster só tem um setor.

.loadImage:

; IN:
; Reads "cx" sectors from disk starting at "ax" 
; into memory location "es:bx"
    xor  cx, cx
    mov  cl, BYTE [SectorsPerCluster]    ; 1. Sectors to read.
    ; #ps: It goes to the next segment if it reaches the end of the current segment.
    call  ReadSectors

.saveThis:

; Vamos savar o offset do próximo setor a ser carregado.
; Onde deve ficar o próximo setor. ??

    push bx

; Agora temos que saber qual será o próximo cluster.
; Vamos calcular o deslocamento dentro da fat para obtermos o 
; o local onde encontraremos o valor do próximo cluster.
; Salvaremos em 'dx'.
; lembrando que a fat está em gs:bx.

    mov ax, WORD [cluster]    ; identify current cluster
    add ax, ax                ; 16 bit(2 byte) FAT entry
    mov bx, FAT_OFFSET        ; location of FAT in memory
    add bx, ax                ; index into FAT    
    mov dx, WORD [gs:bx]      ; read two bytes from FAT

; Saving new cluster.
; Em 'dx', está o valor do próximo cluster.

.DONE:
    mov  WORD [cluster], dx 

; Test for end of file.
; Testamos para ver se é o último cluster. 
; 0xFFFF ? or 0xFFF8 ?
; Se esse foi o último cluster então prosseguiremos.
; Caso contrário volta para o loop.

.testEOF: 
    cmp  dx, END_OF_CLUSTER  
    jne  __loop_LOAD_IMAGE

; Done:
; Pass an argument to the next stage.
; Disk Number.
; Passando o comando para o BM.BIN em 0:8000h.

Step8_PARAMETERS:
    mov dl, byte [DriveNumber]
Step9_FLY:
    PUSH WORD  0         ; CS
    PUSH WORD  0x8000    ; Offset 
    RETF


