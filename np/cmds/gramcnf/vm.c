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


int vm_print(struct object_d *o)
{
    if (!o) return -1;

    // For now, just dump the string
    printf("%s\n", o->token_buffer);

    // Later: handle expressions, formatting, metadata references, etc.
    return 0;
}


static int __vm_push00(struct object_d *obj, int index)
{
    if ((void*)obj == NULL)
        goto fail;
    if (index < 0)
        goto fail;
    if (index >= VM_STACK_COUNT_MAX)
        goto fail;

    vm_stack[index] = (unsigned long) obj;  // Save pointer

    printf("__vm_push00: %d\n", obj->opcode);
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


// 1) Get an object into the stack of objects
// 2) switch over the o->opcode of the current object
int vm_loop(void)
{
    struct object_d *o;

    VMInfo.state = VM_STATE_RUNNING;

    while (1){

    if (VMInfo.state == VM_STATE_SHUTTING_DOWN)
        break;

    // Get object
    o = (struct object_d *) vm_get();

    if ((void*) o == NULL){
        printf ("vm_loop: Invalid object\n");
        goto fail;
    }

    printf("OP: %d\n", o->opcode);

    switch (o->opcode){

        case 0:
            break;

        case OP_EOF:  // The last object
            printf("vm_loop: OP_EOF\n");
            exit(0);
            break;

        case OP_EXIT:
            printf("vm_loop: OP_EXIT\n");
            VMInfo.state = VM_STATE_SHUTTING_DOWN;
            break;

        case OP_PRINT:
            //printf("VM: print => %s\n", o->token_buffer);
            vm_print(o);
            break;

        default:
            break;
    };

    VMInfo.pc++;  // Increment PC

    };  // End of while

    return 0;

fail:
    return (int) -1;
}

int vm_initialize(void)
{
    VMInfo.initialized = FALSE;
    VMInfo.state = VM_STATE_NULL;
    VMInfo.pc = 0;  // Program counter
    VMInfo.last = 0;  // Index for the last object

//...

    VMInfo.initialized = TRUE;
    return 0;
}


