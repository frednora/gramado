// libk.h

#ifndef __LIBK_LIBK_H
#define __LIBK_LIBK_H    1



int libk_initialize(void);

// ------------------
// crt prefix
#define crt_printf   kinguio_printf
#define crt_sprintf  mysprintf
#define crt_fseek    k_fseek
#define crt_rewind   k_rewind
#define crt_ftell    k_ftell
#define crt_fclose   k_fclose
// ...


// ------------------
// libk prefix
#define libk_printf   kinguio_printf
#define libk_sprintf  mysprintf
#define libk_fseek    k_fseek
#define libk_rewind   k_rewind
#define libk_ftell    k_ftell
#define libk_fclose   k_fclose
// ...


#endif    

