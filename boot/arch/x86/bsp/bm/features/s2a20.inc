;
; File: rm/s2a20.inc
; + A20_fast_enable: Enable a20 line using port 0x92.
; + A20_enable: Enable a20 line using keyboard.
; 2005 - Created by Fred Nora.
;

; -----------------------------------
; + A20_fast_enable: Enable a20 line using port 0x92.
A20_fast_enable:
    push ax
    in al, 0x92
    or al, 2
    out 0x92, al
    pop ax
    ret

; -----------------------------------
; + A20_enable: Enable a20 line using keyboard.
;     Habilitar a linha A20.
;     5 tentativas depois tenta outro método.
;++
A20_enable:

    cli
    mov cx, 5    ;Number of attempts.
    mov dx, 0x64
    jmp short .start_attempt1
; Small procedure to wait for the keyboard controller 
; to enter a state where it can accept commands.
.command_wait:
    in al, dx
    test al, 2
    jnz .command_wait
    ret
; Small procedure to return the status 
; of the '8042 output port' from the keyboard controller.
.get_data:
    call .command_wait
    mov al, 0xD0             ; Read port output
    out dx, al
; Wait for the data to be ready to be read.
.wait_data:               
    in al, dx
    test al, 1
    jz .wait_data
    in al, 0x60              ; Retrieve the data
    ret
; Try the first of the 2 methods.
.start_attempt1:
    call .get_data
    push ax
; Tell the controller that we want to write a byte to the output port.
    call .command_wait
    mov al, 0xD1
    out dx, al
; Quando o controlador está pronto para receber, 
; retire o byte da pilha, 
; configure o bit enable-A20, 
; e envie o byte modificado para a porta de saída.
    call .command_wait
    pop ax
    or al, 2
    out 0x60, al
; Leia de novo o byte para verificar se a mudança ocorreu. 
; Se ocorreu, então a rotina existe. 
; Caso contrário, tente as 5 vezes antes do método #2.
    call .get_data
    and al, 2
    jnz .done
    loop .start_attempt1
; If method #1 failed, try method #2.
; Re-initialize CX. 
; (note that CH is already zero, 
; because LOOP stops looping when CX is zero)
    mov cl, 5

; Attempt 2 - Segunda tentativa. Segundo método. 
.start_attempt2:
; Simplesmente envie um comando ao controlador 
; para ligar a linha a20.
    call .command_wait
    mov al, 0xDF
    out dx, al
; Leia o byte  pra ter certeza.
    call .get_data
    and al, 2
    jnz .done
    loop .start_attempt2
.done:
    sti
    RET
;--

;
; End
; 
