// kstdlib.c
// 2015 - Fred Nora.

#include <kernel.h>

static int __randseed = 1234;

// --------------------------

static void *__kmalloc_impl(size_t size, int clean);

// --------------------------

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
    unsigned int seed = (unsigned int) (jiffies & 0xFFFFFFFF);

    srand(seed);
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

    if (sign){
        return (int) (-rv);
    }else{
        return (int) (rv);
    };

    //return (int) (sign ? -rv : rv);
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

// kmalloc implementation.
// IN: Clear or not the allocated memory.
// See: mm/mm.c
static void *__kmalloc_impl(size_t size, int clean)
{
    void *ptr;
    unsigned long ul_size = (unsigned long) (size & 0xFFFFFFFF);

// #todo
    //if (ul_size == 0)
        //ul_size = 1;

// IN: (unsigned long) size.
    ptr = (void *) heapAllocateMemory(ul_size);
    if (clean == TRUE){
        if ((void*) ptr != NULL){
            memset(ptr, 0, size);
        }
    }
    return (void*) ptr;
}

// kmalloc:
// Standard kmalloc function.
// Allocate memory into the kernel heap.
// Virtually and physically contiguous.
void *kmalloc(size_t size)
{
    void *ptr;
    size_t _lsize = size;
    struct process_d *process;
// Se devemos ou não incremetar o contador de uso.
    int IncrementUsageCounter=TRUE;  //P->allocated_memory

// Process structure
    process = (void*) get_current_process_pointer();
    if ((void*) process == NULL){
        IncrementUsageCounter=FALSE;
    }
    if (process->magic != 1234){
        IncrementUsageCounter=FALSE;
    }
// size
    if (_lsize < 0){
        debug_print("kmalloc: _lsize\n");
        return NULL;
    }
    if (_lsize == 0){
        debug_print("kmalloc: _lsize ajust\n");
        _lsize=1;
    }
// Allocate
    ptr = (void *) __kmalloc_impl(_lsize,FALSE);
    if ((void *) ptr == NULL){
        debug_print("kmalloc: ptr\n");
        return NULL;
    }
// Increment the usage conter for each process.
    if (IncrementUsageCounter == TRUE)
    {
        if ((void*) process != NULL)
        {
            if (process->magic == 1234)
            {
                // #todo:
                // A estrutura de processo pode ter
                // mais de um contador como esse,
                // registrando os diferentes tipos de memória
                // alocada pelo processo.
                process->allocated_memory += _lsize;
            }
        }
    }

    return (void *) ptr;
}

/*
 * kfree:
 * >> #importante:
 * >> Tradicionalmente essa função só libera o que foi alocado 
 * pela última chamada de malloc. Mas estamos tentando algo diferente.
 * The free() function frees the memory space pointed to by ptr, 
 * which must have been returned by a previous call 
 * to malloc(), calloc() or realloc(). 
 * Otherwise, or if free(ptr) has already been called before, 
 * undefined behavior occurs. 
 * >> If ptr is NULL, no operation is performed.
 * Importante:
 *     uma estratégia seria apenas sinalizarmos na estrutura que 
 * desejamos que o GC libere os recurso. Em seguida devemos sinalizar 
 * no mmblock que libere o bloco para outras alocações.
 */
// Virtually and physically contiguous.
// See:
// mm/mm.c
void kfree(void *ptr)
{
    if ((void *) ptr == NULL){
        debug_print("kfree: ptr\n");
        return;
    }
    heapFreeMemory(ptr);
}

// kcalloc: 
// Alloca e preenche com zero.
void *kcalloc(size_t count, size_t size)
{
    //void *ptr;
    size_t new_size = (size_t) (count * size);

// #todo
    //if (size <= 0)
        //size = 1;

    if (count <= 0){
        new_size = (1*size);
    }

    //ptr = (void*) kmalloc(new_size);
    //if ( (void*) ptr != NULL ){
    //    memset(ptr, 0, new_size);
    //}
    //return (void*) ptr;

    return (void*) __kmalloc_impl(new_size,TRUE);
}

