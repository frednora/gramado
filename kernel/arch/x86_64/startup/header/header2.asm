; header2.asm
; Constants and macros.
; 2018 - Created by Fred Nora.

; Kernel segments.
;KERNEL_CS EQU  0x8
KERNEL_DS EQU  0x10
KERNEL_SS EQU  0x10

; ??
; Macro used by i/o delay.
; #todo: Explain it better.

%macro IODELAY 0 
    pushf
    popf
    jmp $+2
%endmacro

