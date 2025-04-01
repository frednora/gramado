// msgcode.h
// This is a place for the message codes 
// in the base kernel.
// These numbers are connected with the number in
// the display server (libgws/gramland)
// 2023 - Created by Fred Nora.

#ifndef __MSG_CODE_H
#define __MSG_CODE_H    1

//
// Message codes
//

#define MSG_NULL  0 
 
// Window (1-19)  
#define MSG_CREATE        1
#define MSG_DESTROY       2
#define MSG_MOVE          3 
#define MSG_SIZE          4
#define MSG_RESIZE        5
//#define MSG_OPEN        6
#define MSG_CLOSE         7
#define MSG_PAINT         8
#define MSG_SETFOCUS      9
#define MSG_KILLFOCUS     10
#define MSG_ACTIVATE      11
#define MSG_SHOWWINDOW    12 
#define MSG_SETCURSOR     13
#define MSG_HIDE          14  // ?? (MIN)
//#define MSG_MINIMIZE    14  // ?? (MIN)
#define MSG_MAXIMIZE      15
#define MSG_RESTORE       16
#define MSG_SHOWDEFAULT   17

// Keyboard (20~23)
#define MSG_KEYDOWN       20
#define MSG_KEYUP         21
#define MSG_SYSKEYDOWN    22
#define MSG_SYSKEYUP      23

// Mouse (30~39)
#define MSG_MOUSEKEYDOWN     30
#define MSG_MOUSEKEYUP       31
#define MSG_MOUSEBUTTONDOWN  30
#define MSG_MOUSEBUTTONUP    31 
#define MSG_MOUSEMOVE        32
#define MSG_MOUSEOVER        33
#define MSG_MOUSEWHEEL       34
#define MSG_MOUSEPRESSED     35
#define MSG_MOUSERELEASED    36
#define MSG_MOUSECLICKED     37
#define MSG_MOUSEENTERED     38    //?? capturou ??
#define MSG_MOUSEEXITED      39   //?? descapturou ??

// -----------

#define __MSG_COMMAND  40
#define MSG_COMMAND     __MSG_COMMAND
#define MSG_SYSCOMMAND  __MSG_COMMAND  

#define MSG_CUT           41
#define MSG_COPY          42
#define MSG_PASTE         43
#define MSG_CLEAR         44 
#define MSG_UNDO          45
#define MSG_INSERT        46

// Process and thread
#define MSG_RUN_PROCESS   47
#define MSG_RUN_THREAD    48

// #test ??
// Quando um comando é enviado para o terminal 
// ele será atendido pelo módulo /sm no procedimento de janela do sistema.
// Todas as mensagens de terminal serão atencidas 
// pelo procedimento de janela nessa mensagem.
// #bugbug: 
// Temos outro grupo de mensagems abordadndo esse tema logo abaixo.

// Terminal
#define MSG_TERMINAL_COMMAND   49
#define MSG_TERMINAL_SHUTDOWN  50
#define MSG_TERMINAL_REBOOT    51

// #deprecated
//#define MSG_DEVELOPER  52


// Um timer expirou.
#define MSG_TIMER  53

//
// Network
//

// O servidor de rede se comunica com o processo.
#define MSG_AF_INET  54
#define MSG_NET_DATA_IN  55
// O driver de network está notificando um processo em ring3.
#define MSG_NETWORK_NOTIFY_PROCESS  56

// Mouse (60~62++)
// mouse support: continuação ...
#define MSG_MOUSE_DOUBLECLICKED   60
#define MSG_MOUSE_DRAG            61
#define MSG_MOUSE_DROP            62
//...


// control+a
// Seleciona todos os elementos.
#define MSG_SELECT_ALL    70

// control+f
#define __MSG_SEARCH    71
#define MSG_SEARCH    __MSG_SEARCH
#define MSG_FIND      __MSG_SEARCH

// control+s
#define MSG_SAVE    72

// Arrow
#define MSG_CONTROL_ARROW_UP     80
#define MSG_CONTROL_ARROW_DOWN   81
#define MSG_CONTROL_ARROW_LEFT   82
#define MSG_CONTROL_ARROW_RIGHT  83


// --------------------------
// #deprecated
// (Delete these messages)
//  Terminal commands
#define MSG_TERMINALCOMMAND      100
#define TERMINALCOMMAND_PRINTCHAR 1000
//#define TERMINALCOMMAND_PRINT??? 1001
// --------------------------


// Scroll
// o evento de rolagem aconteceu ...
// O número do evento será entregue em long1.
#define MSG_HSCROLL  2000
#define MSG_VSCROLL  2001

// #test
// Notify parent about some important event that happed with the child.
// The data value can specify what happened.
#define MSG_NOTIFY_PARENT  4000


// ??
// Hotkey
#define MSG_HOTKEY  8888

// ...

// #todo
// Check the gramland/libgws protocol,
// maybe we have some more messages in common.

#endif   

