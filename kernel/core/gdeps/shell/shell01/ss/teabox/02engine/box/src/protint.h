// protint.h
//  Protocol Internal.
// gws message protocol use inside the engine.
// Create by Fred Nora.


#ifndef __GWS_PROTOCOL_H
#define __GWS_PROTOCOL_H    1

// Current protocol version
#define __GWS_PROTOCOL  1


// Um request tem o tamanho total de 368 bytes.
// ((14*8)+256)
// 112+256
#define __sz_gReq     (368)

// Um reply tem o tamanho total de 368 bytes.
// ((14*8)+256)
// 112+256
#define __sz_gRep     (368)


//
// Request
//

struct _gReq_Int
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
typedef struct _gReq_Int gReqInt;


//
// Reply
//

struct _gRep_Int
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
typedef struct _gRep_Int gRepInt;


// Reply codes:
// This reply is a normal reply.
#define __GWS_REPLY  (0)
// This reply is an error notification.
#define __GWS_ERROR  (-1)


//++
// =======================================
// Protocol request constants
// Os primeiros sao os mesmos encontrados na api.
 
//window (1-19)  
#define __GWS_Create        1
#define __GWS_Destroy       2
#define __GWS_Move          3
#define __GWS_Size          4
#define __GWS_Resize        5
//#define GWS_Open        6
#define __GWS_Close         7
#define __GWS_Paint         8
#define __GWS_SetFocus      9
#define __GWS_KillFocus    10
#define __GWS_Activate     11 
#define __GWS_ShowWindow   12
#define __GWS_SetCursor    13 
#define __GWS_Hide         14
#define __GWS_Maximize     15
#define __GWS_Restore      16
#define __GWS_ShowDefault  17

// See:
// wm.h in libgws/

// usdo pelo window manager
#define __GWS_SetFocus2     18
#define __GWS_GetFocus2     19

// keyboard (20-29)
#define __GWS_KeyDown    20
#define __GWS_KeyUp      21
#define __GWS_SysKeyDown 22
#define __GWS_SysKeyUp   23

// Mouse (30 - 39)
// Tem uma lista de eventos de mouse em events.h
// O kernel tambem obedece essa mesma ordem.
#define __GWS_MouseKeyDown     30 
#define __GWS_MouseKeyUp       31
//The event occurs when the user presses a mouse button over an element.
#define __GWS_MouseButtonDown  30
//The event occurs when a user releases a mouse button over an element.
#define __GWS_MouseButtonUp    31
// The event occurs when the pointer is moving while it is over an element.   
#define __GWS_MouseMove        32
#define __GWS_MouseOver        33
#define __GWS_MouseWheel       34
#define __GWS_MousePressed     35
#define __GWS_MouseReleased    36
// The event occurs when the user clicks on an element.
#define __GWS_MouseClicked     37
#define __GWS_MouseEntered     38
#define __GWS_MouseExited      39
//#define __GWS_MouseMoveByOffset
//#define __GWS_MouseMoveToElement

//outros (40 - ...)
#define __GWS_Command  40
#define __GWS_Cut      41
#define __GWS_Copy     42
#define __GWS_Paste    43
#define __GWS_Clear    44
#define __GWS_Undo     45
#define __GWS_Insert   46
#define __GWS_Process  47
#define __GWS_Thread   48
//Quando um comando é enviado para o console. ele será atendido pelo
//módulo /sm no procedimento de janela do sistema.
//Todas as mensagens de console serão atencidas pelo procedimento de janela 
//nessa mensagem.
#define __GWS_ConsoleCommand   49
#define __GWS_ConsoleShutDown  50
#define __GWS_ConsoleReboot    51
#define __GWS_Developer        52

//UM TIMER SE ESGOTOU,
#define __GWS_Timer  53 
//...

// #test
// seleciona todos os elementos.
// control+a
#define __GWS_SelectAll    70
// control+f
#define __GWS_Search    71
#define __GWS_Find      71

// control+s
#define __GWS_Save    72

#define __GWS_ControlArrowUp      80
#define __GWS_ControlArrowDown    81
#define __GWS_ControlArrowLeft    82
#define __GWS_ControlArrowRight   83

// =======================================
// Protocol request constants
//
// #todo:
// Create a consistent interface.
// See: xxxHandleNextClientRequest() and gwsProcedure on aurora/server/main.c 
// See: 
// All the standar messages, just like MSG_SYSKEYUP ...
// There are some old messages below 369.
#define __GWS_GetInputMessage        369
#define __GWS_Hello                 1000
#define __GWS_CreateWindow          1001
#define __GWS_BackbufferPutPixel    1002
#define __GWS_DrawHorizontalLine    1003
#define __GWS_DrawChar              1004
#define __GWS_DrawText              1005
#define __GWS_RefreshWindow         1006
#define __GWS_RedrawWindow          1007
#define __GWS_ResizeWindow          1008
#define __GWS_ChangeWindowPosition  1009
#define __GWS_BackbufferPutPixel2   2000
#define __GWS_Disconnect            2010
#define __GWS_RefreshScreen         2020
#define __GWS_RefreshRectangle      2021

//#define __GWS_GetSendEvent          2030  // send event #
#define __GWS_GetNextEvent          2031
#define __GWS_GrPlot0               2040
#define __GWS_GrCubeZ               2041
#define __GWS_GrRectangle           2042

#define __GWS_AsyncCommand          2222
// Put a message into the client's queue.
#define __GWS_PutClientMessage    2223
// Get a message from the client's queue.
#define __GWS_GetClientMessage    2224

// Quit the process if it's possible.
#define __GWS_Quit    4080

#define __GWS_DrainInput    8080
// ...

// #test
#define __GWS_SwitchFocus    9090

// Called by the kernel x times per second.
//#define __GWS_Compositor     9091    
#define __GWS_RefreshDirtyRectangles  9091

// Redraw all the windows. Back to front.
#define __GWS_UpdateDesktop  9092

#define __GWS_GetWindowInfo   9093

//test
#define __GWS_CloneAndExecute  9099

// =====================================================
//--

#endif    

