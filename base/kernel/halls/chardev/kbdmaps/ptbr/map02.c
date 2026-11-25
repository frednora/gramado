// latin_keymap.c
// Complete Latin American Spanish keyboard mapping for Gramado OS
// Normal, Shift, Control, AltGr, and Extended tables
// Uses ASCII definitions and kbdmaps.h constants

// #todo: Create the header. We need to export things

#include <kernel.h>
#include "ascii.h"
#include "kbdmaps.h"

// ---------------------------------
// Normal Latin American keymap
// Lowercase letters, digits, and symbols
// Used when no modifiers are active

unsigned char map_latam[LATAM_CHARMAP_SIZE] = {
    // 0x00–0x0F
    0, ASCII_ESC, '1','2','3','4','5','6','7','8','9','0','\'','¡', VK_BACKSPACE,
    VK_TAB,

    // 0x10–0x1F
    'q','w','e','r','t','y','u','i','o','p',
    ASCII_ACUTE, '+', VK_RETURN, VK_LCONTROL, 'a','s',

    // 0x20–0x2F
    'd','f','g','h','j','k','l','ñ','{','}',
    VK_LSHIFT, '\\', 'z','x','c','v',

    // 0x30–0x3F
    'b','n','m',',','.','-',
    VK_RSHIFT, '*', VK_ALT, ASCII_SPACE,
    VK_CAPSLOCK, VK_F1, VK_F2, VK_F3, VK_F4, VK_F5,

    // 0x40–0x4F
    VK_F6, VK_F7, VK_F8, VK_F9, VK_F10,
    VK_PAUSEBREAK, VK_SCROLLLOCK,
    VK_HOME, VK_ARROW_UP, VK_PAGEUP,
    '-', VK_ARROW_LEFT, '5', VK_ARROW_RIGHT, '+', VK_END,

    // 0x50–0x5F
    VK_ARROW_DOWN, VK_PAGEDOWN, VK_INSERT, VK_DELETE,
    VK_PRINTSCREEN, '.', '\\',
    VK_F11, VK_F12, '/', 0,
    VK_LWIN, VK_RWIN, VK_APPS, 0, 0,

    // 0x60–0x6F
    '`', VK_LCONTROL, '/', 0, VK_ALT,
    0,0, VK_ARROW_UP, 0,
    VK_ARROW_LEFT, VK_ARROW_RIGHT, 0,
    VK_ARROW_DOWN, 0, VK_INSERT, 0,

    // 0x70–0x7F
    VK_F1, VK_F2, VK_F3, '/', VK_F5, VK_F6, VK_F7, VK_PAUSEBREAK,
    VK_F9, VK_F10, VK_F11, VK_F12, 0, 0,
    '.', VK_F16, VK_F17, VK_F18, VK_F19,
    VK_F20, VK_F21, VK_F22, VK_F23, VK_F24
};

// ---------------------------------------
// Shift Latin American keymap
// Uppercase letters and shifted symbols
// Used when Shift or CapsLock is active

unsigned char shift_latam[LATAM_CHARMAP_SIZE] = {
    0, ASCII_ESC, '!','"','#','$','%','&','/','(',')','=','?','¿',
    VK_BACKSPACE, VK_TAB,

    'Q','W','E','R','T','Y','U','I','O','P',
    '`','*', VK_RETURN, VK_LCONTROL, 'A','S',

    'D','F','G','H','J','K','L','Ñ','[',']',
    VK_LSHIFT, '|', 'Z','X','C','V',

    'B','N','M',';',':','_',
    VK_RSHIFT, '*', VK_ALT, ASCII_SPACE,
    VK_CAPSLOCK, VK_F1, VK_F2, VK_F3, VK_F4, VK_F5,

    VK_F6, VK_F7, VK_F8, VK_F9, VK_F10,
    VK_PAUSEBREAK, VK_SCROLLLOCK,
    VK_HOME, VK_ARROW_UP, VK_PAGEUP,
    '-', VK_ARROW_LEFT, '5', VK_ARROW_RIGHT, '+', VK_END,

    VK_ARROW_DOWN, VK_PAGEDOWN, VK_INSERT, VK_DELETE,
    VK_PRINTSCREEN, '.', '|',
    VK_F11, VK_F12, '/', 0,
    VK_LWIN, VK_RWIN, VK_APPS, 0, 0,

    '~', VK_LCONTROL, '/', 0, VK_ALT,
    0,0, VK_ARROW_UP, 0,
    VK_ARROW_LEFT, VK_ARROW_RIGHT, 0,
    VK_ARROW_DOWN, 0, VK_INSERT, 0,

    VK_F1, VK_F2, VK_F3, '?', VK_F5, VK_F6, VK_F7, VK_PAUSEBREAK,
    VK_F9, VK_F10, VK_F11, VK_F12, 0, 0,
    '.', VK_F16, VK_F17, VK_F18, VK_F19,
    VK_F20, VK_F21, VK_F22, VK_F23, VK_F24
};

// ------------------------------
// Control Latin American keymap
// Maps scancodes to ASCII control characters (0x00–0x1F)

unsigned char ctl_latam[LATAM_CHARMAP_SIZE] = {
    0, ASCII_ESC, ASCII_SOH, ASCII_STX, ASCII_ETX, ASCII_EOT, ASCII_ENQ,
    ASCII_ACK, ASCII_BEL, ASCII_HT, ASCII_LF, ASCII_VT, ASCII_FF, ASCII_CR,
    VK_BACKSPACE, VK_TAB,

    ASCII_DC1, ASCII_DC2, ASCII_DC3, ASCII_DC4,
    ASCII_NAK, ASCII_SYN, ASCII_ETB, ASCII_CAN,
    ASCII_EM, ASCII_SUB, ASCII_ESC, ASCII_FS,
    VK_RETURN, VK_LCONTROL, ASCII_SOH, ASCII_DC3,

    ASCII_EOT, ASCII_ACK, ASCII_BEL, ASCII_BS,
    ASCII_LF, ASCII_VT, ASCII_FF, ASCII_RS,
    ASCII_GS, ASCII_US, VK_LSHIFT, ASCII_ESC,
    ASCII_SUB, ASCII_CAN, ASCII_ETX, ASCII_SYN,

    ASCII_STX, ASCII_SO, ASCII_CR, '<','>','?',
    VK_RSHIFT, '*', VK_ALT, ASCII_NUL, // Ctrl+Space
    VK_CAPSLOCK, VK_F1, VK_F2, VK_F3, VK_F4, VK_F5,

    VK_F6, VK_F7, VK_F8, VK_F9, VK_F10,
    VK_PAUSEBREAK, VK_SCROLLLOCK,
    VK_HOME, VK_ARROW_UP, VK_PAGEUP,
    '-', VK_ARROW_LEFT, '5', VK_ARROW_RIGHT, '+', VK_END,

    VK_ARROW_DOWN, VK_PAGEDOWN, VK_INSERT, VK_DELETE,
    VK_PRINTSCREEN, '.', '\\',
    VK_F11, VK_F12, '/', 0,
    VK_LWIN, VK_RWIN, VK_APPS, 0, 0,

    '`', VK_LCONTROL, '/', 0, VK_ALT,
    0,0, VK_ARROW_UP, 0,
    VK_ARROW_LEFT, VK_ARROW_RIGHT, 0,
    VK_ARROW_DOWN, 0, VK_INSERT, 0,

    VK_F1, VK_F2, VK_F3, '/', VK_F5, VK_F6, VK_F7, VK_PAUSEBREAK,
    VK_F9, VK_F10, VK_F11, VK_F12, 0, 0,
    '.', VK_F16, VK_F17, VK_F18, VK_F19,
    VK_F20, VK_F21, VK_F22, VK_F23, VK_F24
};

// ---------------------------------------
// AltGr Latin American keymap
// Provides access to third-level characters (€, ª, º, @, etc.)

unsigned char altgr_latam[LATAM_CHARMAP_SIZE] = {
    0,0,'¹','²','³','£','¢','¬','{','}','§','[',']','\\','|',
    VK_TAB,

    '@','#','€','¢','¬','¦','¨','´','~','^',
    0,0,VK_RETURN,VK_LCONTROL,'ª','º',

    0,0,0,0,0,0,0,'ñ','~','"',
    VK_LSHIFT,'}',0,0,0,0,

    0,0,0,'<','>',';',
    VK_RSHIFT,'*',VK_ALTGR,ASCII_SPACE,
    VK_CAPSLOCK,VK_F1,VK_F2,VK_F3,VK_F4,VK_F5,

    // rest filled with 0 or VK codes as needed
};

// ---------------------------------------
// Extended Latin American keymap
// Handles E0-prefixed scancodes (navigation, right modifiers, system keys)
// Covers arrows, Insert/Delete, Home/End, PgUp/PgDn, right Ctrl/AltGr, Windows keys

unsigned char extended_latam[LATAM_CHARMAP_SIZE] = {
    // 0x00–0x0F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    // 0x10–0x1F
    0,0,0,0,0,
    0,0,0,0,0,
    VK_RETURN,   // 0x1A (NumPad Enter)
    0,           // 0x1B
    VK_RETURN,   // 0x1C (NumPad Enter again)
    VK_RCONTROL, // 0x1D Right Control
    0,0,

    // 0x20–0x2F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    // 0x30–0x3F
    0,0,0,0,
    0,0,0,0,
    VK_ALTGR,    // 0x38 Right Alt (AltGr)
    0,0,0,0,0,0,0,

    // 0x40–0x4F
    0,0,0,0,
    0,0,0,
    VK_HOME,       // 0x47
    VK_ARROW_UP,   // 0x48
    VK_PAGEUP,     // 0x49
    0,             // 0x4A
    VK_ARROW_LEFT, // 0x4B
    0,             // 0x4C
    VK_ARROW_RIGHT,// 0x4D
    0,             // 0x4E
    VK_END,        // 0x4F

    // 0x50–0x5F
    VK_ARROW_DOWN, // 0x50
    VK_PAGEDOWN,   // 0x51
    VK_INSERT,     // 0x52
    VK_DELETE,     // 0x53
    0,0,0,0,0,0,
    0,             // 0x5A
    VK_LWIN,       // 0x5B Left Windows key
    VK_RWIN,       // 0x5C Right Windows key
    VK_APPS,       // 0x5D Menu key
    0,             // 0x5E
    0,             // 0x5F

    // 0x60–0x6F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    // 0x70–0x7F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

