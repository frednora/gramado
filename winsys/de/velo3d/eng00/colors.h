
// colors.h
// Server-side system colors.

#ifndef ____COLORS_H
#define ____COLORS_H    1


// #test
/*
0 
| 1 
| | 2 
| | | 3 
| | | | 4 
| | | | | 5 
| | | | | | 6 
| | | | | | | 7 
| | | | | | | |
| | | | | | | 8 
| | | | | | 9 
| | | | | A 
| | | | B 
| | | C 
| | D 
| E 
F

0x000000,  // level 7: dark black
0x0000BB,  // level 6: dark blue
0x00BB00,  // level 5: dark green
0xBB0000,  // level 4: dark red
0x00BBBB,  // level 3: dark cyan
0xBB00BB,  // level 2: dark magenta
0xBBBB00,  // level 1: dark yellow
0xBBBBBB,  // level 0: dark white
-------
0x444444,  // level 0: light black
0x4444FF,  // level 1: light blue
0x44FF44,  // level 2: light green
0xFF4444,  // level 3: light red
0x44FFFF,  // level 4: light cyan
0xFF44FF,  // level 5: light magenta
0xFFFF44,  // level 6: light yellow
0xFFFFFF   // level 7: light white
*/

//-------
#define GRCOLOR_DARKBLACK     0x000000  // level 7: dark black
#define GRCOLOR_DARKBLUE      0x0000BB  // level 6: dark blue
#define GRCOLOR_DARKGREEN     0x00BB00  // level 5: dark green
#define GRCOLOR_DARKRED       0xBB0000  // level 4: dark red
#define GRCOLOR_DARKCYAN      0x00BBBB  // level 3: dark cyan
#define GRCOLOR_DARKMAGENTA   0xBB00BB  // level 2: dark magenta
#define GRCOLOR_DARKYELLOW    0xBBBB00  // level 1: dark yellow
#define GRCOLOR_DARKWHITE     0xBBBBBB  // level 0: dark white

#define GRCOLOR_LIGHTBLACK    0x444444  // level 0: light black
#define GRCOLOR_LIGHTBLUE     0x4444FF  // level 1: light blue
#define GRCOLOR_LIGHTGREEN    0x44FF44  // level 2: light green
#define GRCOLOR_LIGHTRED      0xFF4444  // level 3: light red
#define GRCOLOR_LIGHTCYAN     0x44FFFF  // level 4: light cyan
#define GRCOLOR_LIGHTMAGENTA  0xFF44FF  // level 5: light magenta
#define GRCOLOR_LIGHTYELLOW   0xFFFF44  // level 6: light yellow
#define GRCOLOR_LIGHTWHITE    0xFFFFFF  // level 7: light white
//-------

// #todo: We can define a default transparent color,
// just like PINK
// #define COLOR_TRANSPARENT ?
// #define COLOR_TRANSPARENT COLOR_PINK

/*
 losethos
public BGREntry vga_palette4_std[16] = {
    0x000000,0x0000AA,0x00AA00,0x00AAAA,
    0xAA0000,0xAA00AA,0xAA5500,0xAAAAAA,
    0x555555,0x5555FF,0x55FF55,0x55FFFF,
    0xFF5555,0xFF55FF,0xFFFF55,0xFFFFFF };
*/

/*
losethos
public BGREntry vga_palette4_gray[16] = {
    0x000000,0x111111,0x222222,0x333333,
    0x444444,0x555555,0x666666,0x777777,
    0x888888,0x999999,0xAAAAAA,0xBBBBBB,
    0xCCCCCC,0xDDDDDD,0xEEEEEE,0xFFFFFF };
*/

// Basics
#define rgbBlack    0x000000
#define rgbBlue     0x0000FF
#define rgbCyan     0x00FFFF
#define rgbGreen    0x00FF00
#define rgbMagenta  0xFF00FF
#define rgbRed      0xFF0000
#define rgbYellow   0xFFFF00
#define rgbWhite    0xFFFFFF 

// Blue
#define COLOR_BLUE1        0x067AB5
#define COLOR_BLUE2CYAN    0x1BA1E2
#define COLOR_BLUE3COBALT  0x0050EF
#define COLOR_BLUE4INDIGO  0x6A00FF

// Gray
#define xCOLOR_BLACK  0x000000  //preto
#define xCOLOR_GRAY1  0x202020  //cinza mais escuro
#define xCOLOR_GRAY2  0x404040  //cinza
#define xCOLOR_GRAY3  0x606060  //cinza
#define xCOLOR_GRAY4  0x808080  //cinza
#define xCOLOR_GRAY5  0xa0a0a0  //cinza
#define xCOLOR_GRAY6  0xc0c0c0  //cinza
#define xCOLOR_GRAY7  0xe0e0e0  //cinza mais clarinho
#define xCOLOR_WHITE  0xffffff  //branco  

// Gray Win23
#define COLOR_LITBUTTON  0xE0E0E0
#define COLOR_LTGRAY     0xC0C0C0
#define COLOR_GRAY       0x808080 

// Gray shades
#define COLOR_GRAY_SHADES_1  0x1D1D1D
#define COLOR_GRAY_SHADES_2  0x333333
#define COLOR_GRAY_SHADES_3  0x4C4C4C
#define COLOR_GRAY_SHADES_4  0x656565
#define COLOR_GRAY_SHADES_5  0x808080

// Black
//50%   #808080
//45%   #737373
//40%   #666666
//35%   #595959
//30%   #4d4d4d
//25%   #404040
//20%   #333333
//15%   #262626
//10%   #1a1a1a
//5%    #0d0d0d
//0%    #000000

// w2k - system colors
#define COLOR_TEST_0  0x8080FF  // COLOR_BACKGROUND
#define COLOR_TEST_1  0x80FFFF  // COLOR_APPWORKSPACE, MDIWINDOW 
#define COLOR_TEST_2  0x80FF80  // COLOR_WINDOW, CLIENT
#define COLOR_TEST_3  0x80FF00  // COLOR_WINDOWTEXT, CLIENTTEXT 
#define COLOR_TEST_4  0xFFFF80  // COLOR_MENU, MENUBAR
#define COLOR_TEST_5  0xFF8000  // COLOR_MENUTEXT (laranjado+-)
#define COLOR_TEST_6  0xC080FF  // COLOR_ACTIVECAPTION, MYCAPTION
#define COLOR_TEST_7  0xFF80FF  // COLOR_INACTIVECAPTION, CAPTION2
#define COLOR_TEST_8  0x0000FF  // COLOR_CAPTIONTEXT, CAPTIONTEXT(azul)highlight
#define COLOR_TEST_9  0x00FFFF  // COLOR_ACTIVEBORDER, BORDER
#define COLOR_TEST_10 0x00FF80  // COLOR_INACTIVEBORDER, BORDER2
#define COLOR_TEST_11 0x40FF00  // COLOR_WINDOWFRAME
#define COLOR_TEST_12 0xFFFF00  // COLOR_SCROLLBAR (amarelo)
#define COLOR_TEST_13 0xC08000  // COLOR_BTNFACE
#define COLOR_TEST_14 0xC08080  // COLOR_BTNSHADOW (vinho)
#define COLOR_TEST_15 0xFF00FF  // COLOR_BTNTEXT (rosa)
#define COLOR_TEST_16 0x404080  // COLOR_HIGHLIGHT ~ COLOR_GRAYTEXT (azul escuro)
#define COLOR_TEST_17 0x4080FF  // COLOR_HIGHLIGHT
#define COLOR_TEST_18 0x00FF00  // COLOR_HIGHLIGHTTEXT (verde)
#define COLOR_TEST_19 0x808000  // COLOR_INACTIVECAPTIONTEXT
#define COLOR_TEST_20 0x804000  // COLOR_BTNHIGHLIGHT
#define COLOR_TEST_21 0xFF8080 
#define COLOR_TEST_22 0x400080 
#define COLOR_TEST_23 0x8000FF
#define COLOR_TEST_24 0x000080  //COLOR_HIGHLIGHT ~ hot track (azul)
#define COLOR_TEST_25 0x0080FF 
#define COLOR_TEST_26 0x008000 
#define COLOR_TEST_27 0x408000 
#define COLOR_TEST_28 0xFF0000  //red
#define COLOR_TEST_29 0xA00000 
#define COLOR_TEST_30 0x800080 
#define COLOR_TEST_31 0xFF0080
#define COLOR_TEST_32 0x000040
#define COLOR_TEST_33 0x004080 
#define COLOR_TEST_34 0x004000 
#define COLOR_TEST_35 0x404000 
#define COLOR_TEST_36 0x800000 
#define COLOR_TEST_37 0x400000 
#define COLOR_TEST_38 0x400040
#define COLOR_TEST_39 0x800040
#define COLOR_TEST_40 0x000000  //COLOR_BTNSHADOW ~ preto text
#define COLOR_TEST_41 0x008080  //vrde escuro
#define COLOR_TEST_42 0x408080  //vrde escuro
#define COLOR_TEST_43 0x808080  //COLOR_APPWORKSPACE (cinza)
#define COLOR_TEST_44 0x808040 
#define COLOR_TEST_45 0xC0C0C0  //silver
#define COLOR_TEST_46 0x400040 
#define COLOR_TEST_47 0x3366FF  //azul claro

// Misc
#define COLOR_ALICEBLUE           0xF0F8FF
#define COLOR_ANTIQUEWHITE        0xFAEBD7
#define COLOR_AQUA                0x00FFFF
#define COLOR_AQUAMARINE          0x7FFFD4 //verde claro.
#define COLOR_AZURE               0xF0FFFF //azul, quase branco. 
#define COLOR_BEIGE               0xF5F5DC
#define COLOR_BISQUE              0xFFE4C4
#define COLOR_BLANCHEDALMOND      0xFFEBCD
#define COLOR_BLUEVIOLET          0x8A2BE2
#define COLOR_BROWN               0xA52A2A
#define COLOR_BURLYWOOD           0xDEB887
#define COLOR_CADETBLUE           0x5F9EA0
#define COLOR_CHARTREUSE          0x7FFF00
#define COLOR_CHOCOLATE           0xD2691E
#define COLOR_CORAL               0xFF7F50
#define COLOR_CORNFLOWERBLUE      0x6495ED
#define COLOR_CORNSILK            0xFFF8DC
#define COLOR_CRIMSON             0xDC143C
#define COLOR_CYAN                0x00FFFF
#define COLOR_DARKCYAN            0x008B8B
#define COLOR_DARKGOLDENROD       0xB8860B
#define COLOR_DARKGRAY            0xA9A9A9
#define COLOR_DARKKHAKI           0xBDB76B
#define COLOR_DARKMAGENTA         0x8B008B
#define COLOR_DARKOLIVEGREEN      0x556B2F
#define COLOR_DARKORANGE          0xFF8C00
#define COLOR_DARKORCHID          0x9932CC
#define COLOR_DARKSALMON          0xE9967A
#define COLOR_DARKSEAGREEN        0x8FBC8B
#define COLOR_DARKSLATEBLUE       0x483D8B
#define COLOR_DARKSLATEGRAY       0x2F4F4F
#define COLOR_DARKTURQUOISE       0x00CED1
#define COLOR_DARKVIOLET          0x9400D3
#define COLOR_DEEPPINK            0xFF1493
#define COLOR_DEEPSKYBLUE         0x00BFFF
#define COLOR_DIMGRAY             0x696969
#define COLOR_DODGERBLUE          0x1E90FF
#define COLOR_FIREBRICK           0xB22222
#define COLOR_FLORALWHITE         0xFFFAF0
#define COLOR_FORESTGREEN         0x228B22
#define COLOR_FUCHSIA             0xFF00FF
#define COLOR_GAINSBORO           0xDCDCDC
#define COLOR_GHOSTWHITE          0xF8F8FF
#define COLOR_GOLD                0xFFD700
#define COLOR_GOLDENROD           0xDAA520
#define COLOR_GRADIENTACTIVECAPTION   0xB9D1EA
#define COLOR_GRADIENTINACTIVECAPTION 0xD7E4F2
#define COLOR_GREENYELLOW         0xADFF2F
#define COLOR_HONEYDEW            0xF0FFF0
#define COLOR_HOTPINK             0xFF69B4 //rosa chiclete.
#define COLOR_HOTTRACK            0x0066CC //azul legal.
#define COLOR_INDIANRED           0xCD5C5C
#define COLOR_INDIGO              0x4B0082 //violeta
#define COLOR_IVORY               0xFFFFF0
#define COLOR_KHAKI               0xF0E68C
#define COLOR_LAVENDER            0xE6E6FA
#define COLOR_LAVENDERBLUSH       0xFFF0F5
#define COLOR_LAWNGREEN           0x7CFC00
#define COLOR_LEMONCHIFFON        0xFFFACD
#define COLOR_LIGHTCORAL          0xF08080
#define COLOR_LIGHTCYAN           0xE0FFFF
#define COLOR_LIGHTGOLDENRODYELLOW 0xFAFAD2
#define COLOR_LIGHTPINK           0xFFB6C1
#define COLOR_LIGHTSALMON         0xFFA07A
#define COLOR_LIGHTSEAGREEN       0x20B2AA
#define COLOR_LIGHTSKYBLUE        0x87CEFA  //azul clarinho
#define COLOR_LIGHTSLATEGRAY      0x778899
#define COLOR_LIGHTSTEELBLUE      0xB0C4DE
#define COLOR_LIGHTYELLOW         0xFFFFE0
#define COLOR_LIME                0x00FF00
#define COLOR_LIMEGREEN           0x32CD32
#define COLOR_LINEN               0xFAF0E6
#define COLOR_MAGENTA             0xFF00FF
#define COLOR_MAROON              0x800000
#define COLOR_MEDIUMAQUAMARINE    0x66CDAA
#define COLOR_MEDIUMBLUE          0x0000CD
#define COLOR_MEDIUMORCHID        0xBA55D3
#define COLOR_MEDIUMPURPLE        0x9370DB
#define COLOR_MEDIUMSEAGREEN      0x3CB371
#define COLOR_MEDIUMSLATEBLUE     0x7B68EE
#define COLOR_MEDIUMSPRINGGREEN   0x00FA9A
#define COLOR_MEDIUMTURQUOISE     0x48D1CC
#define COLOR_MEDIUMVIOLETRED     0xC71585
#define COLOR_MIDNIGHTBLUE        0x191970
#define COLOR_MINTCREAM           0xF5FFFA
#define COLOR_MISTYROSE           0xFFE4E1
#define COLOR_MOCCASIN            0xFFE4B5
#define COLOR_NAVAJOWHITE         0xFFDEAD //branco areioso.
#define COLOR_NAVY                0x000080
#define COLOR_OLDLACE             0xFDF5E6
#define COLOR_OLIVE               0x808000
#define COLOR_OLIVEDRAB           0x6B8E23
#define COLOR_ORANGE              0xFFA500
#define COLOR_ORANGERED           0xFF4500
#define COLOR_ORCHID              0xDA70D6
#define COLOR_PALEGOLDENROD       0xEEE8AA
#define COLOR_PALEGREEN           0x98FB98
#define COLOR_PALETURQUOISE       0xAFEEEE
#define COLOR_PALEVIOLETRED       0xDB7093
#define COLOR_PAPAYAWHIP          0xFFEFD5
#define COLOR_PEACHPUFF           0xFFDAB9
#define COLOR_PERU                0xCD853F
#define COLOR_PINK                0xFFC0CB
#define COLOR_PLUM                0xDDA0DD
#define COLOR_POWDERBLUE          0xB0E0E6
#define COLOR_PURPLE              0x800080
#define COLOR_ROSYBROWN           0xBC8F8F
#define COLOR_ROYALBLUE           0x4169E1
#define COLOR_SADDLEBROWN         0x8B4513
#define COLOR_SALMON              0xFA8072
#define COLOR_SANDYBROWN          0xF4A460
#define COLOR_SCROLLBAR           0xC8C8C8
#define COLOR_SEAGREEN            0x2E8B57
#define COLOR_SEASHELL            0xFFF5EE  //branco sujinho
#define COLOR_SIENNA              0xA0522D
#define COLOR_SKYBLUE             0x87CEEB  //azul claro
#define COLOR_SLATEBLUE           0x6A5ACD  //azul/violeta
#define COLOR_SLATEGRAY           0x708090
#define COLOR_SNOW                0xFFFAFA //branco
#define COLOR_SPRINGGREEN         0x00FF7F
#define COLOR_STEELBLUE           0x4682B4 //azul legal.
#define COLOR_TAN                 0xD2B48C
#define COLOR_TEAL                0x008080
#define COLOR_THISTLE             0xD8BFD8
#define COLOR_TOMATO              0xFF6347
#define COLOR_TRANSPARENT         0xFFFFFF
#define COLOR_TURQUOISE           0x40E0D0
#define COLOR_VIOLET              0xEE82EE
#define COLOR_WHEAT               0xF5DEB3
#define COLOR_WHITESMOKE          0xF5F5F5 //branco enfumaçado.
#define COLOR_YELLOW              0xFFFF00
#define COLOR_YELLOWGREEN         0x9ACD32
#define STEALTH_ORANGE            0xFF8800
#define STEALTH_OLIVE             0x666600
#define STEALTH_GREEN             0x33DD11
#define STEALTH_PINK              0xFF22EE
#define STEALTH_BLUE              0x0077BB

// Text support
#define COLOR_TEXT2  0x00404040   //25%
#define COLOR_TEXT1  0x00333333   //20% 
#define COLOR_TEXT   COLOR_TEXT1 
//#define COLOR_TEXT  0x00000000  

// Misc
#define COLOR_WINDOW              0x00FFFFFF
#define COLOR_BLACK               0x000000
#define COLOR_WHITE               0xFFFFFF 
#define COLOR_RED                 0xFF0000 
#define COLOR_GREEN               0x00FF00
#define COLOR_BLUE                0x0000FF
#define COLOR_DARKRED             0x8B0000
#define COLOR_DARKGREEN           0x006400
#define COLOR_DARKBLUE            0x00008B
//#define COLOR_GRAY                0x808080
#define COLOR_GREY                0xAFAFAF
#define COLOR_LIGHTGRAY           0xD3D3D3
#define COLOR_LIGHTGREEN          0x90EE90
#define COLOR_LIGHTBLUE           0xADD8E6
#define COLOR_SILVER              0xC0C0C0
 
// Window 
#define COLOR_ACTIVEBORDER        0xB4B4B4
#define COLOR_ACTIVECAPTION       0x99B4D1
#define COLOR_ACTIVECAPTION_TEXT  0x000000
#define COLOR_APPWORKSPACE  COLOR_SILVER 

// Background
#define COLOR_BACKGROUND      0x008080  
#define COLOR_BACKGROUNDTEXT  COLOR_TEXT  

// Border
#define COLOR_BORDER  COLOR_SILVER 

// Button
#define COLOR_BUTTONFACE           0xF0F0F0
#define COLOR_BUTTONFACE2          0xE0E0E0
#define COLOR_BUTTONFACE3          0xD0D0D0
#define COLOR_BUTTONHIGHLIGHT      0xFFFFFF
#define COLOR_BUTTONHIGHLIGHT2     0x404040
#define COLOR_BUTTONHIGHLIGHTTEXT  COLOR_WHITE
#define COLOR_BUTTONHIGHLIGHTTEXT2 COLOR_WHITE //@todo: branco menos intenso.
#define COLOR_BUTTONSHADOW         0xA0A0A0
#define COLOR_BUTTONSHADOW2        0x303030  
#define COLOR_BUTTONTEXT           COLOR_TEXT

// Controls
#define COLOR_CONTROL               0xF0F0F0
#define COLOR_CONTROLTEXT           COLOR_TEXT
#define COLOR_CONTROLDARK           0xA0A0A0
#define COLOR_CONTROLDARKDARK       0x696969
#define COLOR_CONTROLLIGHT          0xE3E3E3
#define COLOR_CONTROLLIGHTLIGHT     0xFFFFFF
#define COLOR_CONTROLLIGHTLIGHTTEXT COLOR_WHITE

// Desktop
#define COLOR_DESKTOP       0x000000

// Topbar
#define COLOR_FOCUS_TOPBAR  0x7FFF00 //verde claro

// Text
#define COLOR_GRAYTEXT         0x808080
#define COLOR_HIGHLIGHT        0x3399FF  //azul claro
#define COLOR_HIGHLIGHTTEXT    0xFFFFFF

// Border.
#define COLOR_INACTIVEBORDER       0xF4F7FC

// Title bar
#define COLOR_INACTIVECAPTION      0xBFCDDB
#define COLOR_INACTIVECAPTIONTEXT  0x434E54

// Info
#define COLOR_INFO      0xFFFFE1
#define COLOR_INFOTEXT  TEXT_COLOR

// Menu
#define COLOR_MENU      COLOR_GRAY
#define COLOR_MENUTEXT  COLOR_TEXT
#define COLOR_MENUITEM      COLOR_WHITE
#define COLOR_MENUITEMTEXT  COLOR_TEXT
#define COLOR_MENUITEMSELECTED      COLOR_BLUE
#define COLOR_MENUITEMSELECTEDTEXT  COLOR_WHITE

// Menubar
#define COLOR_MENUBAR      COLOR_GRAY
#define COLOR_MENUBARTEXT  COLOR_TEXT

// Menu
#define COLOR_MENUHIGHLIGHT      0x003399FF  //azul claro
#define COLOR_MENUHIGHLIGHTTEXT  COLOR_WHITE

// Navigation bar
#define COLOR_NAVIGATIONBAR      COLOR_BLACK
#define COLOR_NAVIGATIONBARTEXT  COLOR_WHITE

// Status bar
#define COLOR_STATUSBAR   COLOR_WINDOW   //branca
#define COLOR_STATUSBAR2  xCOLOR_GRAY6   //cinza
#define COLOR_STATUSBAR3  0x83FCFF       //verde
#define COLOR_STATUSBARTEXT      COLOR_TEXT

// Taskbar
#define COLOR_TASKBAR      COLOR_WINDOW
#define COLOR_TASKBARTEXT  COLOR_TEXT

// Titlebar
#define COLOR_TITLEBAR      COLOR_BLUE
#define COLOR_TITLEBARTEXT  COLOR_TEXT

// Topbar
#define COLOR_TOPBAR      0x83F52C
#define COLOR_TOPBARTEXT  COLOR_TEXT

#define COLOR_WINDOWFRAME  COLOR_GRAY

// Window text
#define COLOR_WINDOWTEXT COLOR_TEXT  

// Working area
#define COLOR_WORKSPACE      0x0000FF00 
#define COLOR_WORKSPACETEXT  COLOR_TEXT

// Terminal
#define COLOR_TERMINAL       COLOR_BLACK
#define COLOR_TERMINAL2      0x303030
#define COLOR_TERMINALTEXT   COLOR_WHITE
#define COLOR_TERMINALTEXT2  xCOLOR_GRAY7
//...

// Blue blackblue 
//#define COLOR_DARKBLUE   0x00008B00 //green
//#define COLOR_BLACK      0x00000000 

// Kernel background.
#define COLOR_KERNEL_BACKGROUND COLOR_BLUE

// Window 1000
#define WINDOW1000 0xF5DEB3

// Blue
#define BLUE1000   0x2d89ef
#define BLUE1001   0x0277bd
#define SKYBLUE    0xA6CAF0

// White
#define SOFT_WHITE  0xFFFBF0

#endif    



