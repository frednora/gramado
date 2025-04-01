// dispsrv.c
// Handle the usermode display server initialization.
// Created by Fred Nora.

// #bugbug
// Remember: We already have a device drivers for display device
// in dev/.


#include <kernel.h>


int WindowServer_initialized=0;
    
window_server_t WindowServer_type=0;  // tipo de window server.
    
// the pid for loadable window servers.
// When the window server is the embedded so this pid needs to be
// the pid of the kernel process.
   
pid_t WindowServer_pid=0;

// Limiting the nale to 64.
char WindowServer_name[64];
   
// the virtual console used by this window server.
int WindowServer_virtual_console=0;

//see: dispsrv.h
struct ds_info_d DisplayServerInfo;


// ===============================================



void ws_show_info(void)
{
    printk("name %s\n",WindowServer_name);
    printk("initialized     %d\n", WindowServer_initialized);
    printk("type            %d\n", WindowServer_type);
    printk("pid             %d\n", WindowServer_pid);
    printk("virtual console %d\n", WindowServer_virtual_console);
    refresh_screen();
}


// Let's setup the window server.
// See:
// ws/model/ws.h
int 
ws_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    
    // #todo
    // We have a structure in ws.h
    
    debug_print("ws_ioctl: [TODO]\n");
    return -1;
}




// This is called at the kernel initialization to initialize
// the window server struct with the embedded window server info.
int ws_init(void)
{
    //debug_print ("ws_init: \n");


// The system starts with the embedded window server.
    WindowServer_type = WindowServerTypeEmbedded;

// #todo
// Use the pid of the kernel process.
    //WindowServer.pid = (pid_t) current_process;

// name.
    WindowServer_name[0] = 'k';
    WindowServer_name[1] = 'g';
    WindowServer_name[2] = 'w';
    WindowServer_name[3] = 's';
    WindowServer_name[4] = 0;
    
    WindowServer_virtual_console = 0;

    // ...

    WindowServer_initialized = TRUE;
    return 0;
}

//
// End
//

