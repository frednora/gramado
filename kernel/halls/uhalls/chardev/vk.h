// vk.h
// Virtual Keys.
// Created by Fred Nora.

// Teclas virtuais padrão gramado.
// Inspiradas nos scancodes do teclado abnt2, pt-br.

#ifndef ____VK_H
#define ____VK_H    1

// Control characters and system keys (0x00–0x2F)

// ASCII control overlaps
#define VK_NULL        0x00  // Null
#define VK_START       0x01  // SOH (Start of Heading)
#define VK_SOH         0x01
#define VK_STX         0x02  // Start of Text
#define VK_ETX         0x03  // End of Text
#define VK_EOT         0x04  // End of Transmission
#define VK_ENQ         0x05  // Enquiry
#define VK_ACK         0x06  // Acknowledge
#define VK_BEL         0x07  // Bell
#define VK_BACK        0x08  // Backspace
#define VK_TAB         0x09  // Horizontal Tab
#define VK_LINEFEED    0x0A  // Line Feed
#define VK_VTAB        0x0B  // Vertical Tab
#define VK_FF          0x0C  // Form Feed
#define VK_RETURN      0x0D  // Carriage Return (Enter)
#define VK_SHIFT_OUT   0x0E  // SO
#define VK_SHIFT_IN    0x0F  // SI

// Windows control keys (mapped in low range)
#define VK_SHIFT       0x10  // Shift
#define VK_CONTROL     0x11  // Ctrl
#define VK_MENU        0x12  // Alt
#define VK_PAUSE       0x13  // Pause/Break
#define VK_CAPITAL     0x14  // Caps Lock
#define VK_ESCAPE      0x1B  // Escape
#define VK_SPACE       0x20  // Space
#define VK_PRIOR       0x21  // Page Up
#define VK_NEXT        0x22  // Page Down
#define VK_END         0x23  // End
#define VK_HOME        0x24  // Home
#define VK_LEFT        0x25  // Arrow Left
#define VK_UP          0x26  // Arrow Up
#define VK_RIGHT       0x27  // Arrow Right
#define VK_DOWN        0x28  // Arrow Down
#define VK_SELECT      0x29  // Select
#define VK_PRINT       0x2A  // Print
#define VK_EXECUTE     0x2B  // Execute
#define VK_SNAPSHOT    0x2C  // Print Screen
#define VK_INSERT      0x2D  // Insert
#define VK_DELETE      0x2E  // Delete
#define VK_HELP        0x2F  // Help


// Digits 0–9 (ASCII codes)
#define VK_0   0x30
#define VK_1   0x31
#define VK_2   0x32
#define VK_3   0x33
#define VK_4   0x34
#define VK_5   0x35
#define VK_6   0x36
#define VK_7   0x37
#define VK_8   0x38
#define VK_9   0x39


// Uppercase letters (ASCII 0x41–0x5A)
#define VK_A   0x41
#define VK_B   0x42
#define VK_C   0x43
#define VK_D   0x44
#define VK_E   0x45
#define VK_F   0x46
#define VK_G   0x47
#define VK_H   0x48
#define VK_I   0x49
#define VK_J   0x4A
#define VK_K   0x4B
#define VK_L   0x4C
#define VK_M   0x4D
#define VK_N   0x4E
#define VK_O   0x4F
#define VK_P   0x50
#define VK_Q   0x51
#define VK_R   0x52
#define VK_S   0x53
#define VK_T   0x54
#define VK_U   0x55
#define VK_V   0x56
#define VK_W   0x57
#define VK_X   0x58
#define VK_Y   0x59
#define VK_Z   0x5A

// Lowercase letters (ASCII 0x61–0x7A)
#define VK_a   0x61
#define VK_b   0x62
#define VK_c   0x63
#define VK_d   0x64
#define VK_e   0x65
#define VK_f   0x66
#define VK_g   0x67
#define VK_h   0x68
#define VK_i   0x69
#define VK_j   0x6A
#define VK_k   0x6B
#define VK_l   0x6C
#define VK_m   0x6D
#define VK_n   0x6E
#define VK_o   0x6F
#define VK_p   0x70
#define VK_q   0x71
#define VK_r   0x72
#define VK_s   0x73
#define VK_t   0x74
#define VK_u   0x75
#define VK_v   0x76
#define VK_w   0x77
#define VK_x   0x78
#define VK_y   0x79
#define VK_z   0x7A


// Function keys (Gramado VKs start at 0x80)
#define VK_F1   0x80
#define VK_F2   0x81
#define VK_F3   0x82
#define VK_F4   0x83
#define VK_F5   0x84
#define VK_F6   0x85
#define VK_F7   0x86
#define VK_F8   0x87
#define VK_F9   0x88
#define VK_F10  0x89
#define VK_F11  0x8A
#define VK_F12  0x8B
#define VK_F13  0x8C
#define VK_F14  0x8D
#define VK_F15  0x8E
#define VK_F16  0x8F
#define VK_F17  0x90
#define VK_F18  0x91
#define VK_F19  0x92
#define VK_F20  0x93
#define VK_F21  0x94
#define VK_F22  0x95
#define VK_F23  0x96
#define VK_F24  0x97

// Escape key (Gramado VK cluster)
#define VK_ESCAPE    0x98  // Escape

// Programming symbols (0x99–0x9F)
#define VK_DQUOTE    0x99  // "  double quote
#define VK_EXCLAM    0x9A  // !  exclamation
#define VK_AT        0x9B  // @  at sign
#define VK_HASH      0x9C  // #  hash / number
#define VK_DOLLAR    0x9D  // $  dollar
#define VK_PERCENT   0x9E  // %  percent
#define VK_AMPERSAND 0x9F  // &  ampersand


// Punctuation group: (), [], {}
#define VK_LPAREN    0xA0  // (
#define VK_RPAREN    0xA1  // )
#define VK_LBRACKET  0xA2  // [
#define VK_RBRACKET  0xA3  // ]
#define VK_LBRACE    0xA4  // {
#define VK_RBRACE    0xA5  // }

// Operators (0xA6–0xA9)
#define VK_PLUS      0xA6  // +
#define VK_MINUS     0xA7  // -
#define VK_ASTERISK  0xA8  // *
#define VK_SLASH     0xA9  // /

// Angle brackets (0xAA–0xAB)
#define VK_LESS     0xAA  // <
#define VK_GREATER  0xAB  // >


// Separators (0xAC–0xAF)
#define VK_PERIOD     0xAC  // .
#define VK_COMMA      0xAD  // ,
#define VK_COLON      0xAE  // :
#define VK_SEMICOLON  0xAF  // ;

// -------------------------------------

// ABNT2 accents (0xB0–0xB4)
#define VK_ACUTE       0xB0  // ´  acute accent
#define VK_GRAVE       0xB1  // `  grave accent
#define VK_TILDE       0xB2  // ~  tilde
#define VK_CIRCUMFLEX  0xB3  // ^  circumflex
#define VK_TREMA       0xB4  // ¨  diaeresis/trema

// smpty slots

// Extra programming symbols (0xBA–0xBE)
#define VK_BSLASH     0xBA  // \  backslash
#define VK_PIPE       0xBB  // |  pipe
#define VK_EQUAL      0xBC  // =  equal sign
#define VK_QUESTION   0xBD  // ?  question mark
#define VK_SQUOTE     0xBE  // '  single quote
#define VK_UNDERSCORE 0xBF  // _  underscore


// Left-side control keys (0xC0 block)
#define VK_TAB        0xC0  // Tab
#define VK_CAPSLOCK   0xC1  // Caps Lock
#define VK_LSHIFT     0xC2  // Left Shift
#define VK_LCONTROL   0xC3  // Left Control
#define VK_LWIN       0xC4  // Left Windows / Gramado key
#define VK_LALT       0xC5  // Left Alt

// Right-side control keys (0xD0 block)
#define VK_BACKSPACE  0xD0  // Backspace
#define VK_ENTER      0xD1  // Enter
#define VK_RSHIFT     0xD2  // Right Shift
#define VK_RCONTROL   0xD3  // Right Control
#define VK_APPS       0xD4  // Application/Menu key
#define VK_RWIN       0xD5  // Right Windows key
#define VK_ALTGR      0xD6  // Right Alt (AltGr)


// Center cluster keys (0xE0 block)
#define VK_INSERT     0xE0  // Insert
#define VK_DELETE     0xE1  // Delete
#define VK_HOME       0xE2  // Home
#define VK_END        0xE3  // End
#define VK_PAGEUP     0xE4  // Page Up
#define VK_PAGEDOWN   0xE5  // Page Down

// Arrow keys (0xF0 block)
#define VK_ARROW_UP     0xF0  // Up Arrow
#define VK_ARROW_DOWN   0xF1  // Down Arrow
#define VK_ARROW_LEFT   0xF2  // Left Arrow
#define VK_ARROW_RIGHT  0xF3  // Right Arrow


// System control trio (0xF4–0xF6)
#define VK_PRINTSCREEN  0xF4  // Print Screen
#define VK_SCROLLLOCK   0xF5  // Scroll Lock
#define VK_PAUSEBREAK   0xF6  // Pause/Break


// Numeric keypad control (Num Lock)
#define VK_NUMLOCK  0xF7  // Num Lock

// Mouse buttons (0xFA–0xFC)
#define VK_LBUTTON   0xFA  // Left mouse button
#define VK_RBUTTON   0xFB  // Right mouse button
#define VK_MBUTTON   0xFC  // Middle mouse button



#endif    



