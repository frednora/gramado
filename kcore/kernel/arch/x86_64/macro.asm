; macro.asm

align 4
; ??
; Macro used by i/o delay.
; #todo: Explain it better.
%macro IODELAY 0 
    pushf
    popf
    jmp $+2
%endmacro


