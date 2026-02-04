// main.c - sysinfo.bin application
// Gramado OS client-side GUI app showing system information.
// Similar architecture to memory_app.c

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <rtl/gramado.h>
#include <gws.h>

// Globals
struct gws_display_d *Display;

static int main_window    = -1;
static int refresh_button = -1;
static int close_button   = -1;
static int default_responder = -1;


// Cached client area
static unsigned long cr_left   = 0;
static unsigned long cr_top    = 0;
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;


static void query_client_area(int fd);

static void test_draw_rects_in_client_area(int fd);
static void test_draw_rects_in_client_area2(int fd);
static void test_draw_rects_in_client_area3(int fd);

static void draw_label(int fd, int x, int y, const char *label, const char *value);
static void draw_system_info(int fd, int base_x, int base_y, int line_h);
static void update_children(int fd);
static void set_default_responder(int wid);
static void trigger_default_responder(int fd);
static void switch_responder(int fd);

static void exitProgram(int fd);

static int 
systemProcedure(
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

    // These values are relative,
    // #todo: Use 0,0 for left/top
    //cr_left   = wi.cr_left;
    //cr_top    = wi.cr_top;
    cr_left   = 0;
    cr_top    = 0;

    cr_width  = wi.cr_width;
    cr_height = wi.cr_height;
}

static void test_draw_rects_in_client_area(int fd)
{
    query_client_area(fd);

    // Small "pixel" size - feel free to change!
    const int PIXEL = 18;
    const int GAP   = 2;   // small spacing between pixels (optional)

    // Center the artwork roughly
    int art_width  = 16 * (PIXEL + GAP);
    int art_height = 16 * (PIXEL + GAP);
    
    int start_x = (cr_width  - art_width)  / 2;
    //int start_y = (cr_height - art_height) / 2 - 40;  // a bit higher
    int start_y = (cr_height - art_height) / 2;

    // Clear client area first (optional but recommended)
    gws_draw_rectangle(fd, main_window,
        0, 0, cr_width, cr_height,
        COLOR_WHITE, TRUE, 0);

    // ─────────────────────────────────────────────────────────────
    // Very simple 16×16 pixel art examples - pick one or mix them!
    // ─────────────────────────────────────────────────────────────

    // Option 1: Classic smiley face
    /*
    const char* smiley[16] = {
        "                ",
        "     ..    ..   ",
        "    .##.  .##.  ",
        "    .########.  ",
        "     .######.   ",
        "      .####.    ",
        "                ",
        "   .          . ",
        "  .##.      .##.",
        "  .############.",
        "   .##########. ",
        "    .########.  ",
        "     ........   ",
        "                ",
        "                ",
        "                "
    };
    */

    // Option 2: Heart (uncomment to use instead)
    /*
    const char* heart[16] = {
        "      ..  ..      ",
        "    .####.####.   ",
        "   .###########.  ",
        "   .###########.  ",
        "    .#########.   ",
        "     .#######.    ",
        "      .#####.     ",
        "       .###.      ",
        "        .#.       ",
        "         .        ",
        "                  ",
        "                  ",
        "                  ",
        "                  ",
        "                  ",
        "                  "
    };
    */

    // Option 3: Tiny ghost (cute one)
    
    const char* ghost[16] = {
        "      ......      ",
        "    .########.    ",
        "   .##########.   ",
        "  .####.##.####.  ",
        "  .############.  ",
        "  .############.  ",
        "  .###.####.###.  ",
        "   .##########.   ",
        "    .########.    ",
        "     .######.     ",
        "      ......      ",
        "     . .  . .     ",
        "    .   ..   .    ",
        "                  ",
        "                  ",
        "                  "
    };
    

    // Pick which art you want to draw (change the name here)
   // const char** art = smiley;   // ← change to heart or ghost to try others
   // const char** art = heart;   // ← change to heart or ghost to try others
    const char** art = ghost;   // ← change to heart or ghost to try others

    // ─── Draw the pixel art ───────────────────────────────────────
    int row=0;
    int col=0;

    for (row = 0; row < 16; row++)
    {
        for (col = 0; col < 16; col++)
        {
            if (art[row][col] == ' ') continue;

            int px = start_x + col * (PIXEL + GAP);
            int py = start_y + row * (PIXEL + GAP);

            unsigned long color;

            // You can make different characters = different colors
            switch(art[row][col])
            {
                case '#':  color = 0xFF3366CC; break;  // nice blue
                case '.':  color = 0xFFFFD700; break;  // gold/yellow
                default:   color = COLOR_BLACK; break;
            }

            gws_draw_rectangle(fd, main_window,
                px, py, PIXEL, PIXEL,
                color, TRUE, 0);
        }
    }

    // Optional: little message under the art
    gws_draw_text(fd, main_window,
        start_x + 20,
        start_y + art_height + 30,
        COLOR_BLACK,
        "Have a nice day!  ^_^");

    // Final refresh
    gws_refresh_window(fd, main_window);
}


static void test_draw_rects_in_client_area2(int fd)
{
    query_client_area(fd);

    // Pixel size - smaller than before, but still chunky/big-pixel style
    const int PIXEL = 12;
    const int GAP   = 1;   // almost no gap - more solid look

    // 24×32 rocket art (taller to fit flame nicely)
    const int ART_W = 24;
    const int ART_H = 32;

    int art_width  = ART_W * (PIXEL + GAP);
    int art_height = ART_H * (PIXEL + GAP);

    int start_x = (cr_width  - art_width)  / 2;
    //int start_y = (cr_height - art_height) / 2 - 30;  // little bit higher
    int start_y = (cr_height - art_height) / 2;  // little bit higher

    // Background - clean dark space feel
    gws_draw_rectangle(fd, main_window,
        0, 0, cr_width, cr_height,
        0x00112233, TRUE, 0);   // very dark blue-black

    // ─────────────────────────────────────────────────────────────
    //     Falcon 9 / Starship inspired rocket pixel art (24×32)
    // ─────────────────────────────────────────────────────────────
    const char* rocket[32] = {
        "                        ", // 0
        "           ##           ",
        "          ####          ",
        "         ######         ",
        "         ######         ",
        "        ########        ", // 5
        "       ##########       ",
        "       ##########       ",
        "      ############      ",
        "      ############      ",
        "     ##############     ", // 10
        "     ##############     ",
        "    ################    ",
        "    ################    ",
        "   ##################   ",
        "   ##################   ", // 15
        "  ####################  ",
        "  ####################  ",
        " ###################### ",
        " ###################### ",
        "########################", // 20
        "########################",
        "########################",
        "########################",
        "########################",
        "########################", // 25
        "           ##           ",
        "          ####          ",
        "         ######         ",
        "        ########        ",
        "       ##########       ", // 30
        "      ############      ",
        "     ##############     "
    };

    // ─── Draw the rocket ──────────────────────────────────────────
    int row=0;
    int col=0;

    for (row = 0; row < ART_H; row++)
    {
        for (col = 0; col < ART_W; col++)
        {
            char c = rocket[row][col];
            if (c == ' ') continue;

            int px = start_x + col * (PIXEL + GAP);
            int py = start_y + row * (PIXEL + GAP);

            unsigned long color;

            // Color choices - Starship/Falcon vibe
            if (row < 8) {
                color = 0xFFBBBBBB;           // silver/steel top
            }
            else if (row < 22) {
                color = 0xFF444488;           // dark stainless steel body
            }
            else if (row < 26) {
                color = 0xFF222244;           // darker lower section
            }
            else {
                // Flames!
                if ((row + col) % 3 == 0)
                    color = 0xFFFF6600;       // bright orange
                else if ((row + col) % 3 == 1)
                    color = 0xFFFFAA00;       // yellow-orange
                else
                    color = 0xFFFF4400;       // deep orange-red
            }

            gws_draw_rectangle(fd, main_window,
                px, py, PIXEL, PIXEL,
                color, TRUE, 0);
        }
    }

    // Little text under the rocket
    gws_draw_text(fd, main_window,
        start_x + 30,
        start_y + art_height + 30,
        0xFFDDDDDD,
        "To Mars & Beyond!  🚀");

    gws_draw_text(fd, main_window,
        start_x + 60,
        start_y + art_height + 55,
        0xFFFFAA00,
        "Elon's rocket");

    // Final refresh
    gws_refresh_window(fd, main_window);
}

static void test_draw_rects_in_client_area3(int fd)
{
    query_client_area(fd);

    // Smaller but still chunky "big pixels"
    const int PIXEL = 3;//4;  //9;
    const int GAP   = 1;

    // Rocket size: 21 wide × 38 tall (good balance)
    const int ART_W = 21;
    const int ART_H = 38;

    int art_width  = ART_W * (PIXEL + GAP);
    int art_height = ART_H * (PIXEL + GAP);

    int start_x = (cr_width  - art_width)  / 2;
    int start_y = (cr_height - art_height) / 2;
    //int start_y = (cr_height - art_height) / 2 - 50;  // shifted up a bit

    // Dark space background
    gws_draw_rectangle(fd, main_window,
        0, 0, cr_width, cr_height,
        0x00081420, TRUE, 0);   // very dark navy-black

    // ─────────────────────────────────────────────────────────────
    //     Compact cute rocket / Starship style (21×38)
    // ─────────────────────────────────────────────────────────────
    const char* rocket[38] = {
        "         ###         ",  // 0 - nose cone tip
        "        #####        ",
        "       #######       ",
        "      #########      ",
        "     ###########     ",
        "    #############    ",  // 5
        "   ###############   ",
        "  #################  ",
        " ################### ",
        "#####################",
        "#####################",  // 10 - top of payload/crew section
        "#####   #####   #####",
        "#####   #####   #####",
        "#####################",
        "#####################",
        "#####################",  // 15 - main body start
        "#####################",
        "#####################",
        "#####################",
        "#####################",
        "#####################",  // 20
        "#####################",
        "#####################",
        "#####################",
        "#####################",
        "#####################",  // 25 - lower body
        "#####################",
        "        #####        ",  // engine section transition
        "       #######       ",
        "      #########      ",
        "     ###########     ",  // 30 - engines + flame base
        "    #############    ",
        "   ###############   ",
        "  ####  ###  ####    ",  // flame variation
        " ###   #####   ###   ",
        " ##   #######   ##   ",  // 35
        "     #########       ",
        "      #######        ",
        "       #####         "
    };

    // ─── Draw the rocket ──────────────────────────────────────────
    int row=0;
    int col=0;
    for (row = 0; row < ART_H; row++)
    {
        for (col = 0; col < ART_W; col++)
        {
            if (rocket[row][col] == ' ') continue;

            int px = start_x + col * (PIXEL + GAP);
            int py = start_y + row * (PIXEL + GAP);

            unsigned long color;

            // Coloring logic
            if (row <= 9) {
                // Nose / fairing - bright steel
                color = 0xFFCCDDEE;
            }
            else if (row <= 26) {
                // Main body - stainless steel look
                if ((row + col) % 12 < 3)
                    color = 0xFFAAAAAA;     // slight shine bands
                else
                    color = 0xFF888899;
            }
            else if (row <= 29) {
                // Engine mount area - darker
                color = 0xFF444466;
            }
            else {
                // Flames - animated feel with variation
                int phase = (row + col * 2) % 6;
                if (phase == 0 || phase == 5) color = 0xFFFF5500;   // deep orange
                else if (phase == 1 || phase == 4) color = 0xFFFF8800;
                else if (phase == 2 || phase == 3) color = 0xFFFFDD44; // yellow core
            }

            gws_draw_rectangle(fd, main_window,
                px, py, PIXEL, PIXEL,
                color, TRUE, 0);
        }
    }

    // Small Gramado "G" logo at the bottom of the rocket
    gws_draw_rectangle(fd, main_window,
        start_x + 7*(PIXEL+GAP), start_y + 24*(PIXEL+GAP),
        7*(PIXEL+GAP), 9*(PIXEL+GAP), 0xFF222244, TRUE, 0);

    gws_draw_rectangle(fd, main_window,
        start_x + 8*(PIXEL+GAP), start_y + 25*(PIXEL+GAP),
        5*(PIXEL+GAP), 7*(PIXEL+GAP), 0xFF88AAFF, TRUE, 0);

    // Texts
    gws_draw_text(fd, main_window,
        start_x - 10,
        start_y + art_height + 30,
        0xFFCCDDFF,
        "Gramado OS Rocket v0.1");

    gws_draw_text(fd, main_window,
        start_x + 30,
        start_y + art_height + 55,
        0xFFFFFF88,
        "To the stars!");

    // Final refresh
    gws_refresh_window(fd, main_window);
}

static void draw_label(int fd, int x, int y, const char *label, const char *value) 
{
    char line[256];
    sprintf(line, "%s: %s", label, value);
    gws_draw_text(fd, main_window, x, y, COLOR_BLACK, line);
}

static void draw_system_info(int fd, int base_x, int base_y, int line_h) 
{
    struct utsname un;
    uname(&un);

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);
    unsigned long total_mem = gws_get_system_metrics(33);
    unsigned long used_mem  = gws_get_system_metrics(34);
    unsigned long free_mem  = gws_get_system_metrics(35);

    int y = base_y;

    draw_label(fd, base_x, y, "System",   un.sysname);   y += line_h;
    draw_label(fd, base_x, y, "Release",  un.release);   y += line_h;
    draw_label(fd, base_x, y, "Version",  un.version);   y += line_h;
    draw_label(fd, base_x, y, "Machine",  un.machine);   y += line_h;
    draw_label(fd, base_x, y, "Node",     un.nodename);  y += line_h;
    draw_label(fd, base_x, y, "Domain",   un.domainname);y += line_h;

    char buf[64];
    sprintf(buf, "%lux%lu", screen_w, screen_h);
    draw_label(fd, base_x, y, "Screen", buf); y += line_h;

    sprintf(buf, "%lu KB", total_mem);
    draw_label(fd, base_x, y, "Total Memory", buf); y += line_h;
    sprintf(buf, "%lu KB", used_mem);
    draw_label(fd, base_x, y, "Used Memory", buf); y += line_h;
    sprintf(buf, "%lu KB", free_mem);
    draw_label(fd, base_x, y, "Free Memory", buf); y += line_h;
}

// ----------------------------------------------------
// Worker: update children on paint
// ----------------------------------------------------

static void update_children(int fd) 
{
    query_client_area(fd);

    unsigned long button_w = cr_width / 5;
    unsigned long button_h = cr_height / 10;
    if (button_h < 32)
        button_h = 32;

    unsigned long label_y   = 20;
    unsigned long metrics_x = 20;
    unsigned long metrics_y = label_y + 30;
    unsigned long line_h    = 20;

    unsigned long refresh_x     = (cr_width / 4) - (button_w / 2);
    unsigned long close_x       = (3 * cr_width / 4) - (button_w / 2);

    unsigned long bottom_band_y = cr_height - (button_h*2);
    unsigned long buttons_y     = bottom_band_y;


// Redraw raw main window (first time)
    gws_redraw_window(fd, main_window, TRUE);

// Redraw main label
    gws_draw_text(
        fd, 
        main_window, 
        20, label_y, 
        COLOR_BLACK, "System Information:");

// ?
    draw_system_info(fd, metrics_x, metrics_y, line_h);

// Change refresh button
    gws_change_window_position(fd, refresh_button, refresh_x, buttons_y);
    gws_resize_window(fd, refresh_button, button_w, button_h);
    gws_redraw_window(fd, refresh_button, FALSE);
    //gws_redraw_window(fd, refresh_button, TRUE);

//  Change close button
    gws_change_window_position(fd, close_button, close_x, buttons_y);
    gws_resize_window(fd, close_button, button_w, button_h);
    gws_redraw_window(fd, close_button, FALSE);
    //gws_redraw_window(fd, close_button, TRUE);

// Refresh main window (again)
    gws_refresh_window(fd, main_window);
}

static void set_default_responder(int wid)
{
    if (wid < 0)
        return;
    default_responder = wid;
}

static void trigger_default_responder(int fd) 
{
    if (default_responder == refresh_button) {
        update_children(fd);
    } else if (default_responder == close_button) {

        exitProgram(fd);
        //gws_destroy_window(fd, refresh_button);
        //gws_destroy_window(fd, close_button);
        //gws_destroy_window(fd, main_window);
        //exit(0);
    }
}

// Toggle between refresh_button and close_button
static void switch_responder(int fd)
{
    if (default_responder == refresh_button) {
        set_default_responder(close_button);
        gws_set_focus(fd, close_button);
    } else {
        set_default_responder(refresh_button);
        gws_set_focus(fd, refresh_button);
    }
}

static void exitProgram(int fd)
{
    if (fd<0)
        return;
    gws_destroy_window(fd, refresh_button);
    gws_destroy_window(fd, close_button);
    gws_destroy_window(fd, main_window);
    exit(0);
}

// ----------------------------------------------------
// Procedure: handles events
// ----------------------------------------------------

static int 
systemProcedure(
    int fd, 
    int event_window, 
    int event_type,
    unsigned long long1, 
    unsigned long long2 ) 
{
    if (fd < 0 || event_window < 0 || event_type < 0)
    {
        return -1;
    }

    switch (event_type) 
    {

    case MSG_KEYDOWN:
        switch (long1) {

        // First responder
        case VK_RETURN: 
            trigger_default_responder(fd); 
            break;

        // Refresh
        case 'R': 
        case 'r': 
            update_children(fd); 
            break;

        // Cancel
        case 'C': 
        case 'c':
            exitProgram(fd);
            break;

        // #test
        case VK_TAB: 
            printf("app: TAB received\n");
            //switch_responder(fd);
            break;
        }
        break;

    case MSG_SYSKEYDOWN:
        switch (long1){
        case VK_F2:
            //test_draw_rects_in_client_area(fd);
            //test_draw_rects_in_client_area2(fd);
            test_draw_rects_in_client_area3(fd);
            break;
        case VK_F5:
            update_children(fd);
            break; 
        case VK_F12:  printf("system_info_app: Debug info\n");  break;
        //
        case VK_ARROW_RIGHT:  printf("sysinfo: VK_ARROW_RIGHT \n"); break;
        case VK_ARROW_UP:     printf("sysinfo: VK_ARROW_UP \n");    break;
        case VK_ARROW_DOWN:   printf("sysinfo: VK_ARROW_DOWN \n");  break;
        case VK_ARROW_LEFT:
            printf("sysinfo: VK_ARROW_LEFT \n"); 
            switch_responder(fd);
            break;
        };
        break;

    case GWS_MouseClicked:
        if ((int)long1 == refresh_button)
            update_children(fd);
        if ((int)long1 == close_button){
            exitProgram(fd);
        }
        break;

    case MSG_CLOSE:
        exitProgram(fd);
        break;

    case MSG_PAINT:
        update_children(fd);
        break;
    }
    return 0;
}

// ----------------------------------------------------
// Pump
// ----------------------------------------------------

static void pump(int fd) 
{
    struct gws_event_d event;
    struct gws_event_d *e =
        (struct gws_event_d *) gws_get_next_event(fd, main_window, &event);

    if (!e || e->magic != 1234 || e->used != TRUE) return;
    if (e->type <= 0) return;

    systemProcedure(fd, e->window, e->type, e->long1, e->long2);
}

// ----------------------------------------------------
// Main
// ----------------------------------------------------

int main(int argc, char *argv[]) 
{

    // Display
    Display = gws_open_display("display:name.0");
    if (!Display) return EXIT_FAILURE;
    int fd = Display->fd;

    // Screen
    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);
    //unsigned long win_w = screen_w / 2;
    //unsigned long win_h = screen_h / 2;
    unsigned long win_w = (screen_w * 7) / 10;
    unsigned long win_h = (screen_h * 7) / 10;
    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

// Main window

    main_window = 
    (int) gws_create_window(
        fd, 
        WT_OVERLAPPED,         // Window type
        WINDOW_STATUS_ACTIVE,  // Window status (active,inactive) / button state
        WINDOW_STATE_NORMAL,   // Window state (min,max, normal)
        "System Information", 
        win_x, win_y, win_w, win_h,
        0,        // Parent window (wid)
        WS_APP,   // Window style
        COLOR_WINDOW,  // Client area color (unused for overlapped) 
        COLOR_WINDOW   // bg color (unused for overlapped)
    );

    if (main_window < 0){
        printf("on main_window\n");
        exit(0);
    }
    gws_refresh_window(fd, main_window);

//-----------------------------

    query_client_area(fd);


//
// Buttons
//

    // Buttons
    unsigned long button_w = cr_width / 5;
    unsigned long button_h = cr_height / 10;
    if (button_h < 32) 
        button_h = 32;

    unsigned long refresh_x = (cr_width / 4) - (button_w / 2);
    unsigned long close_x   = (3 * cr_width / 4) - (button_w / 2);

    unsigned long buttons_y = cr_height - (button_h *2);


// Create Refresh button
    refresh_button = 
        gws_create_window(
            fd, 
            WT_BUTTON,    // Window type
            BS_DEFAULT,   // Window status / Button state
            1,            // Window state
            "Refresh", 
            refresh_x, buttons_y, button_w, button_h,
            main_window, 
            0,  // No style 
            COLOR_GRAY, COLOR_GRAY);
    //gws_refresh_window(fd, refresh_button);

// Create Close button
    close_button = gws_create_window(
        fd,
        WT_BUTTON,
        BS_DEFAULT,
        1,
        "Close",
        close_x, buttons_y, button_w, button_h,
        main_window, 0, COLOR_GRAY, COLOR_GRAY );
    //gws_refresh_window(fd, close_button);


/*
// =========================================
// #test: Testing new API
// Draw a rectangle inside a window, into the backbuffer.
    gws_draw_rectangle (
        fd, 
        main_window,
        20, 20, 20, 20,  // l, t, w, h 
        0x00FF0000,      // color
        TRUE,            // Fill or not
         0 );            // rop
// =========================================
*/

// Default responder
    set_default_responder(refresh_button);
    //set_default_responder(close_button);

// Main window active
    gws_set_active(fd, main_window);
   gws_set_focus(fd, refresh_button);

// Refresh. (again)
    gws_refresh_window(fd, main_window);

/*
// ================================
// Lets say to the kernel that we want to receive the TAB event.
    sc80(
        912,    // syscall number
        2000,   // option
        2000,   // extra values
        0  );   // not used
*/
// Lets say to the kernel that we want to receive the TAB event.
    // rtl_msgctl(2000,2000);

// ================================
// Event loop

    while (1) {
        // 1. Pump events from Display Server
        pump(fd);

        // 2. Pump events from Input Broker (system events)
        if (rtl_get_event() == TRUE) {
            systemProcedure(
                fd,
                (int) RTLEventBuffer[0],   // window id
                (int) RTLEventBuffer[1],   // event type
                (unsigned long) RTLEventBuffer[2], // VK code
                (unsigned long) RTLEventBuffer[3]  // scancode
            );
            RTLEventBuffer[1] = 0; // clear after dispatch
        }
    }

    return EXIT_SUCCESS;
}
