// wm.h


#ifndef __UI_WM_H
#define __UI_WM_H    1

// The window manager global structure.
struct gws_windowmanager_d
{
    int initialized;

// The window manager mode:
// 1: tiling.
// 2: overlapped.
// ...
    int mode;
    
    // 1= vertical 0=horizontal
    int vertical;

// The Working area.
// The screen size, less the task bar.
    int wa_left;
    int wa_top;
    int wa_width;
    int wa_height;

    // ...
    
    unsigned long frame_counter;
    //unsigned long jiffies;
    int fps;

// Default background color.
    unsigned int default_background_color;

// Custom backgrounc color.
    unsigned int custom_background_color;
    int has_custom_background_color;

// Wallpaper
    int has_wallpaper;

// Theme support
// TRUE = Using a loadable theme.
// FALSE = We are not using any theme at all.
    int has_theme;

    // ...

// Window stack
// Quando uma janela foi invalidada, significa que ela foi pintada e que
// precisa receber o refesh, mas também pode significar
// que outras janelas foram afetadas pelo pintura da janela.
// Nesse caso precisado repintar a janelas.
// Se tiver uma janela em fullscreen então pintamos, invalidamos
// seu retângulo e validamos todos os outros.

// Windows.
    
    // root
    struct gws_window_d *root;
    
    // taskbar support
    struct gws_window_d *taskbar;
    // window inside the taskbar.
    //struct gws_window_d *box1;   // left box: button box
    //struct gws_window_d *box2;   // right box: notification box
    // floating windows support
    // windows on top of taskbar if the tb is on bottom of the screen.
    //struct gws_window_d *tray1;  // left tray: start menu
    //struct gws_window_d *tray2;  // right tray: notification window


// #test
// z-order for all the layers.
// linked list
    //struct gws_window_d *layer1_list;
    //struct gws_window_d *layer2_list;
    //struct gws_window_d *layer3_list;
    //struct gws_window_d *layer4_list;


// A are de cliente de uma janela sera mostrada
// na tela toda e essa flag sera acionada.
// Com essa flag acionada a barra de tarefas nao sera exibida.
    int is_fullscreen;
    struct gws_window_d *fullscreen_window;
};

// Global main structure.
// Not a pointer.
extern struct gws_windowmanager_d  WindowManager;

// layouts examples
// tiled, monocle and floating layouts

#define WM_MODE_TILED       1
#define WM_MODE_OVERLAPPED  2
#define WM_MODE_MONO        3

// ======

void wm_change_bg_color(unsigned int color, int tile, int fullscreen);


// Fullscreen mode.
void wm_enter_fullscreen_mode(void);
void wm_exit_fullscreen_mode(int tile);


// list support
// not tested yet
void wm_add_window_into_the_list( struct gws_window_d *window );
void wm_remove_window_from_list_and_kill( struct gws_window_d *window );

unsigned long wmGetLastInputJiffie(int update);

void wm_Update_TaskBar( char *string, int flush );

void wm_update_desktop(int tile, int show);

// Client support.
struct gws_client_d *wintoclient(int window); //#todo: not teste yet
void show_client_list(int tag); //#todo: notworking
void show_client( struct gws_client_d *c, int tag );
int wmManageWindow( struct gws_window_d *w );

void wmInitializeGlobals(void);



#endif   

