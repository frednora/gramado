// vm.h
// Created by Fred Nora

#ifndef __VM_H
#define __VM_H    1


#define VM_STATE_NULL     0
#define VM_STATE_RUNNING  1
#define VM_STATE_SHUTTING_DOWN  2
// ...

struct vm_info_d 
{
    int initialized;
    int state;

//
// Index
//

    int last;  // Index for the last object
    int pc;    // Program counter
};
extern struct vm_info_d  VMInfo;


#define VM_STACK_COUNT_MAX  256
extern unsigned long vm_stack[VM_STACK_COUNT_MAX];


int vm_print(struct object_d *o);

// Put object into the stack
int vm_push(struct object_d *obj);
struct object_d *vm_pop(void);

// Get an object respecting the pc index.
struct object_d *vm_get(void);


int vm_loop(void);
int jsvm_initialize(void);

#endif  

