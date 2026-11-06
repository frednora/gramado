// wink.c
// Wrappers for the wink/ kernel components.
// This is a safer way to call this services.
// Created by Fred Nora.

#include <kernel.h>


int config_use_progressbar = FALSE;

// =============================================



// =============================================


void 
wink_draw_string ( 
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string )
{
    if ((void*) string == NULL)
        return;
    if (*string == 0)
        return;
    draw_string(x, y, color, string);
}

void wink_update_progress_bar(unsigned long percent)
{
    unsigned long rectLeft=0;
    unsigned long rectTop=0;
    unsigned long rectWidth = gSavedX;  // (Device width) see: kmain.c
    unsigned long rectHeight = (8*3);   // Char height * 3  //28;
    unsigned int RectColor = COLOR_GRAY;

    unsigned long OneElement = (unsigned long) (rectWidth/100);
    if (percent > 100)
        percent = 100;
    rectWidth = (unsigned long) OneElement * percent;

// Check if we need to draw
    if (config_use_progressbar != TRUE)
        return;

// Draw
    backbuffer_draw_rectangle ( 
        rectLeft, rectTop, rectWidth, rectHeight, 
        RectColor, 
        0 );
// Flush
    refresh_rectangle(rectLeft, rectTop, rectWidth, rectHeight);
}

void wink_refresh_screen(void)
{
    refresh_screen();
}

// Draw and refresh a single char in the foreground console
void wink_putchar_in_fgconsole(unsigned long _char)
{
    int C = (int) (_char & 0xFF);

    //if (fg_console < 0)
        //return;
    console_outbyte(C, fg_console);
}


// Print the string into a box and halt carefully
void wink_x_panic(const char *final_string)
{
    x_panic(final_string);

    while (1){
        asm ("cli");  
        asm ("hlt"); 
    };
}

// We have a virtual console and we can use the printk.
// This is the first message in the screen.
// see: tty/console.c
void wink_show_banner(int clear_console)
{
// Called by keInitialize() in ke.c.

    char product_string[256];
    char build_string[256];
    size_t size=0;
    unsigned long BannerFlags=0;

    memset(product_string,0,256);
    memset(build_string,0,256);

// product string
    ksprintf(product_string,PRODUCT_NAME);
    size = sizeof(PRODUCT_NAME);
    if (size >= 256)
        return;
    product_string[size+1]=0;

// build string
    ksprintf(build_string,BUILD_STRING);
    size = sizeof(BUILD_STRING);
    if (size >= 256)
        return;
    build_string[size+1]=0;
    
// Crear screen and print version string
    PROGRESS("wink_show_banner:\n");
    if (clear_console == TRUE){
        BannerFlags = 0;
    } else {
        BannerFlags = 0x1000;
    };
    console_banner( product_string, build_string, BannerFlags );
}

// Setup Default kernel font.
// ROM BIOS 8x8 font.
// see: gre/font.c
void wink_initialize_default_kernel_font(void)
{
    font_initialize();
}

// see: bg.c
void wink_initialize_background(void)
{
    bg_initialize(COLOR_KERNEL_BACKGROUND, TRUE);
}

// See: 
// dev/chardev/display/bldisp/bldisp.c
void wink_initialize_video(void)
{
    //#breakpoint: BLACK ON WHITE.
    //ok, funcionou na maq real no modo jail, provavelmente 320x200.
    //for (i=0; i< 320*25; i++){ fb[i] = 0; };
    //while(1){asm("hlt");};

    Video_initialize();
    //PROGRESS("wink_initialize_video: Initialized\n");
}

// As estruturas de console sao estruturas de tty,
// mas sao um array de estruturas, nao precisa de malloc,
// por isso podem ser chamadas nesse momento.
// #test
// We need to test it better.
// see:
// dev/chardev/tty/console.c
// kstdio.c
void wink_initialize_virtual_consoles(void)
{
    int status = -1;
// The early initialization of the virtual consoles,
// it will happen again in kstdio.c if it fails here.
    status = (int) VirtualConsole_early_initialization();
    if (status < 0)
        x_panic("wink_initialize_virtual_consoles");
}

// 34 - Setup cursor for the current virtual console.
// See: core/system.c
// IN: x,y
// #todo: Essa rotina dever pertencer ao user/
void wink_set_cursor(unsigned long x, unsigned long y)
{

// #todo
// Maybe check some limits

    set_up_cursor((unsigned long) x, (unsigned long) y);
}

void wink_use_windowing_system(void)
{
    ibroker_use_kernelside_mouse_drawing(TRUE);
}

