// globals.h
// 2022 - Created by Fred Nora.

#ifndef __GWSSRV_GLOBALS_H
#define __GWSSRV_GLOBALS_H   1


// ============================
// OS modes
#define GRAMADO_JAIL        0x00
#define GRAMADO_P1          0x01
#define GRAMADO_HOME        0x02  // --o--
#define GRAMADO_P2          0x03
#define GRAMADO_CASTLE      0x04
//#define GRAMADO_CALIFORNIA  0x05
extern int os_mode;

// ============================
// Server modes
#define SERVER_MODE_SERVER  1
#define SERVER_MODE_DEMO    2
// ...
extern int server_mode;

extern int gUseCallback;
extern int gUseMouse;

//
// == buffer ===============================================
//

// This is the buffer used when we receive 
// requests via socket.
// see: globals.c
#define MSG_BUFFER_SIZE  512
extern char __buffer[MSG_BUFFER_SIZE];

// Esses valores serão enviados como 
// resposta ao serviço atual.
// Eles são configurados pelo dialogo na hora da 
// prestação do serviço.
// No início desse array fica o header.
// see: globals.c
#define NEXTRESPONSE_BUFFER_SIZE  32
extern unsigned long next_response[32];

//----------------------------------------

// Initialization phase.
// 0: Nothing
// 1: Initialize global variables and structures.
// 2: Initialize graphics infrastructure.
// 3: Entering the main loop. 

struct initialization_d
{
// Initialization phase
    int current_phase;
// Checkpoints
    int ds_struct_checkpoint;  // display server structure
    int wm_struct_checkpoint;   // window manager structure
    int register_ds_checkpoint;  // display server.
    int setup_callback_checkpoint;
    int setup_connection_checkpoint;
    int setup_graphics_interface_checkpoint;
    int inloop_checkpoint;
};
extern struct initialization_d  Initialization;


struct server_profiler_d
{
    int initialized;
    unsigned long dispatch_counter;
// Windows
// (create window) counters.
    unsigned long cw_simple;
    unsigned long cw_editbox_single_line;
    unsigned long cw_overlapped;
    unsigned long cw_popup;
    unsigned long cw_checkbox;
    unsigned long cw_scrollbar;
    unsigned long cw_editbox_multiple_lines;
    unsigned long cw_button;
    unsigned long cw_statusbar;
    unsigned long cw_icon;
    unsigned long cw_titlebar;
    //unsigned long cw_default;   // for error
    // ...
};
extern struct server_profiler_d  ServerProfiler;

// Server states
#define SERVER_STATE_NULL  0
#define SERVER_STATE_BOOTING  1
#define SERVER_STATE_RUNNING  2
#define SERVER_STATE_SHUTTINGDOWN  3

struct server_state_d
{
// Struct initialization.
    int initialized;
// Server state.
    int state;
    // ...
};
extern struct server_state_d ServerState;


// Host Operating System
#define OS_OTHER    0
#define OS_NATIVE   1
#define OS_LINUX    2
#define OS_WINDOWS  3

// Virtual Machine
#define VM_OTHER       0
#define VM_NATIVE      1
#define VM_VMWARE      2
#define VM_QEMU        3
#define VM_VIRTUALBOX  4

// =========================
// h:d.s
extern char *host_name;
extern char *display_number;
extern char *screen_number;



/*
 * ColorSchemeIndex:
 *     csi - color scheme index. 
 *     Enumerando os elementos gráficos
 *     O esquema de cores se aplica ao servidor 
 * de recursos gráficos. GWS.
 */
typedef enum {

    csiNull,                    //0
    csiDesktop,                 //1 área de trabalho.

    //window
    csiWindow,                  //2
    csiWindowBackground,        //3
    csiActiveWindowBorder,      //4
    csiInactiveWindowBorder,    //5

    //bar
    csiActiveWindowTitleBar,    //6
    csiInactiveWindowTitleBar,  //7
    csiMenuBar,                 //8
    csiScrollBar,               //9
    csiStatusBar,               //10
    csiTaskBar,                 //11

    csiMessageBox,              //12
    csiSystemFontColor,         //13 BLACK
    csiTerminalFontColor,       //14 WHITE
    
    csiButton,   // 15

    csiWindowBorder,  // 16 window border
    csiWWFBorder,     // 17 window with focus border.

    csiTitleBarTextColor,  // 18

// Mouse hover

    csiWhenMouseHover,     // 19 bg when mouse hover
    csiWhenMouseHoverMinimizeControl, // 20
    csiWhenMouseHoverMaximizeControl, // 21
    csiWhenMouseHoverCloseControl, //22
    
    csiTaskBarTextColor   // 23: Color for text in the taskbar.

    //...
    //@todo: focus,
    //@todo: Window 'shadow' (black??)

}ColorSchemeIndex;  


/*
 * gws_color_scheme_d:
 *     Estrutura para esquema de cores. 
 *     O esquema de cores se aplica ao servidor 
 * de recursos gráficos. GWS.
 */ 
struct gws_color_scheme_d
{
    int used;
    int magic;

    int id;
    int initialized;

    char *name; 

// Marcador de estilo de design para o padrão de cores.
// Cada estilo pode invocar por si um padrão de cores 
// ou o padrão de cores por si pode representar um estilo.
    int style;

// Cada índice desse array representará um elemento gráfico,
// Os valores no array são cores 
// correspondentes aos elementos gráficos indicados pelos índices.

    unsigned int elements[32];
    //color_t elements[32];

    //...

// Navigation
    struct gws_color_scheme_d *next;
};

// See: wm.c
extern struct gws_color_scheme_d* GWSCurrentColorScheme;


//
// == variables ==============================================
//

// #ordem
// hardware, software
extern unsigned long  ____BACKBUFFER_VA;
extern unsigned long  ____FRONTBUFFER_VA;
// Saving
extern unsigned long SavedBootBlock;
extern unsigned long SavedLFB;   // #bugbug? VA ?
extern unsigned long SavedX;
extern unsigned long SavedY;
extern unsigned long SavedBPP; 
// helper.
extern unsigned long __device_width;
extern unsigned long __device_height;
extern unsigned long __device_bpp;

// refresh rate of the whole screen.
//static unsigned long fps;

// refresh rate for all dirty objects. In one round.
//static unsigned long frames_count;
// static unsigned long frames_count_in_this_round;

// Vamos usar ou nao.
extern int use_vsync;
#define VSYNC_YES  1
#define VSYNC_NO   0


// Indexes to screens.
#define MAX_SCREENS    4 
#define SCREEN_FRONTBUFFER     0  // VRAM
#define SCREEN_BACKBUFFER      1  // RAM
//#define SCREEN_BACKBUFFER2   2
//#define SCREEN_BACKBUFFER3   3


struct offscreen_window_d
{
    int initialized;

// This is an offscreen buffer for a window.
    int wid;  // The Window ID.
    pid_t pid;  // The client's PID.

// The paint is done, let's flush it into the backbuffer.
    int _dirty;

    // ...
};

// Each window needs to have an index for one of this structurea.
// This way the display server will know the information about 
// the window's canvas.
struct screen_information_d
{
    int initialized;
    unsigned long base_address;

// When this screen belongs to an offscreen window.
// It will be used by the compositor to build the desktop scene.
    struct offscreen_window_d  OffscreeWindow;

    // ...
};
extern struct screen_information_d  screens[MAX_SCREENS];

/*
struct vid_d
{
    unsigned long width;
    unsigned long height;
    unsigned long bpp;
    unsigned long frontbuffer;  // lfb
    unsigned long backbuffer;   // screen[0]
};
struct vid_d vidConfig;
*/


//
// == Prototypes ===================================================
//

// wm.c
int gwssrv_initialize_default_color_scheme(void);
unsigned int get_color(int index);

#endif   



