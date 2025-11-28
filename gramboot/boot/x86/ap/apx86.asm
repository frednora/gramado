[bits 16]
[org 0x20000]          ; code physically at 0x20000

; -- 128 KB mark -----------
; 0x20000 - Base for the AP
; 0x28FF0 - Start of 16bit stack
; 0x29000 - Signature position
; So we have 64 KB to use (192-128) 
; -- 192 KB mark -----------
; 0x00030000 - FAT (Don't touch this thing)

RM_STACK_SEGMENT  EQU 0x2000
RM_STACK_OFFSET   EQU 0x8FF0  ; Right before the signature

RM_SIGNATURE_SEGMENT  EQU 0x2000
RM_SIG1_OFFSET        EQU 0x9000 ; writes to physical 0x29000
RM_SIG2_OFFSET        EQU 0x9001

apx86_start:

;
; Stack
;

    cli
    mov ax, RM_STACK_SEGMENT 
    mov ss, ax
    mov sp, RM_STACK_OFFSET 

;
; Signature
;

    ; Switch to 0x2000 segment to reach 0x29000
    cld
    mov ax, RM_SIGNATURE_SEGMENT
    mov ds, ax
    mov es, ax
    mov byte [RM_SIG1_OFFSET], 0xA0 
    mov byte [RM_SIG2_OFFSET], 0xA0

;
; A soft place to fall
;

; Breakpoint
; Without this the system decreases the responsiveness
AP_LOOP:
    cli
    hlt
    jmp AP_LOOP
