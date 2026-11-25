// main.c
// Gramado OS client-side GUI app: Pairs (Concentration / memory match)
//
// A simple board game: 16 face-down cards, 8 pairs. Flip two cards,
// if they are the same they stay open. Finish with the fewest moves.
//
//   [ header : "Moves 5  Pairs 3/8"                     ]  UI_COMPONENT_HEADER
//   [ board  : 4x4 cards                                 ]
//   [ buttons: New   Quit                                ]  UI_COMPONENT_BUTTON
//   [ footer : what is going on                          ]  UI_COMPONENT_FOOTER
//
// Only TWO bitmaps are used, loaded once at startup:
//   #folder.bmp and #file.bmp
// They are the pieces of the game. Two shapes and four ring colors give
// eight different faces (shape + color must both match).
// If a bitmap does not load, a simple shape drawn by the app is used
// instead, so the game always works.
//
// Mouse:
//   hover        blue frame on a card you can flip, gray on the others
//   click        flip a card (press and release on the same card)
// Keyboard:
//   arrows       move the cursor        Enter / Space   flip the card
//   N new game                          Q quit
//
// Same routines as the other apps:
//  - on_button_pressed / on_button_released / on_button_clicked
//  - hover tracking on MSG_MOUSEMOVE
//  - the same event loop: shared-flags blit check, then rtl_get_event()
//    handing the event to the procedure.
//  - Like Reversi: when two cards do not match, the loop reads the timer
//    only while they wait to be turned back, then hands a private message
//    (PAIRS_MSG_HIDE) to the procedure.
//
// #bugbug: We do not know the size of a loaded bitmap. We assume
// ICON_SRC_SIZE pixels and center the bitmap using that.
// #todo: Use the real dimensions when the bmp_cache_d has them.

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

#include "pairs.h"

// ----------------------------------------------------
// Config
// ----------------------------------------------------

// Set to 1 to get messages on the console.
#define PAIRS_DEBUG  0
#if PAIRS_DEBUG
#define PAIRS_LOG(...)  printf(__VA_ARGS__)
#else
#define PAIRS_LOG(...)  ((void)0)
#endif

// Private message: turn the two cards back (sent by the input loop).
#define PAIRS_MSG_HIDE   99710

// The cards wait this long before they are turned back.
// #bugbug: valid only if the timer fires 1000 times a second.
#define PAIRS_HIDE_DELAY_MS   800

// The bitmaps (the pieces of the game)
#define ICON_COUNT      2
#define ICON_SRC_SIZE   16      // we assume 16x16 pixels
#define ICON_MAX_ZOOM   4

// Layout
#define MARGIN        8
#define GAP           8
#define HEADER_H      32      // text is scale 2 (16px), top + 8
#define FOOTER_H      24      // text is scale 1 (8px),  top + 8
#define BUTTON_H      28
#define CARD_GAP      6       // space between the cards
#define RING          6       // the colored ring of a face

// Look
#define PANEL_COLOR        LIBGUI_COLOR_PANEL_FACE
#define FELT_COLOR         0x00386048
#define FRAME_THICKNESS    2
#define FRAME_HOVER_COLOR  0x000060C0
#define FRAME_NONE_COLOR   PANEL_COLOR
#define CURSOR_OK_COLOR    0x000060C0
#define CURSOR_NO_COLOR    0x00909090

// The game
#define COLS         4
#define ROWS         4
#define CARD_COUNT   (COLS * ROWS)
#define PAIR_COUNT   (CARD_COUNT / 2)

// State of a card
#define CARD_HIDDEN   0
#define CARD_OPEN     1
#define CARD_MATCHED  2

static int isTimeToQuit = FALSE;

// Global display pointer
struct gws_display_d *Display;

struct dccanvas_d *dc00;  // shared dc

static int client_fd   = -1;
static int main_window = -1;

static unsigned long __sh_flags = 0;

// ----------------------------------------------------
// The bitmaps: only two
// ----------------------------------------------------
static struct bmp_cache_d *Icon[ICON_COUNT];
static const char *IconFile[ICON_COUNT] = {
    "#folder.bmp",
    "#file.bmp"
};

// The colors of the rings. Four colors x two shapes = eight faces.
static const unsigned int RingColor[PAIR_COUNT / 2] = {
    0x00D04040,   // red
    0x0040A040,   // green
    0x004060D0,   // blue
    0x00E0A020    // orange
};

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
#define BTN_NEW   1
#define BTN_QUIT  2

struct pairs_item_d
{
    struct button_info_d info;
    struct ui_component_d *uic;
    const char *label;
    char key;              // shortcut (lowercase)
};

static struct pairs_item_d  ItemList[] = {
    { {0}, NULL, "New",  'n' },
    { {0}, NULL, "Quit", 'q' }
};
#define ITEM_COUNT  (int)(sizeof(ItemList)/sizeof(ItemList[0]))

// ----------------------------------------------------
// Interface state
// ----------------------------------------------------
static int __hover_button_id  = -1;
static int __mouse_pressed_id = -1;
static int __key_pressed_id   = -1;

// The card under the mouse / the keyboard cursor (-1 = none)
static int cursor_card = -1;
// The card where the mouse button went down (-1 = none)
static int pressed_card = -1;

// Position of the pointer at the time of the event.
// Filled by the input loop from the event buffer.
static unsigned long last_x = 0;
static unsigned long last_y = 0;

// Geometry (relative to the client area)
static long cell_sz  = 80;
static long board_x  = 0;
static long board_y  = 0;
static long board_px = 320;
static int  icon_zoom = 3;

// ----------------------------------------------------
// Game state
// ----------------------------------------------------
struct card_d
{
    unsigned char face;    // 0..7. shape = face & 1, color = face >> 1
    unsigned char state;
};
static struct card_d Card[CARD_COUNT];

static int first_pick  = -1;
static int second_pick = -1;
static int moves       = 0;
static int pairs_found = 0;
static int game_won    = FALSE;

// The two cards that did not match wait to be turned back.
static int hide_pending = FALSE;
static unsigned long hide_due = 0;

static char status_text[48];

static unsigned int rng_state = 2463534242U;

// -----------------
static void on_button_pressed(int button_id);
static void on_button_released(int button_id);
static void on_button_clicked(int button_id);

static int  __hit_test_button(unsigned long rel_mx, unsigned long rel_my);
static void layout(struct gws_window_info_d *wi);
static void draw_board(void);
static void draw_card(int i);
static void refresh_status(void);
static void update_children(int fd);

// =====================================================
// Drawing helpers
// =====================================================

// Fill a rectangle, clipped against the dc. ROP_COPY only.
static void 
pr_fill_rect(
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

// A frame inside a rectangle.
static void 
pr_frame(long x, long y, long w, long h, long t, unsigned int color)
{
    pr_fill_rect(x,         y,         w, t,         color);  // top
    pr_fill_rect(x,         y + h - t, w, t,         color);  // bottom
    pr_fill_rect(x,         y + t,     t, h - (t*2), color);  // left
    pr_fill_rect(x + w - t, y + t,     t, h - (t*2), color);  // right
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

static void set_status(const char *text)
{
    strncpy(status_text, text, sizeof(status_text) - 1);
    status_text[sizeof(status_text) - 1] = '\0';
}

static void refresh_status(void)
{
    char text[32];
    unsigned long cr_w;

    if ((void*) dc00 == NULL)
        return;

    cr_w = (unsigned long) dc00->device_width;

    // Header: the score (scale 2 = 16 pixels per char)
    sprintf(text, "Moves %d  Pairs %d/%d", moves, pairs_found, PAIR_COUNT);
    ui_set_label(uic_header, text, (cr_w > 16) ? ((cr_w - 16) / 16) : 0);
    libgui_redraw_ui_component(uic_header, dc00);

    // Footer: what is going on (scale 1 = 8 pixels per char)
    ui_set_label(uic_footer, status_text, (cr_w > 16) ? ((cr_w - 16) / 8) : 0);
    libgui_redraw_ui_component(uic_footer, dc00);
}

// =====================================================
// The game
// =====================================================

static unsigned int rng_next(void)
{
    unsigned int x = rng_state;

    x ^= (x << 13);
    x ^= (x >> 17);
    x ^= (x << 5);
    rng_state = x;
    return x;
}

// Shuffle the deck and start again.
static void new_game(void)
{
    int i, j;
    unsigned char tmp;

    // The seed comes from the timer
    rng_state ^= (unsigned int) rtl_jiffies();
    if (rng_state == 0)
        rng_state = 2463534242U;

    // Two cards of each face
    for (i = 0; i < CARD_COUNT; i++){
        Card[i].face  = (unsigned char)(i / 2);
        Card[i].state = CARD_HIDDEN;
    }

    // Fisher-Yates
    for (i = CARD_COUNT - 1; i > 0; i--)
    {
        j = (int)(rng_next() % (unsigned int)(i + 1));
        tmp = Card[i].face;
        Card[i].face = Card[j].face;
        Card[j].face = tmp;
    }

    first_pick = -1;
    second_pick = -1;
    moves = 0;
    pairs_found = 0;
    game_won = FALSE;
    hide_pending = FALSE;
    pressed_card = -1;

    set_status("Pick a card");
}

// Can this card be flipped now?
static int can_flip(int i)
{
    if (i < 0 || i >= CARD_COUNT)
        return 0;
    if (game_won == TRUE || hide_pending == TRUE)
        return 0;
    return (Card[i].state == CARD_HIDDEN);
}

static void flip_card(int i)
{
    char text[48];

    if (!can_flip(i))
        return;

    Card[i].state = CARD_OPEN;

    // The first card of the move
    if (first_pick < 0)
    {
        first_pick = i;
        set_status("Pick the second card");
        draw_card(i);
        refresh_status();
        return;
    }

    // The second card
    second_pick = i;
    moves++;

    if (Card[first_pick].face == Card[second_pick].face)
    {
        // A pair
        Card[first_pick].state  = CARD_MATCHED;
        Card[second_pick].state = CARD_MATCHED;
        pairs_found++;

        if (pairs_found >= PAIR_COUNT)
        {
            game_won = TRUE;
            sprintf(text, "You won in %d moves! Press N", moves);
            set_status(text);
        }
        else
        {
            set_status("A pair!");
        }

        draw_card(first_pick);
        draw_card(second_pick);
        first_pick = -1;
        second_pick = -1;
    }
    else
    {
        // Not a pair: they stay open for a moment
        set_status("No match");
        draw_card(second_pick);

        hide_pending = TRUE;
        hide_due = (unsigned long) rtl_jiffies() + PAIRS_HIDE_DELAY_MS;
    }

    refresh_status();
}

// The delay is over: turn the two cards back.
static void hide_cards(void)
{
    int a = first_pick;
    int b = second_pick;

    hide_pending = FALSE;

    if (a >= 0 && Card[a].state == CARD_OPEN)
        Card[a].state = CARD_HIDDEN;
    if (b >= 0 && Card[b].state == CARD_OPEN)
        Card[b].state = CARD_HIDDEN;

    first_pick = -1;
    second_pick = -1;

    set_status("Pick a card");

    draw_card(a);
    draw_card(b);
    refresh_status();
}

// =====================================================
// The board
// =====================================================

// The piece of a face, centered at cx,cy.
// shape 0 = folder, shape 1 = file. 
// Uses the bitmap, or a simple drawing if the bitmap is not loaded.
static void draw_piece(int shape, long cx, long cy)
{
    struct bmp_cache_d *cache = Icon[shape];
    long size, x, y;

    if (cache != NULL && cache->loaded)
    {
        size = (long) ICON_SRC_SIZE * icon_zoom;
        bmp_decode_bmp_image(
            cache, dc00, 
            cx - (size / 2), cy - (size / 2), 
            icon_zoom );
        return;
    }

    // Fallback: the bitmap is not here.
    size = (long) ICON_SRC_SIZE * icon_zoom;
    x = cx - (size / 2);
    y = cy - (size / 2);

    if (shape == 0)
    {
        // A folder: a tab and a body
        pr_fill_rect(x,              y + (size / 6), size / 2, size / 6, 0x00C0A020);
        pr_fill_rect(x,              y + (size / 3), size,     (size * 2) / 3, 0x00F0D040);
    }
    else
    {
        // A file: a page with a border
        pr_fill_rect(x + (size / 6), y,              (size * 2) / 3, size, 0x00404040);
        pr_fill_rect(x + (size / 6) + 2, y + 2,      ((size * 2) / 3) - 4, size - 4, 0x00FFFFFF);
    }
}

static void draw_card(int i)
{
    long cx, cy, x, y, w, h, t;
    int face, shape, color_index;
    unsigned int ring;

    if (i < 0 || i >= CARD_COUNT)
        return;

    cx = board_x + ((i % COLS) * cell_sz);
    cy = board_y + ((i / COLS) * cell_sz);

// The cell: clears what was there (also the old cursor frame)
    pr_fill_rect(cx, cy, cell_sz, cell_sz, FELT_COLOR);

    x = cx + (CARD_GAP / 2);
    y = cy + (CARD_GAP / 2);
    w = cell_sz - CARD_GAP;
    h = cell_sz - CARD_GAP;
    t = RING;

    if (Card[i].state == CARD_HIDDEN)
    {
        // The back of the card
        pr_fill_rect(x,     y,     w,     h,     0x00202850);
        pr_fill_rect(x + 2, y + 2, w - 4, h - 4, 0x00305090);
        pr_fill_rect(x + t, y + t, w - (t*2), h - (t*2), 0x004068B0);
        libgui_drawstring_dc(
            dc00, x + (w / 2) - 4, y + (h / 2) - 4, 
            0x00FFFFFF, 0x004068B0, 0, "?" );
    }
    else
    {
        // The face: colored ring, white inside, the piece
        face = Card[i].face;
        shape = face & 1;
        color_index = face >> 1;
        ring = RingColor[color_index];

        if (Card[i].state == CARD_MATCHED)
            pr_fill_rect(x, y, w, h, 0x00206020);       // dark green edge
        else
            pr_fill_rect(x, y, w, h, 0x00202020);

        pr_fill_rect(x + 2, y + 2, w - 4, h - 4, ring);
        pr_fill_rect(x + t, y + t, w - (t*2), h - (t*2), 0x00FFFFFF);

        draw_piece(shape, x + (w / 2), y + (h / 2));
    }

// The cursor (mouse or keyboard), in the space between the cards
    if (i == cursor_card)
    {
        pr_frame( 
            x - FRAME_THICKNESS, y - FRAME_THICKNESS, 
            w + (FRAME_THICKNESS * 2), h + (FRAME_THICKNESS * 2), 
            FRAME_THICKNESS, 
            can_flip(i) ? CURSOR_OK_COLOR : CURSOR_NO_COLOR );
    }
}

static void draw_board(void)
{
    int i;

    if ((void*) dc00 == NULL)
        return;

// The felt around the cards
    pr_fill_rect(
        board_x - 4, board_y - 4, board_px + 8, board_px + 8, FELT_COLOR );

    for (i = 0; i < CARD_COUNT; i++)
        draw_card(i);
}

// Where is the cursor? Redraw only the two cards that change.
static void set_cursor(int i)
{
    int old = cursor_card;

    if (i == old)
        return;

    cursor_card = i;
    draw_card(old);
    draw_card(i);
}

// Is this point inside the board? Gives the card.
static int hit_card(unsigned long x, unsigned long y)
{
    long lx = (long) x - board_x;
    long ly = (long) y - board_y;

    if (lx < 0 || ly < 0 || lx >= board_px || ly >= board_px)
        return -1;

    return (int)(((ly / cell_sz) * COLS) + (lx / cell_sz));
}

// =====================================================
// Frames around the buttons (hover)
// =====================================================

static void draw_item_frame(int button_id)
{
    struct pairs_item_d *it;
    unsigned int color = FRAME_NONE_COLOR;
    long t = FRAME_THICKNESS;

    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    it = &ItemList[button_id - 1];

    if (button_id == __hover_button_id)
        color = FRAME_HOVER_COLOR;

    pr_frame( 
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
            PAIRS_LOG("pairs: New game\n");
            new_game();
            draw_board();
            refresh_status();
            break;

        case BTN_QUIT:
            isTimeToQuit = TRUE;
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
    long bw, by, zoom;

    avail_w = cr_w - (MARGIN * 2);
    avail_h = cr_h - HEADER_H - FOOTER_H - BUTTON_H - (MARGIN * 4);

    size = (avail_w < avail_h) ? avail_w : avail_h;
    cell_sz = size / COLS;
    if (cell_sz < 24)
        cell_sz = 24;

    board_px = cell_sz * COLS;
    board_x  = (cr_w - board_px) / 2;
    board_y  = HEADER_H + MARGIN + ((avail_h > board_px) ? ((avail_h - board_px) / 2) : 0);

// The bitmap must fit inside the white area of a card.
    zoom = (cell_sz - CARD_GAP - (RING * 2)) / ICON_SRC_SIZE;
    if (zoom < 1) zoom = 1;
    if (zoom > ICON_MAX_ZOOM) zoom = ICON_MAX_ZOOM;
    icon_zoom = (int) zoom;

// Buttons: evenly spaced under the board
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
    pr_fill_rect(0, 0, wi.cr_width, wi.cr_height, PANEL_COLOR);

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
pairsProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    int ButtonId = -1;
    int card = -1;

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

    // Sent by the input loop when the cards have to be turned back.
    case PAIRS_MSG_HIDE:
        hide_cards();
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

        // Flip the card under the cursor
        if (long1 == VK_RETURN || long1 == ' ')
        {
            if (cursor_card >= 0)
                flip_card(cursor_card);
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
        if ( long1 != VK_ARROW_LEFT  && long1 != VK_ARROW_RIGHT &&
             long1 != VK_ARROW_UP    && long1 != VK_ARROW_DOWN )
            return 0;

        card = cursor_card;
        if (card < 0)
        {
            card = 0;
        }
        else
        {
            switch (long1){
            case VK_ARROW_LEFT:   if ((card % COLS) > 0)         card--;       break;
            case VK_ARROW_RIGHT:  if ((card % COLS) < COLS - 1)  card++;       break;
            case VK_ARROW_UP:     if (card >= COLS)              card -= COLS; break;
            case VK_ARROW_DOWN:   if (card < (CARD_COUNT-COLS))  card += COLS; break;
            default: break;
            };
        }
        set_cursor(card);
        return 0;
        break;

    case GWS_MouseClicked:
        break;

    case MSG_MOUSEMOVE:
        // long1/long2: pointer position, relative to the client area.
        ButtonId = (int) __hit_test_button(long1, long2);
        set_hover(ButtonId);   // Frame effect on the buttons

        set_cursor( hit_card(long1, long2) );   // Frame effect on the cards
        return 0;
        break;

    case MSG_MOUSEPRESSED:
        // The pointer position is in last_x/last_y
        // (saved by the input loop from RTLEventBuffer[4]/[5]).
        card = hit_card(last_x, last_y);
        if (card >= 0)
        {
            // A card: it is flipped when the button is released
            pressed_card = card;
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

        // A card: a click only counts if we are still on the same card.
        if (pressed_card >= 0)
        {
            card = hit_card(last_x, last_y);
            if (card == pressed_card)
                flip_card(card);
            pressed_card = -1;
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
        printf("pairs: Could not open display\n");
        return EXIT_FAILURE;
    }

    client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("pairs: Invalid fd\n");
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
        printf("pairs: on libgui_initialize()\n");
        exit(1);
    }

// =========================================
// Main window
// The layout adapts to the client area the server gives us.

    unsigned long mw_style = WS_APP;

    unsigned long win_w = 420;
    unsigned long win_h = 540;
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
                "Pairs",
                win_x, win_y, win_w, win_h,
                0,
                mw_style,  // style
                COLOR_WHITE, COLOR_GRAY );

    if (main_window < 0) {
        printf("pairs: Failed to create main window\n");
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
        printf("pairs: on dc00\n");
        exit(1);
    }

// ============================================================
// Load the two bitmaps, once. If one fails, the game 
// draws a simple shape in its place.

    for (i = 0; i < ICON_COUNT; i++)
    {
        Icon[i] = (struct bmp_cache_d *) bmp_load_bmp_image(IconFile[i]);
        if (Icon[i] == NULL || !Icon[i]->loaded)
            PAIRS_LOG("pairs: %s not loaded\n", IconFile[i]);
    }

// The game starts
    new_game();

// bg for the client area
    pr_fill_rect(0, 0, wi.cr_width, wi.cr_height, PANEL_COLOR);

    layout(&wi);

// ============================================================
// Header and footer
// The header shows the score, the footer the status.

    uic_header = libgui_create_ui_component(
        dc00,
        UI_COMPONENT_HEADER,
        0, 0, wi.cr_width, HEADER_H,
        "Moves 0  Pairs 0/8",
        0x0001 );
    if ((void*) uic_header == NULL){
        printf("pairs: on header\n");
        exit(1);
    }

    uic_footer = libgui_create_ui_component(
        dc00,
        UI_COMPONENT_FOOTER,
        0, wi.cr_height - FOOTER_H, wi.cr_width, FOOTER_H,
        status_text,
        0x0001 );
    if ((void*) uic_footer == NULL){
        printf("pairs: on footer\n");
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
            printf("pairs: on button %d\n", i);
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
            pairsProcedure(
                client_fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3] );
            RTLEventBuffer[1] = 0;  // Clear after dispatch
        }

        // Two cards are waiting to be turned back:
        // the timer is only read while they wait.
        if (hide_pending == TRUE)
        {
            now = (unsigned long) rtl_jiffies();
            if (now >= hide_due)
            {
                pairsProcedure(
                    client_fd,
                    main_window,
                    PAIRS_MSG_HIDE,
                    now,
                    0 );
            }
        }
    };

    if (isTimeToQuit == TRUE){
        printf("Pairs: Close window\n");
        gws_destroy_window(client_fd, main_window);
    }

    if (client_fd > 0)
        close(client_fd);

    return EXIT_SUCCESS;
}
