;
; File:  fs\fat16lib.inc 
; Descrição:
;     Operações em sistema de arquivos fat16.
;     32bit code
;     16bit code?
; 2015 - Created by Fred Nora.
;

;==========================================
; get_dataarea_init: 
;     Calcular o início da área de dados.
get_dataarea_init:
; Calcula o endereço base da área de dados.

    pushad
    xor eax, eax  
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx

    ; Calcula o tamanho do diretório raiz. 
    ;( 32 * 512 ) / 512
    ; quantidade de bytes / bytes por setor = número de setores.    

    xor eax, eax 
    xor ecx, ecx
    mov ax, word 32                        ; 32 bytes por entrada do diretório.         
    mul WORD [fat16_bpb.MaxRootEntries]    ; 512 - Número máximo de entradas.
    div WORD [fat16_bpb.BytesPerSector]    ; 512 - bytes por setor.
    xchg ax, cx                            ; Salva o tamanho do diretório.

    ;32

; Calcula a localização do diretorio raiz. 
;(2*254) + (setores reservados) = "ax".

    xor eax, eax 
    mov al, BYTE [fat16_bpb.TotalFATs]        ; 2   - numero de fats.
    mul WORD [fat16_bpb.SectorsPerFAT]        ; 246 - setores por FATs.
    add ax, WORD [fat16_bpb.ReservedSectors]  ; 3   - setores reservados.
    add ax, 1                                 ; Quantidade de setores do VBR. (1) 
    add ax, word FAT16_VBR_LBA                ; 63  - início da partição.

; Construindo o valor de 'fat16_datasector', 
; o inicio da area de dados. 

    mov WORD [fat16_datasector], ax  ; Coloca 2 fats + setor reservado em 'ax'          
    add WORD [fat16_datasector], cx  ; Adiciona o tamanho do diretorio raiz. 

; Obs:
; O valor de data sector esta salvo em 'fat16_datasector'.

    popad
    ret

;====================================
; fat16_LOAD_ROOT: 
; Carrega o rootdir na memória. 
; #todo: 
; Carrega apenas 1 setor.
; O ideal é carregar o root dir inteiro.
; 32 setores. 512 entradas.

fat16_LOAD_ROOT:

; Reset IDE.
    call init_ide0_master

    pushad

    push eax
    mov eax, FAT16_ROOTDIR_ADDRESS
    mov dword [.address], eax
    mov eax, FAT16_ROOTDIR_LBA
    mov dword [.lba], eax
    pop eax

; Carrega 32 setores.
; 32 setores = 512 entradas.
    mov ecx, dword 32

.loop:

    push ecx
    mov eax, dword [.address]  ; buffer
    mov ebx, dword [.lba]      ; lba 
    call fat16_ReadSectors
    pop ecx

    dec ecx
    cmp ecx, dword 0
    je .done_load_root

    ; Address
    mov eax, dword [.address]
    add eax, dword 512
    mov dword [.address], eax

    ; lba
    mov eax, dword [.lba]
    add eax, dword 1
    mov dword [.lba], eax

    jmp .loop

.done_load_root:
    popad
    ret
.address: dd 0
.lba: dd 0

; =========================================
; fat16_LOAD_FAT: 
;     Carrega a FAT na memória.

fat16_LOAD_FAT: 

	call init_ide0_master    ;reset ide.
	
    pushad
	
	push eax
	mov eax, FAT16_FAT_ADDRESS
	mov dword [.address], eax
	mov eax, FAT16_FAT_LBA
	mov dword [.lba], eax
	pop eax
                   
    ;Calcula o tamanho da FAT e coloca em "cx",(número de setores.).
	xor eax, eax
	xor ecx, ecx	
    mov al, BYTE [fat16_bpb.TotalFATs] ;Número de FATs. 
    mul WORD [fat16_bpb.SectorsPerFAT] ;Setores por FAT. 
    
	;Quantidade de setores na fat. (contador)
	mov ecx, eax      
.loop:	

    ;Carregando apenas 1 setor.
	
    ;carrega FAT.
    push ecx
    mov eax, dword [.address]  ;buffer
    mov ebx, dword [.lba]      ;lba 
	call fat16_ReadSectors
	pop ecx

	dec ecx
	cmp ecx, dword 0
	je .done_load_fat
	
	;address
	mov eax, dword [.address]
	add eax, dword 512
	mov dword [.address], eax
	
	;lba
	mov eax, dword [.lba]
	add eax, dword 1
	mov dword [.lba], eax
	
	jmp .loop	
  

.fail:
	;mov esi, msg_fat16_FatFail
    ;call os_output
    ;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_fat16_FatFail   
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen	
	
    popad
	ret
	
.done_load_fat:
    
	;Ver se a FAT está no lugar certo.  
	cmp word [FAT16_FAT_ADDRESS], word 0xFFF8              
    jne .fail 
	
	mov esi, msg_fat16_FatOk
    call os_output
    popad
	ret

.address: dd 0
.lba: dd 0


; ===================================  
; procura_entrada_vazia_na_fat: 
;     Procurando entradas vazias na FAT.
; IN: 
; eax = número de entradas vazias pra procurar. 
;       (file size).

procura_entrada_vazia_na_fat:

    pushad    
    
; salva o tamanho do arquivo.
    mov dword [CurrentFile.size], eax
; carrega FAT.
    call fat16_LOAD_FAT  ;load_fat_first_sector

;pega valores.
    mov esi, dword FAT16_FAT_ADDRESS   ;fat location
    mov eax, lista_file_clusters       ;base da lista

;pega a quantidade de cluster do arquivo.
    mov ecx, dword [CurrentFile.size]
.next_Entry:
    ;zera id do cluster.
    xor edx, edx
    ;pega um cluster. (WORD = fat16)
    mov DX, WORD [esi]
    ;vazia?
    cmp dx, WORD 0
    je .vazia
.ocupada:  
	add esi, dword 2 ;incrementa deslocamento dentro da fat
	jmp .next_Entry
.vazia:

    ;confere se o tamanho acabou.
    cmp ecx, dword 0
    je .eof

    ;decremtenda a contagem de clusters(size file)
    dec ecx
  
    ;coloca na lista
	mov [eax], WORD DX     ;coloca o cluster na lista.
	add eax, 2             ;incrementa a lista
	add esi, dword 2       ;incrementa deslocamento dentro da fat.
	jmp .next_Entry    

.eof:
	;coloca na lista  
	mov word [eax], DX            ;coloca o cluster na lista.
	add eax, 2
	;coloca na lista uma assinatura de fim de arquivo.
	mov WORD [eax], word 0xfff8   
	
;coloca os elementos da lista na fat.
.salva_lista_na_fat:

    xor eax, eax
    xor ebx, ebx  
    xor ecx, ecx 
    xor edx, edx

;pega localização.
    mov esi, dword lista_file_clusters    ;localização da lista.
    mov edi, dword FAT16_FAT_ADDRESS      ;localização da FAT.

;pega a quantidade de setores
    mov ecx, dword [CurrentFile.size]

.salva_proxima_entrada_na_fat:

;pega da lista
	mov ax, word [esi]         ;atual
	mov bx, word [esi+2]       ;posterior
	mov word [.atual],     ax  ;atual
	mov word [.posterior], bx  ;posterior
	
	;pega base da fat
	mov edx, edi 
    
    ;adiciona	
	add dx, WORD [.atual]     ;adiciona o deslocamento indicado pelo valor atual.
	add dx, WORD [.atual]     ;dobra, pra atribuir o deslocamento correto.
	
	;salva em uma entrada da fat o endereço da entrada do próximo cluster.
	mov ax, WORD [.posterior] ;pega.
	mov WORD[edx], ax             ;salva.   
	
	;confere se chegamos ao fim da lista. (file size--)
	cmp ecx, dword 0
	je .done
	
	;incrementa o contador de tamanho de arquivo.
	dec ecx
	
	;incrementa a lista.
	add esi, dword 2
	
	;proxima entrada.
	jmp .salva_proxima_entrada_na_fat
	
;fim da lista.
.done:
    ;pega o início da fat.
	mov edx, edi
	
	;adiciona o deslocamento atual.
	xor eax, eax
	mov ax, WORD [.atual] 
	add edx, dword eax

	;assina com EOF.
    mov WORD [edx], WORD 0xfff8

.do_exit:
	popad
	RET

;variaveis locais
.atual:     dw 0
.posterior: dw 0

; ========================================
; fat16_search_file_name:
;     Procura por um nome de arquivo no diretório raiz e 
; retorna o primeiro número do primeiro cluster do arquivo em EAX.
; Obs: Se o arquivo não for encontrado, não há como inicializar o sistema.
; Pois nessa fase tudo o que temos a fazer é carregar o arquivo BL.BIN na
; memória e passar o comando para ele. 
; IN: 
;     eax = Nome do arquivo.
; OUT: 
;     AX = Cluster.

fat16_search_file_name:

    pushad

    ;salva nome
    mov dword [.name], eax

    ;carrega o root
    call load_root_first_sector

    xor ecx, ecx
    mov cx, WORD 512                ;16 ;[fat16_bpb.MaxRootEntries] contador de entradas  
    mov edi, FAT16_ROOTDIR_ADDRESS  ;inicio do diretorio    

;++
.loop_search_file:
    push ecx
    mov ecx, 0x000B            ;Eleven character name.

    mov esi, dword [.name]    
    push edi
    rep cmpsb                  ;Test for entry match. 
    pop edi
    je .exit_arquivo_encontrado

    pop ecx
    add edi, 0x0020            ;Queue next directory entry.	
    LOOP .loop_search_file
;--

    ;mov esi, msg_file_not_found
    ;call os_output
    ;push eax
	;push ebx
	;push ecx 
	;push edx

    mov esi, msg_file_not_found   
    call lib32PrintString

	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 

    call _asm_refresh_screen    

    jmp $
    jmp .fail

.sf_root_fail:
    mov esi, msg_sf_root_fail
    call os_output
    popad
    stc
    ret

.fail:
    ;mov esi, msg_fat16_Failure
    ;call os_output
    ;push eax
	;push ebx
	;push ecx 
	;push edx

    mov esi, msg_fat16_Failure   
    call lib32PrintString

	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 

    call _asm_refresh_screen    

    popad
    stc
    ret

.exit_arquivo_encontrado:

    ;faltava o contador.
    pop ecx

    ;arquivo encontrado!
	;mov esi, msg_file_ok
	;call os_output
   ; push eax
	;push ebx
	;push ecx 
	;push edx

    mov esi, msg_file_ok   
    call lib32PrintString

	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 

    call _asm_refresh_screen

    ; Pega o valor do primeiro cluster do arquivo.
    xor edx, edx
    mov dx, WORD [edi + 0x001A]   ;di= 200+20+1a.
    mov WORD [fat16_cluster], dx  ;file's first cluster.

    popad

    ;retorna cluster.
    mov ax, WORD [fat16_cluster]
    clc
    ret

.name: dd 0

; Messages.
msg_sf_root_ok   db "Root Ok!",       13,10,0
msg_sf_root_fail db "Root not found!",13,10,0
;...

; ==========================================================
; fat16_LOAD_IMAGE:
;     Carrega o arquivo.
;     Com a FAT na memória, carrega todos os clusters.
;++
; IN:
;    AX = Cluster inicial do arquivo.
;
fat16_LOAD_IMAGE: 

	pushad

	mov word [fat16_cluster], ax

	call init_ide0_master

    ;Buffer = Destino da imagem.
    mov ebx, dword FAT16_IMAGE_ADDRESS     
	push ebx
.next:	
	pop ebx                          ; buffer to read into

    ;converte cluster em lba.
	xor eax, eax
    mov ax, WORD [fat16_cluster]     ; cluster to read  	
    call fat16_ClusterLBA            ; convert cluster to LBA (eax)
	;mov edx, eax                     ; coloca lba em edx
    mov dword [.lba], eax             ;salva lba
    	
	;quantidade de setores.
	xor ecx, ecx
    mov cl, BYTE 1 ;[fat16_bpb.SectorsPerCluster]  ;1 =  setores por cluster 
	;buffer
	push ebx      	
	mov eax, ebx ; coloca o buffer em 'eax' 
	;lba
	mov ebx, dword [.lba] ; coloca a lba em 'ebx'
	call fat16_ReadSectors
	
	;;
	;; *Importante: Podemos voltar para o mode real e lermos o setor com a ajuda do BIOS.
	;; é necessario passar vários argumentos para o o modo real e voltar para 
	;; o modo protegido preservando todas as características que tinha antes.
	;;
	;;
    
    ;buffer
	pop ebx             ;pega o buffer.
    add ebx, dword 512  ;incrementa o buffer.	
	push ebx            ;coloca o buffer na pilha.
    
	;cluster atual
    xor eax, eax	
	mov ax, WORD [fat16_cluster]    ; identify current cluster
    
	;salva
	xor ecx, ecx
	mov cx, ax                ; copy current cluster	

	;salva novamente
	xor edx, edx
	mov dx, ax                ; copy current cluster
    
	;localização da fat.
	mov ebx, FAT16_FAT_ADDRESS  ; location of FAT in memory    
	add ebx, ecx                ;adiciona deslocamento                 
	add ebx, ecx                ;deslocamento dobrado para fat16 (vetores word)
     
	;lê uma entrada da FAT.
	mov dx, WORD [ebx]
    mov WORD [fat16_cluster], dx  ;salva	
	
    ;barra de progresso
	xor eax, eax
	mov ax, dx              ;mostra o numero do cluster.
	call barra_de_progresso ;(nao altera os registradores)
	
	;testa
	cmp dx, word 0xFFFF              
    jne .next
	
;--	
.DONE:
    ;ajustar a pilha.
    pop ebx 
	
	;mov esi, msg_load_file_eof
	;call os_output
	;push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, msg_load_file_eof   
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen	
	
	popad
	ret

.lba: dd 0
msg_load_file_eof db "fat16_LOAD_IMAGE: EOF.",13,10,0

; =====================
; fat16_ReadSectors: 
; Lê um setor dado Lba e Buffer.
fat16_ReadSectors:
    call hd0_read_sector 
    ret 

; =====================
; fat16_DisplayMessage:
; Display ASCIIZ string at "esi".
fat16_DisplayMessage:
    call os_output
    ret

; =====================
; ClusterLBA:
; Convert FAT cluster into LBA addressing scheme
; LBA = ((cluster - 2) * sectors per cluster) + dataarea.

fat16_ClusterLBA:
    
    sub eax, 0x0002  ;Zero base cluster number.
    xor ecx, ecx
    mov cl, BYTE 1   ;[fat16_bpb.SectorsPerCluster] ;setores por cluster.
    mul ecx
    
    ;debug
    cmp WORD [fat16_datasector], word  FAT16_DATAAREA_LBA ;591 ; Partição 0 8MB.
    jne .fail

    add ax, WORD [fat16_datasector]        ; Base data sector.
    ret

.fail:
    ;mov esi, fail_datasector_s
    ;call os_output
   ; push eax
	;push ebx
	;push ecx 
	;push edx
	mov esi, fail_datasector_s   
	call lib32PrintString
	;pop edx 
	;pop ecx 
	;pop ebx 
	;pop eax 
	call _asm_refresh_screen
	
	xor eax, eax
	mov ax, WORD [fat16_datasector] 
	call print_hex

	JMP $

fail_datasector_s db "fat16_ClusterLBA: Data sector errado!",0

;===============================================
; barra_de_progresso:
; in: 
;     AX = Número do cluster atual.
barra_de_progresso:
    pushad

	;ax = número do cluster atual.
	;call print_hex
	
	;.
	;mov esi, msg_fat16_Progress
	;call os_output
	push eax
	push ebx
	push ecx 
	push edx
	mov esi, msg_fat16_Progress   
	call lib32PrintString
	
	mov eax, dword [g_cursor_x]
	inc eax
	mov dword [g_cursor_x], eax

	pop edx 
	pop ecx 
	pop ebx 
	pop eax 
	call _asm_refresh_screen

	popad
	ret

; ===========================================
; fat16_create_file:
; Cria um arquivo.
; in: 
;     eax = Endereço do nome a ser gravado na entrada.
; out: 
;     eax = lba inicial.

fat16_create_file:

    pushad

	;pega o argumento - endereço do nome
    mov dword [fat16_ImageName], eax

	;procura um cluster livre na fat.
	;++
	call load_fat_first_sector
	call procura_um_cluster_livre_na_fat
	jc .fail
	;pega o retorno, (numero da cluster vazio)
	mov dword [.lba], eax
	call save_fat_first_sector
	;--

	mov esi, msg_cluster_ok
	call os_output

	; carrega o diretorio raiz
	call load_root_first_sector
		
	mov esi, dword [fat16_ImageName]      ;Pega o endereço do nome.
	mov edi, dword FAT16_ROOTDIR_ADDRESS  ;Pega o início do root dir.

	xor eax, eax
	xor ecx, ecx ; inicia o contador
	
;++	
.next_entry:
	
	mov byte al, [edi]
	
	;0 - apagada ou vazia.
	cmp al, 0 
	je .entrada_vazia
	
	;" " - começada com espaço
	cmp al, byte " "
	je .entrada_vazia
    
	;"$" - entrada semi-apagada.
	cmp al, byte "$"
	je .entrada_vazia

	inc ecx              ;Incrementa o contador de entradas.
	
	cmp ecx, dword 16    ;Apenas 16 entradas (primeiro setor do diretorio).
	je .exitCD

	add edi, dword 32    ;Próxima entrada.
	
	jmp .next_entry
;--


;coloca o nome do arquivo na entrada vazia.
;++
.entrada_vazia:
    ;endereço do inicio da entrada vazia.
    mov eax, edi 
    mov dword [.endereco_da_entrada_vazia], eax

    ;tamanho do nome.
    mov ecx, dword 11 
.preenche_entrada:
    ;esi = endereço do nome a ser gravado na entrada.
    ;edi = endereço do inicio da entrada vazia.
    movsb
    loop .preenche_entrada     
;--

;coloca o valor da primeira lba na entrada vazia.
;++
    mov edi, dword [.endereco_da_entrada_vazia]
    add edi, dword 0x1A
    mov eax, dword [.lba]
    mov dword [edi], eax 
;--
    jmp .exitCD

.fail:
    ;mov esi, msg_fat_cluster_fail
    ;call  os_output
    push eax
    push ebx
    push ecx 
    push edx
    mov esi, msg_fat_cluster_fail   
    call lib32PrintString
    pop edx 
    pop ecx 
    pop ebx 
    pop eax 
    call _asm_refresh_screen

    popad
    stc
    ret

.exitCD:
    ;salva o primeiro setor do diretorio raiz da primeira partição.
    call save_root_first_sector

    mov esi, msg_create_dir_done
    call  os_output

    popad
    ;retorna o numero do primeiro cluster.
    mov eax, dword [.lba]
    clc
    ret

.endereco_da_entrada_vazia: dd 0
.lba: dd 0 ; cria um diretorio com apenas um cluster de tamanho.

msg_fat_cluster_fail db "falha ao encontrar um cluster livre",13,10,0
msg_create_dir_done  db "create dir done",13,10,0
msg_cluster_ok       db "cluster ok",13,10,0

; ============================================
; fat16_create_dir:
;     Cria um arquivo.(dir)
;     Cria um diretório usando 'eax' como ponteiro de nome.
;     @todo: Na verdade um nome de diretório começa com ".".
; in:
;     eax = Endereço do nome a ser gravado na entrada.

fat16_create_dir:
	call fat16_create_file
    ret


;=============================
; fat16_force_create_file:
;     Cria um arquivo.
; in: 
;     eax = Endereço do nome a ser gravado na entrada.
; out: 
;     eax = lba inicial.
;     @todo; A opção é cancelar essa rotina para ganhar espaço.

fat16_force_create_file:
 
    pushad

	;pega o argumento - endereço do nome
    mov dword [fat16_ImageName], eax

	;procura um cluster livre na fat.
	;++
	call load_fat_first_sector
	call procura_um_cluster_livre_na_fat
	jc .fail
	;pega o retorno, (numero da cluster vazio)
	mov dword [.lba], eax
	call save_fat_first_sector
	;--

	mov esi, msg_force_cluster_ok
	call os_output
	
	;*** carrega o diretorio raiz
	call load_root_first_sector

	mov esi, dword [fat16_ImageName]      ;pega o endereço do nome
	mov edi, dword FAT16_ROOTDIR_ADDRESS     ;pega o inicio do root dir

	xor eax, eax
	xor ecx, ecx ; inicia o contador
;++	
.next_entry:
	
	mov byte al, [edi]
	
	;0 - apagada ou vazia.
	cmp al, 0 
	je .entrada_vazia
	
	;" " - começada com espaço
	cmp al, byte " "
	je .entrada_vazia
    
	;"$" - entrada semi-apagada.
	cmp al, byte "$"
	je .entrada_vazia

	inc ecx              ;incrementa o contador de entradas.
	
	cmp ecx, dword 16    ;apenas 16 entradas (primeiro setor do diretorio).
	je .exitCD

	add edi, dword 32    ;proxima entrada
	
	jmp .next_entry
;--

;coloca o nome do arquivo na entrada vazia.
;++
.entrada_vazia:
    ;endereço do inicio da entrada vazia.
	mov eax, edi 
    mov dword [.endereco_da_entrada_vazia], eax
    
	;tamanho do nome.
	mov ecx, dword 11 
.preenche_entrada:
    ;esi = endereço do nome a ser gravado na entrada.
	;edi = endereço do inicio da entrada vazia.
	movsb
	loop .preenche_entrada     
;--

;coloca o valor da primeira lba na entrada vazia.
;++
    mov edi, dword [.endereco_da_entrada_vazia]
    add edi, dword 0x1A
    mov eax, dword [.lba]
    mov dword [edi], eax 
;--
    jmp .exitCD

.fail:
    ;mov esi, msg_force_fat_cluster_fail
    ;call  os_output
    push eax
    push ebx
    push ecx 
    push edx
    mov esi, msg_force_fat_cluster_fail
    call lib32PrintString
    pop edx 
    pop ecx 
    pop ebx 
    pop eax 
    call _asm_refresh_screen

    popad
    stc
    ret

.exitCD:
	;salva forçadamente o primeiro setor do diretorio raiz da primeira partição.
	;sem ver se existe entradas padrao ja presentes no diretorio.

	call force_save_root_first_sector

    mov esi, msg_force_create_dir_done
    call  os_output

    popad
    ;retorna o numero do primeiro cluster.
    mov eax, dword [.lba]
    clc
    ret

.endereco_da_entrada_vazia: dd 0
.lba: dd 0 

msg_force_fat_cluster_fail db "free cluster fail",13,10,0
msg_force_create_dir_done  db "create dir done",13,10,0
msg_force_cluster_ok       db "cluster ok",13,10,0

; =============================
; fat16_force_create_dir: 
; Cria um arquivo.(dir)
; Cria um diretório usando 'eax' como nome.
; in:
; eax = Endereço do nome a ser gravado na entrada.

fat16_force_create_dir:
    call fat16_force_create_file
    ret

; ==============================================
; procura_um_cluster_livre_na_fat:
; Procura uma entrada vazia na FAT.
; @todo: Mudar o nome.
; out: 
; eax = lba livre.

procura_um_cluster_livre_na_fat:

    ;call load_fat_first_sector

    ;endereço da fat.
    mov esi, dword FAT16_FAT_ADDRESS

    xor ecx, ecx ;inicia o contador.     
    xor eax, eax
.procura_proximo:
    mov ax, word [esi]    

	cmp ax, word 0
	je .encontrada

    ; procurar em 64 entradas.(provisorio)
    cmp ecx, dword 64
    je .nao_encontrada

	inc ecx            ;incrementa o contador.
	add esi, dword 2   ;ver proxima entrada.
	jmp .procura_proximo

.nao_encontrada:
    xor eax, eax
    stc  ;set flag
    ret

.encontrada:

    mov dword [.free_cluster], ecx     ;salva

    xor eax, eax
    mov ax, word 0xfff8
    mov word [esi], ax    

	;call save_fat_first_sector
	
    ;retorna eax com o numero do cluster livre
    mov eax, dword [.free_cluster]
    clc            ;clear flag
    ret

.free_cluster: dd 0

; =========================
; load_mbr:
;     Carrega o MBR no buffer.

load_mbr:
	
	;Inicializa o ide 0 master.
	call init_ide0_master

    pushad
	
	;Coloca um setor no buffer.
	mov eax, dword FAT16_MBR_ADDRESS    ;buffer
	mov ebx, dword FAT16_MBR_LBA        ;(@todo: Talvez em alguns discos deva ser lba 1.)
	call hd0_read_sector
	
	cmp byte [FAT16_MBR_ADDRESS + 0x1FE], byte 0x55
	jne .load_fail

	cmp byte [FAT16_MBR_ADDRESS + 0x1FF], byte 0xAA
	jne .load_fail
	
	jmp .done
.load_fail:
    ;Mostra mensagem de falha.
    mov esi, msg_load_fail
	call os_output
	;Mostra parametros de disco.
	call hardware_show_currentdisk_info
	jmp $
	;mov ebx, dword 1    ;lba (@todo: Talvez em alguns discos deva ser lba 1.)
	;call hd0_read_sector	
.done:
    popad
    ret

msg_load_fail db "load_mbr fail: Sig",13,10,0

; ========================================
; save_mbr:
; Salva MBR do buffer 00907C00h, para o disco.
save_mbr:
	
	call init_ide0_master
	
    pushad
	cmp byte [FAT16_MBR_ADDRESS + 0x1FE], byte 0x55
	jne .save_fail
	cmp byte [FAT16_MBR_ADDRESS + 0x1FF], byte 0xAA
	jne .save_fail
.do_save:	
	;coloca um setor no buffer.
	mov eax, dword FAT16_MBR_ADDRESS
	mov ebx, dword 0 ;lba
	call hd0_write_sector
    
	jmp .exit_save
.save_fail:
    mov esi, msg_save_fail
	call os_output
.exit_save:
	popad
    ret

msg_save_fail db "save_mbr fail: Sig",13,10,0

; ===============================================
; load_root_first_sector:
;     Carrega o primeiro setor do root no buffer.
;     @todo: Na verdade o ideal é carregar o rootdir inteiro.
;            Quando atualizado algum arquivo do sistema
;            ele pode ser salvo depois do primeiro setor.

load_root_first_sector:
	
    cmp byte [Selected_Drive_Number], 1
    jne .not_slave
.is_slave:

	;Select and initialize slave.
	mov al, 1
    call select_hd

    jmp .go

.not_slave:
    ;Select and initialize master.
    mov al, 0
    call select_hd
.go:
	;call init_ide0_master
	;call init_ide0_master

    pushad
	
	push eax
	push ebx
	;coloca um setor no buffer.
	mov eax, dword FAT16_ROOTDIR_ADDRESS
	mov ebx, dword FAT16_ROOTDIR_LBA        ;lba - (root dir) 
	call hd0_read_sector
	pop ebx
	pop eax

	popad
    ret
	
;@todo: Checar se essa mensagem está sendo usada.	
msg_load_root_fail db "load_root_first_sector fail: Dir",13, 10,0	

; =========================================
; save_root_first_sector:
;     Salva MBR.
save_root_first_sector:

    cmp byte [Selected_Drive_Number], 1
    jne .not_slave
.is_slave:

    ;Select and initialize slave.
    mov al, 1
    call select_hd

    jmp .go
.not_slave:
    ;Select and initialize master.
    mov al, 0
    call select_hd
.go:
    ;call init_ide0_master

    pushad

	;cmp byte [FAT16_ROOTDIR_ADDRESS +0], byte "s" ;system.cnf
	;jne .save_fail
 
	;cmp byte [FAT16_ROOTDIR_ADDRESS +32], byte "i" ;init.cnf
	;jne .save_fail
	
	;cmp byte [FAT16_ROOTDIR_ADDRESS +32 +32], byte "b"   ;bl.bin
	;jne .save_fail
	
	;cmp byte [FAT16_ROOTDIR_ADDRESS +32 +32 +32], byte "b"   ;boot.dir
	;jne .save_fail
	
	;cmp byte [FAT16_ROOTDIR_ADDRESS +32 +32 +32 +32], byte "s"  ;system.dir
	;jne .save_fail

	;cmp byte [FAT16_ROOTDIR_ADDRESS +32 +32 +32 +32 +32], byte "u"  ;user.dir
	;jne .save_fail

.do_save:	
	;coloca um setor no buffer.
	push eax
	push ebx
	mov eax, dword FAT16_ROOTDIR_ADDRESS
	mov ebx, dword FAT16_ROOTDIR_LBA    ;lba (root dir)
	call hd0_write_sector
	pop ebx
	pop eax
	
	jmp .exit_save
.save_fail:
    mov esi, msg_save_root_fail
	call os_output
.exit_save:
	popad
    ret

msg_save_root_fail db "save_root_first_sector fail: Dir",13,10,0

; ===============================
; force_save_root_first_sector:
; Salva MBR. 
force_save_root_first_sector:

    cmp byte [Selected_Drive_Number], 1
    jne .not_slave
.is_slave:

	;Select and initialize slave.
    mov al, 1
    call select_hd

    jmp .go
.not_slave:
    ;Select and initialize master.
    mov al, 0
    call select_hd
.go:
    ;call init_ide0_master
    pushad
.do_save:
	;coloca um setor no buffer.
	push eax
	push ebx
	mov eax, dword FAT16_ROOTDIR_ADDRESS
	mov ebx, dword FAT16_ROOTDIR_LBA ;lba (root dir)
	call hd0_write_sector
	pop ebx
	pop eax
	
.exit_save:
	popad
    ret

;======================================================
; load_fat_first_sector:
; Carrega o primeiro setor da FAT no buffer. 

load_fat_first_sector:

	call init_ide0_master
	
    pushad
	
	;coloca um setor no buffer.
	mov eax, dword FAT16_FAT_ADDRESS
	mov ebx, dword FAT16_FAT_LBA ;fat 
	call hd0_read_sector
	
	;checando a primeira entrada
	;cmp byte [00907e00h + 0], byte "b"
	;jne .load_fail

	;checando a primeira entrada
	;cmp byte [00907e00h + 1], byte "o"
	;jne .load_fail

	;checando a primeira entrada
	;cmp byte [00907e00h + 2], byte "o"
	;jne .load_fail

	;checando a primeira entrada
	;cmp byte [00907e00h + 3], byte "t"
	;jne .load_fail
	
	jmp .exit_load
.load_fail:
    ;mov esi, msg_load_fat_fail
	;call os_output
	;;...
.exit_load:	
	popad
    ret
	
msg_load_fat_fail db "load_fat_first_sector fail: ?",13,10,0

; =================================================
; save_fat_first_sector:
;     Salva o primeiro setor da FAT.
save_fat_first_sector:
	
	call init_ide0_master

    ;pushad
	
	;cmp byte [00907e00h + 0], byte "b"
	;jne .save_fail
 
	;cmp byte [00907e00h + 1], byte "o"
	;jne .save_fail
	
	;cmp byte [00907e00h + 2], byte "o"
	;jne .save_fail
	
	;cmp byte [00907e00h + 3], byte "t"
	;jne .save_fail

.do_save:	
	;coloca um setor no buffer.
	pushad
	mov eax, dword FAT16_FAT_ADDRESS
	mov ebx, dword FAT16_FAT_LBA ;fat1
	call hd0_write_sector
    popad
	
	jmp .exit_save
	
.save_fail:
   ; mov esi, msg_save_root_fail
	;call os_output

.exit_save:
	;popad
    ret

msg_save_fat_fail db "save_fat_first_sector fail: ??",13,10,0	


;====================================================
; fat16_openapp:
;     Abre um app.
;	
; IN:
;     eax = String com o nome do arquivo.
;
; OBS:
;     (nome completo) 8.3. 
;     @todo: (testar se tem ponto ou nao).
;
;
fat16_openapp:
    nop
	;mov esi, eax    
    ;call ?? 
.message:	
	mov esi, msg_open_app
	call os_output
	ret	
	
msg_open_app db "~Open app",13,10,0


;=================================================
; fat16_load_cluster:
;     Carrega um cluster.
;
; IN:
;     eax = mem address.
;     ebx = lba.
;	
fat16_load_cluster:

	;call init_ide0_master
	
    push eax
    push ebx
    push ecx
    
	;buffer
	;mov eax, dword IMG_ADDRESS
	mov dword [.buffer], eax ;memory address
    
	;lba
	;mov eax, dword [IMG_LBA_ORIGEM]
	mov dword [.lba], ebx   ;lba de origem
	
	;prepara o contador
	mov ecx, dword 64 ;IMG_SIZE ; tamanho do cluster

.load_sector:	
	;coloca um setor no buffer.
	mov eax, dword [.buffer] ;buffer
	mov ebx, dword [.lba] ;lba
	call hd0_read_sector
    
	add dword [.buffer], 0200h
	add dword [.lba], 1
	
	loop .load_sector	
	
	;sem mensagem para a função ser usada em modo texto/grafico.
	;mov esi, msg_carregado1
	;call os_output
	
	pop ecx
	pop ebx
	pop eax
	ret
.buffer: dd 0	
.lba: dd 0

;@todo: Checar se essa mensagem está sendo usada.
msg_carregado1 db "[fat16:] File Loaded",13,10,0 


;=====================================================================
; fat16DetectFSParameters:
;     Detectar os parâmetros do sistema de arquivos fat16
; usados na partição atual.
;     Coisas como a lba da fat, a lba do root dir etc ...
;     Os resultados devem ir para variaáveis globais ou salvas
; em blocos de informação ou metafile.
;
fat16DetectFSParameters:
    ;Ainda não implementada.
    ;Setup current dir.
    ;mov dword [g_current_fat_lba],        FAT16_FAT_LBA       ;Fat do volume atual
    ;mov dword [g_current_directory_lba],  FAT16_ROOTDIR_LBA   ;Diretório do volume atual.
    ;mov dword [g_current_dataarea_lba],   FAT16_DATAAREA_LBA  ;?? ;lba do inicio da area de dados do volume atual.
    ret

;
; End
;
