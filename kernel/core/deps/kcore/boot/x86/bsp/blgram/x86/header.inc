; header.inc
; Header for head.s

; ---------------
; i/o delay
%macro IODELAY 0 
    pushf
    popf
    jmp $+2
%endmacro


