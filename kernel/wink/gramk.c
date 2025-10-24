// gramk.c
// Wrappers for the gramk/ kernel components.
// This is a safer way to call this services.
// Created by Fred Nora.

#include <kernel.h>


int config_use_progressbar = FALSE;

// =============================================

static void __x_panic_show_message(const char *final_string, unsigned long flags);

// =============================================

void gramk_update_progress_bar(unsigned long percent)
{
    unsigned long rectLeft=0;
    unsigned long rectTop=0;
    unsigned long rectWidth = gSavedX;  // (Device width) see: kmain.c
    unsigned long rectHeight = (8*3); // Char height * 3  //28;
    unsigned int RectColor = COLOR_GRAY;

    unsigned long OneElement = (unsigned long) (rectWidth/100);
    if (percent > 100)
        percent = 100;
    rectWidth = (unsigned long) OneElement * percent;

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

// see: bldisp.c
void refresh_screen(void)
{
    bldisp_flush(0);
}

void gramk_refresh_screen(void)
{
    refresh_screen();
}

void gramk_putchar_in_fgconsole(unsigned long _char)
{
    int c = (int) (_char & 0xFF);

    //if (fg_console < 0)
        //return;

// Draw and refresh.
    console_outbyte2 ( c, fg_console );
}

// __x_panic_show_message:
// Draw a small red rectangle in the top/left
// and print a message into it.
static void __x_panic_show_message(const char *final_string, unsigned long flags)
{
    unsigned long rectLeft=0;
    unsigned long rectTop=0;
    unsigned long rectWidth = gSavedX;  // (Device width) see: kmain.c
    unsigned long rectHeight = (8*3); // Char height * 3  //28;
    unsigned int RectColor = COLOR_RED;

    unsigned long orLeft=rectLeft;
    unsigned long orTop=rectHeight;
    unsigned long orWidth = rectWidth;
    //unsigned long orHeight=1;
    unsigned int OrnamentColor = COLOR_YELLOW;

    unsigned long s1_left=8;
    unsigned long s1_top=8;
    unsigned int s1_color = COLOR_YELLOW;

    unsigned long s2_left=80;
    unsigned long s2_top=8;
    unsigned int s2_color = COLOR_WHITE;

// #todo
// Check the string size?

//---------------
// Box

    backbuffer_draw_rectangle ( 
        rectLeft, rectTop, rectWidth, rectHeight, 
        RectColor, 
        0 );

    backbuffer_draw_horizontal_line( 
        orLeft, orTop, orWidth,
        OrnamentColor, 
        0 );

//---------------
// Text

// string 1
    draw_string(s1_left,s1_top,s1_color,"PANIC: ");
// string2
    if ((void*) final_string != NULL){
        draw_string(s2_left,s2_top,s2_color,final_string);
    }

    refresh_screen();
}

void x_panic(const char *final_string)
{

// Print the string
    __x_panic_show_message(final_string,0);

// A soft place to fall.
    while (1){
        asm ("cli");  
        asm ("hlt"); 
    };
}

void gramk_panic(const char *final_string)
{

// Print the string
    __x_panic_show_message(final_string,1);

// A soft place to fall.
    while (1){
        asm ("cli");  
        asm ("hlt"); 
    };
}

// =================================
// Console:
// We have a virtual console and we can use the printk.
// This is the first message in the screen.
// see: tty/console.c
void gramk_show_banner(int clear_console)
{
// Called by keInitialize() in ke.c.

    char product_string[256];
    char build_string[256];
    size_t size=0;

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
    
// Crear screen and print version string.
    PROGRESS("gramk_show_banner:\n");

    unsigned long BannerFlags;
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
void gramk_initialize_default_kernel_font(void)
{
    font_initialize();
}

// see:
// gre/bg.c
void gramk_initialize_background(void)
{
    displayInitializeBackground(COLOR_KERNEL_BACKGROUND,TRUE);
    //PROGRESS("gramk_initialize_background: ok\n");
}

// See: 
// dev/chardev/display/bldisp/bldisp.c
void gramk_initialize_video(void)
{
    //#breakpoint: BLACK ON WHITE.
    //ok, funcionou na maq real no modo jail, provavelmente 320x200.
    //for (i=0; i< 320*25; i++){ fb[i] = 0; };
    //while(1){asm("hlt");};

    Video_initialize();
    //PROGRESS("gramk_initialize_video: Initialized\n");
}

// As estruturas de console sao estruturas de tty,
// mas sao um array de estruturas, nao precisa de malloc,
// por isso podem ser chamadas nesse momento.
// #test
// We need to test it better.
// see:
// dev/chardev/tty/console.c
// crt/kstdio.c
void gramk_initialize_virtual_consoles(void)
{
    int status = -1;
// The early initialization of the virtual consoles,
// it will happen again in kstdio.c if it fails here.
    status = (int) VirtualConsole_early_initialization();
    if (status < 0)
        x_panic("gramk_initialize_virtual_consoles");
}

// 34 - Setup cursor for the current virtual console.
// See: core/system.c
// IN: x,y
// #todo: Essa rotina dever pertencer ao user/
void gramk_set_cursor( unsigned long x, unsigned long y )
{

// #todo
// Maybe check some limits.

    set_up_cursor ( 
        (unsigned long) x, 
        (unsigned long) y );
}



// #bugbug
// gramkInitialize() was implemented in user.c
