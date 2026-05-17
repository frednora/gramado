
// byteswap.h

#ifndef __BYTESWAP_H
#define __BYTESWAP_H    1

// #include <sys/cdefs.h>

#define bswap_16(x)  (__builtin_bswap16(x))
#define bswap_32(x)  (__builtin_bswap32(x))
#define bswap_64(x)  (__builtin_bswap64(x))

#endif    

