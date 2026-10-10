
// #importante
// As system calls precisam usar os números do systema operacional alvo.
// Na verdade pode variar de acordo com a arquitetura no mesmo sistema operacional.


#ifdef __GRAMADO
#include <osrtl/gramado/syscall.h>
#endif


#ifdef __LINUX
#include <osrtl/linux/syscall.h>
#endif


//...





