;
; File: s2metafile.inc
;
;     This is the main META$FILE used in the Gramado Boot Manager. 
;
;     Esse é um metafile de inicialização. É uma parte do Boot Manager e fica 
; no segundo setor do HD. Os programas podem acessar as informações contidas 
; aqui. O tamanho do metafile é 2 setores. 1KB.
;
;
;* Importante:
;  O metafile conterá o maior número possível de informações salvas. Para isso 
; talvez o tamanho do metafile seja revisto. Por enquanto é de 1KB. 
;
; *Importante:
; O metafile dará maior importãncia para as informações relativas
; ao armazenamento de dados em disco. (STORAGE).
;
;
; Lista de variáveis presentes nesse metafile:
; ===========================================
;
; .TRAP:
;     Passa o comando para o verdadeiro início, que está logo após área de 
; dados.
;
; .MAGIC_NUMBER:
;     Assinatura para validar o metafile.
;
; .BIOS_STATUS:
;     Sinaliza a presensa de BIOS. ??
;
; .BIOS_VENDOR:
;     Bios vendor.
;
; .DISK_NUMBER:
;     ?? Número do disco.
;     DL = 00h	1st floppy disk ( "drive A:" ) 
;     DL = 01h	2nd floppy disk ( "drive B:" ) 
;     DL = 80h	1st hard disk
;     DL = 81h	2nd hard disk
;
; .PARTITION_NUMBER:
;     Número da partição ativa.
;     Número da partição onde está o Boot Loader, BL.BIN.
;
; (disk geometry: CHS)
;  @todo: Inverter essa ordem se possível, para ficar CHS.
;
; .HEADS: H.
;     Número de lados no disco.
;
; .SPT: S.
;     Número de setores por trilha.
;
;.CYLINDERS: C.
;     Número de cilindros.
;
; .INIT_MODE:
;     Indica o modo de inicialização usado pelo Boot Manager.
;     Tem opções como: Modo texto, Modo gráfico ou Prompt de comandos.
;
; .VIDEO_MODE:
;     ?? Indica qual modo de vídeo o Boot Manager deve configurar.
;     ?? Por enquanto indica um dos modos VESA.
; 
; .SCREEN_X:
;     Largura da tela em pixel para modo gráfico.
;
; .SCREEN_Y:
;     Altura da tela em pixel para modo gráfico.
;
; .GDT_START:
;     Endereço base da GDT quando configurada pelo Boot Manager.
;
; .IDT_START:
;     Endereço base da IDT quando configurada pelo Boot Manager.
;
; .TSS_START:
;     Endereço base da TSS quando configurada pelo Boot Manager.
;
; .PD:
;     Endereço do diretório de tabelas de páginas quando configurado
; pelo Boot Manager.
;     Obs: Se o Boot Manager fizer alguma gerenciamento inicial, isso 
; poderá ser alterado nas p´roximas fases da inicialização do sistema.  
;    
; .KERNEL_BASE:
;     Endereço base do Kernel. Não há problema em deixar salvo o 
; endereço pretendido para o Kernel Base. 
;
; .KERNEL_ENTRYPOINT:
;     Endereço do entry point do kernel.
;  
; .GUI_BASE:
;     Endereço base da GUI em user mode. 
;      Aqui se trata de servido em user mode para gerenciamento de
; alguns recursos gráficos menos primitivos.
;
; .GUI_ENTRYPOINT:
;     Endereço do entrypoint da GUI em user mode.
;
; .MULTIBOOT_BLOCK:
;     Salva o endereço do bloco de informações usados pelo multiboot.
;
; Obs: Continua ...
;



;;++
META$FILE:

;;
;; Uma trap para cada tipo de inicialização. assim o bm pode ser inicializado
;; de varias maneiras, varios sistemas de arquivos e métodos de carregamento.
;; suporte à 8 traps.
;;

;;
;; * Dando oportunidade à opções  *
;;

;No caso de o BM não estar presente no sistema de arquivos.
;então ele foi carregado pelo stage 1 que pasou o comando para ele.
.TRAP1:
    PUSH 0 
	PUSH AFTER_DATA    ;; *fat16 (Padrão) Quando carregado pe stage 1.      
    RETF
;No caso de BM ser um arquivo no sistema de arquivos. Então ele foi carregado 
;pelo mbr em 0:8000h e o metafile fica em 0:8200h.
.TRAP2:
    PUSH 0 
	PUSH AFTER_DATA ;; fat12 ?? @todo:       
    RETF
	
.TRAP3:
    PUSH 0 
	PUSH AFTER_DATA ;; fat16 ?? @todo:
    RETF
	
.TRAP4:
    PUSH 0 
	PUSH AFTER_DATA ;; fat32 ?? @todo:
    RETF
	
.TRAP5:
    PUSH 0 
	PUSH AFTER_DATA ;; ext2 ?? @todo:
    RETF
	
.TRAP6:
    PUSH 0 
	PUSH AFTER_DATA ;; outro fs ?? @todo:
    RETF

.TRAP7:
    PUSH 0 
	PUSH AFTER_DATA ;;  outro fs ?? @todo:
    RETF

.TRAP8:
    PUSH 0 
	PUSH AFTER_DATA ;;  outro fs ?? @todo:
    RETF
	

; Assinatura do metafile.
.MAGIC_NUMBER: DW 1234

; BIOS parameters.
.BIOS_STATUS: DW 0    ;Existe ou não um BIOS ?
.BIOS_VENDOR: DW 0    ;Empresa, AMI ... etc. 
;...

; Cpu parameters.
.CPU_VENDOR: DW 0    ;Empresa ... INTEL etc.
;...

; Disk parameters. (*Importante)
.DISK_NUMBER:      DW 0    ;Em que disco o sistema está.
.PARTITION_NUMBER: DW 0	   ;Em que partição o sistema está.
.HEADS:            DW 0    ;Quantos lados tem o disco.
.SPT:              DW 0    ;Sectors per track.
.CYLINDERS:        DD 0    ;(double) Quantos cilindros tem no disco(tracks)
.S1_BPB_POINTER    DW 0    ;Ponteiro para o BPB do stage 1, do MBR.
;...


;
; Inicialization parameters.
;


;
; Modo inicial de vídeo. 
; 1 = Modo texto.
; 2 = Modo Gráfico.
; 3 = Prompt de comandos do Boot Manager.
; 4 = Nothing.
;

.INIT_MODE:  DB 3    ;; @todo: Escolher uma constante padrão.   

;
; Video parameters.
;      
;  NVidia GeForce, x8r8g8b8:	
;  GeForce 8400 GS: 0x0118 1024x768x32BPP
;  GeForce 8400 GS: 0x0115  800x600x32BPP  *
;  GeForce 8400 GS: 0x0112  640x480x32BPP
;
;  Virtual Box, r8g8b8:
;  Oracle VirtualBox: 0x0118 1024x768x24BPP
;  Oracle VirtualBox: 0x0115  800x600x24BPP  *
;  Oracle VirtualBox: 0x0112  640x480x24BPP
;

.VIDEO_MODE: DW 0        ;Modo de vídeo para o sistema. 
.SCREEN_X:   DW 0        ;Pixels por linha.
.SCREEN_Y:   DW 0        ;Pixels por coluna.
.BPP:        DB 0        ;Bits Per Pixel.
.LFB:        DD 0        ;Linear Frame Buffer from VESA mode.
;...

; Gdt, Idt and Tss.
.GDT_START: DD 0 
.IDT_START: DD 0 
.TSS_START: DD 0 

;
; PD:
; Page Directory. Endereço do diretório de tabelas de páginas. Configura o CR3.
.PD: DD 0

; Kernel physical address. 
.KERNEL_BASE:  DD 0 
.KERNEL_ENTRYPOINT: DD 0

; GUI physical address. ??
; Esse será um server que oferecerá rotinas gráficas em kernelmode. 
.GUI_BASE:  DD 0 
.GUI_ENTRYPOINT: DD 0


;
; Memória.
;


;
; Pci.
;


;Ponteiro para o Bloco de informações usados pelo multiboot. Isso
; oferecerá alguma padronização no carregamento do kernel.
; Obs: não colocaremos todo o bloco no metafile por enquanto.
.MULTIBOOT_BLOCK:  DD 0

;;Ponteiros referentes à storage.
;;
.MBR_POINTER:  DD 0
.BPB_POINTER:  DD 0
.PARTITION_TABLE_POINTER:  DD 0
.VBR_POINTER:  DD 0         ;VBR da partição ativa.
.FAT1_POINTER:  DD 0        ;FAT1 da partição ativa se o fs for FAT.
.FAT2_POINTER:  DD 0        ;FAT2 da partição ativa se o fs for FAT.
.ROOT_DIR_POINTER:  DD 0    
.DATA_AREA_POINTER:  DD 0   
;;@todo: Colocar o máximo de informações sobre o disco principal e 
;; sobre a partição principal.
;;...

;;Clone das informações do VBR da partição ativa.
;; ( BPB, Partition Table e outros )
;; Obs: @todo: Os valores devem se copiados do VBR para cá.
.VBRCLONE_ADDRESS: DD 0 ;; Endereço incial na memória.
.VBRCLONE_LBA:     DD 0 ;; LBA.
.VBRCLONE_NAME_ADDRESS: DD 0  ;; endereço de memória onde se encontra o Nome do volume.
.VBRCLONE_BYTES_PER_SECTOR: DW 0 ;; Bytes Per Sector.
.VBRCLONE_SECTORS_PER_CLUSTER: DB 0  ;; Sectors Per Cluster.
.VBRCLONE_RESERVED_SECTORS: DW 0
.VBRCLONE_TOTAL_FATS: DB 0 ;; Número de FATs no volume.
.VBRCLONE_MAX_ROOT_ENTRIES: DW 0 ;;Número máximo de entradas no diretório raiz do volume.
.VBRCLONE_TOTAL_SECTORS_SMALL: DW 0 ;;??
.VBRCLONE_MEDIA_DESCRIPTOR: DB 0
.VBRCLONE_SECTORS_PER_FAT: DW 0
.VBRCLONE_SECTORS_PER_TRACK: DW 0
.VBRCLONE_NUMBER_OF_HEADS: DW 0
.VBRCLONE_HIDDEN_SECTORS: DD 0
.VBRCLONE_TOTAL_SECTORS_LARGE: DD 0
.VBRCLONE_DRIVE_NUMBER: DB 0
.VBRCLONE_FLAGS: DB 0
.VBRCLONE_SIGNATURE: DB 0
.VBRCLONE_VOLUME_ID: DD 0
.VBRCLONE_VOLUME_LABEL_ADDRESS: DD 0 ;;Endereço onde se encontra a Label do volume.
.VBRCLONE_SYSTEM_ID_ADDRESS: DD 0 ;; Endereço onde se encontra a string do system id.
.VBRCLONE_PARTITION_TABLE_ADDRESS: DD 0 ;;Endereço onde começa a PT.
.VBRCLONE_PT_FLAG: DB 0
.VBRCLONE_PT_INITIAL_HCS: DB 0, 0, 0
.VBRCLONE_PT_OS_TYPE: DB 0
.VBRCLONE_PT_FINAL_HCS: DB 0, 0, 0
.VBRCLONE_PT_INITIAL_LBA: DD 0
.VBRCLONE_PT_PARTITION_SIZE: DD 0
.VBRCLONE_SECTOR_512BYTES_SIG_ADDRESS: DD 0 ;Endereço onde fica a assinatura para setores de 512bytes.
.VBRCLONE_SECTOR_4KB_SIG_ADDRESS: DD 0 ;Endereço onde fica a assinatura para setores de 4KB.
;;


;;INIT.TXT info:
;Nome do arquivo de configurações de inicialização.
.INITIALIZATION_FILE_NAME:     db 'INIT    TXT'
.INITIALIZATION_FILE_FULLNAME: db 'INIT.TXT'
;;...


;;
;; @todo: Isso é muito importante. Criar aqui detalhes sonre o arquivo boot loader.
;;

;;BL.BIN info:
.BL_NAME: db 'BL      BIN'
.BL_FULLNAME: db 'BL.BIN'
;
;;...

;;KERNEL.BIN info:
.KERNEL_NAME: db 'KERNEL  BIN'
.KERNEL_FULLNAME: db 'KERNEL.BIN'
;;...

;GRAMADO NAME.
.OS_GRAMADO_NAME: DB 'Gramado'
.OS_GRAMADO_FULLNAME: DB 'Gramado Operating System'

;GENERIC OS.
;Espaço reservado para o sistema operacional colocar o nome.
.OS_NAME:     DD 0, 0      ;;8
.OS_FULLNAME: DD 0, 0, 0, 0   ;;16 @ aUMENTAR SE POSSÍVEL.	 



;;
;; Memory stuff.
;;

.TOTAL_MEMORY_INSTALLED: DD 0  ;Total de memória instalada.
.BASE_MEMORY: DD 0             ;640KB abaixo de 1MB.
.EXTENDED_MEMORY: DD 0         ;(Total instalado) - (1MB).
.USED_MEMORY: DD 0             ;( base + extended ). (memória disponível).
.UNUSED_MEMORY: DD 0		   ;(Total instalado) - ( base + extended ).

;;...



	

;;
;; Continua ... (*Lembrando que por enquanto temos apenas 1KB de tamanho).
;;
    
;
; @todo:
;  
;    Em 'Modo Real', usando BIOS, é possível detectar 
;    vários parametros do sistema.
;
;    (IMPORTANTE) O metafile pode ser editado por outros programas.
;
;


;;
;; Ponteiro para o endereço de memória onde deve ser carregado
;; o arquivo de configuração de inicialização do sistema.
;;
.INITIALIZATION_FILE_ADDRESS:
    DD 0
;;
;; *Para terminar, colocamos um ponteiro para o endereço de memória 
;;  onde deve ser carregado um arquivo com informações mais completas
;;  sobre o sistema.
;;
.SYSTEM_INFO_FILE_ADDRESS:   
    DD 0
    

;;#test
;; jail, p1, home, p2, castle.
.GRAMADO_MODE: 
    DB 0x00


META$FILE_END:	
    TIMES (2*512) - (META$FILE_END - META$FILE) DB 0
    ;TIMES (4*512) - (META$FILE_END - META$FILE) DB 0
    ;TIMES (8*512) - (META$FILE_END - META$FILE) DB 0
    ;;Almentaremos se for possível, lembrando que por enquanto
	;; o Boot manager te uma limitação de tamanho em 32 KB.
	;;...  	
;;--

;
;fim.
;

