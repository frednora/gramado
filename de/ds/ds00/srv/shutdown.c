// shutdown.c
// Created by Fred Nora.

#include "../gwsint.h"

const char *shutdown_name = "shutdown.bin";

// ===============================

static void __launch_shutdown_cmd(void);

// ===============================

static void __launch_shutdown_cmd(void)
{
// Wait
// We're giving the chance for the applications
// to close themselves first.
    long Times=0;

    for (Times=0; Times<32; Times++){
        rtl_yield();
    };

// Launch a command that is gonna shutdown the machine
// depending on the archtechture we're running on.
    if ((void*) shutdown_name != NULL)
        rtl_clone_and_execute(shutdown_name);
}

void ServerShutdown(int server_fd)
{
    static char shutdown_string[64];

//-------
// Tell to the apps to close.
    gwssrv_broadcast_close();

    //server_debug_print ("GRAMLAND: ServerShutdown\n");
    //printf             ("GRAMLAND: ServerShutdown\n");

    memset(shutdown_string, 0 , 64);

// Clear root window.
// Show final message.

    if ((void*) __root_window != NULL)
    {
        // Clean window
        clear_window_by_id( __root_window->id, TRUE );
        
        yellow_status("Shutting down ...");
  
        // String
        strcat(shutdown_string,"ws: Shutting down ...");
        strcat(shutdown_string,"\0");
        dtextDrawText ( 
            (struct gws_window_d *) __root_window,
            8, 
            40, 
            (unsigned int) COLOR_WHITE, 
            shutdown_string );
        
        // Show the window and the final message.
        wm_flush_window(__root_window);
    }

    __launch_shutdown_cmd();

    DestroyAllWindows();

// Close the server's socket.
    if (server_fd < 0){
        goto done;
    }
    close(server_fd);
done:
    exit(0);
}


