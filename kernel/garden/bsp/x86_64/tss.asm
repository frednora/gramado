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

