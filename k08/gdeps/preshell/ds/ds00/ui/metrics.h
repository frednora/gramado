// metrics.h
// Some default values for window metrics for
// a consistent UI style.
// Created by Fred Nora.

// #obs:
// Um conjunto de meditas consistentes caracterizam 
// um estilo próprio.

#ifndef __UI_METRICS_H
#define __UI_METRICS_H    1

//
// Metrics - Consistent UI style
//

// pad
// The icon on title bar.
#define METRICS_ICON_LEFTPAD  2
#define METRICS_ICON_TOPPAD   2
#define METRICS_ICON_LEFT    METRICS_ICON_LEFTPAD
#define METRICS_ICON_TOP     METRICS_ICON_TOPPAD

// Border size consistente, por questão de estilo.
// Para jaleas de aplicativos. Overlapped.
// Mas o aplicativo pode customizar.
#define METRICS_BORDER_SIZE  2


//
// Taskbar
//

// Default height
// 28 = (20 +2+2 +2+2)
#define METRICS_TASKBAR_DEFAULT_HEIGHT  28

//
// Titlebar
//

// Default height
// 24 = (16 +2+2 +2+2)
//#define METRICS_TITLEBAR_DEFAULT_HEIGHT  24
#define METRICS_TITLEBAR_DEFAULT_HEIGHT  26

// 40 = (32 +2+2 +2+2)
#define METRICS_SUPERTITLEBAR_DEFAULT_HEIGHT  40
//#define METRICS_SUPERTITLEBAR_DEFAULT_HEIGHT  41

// The difference between the titlebar height and 
// the size of the button.
#define __gap_BT_TB  4
//#define __gap_BT_TB  3
//#define __gap_BT_TB  2

// Control width.
// #todo: Bigger will crash?
#define METRICS_TITLEBAR_CONTROLS_DEFAULT_WIDTH \
    (METRICS_TITLEBAR_DEFAULT_HEIGHT - __gap_BT_TB)

// Control height.
// #todo: Bigger will crash?
// Bigger will paint outside the titlebar.
// It is accepted in some cases. Just like a skin.
#define METRICS_TITLEBAR_CONTROLS_DEFAULT_HEIGHT \
    (METRICS_TITLEBAR_DEFAULT_HEIGHT - __gap_BT_TB)

// Ornament size
// Usado para explicitar se a janela é ativa ou não
// e como separador entre a barra de títulos e a segunda
// área da janela de aplicativo.
// Usado somente por overlapped window.
// #todo: Bigger will crash?
#define METRICS_TITLEBAR_ORNAMENT_SIZE  2

//
// Application window.
//

// #todo:
// Application window. Not child window.
#define METRICS_DEFAULT_MINIMUM_WINDOW_WIDTH \
    (METRICS_TITLEBAR_CONTROLS_DEFAULT_WIDTH*5)
#define METRICS_DEFAULT_MINIMUM_WINDOW_HEIGHT \
    (METRICS_TITLEBAR_CONTROLS_DEFAULT_HEIGHT*3)



//
// Editbox
//

#define METRICS_EDITBOX_MARGIN_LEFT  8
#define METRICS_EDITBOX_MARGIN_TOP  8
#define METRICS_MAX_CHARS_PER_LINE 256

// ...

#endif    

