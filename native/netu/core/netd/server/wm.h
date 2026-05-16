

//window messages.
//**** 
//window (1-19)  
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
#define MSG_HIDE          14
#define MSG_MAXIMIZE      15
#define MSG_RESTORE       16
#define MSG_SHOWDEFAULT   17

//keyboard (20-29)
#define MSG_KEYDOWN       20
#define MSG_KEYUP         21
#define MSG_SYSKEYDOWN    22
#define MSG_SYSKEYUP      23



//mouse (30 - 39)
//tem uma lista de eventos de mouse em events.h
#define MSG_MOUSEKEYDOWN  30
#define MSG_MOUSEKEYUP    31
#define MSG_MOUSEBUTTONDOWN	30
#define MSG_MOUSEBUTTONUP	31 
#define MSG_MOUSEMOVE	    32
#define MSG_MOUSEOVER	    33
#define MSG_MOUSEWHEEL	    34
#define MSG_MOUSEPRESSED	  35
#define MSG_MOUSERELEASED	  36
#define MSG_MOUSECLICKED	  37
#define MSG_MOUSEENTERED	  38
#define MSG_MOUSEEXITED	       39
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
#define MSG_RUN_PROCESS   47
#define MSG_RUN_THREAD    48
//Quando um comando é enviado para o console. ele será atendido pelo
//módulo /sm no procedimento de janela do sistema.
//Todas as mensagens de console serão atencidas pelo procedimento de janela 
//nessa mensagem.
#define MSG_CONSOLE_COMMAND 49
#define MSG_CONSOLE_SHUTDOWN 50
#define MSG_CONSOLE_REBOOT   51
#define MSG_DEVELOPER 52
//...


//UM TIMER SE ESGOTOU,
#define MSG_TIMER 53   


//o servidor de rede se comunica com o processo.
#define MSG_AF_INET 54
#define MSG_NET_DATA_IN 55

//o driver de network está notificando um processo em ring3.
#define MSG_NETWORK_NOTIFY_PROCESS 56


//
// mouse support: continuação ...
//
#define MSG_MOUSE_DOUBLECLICKED   60
#define MSG_MOUSE_DRAG            61
#define MSG_MOUSE_DROP            62
//...



//
//  terminal commands
//

#define MSG_TERMINALCOMMAND      100

//#deprecated!!!
//#define TERMINALCOMMAND_PRINTCHAR 1000
//#define TERMINALCOMMAND_PRINT??? 1001
//...


//#deprecated ??
// o evento de rolagem aconteceu ...
// O número do evento será entregue em long1.
//#define MSG_HSCROLL 2000
//#define MSG_VSCROLL 2001

//
// Socket messages
//


#define MSG_GNS_HELLO 1000
#define MSG_GNS_INITIALIZENETWORK   1001
#define MSG_GNS_SHUTDOWN   2000   //SOCKET
// The buffer has a protocol starting in the 5th long.
// buffer[4] = ethernet header.
#define MSG_GNS_PROTOCOL          3000   



