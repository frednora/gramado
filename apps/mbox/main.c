// main.c - mbox application
// A minimal client-side GUI app for Gramado OS.
// No windows, no event loop. Just connect and show a message box.

#include <stdio.h>
#include <stdlib.h>
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
