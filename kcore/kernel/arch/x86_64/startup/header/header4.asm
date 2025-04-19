; header4.asm
; Created by Fred Nora.

;extern _Module0EntryPoint

; dirty: The registers are dirty.
global _asm_initialize_module0
_asm_initialize_module0:

    mov rdi, qword 1000   ; Reason = Initialize module.
    ;call _Module0EntryPoint
    ;call 0x30A00000
    ret

;=====================================
; _asm_reboot:
;     Reboot the system via ps2 keyboard.
; Steps:
; Wait for an empty Input Buffer with timeout, 
; but still send reboot command even if buffer stays busy.
;

PS2KEYBOARD_PORT        EQU  0x64
PS2KEYBOARD_CMD_REBOOT  EQU  0xFE

global _asm_reboot
_asm_reboot:

    xor rax, rax

; Timeout counter (arbitrary large value)
    mov rcx, 100000

; Get value and test status
; Testing for empty buffer with timeout.
.LNotEmpty:
    in al, PS2KEYBOARD_PORT
    test al, 00000010b
    je .LContinue ; Proceed if buffer becomes empty
    loop .LNotEmpty

; Continue sending the reboot command even if timeout occurs
.LContinue:

; (Write-Back and Invalidate Cache)
; Copy cache back to the main memory and invalidate cache.
; Forcing cache/RAM synchronization.
    wbinvd

; Send command
    mov al, PS2KEYBOARD_CMD_REBOOT
    out PS2KEYBOARD_PORT, al

; Infinite loop to halt
.Lloop:
    cli
    hlt
    jmp .Lloop



;=====================================
; Wrapper for reboot function.
unit4_reboot:
    jmp _asm_reboot

