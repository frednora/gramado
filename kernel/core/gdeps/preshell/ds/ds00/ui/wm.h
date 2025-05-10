// wm.h
// Window manager support.
// Created by Fred Nora.

#ifndef ____WM_H
#define ____WM_H    1

struct gws_container_d
{
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;
};

//
// Gravity
//

// #todo: Move this to window.h
// Circulating
#define NorthGravity      0
#define NorthEastGravity  1
#define EastGravity       2
#define SouthEastGravity  3
#define SouthGravity      4
#define SouthWestGravity  5
#define WestGravity       6
#define NorthWestGravity  7
#define CenterGravity     8
#define StaticGravity     9
#define DefaultGravity    NorthWestGravity  


// #todo: Move this to window.h
void
wm_gravity_translate(
    unsigned long l, unsigned long t, unsigned long w, unsigned long h, 
    unsigned long oldl, unsigned long oldt,
    unsigned int gravity, 
    unsigned long *destl, unsigned long *destt );

//
// double click
//

//#define DEFAULT_DOUBLE_CLICK_SPEED  250
#define DEFAULT_DOUBLE_CLICK_SPEED  350

struct double_click_d
{
    int initialized;

    unsigned long last;     // Last jiffie for mouse pressed.
    unsigned long current;  // Current jiffie for mouse pressed.
    unsigned long speed;    // Double click speed.
    unsigned long delta;    // Current delta.
    int is_doubleclick;     // Yes, it is a double click.
};
extern struct double_click_d DoubleClick;


// Layouts examples
// tiled, monocle and floating layouts
// Used in WindowManager.mode

#define WM_MODE_TILED       1
#define WM_MODE_OVERLAPPED  2
#define WM_MODE_MONO        3
// ...

struct maximization_style_d
{
    int initialized;
    int style;
};
extern struct maximization_style_d  MaximizationStyle;


// ===============================================================
// #todo
// All the application windows
// are gonna have a offscreen surface,
// and then the compositor is gonna
// copy them all into the backbuffer.

struct desktop_composition_d
{
    int initialized;
    int is_enabled;
// Transparent window frames.
    int use_transparence;
// 2D visual effects associated with the composition method.
    int use_visual_effects;
    // ...
};

struct gws_wm_config_d
{
// Notification area are the top.
    int has_na;
// Taskbar at the bottom.
    int has_taskbar;
// Wallpaper
// #todo: Create a wallpaper structure?
    int has_wallpaper;
};

// The window manager global structure.
struct gws_windowmanager_d
{
    int initialized;

// Info for the areas.
    struct wa_info_d     wa_info;
    struct swamp_info_d  swamp_info;

    struct gws_wm_config_d  Config;

// The window manager mode:
// 1: tiling.
// 2: overlapped.
// ...
    int mode;

// Tiling orientation.
// 1=vertical 0=horizontal
    int vertical;

// #todo
// Maybe we can create more containers,
// and shrink the body container 'wa'.
    //struct gws_container_d icon_area;

    unsigned long frame_counter;
    //unsigned long jiffies;
    int fps;

// Background
    unsigned int default_background_color;
    unsigned int custom_background_color;
    int has_custom_background_color;

// Window stack
// Quando uma janela foi invalidada, significa que ela foi pintada e que
// precisa receber o refesh, mas também pode significar
// que outras janelas foram afetadas pelo pintura da janela.
// Nesse caso precisado repintar a janelas.
// Se tiver uma janela em fullscreen então pintamos, invalidamos
// seu retângulo e validamos todos os outros.

// Windows

    struct gws_window_d *root;
    //struct gws_window_d *taskbar;
    struct gws_window_d *fullscreen_window;
    // ...

    //int is_super_taskbar;

// A area de cliente de uma janela sera mostrada
// na tela toda e essa flag sera acionada.
// Com essa flag acionada a barra de tarefas nao sera exibida.
// fullscreen_window sera a janela usada.
// #todo: Create a structure for fullscreen support?
    int is_fullscreen;

// #todo
// Notification area.
// At the top of the screen.
    //struct gws_container_d na;

// The 'Working Area'.
// The screen size, less the taskbar.
    struct gws_container_d  wa;

// Theme support
// TRUE = Using a loadable theme.
// FALSE = We are not using any theme at all.
// #todo: Create a theme structure?
    int has_theme;
    // ...

// #todo
// All the application windows
// are gonna have a offscreen surface,
// and then the compositor is gonna
// copy them all into the backbuffer.
    struct desktop_composition_d  comp;
};

// Global main structure.
// Not a pointer.
extern struct gws_windowmanager_d  WindowManager;

// =========================================

extern int grab_is_active;
extern int is_dragging;

int control_action(int msg, unsigned long long1);
void on_enter(void);
void on_mouse_pressed(void);
void on_mouse_released(void);
void on_doubleclick(void);

void wm_hit_test_00(unsigned long long1, unsigned long long2);


//
// Input support
//

void wmInitializeGlobals(void);
unsigned long wmGetLastInputJiffie(int update);

// Associa a estrutura de janela
// com uma estrutura de cliente. 
// see: wm.c
int wmBindWindowToClient(struct gws_window_d *w);

// Update desktop.
void wm_update_desktop(int tile, int show);
void wm_update_desktop2(void);
void wm_update_desktop3(struct gws_window_d *new_active_window);

void restore_desktop_windows(void);
void show_desktop(void);

// Update window.
int 
update_window ( 
    struct gws_window_d *window, 
    unsigned long flags );

void wm_update_window_by_id(int wid);
void wm_update_active_window(void);

// Fullscreen mode.
void wm_enter_fullscreen_mode(void);
void wm_exit_fullscreen_mode(int tile);


// Add a window on top of the list of childs.
void wm_add_childwindow(struct gws_window_d *parent, struct gws_window_d *window);

// list support
// not tested yet
void wm_add_window_to_bottom(struct gws_window_d *window);
void wm_add_window_to_top(struct gws_window_d *window);
void wm_rebuild_list(void);
void wm_rebuild_list2(void);

void wm_remove_window_from_list_and_kill(struct gws_window_d *window);
void wm_remove_window_from_list(struct gws_window_d *window);

void wm_reboot(void);

#endif   


