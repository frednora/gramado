// printk.c
// printk support.


#include <kernel.h>


/*
 * printk_old:
 *     @field 2
 *     The printk function.
 *     Assuming sizeof(void *) == sizeof(int).
 * Em user mode temos uma modelo mais tradicional de printk,
 * talvez seja bom implementa-lo aqui tambem.
 */
// #bugbug
// #todo:
// Devemos tentar usar o mesmo printk implementado na libc
// Essa aqui não está no padrão.
// #todo:
// Vamos substtuir essa função por uma de licensa bsd.
// Olhar na biblioteca.
// #suspensa
// Essa implementação foi feita para 32bit e não funciona
// por inteiro em long mode.
// Usaremos kinguio_printf por enquanto.

int printk_old( const char *format, ... )
{
    register int *varg = (int *) (&format);

// #bugbug:
// Se print() está usando '0' como buffer,
// então ele está sujando a IVT?
// #:
// print() nao analisa flags.

    return (int) print(0,varg);
}


// printk worker.
// Credits: Nelson Cole. Project Sirius/Kinguio.
int kinguio_printf(const char *fmt, ...)
{
    static char data_buffer[1024];
    int ret=0;

// If the virtual console isn't full initialized yet.
    if (Initialization.is_console_log_initialized != TRUE){
        return -1;
    }

    memset (data_buffer, 0, 1024); 

//----------
    va_list ap;
    va_start(ap, fmt);
    ret = kinguio_vsprintf(data_buffer, fmt, ap);
    va_end(ap);
//-----------

//
// Now we already have the formated string.
// Lets print it or send it to the serial port.
//

    if (Initialization.printk_to_serial == TRUE){
            return (int) debug_print_nbytes( 
                         (const void *) data_buffer, 
                         (size_t) sizeof(data_buffer) );
    } else {
        // Print the data buffer.
        kinguio_puts(data_buffer);
    };

    return (int) ret;
}
// ===================================


