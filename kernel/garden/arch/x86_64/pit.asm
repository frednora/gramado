; pit.as
; Early initialization of PIT device.

;============================================
; PIT
; Early PIT initialization.
; Setup system timers.
; Some frequencies to remember.
; PIT 8253 e 8254 = (1234DD) 1193181.6666 / 100 = 11930. ; 1.19MHz.
; APIC timer      = 3,579,545 / 100 = 35796  3.5 MHz.
; 11931    ; (1193181.6666 / 100 = 11930) timer frequency 100 HZ.
; Send 0x36 to 0x43.
; Send 11931 to 0x40.

PIT_early_initialization:

    xor rax, rax

    mov al, byte 0x36
    mov dx, word 0x43
    out dx, al

    IODELAY
    mov eax, dword 11931
    mov dx, word 0x40
    out dx, al

    IODELAY
    mov al, ah
    out dx, al

    IODELAY
    ret 
