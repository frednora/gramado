// vm.h

#ifndef __VM_H
#define __VM_H    1


struct vm_info_d 
{
    int dummy;
};
extern struct vm_info_d  VMInfo;


int vm_loop(void);
int vm_initialize(void);

#endif  

