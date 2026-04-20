// main.c - menu.bin
// Gramado OS - Simple Big Letter Menu / Selector

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>

#include <gws.h>
#include <libgui.h>

// Globals
struct gws_display_d *Display;

static int main_window = -1;

static unsigned long frame_left   = 0;
static unsigned long frame_top    = 0;
static unsigned long frame_width  = 0;
static unsigned long frame_height = 0;

static unsigned long cr_left   = 0;
static unsigned long cr_top    = 0;
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;

// Menu state
#define MENU_ITEMS 5
static const char* menu_options[MENU_ITEMS] = {
    "Start Game",
    "Load File",
    "Settings",
    "About",
    "Exit"
};

static int selected = 0;   // currently highlighted item

// Prototypes
static void query_client_area(int fd);
static void draw_big_menu(int fd);
static void init_menu(int fd);

static void exitProgram(int fd);

static int systemProcedure(
    int fd, 
    int event_window, 
    int event_type,
    unsigned long long1, 
    unsigned long long2 );

static void pump(int fd);

// ----------------------------------------------------
// Helpers
// ----------------------------------------------------

static void query_client_area(int fd) 
{
    struct gws_window_info_d wi;
    gws_get_window_info(fd, main_window, &wi);

    frame_left   = wi.left;
    frame_top    = wi.top;
    frame_width  = wi.width;
    frame_height = wi.height;

    cr_left   = wi.cr_left;
    cr_top    = wi.cr_top;
    cr_width  = wi.cr_width;
    cr_height = wi.cr_height;
}

static void init_menu(int fd)
{
    query_client_area(fd);
    selected = 0;
}

static void draw_big_menu(int fd)
{
    query_client_area(fd);

    // Background
    libgui_backbuffer_draw_rectangle0(
        frame_left + cr_left, 
        frame_top  + cr_top, 
        cr_width, 
        cr_height,
        0xFF1C2F1C, 1, 0, FALSE);

    // Title
    libgui_drawstring(
        frame_left + 60, 
        frame_top  + 50,  //frame_top  + 40,
        "GRAMADO OS MENU",
        0xFF88FFAA, 0xFF1C2F1C, 0);

    // Draw menu items with big block letters
    int i;
    unsigned int color;
    for (i = 0; i < MENU_ITEMS; i++)
    {
        color = (i == selected) ? 0xFFFFFF00 : 0xFFCCCCCC;

        libgui_drawstringblock(
            frame_left + 100,
            frame_top  + 120 + (i * 50), //frame_top  + 120 + (i * 70),
            color,
            menu_options[i],
            3);                     // big chunky letters
    }

    // Instructions at bottom
    libgui_drawstring(
        frame_left + 60, 
        frame_top  + (cr_height - 10),  //frame_top  + (cr_height - 60),
        "UP/DOWN = Navigate    ENTER = Select    Q = Quit",
        0xFFAAAAAA, 0xFF1C2F1C, 0);

    // Refresh
    libgui_refresh_rectangle_via_kernel(
        frame_left + cr_left, 
        frame_top  + cr_top, 
        cr_width, 
        cr_height);
}

static void draw_game_scene(int fd)
{
    draw_big_menu(fd);
}

static void draw_current_art(int fd)
{
    draw_game_scene(fd);
}

// ----------------------------------------------------
// Event handling
// ----------------------------------------------------

static void exitProgram(int fd)
{
    if (fd < 0) return;
    gws_destroy_window(fd, main_window);
    exit(0);
}

static int 
systemProcedure(
    int fd, 
    int event_window, 
    int event_type,
    unsigned long long1, 
    unsigned long long2 ) 
{
    if (fd < 0 || event_window < 0 || event_type < 0)
        return -1;

    switch (event_type) 
    {
    case MSG_KEYDOWN:
        switch (long1)
        {
        case VK_RETURN:
        case '\n':     // Enter key
        case '\r':
            // For now, just show which option was selected
            printf("Selected: %s\n", menu_options[selected]);
            if (selected == 4) {   // Exit option
                exitProgram(fd);
            }
            break;
        case 'Q': case 'q':
            exitProgram(fd);
            break;
        }
        draw_current_art(fd);
        break;

    case MSG_SYSKEYDOWN:
        if (long1 == VK_F5){
            draw_current_art(fd);
            break;
        }

        switch (long1) {
        case VK_ARROW_UP:
            if (selected > 0) selected--;
            break;
        case VK_ARROW_DOWN:
            if (selected < MENU_ITEMS - 1) selected++;
            break;
        };

        break;

    case MSG_CLOSE:
        exitProgram(fd);
        break;

    case MSG_PAINT:
        draw_current_art(fd);
        break;
    }
    return 0;
}

static void pump(int fd) 
{
    struct gws_event_d event;
    struct gws_event_d *e = (struct gws_event_d *) gws_get_next_event(fd, main_window, &event);

    if (!e || e->magic != 1234 || e->used != TRUE) return;
    if (e->type <= 0) return;

    systemProcedure(fd, e->window, e->type, e->long1, e->long2);
}

// ----------------------------------------------------
// Main
// ----------------------------------------------------

int main(int argc, char *argv[]) 
{
    Display = gws_open_display("display:name.0");
    if (!Display) return EXIT_FAILURE;
    int fd = Display->fd;

    int status = (int) libgui_initialize();
    if (status < 0){
        printf("menu: libgui_initialize fail\n");
        exit(1);
    }

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);
    unsigned long win_w = (screen_w * 7) / 10;
    unsigned long win_h = (screen_h * 7) / 10;
    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

    main_window = (int) gws_create_window(
        fd, WT_OVERLAPPED, WINDOW_STATUS_ACTIVE, WINDOW_STATE_NORMAL,
        "Menu", win_x, win_y, win_w, win_h, 0, WS_APP, COLOR_WINDOW, COLOR_WINDOW);

    if (main_window < 0){
        printf("on main_window\n");
        exit(0);
    }

    gws_set_active(fd, main_window);
    gws_refresh_window(fd, main_window);

    query_client_area(fd);

    unsigned long m[10];
    int mytid = gettid();
    m[0] = (unsigned long) (mytid & 0xFFFFFFFF);
    m[1] = frame_left; m[2] = frame_top; m[3] = frame_width; m[4] = frame_height;
    m[5] = cr_left;    m[6] = cr_top;    m[7] = cr_width;    m[8] = cr_height;
    sc80(48, &m[0], &m[0], &m[0]);

    init_menu(fd);
    draw_current_art(fd);

    int nSysMsg = 0;

    while (1) {
        pump(fd);

        for (nSysMsg = 0; nSysMsg < 32; nSysMsg++){
            if (rtl_get_event() == TRUE) {
                systemProcedure(fd,
                    (int) RTLEventBuffer[0],
                    (int) RTLEventBuffer[1],
                    (unsigned long) RTLEventBuffer[2],
                    (unsigned long) RTLEventBuffer[3]);
                RTLEventBuffer[1] = 0;
            }
        }

        draw_current_art(fd);
        //rtl_sleep(16);
    }

    return EXIT_SUCCESS;
}
