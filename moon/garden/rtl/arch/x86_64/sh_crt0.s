

extern __shared_main

global _x86_64_crt0
_x86_64_crt0:

     ;;#todo

     push rax   ;argc
	 push rbx   ;argv
	 
     call __shared_main

hang:
    ;pause
    jmp hang