// mapabnt2.c
// Ring0 keymap support for abnt2 keyboards.
// Created by Fred Nora.

#include <kernel.h>

// ---------------------------------
// map_abnt2[]
// Normal (lowercase) ABNT2 keymap.
// Used when no modifiers are active.
// Converts raw scancodes into ASCII characters or VK codes.
// Examples: 'a', '1', 'ç', Enter, arrows.
// This is the default lookup table.

unsigned char map_abnt2[ABNT2_CHARMAP_SIZE] = {

// 0x00 ~ 0x0F
 0,    // 0x00
 033,  // 0x01
 '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', VK_BACKSPACE, 
 VK_TAB,

// 0x10 ~ 0x1F
 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
VK_ACUTE,
 '[', 
VK_RETURN, 
VK_LCONTROL,
 'a', 
 's',

// 0x20 ~ 0x2F 
 'd', 'f', 'g', 'h', 'j', 'k', 'l',
 135,
 '~', 
 '\'',       //41 (") (espaço) (') dead_acute dead_diaeresis 
 VK_LSHIFT,  //42 Shift.  
 ']',        //43 bracketright braceright asciitilde Control_bracketright 
 'z',        //44 
 'x',
 'c',
 'v',

// 0x30 ~ 0x3F 
 'b', 'n', 'm', ',', '.', ';',
 VK_RSHIFT, 
 '*', 
 VK_ALTGR,
 ' ',
 VK_CAPSLOCK, 
 VK_F1,
 VK_F2,
 VK_F3,
 VK_F4,
 VK_F5,

// 0x40 ~ 0x4F 
 VK_F6,
 VK_F7,
 VK_F8,
 VK_F9,
 VK_F10,
 VK_PAUSEBREAK,
 VK_SCROLLLOCK,
 VK_HOME, 
 VK_ARROW_UP, 
 VK_PAGEUP, 
 '-', 
 VK_ARROW_LEFT,
 '5',
 VK_ARROW_RIGHT, 
 '+',
 VK_END, 

// 0x50 ~ 0x5F 
VK_ARROW_DOWN, 
VK_PAGEDOWN,   
VK_INSERT,
VK_DELETE,
VK_PRINTSCREEN,
'.',
'\\',
VK_F11,
VK_F12,
'/', 
VK_F14,
VK_LWIN,
VK_RWIN,
VK_APPS,
VK_F18,
VK_F19,

// 0x60 ~ 0x6F 
'`',  // (`) grave
VK_LCONTROL,
'/', 
VK_F23,
VK_ALTGR,
'b',
'f',
VK_ARROW_UP,
'p', 
VK_ARROW_LEFT,
VK_ARROW_RIGHT,
's',  
VK_ARROW_DOWN,
'n',
VK_INSERT, 
'r', 

// 0x70 ~ 0x7F 
VK_F1,
VK_F2,
VK_F3,
'/', 
VK_F5,
VK_F6,
VK_F7,
VK_PAUSEBREAK, 
VK_F9, 
VK_F10, VK_F11, VK_F12, VK_F13, VK_F14, 
'.',  
VK_F16, 

// 0x70 ~ 0x7F 
VK_F17, 
VK_F18,
VK_F19,
VK_F20, VK_F21, VK_F22, VK_F23, VK_F24,
};

// ---------------------------------------
// shift_abnt2[]
// Shifted (uppercase) ABNT2 keymap.
// Used when Shift or CapsLock is active.
// Provides uppercase letters and shifted symbols.
// Examples: 'A', '!', '@', 'Ç', ':', '?'
// This table ensures proper output when modifiers are pressed.

unsigned char shift_abnt2[ABNT2_CHARMAP_SIZE] = {

// 0x00 ~ 0x0F
0, 
033, 
'!', '@', '#', '$', '%', 
168,  // Trema ?
'&', '*', '(',  ')', '_', '+', 
VK_BACKSPACE,
VK_TAB, 

// 0x10 ~ 0x1F
'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 
'`', // Grave
'{', 
VK_RETURN,
CTL,  
'A',
'S',

// 0x20 ~ 0x2F
'D', 'F', 'G', 'H', 'J', 'K', 'L', 
128,  //  Ç cedilha
'^',
'"',
SHF,
 '}', 
'Z', 'X', 'C', 'V',

// 0x30 ~ 0x3F
'B', 'N', 'M', 
'<', 
'>', 
':',
SHF, 
CTL, 
ALT,
' ',
CPS | L, 
0, 
0,
' ', 
0, 
0, 

// 0x40 ~ 0x4F
0,
0,
0,
0,
0,
NUM | L,
STP | L,
VK_HOME,
VK_ARROW_UP,
VK_PAGEUP,
'-',
VK_ARROW_LEFT,
'5',
VK_ARROW_RIGHT,
'+', 
VK_END,

// 0x50 ~ 0x5F
VK_ARROW_DOWN,
VK_PAGEDOWN,
VK_INSERT,
VK_DELETE, 
0,
0,
'|',
0, 0, 0, 0, 0, 0, 0, 0, 0,

// 0x60 ~ 0x6F
0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0,

// 0x70 ~ 0x7F
0,
0,
0,
'?',
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
'.',
0,
};


// ------------------------------
// ctl_abnt2[]
// Control ABNT2 keymap.
// Used when Ctrl is active.
// Maps scancodes to ASCII control characters (0x01–0x1F).
// Examples: Ctrl+A → SOH, Ctrl+C → ETX, Ctrl+Z → SUB.
// Enables terminal shortcuts and kernel shell commands.

unsigned char ctl_abnt2[ABNT2_CHARMAP_SIZE] = {

// 0x00 ~ 0x0F
0, 
033,
'!', 
000,
'#', '$', '%',
036,
'&', '*', '(', ')',
ASCII_US,  // ^_
'+',
034,
'\177',

// 0x10 ~ 0x1F
ASCII_DC1,    // ^Q  DC1
ASCII_ETB,    // ^W
ASCII_ENQ,    // ^E  
ASCII_DC2,    // ^R  DC2
ASCII_DC4,    // ^T  DC4
ASCII_EM,     // ^Y
ASCII_NAK,    // ^U
ASCII_HT,     // ^I
ASCII_SI,     // ^O
ASCII_DLE,    // ^P
033,          //
ASCII_ESC,    // ^[
'\r',         //
CTL,          //
ASCII_SOH,    // ^A
ASCII_DC3,    // ^S  DC3

// 0x20 ~ 0x2F
ASCII_EOT,    // ^D
ASCII_ACK,    // ^F
ASCII_BEL,    // ^G
ASCII_BS,     // ^H
ASCII_LF,     // ^J
ASCII_VT,     // ^K
ASCII_FF,     // ^L
';',          //
ASCII_RS,     // ^^
'`',          //
SHF,          //
ASCII_GS,     // ^]
ASCII_SUB,    // ^Z
ASCII_CAN,    // ^X
ASCII_ETX,    // ^C
ASCII_SYN,    // ^V

// 0x30 ~ 0x3F
ASCII_STX,    // ^B
ASCII_SO,     // ^N
ASCII_CR,     // ^M
'<', 
'>',
'?', 
SHF, 
'*', 
ALT,
' ', 
CPS|L,
0,
0, 
' ', 
0, 
0,

// 0x40 ~ 0x4F
CPS|L,
0, 
0,
0,
0,
0,
0,
0,
0,
0,
0, 0, 0, 0, 0, 0, 

// 0x50 ~ 0x5F
0, 
0,
0,
0,
0,
0,
ASCII_FS,
0,
0,
0,
033,
'7', '4', '1',
0, 
NUM|L, 

// 0x60 ~ 0x6F
'8', '5', '2',
0,
STP|L,
'9', '6', '3',
'.',
0,
'*', '-', '+',
0,
0,
0,

// 0x60 ~ 0x6F
0, 0, 0, 0, 0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0,
};

// ---------------------------------------
// extended_abnt2[]
// Extended ABNT2 keymap.
// Used when the scancode stream has an E0 prefix.
// Covers navigation cluster, right-side modifiers, and system keys.

unsigned char extended_abnt2[ABNT2_CHARMAP_SIZE] = {

// 00 ~ 0F
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

// 10 ~ 1F
0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 
VK_RETURN, // 0x1A
0, // 0x1B
VK_RETURN, // 0x1C
VK_RCONTROL,   // 0x1D
0, 
0,

// 20 ~ 2F
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

// 30 ~ 3F
0, 0, 0, 0, 
0, 0, 0, 0, 
VK_ALTGR,  // 0x38 
0, 0, 0, 0, 0, 0, 0,

// 40 ~ 4F
0, 0, 0, 0, 
0, 0, 0, 
VK_HOME, // 0x47 
VK_ARROW_UP,  // 0x48 
VK_PAGEUP,    // 0x49
0,  // 0x4A
VK_ARROW_LEFT,  // 0x4B
0,  // 0x4C
VK_ARROW_RIGHT,  // 0x4D
0,  // 0x4E
VK_END,  // 0x4F


// 50 ~ 5F
VK_ARROW_DOWN, //0x50
VK_PAGEDOWN, //0x51
VK_INSERT, //0x52
VK_DELETE, //0x53
0, 
0, 
0, 
0, 
0, 
0, 
0, // 0x5A
VK_LWIN, // 0x5B 
VK_RWIN, // 0x5C
VK_APPS, // 0x5D 
0, // 0x5E 
0, // 0x5F

// 60 ~ 6F
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

// 70 ~ 7F
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

};

