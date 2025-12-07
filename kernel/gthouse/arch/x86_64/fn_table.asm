; fn_table.asm


; ----------------------
; Exporting ...
; see: mod.c
; see: newm0/ in the module.
; #todo: maybe we can have this table in another place.


    nop ; Nop
    DB '__GRAMADO__'   ; Signature
    DQ _xp_die                   ; symbol 0: (no arg)
    DQ _xp_putchar_test          ; symbol 1: (no arg)
    DQ _xp_reboot                ; symbol 2: (no arg)
    DQ _xp_refresh_screen        ; symbol 3: (no arg)
    DQ _xp_putchar_in_fgconsole  ; symbol 4: (1 arg)
    ; ...
; ----------------------
