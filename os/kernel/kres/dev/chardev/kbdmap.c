
// kbdmap.c
// ring0 keymap support.
// It's working fine.
// #todo: Check the same thing in the ring3 window server.

#include <kernel.h>

// ---------------------------------
// lowercase
// NORMAL ABNT2.
// obs: 
// Isso pode ser carregável para várias configurações de teclado.
unsigned char map_abnt2[ABNT2_CHARMAP_SIZE] = {
 0,      //Scancode 0.
 033,    //1  (Esc). (\e)
 '1',    //2
 '2',    //3
 '3',
 '4',
 '5',
 '6',
 '7',
 '8',      //9
 '9',      //10
 '0',      //11
 '-',      //12
 '=',      //13
 VK_BACK,  //14 BackSpace. (0xF7) delete
 VK_TAB,   //15 TAB.        (\t)
 'q',      //16 
 'w',      //17 (alt)
 'e',
 'r',
 't',
 'y',
 'u',
 'i',
 'o',
 'p',          //25
 239, //'$',  //239   //26  VK_ACENTO_AGUDO acento agudo (00b4 @) dead_acute dead_grave
 '[',          //27   bracketleft   braceleft
 VK_RETURN,    //28 [Enter].
 VK_LCONTROL,  //29 ms right control. ??
 'a',          //30 (O a minúsculo é 30 em abnt 2).
 's',          //31
 'd',          //32
 'f',
 'g',
 'h',
 'j',
 'k',
 'l',        //38 
 135,        //39 00e7 VK_CCEDILHA ccedilla
 '~',        //40 dead_tilde dead_circumflex
 '\'',       //41 (") (espaço) (') dead_acute dead_diaeresis 
 VK_LSHIFT,  //42 Shift.  
 ']',        //43 bracketright braceright asciitilde Control_bracketright 
 'z',        //44 
 'x',
 'c',
 'v',
 'b',
 'n',
 'm',          //50 
 ',',          //51 comma  less
 '.',          //52 period greater 
 ';',          //53  numlockdesligado=(;) numlockligado=(/)abnt2.
 VK_RSHIFT,    //54 Right Shift.
 '*',          //55 abnt2(NUM *) (NUM lock??).(printscreen(abnt2))
 VK_ALTGR,     //56 abnt2=(Alt). VK_ALTGR (@todo)
 ' ',          //57 Space. VK_SPACE
 VK_CAPITAL,   //58 Caps Lock.
 VK_F1,        //59 F1. abnt2=(F1)
 VK_F2,        //60 F2.(abnt)
 VK_F3,        //61 F3.(abnt)
 VK_F4,        //62 F4.(abnt)
 VK_F5,        //63 F5.(abnt)
 VK_F6,        //64 F6.(abnt)
 VK_F7,        //65 F7.(abnt)
 VK_F8,        //66 F8.(abnt)
 VK_F9,        //67 F9.(abnt)
 VK_F10,       //68 F10.(abnt)
 VK_PAUSE,      //69 abnt2=(PAUSE). (num lock) abnt2=VK_NUMLOCK,
 VK_SCROLL,     //70 abnt2(Scroll Lock) abnt2=break.
 VK_HOME,      //'7',    //71 abnt2=(7). abnt2=home
 VK_ARROW_UP,  //'8',    //72       >>> (8). abnt2=up
 VK_PAGEUP,  //'9',    //73       (9). abnt2=pgup
 '-',    //74        -
 VK_ARROW_LEFT,  //'4',    //75       >>> (4). abnt2=left 
 '5',    //76       (5).
 VK_ARROW_RIGHT,  //'6',    //77       >>> (6). abnt2=right
 '+',    //78       +
 VK_END,  //'1',    //79       (1). abnt2=end  
 VK_ARROW_DOWN,  //'2',    //80       >>> (2). abnt2=down
 VK_PAGEDOWN,  //'3',    //81       (3). abnt2=pgdn (q).     
 VK_ABNT2_INSERT,  //'0',    //82       (0). abnt2=Ins
 VK_ABNT2_DELETE,  //VK_DELETE_TECLADONUMERICO,    //83 abnt2=(,) abnt2=(Num DEL) (abnt).(´)
VK_PRINT,   //84 abnt2=(SysRq) PRINT SCREEN E sys req (abnt).
'.',        //85
'\\',       //86 abnt2=(\). abnt2=(help) backslash  bar
VK_F11,     //87 abnt2=(F11). 
VK_F12,     //88 abnt2=(F12).
'/',        //89 slash question /?
VK_F14,     //90
VK_LWIN,               //91 abnt2=(Left Windows)  (right WinKey) ([).
VK_RWIN,               //92 abnt2=(Right Windows) (left WinKey) (\).
VK_CONTROL_MENU,       //93 abnt2=(Application). CONTROL MENU, (]).
VK_F18,       // 94
VK_F19,       // 95
'`',          // 96 (0) (`)Acento grave.(enter)
VK_LCONTROL,  // 97 (1) (a) Minúscula. control
'/',          // 98 (2) (/) KP_Divide
VK_F23,       // 99 (3) VoidSymbol  
VK_ALTGR,     //100 (4) AltGr
VK_BREAK,     //101 (5) 0xE9
VK_FIND,      //102 (6) (\).
VK_UP,        //103 (7)
VK_PRIOR,     //104 (8)
VK_LEFT,      //105 (9) 0xED
VK_RIGHT,     //106 *
VK_SELECT,    //107 +
VK_DOWN,      //108 RETURN.
VK_NEXT,      //109 -
VK_INSERT,    // 
VK_REMOVE,    //111 (/). 0xF3
VK_F1,   //112 (F1). MACRO
VK_F2,   //113 (F2) F13
VK_F3,   //114 (F3) F14
'/',     //115   abnt2=(/)(F4). HELP
VK_F5,   //116 (F5) DO
VK_F6,   //117 (F6) F17
VK_F7,   //118 (F7) (esc) KP_MinPlus
VK_PAUSE,   //119 (F8) Pause 
VK_F9,   //120 (F9)
VK_F10,  //121 (F10)
VK_F11,  //122 (F11)
VK_F12,  //123 (F12)
VK_F13,  //124 abnt2=F13 
VK_F14,  //125 abnt2=F14
'.',     //126 abnt=(.) abnt=(F15) VK_ACENTO_TIL   
VK_F16,  //127          abnt=(F16) 
VK_F17,  //128
VK_F18,  //129
VK_F19,  //130
VK_F20,  //131
VK_F21,  //132
VK_F22,  //133
VK_F23,  //134
VK_F24,  //135
};

// ---------------------------------------
// uppercase
// maiúsculas
// SHIFT ABNT2.
// +Combinação com shift pressionado.
// +Com capslock acionado.
unsigned char shift_abnt2[ABNT2_CHARMAP_SIZE] = {
0, 
033,   // "
'!',   //2 
'@',   //3
'#',   //4
'$',   //5
'%',   //6
168, //VK_ABNT2_TREMA,  //'$',   //VK_ABNT2_TREMA, //168,   //7 (00a8 @) (trema)
'&',   //8
'*',   //9
'(',   //10
')',   //11
'_',   //12
'+',   //13 
VK_BACK,  // 0x0E     0177,
VK_TAB,   // 0x0F     '\t',   //15 TAB
'Q',    //16
'W',    //17
'E',    //18
'R',    //19
'T',    //20
'Y',    //21
'U',    //22
'I',    //23
'O',    //24
'P',    //25   (FAIL!)
'`',    //26   (ASCII 96) (grave accent)
'{',    //27
VK_RETURN,   // 28
CTL,         // 29 
'A',  //30
'S',
'D',
'F',
'G',
'H',  //FAIL
'J',
'K',  //FAIL
'L',  //38
128,  //39 (00c7) Ç
'^',  //40             //OK
'"',  //41 abnt2=(")
SHF,
 '}',  //43            //OK
'Z',   //44 
'X',
'C',
'V',
'B',
'N',
'M',  //50    //FAIL
'<',  //51 abnt2
'>',  //52 abnt2
':',  //53 abnt2
SHF,  //54 
CTL,  //55
ALT,  //56
' ',  //57
CPS|L,  //58
0,      //59
0,      //60
' ',    //61
0,   //62
0,   //63
0,   //64
0,   //65
0,   //66
0,   //67
0,   //68
NUM|L,  //69
STP|L,  //70
VK_HOME,  //'7',    //71
VK_ARROW_UP,  //'8',    //72
VK_PAGEUP,  //'9',    //73
'-',    //74
VK_ARROW_LEFT,  //'4',    //75
'5',    //76
VK_ARROW_RIGHT,  //'6',    //77
'+',    //78
VK_END,  //'1',    //79
VK_ARROW_DOWN,  //'2',    //80
VK_PAGEDOWN,  //'3',    //81
VK_ABNT2_INSERT,  //'0',    //82
VK_ABNT2_DELETE,  //',',    //83 abnt2(,)
0,      //84
0,      //85
'|',    //86 abnt2(|)
0,  //87
0,  //88
0,  //89
0,  //90
0,  //91
0,  //92
0,  //93
0,  //94
0,  //95
0,  //96
0,  //97
0,  //98
0,  //99
0,  //100
0,  //101
0,  //102
0,  //103
0,  //104
0,  //105
0,  //106
0,  //107
0,  //108
0,  //109
0,  //110
0,  //111
0,  //112
0,  //113
0,  //114
'?',  //115 abnt2(?)
0,  //116
0,  //117
0,  //118
0,  //119
0,  //120
0,  //121
0,  //122
0,  //123
0,  //124
0,  //125
'.',  //126 abnt2(.).
0,
};

// ------------------------------
// control + key
// CONTROL ABNT2.
unsigned char ctl_abnt2[ABNT2_CHARMAP_SIZE] = {

0,     //0
033,   //1
'!',   //2
000,   //3
'#',   //4
'$',   //5
'%',   //6
036,   //7
'&',   //8
'*',   //9
'(',   //10
')',   //11
ASCII_US,     //12   ^_
'+',          //13 
034,          //14
'\177',       //15

// No spot 17 colocamos o valor 17 (ASCII_DC1).
ASCII_DC1,    //16  ^Q
ASCII_ETB,    //17  ^W
ASCII_ENQ,    //18  ^E  
ASCII_SO,     //19  ^R
ASCII_DC4,    //20  ^T
ASCII_EM,     //21  ^Y
ASCII_NAK,    //22  ^U
ASCII_HT,     //23  ^I
ASCII_SI,     //24  ^O
ASCII_DLE,    //25  ^P
033,          //26
ASCII_ESC,    //27  ^[
'\r',         //28
CTL,          //29

// No spot 31 comocamos o valor 31 (ASCII_SOH)
ASCII_SOH,    //30 ^A
ASCII_DC3,    //31 ^S
ASCII_EOT,    //32 ^D
ASCII_ACK,    //33 ^F
ASCII_BEL,    //34 ^G
ASCII_BS,     //35 ^H
ASCII_LF,     //36 ^J
ASCII_VT,     //37 ^K
ASCII_FF,     //38 ^L
';',          //39
ASCII_RS,     //40  ^^
'`',          //41
SHF,          //42
ASCII_GS,     //43 ^]

ASCII_SUB,    //44 ^Z
ASCII_CAN,    //45 ^X
ASCII_ETX,    //46 ^C
ASCII_SYN,    //47 ^V
ASCII_STX,    //48 ^B
ASCII_SO,     //49 ^N
ASCII_CR,     //50 ^M

'<',    //51 
'>',    //52
'?',    //53
SHF,    //54
'*',    //55
ALT,    //56
' ',    //57
CPS|L,  //58
0,      //59
0,      //60
' ',    //61
0,      //62
0,      //63
CPS|L,  //64
0,      //65 
0,   //66
0,   //67
0,   //68
0,   //69
0,   //70
0,   //71
0,   //72
0,   //73
0,   //74
0,   //75
0,   //76
0,   //77
0,   //78
0,   //79
0,   //80
0,   //81
0,   //82
0,   //83
0,   //84
0,   //85
ASCII_FS,   //86 
0,    //87
0,    //88
0,    //89
033,  //90
'7',
'4',
'1',
0, 
NUM|L,
'8',
'5',
'2',
0,
STP|L,
'9',
'6',
'3',
'.',
0,
'*',
'-',
'+',
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
0,
0,
0,
0,
0,
0,
0,
0,
0,
};







