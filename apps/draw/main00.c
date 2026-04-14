// main.c - monkeygame.bin application
// Gramado OS client-side GUI app: "Monkey Adventure" - a fully playable 8-bit style game
// One controllable character (the monkey) with movement, jumping, gravity, collectibles and score.

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

// Cached frame/chrome area
static unsigned long frame_left   = 0;
static unsigned long frame_top    = 0;
static unsigned long frame_width  = 0;
static unsigned long frame_height = 0;

// Cached client area
static unsigned long cr_left   = 0;
static unsigned long cr_top    = 0;
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;

// Game state
static int current_mode = 0;           // 0 = playable game (monkey), future modes can be added
static int monkey_x = 0;               // client-area coordinates
static int monkey_y = 0;
static int monkey_vel_y = 0;
static int is_jumping = FALSE;
static int score = 0;

struct Banana {
    int x;
    int y;
    int active;
};

static struct Banana bananas[3];

// Prototypes
static void query_client_area(int fd);

static void draw_monkey_sprite(int fd, int base_x, int base_y);  // the character
static void draw_game_scene(int fd);
static void init_game(int fd);
static void update_physics(void);

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

    frame_left   = wi.left;
    frame_top    = wi.top;
    frame_width  = wi.width;
    frame_height = wi.height;

    cr_left   = wi.cr_left;
    cr_top    = wi.cr_top;
    cr_width  = wi.cr_width;
    cr_height = wi.cr_height;
}


static void init_game(int fd)
{
    query_client_area(fd);

// Changed: Monkey now starts in the upper area of the screen
    monkey_x = (cr_width / 2) - 80;           // keep centered horizontally
    monkey_y = (cr_height / 4) + 40;          // upper area (about 1/4 from the top)

    monkey_vel_y = 0;
    is_jumping = FALSE;
    score = 0;

    // Place 3 collectible bananas
    bananas[0].x = 120;  bananas[0].y = 180;  bananas[0].active = TRUE;
    bananas[1].x = cr_width - 220; bananas[1].y = 140; bananas[1].active = TRUE;
    bananas[2].x = cr_width / 2 + 80; bananas[2].y = 220; bananas[2].active = TRUE;
}

static void update_physics(void)
{
    const int GRAVITY = 4;  // stronger gravity for better feel
    const int GROUND_Y = (int)cr_height - 150;   // adjusted for upper start

    // Always apply gravity
    monkey_vel_y += GRAVITY;
    monkey_y += monkey_vel_y;

    // Ground collision
    if (monkey_y >= GROUND_Y)
    {
        monkey_y = GROUND_Y;
        monkey_vel_y = 0;
        is_jumping = FALSE;
    }

    // Keep monkey inside bounds
    if (monkey_x < 20) monkey_x = 20;
    if (monkey_x > (int)cr_width - 180) monkey_x = (int)cr_width - 180;
}


static void draw_monkey_sprite(int fd, int base_x, int base_y)
{
    const int PIXEL = 4; //5
    const int GAP   = 1;

    // 32×40 classic 8-bit monkey (head, ears, eyes, mouth, body, arms, legs)
    const char* sprite[40] = {
        "           #########          ", // 0
        "        ################      ",
        "      ####################    ",
        "     #######################  ",
        "    ######################### ",
        "   ###########################", // 5
        "  ############################",
        " #############################",
        "##############################",
        "##############################", // 9  head
        "####                    ####  ", // 10 ears
        "####                    ####  ",
        "####                    ####  ",
        " ############################ ",
        "  ##########################  ", // 14
        "   ########################   ",
        "    ######################    ",
        "     ####################     ",
        "       ################       ",
        "          ##########          ", // 19 neck
        "        ################      ", // 20 body
        "       ##################     ",
        "      ####################    ",
        "     ######################   ",
        "    ########################  ",
        "   ########################## ", // 25
        "  ############################",
        " #############################",
        "### ###################### ###",
        "    #####           #####     ", // 29 legs start
        "   ######            ######   ",
        "  ######              ######  ",
        " ######                ###### ", // 32
        "   #####              #####   ", // 33 knees
        "    #####            #####    ",
        "     #####          #####     ",
        "      #####        #####      ",
        "       #####      #####       ", // 37
        "        #####    #####        ",
        "         ##### ######         ",
        "          #### #####          "
    };

    int row = 0;
    int col = 0;
    for (row = 0; row < 40; row++)
    {
        for (col = 0; col < 30; col++)
        {
            char c = sprite[row][col];
            if (c == ' ') continue;

            int px = base_x + col * (PIXEL + GAP);
            int py = base_y + row * (PIXEL + GAP);

            unsigned long color = (c == '#') ? 
                ((row < 20) ? 0xFFBE7F4A : 0xFFAC6E3F) : 0xFF111111;

            // Clipping to client area (optional, but good practice)
            if (
                ((frame_top  + cr_top  + py) + PIXEL) < (frame_top + frame_height -8)
               )
            {
            libgui_backbuffer_draw_rectangle0(
                frame_left + cr_left + px, 
                frame_top  + cr_top  + py, 
                PIXEL, PIXEL,
                color, 1, 0, FALSE );

            }

        }
    }

    // Extra detail: eyes and mouth (over the sprite)
    // Left eye
    libgui_backbuffer_draw_rectangle0(frame_left + cr_left + base_x + 11*(PIXEL+GAP), frame_top + cr_top + base_y + 13*(PIXEL+GAP), PIXEL*2, PIXEL, 0xFF111111, 1, 0, FALSE);
    libgui_backbuffer_draw_rectangle0(frame_left + cr_left + base_x + 12*(PIXEL+GAP), frame_top + cr_top + base_y + 14*(PIXEL+GAP), PIXEL,   PIXEL, 0xFFFFFFFF, 1, 0, FALSE);
    // Right eye
    libgui_backbuffer_draw_rectangle0(frame_left + cr_left + base_x + 19*(PIXEL+GAP), frame_top + cr_top + base_y + 13*(PIXEL+GAP), PIXEL*2, PIXEL, 0xFF111111, 1, 0, FALSE);
    libgui_backbuffer_draw_rectangle0(frame_left + cr_left + base_x + 20*(PIXEL+GAP), frame_top + cr_top + base_y + 14*(PIXEL+GAP), PIXEL,   PIXEL, 0xFFFFFFFF, 1, 0, FALSE);
    // Mouth
    libgui_backbuffer_draw_rectangle0(frame_left + cr_left + base_x + 13*(PIXEL+GAP), frame_top + cr_top + base_y + 18*(PIXEL+GAP), PIXEL*6, PIXEL, 0xFF111111, 1, 0, FALSE);
}


static void draw_game_scene(int fd)
{
    // query_client_area(fd);

    // Background (jungle green)
    libgui_backbuffer_draw_rectangle0(
        frame_left + cr_left, 
        frame_top  + cr_top, 
        cr_width, 
        cr_height,
        0xFF1C2F1C, 1, 0, FALSE
    );

    // Simple ground
    libgui_backbuffer_draw_rectangle0(
        frame_left + cr_left, 
        frame_top  + cr_top + (int)cr_height - 80,
        cr_width, 80,
        0xFF3D5F2A, 1, 0, FALSE
    );

    // Some simple trees / decoration (optional visual polish)
    libgui_backbuffer_draw_rectangle0(frame_left + cr_left + 80,  frame_top + cr_top + (int)cr_height - 160, 40, 80, 0xFF2A3F1A, 1, 0, FALSE); // trunk
    libgui_backbuffer_draw_rectangle0(frame_left + cr_left + 60,  frame_top + cr_top + (int)cr_height - 200, 80, 60, 0xFF1F4F1F, 1, 0, FALSE); // leaves
    libgui_backbuffer_draw_rectangle0(frame_left + cr_left + (int)cr_width - 140, frame_top + cr_top + (int)cr_height - 160, 40, 80, 0xFF2A3F1A, 1, 0, FALSE);
    libgui_backbuffer_draw_rectangle0(frame_left + cr_left + (int)cr_width - 160, frame_top + cr_top + (int)cr_height - 200, 80, 60, 0xFF1F4F1F, 1, 0, FALSE);

    // Draw collectible bananas
    int i=0;
    for (i = 0; i < 3; i++)
    {
        if (!bananas[i].active) continue;

        // Simple banana (yellow rectangle with highlight)
        libgui_backbuffer_draw_rectangle0(
            frame_left + cr_left + bananas[i].x,
            frame_top  + cr_top  + bananas[i].y,
            28, 18,
            0xFFFFDD00, 1, 0, FALSE
        );
        libgui_backbuffer_draw_rectangle0(
            frame_left + cr_left + bananas[i].x + 4,
            frame_top  + cr_top  + bananas[i].y + 4,
            12, 10,
            0xFFFFEE66, 1, 0, FALSE
        );
    }

    // Draw the monkey character
    draw_monkey_sprite(fd, monkey_x, monkey_y);

    // Score and instructions
    char score_text[64];
    sprintf(score_text, "Score: %d / 3", score);
    //gws_draw_text(fd, main_window, 30, 30, 0xFFFFFF88, score_text);
    libgui_drawstring(
        frame_left + cr_left + 30, 
        frame_top  + cr_top  + 30, 
        score_text, 0xFFFFFF88, 0xFF1C2F1C, 0);

    //gws_draw_text(fd, main_window, 30, 60, 0xFF88FF99, "A/D = Move   SPACE = Jump   Q = Quit");
    libgui_drawstring(
        frame_left + cr_left + 30, 
        frame_top  + cr_top  + 60, 
        "A/D = Move   SPACE = Jump   Q = Quit", 0xFF88FF99, 0xFF1C2F1C, 0);

    // Check win condition
    if (score >= 3)
    {
        //gws_draw_text(fd, main_window, 
            //(cr_width/2)-120, (cr_height/2)-20, 0xFFFFFF00, 
            //"YOU WIN! All bananas collected!");
        libgui_drawstring(
            frame_left + ((cr_width/2)-120), 
            frame_top  + ((cr_height/2)-20), 
            "YOU WIN! All bananas collected!", 0xFFFFFF00, 0xFF1C2F1C, 0);
    }

    // Refresh everything in one kernel call (fast client-side path)
    libgui_refresh_rectangle_via_kernel(
        frame_left + cr_left, 
        frame_top  + cr_top, 
        cr_width, 
        cr_height
    );
}

static void draw_current_art(int fd)   // kept the original name for compatibility
{
    if (current_mode == 0)
        draw_game_scene(fd);
    else
        draw_game_scene(fd);   // future modes can be added here
}

// ----------------------------------------------------
// Procedure & pump (exactly as before)
// ----------------------------------------------------

static void exitProgram(int fd)
{
    if (fd < 0) 
        return;
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
        case 'A': case 'a': 
            monkey_x -= 18; 
            break;
        case 'D': case 'd': 
            monkey_x += 18; 
            break;
        case ' ': 
            if (!is_jumping)
            {
                monkey_vel_y = -28;  //-24;   // stronger jump (feels better)
                is_jumping = TRUE;
            }
            break;

        case 'Q': case 'q':
            exitProgram(fd);
            break;
        }
        
        // IMPORTANT: Only redraw after input, but gravity runs every frame
        draw_current_art(fd);
        break;

    case MSG_SYSKEYDOWN:
        switch (long1)
        {
        case VK_F5:
            draw_current_art(fd);
            break; 
        case VK_F12:  
            printf("monkeygame: Debug info - score=%d\n", score);  
            break;
        }
        break;

    case MSG_CLOSE:
        exitProgram(fd);
        break;

    case MSG_PAINT:
        query_client_area(fd);
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
// Main (exactly the same initialization you already use)
// ----------------------------------------------------

int main(int argc, char *argv[]) 
{
    Display = gws_open_display("display:name.0");
    if (!Display) return EXIT_FAILURE;
    int fd = Display->fd;

    int status = -1;
    status = (int) libgui_initialize();
    if (status < 0){
        printf("monkeygame: libgui_initialize fail\n");
        exit(1);
    }

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);
    unsigned long win_w = (screen_w * 7) / 10;
    unsigned long win_h = (screen_h * 7) / 10;
    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

    main_window = 
    (int) gws_create_window(
        fd, 
        WT_OVERLAPPED,
        WINDOW_STATUS_ACTIVE,
        WINDOW_STATE_NORMAL,
        "Draw App",                     // kept original title as requested
        win_x, win_y, win_w, win_h,
        0,
        WS_APP,
        COLOR_WINDOW,
        COLOR_WINDOW
    );

    if (main_window < 0){
        printf("on main_window\n");
        exit(0);
    }

    gws_set_active(fd, main_window);
    gws_refresh_window(fd, main_window);

    query_client_area(fd);

    // === Exactly the same wproxy update you already use ===
    unsigned long m[10];
    int mytid = gettid();
    m[0] = (unsigned long) (mytid & 0xFFFFFFFF);
    m[1] = frame_left;   m[2] = frame_top;   m[3] = frame_width;   m[4] = frame_height;
    m[5] = cr_left;      m[6] = cr_top;      m[7] = cr_width;      m[8] = cr_height;
    sc80(48, &m[0], &m[0], &m[0]);

    // Initialize the game (monkey position, bananas, etc.)
    init_game(fd);

    // First frame
    draw_current_art(fd);

    int nSysMsg = 0;

    // Event loop (exactly as before)
    while (1) {

        pump(fd);

        for (nSysMsg = 0; nSysMsg < 32; nSysMsg++){
            if (rtl_get_event() == TRUE) {
                systemProcedure(
                    fd,
                    (int) RTLEventBuffer[0],
                    (int) RTLEventBuffer[1],
                    (unsigned long) RTLEventBuffer[2],
                    (unsigned long) RTLEventBuffer[3]
                );
                RTLEventBuffer[1] = 0;
            }
        };

        // Very light continuous update (gravity & redraw) so the monkey falls naturally
        // This keeps the game "alive" even without key presses
        //update_physics();
        // Only redraw when needed to avoid unnecessary CPU usage
        // (you can remove the counter if you want smoother animation)

        // Continuous physics and redraw
        update_physics();
        draw_current_art(fd);
        //rtl_sleep(16);   // smooth falling and jumping
    };

    return EXIT_SUCCESS;
}
