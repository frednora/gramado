// main.c - drawapp.bin application
// Gramado OS client-side GUI app for drawing cool pixel art things.

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>
#include <gws.h>

// Globals
struct gws_display_d *Display;

static int main_window = -1;

// Cached client area
static unsigned long cr_left   = 0;
static unsigned long cr_top    = 0;
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;

// Current drawing mode (0: ghost, 1: big rocket, 2: detailed rocket)
static int current_mode = 0;

static void query_client_area(int fd);

static void draw_ghost(int fd);
static void draw_big_rocket(int fd);
static void draw_detailed_rocket(int fd);

static void draw_current_art(int fd);

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

    // #importante: 'Cause the values are relative
    cr_left   = 0;
    cr_top    = 0;

    cr_width  = wi.cr_width;
    cr_height = wi.cr_height;
}

static void draw_ghost(int fd)
{
    query_client_area(fd);

    // Small "pixel" size - feel free to change!
    const int PIXEL = 8; //18;
    const int GAP   = 2;   // small spacing between pixels (optional)

    // Center the artwork roughly
    int art_width  = 16 * (PIXEL + GAP);
    int art_height = 16 * (PIXEL + GAP);
    
    int start_x = (cr_width  - art_width)  / 2;
    int start_y = (cr_height - art_height) / 2;

    // Clear client area first (optional but recommended)
    gws_draw_rectangle(fd, main_window,
        0, 0, cr_width, cr_height,
        COLOR_WHITE, TRUE, 0);

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

    // ─── Draw the pixel art ───────────────────────────────────────
    int row=0;
    int col=0;

    for (row = 0; row < 16; row++)
    {
        for (col = 0; col < 16; col++)
        {
            if (ghost[row][col] == ' ') continue;

            int px = start_x + col * (PIXEL + GAP);
            int py = start_y + row * (PIXEL + GAP);

            unsigned long color;

            // You can make different characters = different colors
            switch(ghost[row][col])
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

static void draw_big_rocket(int fd)
{
    query_client_area(fd);

    // Pixel size - smaller than before, but still chunky/big-pixel style
    const int PIXEL = 8; //12;
    const int GAP   = 1;   // almost no gap - more solid look

    // 24×32 rocket art (taller to fit flame nicely)
    const int ART_W = 24;
    const int ART_H = 32;

    int art_width  = ART_W * (PIXEL + GAP);
    int art_height = ART_H * (PIXEL + GAP);

    int start_x = (cr_width  - art_width)  / 2;
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
        "To Mars & Beyond! ");

    gws_draw_text(fd, main_window,
        start_x + 60,
        start_y + art_height + 55,
        0xFFFFAA00,
        "Elon's rocket");

    // Final refresh
    gws_refresh_window(fd, main_window);
}

static void draw_detailed_rocket(int fd)
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

static void draw_current_art(int fd)
{
    switch (current_mode) {
        case 0:
            draw_ghost(fd);
            break;
        case 1:
            draw_big_rocket(fd);
            break;
        case 2:
            draw_detailed_rocket(fd);
            break;
        default:
            draw_ghost(fd);
            break;
    }
}

static void exitProgram(int fd)
{
    if (fd < 0)
        return;
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
        case '1': 
            current_mode = 0;
            draw_current_art(fd);
            break;
        case '2': 
            current_mode = 1;
            draw_current_art(fd);
            break;
        case '3': 
            current_mode = 2;
            draw_current_art(fd);
            break;
        case 'Q': 
        case 'q':
            exitProgram(fd);
            break;
        }
        break;

    case MSG_SYSKEYDOWN:
        switch (long1){
        case VK_F5:
            draw_current_art(fd);
            break; 
        case VK_F12:  
            printf("draw_app: Debug info\n");  
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
        "Draw App", 
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

// Main window active
    gws_set_active(fd, main_window);
    //gws_set_focus(fd, main_window);

    gws_refresh_window(fd, main_window);

//
// Draw art
//

    query_client_area(fd);

// Initial draw
    draw_current_art(fd);

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
