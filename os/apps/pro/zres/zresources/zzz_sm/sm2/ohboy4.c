
#define	NULL	((void *)0)

// system call.
void *ohboy_system_call ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
    long __Ret = 0;

    // System interrupt.

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x80), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}


void main(void)
{
    
    // #todo
    // This is a place for 

    ohboy_system_call(0,0,0,0);
    //ohboy_system_call(0,0,0,0);
    //ohboy_system_call(0,0,0,0);
    //ohboy_system_call(0,0,0,0);
    
    
    // Get pid
    int pid = (int) ohboy_system_call(85,0,0,0);

    //get Heap address given the pid.
    void *heap;
    heap = (void*)ohboy_system_call(184,0,0,0);

    // Heap not valid.
    if ( (void*) heap == NULL )
        ohboy_system_call(65,'N',0,0);

    // Valid heap.
    if ( (void*) heap != NULL )
        ohboy_system_call(65,'Y',0,0);

    //ohboy_system_call(49,0,0,0);

    // console put char
    // ohboy_system_call(65,'x',0,0);

    //asm("int $3");
    //while(1){}

    // reboot
    //ohboy_system_call(110,0,0,0);

    // Debug on serial
    //char *s="fred";
    //ohboy_system_call(289,s,s,s);

    // Exit
    //ohboy_system_call(70,0,0,0);
    //while(1){
    //    ohboy_system_call(289,s,s,s);
    //}

    // List files.
    //ohboy_system_call(173,0,0,0);

    // refresh screen
    ohboy_system_call(11,0,0,0);
    
    // ok, a calling conventions funcionou,
    // os parametros estao em ordem.
    //ohboy_system_call(4321,0xa,0xb,0xc);
}













