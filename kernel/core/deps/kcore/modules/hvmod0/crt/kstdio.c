// kstdio.c
// Created by Fred Nora.

#include <kernel.h>


// ================================

// printk support.
static void __kstdio_puts(const char* str);
static char *_vsputs_r(char *dest, char *src);

// ================================

// Print a string.
static void __kstdio_puts(const char* str)
{
    register int i=0;
    ssize_t StringLen=0;
    int _char=0;

    if (ModuleInitialization.initialized != TRUE){
        return;
    }

    if (!str){
        return;
    }

    StringLen = (ssize_t) module_strlen(str);
    if (StringLen<=0)
        return;

// Print chars. 
    for (i=0; i<StringLen; i++)
    {
        _char = (int) (str[i] & 0xFF);
        caller1( kfunctions[FN_PUTCHAR_FGCONSOLE], _char ); 
    };
}

// local worker
static char *_vsputs_r(char *dest, char *src)
{
    unsigned char *usrc = (unsigned char *) src;
    unsigned char *udest = (unsigned char *) dest;

    while ( *usrc )
    { 
        *udest++ = *usrc++; 
    };

    return (char *) udest;
}

void newm0_print_string (char *s)
{
    register int i=0;
    size_t size=0;

    size = module_strlen(s);
    if (size <= 0)
        return;

// print string
    for (i=0; i<size; i++){
        caller1( kfunctions[FN_PUTCHAR_FGCONSOLE], s[i] );
    };
}




char *kinguio_itoa (int val, char *str) 
{
    int value = val;
    char *valuestring = (char *) str;
    int min_flag=0;
    char swap=0; 
    char *p;

    if (0 > value)
    {
        *valuestring++ = '-';
        value = -____INT_MAX> value ? min_flag = ____INT_MAX : -value;
    }

    p = valuestring;

    do {
         *p++ = (char) (value % 10) + '0';
         value /= 10;
    } while (value);

    if (min_flag != 0)
    {
        ++*valuestring;
    }

    *p-- = '\0';

    while (p > valuestring)
    {
        swap = *valuestring;
        *valuestring++ = *p;
        *p-- = swap;
    };

    return str;
}

void 
kinguio_i2hex( 
    unsigned int val, 
    char *dest, 
    int len )
{
    char *cp;
    register int i=0;
    int x=0;
    unsigned n=0;  //??

    if (val == 0)
    {
        cp = &dest[0];
        *cp++ = '0';
        *cp = '\0';
        return;
    }

    n = val;
    cp = &dest[len];

    while (cp > dest)
    {
        x = (n & 0xF);
        n >>= 4;
        
        // #
        *--cp = x + ((x > (HEX_LEN+1)) ? 'A' - 10 : '0');
    };

    dest[len] = '\0';

    cp = &dest[0];

    for (i=0; i<len; i++)
    {
        if (*cp == '0'){
            cp++;
        }else{
            strcpy (dest,cp);
            break;
        };
    }

    cp = &dest[0];
    n = module_strlen(cp);

    memset( (dest+n), 0, (8-n) );
}

// Print a string.
void kinguio_puts(const char* str)
{
    if ((void*) str == NULL){
        return;
    }
    __kstdio_puts(str);
}

int 
kinguio_vsprintf(
    char *str, 
    const char *fmt, 
    va_list ap )
{
    char *str_tmp = str;
    int index=0;
    unsigned char u=0;
    int d=0;
    char c=0; 
    char *s;
    char buffer[256];  //#bugbug: Too short.
    char _c_r[] = "\0\0";

    while ( fmt[index] )
    {
        switch (fmt[index]){

        case '%':

            ++index;

            switch (fmt[index]){

            case 'c':
                *_c_r = c = (char) va_arg (ap, int);
                str_tmp = _vsputs_r(str_tmp,_c_r);
                break;

            case 's':
                s = va_arg (ap, char*);
                str_tmp = _vsputs_r(str_tmp,s);
                break;

            case 'd':
            case 'i':
                d = va_arg (ap, int);
                kinguio_itoa (d,buffer);
                str_tmp = _vsputs_r(str_tmp,buffer);
                break;

            case 'u':
                u = va_arg (ap, unsigned int);
                kinguio_itoa (u,buffer);
                str_tmp = _vsputs_r(str_tmp,buffer);
                break;

            case 'X':
            case 'x':
                d = va_arg (ap, int);
                kinguio_i2hex(d, buffer,8);
                str_tmp = _vsputs_r(str_tmp,buffer);
                break;

            default:
                str_tmp = _vsputs_r(str_tmp,"%%");
                break;
            }
            break;

        default:
            *_c_r = fmt[index];
            str_tmp = _vsputs_r(str_tmp,_c_r);
            break;
        }
        ++index;
    }

    return (int) ( (long) str_tmp - (long) str );
}


// printk implementation.
// Credits:
// Nelson Cole. Kinguio/Sirius OS.
int kinguio_printf(const char *fmt, ...)
{
    static char data_buffer[1024];
    int ret=0;

/*
// If the virtual console isn't full initialized yet.
    if (Initialization.console_log != TRUE){
        return -1;
    }
*/
    memset (data_buffer, 0, 1024); 

//----------
    va_list ap;
    va_start(ap, fmt);
    ret = kinguio_vsprintf(data_buffer, fmt, ap);
    va_end(ap);
//-----------

// Print the data buffer.
    kinguio_puts(data_buffer);

    return (int) ret;
}
// ===================================

// mysprintf: (ksprintf)
// Variable parameter form to achieve ksprintf.
int mysprintf(char *buf, const char *fmt, ...)
{
    int i=0;

// Write the fmt format string to the buffer buf 
    va_list args;
    va_start(args, fmt);
    i = kinguio_vsprintf(buf, fmt, args);
    va_end(args);

    return (int) i;
}

