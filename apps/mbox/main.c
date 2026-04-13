// main.c - mbox application
// A minimal client-side GUI app for Gramado OS.
// No windows, no event loop. Just connect and show a message box.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtl/gramado.h>
#include <gws.h>

int main(int argc, char *argv[])
{
    const char *display_name_string = "display:name.0";
    struct gws_display_d *Display;
    int client_fd = -1;
    int result = -1;

    // 1. Open display (connect to Display Server)
    Display = (struct gws_display_d *) gws_open_display(display_name_string);
    if ((void*) Display == NULL){
        printf("minimal_messagebox: Failed to open display\n");
        return EXIT_FAILURE;
    }

    client_fd = (int) Display->fd;
    if (client_fd <= 0){
        printf("minimal_messagebox: Invalid fd\n");
        return EXIT_FAILURE;
    }


// ============================================================
// #test
// Update the wproxy structure that belongs to this thread.

/*
    unsigned long m[10];
    int mytid = gettid();
    m[0] = (unsigned long) (mytid & 0xFFFFFFFF);

    // Frame/chrome rectangle
    m[1] = lWi.left;
    m[2] = lWi.top;
    m[3] = lWi.width;
    m[4] = lWi.height;

    // Client area rectangle
    m[5] = lWi.cr_left;
    m[6] = lWi.cr_top;
    m[7] = lWi.cr_width;
    m[8] = lWi.cr_height;

    sc80( 48, &m[0], &m[0], &m[0] );
*/

//
// #test: Expand non-client area.
//

    //int tid = gettid();
    //sc80( 45, tid, tid, tid );  // Expand non-client area (for testing)

    // 2. Call the message box API
    // Parent window = 0 (no parent, since we have no windows)
    result = gws_message_box(
                 client_fd,
                 0,                  // parent window <<<< #bugbug
                 "Hello from MessageBox!", // text
                 MSGBOX_INFO );      // style/type

    // 3. Print the return value
    // For message box: 1 means "OK" clicked, -1 means error
    printf("MessageBox returned: %d\n", result);

    return EXIT_SUCCESS;
}
