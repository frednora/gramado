// kstdlib.c
// 2015 - Fred Nora.

#include <kernel.h>

static int __randseed = 1234;

// --------------------------


/*
// Simple xorshift32 PRNG
static uint32_t prng_state = 0x12345678;  // seed once at boot

static uint32_t xorshift32(void)
{
    uint32_t x = prng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    prng_state = x;
    return x;
}
*/

/*
// Wrapper for ISN salt
static uint32_t __get_random_32bit(void)
{
    return xorshift32();
}
*/


// ring0 srand:
// Alimenta a função rand.
// see: https://linux.die.net/man/3/srand.
void srand(unsigned int seed)
{
    __randseed = (unsigned int) seed;
}

// ring0 rand: 
// Gera um número inteiro semi-randômico. 
int rand(void)
{
    int ret_value=0;
    int extra_value = 8;
    unsigned int Seed = (unsigned int) (jiffies & 0xFFFFFFFF);

    srand(Seed);
    ret_value = (int) ((__randseed * extra_value) + extra_value);

    return (int) ret_value;
}

int abs(int j)
{
    int Value = (int) (j<0 ? -j : j);
    return (int) Value;
}

// atoi:
int k_atoi(const char *str)
{
    int rv=0; 
    char sign = 0;

    /* skip till we find either a digit or '+' or '-' */
    while (*str) 
    {
        if (*str <= '9' && *str >= '0')
            break;

        if (*str == '-' || *str == '+') 
            break;

        str++;
    }; 

    if (*str == '-')
        sign=1;

    // sign = (*s == '-');
    if (*str == '-' || *str == '+') 
        str++;

    while (*str && *str >= '0' && *str <= '9') 
    {
        rv = (rv * 10) + (*str - '0');
        str++;
    };

    //return (int) (sign ? -rv : rv);
    if (sign){
        return (int) (-rv);
    }
    return (int) (rv);
}

// Supporting the services 808 e 809.
// See: sci.c
// #todo: change the name to 'ubuf'.
int __ptsname(int fd, char *buf, size_t buflen)
{
    char *ptsname_buffer = (char *) buf;
    // #bugbug
    char test_str[50] = "__ptsname: test string";

// #todo: fd
    //if (fd<0)
        //return -1;

    if ((void*) buf == NULL){
        return (int) (-EINVAL);
    }
// 64 bytes limit
    strcpy( ptsname_buffer, (const char *) test_str );
// Lá na lib em ring3 a rotina retorna 
// para o app o ponteiro para o buffer
    return 0;
}

/*
 // See: https://en.wikipedia.org/wiki/Slab_allocation
void *slab_alloc (size_t size);
void *slab_alloc (size_t size)
{}
*/


// kmalloc:
// Standard kmalloc function.
// Allocate memory into the kernel heap.
// Virtually and physically contiguous.
// Call mm/ API for that.
void *kmalloc(size_t size)
{
    // #todo: hight level filters?

    return (void*) mm_kmalloc(size);
}

// kcalloc: 
// Allocate and fill with zeros
// See mm/
void *kcalloc(size_t count, size_t size)
{
    return (void*) mm_kcalloc(count, size);
}


// see: mm/
void kfree(void *ptr)
{
    if ((void*) ptr == NULL)
        return;

    mm_kfree(ptr);
}


