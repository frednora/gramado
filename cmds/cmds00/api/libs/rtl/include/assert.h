
// credits: serenity os



#ifdef DEBUG

__attribute__((noreturn)) void __assertion_failed (const char* msg, 
                                  const char* file, 
                                  unsigned line, 
                                  const char* func );

    //See: __PRETTY_FUNCTION__ , __FUNCTION__ and __func__

    #define assert(expr) ((expr) ? (void)0 : __assertion_failed (#    expr, __FILE__, __LINE__, __FUNCTION__) )
    #define ASSERT_NOT_REACHED() assert(false)

#else

    #define assert(expr)
    #define ASSERT_NOT_REACHED() __CRASH()
#endif



//ud2 - (Generates invalid opcode exception).
#define __CRASH()              \
    do {                     \
        asm volatile("ud2"); \
    } while (0)
    
#define ASSERT assert
#define RELEASE_ASSERT assert

// #testing: bsd style. 
#define _DIAGASSERT assert

// basic
//#define assert(check) do { if(!(check)) die("assert"); } while(0)


