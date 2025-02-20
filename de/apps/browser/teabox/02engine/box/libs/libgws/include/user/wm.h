
// wm.h
// This header should be included by including "gws.h".

#ifndef  __LIBGWS_WM_H
#define  __LIBGWS_WM_H    1

// #todo:
// Its a good choice using hexa instead of integer.
// We need to change it in the whole system.

//
// Window messages.
//

//window (1-19)  
#define MSG_CREATE        1
#define MSG_DESTROY       2
#define MSG_MOVE          3 
#define MSG_SIZE          4  // Sent to an app after a resize operation. 
#define MSG_RESIZE        5
//#define MSG_OPEN        6
#define MSG_CLOSE         7
#define MSG_PAINT         8
#define MSG_SETFOCUS      9
#define MSG_KILLFOCUS     10
#define MSG_ACTIVATE      11
#define MSG_SHOWWINDOW    12 
#define MSG_SETCURSOR     13
#define MSG_HIDE          14
#define MSG_MAXIMIZE      15
#define MSG_RESTORE       16
#define MSG_SHOWDEFAULT   17

// see:
// globals.h in gwssrv.bin

// usdo pelo window manager
#define GWS_SETFOCUS2     18
#define GWS_GETFOCUS2     19

// keyboard (20-29)
#define MSG_KEYDOWN       20
#define MSG_KEYUP         21
#define MSG_SYSKEYDOWN    22
#define MSG_SYSKEYUP      23

// mouse (30 - 39)
// Tem uma lista de eventos de mouse em events.h

#define MSG_MOUSEKEYDOWN  30
#define MSG_MOUSEKEYUP    31

#define MSG_MOUSEBUTTONDOWN  30
#define MSG_MOUSEBUTTONUP    31 

#define MSG_MOUSEMOVE     32
#define MSG_MOUSEOVER     33
#define MSG_MOUSEWHEEL    34

#define MSG_MOUSEPRESSED     35
#define MSG_MOUSERELEASED    36

#define MSG_MOUSECLICKED    37
#define MSG_MOUSEENTERED    38
#define MSG_MOUSEEXITED     39
//#define MSG_MOUSEMOVEBYOFFSET
//#define MSG_MOUSEMOVETOELEMENT

//outros (40 - ...)

#define MSG_COMMAND       40

#define MSG_CUT           41
#define MSG_COPY          42
#define MSG_PASTE         43
#define MSG_CLEAR         44 
#define MSG_UNDO          45
#define MSG_INSERT        46

// process and thread.
#define MSG_RUN_PROCESS   47
#define MSG_RUN_THREAD    48

// ??
// Quando um comando é enviado para o console 
// ele será atendido pelo módulo /sm no 
// procedimento de janela do sistema.
// Todas as mensagens de console serão atencidas 
// pelo procedimento de janela nessa mensagem.

#define MSG_CONSOLE_COMMAND   49
#define MSG_CONSOLE_SHUTDOWN  50
#define MSG_CONSOLE_REBOOT    51

#define MSG_DEVELOPER  52

// Timer timeout.
#define MSG_TIMER    53

// O servidor de rede se comunica com o processo.
#define MSG_AF_INET       54
#define MSG_NET_DATA_IN   55

// O driver de network está notificando um processo em ring3.
#define MSG_NETWORK_NOTIFY_PROCESS 56

// mouse support: 
// continuação ...
#define MSG_MOUSE_DOUBLECLICKED   60
#define MSG_MOUSE_DRAG            61
#define MSG_MOUSE_DROP            62
//...



//  terminal commands
#define MSG_TERMINALCOMMAND      100

//
// Socket messages
//

// #bugbug
// These are the number used by the Aurora window server.
// But its client-side library (libgws) has already defined 
// all these numbers.
// Maybe we need to synch with gws.h in libgws/.

#define MSG_GWS_HELLO               1000 

#define MSG_GWS_CREATEWINDOW        1001
#define MSG_GWS_BACKBUFFERPUTPIXEL  1002
#define MSG_GWS_BACKBUFFERHORIZONTALLINE 1003
#define MSG_GWS_DRAWCHAR       1004
#define MSG_GWS_DRAWTEXT       1005
#define MSG_GWS_REFRESHWIDNOW  1006
#define MSG_GWS_REDRAWWINDOW   1007
#define MSG_GWS_RESIZEWINDOW   1008
#define MSG_GWS_CHANGEWINDOWPOSITION 1009
//...

#define MSG_GWS_SHUTDOWN          2010  //SOCKET

#define MSG_GWS_REFRESHSCREEN     2020
#define MSG_GWS_REFRESHRECTANGLE  2021

#define MSG_GWS_CLIENTEVENT       2030
#define MSG_GWS_NEXTEVENT         2031

// The buffer has a protocol starting in the 5th long.
// buffer[4] = ethernet header.
#define MSG_GWS_PROTOCOL          3000   

// Quit the process if it's possible.
#define MSG_GWS_QUIT    4080

// drain input 8080

#define MSG_HOTKEY 8888

// switch focus 9090
// refresh dirty rectangles 9091
// update desktop 9092

// ...

#endif    

