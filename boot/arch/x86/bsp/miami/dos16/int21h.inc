; int21.inc
; The interrupt 021h worker.


DOS125_MAXCALL  EQU    36


; called by the handler
interrupt21_worker:

    ;mov si, msgzzz
    ;call DisplayMessage

    cmp al, 0x09
    je service09
    ret 

service09:
    ret

DOS125_BADCALL:
    MOV AL, 0
DOS125_IRET:
    IRET


; ------------------------------------------------------------
;System call entry point and dispatcher
;int21h_ENTRY:
; int21h_offset:
int21h_handler:

; Classic DOS programs: They set AH=function_number before int 21h.
; Function number in CL: DOS 1.25’s dispatcher expected the service index in CL.
; Function number in AH: DOS 2.00 standardized on AH.

; These early calls remained available in DOS 2.00+
; CONIN (AH=01h) → Read character from keyboard, echoed.
; CONOUT (AH=02h) → Write character to console.
; Print String (AH=09h) → Output $‑terminated string.
; Terminate Program (AH=00h in DOS 1.x, later standardized as AH=4Ch).

    ;; #TODO: PUSH AND POP THE REGISTERS USING THE SAME
    ;; ROUTINE FOUND IN DOS 1.25

    ;; #TODO
    ;; We can make the whole gramado operates using the
    ;; open source DOS structures. (MIT) 16/32/64.
    ;; Reusing definitions, indexes and tables.


    ;POP     AX              ;IP from the long call at 5
    ;POP     AX              ;Segment from the long call at 5
    ;POP  WORD [.TEMP]  ;CS:[TEMP]       ;IP from the CALL 5

    ;PUSHF                   ;Start re-ordering the stack
    ;CLI
    ;PUSH    AX              ;Save segment
    ;PUSH  WORD [.TEMP]  ;CS:[TEMP]       ;Stack now ordered as if INT had been used

    ; Limits in DOS 1.25
    ;CMP     CL, DOS125_MAXCALL     ;This entry point doesn't get as many calls
    ;JA      DOS125_BADCALL
    ;MOV     AH, CL

    ; Limits in DOS 2.00 and above
    ;CMP AH, DOS125_MAXCALL
    ;JA DOS125_BAD_CALL

.SAVREGS:
; --------------------------------------
;PUSHA 

        PUSH    ES
        PUSH    DS

        PUSH    BP

        PUSH    DI
        PUSH    SI

        PUSH    DX
        PUSH    CX
        PUSH    BX
        PUSH    AX

    ;    IF      DSKTEST
    ;    MOV     AX,CS:[SPSAVE]
    ;    MOV     CS:[NSP],AX
    ;    MOV     AX,CS:[SSSAVE]
    ;    MOV     CS:[NSS],AX
    ;    POP     AX
    ;    PUSH    AX
    ;    ENDIF

        MOV  WORD  [.SPSAVE], SP  ;CS:[SPSAVE], SP
        MOV  WORD  [.SSSAVE], SS  ;CS:[SSSAVE], SS
        MOV     SP, CS
        MOV     SS, SP


; Calling Convention (DOS 2.00+)
; AH = 4Ch → Terminate Program.
; AL = exit code → returned to the parent process.
; Effect: Ends the current program, cleans up, and passes AL back.

    CMP AH, 0x4C ; terminate program
    JE .TERMINATE_PROGRAM

    CMP AH, 0x30 ; Get DOS version
    JE .GET_DOS_VERSION

    ;CMP AH, 0x4C ; terminate program
    ;JE .TERMINATE_PROGRAM

    JMP .REDISP

; Calling Convention (DOS 2.00+)
; AH = 4Ch → Terminate Program.
; AL = exit code → returned to the parent process.
; Effect: Ends the current program, cleans up, and passes AL back.
.TERMINATE_PROGRAM:
    ; Nothig for now
    XOR AX, AX
    JMP .LEAVE

; AH=30h
.GET_DOS_VERSION:
    mov ax, 0200h   ; DOS version 2.00 (AH=02, AL=00)
    ;mov ax, 0300h
    ;mov ax, 0400h
    MOV WORD [.RETURN_VALUE], AX
    mov bh, 00h     ; OEM number
    mov bl, 00h     ; version flag
    MOV WORD [.TEMP], BX
    JMP .LEAVE

; ...

.REDISP:


; Index calculation  
; You already load AH = CL (the function number).
; Each entry in the table is a word (2 bytes), so you multiply the index by 2:

        ;MOV     SP, OFFSET DOSGROUP:IOSTACK
        ;STI                     ;Stack OK now
    
        ;XOR     BX, BX
        MOV     BL, AH  ; function number
        MOV     BH, 0   ; clear high byte
        SHL     BX, 1   ; BX = index * 2

        CLD
        ;CMP     AH, 12
        ;JLE     .SAMSTK
        ;MOV     SP, OFFSET DOSGROUP:DSKSTACK

.SAMSTK:

    ;mov si, int21h_message00
    ;call DisplayMessage   

; #TODO
; Dispatch a handler.

    ; CALL CS:[BX+DISPATCH]
    ; CALL [INT21H_DISPATCH + BX]

; NASM-Compatible Dispatcher

    ; BX now points to the correct entry in the table
    ; Each entry is a word (offset of handler)

    call    word [cs:INT21H_DISPATCH + bx]

.LEAVE:

    ;CLI
    MOV     SP, WORD [.SPSAVE]
    MOV     SS, WORD [.SSSAVE]

    ;MOV     BP, SP
    ;MOV     BYTE PTR [BP.AXSAVE], AL

    ;    IF      DSKTEST
    ;    MOV     AX,CS:[NSP]
    ;    MOV     CS:[SPSAVE],AX
    ;    MOV     AX,CS:[NSS]
    ;    MOV     CS:[SSSAVE],AX
    ;    ENDIF


; --------------------------------------
;POPA 
        POP     AX
        POP     BX
        POP     CX
        POP     DX

        POP     SI
        POP     DI

        POP     BP

        POP     DS
        POP     ES

        ; AX
        MOV AX, WORD [.RETURN_VALUE]

        ; BX
        MOV BX, WORD [.TEMP]

        ;STI
        IRET

.RETURN_VALUE:
    DW 0 
.TEMP:
    DW 0 
.SPSAVE:  DW 0
.SSSAVE:  DW 0

; -------------------------------------
; DISPATCH TABLE for DOS 1.25
INT21H_DISPATCH:
    DW      DOS125_ABORT           ;0
    DW      DOS125_CONIN
    DW      DOS125_CONOUT
    DW      DOS125_READER
    DW      DOS125_PUNCH
    DW      DOS125_LIST            ;5
    DW      DOS125_RAWIO
    DW      DOS125_RAWINP
    DW      DOS125_IN
    DW      DOS125_PRTBUF
    DW      DOS125_BUFIN           ;10
    DW      DOS125_CONSTAT
    DW      DOS125_FLUSHKB
    DW      DOS125_DSKRESET
    DW      DOS125_SELDSK
    DW      DOS125_OPEN            ;15
    DW      DOS125_CLOSE
    DW      DOS125_SRCHFRST
    DW      DOS125_SRCHNXT
    DW      DOS125_DELETE
    DW      DOS125_SEQRD           ;20
    DW      DOS125_SEQWRT
    DW      DOS125_CREATE
    DW      DOS125_RENAME
    DW      DOS125_INUSE
    DW      DOS125_GETDRV          ;25
    DW      DOS125_SETDMA
    DW      DOS125_GETFATPT
    DW      DOS125_GETFATPTDL
    DW      DOS125_GETRDONLY
    DW      DOS125_SETATTRIB       ;30
    DW      DOS125_GETDSKPT
    DW      DOS125_USERCODE
    DW      DOS125_RNDRD
    DW      DOS125_RNDWRT
    DW      DOS125_FILESIZE        ;35
    DW      DOS125_SETRNDREC
; Extended Functions
    DW      DOS125_SETVECT
    DW      DOS125_NEWBASE
    DW      DOS125_BLKRD
    DW      DOS125_BLKWRT          ;40
    DW      DOS125_MAKEFCB
    DW      DOS125_GETDATE
    DW      DOS125_SETDATE
    DW      DOS125_GETTIME
    DW      DOS125_SETTIME         ;45
    DW      DOS125_VERIFY

;
; THE HANDLERS
;

;XXXXX:
;DOS125_ABORT:           ;0
;DOS125_CONIN:
;DOS125_CONOUT:
DOS125_READER:
DOS125_PUNCH:
DOS125_LIST:            ;5
DOS125_RAWIO:
DOS125_RAWINP:
;DOS125_IN:
;DOS125_PRTBUF:
DOS125_BUFIN:           ;10
DOS125_CONSTAT:
DOS125_FLUSHKB:
DOS125_DSKRESET:
DOS125_SELDSK:
DOS125_OPEN:            ;15
DOS125_CLOSE:
DOS125_SRCHFRST:
DOS125_SRCHNXT:
DOS125_DELETE:
DOS125_SEQRD:           ;20
DOS125_SEQWRT:
DOS125_CREATE:
DOS125_RENAME:
DOS125_INUSE:
DOS125_GETDRV:          ;25
DOS125_SETDMA:
DOS125_GETFATPT:
DOS125_GETFATPTDL:
DOS125_GETRDONLY:
DOS125_SETATTRIB:       ;30
DOS125_GETDSKPT:
DOS125_USERCODE:
DOS125_RNDRD:
DOS125_RNDWRT:
DOS125_FILESIZE:        ;35
DOS125_SETRNDREC:
; Extended Functions
DOS125_SETVECT:
DOS125_NEWBASE:
DOS125_BLKRD:
DOS125_BLKWRT:          ;40
DOS125_MAKEFCB:
DOS125_GETDATE:
DOS125_SETDATE:
DOS125_GETTIME:
DOS125_SETTIME:         ;45
DOS125_VERIFY:
    MOV AL, 0
    RET

; (Terminate Program)
DOS125_ABORT:
    MOV AL, 0
    RET



DOS125_CONIN:  ;System call 1
    CALL    DOS125_IN
    ; #ps: In DOS 2.00+ it does not echo the char.
    ;PUSH    AX
    ;CALL    DOS125_OUT
    ;POP     AX
    RET

; The CONIN loop depended on the Zero Flag (ZF) to indicate 
; whether a character was available.
; If BIOS reported “no character,” ZF=1 → the loop repeats.
; If a character was returned, ZF=0 → exit loop with AL holding the character.
DOS125_IN:     ;System call 8
    CALL    DOS125_INCHK
    ;JZ      DOS125_IN
;RET29:  
    RET


DOS125_INCHK:
    CALL DOS125_BIOSIN
    RET

; DOS125_BIOSIN
; Replacement for BIOSIN in DOS 1.25
; Calls our Stage 2 worker Io.GetCh
; Output: AL = character read

DOS125_BIOSIN:
    call Io.GetCh
    ; AL = character
    ret


; Input parameter:
; Character to print is passed in DL.
; Output:
; The character is written to the standard output (usually the console).
; Registers:
; DOS doesn’t require you to preserve AX or DL after the call, 
; but it’s good practice to leave them unchanged unless you’re returning a status.

DOS125_CONOUT: ;System call 2
    MOV  AL, DL
DOS125_OUT:
    
; If the character is below ASCII 20h (non‑printable control codes), 
; DOS branches to a routine (CTRLOUT) that interprets them specially (like BEL, BS, CR, LF, etc.).
; These aren’t sent directly to the console — they trigger cursor movement, beeps, etc.

    CMP AL, 20H
    JB  DOS125_CTRLOUT  ;CTRLOUT

; ASCII 7Fh is DEL. DOS treats it as a control character too, 
; so it jumps into the same path as the other non‑printables.

    CMP  AL, 7FH
    JZ  DOS125_OUTCH  ; DEL treated as printable by DOS 1.25

    ;INC  CS:BYTE PTR [CARPOS]

DOS125_OUTCH:
    
    ;PUSH    AX
    ;CALL    STATCHK
    ;POP     AX

; Printable characters (20h–7Eh) fall through and are sent to the 
; BIOS teletype output routine (BIOSOUT), which actually writes them to the screen.

    CALL DOS125_BIOSOUT

    ;CALL    FAR PTR BIOSOUT 
    ;TEST    CS:BYTE PTR [PFLAG], -1
    ;JZ      RET18

    ;CALL    FAR PTR BIOSPRINT 

RET18:
    RET


; -----------------------------------------------------------
; DOS125_BIOSOUT
; Prints the character in AL using BIOS teletype output (INT 10h, AH=0Eh).
; Parameters:
;   AL = character to print
;   BH = page number (usually 0)
;   BL = attribute (color, usually 07h for white on black)
; Output:
;   Character written to console
; -----------------------------------------------------------

DOS125_BIOSOUT:
    mov dl, al
    call Io.PutCh
    ret


DOS125_CTRLOUT:
    cmp  al, 07h          ; BEL
    jz   .bell
    cmp  al, 08h          ; BS
    jz   .backspace
    cmp  al, 0Dh          ; CR
    jz   .carriage
    cmp  al, 0Ah          ; LF
    jz   .linefeed
    ret

.bell:
    ; TODO: beep speaker
    ret

.backspace:
    push ax
    push bx
    push dx

    mov ah, 03h        ; get cursor position
    mov bh, 0
    int 10h

    dec dl             ; move left one column
    mov ah, 02h        ; set cursor position
    int 10h

    pop dx
    pop bx
    pop ax
    ret

.carriage:
    ; TODO: move cursor to column 0
    ret

.linefeed:
    ; TODO: move cursor down
    ret


DOS125_PRTBUF: ;System call 9
        MOV     SI, DX
;DOS125_OUTSTR:
        ;LODSB
        ;CMP     AL, "$"
        ;JZ      RET20
        ;CALL    DOS125_OUT
        ;JMP     SHORT DOS125_OUTSTR
    CALL DisplayMessageDollar
    RET

; -----------------------------------------------------------
int21h_message00:
    db "finish.inc: Hello from 21h", 13, 10, 0
