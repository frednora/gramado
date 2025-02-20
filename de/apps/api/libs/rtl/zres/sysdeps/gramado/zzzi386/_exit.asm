
;; exit in nasm assembly i386


[bits 32]

extern __SYS_exit

global __exit
__exit:

    mov eax, dword [__SYS_exit]
	int 0x80
	
.hang:
    pause
	jmp .hang
	
	
	
	
	