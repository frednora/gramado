; int21.inc
; The interrupt 021h worker.


; called by the handler
interrupt21_worker:
    cmp al, 0x09
    je service09
    ret 

service09:
    ret

