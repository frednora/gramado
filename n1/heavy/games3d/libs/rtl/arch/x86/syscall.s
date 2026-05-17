
;;#todo

global _x86_system_call

_x86_system_call:

    push ebx
	push ecx
	push edx
	
    int 0x80
	
	pop edx
	pop ecx 
	pop ebx
	
	ret