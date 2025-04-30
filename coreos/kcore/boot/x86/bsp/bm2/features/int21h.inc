; int21.inc
; The interrupt 021h worker.


; called by the handler
interrupt21_worker:

    ;mov si, msgzzz
    ;call DisplayMessage

    cmp al, 0x09
    je service09
    ret 

service09:
    ret


; ------------------------------------------------------------
; int21h_offset:
int21h_handler:
    PUSHA 

    mov si, int21h_message00
    call DisplayMessage   

; #TODO
; Dispatch a handler.
    ;CALL CS:[BX+DISPATCH]

    POPA 
    IRET 

int21h_message00:
    db "finish.inc: Hello from 21h", 13, 10, 0
