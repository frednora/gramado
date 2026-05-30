; a20fast.inc
; Fat implementation for a20 initialization
; in order to safe space.

; -----------------------------------
; A20_fast_enable:
; Enable the A20 line using port 0x92.
; This is the preferred method on modern hardware,
; as it is simple and reliable.
A20_fast_enable:
    push ax
    in al, 0x92
    or al, 2
    out 0x92, al
    pop ax
    ret

