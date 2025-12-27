// main.c - dbox application
// A minimal client-side GUI app for Gramado OS.
// No windows, no event loop. Just connect and show a dialog box.

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
        printf("minimal_dialogbox: Failed to open display\n");
        return EXIT_FAILURE;
    }

    client_fd = (int) Display->fd;
    if (client_fd <= 0){
        printf("minimal_dialogbox: Invalid fd\n");
        return EXIT_FAILURE;
    }

    // 2. Call the dialog box API
    // Parent window = 0 (no parent, since we have no windows)
    result = gws_dialog_box(
                 client_fd,
                 0,                       // parent window
                 "Do you want to continue?", // text
                 DIALOG_YESNO );          // type

    // 3. Print the return value
    // For dialog box:
    //   1 → YES
    //   0 → NO
    //  -1 → Error
    if (result == 1) {
        printf("DialogBox returned: YES\n");
    } else if (result == 0) {
        printf("DialogBox returned: NO\n");
    } else {
        printf("DialogBox returned: Invalid value (%d)\n", result);
    }

    return EXIT_SUCCESS;
}
