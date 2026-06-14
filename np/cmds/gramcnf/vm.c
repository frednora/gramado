// vm.c
// Created by Fred Nora

#include "gramcnf.h"


struct vm_info_d  VMInfo;

// Stack of object pointers
unsigned long vm_stack[VM_STACK_COUNT_MAX];

// ===========================================

static int __vm_push00(struct object_d *obj, int index);
static struct object_d *__vm_get00(int index);

// ===========================================

static int __vm_push00(struct object_d *obj, int index)
{
    if ((void*)obj == NULL)
        goto fail;
    if (index < 0)
        goto fail;
    if (index >= VM_STACK_COUNT_MAX)
        goto fail;

    vm_stack[index] = (unsigned long) obj;  // Save pointer
    return 0;

fail:
    return (int) -1;
}

int vm_push(struct object_d *obj)
{
    int i = VMInfo.last;

    if (i<0)
        goto fail;
    if (i >= VM_STACK_COUNT_MAX)
        goto fail;
    VMInfo.last++;
    return (int) __vm_push00(obj,i);

fail:
    return (int) -1;
}

static struct object_d *__vm_get00(int index)
{
    if (index<0)
        goto fail;
    if (index >= VM_STACK_COUNT_MAX)
        goto fail;

    return (struct object_d *) vm_stack[index];

fail:
    return NULL;
}

struct object_d *vm_pop(void)
{
    if (VMInfo.last<0)
        goto fail;
    if (VMInfo.last >= VM_STACK_COUNT_MAX)
        goto fail;

    if (VMInfo.last > 0)
        VMInfo.last--;

    return (struct object_d *) __vm_get00(VMInfo.last);

fail:
    return NULL;
}


struct object_d *vm_get(void)
{
    int i = VMInfo.pc;

    if (i<0)
        goto fail;
    if (i >= VM_STACK_COUNT_MAX)
        goto fail;

    return (struct object_d *) __vm_get00(i);

fail:
    return NULL;
}

int vm_loop(void)
{
    // 1) Get an object into the stack of objects
    // 2) switch over the o->opcode of the current object


    // Get object
    // o = get obj ()

    //switch (o->opcode){
    //    case ...
    //}

    return 0;
}

int vm_initialize(void)
{
    VMInfo.initialized = FALSE;
    VMInfo.pc = 0;  // Program counter
    VMInfo.last = 0;  // Index for the last object

//...

    VMInfo.initialized = TRUE;
    return 0;
}


