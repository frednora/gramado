// main.c
// Gramado OS client-side GUI app: Launch
// Launches #power.bin, #memory.bin or #sysinfo.bin.
// Respects the same event loop, pump, DC, shared flags and
// interaction pattern used by the Power Manager example.

// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

// The client-side library
#include <gws.h>

// #test
// The client-side library
#include <libgui.h>

#include "launch.h"

static int toggle_flag = 0;

static int isTimeToQuit = FALSE;

// Global display pointer
struct gws_display_d *Display;

struct dccanvas_d *dc00;  // shared dc

static unsigned long __sh_flags = 0;


struct ui_component_d *uic_button_power;
struct ui_component_d *uic_button_memory;
struct ui_component_d *uic_button_sysinfo;
struct ui_component_d *uic_footer;

struct button_info_d
{
    int button_id;

// This is the window id that represents the icon.
    int wid;

// Absolute values
    unsigned long absolute_left;
    unsigned long absolute_top;
    unsigned long width; 
    unsigned long height;

// Relative values
    unsigned long left;
    unsigned long top;

// The state of the icon, it also represents
// the state of the client application.
// (running, minimized, etc.).
    int state;
};
static struct button_info_d  MyButton_Power;
static struct button_info_d  MyButton_Memory;
static struct button_info_d  MyButton_Sysinfo;

static int __hover_button_id = -1; // Invalidate.


struct bmp_cache_d *icon00_cache;

// -----------------

// Window IDs
static int main_window     = -1;

static int __paint_is_needed = FALSE;

// Default responder (button to trigger on Enter)
static int default_responder = -1;

static void set_default_responder(int wid);
static void switch_responder(int fd);
static void trigger_default_responder(int fd);

static void on_button_clicked(int id);
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my);
static void update_children(int fd);

// =====================================================


/**
 * Generic syscall stub for x86_64.
 * num   The syscall number (placed in rax)
 * arg1  First argument (placed in rdi)
 * arg2  Second argument (placed in rsi)
 * arg3  Third argument (placed in rdx)
 * return      Result from the kernel (returned in rax)
 */

// "a"(num)  → RAX = syscall number
// "D"(arg1) → RDI = first argument
// "S"(arg2) → RSI = second argument
// "d"(arg3) → RDX = third argument

/*
unsigned long 
syscall3 (
    unsigned long num, 
    unsigned long arg1, 
    unsigned long arg2, 
    unsigned long arg3 ) 
{
    unsigned long ret=0;

    asm volatile (
        "syscall \n"
        : "=a" (ret)                    // Output: rax contains the return value
        : "a" (num),                    // Input: rax = syscall number
          "D" (arg1),                   // Input: rdi = arg1
          "S" (arg2),                   // Input: rsi = arg2
          "d" (arg3)                    // Input: rdx = arg3
        : "rcx", "r11", "memory"        // Clobbers: syscall changes rcx and r11
    );

    return (unsigned long) ret;
}
*/


// Handle click events for components
static void on_button_clicked(int id)
{
    if (id < 0)
        return;

    switch (id)
    {
        case 1:  // MyButton_Power
            printf("Button %d clicked! Launching #power.bin\n", id);
            rtl_clone_and_execute("#power.bin");
            // Do not exit — keep the launcher alive so the user can
            // launch more programs if desired.
            break;

        case 2:  // MyButton_Memory
            printf("Button %d clicked! Launching #memory.bin\n", id);
            rtl_clone_and_execute("#memory.bin");
            break;

        case 3:  // MyButton_Sysinfo
            printf("Button %d clicked! Launching #sysinfo.bin\n", id);
            rtl_clone_and_execute("#sysinfo.bin");
            break;

        default:
            printf("Unknown button clicked: %d\n", id);
            break;
    };
}


// Hit-test for our fake buttons in LaunchApp
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my) 
{

// Button Power
    if ( rel_mx >= MyButton_Power.left && 
         rel_mx <= MyButton_Power.left + MyButton_Power.width &&
         rel_my >= MyButton_Power.top  && 
         rel_my <= MyButton_Power.top + MyButton_Power.height )
    {
        return (int) MyButton_Power.button_id;
    }

// Button Memory
    if ( rel_mx >= MyButton_Memory.left && 
         rel_mx <= MyButton_Memory.left + MyButton_Memory.width &&
         rel_my >= MyButton_Memory.top  && 
         rel_my <= MyButton_Memory.top + MyButton_Memory.height )
    {
        return (int) MyButton_Memory.button_id;
    }

// Button Sysinfo
    if ( rel_mx >= MyButton_Sysinfo.left && 
         rel_mx <= MyButton_Sysinfo.left + MyButton_Sysinfo.width &&
         rel_my >= MyButton_Sysinfo.top  && 
         rel_my <= MyButton_Sysinfo.top + MyButton_Sysinfo.height )
    {
        return (int) MyButton_Sysinfo.button_id;
    }

    return -1;
}

static void update_children(int fd)
{
    struct gws_window_info_d  wi;

    // Get window info
    gws_get_window_info(fd, main_window, &wi);

    unsigned long button_w = wi.cr_width / 5;
    unsigned long button_h = wi.cr_height / 8;

    unsigned long button_y = (wi.cr_height - button_h) / 2;

    // Three buttons evenly spaced
    unsigned long power_x   = (wi.cr_width / 6)     - (button_w / 2);
    unsigned long memory_x  = (wi.cr_width / 2)     - (button_w / 2);
    unsigned long sysinfo_x = (5 * wi.cr_width / 6) - (button_w / 2);


// #test: We are still thinking about the bits configuration
//#define FLAG_DIRTY  0x0001  // client started drawing
//#define FLAG_READY  0x0002  // client finished drawing
//#define FLAG_ACK    0x0004  // server acknowledged


// -----------------

    if ((void*)dc00 == NULL)
        return;

// The background for the client area
    lingui_draw_rectangle0_dc (
        dc00,
        0, 0, wi.cr_width, wi.cr_height,
        COLOR_WHITE,
        0  // ROP
    );

// ------------------------------------------
// String

    libgui_drawstringblock_dc(
        dc00,
        8,
        8,
        COLOR_BLACK,
        "P=Power  M=Memory  I=Sysinfo  Q=Quit",
        2
    );

//
// Support for button positions and dimensions
//

// -------------------------------------------------
// Redraw the Power button

    // Relative values
    MyButton_Power.left = power_x;
    MyButton_Power.top  = button_y;

    // Update absolute values.
    MyButton_Power.absolute_left = 
        wi.left + wi.cr_left + MyButton_Power.left;
    MyButton_Power.absolute_top = 
        wi.top + wi.cr_top + MyButton_Power.top;

    libgui_set_ui_component_position(
        uic_button_power, 
        MyButton_Power.left, 
        MyButton_Power.top );
    libgui_set_ui_component_dimension(
        uic_button_power,
        button_w,
        button_h );
    libgui_set_ui_component_flags(uic_button_power, (0x0001 | 0x0002) );
    libgui_redraw_ui_component(uic_button_power, dc00);

// -------------------------------------------------
// Redraw the Memory button

    // Relative values
    MyButton_Memory.left = memory_x;
    MyButton_Memory.top  = button_y;

    // Update absolute values
    MyButton_Memory.absolute_left = 
        wi.left + wi.cr_left + MyButton_Memory.left;
    MyButton_Memory.absolute_top = 
        wi.top + wi.cr_top + MyButton_Memory.top;

    libgui_set_ui_component_position(
        uic_button_memory, 
        MyButton_Memory.left, 
        MyButton_Memory.top );
    libgui_set_ui_component_dimension(
        uic_button_memory,
        button_w,
        button_h );
    libgui_set_ui_component_flags(uic_button_memory, (0x0001 | 0x0002) );
    libgui_redraw_ui_component(uic_button_memory, dc00);

// -------------------------------------------------
// Redraw the Sysinfo button

    // Relative values
    MyButton_Sysinfo.left = sysinfo_x;
    MyButton_Sysinfo.top  = button_y;

    // Update absolute values
    MyButton_Sysinfo.absolute_left = 
        wi.left + wi.cr_left + MyButton_Sysinfo.left;
    MyButton_Sysinfo.absolute_top = 
        wi.top + wi.cr_top + MyButton_Sysinfo.top;

    libgui_set_ui_component_position(
        uic_button_sysinfo, 
        MyButton_Sysinfo.left, 
        MyButton_Sysinfo.top );
    libgui_set_ui_component_dimension(
        uic_button_sysinfo,
        button_w,
        button_h );
    libgui_set_ui_component_flags(uic_button_sysinfo, (0x0001 | 0x0002) );
    libgui_redraw_ui_component(uic_button_sysinfo, dc00);

//
// bmp
//

    if (icon00_cache && icon00_cache->loaded) 
    {
        bmp_decode_bmp_image(icon00_cache, dc00, 50, 50, 4); // zoom=4
    }

//
// footer
//

    libgui_set_ui_component_position(
        uic_footer, 0, wi.cr_height - 24 );
    libgui_set_ui_component_dimension(
        uic_footer, wi.cr_width, 24 );
    libgui_set_ui_component_flags(uic_footer, (0x0001 | 0x0002));
    libgui_redraw_ui_component(uic_footer, dc00);
}

static void set_default_responder(int wid)
{
    if (wid >= 0)
        default_responder = wid;
}

static void switch_responder(int fd)
{
    // Placeholder — can cycle focus among the three buttons later.
}

static void trigger_default_responder(int fd) 
{
    // Placeholder for Enter key.
}

// ----------------------------------------------------
// Procedure: handles events sent by the display server
// ----------------------------------------------------
static int 
launchProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    int ButtonId = -1;

    if (fd < 0)
        return (int) -1;

    if (event_window < 0)
        return (int) -1;
    if (event_type < 0)
        return (int) -1;

    switch (event_type) {

    // Null event
    case 0:
        return 0;
        break;

    // Redraw child windows
    case MSG_PAINT:
        update_children(fd);
        return 0;
        break;

    case MSG_KEYDOWN:
        switch (long1){

        case VK_RETURN:
            //trigger_default_responder(fd);
            break;

        case 'P':
        case 'p':
            printf("launch: P >> Power\n");
            rtl_clone_and_execute("#power.bin");
            break;

        case 'M':
        case 'm':
            printf("launch: M >> Memory\n");
            rtl_clone_and_execute("#memory.bin");
            break;

        case 'I':
        case 'i':
            printf("launch: I >> Sysinfo\n");
            rtl_clone_and_execute("#sysinfo.bin");
            break;

        // Quit the launcher itself
        case 'Q':
        case 'q':
            printf("launch: Send QUIT message\n");
            gws_async_command(fd,88,0,0);  // Send quit message
            isTimeToQuit = TRUE;
            break;

        };
        break;

    case MSG_SYSKEYDOWN:
        switch (long1) {
            case VK_F1:
                printf("LaunchApp: VK_F1 >> Power\n");
                rtl_clone_and_execute("#power.bin");
                return 0;
            case VK_F2:
                printf("LaunchApp: VK_F2 >> Memory\n");
                rtl_clone_and_execute("#memory.bin");
                return 0;
            case VK_F3:
                printf("LaunchApp: VK_F3 >> Sysinfo\n");
                rtl_clone_and_execute("#sysinfo.bin");
                return 0;
            case VK_F11:
                // Should not appear — broker intercepts fullscreen toggle
                printf("LaunchApp: VK_F11 (unexpected)\n");
                return 0;

            case VK_ARROW_LEFT: 
            case VK_ARROW_RIGHT: 
                switch_responder(fd); 
                break;

            // reserved for future
            case VK_ARROW_UP: 
            case VK_ARROW_DOWN: 
                printf("LaunchApp: Arrow up/down pressed (no action yet)\n"); 
                break;
        };
        break;

    case GWS_MouseClicked:
        break;

    // #test
    case MSG_MOUSEMOVE:
        // #bugbug
        // Kernel is sending us absolute values
        // instead of relative values.
        ButtonId = (int) __hit_test_button(long1, long2);
        if (ButtonId > 0)
            __hover_button_id = ButtonId;
        if (ButtonId <= 0)
            __hover_button_id = -1;
        break;

    case MSG_MOUSEPRESSED:
        break;

    case MSG_MOUSERELEASED:
        printf("launch: Button released: %d\n", __hover_button_id);
        on_button_clicked(__hover_button_id);
        break;

    case MSG_CLOSE:
        isTimeToQuit = TRUE;  // #test
        break;


    // Unknown event
    default:
        return -1;
        break;
    };

// Fail
    return (int) -1;
}

// ----------------------------------------------------
// Pump: fetches events from the server and dispatches
// ----------------------------------------------------
static void pump(int fd)
{
    struct gws_event_d event;
    event.used = FALSE;
    event.magic = 0;
    event.type = 0;

    struct gws_event_d *e;
    e = (struct gws_event_d *) gws_get_next_event(
        fd, (int) main_window, (struct gws_event_d *) &event );

    if ((void*) e == NULL)
        return;
    if (e->magic != 1234 || e->used != TRUE) 
        return;
    if (e->type <= 0)
        return;

    launchProcedure(fd, e->window, e->type, e->long1, e->long2);
}


int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    int client_fd = -1;

    isTimeToQuit = FALSE;

    // Connect to display server
    Display = gws_open_display(display_name);
    if ((void*) Display == NULL) {
        printf("launch_app: Could not open display\n");
        return EXIT_FAILURE;
    }

    client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("launch: Invalid fd\n");
        return EXIT_FAILURE;
    }

    // Screen size
    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

// =========================================
// Library initialization

    int status = -1;
    status = (int) libgui_initialize();
    if (status < 0){
        printf("launch_app: on libgui_initialize()\n");
        exit(1);
    }


// =========================================
// Main window

    unsigned long mw_style = WS_APP;

    unsigned long win_w = screen_w / 2;
    unsigned long win_h = screen_h / 2;
    unsigned long win_x = screen_w/4;
    unsigned long win_y = screen_h/4;

    main_window = 
        (int) gws_create_window(
                client_fd,
                WT_OVERLAPPED,
                WINDOW_STATUS_ACTIVE,  //status
                WINDOW_STATE_NULL,  //state
                "Launch",
                win_x, win_y, win_w, win_h,
                0,
                mw_style,  // style
                COLOR_WHITE, COLOR_GRAY );

    if (main_window < 0) {
        printf("launch_app: Failed to create main window\n");
        return EXIT_FAILURE;
    }

// After creating main_window,
// get information about it.
    struct gws_window_info_d wi;
    gws_get_window_info(
        client_fd,
        main_window,
        (struct gws_window_info_d *) &wi );

// ============================================================
// #test
// Update the wproxy structure that belongs to this thread.

    unsigned long m[10];
    int mytid = gettid();
    m[0] = (unsigned long) (mytid & 0xFFFFFFFF);

    // Frame/chrome rectangle
    m[1] = wi.left;
    m[2] = wi.top;
    m[3] = wi.width;
    m[4] = wi.height;

    // Client area rectangle
    m[5] = wi.cr_left;
    m[6] = wi.cr_top;
    m[7] = wi.cr_width;
    m[8] = wi.cr_height;

    sc80( 48, &m[0], &m[0], &m[0] );


// ============================================================
// Getting the flag earlier. This way we can use it in the loop.

    __sh_flags = (unsigned long) wi.sh_flags;

// ============================================================
// Create a device context structure
// based on the information we got with the server.
// This is a dc to draw into the client area.

    dc00 = (struct dccanvas_d *) libgui_create_dc(
        wi.ca_canvas_base_address,
        wi.ca_canvas_width,
        wi.ca_canvas_height,
        wi.ca_canvas_bpp
    );
    if ((void*)dc00 == NULL){
        printf("launch: on dc00\n");
        exit(1);
    }

// bg for the client area
    lingui_draw_rectangle0_dc (
        dc00,
        0, 0, wi.cr_width, wi.cr_height,
        COLOR_WHITE,
        0  // ROP
    );

// String

    libgui_drawstringblock_dc(
        dc00,
        8,
        8,
        COLOR_BLACK,
        "P=Power  M=Memory  I=Sysinfo  Q=Quit",
        2
    );

// ============================================================

//
// Support for button positions and dimensions
//

    unsigned long button_w = wi.cr_width / 5;
    unsigned long button_h = wi.cr_height / 8;
    unsigned long button_y = (wi.cr_height - button_h) / 2;

    unsigned long power_x   = (wi.cr_width / 6)     - (button_w / 2);
    unsigned long memory_x  = (wi.cr_width / 2)     - (button_w / 2);
    unsigned long sysinfo_x = (5 * wi.cr_width / 6) - (button_w / 2);

// ============================================================
// Create Power button

    MyButton_Power.button_id = 1;

    // Relative values
    MyButton_Power.left = power_x;
    MyButton_Power.top  = button_y;

    // Absolute coordinates (relative to screen)
    MyButton_Power.absolute_left = wi.left + wi.cr_left + MyButton_Power.left;
    MyButton_Power.absolute_top  = wi.top + wi.cr_top + MyButton_Power.top;
    MyButton_Power.width         = button_w;
    MyButton_Power.height        = button_h;

// Create a button
    uic_button_power = libgui_create_ui_component (
        dc00, 
        1,   // type = button 
        MyButton_Power.left, 
        MyButton_Power.top, 
        MyButton_Power.width, 
        MyButton_Power.height,
        "Power",
        (0x0001 | 0x0002)
    );

// ============================================================
// Create Memory button

    MyButton_Memory.button_id = 2;

    // Relative values
    MyButton_Memory.left = memory_x;
    MyButton_Memory.top  = button_y;

    // Absolute coordinates (relative to screen)
    MyButton_Memory.absolute_left = wi.left + wi.cr_left + MyButton_Memory.left;
    MyButton_Memory.absolute_top  = wi.top + wi.cr_top + MyButton_Memory.top;
    MyButton_Memory.width         = button_w;
    MyButton_Memory.height        = button_h;

// Create a button
    uic_button_memory = libgui_create_ui_component (
        dc00, 
        1,   // type = button 
        MyButton_Memory.left, 
        MyButton_Memory.top, 
        MyButton_Memory.width, 
        MyButton_Memory.height,
        "Memory",
        (0x0001 | 0x0002)
    );

// ============================================================
// Create Sysinfo button

    MyButton_Sysinfo.button_id = 3;

    // Relative values
    MyButton_Sysinfo.left = sysinfo_x;
    MyButton_Sysinfo.top  = button_y;

    // Absolute coordinates (relative to screen)
    MyButton_Sysinfo.absolute_left = wi.left + wi.cr_left + MyButton_Sysinfo.left;
    MyButton_Sysinfo.absolute_top  = wi.top + wi.cr_top + MyButton_Sysinfo.top;
    MyButton_Sysinfo.width         = button_w;
    MyButton_Sysinfo.height        = button_h;

// Create a button
    uic_button_sysinfo = libgui_create_ui_component (
        dc00, 
        1,   // type = button 
        MyButton_Sysinfo.left, 
        MyButton_Sysinfo.top, 
        MyButton_Sysinfo.width, 
        MyButton_Sysinfo.height,
        "Sysinfo",
        (0x0001 | 0x0002)
    );

// -----------------------------------------------------
// #test: BMP image

    icon00_cache = (struct bmp_cache_d*) bmp_load_bmp_image("#folder.bmp");
    if (icon00_cache && icon00_cache->loaded) 
    {
        bmp_decode_bmp_image(icon00_cache, dc00, 50, 50, 4); // zoom=4
    }

// ================================================================================

// Create footer component
    uic_footer =
        libgui_create_ui_component(
            dc00,
            UI_COMPONENT_FOOTER,
            0, 
            wi.cr_height -24, 
            wi.cr_width, 
            24,
            "-- launch --", 
            (0x0001 | 0x0002)
        );

// ================================================================================

// Main window: Activate and show.
    gws_set_active( client_fd, main_window );


    int nSysMsg = 0;

//
// Event loop
//

    while (1){

        if (isTimeToQuit == TRUE)
            break;

        // Shared flags handling (same as Power app)
        if (__sh_flags != 0)
        {
            char *flags_ptr = (char *) __sh_flags;
            if (*flags_ptr & 0x0008)
            {
                // Clear BLIT bit
                *flags_ptr &= ~0x0008;
                // Redraw
                update_children(client_fd);
            }
        }

        // 1. Pump events from Display Server
        // #bugbug:
        // This pump is very slow, affecting the responsivity
        // for the other pump that gets events from the system.
        pump(client_fd);

        // 2. Pump events from Input Broker (system events)
        for (nSysMsg=0; nSysMsg<32; nSysMsg++){
        if (rtl_get_event() == TRUE)
        {
            // IN: wid, event type, VK, scancode.
            launchProcedure(
                client_fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3] );
            RTLEventBuffer[1] = 0;  // Clear after dispatch
        }
        };
    };

    if (isTimeToQuit == TRUE){
        printf("LaunchApp: Close window\n");
        gws_destroy_window(client_fd, main_window);
    }

    if (client_fd > 0)
        close(client_fd);

    return EXIT_SUCCESS;
}
