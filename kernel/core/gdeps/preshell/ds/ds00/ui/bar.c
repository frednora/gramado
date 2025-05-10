
// bar.c
// Draw and update the notification bar. (yellow bar) (status bar).
// #todo 
// Change the name of this file.
// Created by Fred Nora.

#include "../gwsint.h"


struct statusbar_info_d  StatusBarInfo;


struct yellow_dialog_info_d  YellowDialogInfo;

// ===================================

// yellow bar. (rectangle not window)
// developer status.
void yellowstatus0(const char *string,int refresh)
{
// System notifications?
// Combinations

// methods. get with the w.s., not with the system.
    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();

// String offset
// right after the bmp icon. (16x16)
    unsigned long offset_string1 = 48;  //( 8*1 );
    //unsigned long offset_string2 = ( 8*5 );
    //unsigned long bar_size = w;
    //struct gws_window_d *aw;

    if (WindowManager.initialized != TRUE)
        return;


    // Initializing
    if (StatusBarInfo.initialized != TRUE)
    {
        StatusBarInfo.style = 1;  // full
        //StatusBarInfo.style = 2;  // partial
        StatusBarInfo.initialized = TRUE;
    }


/*
    // Working Area
    unsigned long Left   = 0;  //WindowManager.wa.left;
    unsigned long Top    = 0;  // WindowManager.wa.top;
    unsigned long Width  = w;  //WindowManager.wa.width;
    unsigned long Height = 24; //WindowManager.wa.height;
*/
/*
    unsigned long Left   = WindowManager.wa.left;
    unsigned long Top    = WindowManager.wa.height - 24;
    unsigned long Width  = w;  //WindowManager.wa.width;
    unsigned long Height = 24; //WindowManager.wa.height;
*/

    // Initializing
    unsigned long Left   = WindowManager.wa.width >> 1;
    unsigned long Top    = WindowManager.wa.height - 24;
    unsigned long Width  = WindowManager.wa.width >> 1;
    unsigned long Height = 24; //WindowManager.wa.height;

    unsigned long DefaultHeight = 24;

    // Based on style
    int Style = StatusBarInfo.style;
    switch (Style)
    {
        // full
        case 1:
            Left   = WindowManager.wa.left;
            Top    = (WindowManager.wa.height - DefaultHeight);
            Width  = WindowManager.wa.width;
            Height = DefaultHeight;
            break;
        // partial
        case 2:
            Left   = (WindowManager.wa.left + 24);
            Top    = (WindowManager.wa.height - DefaultHeight);
            Width  = (WindowManager.wa.width -24 -24);
            Height = DefaultHeight;
            break;
        // full
        default:
            Left   = WindowManager.wa.left;
            Top    = (WindowManager.wa.height - DefaultHeight);
            Width  = WindowManager.wa.width;
            Height = DefaultHeight;
            break;
    };


    unsigned long bar_size = Width;
    unsigned int bar_color = COLOR_WHITE;

// Validation
    //aw = (struct gws_window_d *) windowList[active_window];
    //aw = (void*) active_window;
    //if ( (void*) aw == NULL ){
    //    return;
    //}
    //if (aw->magic!=1234){
    //    return;
    //}

    //if(aw->type!=WT_OVERLAPPED){
    //    return;
    //}

    //debug_print ("yellow_status:\n");
    
    //#todo
    //if ( (void*) string == NULL ){ return; }
    //if ( *string == 0 ){ return; }


// paint the bar into the backbuffer
    painterFillWindowRectangle ( 
        Left, Top, bar_size, Height, 
        bar_color, 0 );

// paint the bmp into the backbuffer
// dont show it.
// bar = 24
// 24-16 = 8;
// 8/2=4
    bmp_decode_system_icon( 
        (int) 1,  // icon id 
        (unsigned long) Left +4, 
        (unsigned long) Top +4,
        FALSE );  // dont show it

// Paint the string, but dont show it.
    grDrawString ( 
        Left + offset_string1, 
        Top  +8, 
        COLOR_BLACK, 
        string );

    //grDrawString ( offset_string2, 8, COLOR_BLACK, "FPS" );

    if (bar_size == 0){
        bar_size = 32;
    }

// Mostra o retângulo.
    if (refresh){
        gws_refresh_rectangle( Left, Top, bar_size, Height );
    }
}

void yellow_status(const char *string)
{
    if ( (void*)string==NULL ){
        return;
    }
    yellowstatus0(string,TRUE);
}

int 
yellow_status_dialog (
    int msg,
    unsigned long long1,
    unsigned long long2,
    unsigned long long3 )
{
    if (YellowDialogInfo.display_dialog == TRUE){
        yellow_status("DIALOG: Reboot system?");
        YellowDialogInfo.display_dialog = FALSE;
    }

    // break f11
    if (msg == GWS_SysKeyUp && long1 == VK_F11)
    {
        YellowDialogInfo.display_dialog = FALSE;
        YellowDialogInfo.useYellowDialog = FALSE;
        return 0;
    }

    // break f12
    if (msg == GWS_SysKeyUp && long1 == VK_F12)
    {
        YellowDialogInfo.display_dialog = FALSE;
        YellowDialogInfo.useYellowDialog = FALSE;
        // #debug: Only for debug.
        wm_reboot();
    }

    return 0;
}

int yellow_dialog_initialize(void)
{
    YellowDialogInfo.initialized = TRUE;
    YellowDialogInfo.display_dialog = FALSE;
    YellowDialogInfo.useYellowDialog = FALSE;
    return 0;
}
