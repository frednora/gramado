; header1.asm
; For global assembly routines
; Is it valid only for nasm?
; Created by Fred Nora.

%define __HEAD  segment .head_x86_64

; Kernel segments.
;KERNEL_CS EQU  0x8
KERNEL_DS EQU  0x10
KERNEL_SS EQU  0x10


