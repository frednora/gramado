

extern __shared_main

global _x86crt0
_x86crt0:

     ;;#todo

     push eax   ;argc
	 push ebx   ;argv
	 
     call __shared_main

hang:
    ;pause
    jmp hang