; transfer.inc
; Transfer the execution to the kernel.
; 2023 - Created by Fred Nora.

;
; Bootblock
;

BOOTBLOCK_ADDRESS EQU  0x00090000
BOOTBLOCK_SIGNATURE EQU  1234

;
; Kernel image
;

;KERNEL_BASE        EQU  0x30000000
KERNEL_ENTRY_POINT EQU  0x30001000

; == Go to kernel ================================
; A parte em C salta para cá depois da inicializaçao,
; para enfim saltarmos para o kernel.
; Called by SetUpPaging() in pages.c.

global _BlTransferToKernel
_BlTransferToKernel:

; cr3
; Flush

    mov EAX, CR3  
    ; nop
    mov CR3, EAX

; cr0
; Enable paging to activate long mode
; Enable paging and protected mode.
; The paging was NOT enabled in pages.c

    mov ebx, cr0
    or ebx, 0x80000001 
    mov cr0, ebx 

; GDT
; Load the 64-bit global descriptor table.

    lgdt [GDT64.Pointer]

; Maybe we are still in compatibility mode,
; so, this way we can setup DS, ES and SS.
; #todo: Rever isso.
; Os modos de operação são determinados via CS.L e CS.D
; Modo 64-bit = CS.L = 1 e CS.D = 0
; Modo compatibilidade 32-bit CS.L = 0 e CS.D = 1
; Modo compatibilidade 16-bit CS.L = CS.D = 0

    xor eax, eax
    mov ax, 0x10    ;GDT64.Data
    mov ds, ax
    mov es, ax

    ;mov fs, ax
    ;mov gs, ax

;
; Go!
;

; IN:
; Temos um bootblock em 0x00090000.
; We filled the bootblock during the bl initialization,
; see the file head.s in this project.
; #todo: 
; Vamos precisa passar o 'Gramado mode',
; muita coisa no sistema depende disso.
; Isso funciona na maq real.
; Jump to the 64bit code in KERNEL.BIN.
; It will change the game and we will enter in 64bit long mode.
; See: _kernel_begin in head_64.asm.
; search for '__HEAD'.

    xor eax, eax
    mov ebx, dword BOOTBLOCK_ADDRESS
    xor ecx, ecx 
    mov edx, dword BOOTBLOCK_SIGNATURE

    jmp GDT64.Code:KERNEL_ENTRY_POINT

; Hang
bl_Loop2:
    cli
    hlt
    jmp bl_Loop2
