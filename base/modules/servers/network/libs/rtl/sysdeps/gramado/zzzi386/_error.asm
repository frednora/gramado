
[bits 32]

global __syscall_error

__syscall_error:
    
	mov eax, dword 1
    ret
	
	
	