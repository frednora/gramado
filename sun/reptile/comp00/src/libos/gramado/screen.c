// screen.c
// Scress support.
// Created by Fred Nora.

#include "../../ds.h"

// The device screen.
// The refresh is called by refresh_device_screen();
// It is called explicit by the app.
struct gws_screen_d *DeviceScreen;



// Refresh the device screen
void refresh_device_screen(void)
{
    gws_show_backbuffer();
    refresh_device_screen_flag = FALSE;  // Validation
}

// Refresh the device screen
void refresh_screen(void){
    refresh_device_screen();
}

// Refresh the valid screen of the current display.
void refresh_valid_screen(void)
{
    //debug_print ("refresh_valid_screen:\n");

    if ( (void*) CurrentDisplay == NULL ){
        printf("refresh_valid_screen: [ERROR] CurrentDisplay\n");
        exit (1);
    }

//
// The valid SCREEN
//

    // Se a valid screen não existe.
    if ( (void*) CurrentDisplay->valid_screen == NULL ){
        debug_print ("refresh_valid_screen: [FAIL] No valid screen\n");
        return;
    }

    // A valid screen é justamente a screen do device.
    if ( CurrentDisplay->valid_screen == CurrentDisplay->device_screen )
    {
        debug_print ("refresh_valid_screen: show the device screen\n");
        gws_show_backbuffer();
    }

    refresh_valid_screen_flag = FALSE;  // VAlidation
}

