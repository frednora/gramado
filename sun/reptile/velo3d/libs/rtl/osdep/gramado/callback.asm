
; o kernel vai saltar pra ca via iretq e vamos
; chamar o callback indicado pelo kernel
; depois retornarmos ao kernel via interrupçao.

extern _rtl_fn_table
; IN: rax = index.
global _rtlUserCallbackDispatcher
_rtlUserCallbackDispatcher:

; get the pointer for the function table
    mov rdx, _rtl_fn_table
    
    ; IN: rax has the index.
    ; rax = index.
    mov rcx, qword 8
    mul rcx
    ; result in rax.  rax= rax*rcx
    call rax
    
    ;call [ rdx + rax*8 ]

    ; Restorer
    ;int 0x??
    int 0x03
    

