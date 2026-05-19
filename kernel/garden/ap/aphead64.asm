; aphead64.asm

; #todo
; This is gonna be the entrypoint for the AP processor in long mode.
; The BSP will share this new symbol with the trampoline,
; instead of sharing the old symbol that is inside the bsp's 
; initialization code.

; #todo

global AP_new_entry_point_for_long_mode
AP_new_entry_point_for_long_mode:
    cli
    hlt 


