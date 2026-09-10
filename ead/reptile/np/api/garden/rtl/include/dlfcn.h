
//  dlfcn.h — dynamic linking

//from linux man 
/*
       RTLD_LAZY     Relocations are performed at an implementation-defined
                     time.
       RTLD_NOW      Relocations are performed when the object is loaded.
       RTLD_GLOBAL   All symbols are available for relocation processing of
                     other modules.
       RTLD_LOCAL    All symbols are not made available for relocation
                     processing by other modules.
*/ 

#define RTLD_DEFAULT  0
#define RTLD_LAZY     1
#define RTLD_NOW      2
#define RTLD_GLOBAL   3
#define RTLD_LOCAL    4

