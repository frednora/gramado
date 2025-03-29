;
; Arquivo: idt.inc
; Descrição:
;     A IDT do Boot Manager.
;     2015 - Fred Nora


;====================================================;
;  Idt.                                              ;
;====================================================;
idt:

;0 interrupt 0h
	dw unhandled_int    ; div error
	dw sys_code         ; seletor 8
	db 0
	db sys_interrupt
	dw 0

;1 interrupt 1h
	dw unhandled_int    ; debug exception
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;2 interrupt 2h
	dw unhandled_int    ; non maskable interrupt
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;3 interrupt 3h
	dw unhandled_int    ; int3 trap
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;4 interrupt 4h
	dw unhandled_int			   ; into trap
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;5 interrupt 5h
	dw unhandled_int			   ; bound trap
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;6 interrupt 6h
	dw unhandled_int		   ; invalid instruction
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;7 interrupt 7h
	dw unhandled_int		   ; no coprocessor
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;8 interrupt 8h
	dw unhandled_int 		   ; double fault
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;9 interrupt 9h
	dw unhandled_int	   ; coprocessor segment overrun 1 
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;10 interrupt Ah
	dw unhandled_int			   ; invalid tss
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;11 interrupt Bh
	dw unhandled_int		   ; segment not present
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;12 interrupt Ch
	dw unhandled_int			   ; stack fault
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;13 interrupt Dh
	dw unhandled_int			   ; general protection fault
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;14 interrupt Eh
	dw unhandled_int			   ; page fault
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;15 interrupt Fh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;16 interrupt 10h
	dw unhandled_int		   ; coprocessor error
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;17 interrupt 11h
	dw unhandled_int		   ; alignment check
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;18 interrupt 12h
	dw unhandled_int		   ; machine check 
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;19 interrupt 13h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;20 interrupt 14h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;21 interrupt 15h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;22 interrupt 16h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;23 interrupt 17h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;24 interrupt 18h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;25 interrupt 19h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;26 interrupt 1Ah
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;27 interrupt 1Bh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;28 interrupt 1Ch
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;29 interrupt 1Dh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;30 interrupt 1Eh
	dw unhandled_int		   ; reserved
	dw sys_code		
	db 0
	db sys_interrupt
	dw 0

;31 interrupt 1Fh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0


;
; IRQs ...
;

;
;32 interrupt 20h, irq 0, TIMER. 
	dw __irq0          
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

	
;33 interrupt 21h, Irq 1, KEYBOARD. 
	dw _irq1
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;34 interrupt 22h, IRQ2. 
	dw unhandled_irq  
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;35 interrupt 23h, IRQ3. 
	dw unhandled_irq   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;36 interrupt 24h, IRQ4. 
	dw unhandled_irq
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;37 interrupt 25h, IRQ5. 
	dw unhandled_irq
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;38 interrupt 26h, IRQ6, FLOPPY. 
	dw _irq6
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;39 interrupt 27h, IRQ7. 
	dw unhandled_irq
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;40 interrupt 28h, ; IRQ8, CLOCK. 
	dw _irq8
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;41 interrupt 29h, IRQ 9. 
	dw unhandled_irq
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;42 interrupt 2Ah, IRQ10. 
	dw unhandled_irq
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;43 interrupt 2Bh, IRQ11. 
	dw unhandled_irq
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;44 interrupt 2Ch, IRQ12. 
	dw unhandled_irq
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;45 interrupt 2Dh, IRQ13. 
	dw unhandled_irq
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;46 interrupt 2Eh, IRQ14, HD MASTER. 
	dw _irq14
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;47 interrupt 2Fh, IRQ15, HD SLAVE. 
	dw _irq15
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;
; ...
;

; ## SYSTEM INT ##
;48 interrupt 30h, 
	dw test_int30h 
	dw sys_code
	db 0
	db sys_interrupt
	dw 0


;49 interrupt 31h
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;50 interrupt 32h
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;51 interrupt 33h
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;52 interrupt 34h
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;53 interrupt 35h
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;54 interrupt 36h
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;55 interrupt 37h
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;56 interrupt 38h
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;57 interrupt 39h
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;58 interrupt 3Ah
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;59 interrupt 3Bh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;60 interrupt 3Ch
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;61 interrupt 3Dh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;62 interrupt 3Eh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;63 interrupt 3Fh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;64 interrupt 40h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;65 interrupt 41h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;66 interrupt 42h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;67 interrupt 43h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;68 interrupt 44h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;69 interrupt 45h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;70 interrupt 46h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;71 interrupt 47h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;72 interrupt 48h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;73 interrupt 49h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;74 interrupt 4Ah
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;75 interrupt 4Bh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;76 interrupt 4Ch
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;77 interrupt 4Dh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;78 interrupt 4Eh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;79 interrupt 4Fh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;80 interrupt 50h
	dw unhandled_int 		          ; INT 50H FUNCTIONS
	dw sys_code		
	db 0
	db sys_interrupt
	dw 0

;81 interrupt 51h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;82 interrupt 52h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;83 interrupt 53h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;84 interrupt 54h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;85 interrupt 55h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;86 interrupt 56h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;87 interrupt 57h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;88 interrupt 58h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;89 interrupt 59h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;90 interrupt 5Ah
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;91 interrupt 5Bh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;92 interrupt 5Ch
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;93 interrupt 5Dh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;94 interrupt 5Eh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;95 interrupt 5Fh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;96 interrupt 60h
	dw unhandled_int			   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;97 interrupt 61h 
	dw unhandled_int		   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;98 interrupt 62h 
	dw unhandled_int		   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;99 interrupt 63h 
	dw unhandled_int		   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;100 interrupt 64h 
	dw unhandled_int		   
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;101 interrupt 65h 
	dw unhandled_int		    
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;102 interrupt 66h 
	dw unhandled_int			    
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;103 interrupt 67h 
	dw unhandled_int		    ; 
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;104 interrupt 68h 
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;105 interrupt 69h 
	dw unhandled_int	    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;106 interrupt 6Ah 
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;107 interrupt 6Bh 
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;108 interrupt 6Ch 
	dw unhandled_int			    ; mouse
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;109 interrupt 6Dh 
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;110 interrupt 6Eh 
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;111 interrupt 6Fh 
	dw unhandled_int		    ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;112 interrupt 70h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;113 interrupt 71h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;114 interrupt 72h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;115 interrupt 73h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;116 interrupt 74h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;117 interrupt 75h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;118 interrupt 76h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;119 interrupt 77h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;120 interrupt 78h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;121 interrupt 79h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;122 interrupt 7Ah
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;123 interrupt 7Bh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;124 interrupt 7Ch
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;125 interrupt 7Dh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;126 interrupt 7Eh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;127 interrupt 7Fh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;128 interrupt 80h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;129 interrupt 81h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;130 interrupt 82h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;131 interrupt 83h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;132 interrupt 84h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;133 interrupt 85h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;134 interrupt 86h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;135 interrupt 87h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;136 interrupt 88h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;137 interrupt 89h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;138 interrupt 8Ah
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;139 interrupt 8Bh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;140 interrupt 8Ch
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;141 interrupt 8Dh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;142 interrupt 8Eh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;143 interrupt 8Fh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;144 interrupt 90h
	dw unhandled_int		   ; reserved
	dw sys_code		
	db 0
	db sys_interrupt
	dw 0

;145 interrupt 91h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;146 interrupt 92h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;147 interrupt 93h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;148 interrupt 94h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;149 interrupt 95h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;150 interrupt 96h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;151 interrupt 97h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;152 interrupt 98h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;153 interrupt 99h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;154 interrupt 9Ah
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;155 interrupt 9Bh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;156 interrupt 9Ch
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;157 interrupt 9Dh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;158 interrupt 9Eh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;159 interrupt 9Fh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;160 interrupt A0h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;161 interrupt A1h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;162 interrupt A2h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;163 interrupt A3h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;164 interrupt A4h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;165 interrupt A5h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;166 interrupt A6h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;167 interrupt A7h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;168 interrupt A8h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;169 interrupt A9h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;170 interrupt AAh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;171 interrupt ABh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;172 interrupt ACh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;173 interrupt ADh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;174 interrupt AEh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;175 interrupt AFh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;176 interrupt B0h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;177 interrupt B1h
	dw unhandled_int		   ; reserved
	dw sys_code		
	db 0
	db sys_interrupt
	dw 0

;178 interrupt B2h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;179 interrupt B3h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;180 interrupt B4h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;181 interrupt B5h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;182 interrupt B6h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;183 interrupt B7h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;184 interrupt B8h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;185 interrupt B9h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;186 interrupt BAh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;187 interrupt BBh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;188 interrupt BCh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;189 interrupt BDh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;190 interrupt BEh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;191 interrupt BFh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;192 interrupt C0h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;193 interrupt C1h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;194 interrupt C2h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;195 interrupt C3h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;196 interrupt C4h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;197 interrupt C5h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;198 interrupt C6h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;199 interrupt C7h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;200 interrupt C8h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;201 interrupt C9h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;202 interrupt CAh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;203 interrupt CBh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;204 interrupt CCh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;205 interrupt CDh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;206 interrupt CEh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;207 interrupt CFh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;208 interrupt D0h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;209 interrupt D1h
	dw unhandled_int		   ; reserved
	dw sys_code		
	db 0
	db sys_interrupt
	dw 0

;210 interrupt D2h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
;211 interrupt D3h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;212 interrupt D4h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;213 interrupt D5h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;214 interrupt D6H
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;215 interrupt D7h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;216 interrupt D8h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;217 interrupt D9h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;218 interrupt DAh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;219 interrupt DBh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;220 interrupt DCh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;221 interrupt DDh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;222 interrupt DEh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;223 interrupt DFh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;224 interrupt E0h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;225 interrupt E1h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;226 interrupt E2h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;227 interrupt E3h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;228 interrupt E4h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;229 interrupt E5h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;230 interrupt E6h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;231 interrupt E7h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;232 interrupt E8h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;233 interrupt E9h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;234 interrupt EAh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;235 interrupt EBh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;236 interrupt ECh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;237 interrupt EDh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;238 interrupt EEh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;239 interrupt EFh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;240 interrupt F0h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;241 interrupt F1h
	dw unhandled_int		   ; reserved
	dw sys_code		
	db 0
	db sys_interrupt
	dw 0

;242 interrupt F2h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;243 interrupt F3h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;244 interrupt F4h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;245 interrupt F5h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;246 interrupt F6h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;247 interrupt F7h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;248 interrupt F8h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;249 interrupt F9h
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;250 interrupt FAh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;251 interrupt FBh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;252 interrupt FCh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;253 interrupt FDh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;254 interrupt FEh
	dw unhandled_int		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0

;255 interrupt FFh
	dw unhandled_int ;system_interrupt		   ; reserved
	dw sys_code
	db 0
	db sys_interrupt
	dw 0
idt_end:
dd 0

;
; idtr
lidt_opcode:
    dw  (256*8) - (1)
    dd idt ;.long idt # + KRN_BASE
	
;
;fim
;

