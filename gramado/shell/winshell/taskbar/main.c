// main.c
// Main file for taskbar.bin application. (Taskbar only)
// Program image: taskbar.bin
// Based on the desktop.bin (explorer-like) v2 design.
// Created by Fred Nora
//
// Scope:
//  - This application is ONLY the taskbar. There is no desktop window
//    and no desktop dc here. The desktop belongs to another program.
//  - It is the system shell thread (sc80 46), so it must never die.
//
// Layout (left to right):
//    [ launch area ][        icon area        ][ notification area ]
//      Start button   one icon per AppList       live clock
//
// Design rules (reliability first):
//  - One generic item table (ItemList) for hit-test, press/release
//    effects and clicks.
//  - Each item stores the program name it launches. No 1-based
//    numbers, no id arithmetic, so no off-by-one mistakes.
//  - A click only happens when the button that was pressed is still
//    under the pointer on release.
//  - Window info is fetched once and cached.
//  - Backgrounds use a direct-write rectangle routine (tb_fill_rect).
//  - Every pointer is checked before use. Creation failures of
//    optional parts (icons, clock) are not fatal. Creation failures
//    of the mandatory parts (display, taskbar window, dc) exit
//    before this thread is announced as the shell... see main().
//  - Launches have a short cooldown, so a double click or a key
//    repeat does not spawn two processes.
//  - No printf in the event path (enable with TB_DEBUG).
//  - The main loop always yields, so it never spins at 100% when
//    a round takes less than one jiffie.
//
// The input loop:
//   start_jiffie; shared-flags blit check; if (rtl_get_event()) procedure();
//   clock tick; yield.

// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>

// libgws - The client-side library.
#include <gws.h>

// The client-side library
#include <libgui.h>

#include "theme.h"
#include "taskbar.h"

// ----------------------------------------------------
// Config
// ----------------------------------------------------

// Set to 1 to get messages on the console. Keep it 0 normally:
// printf on the console is slow and this app is the shell.
#define TB_DEBUG  0
#if TB_DEBUG
#define TB_LOG(...)  printf(__VA_ARGS__)
#else
#define TB_LOG(...)  ((void)0)
#endif

// #bugbug
// The display server must agree with this value.
#define TASKBAR_HEIGHT  48

// Fallback if taskbar.h does not define it.
#ifndef CONFIG_MAIN_LOOP_INTERVAL_MS
#define CONFIG_MAIN_LOOP_INTERVAL_MS  16
#endif

// The clock is refreshed once per this many jiffies.
#define TB_CLOCK_INTERVAL_MS   1000

// #bugbug: valid only if the timer fires 1000 times a second.
#define TB_JIFFIES_PER_SECOND  1000

// Minimum time between two launches (ms).
#define TB_LAUNCH_COOLDOWN_MS  300

// Common component flags (centered label)
#define TB_FLAGS_CENTERED  (0x0001 | 0x0002)

// Layout
#define __ICON_GAP      2    // space between icons
#define __LEFT_MARGIN   2    // small margin from the left of the area
#define CLOCK_WIDTH     90
#define CLOCK_MARGIN    4

// Shared flags (shared area of the window)
#define SH_FLAG_BLIT    0x0008

// Component magic
#define UIC_MAGIC       1234

// ----------------------------------------------------
// Globals
// ----------------------------------------------------

struct gws_display_d *Display;

struct dccanvas_d *dc_taskbar;  // shared dc for the taskbar window

static unsigned long __sh_flags = 0;

// Device info
struct device_info_d 
{
    unsigned long width;
    unsigned long height;
};
struct device_info_d  DeviceInfo;

// Taskbar info
// See: taskbar.h
struct taskbar_info_d  TaskbarInfo;

// Clients
// It represents the order in the taskbar.
// If an entry is not used, it will not be shown in the taskbar.
struct tb_client_d clientList[CLIENT_COUNT_MAX];

// Client area size of the taskbar window (cached, it never changes).
static unsigned long tb_cr_w = 0;
static unsigned long tb_cr_h = 0;

// Taskbar window
static int taskbar_window = -1;

static int isTimeToQuit = FALSE;

const char *program_name = "Taskbar";

// ----------------------------------------------------
// Programs that can be launched from the taskbar
// ----------------------------------------------------

// The Start button. It launches this program for now.
// #todo: Start menu.
static const char *StartProgram = "#power.bin";

struct app_entry_d
{
    const char *name;         // program image
    const char *icon_label;   // 1 or 2 chars
};

// One icon for each entry. Keys '1'..'9' launch them in order.
static const struct app_entry_d AppList[] = {
    { "#editor.bin",   "E" },  // key '1'
    { "#calc.bin",     "C" },  // key '2'
    { "#paint.bin",    "P" }   // key '3'
    // { "#terminal.bin", "T" },
    // { "#browser.bin",  "B" },
};
#define APP_COUNT  ((int)(sizeof(AppList)/sizeof(AppList[0])))

// ----------------------------------------------------
// Items: everything clickable on the taskbar
// ----------------------------------------------------

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

#define TB_ITEM_MAX      16
#define ID_START_BUTTON  1     // icon ids are ID_ICON_FIRST + index
#define ID_ICON_FIRST    2

struct tb_item_d
{
    int used;
    struct button_info_d info;
    struct ui_component_d *uic;
    const char *program;       // program image launched by this item
};
static struct tb_item_d  ItemList[TB_ITEM_MAX];
static int ItemCount = 0;

// The clock
static struct ui_component_d *uic_clock = NULL;
static unsigned long clock_last_secs = (unsigned long) -1;

// Pointer state
static int __hover_icon_id   = -1;
static int __pressed_icon_id = -1;

// Position of the pointer at the time of the event.
// Filled by the input loop from the event buffer.
static unsigned long last_x = 0;
static unsigned long last_y = 0;

// Launch cooldown
static unsigned long last_launch_jiffie = 0;
static int           launched_once = FALSE;

//
// == Private functions: prototypes ====================
//
static void __initialize_client_list(void);

static void tb_fill_rect(
    struct dccanvas_d *dc,
    long x, long y, long w, long h,
    unsigned int color );

static struct tb_item_d *item_by_id(int id);
static struct tb_item_d *item_register(
    int id, 
    struct ui_component_d *uic,
    unsigned long left, unsigned long top,
    unsigned long width, unsigned long height,
    const char *program );

static int do_launch_program(const char *program);
static int do_launch_icon(int index);

static void on_button_pressed(int button_id);
static void on_button_released(int button_id);
static void on_button_clicked(int button_id);

static int  __hit_test_icon(unsigned long rel_mx, unsigned long rel_my);

static void clock_format(char *out, unsigned long secs);
static void clock_update(unsigned long jiffies, int force);
static struct ui_component_d *create_clock_label(const char *label);

static void create_start_button(void);
static struct ui_component_d *create_icon_area_icon(
    int index,
    const char *label,
    unsigned long flags );

static void draw_taskbar_areas(void);
static void update_clients(int fd);

static int 
tbProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 );

// ====================================================
// Drawing helper
// ====================================================

// Fill a rectangle, clipped against the dc. ROP_COPY only.
// Much cheaper than lingui_draw_rectangle0_dc(), which goes
// through the ROP chain for every single pixel.
static void 
tb_fill_rect(
    struct dccanvas_d *dc,
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

    if ((void*) dc == NULL || (void*) dc->data == NULL)
        return;
    if (w <= 0 || h <= 0)
        return;
    if (dc->bpp != 32 && dc->bpp != 24)
        return;

    dw = (long) dc->device_width;
    dh = (long) dc->device_height;

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
        row = (unsigned char *) dc->data + ((unsigned long) yy * dc->pitch);

        if (dc->bpp == 32)
        {
            unsigned int *p = (unsigned int *) (row + ((unsigned long) x << 2));
            for (xx = x; xx < x2; xx++)
                *p++ = c;
        }
        else
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

// ====================================================
// Items
// ====================================================

static struct tb_item_d *item_by_id(int id)
{
    int i;

    if (id < 0)
        return NULL;

    for (i = 0; i < ItemCount; i++)
    {
        if (ItemList[i].used == TRUE && ItemList[i].info.button_id == id)
            return &ItemList[i];
    }
    return NULL;
}

static struct tb_item_d *item_register(
    int id, 
    struct ui_component_d *uic,
    unsigned long left, unsigned long top,
    unsigned long width, unsigned long height,
    const char *program )
{
    struct tb_item_d *it;

    if ((void*) uic == NULL)
        return NULL;
    if (ItemCount >= TB_ITEM_MAX)
        return NULL;

    it = &ItemList[ItemCount];
    memset(it, 0, sizeof(struct tb_item_d));

    it->used    = TRUE;
    it->uic     = uic;
    it->program = program;

    it->info.button_id = id;
    it->info.wid = -1;
    // The taskbar client area starts at the window origin,
    // so relative and absolute offsets inside the window are the same.
    it->info.left   = left;
    it->info.top    = top;
    it->info.width  = width;
    it->info.height = height;
    it->info.absolute_left = TaskbarInfo.left + left;
    it->info.absolute_top  = TaskbarInfo.top  + top;
    it->info.state  = 0;

    ItemCount++;
    return it;
}

// Hit test against every registered item.
// Uses the relative values.
static int __hit_test_icon(unsigned long rel_mx, unsigned long rel_my) 
{
    int i;
    struct tb_item_d *it;

    for (i = 0; i < ItemCount; i++)
    {
        it = &ItemList[i];
        if (it->used != TRUE)
            continue;

        if ( rel_mx >= it->info.left && 
             rel_mx <  it->info.left + it->info.width &&
             rel_my >= it->info.top  && 
             rel_my <  it->info.top  + it->info.height )
        {
            return (int) it->info.button_id;
        }
    }

    return (int) -1;
}

// ====================================================
// Launching
// ====================================================

// Launch a program image. Returns the TID, or -1 on failure
// (or when the call was ignored by the cooldown).
static int do_launch_program(const char *program)
{
    int ret_val = -1;
    char filename[32];
    unsigned long now;

    if ((void*) program == NULL)
        return -1;
    if (program[0] == 0)
        return -1;
    if (strlen(program) >= sizeof(filename))
        return -1;

    // Cooldown: ignore a second launch that comes too soon.
    now = (unsigned long) rtl_jiffies();
    if (launched_once == TRUE)
    {
        if ((now - last_launch_jiffie) < TB_LAUNCH_COOLDOWN_MS)
            return -1;
    }

    memset(filename, 0, sizeof(filename));
    strncpy(filename, program, sizeof(filename) - 1);

    last_launch_jiffie = now;
    launched_once = TRUE;

    ret_val = (int) rtl_clone_and_execute_return_tid(filename);
    if (ret_val <= 0){
        TB_LOG("taskbar: Couldn't clone %s\n", filename);
        return -1;
    }

    return (int) ret_val;  // TID
}

// index: 0-based index in AppList.
static int do_launch_icon(int index)
{
    if (index < 0 || index >= APP_COUNT)
        return -1;
    return do_launch_program(AppList[index].name);
}

// ====================================================
// Button handlers
// ====================================================

// For visual effects
// #todo: We need to implement this kind of worker inside the library
static void on_button_pressed(int button_id)
{
    struct tb_item_d *it = item_by_id(button_id);
    if (it == NULL)
        return;
    if (it->uic->magic != UIC_MAGIC)
        return;
    libgui_on_button_pressed(it->uic, dc_taskbar);
}

// For visual effects
// #todo: We need to implement this kind of worker inside the library
static void on_button_released(int button_id)
{
    struct tb_item_d *it = item_by_id(button_id);
    if (it == NULL)
        return;
    if (it->uic->magic != UIC_MAGIC)
        return;
    libgui_on_button_released(it->uic, dc_taskbar);
}

// Handle click events for components
static void on_button_clicked(int button_id)
{
    struct tb_item_d *it = item_by_id(button_id);
    if (it == NULL)
        return;

    // Each item knows what it launches.
    do_launch_program(it->program);
}

// ====================================================
// Clock
// ====================================================

// "HH:MM:SS" (9 bytes with the terminator).
// #todo: Replace the source by a real time clock when we have one.
// For now it shows the time since boot.
static void clock_format(char *out, unsigned long secs)
{
    unsigned long hh = (secs / 3600) % 100;
    unsigned long mm = (secs / 60) % 60;
    unsigned long ss = secs % 60;

    out[0] = (char)('0' + (hh / 10));
    out[1] = (char)('0' + (hh % 10));
    out[2] = ':';
    out[3] = (char)('0' + (mm / 10));
    out[4] = (char)('0' + (mm % 10));
    out[5] = ':';
    out[6] = (char)('0' + (ss / 10));
    out[7] = (char)('0' + (ss % 10));
    out[8] = '\0';
}

// Update the label and redraw only the clock.
// Does nothing if the second did not change (unless forced).
static void clock_update(unsigned long jiffies, int force)
{
    unsigned long secs = jiffies / TB_JIFFIES_PER_SECOND;
    char text[9];

    if ((void*) uic_clock == NULL || (void*) dc_taskbar == NULL)
        return;
    if (uic_clock->magic != UIC_MAGIC)
        return;
    if (force != TRUE && secs == clock_last_secs)
        return;
    if (sizeof(uic_clock->label) < sizeof(text))
        return;

    clock_last_secs = secs;

    clock_format(text, secs);
    memcpy(uic_clock->label, text, sizeof(text));
    uic_clock->label_size = 8;

    libgui_redraw_ui_component(uic_clock, dc_taskbar);
}

// Creates the clock as a UI_COMPONENT_LABEL inside the notification area.
// Returns the component pointer or NULL on failure.
static struct ui_component_d *create_clock_label(const char *label)
{
    unsigned long na_l = TaskbarInfo.notification_area.left;
    unsigned long na_t = TaskbarInfo.notification_area.top;
    unsigned long na_w = TaskbarInfo.notification_area.width;
    unsigned long na_h = TaskbarInfo.notification_area.height;
    unsigned long clock_left;
    unsigned long clock_top;
    unsigned long clock_height;

    if ((void*)dc_taskbar == NULL)
        return NULL;

    // The area must be big enough. Avoid unsigned underflow.
    if (na_w < (CLOCK_WIDTH + CLOCK_MARGIN))
        return NULL;
    if (na_h <= 4)
        return NULL;

    clock_left   = na_l + na_w - CLOCK_WIDTH - CLOCK_MARGIN;
    clock_top    = na_t + 2;
    clock_height = na_h - 4;

    return (struct ui_component_d *) libgui_create_ui_component(
        dc_taskbar,
        UI_COMPONENT_LABEL,
        clock_left, clock_top, 
        CLOCK_WIDTH, clock_height,
        label,
        TB_FLAGS_CENTERED
    );
}

// ====================================================
// Taskbar painting
// ====================================================

static void draw_taskbar_areas(void)
{
    if ((void*) dc_taskbar == NULL)
        return;

    tb_fill_rect(dc_taskbar,
        TaskbarInfo.launch_area.left, TaskbarInfo.launch_area.top,
        TaskbarInfo.launch_area.width, TaskbarInfo.launch_area.height,
        THEME_TASKBAR_BG);

    tb_fill_rect(dc_taskbar,
        TaskbarInfo.icon_area.left, TaskbarInfo.icon_area.top,
        TaskbarInfo.icon_area.width, TaskbarInfo.icon_area.height,
        THEME_TASKBAR_BG);

    tb_fill_rect(dc_taskbar,
        TaskbarInfo.notification_area.left, TaskbarInfo.notification_area.top,
        TaskbarInfo.notification_area.width, TaskbarInfo.notification_area.height,
        THEME_TASKBAR_BG);
}

// Full repaint of the taskbar.
// The window info is cached, so no request goes to the server here.
static void update_clients(int fd)
{
    int i;

    if (fd < 0)
        return;
    if ((void*)dc_taskbar == NULL)
        return;

    // Background for the client area
    tb_fill_rect(
        dc_taskbar,
        0, 0, tb_cr_w, tb_cr_h,
        THEME_TASKBAR_CLIENT_BG );

    draw_taskbar_areas();

    // Start button and app icons
    for (i = 0; i < ItemCount; i++)
    {
        if (ItemList[i].used != TRUE)
            continue;
        if (ItemList[i].uic->magic == UIC_MAGIC)
            libgui_redraw_ui_component(ItemList[i].uic, dc_taskbar);
    }

    // The clock
    if ((void*) uic_clock != NULL && uic_clock->magic == UIC_MAGIC)
        libgui_redraw_ui_component(uic_clock, dc_taskbar);

    // #todo: Redraw the running clients (clientList).
}

// ====================================================
// The procedure
// ====================================================
static int 
tbProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    int IconId = -1;

// Parameters:
    if (fd < 0){
        goto fail;
    }
    if (event_type <= 0){
        goto fail;
    }

// Process the event
    switch (event_type){

        // Sent by the server or the shared flag is set.
        case MSG_PAINT:
            update_clients(fd);
            return 0;

        case MSG_MOUSEMOVE:
            // long1/long2: position relative to the taskbar.
            __hover_icon_id = (int) __hit_test_icon(long1, long2);
            return 0;

        case MSG_MOUSEPRESSED:
            // The pointer position is in last_x/last_y
            // (saved by the input loop from RTLEventBuffer[4]/[5]).
            IconId = (int) __hit_test_icon(last_x, last_y);
            if (IconId < 0)
                IconId = __hover_icon_id;

            __pressed_icon_id = IconId;
            if (IconId >= 0)
                on_button_pressed(IconId);  // Visual effect
            return 0;

        case MSG_MOUSERELEASED:
            if (__pressed_icon_id >= 0)
            {
                on_button_released(__pressed_icon_id);  // Visual effect

                // A click only counts if the pointer is still
                // over the button that was pressed.
                IconId = (int) __hit_test_icon(last_x, last_y);
                if (IconId < 0)
                    IconId = __hover_icon_id;

                if (IconId == __pressed_icon_id)
                    on_button_clicked(IconId);

                __pressed_icon_id = -1;
            }
            return 0;

        // #suspended: This message was sent by the server
        case GWS_MouseClicked:
            return 0;

        case MSG_KEYDOWN:
            switch (long1) 
            {
                case '<':
                case ',':
                    gws_dock_active_window(fd, 1);   // margin 1 = left
                    break;

                case '>':
                case '.':
                    gws_dock_active_window(fd, 2);   // margin 2 = right
                    break;

                default:
                    break;
            };
            return 0;

        case MSG_KEYUP:
            switch (long1) 
            {
                case 'S':  // Start
                case 's': 
                    do_launch_program(StartProgram);
                    break;

                // '1'..'9' launch the programs of the icon area
                case '1': do_launch_icon(0); break;
                case '2': do_launch_icon(1); break;
                case '3': do_launch_icon(2); break;
                case '4': do_launch_icon(3); break;
                case '5': do_launch_icon(4); break;
                case '6': do_launch_icon(5); break;
                case '7': do_launch_icon(6); break;
                case '8': do_launch_icon(7); break;
                case '9': do_launch_icon(8); break;

                default:
                    break;
            };
            return 0;

        case MSG_SYSKEYDOWN:
            switch (long1){
                case VK_F5: 
                    // gws_update_desktop(fd);
                    break;

                case VK_ARROW_RIGHT:
                case VK_ARROW_UP:
                case VK_ARROW_DOWN:
                case VK_ARROW_LEFT:
                    // #todo: Move focus between the taskbar items.
                    break;
                default:
                    break;
            };
            return 0;

        // Add new client. Given the wid.
        case 99440:
            TB_LOG("taskbar: [99440]\n");
            return 0;

        // Remove client. Given the wid.
        case 99441:
            TB_LOG("taskbar: [99441]\n");
            return 0;
        
        // Update client info.
        case 99443:
            TB_LOG("taskbar: [99443]\n");
            return 0;

        // #test:
        // ds sent us a message to create an iconic window for an app.
        case 99500:
            return 0;

        case MSG_COMMAND:
            return 0;

        // The taskbar application is the Shell, just like the explorer.exe
        // in Windows. We can't simply close it.
        // #todo: It only can happen in certain situations.
        case MSG_CLOSE:
            TB_LOG("taskbar: MSG_CLOSE ignored\n");
            // isTimeToQuit = TRUE;
            return 0;

        default:
            goto fail;
    };

    return 0;
fail:
    return (int) (-1);
}

static void __initialize_client_list(void)
{
    register int i=0;
    for (i=0; i<CLIENT_COUNT_MAX; i++)
    {
        clientList[i].used = FALSE;
        clientList[i].magic = 0;
        clientList[i].client_id = -1;
        
        clientList[i].icon_info.wid = -1;
        clientList[i].icon_info.icon_id = -1;
        clientList[i].icon_info.state = 0;

        // Icon info
        clientList[i].icon_info.left = 0;
        clientList[i].icon_info.top = 0;
        clientList[i].icon_info.width = 0;
        clientList[i].icon_info.height = 0;
    };
}

// ====================================================
// Creation of the components
// ====================================================

static void create_start_button(void)
{
    struct ui_component_d *uic;
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    if (TaskbarInfo.launch_area.initialized != TRUE)
        return;
    if ((void*) dc_taskbar == NULL)
        return;
    if (TaskbarInfo.launch_area.width <= 2 || 
        TaskbarInfo.launch_area.height <= 2)
    {
        return;
    }

    left   = TaskbarInfo.launch_area.left + 1;
    top    = TaskbarInfo.launch_area.top  + 1;
    width  = (TaskbarInfo.launch_area.width - 2) >> 1;
    height = TaskbarInfo.launch_area.height - 2;

    // Create UI component (client-side only)
    uic = libgui_create_ui_component(
        dc_taskbar,
        UI_COMPONENT_BUTTON,
        left, top, width, height,
        "Start", 
        TB_FLAGS_CENTERED
    );
    if ((void*) uic == NULL)
        return;

    item_register(ID_START_BUTTON, uic, left, top, width, height, StartProgram);
}

// Creates a squared icon inside the icon area (middle section of the taskbar).
// Automatically finds the next free horizontal slot.
// index: 0-based index in AppList.
// Returns the new ui_component pointer, or NULL on failure.
static struct ui_component_d *create_icon_area_icon(
    int index,
    const char *label,           // 1 or 2 chars
    unsigned long flags )
{
    struct ui_component_d *icon;
    unsigned long IconSize;
    unsigned long icon_left;
    unsigned long icon_top;
    int count;

    if ((void*)dc_taskbar == NULL)
        return NULL;
    if (index < 0 || index >= APP_COUNT)
        return NULL;
    if (TaskbarInfo.icon_area.height <= 2)
        return NULL;

    IconSize = TaskbarInfo.icon_area.height - 2;

    // Current number of icons already in the area
    count = TaskbarInfo.icon_area.component_count;

    // Calculate next free X position
    icon_left = TaskbarInfo.icon_area.left 
              + __LEFT_MARGIN 
              + count * (IconSize + __ICON_GAP);

    // Vertical centering inside the icon area
    icon_top = TaskbarInfo.icon_area.top 
             + (TaskbarInfo.icon_area.height - IconSize) / 2;

    // Safety: do not overflow the icon area
    if (icon_left + IconSize > 
        TaskbarInfo.icon_area.left + TaskbarInfo.icon_area.width)
    {
        return NULL;
    }

    icon = libgui_create_ui_component(
        dc_taskbar,
        UI_COMPONENT_ICON,
        icon_left, icon_top, IconSize, IconSize,
        label ? label : "",
        flags
    );
    if ((void*)icon == NULL)
        return NULL;

    if (item_register( 
            ID_ICON_FIRST + index, icon, 
            icon_left, icon_top, IconSize, IconSize, 
            AppList[index].name ) == NULL)
    {
        return NULL;
    }

    TaskbarInfo.icon_area.component_count++;

    return icon;
}

// ==========================================
// Main

int main(int argc, char *argv[])
{
    const char *display_name_string = "display:name.0";

    int client_fd = -1;
    int status = -1;
    int i;

// --------------------------------
// Initialize the library
    status = (int) libgui_initialize();
    if (status < 0){
        printf("taskbar: libgui_initialize fail\n");
        exit(1);
    }

    ItemCount = 0;
    memset(ItemList, 0, sizeof(ItemList));
    uic_clock = NULL;
    clock_last_secs = (unsigned long) -1;
    __hover_icon_id = -1;
    __pressed_icon_id = -1;
    launched_once = FALSE;

// ============================
// Open display
// IN: hostname:number.screen_number
    Display = (struct gws_display_d *) gws_open_display(display_name_string);
    if ((void*) Display == NULL){
        printf("taskbar.bin: Display\n");
        exit(1);
    }
// Get client socket.
    client_fd = (int) Display->fd;
    if (client_fd <= 0){
        printf("taskbar.bin: fd\n");
        exit(1);
    }

//========================================
// Device info

    unsigned long w = gws_get_system_metrics(1);
    unsigned long h = gws_get_system_metrics(2);
    if ( w == 0 || h == 0 ){
        printf ("taskbar.bin: w h\n");
        exit(1);
    }
    if (h <= TASKBAR_HEIGHT){
        printf ("taskbar.bin: screen too small\n");
        exit(1);
    }
    // Saving display properties
    DeviceInfo.width  = (unsigned long) (w & 0xFFFF);
    DeviceInfo.height = (unsigned long) (h & 0xFFFF);

//
// == Taskbar window =====================================
//

    // #important:
    // Taskbar position and dimensions.
    // #ps: The server only knows how to handle these values
    // for the taskbar at the bottom. Otherwise it will crash.
    // The working area is created by the server. It is the desktop area,
    // less the taskbar area.
    unsigned long tb_l = 0;
    unsigned long tb_t = h - TASKBAR_HEIGHT;
    unsigned long tb_w = w;
    unsigned long tb_h = TASKBAR_HEIGHT;

    TaskbarInfo.left   = tb_l;
    TaskbarInfo.top    = tb_t;
    TaskbarInfo.width  = tb_w;
    TaskbarInfo.height = tb_h;

    unsigned long style = WS_APP | WS_TASKBAR;

    taskbar_window = 
        (int) gws_create_window (
                client_fd,
                WT_SIMPLE,             // type
                WINDOW_STATUS_ACTIVE,  // status
                WINDOW_STATE_NULL,     // state
                program_name,
                tb_l, tb_t, tb_w, tb_h,
                0, 
                style,
                THEME_TASKBAR_BG, THEME_TASKBAR_BG );

    if (taskbar_window < 0){
        printf ("taskbar: taskbar_window\n");
        exit(1);
    }

// ====================================
// Get the window info and the dc BEFORE we announce ourselves
// as the system shell. This way, if something fails here, we
// exit without leaving the system pointing to a broken shell.

    struct gws_window_info_d wi;
    gws_get_window_info(
        client_fd,
        taskbar_window,
        (struct gws_window_info_d *) &wi );

// Cache the client area size. We do not ask the server again.
    tb_cr_w = wi.cr_width;
    tb_cr_h = wi.cr_height;
    if (tb_cr_w == 0 || tb_cr_h == 0){
        printf("taskbar: client area\n");
        exit(1);
    }

// Getting the flag earlier. This way we can use it in the loop.
    __sh_flags = (unsigned long) wi.sh_flags;

    dc_taskbar = (struct dccanvas_d *) libgui_create_dc(
        wi.ca_canvas_base_address,
        wi.ca_canvas_width,
        wi.ca_canvas_height,
        wi.ca_canvas_bpp
    );
    if ((void*)dc_taskbar == NULL)
    {
        printf("taskbar: on dc_taskbar\n");
        exit(1);
    }

    gws_set_active( client_fd, taskbar_window );

// =============================
// Update the wproxy structure that belongs to this thread.

    unsigned long m[10];
    int mytid = gettid();

    m[0] = (unsigned long) (mytid & 0xFFFFFFFF);

    // Frame/chrome rectangle
    m[1] = TaskbarInfo.left;
    m[2] = TaskbarInfo.top;
    m[3] = TaskbarInfo.width;
    m[4] = TaskbarInfo.height;

    // #ps: 
    // In the case of taskbar, frame and client are equal.
    // It means the offset for the client area is 0.

    // Client area rectangle
    m[5] = 0;
    m[6] = 0;
    m[7] = TaskbarInfo.width;
    m[8] = TaskbarInfo.height;
    m[9] = 0;

    sc80( 48, &m[0], &m[0], &m[0] );

// =============================
// Set this thread and its window as the system shell.
// The taskbar.

    sc80(46, 0, 0, 0);

// Draw background for the client area
    tb_fill_rect (
        dc_taskbar,
        0, 0, tb_cr_w, tb_cr_h,
        THEME_TASKBAR_CLIENT_BG );

// #test:
// Setup kernel console.
// Set cursor position on top of the raw window.
    sc80 ( 34, 2, 2, 0 );

    __initialize_client_list();

// ===============================
// Launch area (Start button)

    TaskbarInfo.launch_area.initialized = FALSE;

    TaskbarInfo.launch_area.left   = 0;
    TaskbarInfo.launch_area.top    = 2;
    TaskbarInfo.launch_area.width  = tb_cr_w/4;
    TaskbarInfo.launch_area.height = tb_cr_h - 4;
    TaskbarInfo.launch_area.component_count = 0;

    TaskbarInfo.launch_area.initialized = TRUE;

    create_start_button();

// ===============================
// Notification area (clock)

    TaskbarInfo.notification_area.initialized = FALSE;

    TaskbarInfo.notification_area.left   = (tb_cr_w - (tb_cr_w/4));
    TaskbarInfo.notification_area.top    = 2;
    TaskbarInfo.notification_area.width  = (tb_cr_w/4);
    TaskbarInfo.notification_area.height = tb_cr_h - 4;
    TaskbarInfo.notification_area.component_count = 0;

    TaskbarInfo.notification_area.initialized = TRUE;

// ===============================
// Icon area

    TaskbarInfo.icon_area.initialized = FALSE;

    TaskbarInfo.icon_area.left   = (tb_cr_w/4);
    TaskbarInfo.icon_area.top    = 2;
    TaskbarInfo.icon_area.width  = (
        tb_cr_w - 
        TaskbarInfo.launch_area.width - 
        TaskbarInfo.notification_area.width
    );
    TaskbarInfo.icon_area.height = tb_cr_h - 4;
    TaskbarInfo.icon_area.component_count = 0;

    TaskbarInfo.icon_area.initialized = TRUE;

// One icon for each program in AppList
    for (i = 0; i < APP_COUNT; i++)
    {
        create_icon_area_icon(
            i, 
            AppList[i].icon_label, 
            TB_FLAGS_CENTERED );
    }

// The clock
    uic_clock = (struct ui_component_d *) create_clock_label("00:00:00");

// First full paint (areas, items, clock).
    update_clients(client_fd);
    clock_update( (unsigned long) rtl_jiffies(), TRUE );

// =======================
// Event loop
// Getting input events from the system.

    unsigned long MainLoopIntervalMS;
    if (CONFIG_MAIN_LOOP_INTERVAL_MS == 0){
        MainLoopIntervalMS = 16;
    }else{
        MainLoopIntervalMS = CONFIG_MAIN_LOOP_INTERVAL_MS;
    };

    unsigned long start_jiffie=0;
    unsigned long end_jiffie=0;
    unsigned long delta_jiffie=0;
    unsigned long next_clock_jiffie=0;

// #ps: We yield if a round was less than MainLoopIntervalMS.
// It gives the opportunity for other threads to run a bit more.
// #bugbug: Valid only if the timer fires 1000 times a second.

    isTimeToQuit = FALSE;

    while (1){
        if (isTimeToQuit == TRUE)
            break;

        start_jiffie = (unsigned long) rtl_jiffies();

        // Get value inside the shared area
        if (__sh_flags != 0)
        {
            char *flags_ptr = (char *) __sh_flags;
            if (*flags_ptr & SH_FLAG_BLIT)
            {
                // Clear BLIT bit
                *flags_ptr &= ~SH_FLAG_BLIT;
                // Redraw
                update_clients(client_fd);
            }
        }

        // Pump events from Input Broker (system events)
        // #bugbug:
        // The display-server pump was very slow and affected the
        // responsivity of this one, so it is not used here.
        if (rtl_get_event() == TRUE)
        {
            last_x = (unsigned long) RTLEventBuffer[4];
            last_y = (unsigned long) RTLEventBuffer[5];

            tbProcedure(
                client_fd,
                (int) RTLEventBuffer[0],   // window id
                (int) RTLEventBuffer[1],   // event type
                (unsigned long) RTLEventBuffer[2], // VK code / x
                (unsigned long) RTLEventBuffer[3]  // scancode / y
                );
            RTLEventBuffer[1] = 0; // clear after dispatch
        };

        // Clock tick: one compare per round, no message needed.
        // The work is done by clock_update(), which only repaints
        // when the second changes.
        if (start_jiffie >= next_clock_jiffie)
        {
            next_clock_jiffie = start_jiffie + TB_CLOCK_INTERVAL_MS;
            clock_update(start_jiffie, FALSE);
        }

        // Always yield when the round was short, including the case
        // where it took less than one jiffie (end == start). Otherwise
        // the loop would spin at 100% in that case.
        end_jiffie = (unsigned long) rtl_jiffies();
        delta_jiffie = (end_jiffie >= start_jiffie) ? (end_jiffie - start_jiffie) : 0;
        if (delta_jiffie < MainLoopIntervalMS)
        {
            // #suspended:
            // We are still implementing the sleep() support.
            // rtl_sleep(MainLoopIntervalMS - delta_jiffie);
            rtl_yield();
        }
    };

    if (isTimeToQuit == TRUE)
    {
        gws_destroy_window(client_fd, taskbar_window);
    }

    // Close the socket
    if (client_fd > 0)
        close(client_fd);
 
    return EXIT_SUCCESS;  // OK
}

//
// End
//

