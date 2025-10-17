// ascii.h

#ifndef __ASCII_H
#define __ASCII_H    1

//  ASCII  TABLE
 
#define ASCII_NUL 0    //NULL               ^@  Null/Idle

#define ASCII_SOH 1    //START OF HEADING   ^A  Start of message
#define ASCII_STX 2    //START OF TEXT      ^B  End of address
#define ASCII_ETX 3    //END OF TEXT        ^C  End of message
#define ASCII_EOT 4    //END OF TRANSMITION ^D  End of transmission
#define ASCII_ENQ 5    //ENQUIRY            ^E  "Who are you...?"
#define ASCII_ACK 6    //ACKNOWLEDGE        ^F  "Are you...?"
#define ASCII_BEL 7    //BELL               ^G  Audible signal
#define ASCII_BS  8    //BACKSPACE          ^H  Format effector
#define ASCII_HT  9    //HORIZONTAL TAB     ^I  Horizontal tabulation/ Skip (punched card)
#define ASCII_LF   0xA   //LINE FEED          ^J  Line feed
#define ASCII_VT   0xB   //VERTICAL TAB       ^K  Vertical tabulation
#define ASCII_FF   0xC   //FORM FEED          ^L  Form feed
#define ASCII_CR   0xD   //CARRIEGE RETURN    ^M  Carriage return
#define ASCII_SO   0xE   //SHIFT OUT          ^N  Shift out
#define ASCII_SI   0xF   //SHIFT IN           ^O  Shift in
#define ASCII_DLE  0x10  //DATA LINK ESCAPE   ^P  Device control reserved for data link escape

/*
dc1: ^q
A device control character, 
basically dedicated to turning on or starting essential device. 
It might also be used to restore any device to basic mode of operation.

dc2: ^r
A device control character which is 
basically dedicated to turning on 
or starting essential device. 
It might also be used to set any device to special mode of operation.

dc3: ^s
A device control character which is basically 
dedicated to turning off or 
stopping an essential device. 
Also acts as secondary step stop (e.g: wait, pause, stand-by or halt).

dc4: ^t
A device control character, 
basically dedicated to turning off a device. 
It might also be used for any device control purpose 
not granted by other DCs.
*/

#define ASCII_DC1   0x11  //DEVICE CONTROL 1   ^Q  Device control
#define ASCII_DC2   0x12  //DEVICE CONTROL 2   ^R  Device control
#define ASCII_DC3   0x13  //DEVICE CONTROL 3   ^S  Device control
#define ASCII_DC4   0x14  //DEVICE CONTROL 4   ^T  Device control (stop)

#define ASCII_NAK   0x15  //NEGATIVE ACKNOWLEDGE      ^U  Error
#define ASCII_SYN   0x16  //SYNCHRONOUS IDLE          ^V  Synchronous idle
#define ASCII_ETB   0x17  //END OF TRANSMITION BLOCK  ^W  Logical end of media
#define ASCII_CAN   0x18  //CANCEL                    ^X  Separator (information)
#define ASCII_EM    0x19  //END OF MEDIUM             ^Y  Separator (information)
#define ASCII_SUB   0x1A  //SUBSTITUTE                ^Z  Separator (information)
#define ASCII_ESC   0x1B  //ESCAPE                    ^[  Separator (information)
#define ASCII_FS    0x1C  //FILE SEPARATOR            ^\  Separator (information)
#define ASCII_GS    0x1D  //GROUP SEPARATOR           ^]  Separator (information)
#define ASCII_RS    0x1E  //RECORD SEPARATOR          ^^  Separator (information)
#define ASCII_US    0x1F  //UNIT SEPARATOR            ^_  Separator (information)
#define ASCII_SPACE 0x20  //SPACE

//usando hexa
#define ASCII_EXCLAM     0x21 /* Exclamation mark (!) */
#define ASCII_QUOTE      0x22 /* Quotation mark (") */
#define ASCII_NUMBER     0x23 /* Number sign (#) */
#define ASCII_HASH       0x23 /* Hash (#) */
#define ASCII_DOLLAR     0x24 /* Dollar sign ($) */
#define ASCII_PERCENT    0x25 /* Percent sign (%) */
#define ASCII_AMPERSAND  0x26 /* Ampersand (&) */
#define ASCII_RSQUOTE    0x27 /* Closing single quote (') */
#define ASCII_APOSTROPHE 0x27 /* Apostrophe (') */
#define ASCII_LPAREN     0x28 /* Opening parenthesis (() */
#define ASCII_RPAREN     0x29 /* Closing parenthesis ()) */
#define ASCII_ASTERISK   0x2a /* Asterisk (*) */
#define ASCII_PLUS       0x2b /* Plus sign (+) */
#define ASCII_COMMA      0x2c /* Comma (,) */
#define ASCII_HYPHEN     0x2d /* Hyphen (-) */
#define ASCII_DASH       0x2d /* Dash (-) */
#define ASCII_MINUS      0x2d /* Minus sign (-) */
#define ASCII_PERIOD     0x2e /* Period (.) */
#define ASCII_SLASH      0x2f /* Forward Slash (/) */
#define ASCII_DIVIDE     0x2f /* Divide (/) */

#define ASCII_0          0x30 /* Numbers */
#define ASCII_1          0x31 /* "     " */
#define ASCII_2          0x32 /* "     " */
#define ASCII_3          0x33 /* "     " */
#define ASCII_4          0x34 /* "     " */
#define ASCII_5          0x35 /* "     " */
#define ASCII_6          0x36 /* "     " */
#define ASCII_7          0x37 /* "     " */
#define ASCII_8          0x38 /* "     " */
#define ASCII_9          0x39 /* "     " */
#define ASCII_COLON      0x3a /* Colon (:) */
#define ASCII_SEMICOLON  0x3b /* Semicolon (;) */
#define ASCII_LT         0x3c /* Less than (<) */
#define ASCII_EQUAL      0x3d /* Equal (=) */
#define ASCII_GT         0x3e /* Greater than (>) */
#define ASCII_QUESTION   0x3f /* Question mark (?) */
#define ASCII_AT         0x40 /* At sign (@) */

#define ASCII_A 0x41
#define ASCII_B 0x42
#define ASCII_C 0x43
#define ASCII_D 0x44
#define ASCII_E 0x45
#define ASCII_F 0x46
#define ASCII_G 0x47
#define ASCII_H 0x48
#define ASCII_I 0x49
#define ASCII_J 0x4A
#define ASCII_K 0x4B
#define ASCII_L 0x4C
#define ASCII_M 0x4D
#define ASCII_N 0x4E
#define ASCII_O 0x4F
#define ASCII_P 0x50
#define ASCII_Q 0x51
#define ASCII_R 0x52
#define ASCII_S 0x53
#define ASCII_T 0x54
#define ASCII_U 0x55
#define ASCII_V 0x56
#define ASCII_W 0x57
#define ASCII_X 0x58
#define ASCII_Y 0x59
#define ASCII_Z 0x5A


#define ASCII_LBRACKET   0x5b /* Left bracket ([) */
#define ASCII_BACKSLASH  0x5c /* Back slash (\) */
#define ASCII_RBRACKET   0x5d /* Right bracket (]) */
#define ASCII_CARET      0x5e /* Caret (^) */
#define ASCII_CIRCUMFLEX 0x5e /* Circumflex (^) */
#define ASCII_UNDERSCORE 0x5f /* Underscore (_) */
	
#define ASCII_RSQUOT     0x60 /* Closing single quote */

#define ASCII_a 0x61
#define ASCII_b 0x62
#define ASCII_c 0x63
#define ASCII_d 0x64
#define ASCII_e 0x65
#define ASCII_f 0x66
#define ASCII_g 0x67
#define ASCII_h 0x68
#define ASCII_i 0x69
#define ASCII_j 0x6A
#define ASCII_k 0x6B
#define ASCII_l 0x6C
#define ASCII_m 0x6D
#define ASCII_n 0x6E
#define ASCII_o 0x6F
#define ASCII_p 0x70
#define ASCII_q 0x71
#define ASCII_r 0x72
#define ASCII_s 0x73
#define ASCII_t 0x74
#define ASCII_u 0x75
#define ASCII_v 0x76
#define ASCII_w 0x77
#define ASCII_x 0x78
#define ASCII_y 0x79
#define ASCII_z 0x7A

#define ASCII_LBRACE     0x7b /* Left brace ({) */
#define ASCII_VERTBAR    0x7c /* Vertical bar (|) */
#define ASCII_PIPE       0x7c /* Pipe (|) */
#define ASCII_RBRACE     0x7d /* Right brace (}) */
#define ASCII_TILDE      0x7e /* Tilde (~) */
#define ASCII_DEL        0x7f /* Delete (rubout) */

#endif    



