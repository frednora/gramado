

#include <kernel.h>

// The list of namespaces in this machine.
// Only 32 containers.
struct ns_d ns[32];


//
// #
// INITIALIZE
//

int ns_initialize(int phase)
{
    int i=0;
    const int system_ns_id = 0;


    if (phase == 0){

        printk("ns_initialize: Phase 0\n");

        for (i=0; i<32; i++)
        {
            ns[i].used = TRUE;
            ns[i].magic = 1234;
            ns[i].initialized = FALSE;
            // ...
            ns[i].next = NULL;
        };

        // Initialize the first one. (system ns)
        ns[system_ns_id].te_list_base_address = (unsigned long) &teList[0];
        ns[system_ns_id].initialized = TRUE;

    } else if (phase == 1){

        printk("ns_initialize: Phase 1\n");
        // ...

    } else {

    };

    return 0;
}

