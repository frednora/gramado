; header3.asm
; Created by Fred Nora.

; See:
; head_64.asm for the stack.

align 8
tss0:
    dd 0 ;reserved
    dq _rsp0Stack ;rsp0        #todo
    dq 0 ;rsp1
    dq 0 ;rsp2
    dd 0 ;reserved
    dd 0 ;reserved
    dq 0 ;ist1
    dq 0 ;ist2
    dq 0 ;ist3
    dq 0 ;ist4
    dq 0 ;ist5
    dq 0 ;ist6
    dq 0 ;ist7
    dd 0 ;reserved
    dd 0 ;reserved
    dw 0 ;reserved
    dw 0 ;IOPB offset
tss0_end:


align 8
tss1:
    dd 0 ;reserved
    dq _rsp0Stack ;rsp0        #todo
    dq 0 ;rsp1
    dq 0 ;rsp2
    dd 0 ;reserved
    dd 0 ;reserved
    dq 0 ;ist1
    dq 0 ;ist2
    dq 0 ;ist3
    dq 0 ;ist4
    dq 0 ;ist5
    dq 0 ;ist6
    dq 0 ;ist7
    dd 0 ;reserved
    dd 0 ;reserved
    dw 0 ;reserved
    dw 0 ;IOPB offset
tss1_end:


; ==================================
; Clear Nested Task bit in RFLAGS.
; bit 14 - NT, Nested task flag.
; Limpar a flag nt em rflags
; e dar refresh na pipeline. #todo
; Isso evita taskswitching via hardware quando em 32bit.

global _x64_clear_nt_flag
_x64_clear_nt_flag:
    push rax
    push rbx

    pushfq
    pop rax
    mov rbx, 0x0000000000004000
    not rbx
    and rax, rbx
    push rax
    popfq

    pop rbx
    pop rax
    ret

; Called by x64_init_gdt() in x64.c
global _gdt_flush
_gdt_flush:

    mov rax, rdi
    lgdt [rax]

    mov rax, 0x10  ; Hardcoded
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ret

;=================================================
; setup_idt:
;     Configura a IDT.
;     Dado o endereço da IDT, 
;     preenche todos os vetores, 
;     apontando para um só endereço. 'unhandled_int'.
;     See: sw.asm
;     called by xxxhead.asm
;     See: sw.asm
;
; 0xEE00 - (present, dpl=3, interrupt gate) (disable interrupts)
;
; 0x8E00 - (present, dpl=0, interrupt gate) (disable interrupts)
; In the case of trap gates the interrupt are not disable,
; allowing nesting interrupts. 
;
; Every interrupt will use
; 0xEE00 - (present, dpl=3, interrupt gate) (disable interrupts)
; It means that it cam be called from ring3 and the interrupts
; will be disabled.
;
setup_idt:

    ;pushad
    push rax
    push rcx
    push rdx
    
    ; #todo: x86_64: 64bit address
    mov rdx, qword unhandled_int  

    ; low 16 
    mov rax, rdx 
    mov word  [d_offset_15_0],  ax
 
    ; high 16
    mov rax, rdx
    shr rax, 16
    mov word  [d_offset_31_16], ax

    ; high 32
    mov rax, rdx
    shr rax, 32
    mov dword [d_offset_63_32], eax

    ;; The pointer. 64bit address
    mov rdi, qword _idt

    ;; The counter
    mov rcx, qword 256

; loop
rp_sidt:

    ;; #bugbug
    ;; Nao sei se essa operação eh possivel,
    ;; mas queremos mover de double em double

    ;==========================
    ; Primeiros 32 bits. 
    ; (offset low and selector)
    xor rax, rax
    mov eax, dword 0x00080000     ; Step1: selector = 0x0008 = cs  na parte alta.
    mov ax, word  [d_offset_15_0] ; Step2: uma parte do endereço na parte baixa (16 bits)
    mov dword [rdi+0], eax   

    ;=========================================
    ; segundos 32 bit.
    xor rax, rax
    mov ax, word [d_offset_31_16]
    shl rax, 16
    ;0xEE00 - (present, dpl=3, interrupt gate) (disable interrupts)
    mov ax, word 0xEE00
    mov dword [rdi+4], eax   

    ;=========================================
    ; terceiros 32 bit.
    xor rax, rax
    mov eax, dword [d_offset_63_32]
    mov dword [rdi+8], eax

    ;=========================================
    ; quartos 32 bit.
    xor rax, rax
    mov dword [rdi+12], eax

    ;; done: go to next

    add  rdi, qword 16    ;;  vai pra proxima entrada.
    dec rcx               ;;  decrementa o contador.

    jne rp_sidt  ; loop

    ; #bugbug
    ; No. We will do this later.
    ; lidt [IDT_register]

    pop rdx
    pop rcx
    pop rax
    ;popad

    ret

;Offset address used in the function above.
d_offset_15_0:  dw 0
d_offset_31_16: dw 0
d_offset_63_32: dd 0
;;====================================


; =============================================================
; _setup_system_interrupt: 
; Configura um vetor da IDT para a interrupção do sistema. 
; O endereço do ISR e o número do vetor são passados via argumento.
; IN:
;    rax = endereço. (callback)(endereço do handler)
;    rbx = número do vetor (0x80).(número da interrupção.)
;
; 0xEE00 - (present, dpl=3, interrupt gate) (disable interrupts)
;
; 0x8E00 - (present, dpl=0, interrupt gate) (disable interrupts)
;
; In the case of 'trap gates' the interrupt are not disable,
; allowing nesting interupts.
;
; Every interrupt will use
; >>> 0xEE00 - (present, dpl=3, interrupt gate) (disable interrupts)
; It means that it cam be called from ring3 and the interrupts
; will be disabled.
;
global _setup_system_interrupt
_setup_system_interrupt:

    push rax
    push rbx
    push rcx
    push rdx

; Endereço e índice na tabela.
    mov qword [d__address], rax  ; Endereço. 64bit
    mov qword  [d__number], rbx  ; Número do vetor.

; Calcula o deslocamento.
    xor rax, rax
    mov rax, qword  16
    mov rbx, qword [d__number]
    mul rbx
    ; O resuldado está em rax.
; Adiciona o deslocamento à base rdi.
; A base é o início da idt.
; lembra? O resultado estava em rax.
    mov rdi, qword _idt
    add rdi, rax

; Agora rdi contém o endereço de memória
; dentro da idt, onde desejamos contruir a entrada.

; Lidando com o endereço.
; Salva o endereço de 64bit em rdx.
    mov rdx, qword [d__address] 
; low 16 
    mov rax, rdx 
    mov word  [address_offset_15_0],  ax
; high 16
    mov rax, rdx
    shr rax, 16
    mov word  [address_offset_31_16], ax
; high 32
    mov rax, rdx
    shr rax, 32
    mov dword [address_offset_63_32], eax

;------------------
; Lembre-se: 
; rdi contém o endereço de memória
; dentro da idt, onde desejamos contruir a entrada.
; #bugbug
; Nao sei se essa operação eh possivel,
; mas queremos mover de double em double
; #bugbug
; Checar o que representa esse seletor.

;==========================
; Primeiros 32 bits. 
; (offset low and selector)
    xor rax, rax
    mov eax, dword 0x00080000            ; Step1: selector = 0x0008 = cs  na parte alta.
    mov ax, word  [address_offset_15_0]  ; Step2: uma parte do endereço na parte baixa (16 bits)
    mov dword [rdi+0], eax
;=========================================
; segundos 32 bit.
    xor rax, rax
    mov ax, word [address_offset_31_16]
    shl rax, 16
    ;0xEE00 - (present, dpl=3, interrupt gate) (disable interrupts)
    mov ax, word 0xEE00
    mov dword [rdi+4], eax
;=========================================
; terceiros 32 bit.
    xor rax, rax
    mov eax, dword [address_offset_63_32]
    mov dword [rdi+8], eax
;=========================================
; quartos 32 bit.
    xor rax, rax
    mov dword [rdi+12], eax
;-----------------

; Do not load.
; recarrega a nova idt
    ;lidt [IDT_register]

    pop rdx
    pop rcx
    pop rbx
    pop rax

    ret

d__address:  dq 0
d__number:   dq 0
;Offset address used in the function above.
address_offset_15_0:  dw 0
address_offset_31_16: dw 0
address_offset_63_32: dd 0
;;--
;=============================================

; =============================================================
; _setup_system_interrupt: 
; Configura um vetor da IDT para a interrupção do sistema. 
; O endereço do ISR e o número do vetor são passados via argumento.
; IN:
;    rax = endereço. (callback)(endereço do handler)
;    rbx = número do vetor (0x80).(número da interrupção.)
;
; 0xEE00 - (present, dpl=3, interrupt gate) (disable interrupts)
;
; 0x8E00 - (present, dpl=0, interrupt gate) (disable interrupts)
;
; In the case of 'trap gates' the interrupt are not disable,
; allowing nesting interupts.
;
; Every interrupt will use
; >>> 0x8E00 - (present, dpl=0, interrupt gate) (disable interrupts)
; In the case of trap gates the interrupt are not disable,
; allowing nesting interrupts. 
;

; Testing this for hw interrupts.
global _setup_system_interrupt_hw
_setup_system_interrupt_hw:

    push rax
    push rbx
    push rcx
    push rdx

; Endereço e índice na tabela.
    mov qword [__d__address], rax  ; Endereço. 64bit
    mov qword  [__d__number], rbx  ; Número do vetor.

; Calcula o deslocamento.
    xor rax, rax
    mov rax, qword  16
    mov rbx, qword [__d__number]
    mul rbx
    ; O resuldado está em rax.
; Adiciona o deslocamento à base rdi.
; A base é o início da idt.
; lembra? O resultado estava em rax.
    mov rdi, qword _idt
    add rdi, rax

; Agora rdi contém o endereço de memória
; dentro da idt, onde desejamos contruir a entrada.

; Lidando com o endereço.
; Salva o endereço de 64bit em rdx.
    mov rdx, qword [__d__address] 
; low 16 
    mov rax, rdx 
    mov word  [__address_offset_15_0],  ax
; high 16
    mov rax, rdx
    shr rax, 16
    mov word  [__address_offset_31_16], ax
; high 32
    mov rax, rdx
    shr rax, 32
    mov dword [__address_offset_63_32], eax

;------------------
; Lembre-se: 
; rdi contém o endereço de memória
; dentro da idt, onde desejamos contruir a entrada.
; #bugbug
; Nao sei se essa operação eh possivel,
; mas queremos mover de double em double
; #bugbug
; Checar o que representa esse seletor.

;==========================
; Primeiros 32 bits. 
; (offset low and selector)
    xor rax, rax
    mov eax, dword 0x00080000            ; Step1: selector = 0x0008 = cs  na parte alta.
    mov ax, word  [__address_offset_15_0]  ; Step2: uma parte do endereço na parte baixa (16 bits)
    mov dword [rdi+0], eax
;=========================================
; segundos 32 bit.
    xor rax, rax
    mov ax, word [__address_offset_31_16]
    shl rax, 16
    ;0x8E00 - (present, dpl=0, interrupt gate) (disable interrupts)
    mov ax, word 0x8E00
    mov dword [rdi+4], eax
;=========================================
; terceiros 32 bit.
    xor rax, rax
    mov eax, dword [__address_offset_63_32]
    mov dword [rdi+8], eax
;=========================================
; quartos 32 bit.
    xor rax, rax
    mov dword [rdi+12], eax
;-----------------

; Do not load.
; recarrega a nova idt
    ;lidt [IDT_register]

    pop rdx
    pop rcx
    pop rbx
    pop rax

    ret

__d__address:  dq 0
__d__number:   dq 0
;Offset address used in the function above.
__address_offset_15_0:  dw 0
__address_offset_31_16: dw 0
__address_offset_63_32: dd 0
;;--
;=============================================


;=============================================
; setup_faults:
;    Configura vetores da idt para faults.
; hw interrupts:
; see: hw1.asm

; using 0xEE00
; present, dpl=3, interrupt gate
; Interrupts desabled.

setup_faults:
    push rax
    push rbx

;#0
    mov rax, qword _fault_N0
    mov rbx, qword 0
    call _setup_system_interrupt
;#1  
    mov rax, qword _fault_N1
    mov rbx, qword 1
    call _setup_system_interrupt
;#2  
    mov rax, qword _fault_N2
    mov rbx, qword 2
    call _setup_system_interrupt
;#3  debug
    mov rax, qword _fault_N3
    mov rbx, qword 3
    call _setup_system_interrupt
;#4  
    mov rax, qword _fault_N4
    mov rbx, qword 4
    call _setup_system_interrupt
;#5  
    mov rax, qword _fault_N5
    mov rbx, qword 5
    call _setup_system_interrupt
;#6 - Invalid opcode
    mov rax, qword _fault_INVALID_OPCODE
    mov rbx, qword 6
    call _setup_system_interrupt
;#7  
    mov rax, qword _fault_N7
    mov rbx, qword 7
    call _setup_system_interrupt
;#8 - double fault
    mov rax, qword _fault_DOUBLE
    mov rbx, qword 8
    call _setup_system_interrupt
;#9  
    mov rax, qword _fault_N9
    mov rbx, qword 9
    call _setup_system_interrupt
;#10  
    mov rax, qword _fault_N10
    mov rbx, qword 10
    call _setup_system_interrupt
;#11  
    mov rax, qword _fault_N11
    mov rbx, qword 11
    call _setup_system_interrupt
;#12 - stack
    mov rax, qword _fault_STACK
    mov rbx, qword 12
    call _setup_system_interrupt
;#13 - general protection
    mov rax, qword _fault_GP
    mov rbx, qword 13
    call _setup_system_interrupt
;#14  
    mov rax, qword _fault_N14
    mov rbx, qword 14
    call _setup_system_interrupt
;#15 
    mov rax, qword _fault_N15
    mov rbx, qword 15
    call _setup_system_interrupt
;#16 
    mov rax, qword _fault_N16
    mov rbx, qword 16
    call _setup_system_interrupt
;#17  
    mov rax, qword _fault_N17
    mov rbx, qword 17
    call _setup_system_interrupt
;#18  
    mov rax, qword _fault_N18
    mov rbx, qword 18
    call _setup_system_interrupt
;#19 - Intel reserved.
    mov rax, qword _fault_N19
    mov rbx, qword 19
    call _setup_system_interrupt
;#20 - Intel reserved.
    mov rax, qword _fault_N20
    mov rbx, qword 20
    call _setup_system_interrupt
;#21 - Intel reserved.
    mov rax, qword _fault_N21
    mov rbx, qword 21
    call _setup_system_interrupt
;#22 - Intel reserved.
    mov rax, qword _fault_N22
    mov rbx, qword 22
    call _setup_system_interrupt
;#23 - Intel reserved.
    mov rax, qword _fault_N23
    mov rbx, qword 23
    call _setup_system_interrupt
;#24 - Intel reserved.
    mov rax, qword _fault_N24
    mov rbx, qword 24
    call _setup_system_interrupt
;#25 - Intel reserved.
    mov rax, qword _fault_N25
    mov rbx, qword 25
    call _setup_system_interrupt
;#26 - Intel reserved.
    mov rax, qword _fault_N26
    mov rbx, qword 26
    call _setup_system_interrupt
;#27 - Intel reserved.
    mov rax, qword _fault_N27
    mov rbx, qword 27
    call _setup_system_interrupt
;#28 - Intel reserved.
    mov rax, qword _fault_N28
    mov rbx, qword 28
    call _setup_system_interrupt
;#29 - Intel reserved.
    mov rax, qword _fault_N29
    mov rbx, qword 29
    call _setup_system_interrupt
;#30 - Intel reserved.
    mov rax, qword _fault_N30
    mov rbx, qword 30
    call _setup_system_interrupt
;#31 - Intel reserved.
    mov rax, qword _fault_N31
    mov rbx, qword 31
    call _setup_system_interrupt

    pop rbx
    pop rax
    ret  

;=====================================
; setup_vectors:
;     Setup some IDT vectors.
; hw interrupts:
; see: hw1.asm


setup_vectors:

    push rax
    push rbx 

; using 0x8E00
; present, dpl=0, interrupt gate

; 32 - Timer.
; Iniciamos um timer provisório, depois iniciaremos o definitivo.
; Recreating this entry in _turn_task_switch_on() in hw2.asm.
    mov rax,  qword unhandled_irq
    mov rbx,  qword 32
    ;call _setup_system_interrupt
    call _setup_system_interrupt_hw  ;#testing 0x8E00

; 33 - PS2 Keyboard.
; See: unit1hw.asm
    mov rax,  qword _irq1
    mov rbx,  qword 33
    ;call _setup_system_interrupt
    call _setup_system_interrupt_hw  ;#testing 0x8E00

; 40 - Clock, rtc.
    mov rax,  qword unhandled_irq
    mov rbx,  qword 40
    ;call _setup_system_interrupt
    call _setup_system_interrupt_hw  ;#testing 0x8E00

; fake nic
    ;mov rax,  qword unhandled_irq
    ;mov rbx,  qword 41
    ;call _setup_system_interrupt
    call _setup_system_interrupt_hw  ;#testing 0x8E00


; 44 - PS2 Mouse.
; See: unit1hw.asm
    mov rax,  qword _irq12
    mov rbx,  qword 44
    ;call _setup_system_interrupt
    call _setup_system_interrupt_hw  ;#testing 0x8E00

; 46 - ide
; irq 14
    mov rax,  qword unhandled_irq
    mov rbx,  qword 46
    ;call _setup_system_interrupt
    call _setup_system_interrupt_hw  ;#testing 0x8E00


; 47 - ide
; irq 15
    mov rax,  qword unhandled_irq
    mov rbx,  qword 47
    ;call _setup_system_interrupt
    call _setup_system_interrupt_hw  ;#testing 0x8E00

;
; == System calls ===========================
;

; using EE00
; present, dpl=3, interrupt gate

; System interrupts
; see: sw1.asm
; 128 - 0x80
; 129 - 0x81
; 130 - 0x82
; 131 - 0x83

; 0x80
    mov rax,  qword _int128
    mov rbx,  qword 128
    call _setup_system_interrupt  
; 0x81
    mov rax,  qword _int129
    mov rbx,  qword 129
    call _setup_system_interrupt  
; 0x82
    mov rax,  qword _int130
    mov rbx,  qword 130
    call _setup_system_interrupt  
; 0x83
    mov rax, qword _int131
    mov rbx, qword 131
    call _setup_system_interrupt  
; ...

;========================
; The callback restorer.
; It is called by the ring 3 application
; after the callback routine ends. Delivering the control back
; to the kernel.
; #
; Temos que terminar a rotina do timer e
; retornarmos para ring 3 com o contexto o último contexto salvo.
; #todo
; Explain it better.
; See: sw2.asm and hw2.asm

    mov rax,  qword _int198
    mov rbx,  qword 198
    call _setup_system_interrupt  

; =====================
; Called by the ring3 process at the initialization
; to enable the maskable interrupts.
; It drops the iopl to ring 0.
; 32~255.
; The init process is the first ring3 process
; to call this interrupt, enabling the PIT itnerrupt
; for the first time and then we have the multithead working.
; Uma interrupção para habilitar as interrupções mascaráveis.
; quem usará isso será a thread primária 
; dos processos em ring3, apenas uma vez.
; See: sw.asm

    mov rax,  qword _int199
    mov rbx,  qword 199
    call _setup_system_interrupt  

    ;; ...

    pop rbx
    pop rax

    ret

;;=================================================
;; # NIC #
;; O kernel chma isso provisoriamente para criar uma entrada
;; na idt para o nic Intel.
;; #bugbug: isso está em nicintel.c, 
;; mas precisa ser global para que todos possam usar.
;; talvez em kernel.h
;; isso funcionou, tentar configurar outras interupções com isso.

;; Isso foi declarado em nicintel.c
;;pegaremos o valor 41 e o endereço do handler.
extern _nic_idt_entry_new_number
;extern _nic_idt_entry_new_address
extern _IDT_register

global _asm_nic_create_new_idt_entry
_asm_nic_create_new_idt_entry:

    push rax
    push rbx

    xor rax, rax
    xor rbx, rbx

;; Isso é o endereço da rotina de handler, em assembly;
;; está em hw.asm
;; #bugbug: não usaremos o endereço enviado pois temos que configurar 
;; o EOI e a pilha da rotina de handler.

    mov rax, qword _irq9_nic_handler
    ;mov rax, qword [_nic_idt_entry_new_address]

; (41) ?
; Isso é o número da interrupção. 
; #bugbug: 
; Na virtualbox é 9 mas no qemu é 11?
; #todo
; We gotta get this number during the initialization
; and setup a valid number here not this hardcoded 41.

    ;mov rbx, qword [_nic_idt_entry_new_number]
    mov rbx, qword 41  ;32+9
    ;call _setup_system_interrupt
    call _setup_system_interrupt_hw  ;#testing 0x8E00

;; #test: 
;; Não sei se precisa carregar novamente.
;; ok, sem problemas.
    lidt [_IDT_register] 
    pop rbx
    pop rax
    ret 

; #test:
;Other atomic hardware primitives:
;    test and set (x86)
;    atomic increment (x86)
;    bus lock prefix (x86)
;global _atomic_increment_spinlock
;_atomic_increment_spinlock:
;    lock inc qword [__spinlock_test]
;    ret
;global _get_spinlock
;_get_spinlock:
;    mov rax, qword [__spinlock_test]
;    ret
;__spinlock_test: dq 0
;

