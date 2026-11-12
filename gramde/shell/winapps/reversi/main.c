// main.c
// Gramado OS client-side GUI app: Reversi
//
// In the spirit of the simple games of the Windows 2.x era.
// You play Black against the computer (White).
//
//   [ header : scoreboard  "Black 12 - White 9"        ]  UI_COMPONENT_HEADER
//   [ board  : 8x8, discs, hint markers, hover frame   ]  drawn on the dc
//   [ buttons: New   Undo   Hints                      ]  UI_COMPONENT_BUTTON
//   [ footer : whose turn / game result                ]  UI_COMPONENT_FOOTER
//
// Ideas taken from the other apps:
//  - Memory:  header + body + footer layout.
//  - Paint:   direct-write fills on the dc, hover/selection frames,
//             press on one thing and release on the same thing = click.
//  - Power / Launch / Calculator: on_button_pressed / released / clicked,
//             hover tracking, key down = press effect, key up = action.
//
// The input loop is the same as in the other apps:
//   shared-flags blit check; if (rtl_get_event()) reversiProcedure(...)
// The only addition: when the computer has a move to make, the loop calls
// rtl_jiffies() and, when the delay is over, hands a private message
// (RV_MSG_AI_TURN) to the procedure. When there is no pending computer
// move the loop does not touch the timer at all.
//
// Mouse:
//   hover        blue frame on a legal cell, gray on an illegal one
//   click        play on that cell (press and release on the same cell)
// Keyboard:
//   arrows       move the cursor        Enter / Space   play on the cursor
//   N new game   U undo (your last move and the reply)
//   H hints on/off                      Q quit
//
// #todo: A libgui_set_ui_component_label() would replace ui_set_label() here.

// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>

// The client-side library
#include <gws.h>

// #test
// The client-side library
#include <libgui.h>

// ----------------------------------------------------
// Config
// ----------------------------------------------------

// Set to 1 to get messages on the console.
#define RV_DEBUG  0
#if RV_DEBUG
#define RV_LOG(...)  printf(__VA_ARGS__)
#else
#define RV_LOG(...)  ((void)0)
#endif

// Private message: the computer's turn (sent by the input loop).
#define RV_MSG_AI_TURN   99700

// The computer waits this long before it plays.
// #bugbug: valid only if the timer fires 1000 times a second.
#define RV_AI_DELAY_MS   500

// Layout
#define MARGIN       8
#define GAP          8
#define HEADER_H     32      // text is scale 2 (16px), top + 8
#define FOOTER_H     24      // text is scale 1 (8px),  top + 8
#define BUTTON_H     28

// Look
#define PANEL_COLOR        LIBGUI_COLOR_PANEL_FACE
#define BOARD_COLOR        0x00208040
#define GRID_COLOR         0x00104820
#define BOARD_EDGE_COLOR   0x00000000
#define HINT_COLOR         0x0070C080
#define LASTMOVE_COLOR     0x00E04040
#define CURSOR_LEGAL_COLOR 0x000060C0
#define CURSOR_NONE_COLOR  0x00808080
#define FRAME_THICKNESS    2
#define FRAME_HOVER_COLOR  0x000060C0
#define FRAME_NONE_COLOR   PANEL_COLOR

// The game
#define N          8
#define EMPTY      0
#define BLACK      1       // the human
#define WHITE      2       // the computer
#define OPP(p)     ((p) == BLACK ? WHITE : BLACK)

static int isTimeToQuit = FALSE;

// Global display pointer
struct gws_display_d *Display;

struct dccanvas_d *dc00;  // shared dc

static int client_fd   = -1;
static int main_window = -1;

static unsigned long __sh_flags = 0;

// ----------------------------------------------------
// Components
// ----------------------------------------------------
struct ui_component_d *uic_header;
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

// button_id = index + 1
#define BTN_NEW    1
#define BTN_UNDO   2
#define BTN_HINTS  3

struct rv_item_d
{
    struct button_info_d info;
    struct ui_component_d *uic;
    const char *label;
    char key;              // shortcut (lowercase)
};

static struct rv_item_d  ItemList[] = {
    { {0}, NULL, "New",   'n' },
    { {0}, NULL, "Undo",  'u' },
    { {0}, NULL, "Hints On", 'h' }
};
#define ITEM_COUNT  (int)(sizeof(ItemList)/sizeof(ItemList[0]))

// ----------------------------------------------------
// Interface state
// ----------------------------------------------------
static int __hover_button_id  = -1;
static int __mouse_pressed_id = -1;
static int __key_pressed_id   = -1;

// The cell under the mouse / the keyboard cursor (-1 = none)
static int cursor_r = -1;
static int cursor_c = -1;
// The cell where the mouse button went down (-1 = none)
static int pressed_r = -1;
static int pressed_c = -1;

// Position of the pointer at the time of the event.
// Filled by the input loop from the event buffer.
static unsigned long last_x = 0;
static unsigned long last_y = 0;

// Geometry of the board (relative to the client area)
static long cell_sz  = 40;
static long board_x  = 0;
static long board_y  = 0;
static long board_px = 320;

// ----------------------------------------------------
// Game state
// ----------------------------------------------------
static unsigned char board[N][N];
static unsigned char undo_board[N][N];
static int undo_valid = FALSE;

static int turn      = BLACK;
static int game_over = FALSE;
static int hints_on  = TRUE;
static int last_r    = -1;     // the last disc placed
static int last_c    = -1;

static int ai_pending = FALSE;
static unsigned long ai_due = 0;

static char status_text[48];

// Positional weights for the computer.
// Corners are great, the cells next to them are bad.
static const int Weight[N][N] = {
    { 100, -20,  10,   5,   5,  10, -20, 100 },
    { -20, -50,  -2,  -2,  -2,  -2, -50, -20 },
    {  10,  -2,  -1,  -1,  -1,  -1,  -2,  10 },
    {   5,  -2,  -1,  -1,  -1,  -1,  -2,   5 },
    {   5,  -2,  -1,  -1,  -1,  -1,  -2,   5 },
    {  10,  -2,  -1,  -1,  -1,  -1,  -2,  10 },
    { -20, -50,  -2,  -2,  -2,  -2, -50, -20 },
    { 100, -20,  10,   5,   5,  10, -20, 100 }
};

// -----------------
static void on_button_pressed(int button_id);
static void on_button_released(int button_id);
static void on_button_clicked(int button_id);

static int  __hit_test_button(unsigned long rel_mx, unsigned long rel_my);
static void layout(struct gws_window_info_d *wi);
static void draw_board(void);
static void draw_cell(int r, int c);
static void refresh_status(void);
static void update_children(int fd);

// =====================================================
// Drawing helpers
// =====================================================

// Fill a rectangle, clipped against the dc. ROP_COPY only.
static void 
rv_fill_rect(
    long x, long y, long w, long h,
    unsigned int color )
{
    long x2, y2, yy, xx;
    long dw, dh;
    unsigned char *row;
    unsigned int c = (color & 0x00FFFFFF);
    unsigned char b = (unsigned char)( c        & 0xFF);
    unsigned char g = (unsigned char)((c >> 8)  & 0xFF);
    unsigned char r = (unsigned char)((c >> 16) & 0xFF);

    if ((void*) dc00 == NULL || (void*) dc00->data == NULL)
        return;
    if (w <= 0 || h <= 0)
        return;

    dw = (long) dc00->device_width;
    dh = (long) dc00->device_height;

    x2 = x + w;
    y2 = y + h;
    if (x  < 0)  x  = 0;
    if (y  < 0)  y  = 0;
    if (x2 > dw) x2 = dw;
    if (y2 > dh) y2 = dh;
    if (x >= x2 || y >= y2)
        return;

    for (yy = y; yy < y2; yy++)
    {
        row = (unsigned char *) dc00->data + ((unsigned long) yy * dc00->pitch);

        if (dc00->bpp == 32)
        {
            unsigned int *p = (unsigned int *) (row + ((unsigned long) x << 2));
            for (xx = x; xx < x2; xx++)
                *p++ = c;
        }
        else if (dc00->bpp == 24)
        {
            unsigned char *p = row + ((unsigned long) x * 3);
            for (xx = x; xx < x2; xx++)
            {
                p[0] = b;
                p[1] = g;
                p[2] = r;
                p += 3;
            }
        }
    }
}

// Integer square root (floor) for small numbers.
static int isqrt_i(int n)
{
    int r = 0;
    while (((r + 1) * (r + 1)) <= n)
        r++;
    return r;
}

// A filled circle, drawn as horizontal spans.
static void rv_fill_circle(long cx, long cy, long rad, unsigned int color)
{
    long dy;
    long dx;

    if (rad < 1)
        return;

    for (dy = -rad; dy <= rad; dy++)
    {
        dx = (long) isqrt_i((int)((rad * rad) - (dy * dy)));
        rv_fill_rect(cx - dx, cy + dy, (dx * 2) + 1, 1, color);
    }
}

// A frame inside a rectangle.
static void 
rv_frame(long x, long y, long w, long h, long t, unsigned int color)
{
    rv_fill_rect(x,         y,         w, t,         color);  // top
    rv_fill_rect(x,         y + h - t, w, t,         color);  // bottom
    rv_fill_rect(x,         y + t,     t, h - (t*2), color);  // left
    rv_fill_rect(x + w - t, y + t,     t, h - (t*2), color);  // right
}

// =====================================================
// Labels (header and footer)
// =====================================================

// Change the text of a component. Truncated to the buffer
// of the component and to max_chars (so it never leaves the window).
static void 
ui_set_label(struct ui_component_d *uic, const char *text, unsigned long max_chars)
{
    unsigned long n;
    unsigned long cap;

    if ((void*) uic == NULL || (void*) text == NULL)
        return;
    if (uic->magic != 1234)
        return;

    cap = (unsigned long) sizeof(uic->label) - 1;
    n = (unsigned long) strlen(text);
    if (n > cap)
        n = cap;
    if (n > max_chars)
        n = max_chars;

    memcpy(uic->label, text, n);
    uic->label[n] = '\0';
    uic->label_size = n;
}

// =====================================================
// The rules
// =====================================================

static int in_board(int r, int c)
{
    return (r >= 0 && r < N && c >= 0 && c < N);
}

// How many discs a move flips in one direction.
static int flips_in_dir(int pl, int r, int c, int dr, int dc)
{
    int n = 0;

    r += dr;
    c += dc;
    while (in_board(r, c) && board[r][c] == OPP(pl))
    {
        n++;
        r += dr;
        c += dc;
    }
    if (n > 0 && in_board(r, c) && board[r][c] == pl)
        return n;

    return 0;
}

// How many discs a move would flip (0 = illegal).
static int count_flips(int pl, int r, int c)
{
    int dr, dc;
    int total = 0;

    if (!in_board(r, c) || board[r][c] != EMPTY)
        return 0;

    for (dr = -1; dr <= 1; dr++)
    {
        for (dc = -1; dc <= 1; dc++)
        {
            if (dr == 0 && dc == 0)
                continue;
            total += flips_in_dir(pl, r, c, dr, dc);
        }
    }
    return total;
}

static int is_legal(int pl, int r, int c)
{
    return (count_flips(pl, r, c) > 0);
}

// Put a disc and flip what has to be flipped.
static void play_move(int pl, int r, int c)
{
    int dr, dc, n, i;

    for (dr = -1; dr <= 1; dr++)
    {
        for (dc = -1; dc <= 1; dc++)
        {
            if (dr == 0 && dc == 0)
                continue;

            n = flips_in_dir(pl, r, c, dr, dc);
            for (i = 1; i <= n; i++)
                board[r + (dr * i)][c + (dc * i)] = (unsigned char) pl;
        }
    }

    board[r][c] = (unsigned char) pl;
    last_r = r;
    last_c = c;
}

static int count_moves(int pl)
{
    int r, c;
    int n = 0;

    for (r = 0; r < N; r++)
        for (c = 0; c < N; c++)
            if (is_legal(pl, r, c))
                n++;
    return n;
}

static void count_discs(int *black, int *white)
{
    int r, c;
    int b = 0;
    int w = 0;

    for (r = 0; r < N; r++){
        for (c = 0; c < N; c++){
            if (board[r][c] == BLACK) b++;
            if (board[r][c] == WHITE) w++;
        }
    }
    *black = b;
    *white = w;
}

// =====================================================
// The flow of the game
// =====================================================

static void set_status(const char *text)
{
    strncpy(status_text, text, sizeof(status_text) - 1);
    status_text[sizeof(status_text) - 1] = '\0';
}

static void schedule_ai(void)
{
    ai_pending = TRUE;
    ai_due = (unsigned long) rtl_jiffies() + RV_AI_DELAY_MS;
}

static void new_game(void)
{
    memset(board, EMPTY, sizeof(board));
    board[3][3] = WHITE;
    board[3][4] = BLACK;
    board[4][3] = BLACK;
    board[4][4] = WHITE;

    turn = BLACK;
    game_over = FALSE;
    last_r = -1;
    last_c = -1;
    undo_valid = FALSE;
    ai_pending = FALSE;
    pressed_r = -1;
    pressed_c = -1;

    set_status("Your move - you are Black");
}

// After a move: who plays next? Passes and the end of the game.
static void after_move(int mover)
{
    int other = OPP(mover);
    int b, w;
    const char *msg = NULL;

    if (count_moves(other) > 0)
    {
        turn = other;
    }
    else if (count_moves(mover) > 0)
    {
        // The other player has no moves: the mover plays again.
        turn = mover;
        if (other == BLACK)
            msg = "You have no moves - pass";
        else
            msg = "Computer has no moves - your turn";
    }
    else
    {
        game_over = TRUE;
    }

    if (game_over == TRUE)
    {
        count_discs(&b, &w);
        if (b > w)       set_status("Game over - You win!");
        else if (w > b)  set_status("Game over - Computer wins");
        else             set_status("Game over - Draw");
        return;
    }

    if (turn == WHITE)
    {
        set_status( msg ? msg : "Computer is thinking..." );
        schedule_ai();
    }
    else
    {
        set_status( msg ? msg : "Your move - you are Black" );
    }
}

static void human_move(int r, int c)
{
    if (game_over == TRUE || turn != BLACK || ai_pending == TRUE)
        return;
    if (!is_legal(BLACK, r, c))
        return;

    // One level of undo: the board before this move.
    memcpy(undo_board, board, sizeof(board));
    undo_valid = TRUE;

    play_move(BLACK, r, c);
    after_move(BLACK);

    draw_board();
    refresh_status();
}

// The computer plays. Greedy, with positional weights.
static void ai_move(void)
{
    int r, c;
    int best_r = -1;
    int best_c = -1;
    int best_score = -100000;
    int f, score;

    ai_pending = FALSE;

    if (game_over == TRUE || turn != WHITE)
        return;

    for (r = 0; r < N; r++)
    {
        for (c = 0; c < N; c++)
        {
            f = count_flips(WHITE, r, c);
            if (f <= 0)
                continue;

            score = (Weight[r][c] * 4) + f;
            if (score > best_score){
                best_score = score;
                best_r = r;
                best_c = c;
            }
        }
    }

    if (best_r < 0)
        return;

    play_move(WHITE, best_r, best_c);
    after_move(WHITE);

    draw_board();
    refresh_status();
}

static void do_undo(void)
{
    if (undo_valid != TRUE)
        return;

    memcpy(board, undo_board, sizeof(board));
    undo_valid = FALSE;
    turn = BLACK;
    game_over = FALSE;
    ai_pending = FALSE;
    last_r = -1;
    last_c = -1;
    set_status("Your move - you are Black");

    draw_board();
    refresh_status();
}

// =====================================================
// Header and footer
// =====================================================
static void refresh_status(void)
{
    char text[32];
    int b, w;
    unsigned long cr_w;

    if ((void*) dc00 == NULL)
        return;

    cr_w = (unsigned long) dc00->device_width;

    // Header: the scoreboard (scale 2 = 16 pixels per char)
    count_discs(&b, &w);
    sprintf(text, "Black %d - White %d", b, w);
    ui_set_label(uic_header, text, (cr_w > 16) ? ((cr_w - 16) / 16) : 0);
    libgui_redraw_ui_component(uic_header, dc00);

    // Footer: what is going on (scale 1 = 8 pixels per char)
    ui_set_label(uic_footer, status_text, (cr_w > 16) ? ((cr_w - 16) / 8) : 0);
    libgui_redraw_ui_component(uic_footer, dc00);
}

// =====================================================
// The board
// =====================================================

// A disc: a ring and the inside.
static void draw_disc(long cx, long cy, long rad, int pl)
{
    if (pl == BLACK)
    {
        rv_fill_circle(cx, cy, rad,     0x00707070);   // gray ring
        rv_fill_circle(cx, cy, rad - 2, 0x00000000);
    }
    else
    {
        rv_fill_circle(cx, cy, rad,     0x00000000);   // black ring
        rv_fill_circle(cx, cy, rad - 2, 0x00FFFFFF);
    }
}

static void draw_cell(int r, int c)
{
    long x, y, rad, mark;
    int legal;
    unsigned int color;

    if (!in_board(r, c))
        return;

    x = board_x + (c * cell_sz);
    y = board_y + (r * cell_sz);

// The cell
    rv_fill_rect(x,     y,     cell_sz,     cell_sz,     GRID_COLOR);
    rv_fill_rect(x + 1, y + 1, cell_sz - 2, cell_sz - 2, BOARD_COLOR);

    legal = (turn == BLACK && game_over != TRUE && ai_pending != TRUE &&
             is_legal(BLACK, r, c));

// A disc, or a hint marker on the cells where you can play
    if (board[r][c] != EMPTY)
    {
        rad = (cell_sz * 2) / 5;
        if (rad < 3) rad = 3;
        draw_disc(x + (cell_sz / 2), y + (cell_sz / 2), rad, board[r][c]);

        // The last disc placed
        if (r == last_r && c == last_c)
        {
            mark = cell_sz / 8;
            if (mark < 2) mark = 2;
            rv_fill_rect( 
                x + (cell_sz / 2) - (mark / 2), 
                y + (cell_sz / 2) - (mark / 2),
                mark, mark, LASTMOVE_COLOR );
        }
    }
    else if (hints_on == TRUE && legal)
    {
        mark = cell_sz / 5;
        if (mark < 2) mark = 2;
        rv_fill_rect( 
            x + (cell_sz / 2) - (mark / 2), 
            y + (cell_sz / 2) - (mark / 2),
            mark, mark, HINT_COLOR );
    }

// The cursor (mouse or keyboard)
    if (r == cursor_r && c == cursor_c)
    {
        color = legal ? CURSOR_LEGAL_COLOR : CURSOR_NONE_COLOR;
        rv_frame(x + 1, y + 1, cell_sz - 2, cell_sz - 2, FRAME_THICKNESS, color);
    }
}

static void draw_board(void)
{
    int r, c;

    if ((void*) dc00 == NULL)
        return;

// The edge around the board
    rv_fill_rect(
        board_x - 2, board_y - 2, board_px + 4, board_px + 4, BOARD_EDGE_COLOR );

    for (r = 0; r < N; r++)
        for (c = 0; c < N; c++)
            draw_cell(r, c);
}

// Where is the cursor? Redraw only the two cells that change.
static void set_cursor(int r, int c)
{
    int old_r = cursor_r;
    int old_c = cursor_c;

    if (r == old_r && c == old_c)
        return;

    cursor_r = r;
    cursor_c = c;
    draw_cell(old_r, old_c);
    draw_cell(r, c);
}

// Is this point inside the board? Gives the cell.
static int hit_cell(unsigned long x, unsigned long y, int *r, int *c)
{
    long lx = (long) x - board_x;
    long ly = (long) y - board_y;

    if (lx < 0 || ly < 0 || lx >= board_px || ly >= board_px)
        return 0;

    *c = (int)(lx / cell_sz);
    *r = (int)(ly / cell_sz);
    return 1;
}

// =====================================================
// Frames around the buttons (hover)
// =====================================================

static void draw_item_frame(int button_id)
{
    struct rv_item_d *it;
    unsigned int color = FRAME_NONE_COLOR;
    long t = FRAME_THICKNESS;

    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    it = &ItemList[button_id - 1];

    if (button_id == __hover_button_id)
        color = FRAME_HOVER_COLOR;

    rv_frame( 
        (long) it->info.left - t, (long) it->info.top - t,
        (long) it->info.width + (t * 2), (long) it->info.height + (t * 2),
        t, color );
}

static void set_hover(int button_id)
{
    int old = __hover_button_id;

    if (button_id == old)
        return;

    __hover_button_id = button_id;
    draw_item_frame(old);
    draw_item_frame(button_id);
}

// =====================================================
// Button handlers (same roles as in the Power example)
// =====================================================

// For visual effects
// #todo: We need to implement this kind of worker inside the library
static void on_button_pressed(int button_id)
{
    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    libgui_on_button_pressed(ItemList[button_id - 1].uic, dc00);
}

// For visual effects
// #todo: We need to implement this kind of worker inside the library
static void on_button_released(int button_id)
{
    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    libgui_on_button_released(ItemList[button_id - 1].uic, dc00);
}

// Handle click events for components
static void on_button_clicked(int button_id)
{
    switch (button_id)
    {
        case BTN_NEW:
            RV_LOG("reversi: New game\n");
            new_game();
            draw_board();
            refresh_status();
            break;

        case BTN_UNDO:
            do_undo();
            break;

        case BTN_HINTS:
            hints_on = (hints_on == TRUE) ? FALSE : TRUE;
            ui_set_label(
                ItemList[BTN_HINTS - 1].uic, 
                (hints_on == TRUE) ? "Hints On" : "Hints Off", 
                16 );
            libgui_redraw_ui_component(ItemList[BTN_HINTS - 1].uic, dc00);
            draw_board();
            break;

        default:
            break;
    };
}

// Hit-test for the buttons
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my) 
{
    int i;
    struct button_info_d *b;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        b = &ItemList[i].info;

        if ( rel_mx >= b->left && 
             rel_mx <  b->left + b->width &&
             rel_my >= b->top  && 
             rel_my <  b->top  + b->height )
        {
            return (int) b->button_id;
        }
    }

    return -1;
}

// Which button does this key activate? (-1 = none)
static int __key_to_button_id(unsigned long key)
{
    int i;
    char c = (char) key;

    if (c >= 'A' && c <= 'Z')
        c = (char)(c + 32);
    if (c == 0)
        return -1;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        if (ItemList[i].key == c)
            return ItemList[i].info.button_id;
    }

    return -1;
}

// =====================================================
// Layout and painting
// =====================================================

// The board is a square that fits what is left between
// the header and the buttons/footer. The buttons go under the board.
static void layout(struct gws_window_info_d *wi)
{
    int i;
    long cr_w = (long) wi->cr_width;
    long cr_h = (long) wi->cr_height;
    long avail_w, avail_h, size;
    long bw, by;

    avail_w = cr_w - (MARGIN * 2);
    avail_h = cr_h - HEADER_H - FOOTER_H - BUTTON_H - (MARGIN * 4);

    size = (avail_w < avail_h) ? avail_w : avail_h;
    cell_sz = size / N;
    if (cell_sz < 8)
        cell_sz = 8;

    board_px = cell_sz * N;
    board_x  = (cr_w - board_px) / 2;
    board_y  = HEADER_H + MARGIN + ((avail_h > board_px) ? ((avail_h - board_px) / 2) : 0);

// Buttons: three, evenly spaced under the board
    bw = (board_px - (GAP * (ITEM_COUNT - 1))) / ITEM_COUNT;
    by = board_y + board_px + MARGIN + FRAME_THICKNESS;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        ItemList[i].info.button_id = i + 1;

        // Relative values
        ItemList[i].info.left   = (unsigned long)(board_x + (i * (bw + GAP)));
        ItemList[i].info.top    = (unsigned long) by;
        ItemList[i].info.width  = (unsigned long) bw;
        ItemList[i].info.height = BUTTON_H;

        // Absolute values
        ItemList[i].info.absolute_left = 
            wi->left + wi->cr_left + ItemList[i].info.left;
        ItemList[i].info.absolute_top = 
            wi->top  + wi->cr_top  + ItemList[i].info.top;
    }
}

static void update_children(int fd)
{
    struct gws_window_info_d  wi;
    int i;

    // Get window info
    gws_get_window_info(fd, main_window, &wi);

    if ((void*)dc00 == NULL)
        return;

    layout(&wi);

// The background for the client area
    rv_fill_rect(0, 0, wi.cr_width, wi.cr_height, PANEL_COLOR);

// Header and footer
    libgui_set_ui_component_position(uic_header, 0, 0);
    libgui_set_ui_component_dimension(uic_header, wi.cr_width, HEADER_H);
    libgui_set_ui_component_flags(uic_header, 0x0001);   // only centered horizontally

    libgui_set_ui_component_position(uic_footer, 0, wi.cr_height - FOOTER_H);
    libgui_set_ui_component_dimension(uic_footer, wi.cr_width, FOOTER_H);
    libgui_set_ui_component_flags(uic_footer, 0x0001);

    refresh_status();

// The board
    draw_board();

// Buttons: position, dimension and redraw.
// The component keeps its status, so a pressed button stays pressed.
    for (i = 0; i < ITEM_COUNT; i++)
    {
        libgui_set_ui_component_position(
            ItemList[i].uic, 
            ItemList[i].info.left, 
            ItemList[i].info.top );
        libgui_set_ui_component_dimension(
            ItemList[i].uic,
            ItemList[i].info.width,
            ItemList[i].info.height );
        libgui_set_ui_component_flags(ItemList[i].uic, (0x0001 | 0x0002) );
        libgui_redraw_ui_component(ItemList[i].uic, dc00);
    }

// The hover frame goes on top
    draw_item_frame(__hover_button_id);
}

// ----------------------------------------------------
// Procedure: handles the events
// ----------------------------------------------------
static int 
reversiProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    int ButtonId = -1;
    int r = 0;
    int c = 0;

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

    // Sent by the input loop when the delay of the computer is over.
    case RV_MSG_AI_TURN:
        ai_move();
        return 0;
        break;

    // Keyboard: the press effect on key down,
    // the action on key up. Just like the Power example.
    case MSG_KEYDOWN:

        if (long1 == 'Q' || long1 == 'q')
        {
            isTimeToQuit = TRUE;
            return 0;
        }

        // Play on the cursor
        if (long1 == VK_RETURN || long1 == ' ')
        {
            if (cursor_r >= 0)
                human_move(cursor_r, cursor_c);
            return 0;
        }

        ButtonId = __key_to_button_id(long1);
        // Ignore the auto-repeat: only the first key down counts.
        if (ButtonId > 0 && __key_pressed_id != ButtonId)
        {
            __key_pressed_id = ButtonId;
            on_button_pressed(ButtonId);  // Visual effect
        }
        return 0;
        break;

    case MSG_KEYUP:
        if (__key_pressed_id > 0)
        {
            on_button_released(__key_pressed_id);  // Visual effect

            // The key up can report another character (Shift, for example),
            // so the action only happens if it is the same button.
            ButtonId = __key_to_button_id(long1);
            if (ButtonId == __key_pressed_id)
                on_button_clicked(ButtonId);   // Action

            __key_pressed_id = -1;
        }
        return 0;
        break;

    // Arrow keys move the cursor on the board.
    case MSG_SYSKEYDOWN:
        r = cursor_r;
        c = cursor_c;
        if (r < 0){
            r = 3;
            c = 3;
        } else {
            switch (long1){
            case VK_ARROW_LEFT:   if (c > 0)     c--; break;
            case VK_ARROW_RIGHT:  if (c < N - 1) c++; break;
            case VK_ARROW_UP:     if (r > 0)     r--; break;
            case VK_ARROW_DOWN:   if (r < N - 1) r++; break;
            default: return 0;
            };
        }
        // Only the arrows get here
        if ( long1 == VK_ARROW_LEFT  || long1 == VK_ARROW_RIGHT ||
             long1 == VK_ARROW_UP    || long1 == VK_ARROW_DOWN )
        {
            set_cursor(r, c);
        }
        return 0;
        break;

    case GWS_MouseClicked:
        break;

    case MSG_MOUSEMOVE:
        // long1/long2: pointer position, relative to the client area.
        ButtonId = (int) __hit_test_button(long1, long2);
        set_hover(ButtonId);   // Frame effect on the buttons

        if (hit_cell(long1, long2, &r, &c))
            set_cursor(r, c);       // Frame effect on the board
        else
            set_cursor(-1, -1);
        return 0;
        break;

    case MSG_MOUSEPRESSED:
        // The pointer position is in last_x/last_y
        // (saved by the input loop from RTLEventBuffer[4]/[5]).
        if (hit_cell(last_x, last_y, &r, &c))
        {
            // A cell: the move happens when the button is released
            pressed_r = r;
            pressed_c = c;
            return 0;
        }

        ButtonId = (int) __hit_test_button(last_x, last_y);
        if (ButtonId < 0)
            ButtonId = __hover_button_id;

        __mouse_pressed_id = ButtonId;
        if (ButtonId > 0)
            on_button_pressed(ButtonId);  // Visual effect
        return 0;
        break;

    case MSG_MOUSERELEASED:

        // A cell: a click only counts if we are still on the same cell.
        if (pressed_r >= 0)
        {
            if (hit_cell(last_x, last_y, &r, &c) && 
                r == pressed_r && c == pressed_c)
            {
                human_move(r, c);
            }
            pressed_r = -1;
            pressed_c = -1;
        }

        if (__mouse_pressed_id > 0)
        {
            on_button_released(__mouse_pressed_id);  // Visual effect

            // A click only counts if the pointer is still
            // over the button that was pressed.
            ButtonId = (int) __hit_test_button(last_x, last_y);
            if (ButtonId < 0)
                ButtonId = __hover_button_id;

            if (ButtonId == __mouse_pressed_id)
                on_button_clicked(ButtonId);  // Action

            __mouse_pressed_id = -1;
        }
        return 0;
        break;

    case MSG_CLOSE:
        isTimeToQuit = TRUE;
        return 0;
        break;

    // Unknown event
    default:
        return -1;
        break;
    };

// Fail
    return (int) -1;
}

int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    int i;
    unsigned long now = 0;

    isTimeToQuit = FALSE;

    // Connect to display server
    Display = gws_open_display(display_name);
    if ((void*) Display == NULL) {
        printf("reversi: Could not open display\n");
        return EXIT_FAILURE;
    }

    client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("reversi: Invalid fd\n");
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
        printf("reversi: on libgui_initialize()\n");
        exit(1);
    }

// =========================================
// Main window
// The layout adapts to the client area the server gives us.

    unsigned long mw_style = WS_APP;

    unsigned long win_w = 420;
    unsigned long win_h = 560;
    if (win_w > screen_w) win_w = screen_w;
    if (win_h > screen_h) win_h = screen_h;

    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

    main_window = 
        (int) gws_create_window(
                client_fd,
                WT_OVERLAPPED,
                WINDOW_STATUS_ACTIVE,  //status
                WINDOW_STATE_NULL,  //state
                "Reversi",
                win_x, win_y, win_w, win_h,
                0,
                mw_style,  // style
                COLOR_WHITE, COLOR_GRAY );

    if (main_window < 0) {
        printf("reversi: Failed to create main window\n");
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
        printf("reversi: on dc00\n");
        exit(1);
    }

// The game starts
    new_game();

// bg for the client area
    rv_fill_rect(0, 0, wi.cr_width, wi.cr_height, PANEL_COLOR);

    layout(&wi);

// ============================================================
// Header and footer
// The header shows the scoreboard, the footer the status.

    uic_header = libgui_create_ui_component(
        dc00,
        UI_COMPONENT_HEADER,
        0, 0, wi.cr_width, HEADER_H,
        "Black 2 - White 2",
        0x0001 );
    if ((void*) uic_header == NULL){
        printf("reversi: on header\n");
        exit(1);
    }

    uic_footer = libgui_create_ui_component(
        dc00,
        UI_COMPONENT_FOOTER,
        0, wi.cr_height - FOOTER_H, wi.cr_width, FOOTER_H,
        status_text,
        0x0001 );
    if ((void*) uic_footer == NULL){
        printf("reversi: on footer\n");
        exit(1);
    }

// ============================================================
// Create the buttons.

    for (i = 0; i < ITEM_COUNT; i++)
    {
        ItemList[i].uic = libgui_create_ui_component (
            dc00, 
            UI_COMPONENT_BUTTON,   // type 1 = button 
            ItemList[i].info.left, 
            ItemList[i].info.top, 
            ItemList[i].info.width, 
            ItemList[i].info.height,
            ItemList[i].label,
            (0x0001 | 0x0002)
        );
        if ((void*) ItemList[i].uic == NULL){
            printf("reversi: on button %d\n", i);
            exit(1);
        }
    }

    draw_board();
    refresh_status();

// Main window: Activate and show.
    gws_set_active( client_fd, main_window );

//
// Event loop
//

// #ps:
// The purpose of this loop is only get system events,
// not to get events from the display server.
// We do this for performance reasons.

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

        // Pump events from Input Broker (system events)
        if (rtl_get_event() == TRUE)
        {
            last_x = (unsigned long) RTLEventBuffer[4];
            last_y = (unsigned long) RTLEventBuffer[5];

            // IN: wid, event type, VK, scancode.
            reversiProcedure(
                client_fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3] );
            RTLEventBuffer[1] = 0;  // Clear after dispatch
        }

        // The computer has a move to make:
        // the timer is only read while it is waiting.
        if (ai_pending == TRUE)
        {
            now = (unsigned long) rtl_jiffies();
            if (now >= ai_due)
            {
                reversiProcedure(
                    client_fd,
                    main_window,
                    RV_MSG_AI_TURN,
                    now,
                    0 );
            }
        }
    };

    if (isTimeToQuit == TRUE){
        printf("Reversi: Close window\n");
        gws_destroy_window(client_fd, main_window);
    }

    if (client_fd > 0)
        close(client_fd);

    return EXIT_SUCCESS;
}

