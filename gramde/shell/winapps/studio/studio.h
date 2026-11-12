#ifndef __STUDIO_H
#define __STUDIO_H  1

//
// Pages
//

#define PAGE_SYSTEM      1
#define PAGE_MEMORY      2
#define PAGE_EVENTS      3
#define PAGE_SCOPE       4
#define PAGE_INSPECTOR   5

//
// Toolbar buttons
//

#define BTN_SYSTEM       1
#define BTN_MEMORY       2
#define BTN_EVENTS       3
#define BTN_SCOPE        4
#define BTN_INSPECTOR    5
#define BTN_EXIT         6

#define BTN_COUNT        6

//
// Theme swatches
//

#define SWATCH_BASE      100
#define SWATCH_COUNT     6

//
// Layout
//

#define HEADER_HEIGHT    48
#define STATUS_HEIGHT    20
#define PALETTE_HEIGHT   30

//
// Event log
//

#define EVENT_LOG_LINES  32
#define EVENT_LOG_TEXT   64

//
// Scope history
//

#define SCOPE_HISTORY    512

struct button_info_d
{
    int button_id;

    unsigned long left;
    unsigned long top;

    unsigned long width;
    unsigned long height;

    unsigned long absolute_left;
    unsigned long absolute_top;

    int state;
};

//
// Theme
//

struct theme_d
{
    unsigned int header_color;
    unsigned int footer_color;
    unsigned int background_color;
    unsigned int accent_color;
};

#endif

