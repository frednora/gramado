// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/utsname.h>
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

#include "paint.h"
#include "studio.h"

// =========================================================
// Application state
// =========================================================

static int isTimeToQuit = FALSE;

struct gws_display_d *Display = NULL;
struct dccanvas_d    *dc00    = NULL;

static int main_window = -1;

// Cached client area
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;

static unsigned long frame_left = 0;
static unsigned long frame_top  = 0;

static unsigned long __sh_flags = 0;

// Current page
static int CurrentPage = PAGE_SCOPE;

// Pointer
static unsigned long last_x = 0;
static unsigned long last_y = 0;

// =========================================================
// Event log
// =========================================================

#define EVENT_LOG_LINES 32
#define EVENT_LOG_TEXT  64

static char EventLog[EVENT_LOG_LINES][EVENT_LOG_TEXT];
static int  EventLogIndex = 0;

// =========================================================
// Toolbar
// =========================================================

static struct button_info_d   Btn[BTN_COUNT];
static struct ui_component_d *Uic[BTN_COUNT];

static const char *BtnLabel[BTN_COUNT] =
{
    "System",
    "Memory",
    "Events",
    "Scope",
    "Inspect",
    "Exit"
};

// =========================================================
// Content rectangle
// =========================================================

static unsigned long ContentLeft   = 0;
static unsigned long ContentTop    = 0;
static unsigned long ContentWidth  = 0;
static unsigned long ContentHeight = 0;

// =========================================================
// Scope statistics / history
// =========================================================

static unsigned long ScopeMouseEvents  = 0;
static unsigned long ScopeKeyEvents    = 0;
static unsigned long ScopePaintEvents  = 0;
static unsigned long ScopeSystemEvents = 0;
static unsigned long ScopeTotalEvents  = 0;

static unsigned char ScopeHistory[SCOPE_HISTORY];
static int           ScopePosition = 0;

// =========================================================
// Selected / hover state
// =========================================================

static int __hover_button_id   = -1;
static int __pressed_button_id = -1;

// =========================================================
// Themes
// =========================================================

static struct theme_d Themes[SWATCH_COUNT] =
{
    {
        COLOR_LIGHTGRAY,
        COLOR_STEELBLUE,
        COLOR_WHITE,
        COLOR_BLUE
    },
    {
        0x00305080,
        0x00204060,
        COLOR_WHITE,
        0x00A0D0FF
    },
    {
        0x00306030,
        0x00204020,
        COLOR_WHITE,
        0x0000FF00
    },
    {
        0x00604000,
        0x00402000,
        COLOR_WHITE,
        0x00FFC000
    },
    {
        0x00502070,
        0x00301050,
        COLOR_WHITE,
        0x00D080FF
    },
    {
        0x00802020,
        0x00401010,
        COLOR_WHITE,
        0x00FF4040
    }
};

struct swatch_info_d
{
    int id;
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;
};

static struct swatch_info_d Swatch[SWATCH_COUNT];

#define SWATCH_SIZE  24
#define SWATCH_GAP   6

static int CurrentTheme = 0;

// =========================================================
// Inspector page data
// =========================================================

static unsigned long InspectorMouseX = 0;
static unsigned long InspectorMouseY = 0;
static int           InspectorWindow = -1;

// =========================================================
// Forward declarations
// =========================================================

static void draw_toolbar(void);
static void draw_status(void);

static void draw_system_page(void);
static void draw_memory_page(void);
static void draw_events_page(void);
static void draw_scope_page(void);
static void draw_inspector_page(void);

static void draw_theme_swatches(void);

static void update_toolbar_layout(void);
static void query_client_area(int fd);
static void update_children(int fd);

static void eventlog_push(const char *text);
static void scope_push_sample(unsigned char value);
static void select_theme(int index);
static void update_swatch_layout(void);
static int  hit_test_swatch(unsigned long mx, unsigned long my);
static void theme_swatch_clicked(int fd, int id);
static int  hit_test_toolbar(unsigned long mx, unsigned long my);

static void create_toolbar(void);
static void toolbar_button_pressed(int id);
static void toolbar_button_released(int id);
static void toolbar_button_clicked(int fd, int id);

static int  studioProcedure(
    int fd,
    int event_window,
    int event_type,
    unsigned long long1,
    unsigned long long2);

// =========================================================
// Event log helpers
// =========================================================

static void
eventlog_push(const char *text)
{
    int slot;

    if (text == NULL)
        return;

    slot = EventLogIndex % EVENT_LOG_LINES;

    strncpy(EventLog[slot], text, EVENT_LOG_TEXT - 1);
    EventLog[slot][EVENT_LOG_TEXT - 1] = 0;

    EventLogIndex++;
}

// =========================================================
// Page drawing
// =========================================================

/*
static void
draw_events_page(void)
{
    int i;
    int index;
    unsigned long x;
    unsigned long y;
    char line[96];

    if (dc00 == NULL)
        return;

    x = ContentLeft;
    y = ContentTop;

    libgui_drawstringblock_dc(
        dc00,
        x, y,
        COLOR_BLACK,
        "Event Log",
        2);

    y += 40;

    // Background
    lingui_draw_rectangle0_dc(
        dc00,
        ContentLeft,
        y,
        ContentWidth,
        ContentHeight - 40,
        COLOR_WHITE,
        0);

    // Newest first
    for (i = 0; i < EVENT_LOG_LINES; i++)
    {
        index = EventLogIndex - 1 - i;

        if (index < 0)
            break;

        index = index % EVENT_LOG_LINES;

        sprintf(line, "[%04d] %s", EventLogIndex - i, EventLog[index]);

        libgui_drawstring_dc(
            dc00,
            x + 8,
            y + (i * 16),
            COLOR_BLACK,
            COLOR_WHITE,
            0,
            line);
    }
}
*/

static void
draw_events_page(void)
{
    int i;
    int index;
    int max_lines;
    unsigned long x;
    unsigned long y;
    unsigned long bg_height;
    char line[96];

    if (dc00 == NULL)
        return;

    x = ContentLeft;
    y = ContentTop;

    libgui_drawstringblock_dc(
        dc00,
        x, y,
        COLOR_BLACK,
        "Event Log",
        2);

    y += 40;

    // Protect against underflow
    if (ContentHeight > 40)
        bg_height = ContentHeight - 40;
    else
        bg_height = 0;

    // Background
    if (bg_height > 0 && ContentWidth > 0)
    {
        lingui_draw_rectangle0_dc(
            dc00,
            ContentLeft,
            y,
            ContentWidth,
            bg_height,
            COLOR_WHITE,
            0);
    }

    // How many lines can actually fit
    max_lines = (int)(bg_height / 16);
    if (max_lines > EVENT_LOG_LINES)
        max_lines = EVENT_LOG_LINES;
    if (max_lines < 0)
        max_lines = 0;

    // Newest first
    for (i = 0; i < max_lines; i++)
    {
        index = EventLogIndex - 1 - i;

        if (index < 0)
            break;

        index = index % EVENT_LOG_LINES;

        // Safety: never read out of bounds
        if (index < 0 || index >= EVENT_LOG_LINES)
            continue;

        sprintf(line, "[%d] %s", EventLogIndex - i, EventLog[index]);

        libgui_drawstring_dc(
            dc00,
            x + 8,
            y + (i * 16),
            COLOR_BLACK,
            COLOR_WHITE,
            0,
            line);
    }
}

static void
draw_memory_page(void)
{
    unsigned long total;
    unsigned long used;
    unsigned long free_;
    char tmp[128];
    unsigned long bar_w;

    if (dc00 == NULL)
        return;

    total = gws_get_system_metrics(33);
    used  = gws_get_system_metrics(34);
    free_ = gws_get_system_metrics(35);

    libgui_drawstringblock_dc(
        dc00,
        ContentLeft,
        ContentTop,
        COLOR_BLACK,
        "Memory",
        2);

    sprintf(tmp, "Total: %lu KB", total);
    libgui_drawstring_dc(
        dc00,
        ContentLeft, ContentTop + 40,
        COLOR_BLACK, COLOR_WHITE, 0, tmp);

    sprintf(tmp, "Used: %lu KB", used);
    libgui_drawstring_dc(
        dc00,
        ContentLeft, ContentTop + 60,
        COLOR_BLACK, COLOR_WHITE, 0, tmp);

    sprintf(tmp, "Free: %lu KB", free_);
    libgui_drawstring_dc(
        dc00,
        ContentLeft, ContentTop + 80,
        COLOR_BLACK, COLOR_WHITE, 0, tmp);

    // Usage bar
    if (total != 0)
    {
        bar_w = (used * 300) / total;

        lingui_draw_rectangle0_dc(
            dc00,
            ContentLeft, ContentTop + 120,
            300, 24,
            COLOR_LIGHTGRAY, 0);

        lingui_draw_rectangle0_dc(
            dc00,
            ContentLeft, ContentTop + 120,
            bar_w, 24,
            COLOR_GREEN, 0);
    }
}

static void
draw_system_page(void)
{
    struct utsname un;
    char tmp[256];
    int y;

    if (dc00 == NULL)
        return;

    uname(&un);

    y = ContentTop;

    libgui_drawstringblock_dc(
        dc00,
        ContentLeft, y,
        COLOR_BLACK,
        "System Information",
        2);

    y += 40;

    sprintf(tmp, "System: %s", un.sysname);
    libgui_drawstring_dc(dc00, ContentLeft, y, COLOR_BLACK, COLOR_WHITE, 0, tmp);
    y += 16;

    sprintf(tmp, "Release: %s", un.release);
    libgui_drawstring_dc(dc00, ContentLeft, y, COLOR_BLACK, COLOR_WHITE, 0, tmp);
    y += 16;

    sprintf(tmp, "Version: %s", un.version);
    libgui_drawstring_dc(dc00, ContentLeft, y, COLOR_BLACK, COLOR_WHITE, 0, tmp);
    y += 16;

    sprintf(tmp, "Machine: %s", un.machine);
    libgui_drawstring_dc(dc00, ContentLeft, y, COLOR_BLACK, COLOR_WHITE, 0, tmp);
    y += 16;

    sprintf(tmp, "Node: %s", un.nodename);
    libgui_drawstring_dc(dc00, ContentLeft, y, COLOR_BLACK, COLOR_WHITE, 0, tmp);
}

static void
draw_inspector_page(void)
{
    char tmp[128];

    if (dc00 == NULL)
        return;

    libgui_drawstringblock_dc(
        dc00,
        ContentLeft, ContentTop,
        COLOR_BLACK,
        "Inspector",
        2);

    sprintf(tmp, "Mouse X: %lu", InspectorMouseX);
    libgui_drawstring_dc(
        dc00,
        ContentLeft, ContentTop + 50,
        COLOR_BLACK, COLOR_WHITE, 0, tmp);

    sprintf(tmp, "Mouse Y: %lu", InspectorMouseY);
    libgui_drawstring_dc(
        dc00,
        ContentLeft, ContentTop + 70,
        COLOR_BLACK, COLOR_WHITE, 0, tmp);

    sprintf(tmp, "Window: %d", InspectorWindow);
    libgui_drawstring_dc(
        dc00,
        ContentLeft, ContentTop + 90,
        COLOR_BLACK, COLOR_WHITE, 0, tmp);
}

static void
draw_scope_page(void)
{
    int x;
    int value;
    int h;
    unsigned long left = 16;
    unsigned long top  = 80;

    if (dc00 == NULL)
        return;

    for (x = 0; x < SCOPE_HISTORY; x++)
    {
        value = ScopeHistory[(ScopePosition + x) % SCOPE_HISTORY];
        h = value;

        if (h < 0)
            h = 0;
        if (h > 100)
            h = 100;

        lingui_draw_rectangle0_dc(
            dc00,
            left + x,
            top + (100 - h),
            1, h,
            COLOR_BLUE,
            0);
    }
}

// =========================================================
// Theme helpers
// =========================================================

static void
select_theme(int index)
{
    if (index < 0 || index >= SWATCH_COUNT)
        return;

    CurrentTheme = index;
}

static void
update_swatch_layout(void)
{
    int i;
    unsigned long y;

    y = cr_height - STATUS_HEIGHT - PALETTE_HEIGHT + 3;

    for (i = 0; i < SWATCH_COUNT; i++)
    {
        Swatch[i].id     = SWATCH_BASE + i;
        Swatch[i].left   = 8 + (i * (SWATCH_SIZE + SWATCH_GAP));
        Swatch[i].top    = y;
        Swatch[i].width  = SWATCH_SIZE;
        Swatch[i].height = SWATCH_SIZE;
    }
}

static void
draw_theme_swatches(void)
{
    int i;
    unsigned long y;
    unsigned int color;

    if (dc00 == NULL)
        return;

    y = cr_height - STATUS_HEIGHT - PALETTE_HEIGHT;

    // Palette background band
    lingui_draw_rectangle0_dc(
        dc00,
        0, y,
        cr_width, PALETTE_HEIGHT,
        COLOR_LIGHTGRAY, 0);

    for (i = 0; i < SWATCH_COUNT; i++)
    {
        color = Themes[i].accent_color;

        if (i == CurrentTheme)
        {
            // Outer black border
            lingui_draw_rectangle0_dc(
                dc00,
                Swatch[i].left, Swatch[i].top,
                Swatch[i].width, Swatch[i].height,
                COLOR_BLACK, 0);

            // Inner white border
            lingui_draw_rectangle0_dc(
                dc00,
                Swatch[i].left + 2, Swatch[i].top + 2,
                Swatch[i].width - 4, Swatch[i].height - 4,
                COLOR_WHITE, 0);

            // Color center
            lingui_draw_rectangle0_dc(
                dc00,
                Swatch[i].left + 4, Swatch[i].top + 4,
                Swatch[i].width - 8, Swatch[i].height - 8,
                color, 0);
        }
        else
        {
            // Border
            lingui_draw_rectangle0_dc(
                dc00,
                Swatch[i].left, Swatch[i].top,
                Swatch[i].width, Swatch[i].height,
                COLOR_DARKGRAY, 0);

            // Color block
            lingui_draw_rectangle0_dc(
                dc00,
                Swatch[i].left + 1, Swatch[i].top + 1,
                Swatch[i].width - 2, Swatch[i].height - 2,
                color, 0);
        }
    }
}

static int
hit_test_swatch(unsigned long mx, unsigned long my)
{
    int i;

    for (i = 0; i < SWATCH_COUNT; i++)
    {
        if (mx >= Swatch[i].left &&
            mx <  Swatch[i].left + Swatch[i].width &&
            my >= Swatch[i].top  &&
            my <  Swatch[i].top  + Swatch[i].height)
        {
            return Swatch[i].id;
        }
    }

    return -1;
}

static void
theme_swatch_clicked(int fd, int id)
{
    int index;

    index = id - SWATCH_BASE;

    if (index < 0 || index >= SWATCH_COUNT)
        return;

    select_theme(index);
    update_children(fd);
}

// =========================================================
// Scope helpers
// =========================================================

static void
scope_push_sample(unsigned char value)
{
    ScopeHistory[ScopePosition] = value;

    ScopePosition++;
    if (ScopePosition >= SCOPE_HISTORY)
        ScopePosition = 0;
}

// =========================================================
// Geometry / client area
// =========================================================

static void
query_client_area(int fd)
{
    struct gws_window_info_d wi;

    if (fd < 0 || main_window < 0)
        return;

    gws_get_window_info(fd, main_window, &wi);

    frame_left = wi.left;
    frame_top  = wi.top;

    cr_width  = wi.cr_width;
    cr_height = wi.cr_height;
}

// =========================================================
// Header / status / toolbar drawing
// =========================================================

static void
draw_status(void)
{
    char line[128];
    struct theme_d *t;

    if (dc00 == NULL)
        return;

    t = &Themes[CurrentTheme];

    lingui_draw_rectangle0_dc(
        dc00,
        0,
        cr_height - STATUS_HEIGHT,
        cr_width,
        STATUS_HEIGHT,
        t->footer_color,
        0);

    sprintf(line, "Page:%d  Events:%d", CurrentPage, EventLogIndex);

    libgui_drawstring_dc(
        dc00,
        4,
        cr_height - 16,
        COLOR_WHITE,
        t->footer_color,
        0,
        line);
}

static void
draw_toolbar(void)
{
    int i;
    struct theme_d *t;

    if (dc00 == NULL)
        return;

    t = &Themes[CurrentTheme];

    lingui_draw_rectangle0_dc(
        dc00,
        0, 0,
        cr_width, HEADER_HEIGHT,
        t->header_color,
        0);

    for (i = 0; i < BTN_COUNT; i++)
    {
        if (Uic[i] != NULL)
            libgui_redraw_ui_component(Uic[i], dc00);
    }
}

// =========================================================
// Toolbar
// =========================================================

#define TOOLBAR_HEIGHT  40
#define TOOLBAR_Y       4
#define TOOLBAR_GAP     6

static void
create_toolbar(void)
{
    int i;
    unsigned long btn_w;
    unsigned long btn_h;
    unsigned long x;

    if (dc00 == NULL)
        return;

    btn_h = 28;
    btn_w = (cr_width - ((BTN_COUNT + 1) * TOOLBAR_GAP)) / BTN_COUNT;

    for (i = 0; i < BTN_COUNT; i++)
    {
        x = TOOLBAR_GAP + ((btn_w + TOOLBAR_GAP) * i);

        Btn[i].button_id = i + 1;
        Btn[i].left      = x;
        Btn[i].top       = TOOLBAR_Y;
        Btn[i].width     = btn_w;
        Btn[i].height    = btn_h;

        Uic[i] = libgui_create_ui_component(
            dc00,
            UI_COMPONENT_BUTTON,
            Btn[i].left,
            Btn[i].top,
            Btn[i].width,
            Btn[i].height,
            BtnLabel[i],
            0x0001 | 0x0002);
    }
}

static void
update_toolbar_layout(void)
{
    int i;
    unsigned long btn_w;
    unsigned long btn_h;
    unsigned long x;

    if (dc00 == NULL)
        return;

    btn_h = 28;
    btn_w = (cr_width - ((BTN_COUNT + 1) * TOOLBAR_GAP)) / BTN_COUNT;

    for (i = 0; i < BTN_COUNT; i++)
    {
        if (Uic[i] == NULL)
            continue;

        x = TOOLBAR_GAP + ((btn_w + TOOLBAR_GAP) * i);

        Btn[i].left   = x;
        Btn[i].top    = TOOLBAR_Y;
        Btn[i].width  = btn_w;
        Btn[i].height = btn_h;

        libgui_set_ui_component_position(Uic[i], Btn[i].left, Btn[i].top);
        libgui_set_ui_component_dimension(Uic[i], Btn[i].width, Btn[i].height);
    }
}

static int
hit_test_toolbar(unsigned long mx, unsigned long my)
{
    int i;

    for (i = 0; i < BTN_COUNT; i++)
    {
        if (mx >= Btn[i].left &&
            mx <  Btn[i].left + Btn[i].width &&
            my >= Btn[i].top  &&
            my <  Btn[i].top  + Btn[i].height)
        {
            return Btn[i].button_id;
        }
    }

    return -1;
}

static void
toolbar_button_pressed(int id)
{
    if (id < 1 || id > BTN_COUNT)
        return;

    if (Uic[id - 1] == NULL || dc00 == NULL)
        return;

    libgui_on_button_pressed(Uic[id - 1], dc00);
}

static void
toolbar_button_released(int id)
{
    if (id < 1 || id > BTN_COUNT)
        return;

    if (Uic[id - 1] == NULL || dc00 == NULL)
        return;

    libgui_on_button_released(Uic[id - 1], dc00);
}

static void
toolbar_button_clicked(int fd, int id)
{
    switch (id)
    {
        case BTN_SYSTEM:
            CurrentPage = PAGE_SYSTEM;
            break;

        case BTN_MEMORY:
            CurrentPage = PAGE_MEMORY;
            break;

        case BTN_EVENTS:
            CurrentPage = PAGE_EVENTS;
            break;

        case BTN_SCOPE:
            CurrentPage = PAGE_SCOPE;
            break;

        case BTN_INSPECTOR:
            CurrentPage = PAGE_INSPECTOR;
            break;

        case BTN_EXIT:
            isTimeToQuit = TRUE;
            return;

        default:
            return;
    }

    update_children(fd);
}

// =========================================================
// Full redraw
// =========================================================

/*
static void
update_children(int fd)
{
    struct theme_d *t;

    if (fd < 0)
        return;

    if (dc00 == NULL)
        return;

    // Refresh geometry first
    query_client_area(fd);

    // Content area (single consistent calculation)
    ContentLeft   = 8;
    ContentTop    = HEADER_HEIGHT + 8;
    ContentWidth  = (cr_width > 16) ? (cr_width - 16) : 0;
    ContentHeight = (cr_height > (HEADER_HEIGHT + STATUS_HEIGHT + PALETTE_HEIGHT + 24))
                    ? (cr_height - HEADER_HEIGHT - STATUS_HEIGHT - PALETTE_HEIGHT - 24)
                    : 0;

    t = &Themes[CurrentTheme];

    // Responsive controls
    update_toolbar_layout();
    update_swatch_layout();

    // Clear client area
    lingui_draw_rectangle0_dc(
        dc00,
        0, 0,
        cr_width, cr_height,
        t->background_color,
        0);

    // Header / Toolbar
    draw_toolbar();

    // Current page
    switch (CurrentPage)
    {
        case PAGE_SYSTEM:
            draw_system_page();
            break;

        case PAGE_MEMORY:
            draw_memory_page();
            break;

        case PAGE_EVENTS:
            draw_events_page();
            break;

        case PAGE_SCOPE:
            draw_scope_page();
            break;

        case PAGE_INSPECTOR:
            draw_inspector_page();
            break;

        default:
            draw_scope_page();
            break;
    }

    // Theme swatches
    draw_theme_swatches();

    // Footer
    draw_status();
}
*/

static void
update_children(int fd)
{
    struct theme_d *t;

    if (fd < 0)
        return;

    if (dc00 == NULL)
        return;

    // Refresh geometry first
    query_client_area(fd);

    // Content area – protect against underflow
    ContentLeft  = 8;
    ContentTop   = HEADER_HEIGHT + 8;

    if (cr_width > 16)
        ContentWidth = cr_width - 16;
    else
        ContentWidth = 0;

    if (cr_height > (HEADER_HEIGHT + STATUS_HEIGHT + PALETTE_HEIGHT + 24))
        ContentHeight = cr_height - HEADER_HEIGHT - STATUS_HEIGHT - PALETTE_HEIGHT - 24;
    else
        ContentHeight = 0;

    t = &Themes[CurrentTheme];

    // Responsive controls
    update_toolbar_layout();
    update_swatch_layout();

    // Clear client area
    lingui_draw_rectangle0_dc(
        dc00,
        0, 0,
        cr_width, cr_height,
        t->background_color,
        0);

    // Header / Toolbar
    draw_toolbar();

    // Current page
    switch (CurrentPage)
    {
        case PAGE_SYSTEM:
            draw_system_page();
            break;

        case PAGE_MEMORY:
            draw_memory_page();
            break;

        case PAGE_EVENTS:
            draw_events_page();
            break;

        case PAGE_SCOPE:
            draw_scope_page();
            break;

        case PAGE_INSPECTOR:
            draw_inspector_page();
            break;

        default:
            draw_scope_page();
            break;
    }

    // Theme swatches
    draw_theme_swatches();

    // Footer
    draw_status();
}

// =========================================================
// Main event dispatcher
// =========================================================

static int
studioProcedure(
    int fd,
    int event_window,
    int event_type,
    unsigned long long1,
    unsigned long long2)
{
    int ButtonId;
    int SwatchId;

    if (fd < 0)
        return -1;

    if (event_type < 0)
        return -1;

    switch (event_type)
    {
        // Null heartbeat
        case 0:
            return 0;

        // Paint
        case MSG_PAINT:
            ScopePaintEvents++;
            ScopeTotalEvents++;
            scope_push_sample(40);
            eventlog_push("MSG_PAINT");
            update_children(fd);
            return 0;

        // Keyboard
        case MSG_KEYDOWN:
            ScopeKeyEvents++;
            ScopeTotalEvents++;
            scope_push_sample(100);
            eventlog_push("MSG_KEYDOWN");

            switch (long1)
            {
                case '1':
                    CurrentPage = PAGE_SYSTEM;
                    update_children(fd);
                    return 0;

                case '2':
                    CurrentPage = PAGE_MEMORY;
                    update_children(fd);
                    return 0;

                case '3':
                    CurrentPage = PAGE_EVENTS;
                    update_children(fd);
                    return 0;

                case '4':
                    CurrentPage = PAGE_SCOPE;
                    update_children(fd);
                    return 0;

                case '5':
                    CurrentPage = PAGE_INSPECTOR;
                    update_children(fd);
                    return 0;

                case 'T':
                case 't':
                    CurrentTheme++;
                    if (CurrentTheme >= SWATCH_COUNT)
                        CurrentTheme = 0;
                    update_children(fd);
                    return 0;

                case 'R':
                case 'r':
                    eventlog_push("Refresh");
                    update_children(fd);
                    return 0;

                case 'L':
                case 'l':
                    memset(EventLog, 0, sizeof(EventLog));
                    EventLogIndex = 0;
                    update_children(fd);
                    return 0;

                case 'Q':
                case 'q':
                    eventlog_push("Exit requested");
                    isTimeToQuit = TRUE;
                    return 0;

                default:
                    break;
            }
            return 0;

        // System keys
        case MSG_SYSKEYDOWN:
            ScopeSystemEvents++;
            ScopeTotalEvents++;
            scope_push_sample(80);
            eventlog_push("MSG_SYSKEYDOWN");

            switch (long1)
            {
                case VK_F5:
                    update_children(fd);
                    return 0;

                case VK_F1:
                    CurrentPage = PAGE_SYSTEM;
                    update_children(fd);
                    return 0;

                case VK_F2:
                    CurrentPage = PAGE_MEMORY;
                    update_children(fd);
                    return 0;

                case VK_F3:
                    CurrentPage = PAGE_EVENTS;
                    update_children(fd);
                    return 0;

                case VK_F4:
                    CurrentPage = PAGE_SCOPE;
                    update_children(fd);
                    return 0;

                case VK_F6:
                    CurrentPage = PAGE_INSPECTOR;
                    update_children(fd);
                    return 0;

                default:
                    break;
            }
            return 0;

        // Mouse movement
        case MSG_MOUSEMOVE:
            ScopeMouseEvents++;
            ScopeTotalEvents++;
            scope_push_sample(20);
            eventlog_push("MSG_MOUSEMOVE");

            last_x = long1;
            last_y = long2;

            InspectorMouseX = long1;
            InspectorMouseY = long2;
            InspectorWindow = event_window;

            ButtonId = hit_test_toolbar(long1, long2);
            __hover_button_id = ButtonId;

            return 0;

        // Mouse press
        case MSG_MOUSEPRESSED:
            ScopeMouseEvents++;
            ScopeTotalEvents++;
            scope_push_sample(60);
            eventlog_push("MSG_MOUSEPRESSED");

            ButtonId = hit_test_toolbar(last_x, last_y);
            __pressed_button_id = ButtonId;

            toolbar_button_pressed(__pressed_button_id);
            return 0;

        // Mouse release
        case MSG_MOUSERELEASED:
            ScopeMouseEvents++;
            ScopeTotalEvents++;
            scope_push_sample(70);
            eventlog_push("MSG_MOUSERELEASED");

            // Theme swatches first
            SwatchId = hit_test_swatch(last_x, last_y);

            if (SwatchId >= SWATCH_BASE)
            {
                theme_swatch_clicked(fd, SwatchId);
                return 0;
            }

            // Toolbar
            toolbar_button_released(__pressed_button_id);

            if (__pressed_button_id >= 0)
            {
                ButtonId = hit_test_toolbar(last_x, last_y);

                if (ButtonId == __pressed_button_id)
                    toolbar_button_clicked(fd, ButtonId);
            }

            __pressed_button_id = -1;
            return 0;

        // Close
        case MSG_CLOSE:
            eventlog_push("MSG_CLOSE");
            isTimeToQuit = TRUE;
            return 0;

        // Unknown
        default:
            return -1;
    }

    return 0;
}

// =========================================================
// Main
// =========================================================

int
main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    int fd = -1;
    struct gws_window_info_d wi;

    unsigned long screen_w;
    unsigned long screen_h;
    unsigned long win_x;
    unsigned long win_y;
    unsigned long win_w;
    unsigned long win_h;

    // Display
    Display = gws_open_display(display_name);

    if (Display == NULL)
    {
        printf("studio: gws_open_display fail\n");
        return EXIT_FAILURE;
    }

    fd = Display->fd;

    if (fd <= 0)
    {
        printf("studio: invalid fd\n");
        return EXIT_FAILURE;
    }

    // Screen
    screen_w = gws_get_system_metrics(1);
    screen_h = gws_get_system_metrics(2);

    // Library
    if (libgui_initialize() < 0)
    {
        printf("studio: libgui_initialize fail\n");
        return EXIT_FAILURE;
    }

    // Window geometry
    win_w = (screen_w * 8) / 10;
    win_h = (screen_h * 8) / 10;
    win_x = (screen_w - win_w) / 2;
    win_y = (screen_h - win_h) / 2;

    // Main window
    main_window = gws_create_window(
        fd,
        WT_OVERLAPPED,
        WINDOW_STATUS_ACTIVE,
        WINDOW_STATE_NULL,
        "Gramado Studio",
        win_x, win_y, win_w, win_h,
        0,
        WS_APP,
        COLOR_WHITE,
        COLOR_GRAY);

    if (main_window < 0)
    {
        printf("studio: main_window fail\n");
        return EXIT_FAILURE;
    }

    // Client area
    query_client_area(fd);

    // Window info
    gws_get_window_info(fd, main_window, &wi);

    // Shared flag
    __sh_flags = (unsigned long) wi.sh_flags;

    // Update wproxy
    {
        unsigned long m[10];
        int mytid;

        mytid = gettid();

        m[0] = (unsigned long)(mytid & 0xFFFFFFFF);

        // Frame rectangle
        m[1] = wi.left;
        m[2] = wi.top;
        m[3] = wi.width;
        m[4] = wi.height;

        // Client rectangle
        m[5] = wi.cr_left;
        m[6] = wi.cr_top;
        m[7] = wi.cr_width;
        m[8] = wi.cr_height;

        sc80(48, &m[0], &m[0], &m[0]);
    }

    // DC
    dc00 = libgui_create_dc(
        wi.ca_canvas_base_address,
        wi.ca_canvas_width,
        wi.ca_canvas_height,
        wi.ca_canvas_bpp);

    if (dc00 == NULL)
    {
        printf("studio: libgui_create_dc fail\n");
        return EXIT_FAILURE;
    }

    // Initial state
    CurrentPage  = PAGE_SCOPE;
    CurrentTheme = 0;

    memset(EventLog, 0, sizeof(EventLog));
    memset(ScopeHistory, 0, sizeof(ScopeHistory));
    ScopePosition = 0;

    // Toolbar
    create_toolbar();

    // Initial paint
    update_children(fd);

    // Active
    gws_set_active(fd, main_window);

    isTimeToQuit = FALSE;

    // =====================================================
    // Event loop  (kept as close as possible to original)
    // =====================================================

    while (1)
    {
        if (isTimeToQuit == TRUE)
            break;

        // Shared flags
        if (__sh_flags != 0)
        {
            char *flags_ptr = (char *) __sh_flags;

            if (flags_ptr != NULL && (*flags_ptr & 0x0008))
            {
                // Clear BLIT
                *flags_ptr &= ~0x0008;
                update_children(fd);
            }
        }

        // Input Broker
        if (rtl_get_event() == TRUE)
        {
            // mouse coords
            last_x = (unsigned long) RTLEventBuffer[4];
            last_y = (unsigned long) RTLEventBuffer[5];

            // dispatch
            studioProcedure(
                fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3]);

            // consume
            RTLEventBuffer[1] = 0;
        }
    }

    // Exit
    if (main_window >= 0)
        gws_destroy_window(fd, main_window);

    if (fd > 0)
        close(fd);

    return EXIT_SUCCESS;
}

