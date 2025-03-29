
// protocol.h
// gws message protocol.
// Create by Fred Nora.

#ifndef __GWS_PROTOCOL_H
#define __GWS_PROTOCOL_H    1

// Current protocol version
#define GWS_PROTOCOL  1


// Um pacote tem o tamanho total de 512 bytes.
#define sz_gPacket  (512)

// Um request tem o tamanho total de 368 bytes.
// ((14*8)+256)
// 112+256
#define sz_gReq     (368)

// Um reply tem o tamanho total de 368 bytes.
// ((14*8)+256)
// 112+256
#define sz_gRep     (368)


//
// Request
//

struct _gReq
{
// packed header
    unsigned long wid;   // window id
    unsigned long code;  // message code
    unsigned long ul2;
    unsigned long ul3;

// extra
    unsigned long ul4;
    unsigned long ul5;
    unsigned long ul6;
    unsigned long ul7;

// extra
    unsigned long ul8;
    unsigned long ul9;

// extra
    unsigned long ul10;
    unsigned long ul11;
    unsigned long ul12;
    unsigned long ul13;


// Strings or some other data.
    unsigned char data[256];
};
typedef struct _gReq gReq;


//
// Reply
//

struct _gRep
{
// packed header
    unsigned long wid;   // window id
    unsigned long code;  // message code
    unsigned long ul2;
    unsigned long ul3;

// extra
    unsigned long ul4;
    unsigned long ul5;
    unsigned long ul6;
    unsigned long ul7;

// extra
    unsigned long ul8;
    unsigned long ul9;

// extra
    unsigned long ul10;
    unsigned long ul11;
    unsigned long ul12;
    unsigned long ul13;


// Strings or some other data.
    unsigned char data[256];
};
typedef struct _gRep gRep;


// Reply codes:
// This reply is a normal reply.
#define GWS_REPLY  (0)
// This reply is an error notification.
#define GWS_ERROR  (-1)


//++
// =======================================
// Protocol request constants
// Os primeiros sao os mesmos encontrados na api.
 
//window (1-19)  
#define GWS_Create        1
#define GWS_Destroy       2
#define GWS_Move          3
#define GWS_Size          4
#define GWS_Resize        5
//#define GWS_Open        6
#define GWS_Close         7
#define GWS_Paint         8
#define GWS_SetFocus      9
#define GWS_KillFocus    10
#define GWS_Activate     11 
#define GWS_ShowWindow   12
#define GWS_SetCursor    13 
#define GWS_Hide         14
#define GWS_Maximize     15
#define GWS_Restore      16
#define GWS_ShowDefault  17

// See:
// wm.h in libgws/

// usdo pelo window manager
#define GWS_SetFocus2     18
#define GWS_GetFocus2     19

// keyboard (20-29)
#define GWS_KeyDown    20
#define GWS_KeyUp      21
#define GWS_SysKeyDown 22
#define GWS_SysKeyUp   23

// Mouse (30 - 39)
// Tem uma lista de eventos de mouse em events.h
// O kernel tambem obedece essa mesma ordem.
#define GWS_MouseKeyDown     30 
#define GWS_MouseKeyUp       31
//The event occurs when the user presses a mouse button over an element.
#define GWS_MouseButtonDown  30
//The event occurs when a user releases a mouse button over an element.
#define GWS_MouseButtonUp    31
// The event occurs when the pointer is moving while it is over an element.   
#define GWS_MouseMove        32
#define GWS_MouseOver        33
#define GWS_MouseWheel       34
#define GWS_MousePressed     35
#define GWS_MouseReleased    36
// The event occurs when the user clicks on an element.
#define GWS_MouseClicked     37
#define GWS_MouseEntered     38
#define GWS_MouseExited      39
//#define GWS_MouseMoveByOffset
//#define GWS_MouseMoveToElement

//outros (40 - ...)
#define GWS_Command  40
#define GWS_Cut      41
#define GWS_Copy     42
#define GWS_Paste    43
#define GWS_Clear    44
#define GWS_Undo     45
#define GWS_Insert   46
#define GWS_Process  47
#define GWS_Thread   48
//Quando um comando é enviado para o console. ele será atendido pelo
//módulo /sm no procedimento de janela do sistema.
//Todas as mensagens de console serão atencidas pelo procedimento de janela 
//nessa mensagem.
#define GWS_ConsoleCommand   49
#define GWS_ConsoleShutDown  50
#define GWS_ConsoleReboot    51
#define GWS_Developer        52

//UM TIMER SE ESGOTOU,
#define GWS_Timer  53 
//...

// #test
// seleciona todos os elementos.
// control+a
#define GWS_SelectAll    70
// control+f
#define GWS_Search    71
#define GWS_Find      71

// control+s
#define GWS_Save    72



#define GWS_ControlArrowUp      80
#define GWS_ControlArrowDown    81
#define GWS_ControlArrowLeft    82
#define GWS_ControlArrowRight   83

// =======================================
// Protocol request constants
//
// #todo:
// Create a consistent interface.
// See: xxxHandleNextClientRequest() and gwsProcedure on aurora/server/main.c 
// See: 
// All the standar messages, just like MSG_SYSKEYUP ...
// There are some old messages below 369.
#define GWS_GetInputMessage        369
#define GWS_Hello                 1000
#define GWS_CreateWindow          1001
#define GWS_BackbufferPutPixel    1002
#define GWS_DrawHorizontalLine    1003
#define GWS_DrawChar              1004
#define GWS_DrawText              1005
#define GWS_RefreshWindow         1006
#define GWS_RedrawWindow          1007
#define GWS_ResizeWindow          1008
#define GWS_ChangeWindowPosition  1009
#define GWS_BackbufferPutPixel2   2000
#define GWS_Disconnect            2010
#define GWS_RefreshScreen         2020
#define GWS_RefreshRectangle      2021


//#define GWS_GetSendEvent          2030  // send event #
#define GWS_GetNextEvent          2031
#define GWS_GrPlot0               2040
#define GWS_GrCubeZ               2041
#define GWS_GrRectangle           2042


#define GWS_AsyncCommand          2222
// Put a message into the client's queue.
#define GWS_PutClientMessage    2223
// Get a message from the client's queue.
#define GWS_GetClientMessage    2224

// Quit the process if it's possible.
#define GWS_Quit    4080

#define GWS_DrainInput    8080
// ...

// #test
#define GWS_SwitchFocus    9090

// Called by the kernel x times per second.
//#define GWS_Compositor     9091    
#define GWS_RefreshDirtyRectangles  9091

// Redraw all the windows. Back to front.
#define GWS_UpdateDesktop  9092

#define GWS_GetWindowInfo   9093

//test
#define GWS_CloneAndExecute  9099

// =====================================================
//--


#endif    

