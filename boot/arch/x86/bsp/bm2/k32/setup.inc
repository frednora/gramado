;
; File: msm\setup.inc 
;
; Descrição:
;     Faz configurações de hardware para a arquitetura x86.
;
; 2015 - Created by Fred Nora.
;


;=======================================================================
; setup_machine:
;     Configura a arquitetura.  ??obs: @todo: certificar que estamos um 32bit ??
;

setup_machine:

    ;; Interrupt enabling: 
    ;; Começaremos configurando suporte a interrupções.
    
    pushad

    cli

    ; GDT and IDT.

    lgdt [lgdt_opcode] 
    lidt [lidt_opcode]


	;; todo: 
	;; Nessa hora alguns vetores podem ser configurados.
	;; primeiro as excessões e depois as ISRs.

	;; Selecting the 'Processor Interrup Mode'.
	;; * PIC MODE *

    ;; Todos os componentes APIC são ignorados e o sistema opera
    ;; no modo single-thread usando LINT0.


;.setupPICMODE:

	; PIC.

	cli
	mov al, 00010001b		;begin PIC 1 initialization.
	out 0x20, al
	mov al, 00010001b		;begin PIC 2 initialization.
	out 0xA0, al	
	mov al, 0x20			;IRQ 0-7: interrupts 20h-27h.
	out 0x21, al
	mov al, 0x28			;IRQ 8-15: interrupts 28h-2Fh.
	out 0xA1, al
	mov al, 4
	out 0x21, al
	mov al, 2
	out 0xA1, al
	mov al, 1
	out 0x21, al
	out 0xA1, al
	
	;; #importante
	;; Vamos deixar o kernel inicializar o PIT.	
	
	;;@todo:
	;;Init RTC.

	
	;;step 
    ;;@todo: memory caching control.	
	
	; step 
	; processor Discovery and initialization
	; (cpuid threads and cores, start inter-processor interrupt sipi, 
	; ap wakeu pstate, wakeup vector alignment, caching consdarations,
	; ap idle state)	
	; apenas o básico para o boot manager.
	
		
	;;
	;; Fazendo alguma inicialização de dispositivos de I/O suportados.
	;;

	; step
	; i/o devices
	; ( embedded controller EC, super io SIO, legacy free systems, miscellaneous io devices)
    ; Nesse momento os dispositivos de i/o suportados aqui são (keyboard, IDE, graphic card).
	
	;;
	;; #bugbug
	;; Cancelado.
	;; Não precisamos disso, pois vamos configurar o teclado 
	;; durante a inicialização do i8042 no kernel.
	;;
	
	;KEYBOARD.
	;call keyboardInit


    ; IDE0.
    call init_ide0_master


    ;; LFB.
    ;; See: start.inc

    call setup_lfb 


	; #bugbug
	; #todo:
	; Aqui estamos usando constantes, porem esses valores precisam ser
	; detectados, pois variam à cada formatação.


	;Detectar os parâmetros do sistema de arquivos FAT16.
	;fat lba, root lba ...
	call fat16DetectFSParameters
	
	;Setup current dir.
    mov dword [g_current_fat_lba],        FAT16_FAT_LBA       ;Fat do volume atual
    mov dword [g_current_directory_lba],  FAT16_ROOTDIR_LBA   ;Diretório do volume atual.
	mov dword [g_current_dataarea_lba],   FAT16_DATAAREA_LBA  ;?? ;lba do inicio da area de dados do volume atual.
	;Continua...
	
    ;; Configurando PCI. (O básico suportado pelo Boot Manager.)
	;; O boot manager está carregando o BL utilizando recursos 
	;; do BIOS em modo real. Mas se o BM fosse carregar o BL 
	;; de um dispositivo SATA em 32bit teria que configurar o PCI.
	;; (Se possível deixaremos para o BL o penoso trabalho de lidar
	;; com PCI, já que o BL é feito em C, isso deve ser mais fácil para
	;; o BL do que para o BM.)
	
	;step
	;;PCI device descovery.	


	;CURSOR.
	call reset_cursor 

	;
	; Continua ...
	;
	
	; mask all interrupts.
	
	cli 
	mov al, 255
	out 0xA1, al
	out 0x21, al
	
	popad
	RET
	
;
; End.
;
