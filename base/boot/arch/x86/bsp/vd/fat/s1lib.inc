; s1lib.inc
; Functions for stage1.asm

; -----------------------------------------
; DisplayMessage:
; Display ASCIIZ string at "ds:si" via BIOS.
; Standadr print string routine.
DisplayMessage:
    lodsb                  ; Load next character.
    or al, al              ; Test for NUL character.
    jz .DONE
    mov ah, 0x0E           ; BIOS teletype. 
    mov bx, 0x0007         ; Página e atributo.  
    int 0x10               ; Invoke BIOS.
    jmp  DisplayMessage
.DONE:
    RET

; -----------------------------------------
; XXX
FAILURE:
    ; int 0x18  
    mov si, msgFailure
    call DisplayMessage
    mov ah, 0x00
    int 0x16        ; Await keypress
    int 0x19        ; Warm boot computer



