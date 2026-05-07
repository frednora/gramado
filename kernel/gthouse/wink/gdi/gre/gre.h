// gre.h
// Graphics Engine
// Environment: ring 0.
// Created by Fred Nora.

#ifndef __GRE_GRE_H
#define __GRE_GRE_H    1


// ===========================


// RGBA tag
struct tagRGBA
{
    object_type_t objectType;
    object_class_t objectClass;

// RGBA
   char red;
   char green;
   char blue;
   char alpha;
};


// High level GUI structure.
// General information about the ring 0 GUI subsystem.
struct gui_d
{
    int initialized;

// #todo: 
// It indicated that we gotta repaint some components.
// (Not defined yet)
    int redraw;

// #todo: 
// It indicated that we gotta flush into the LFB some components.
// (Not defined yet)
    int refresh;

// Security
// User information
    struct usession_d  *user_session;

// #todo: 
// cgroup spport
    // struct cgroup_d  *cgroup;
};
extern struct gui_d  *gui; 

//===========================

// # Principais variáveis globais #
// This is not the right place for them.

// see: graphics.c
extern int current_display; 
extern int current_screen;
extern int guiStatus;
// Status de ambientes gráficos.
extern int logonStatus;              //Logon status.
extern int logoffStatus;             //Logoff status.
extern int userenvironmentStatus;    //User environment status.

extern unsigned long g_system_color;
extern unsigned long g_char_attrib;
// LFB - address for kernel graphic mode
extern unsigned long g_kernel_lfb; 
//video mode
extern unsigned long g_current_vm;          //video memory
extern unsigned long g_current_video_mode;  //video mode
//status do cursor.
//se ele deve aparecer e piscar ou não.
extern int g_show_text_cursor;
//status: aceso ou apagado.
//0=apaga 1=acende.
extern int textcursorStatus;      
extern unsigned long g_mousepointer_x;
extern unsigned long g_mousepointer_y;

//  Compositor
extern int DemoFlag;
extern int UpdateScreenFlag;

// ===========================

// Macros used in the graphics engine.
#define grMIN2(a, b)     (((a) < (b)) ? (a) : (b))
#define grMAX2(a, b)     (((a) > (b)) ? (a) : (b))
#define grMIN3(x,y,z)    (x < y  ? (x < z ? x : z) : (y < z ? y : z))
#define grMAX3(x,y,z)    ( (x>y) ? ((x>z)?x:z)     : ((y>z)?y:z) )


//
// == prototypes =====================================
//

// (1000/fps)=presence_level
unsigned long get_update_screen_frequency(void);
void set_update_screen_frequency(unsigned long fps);
unsigned long get_presence_level(void);
void set_presence_level(unsigned long value);
void schedulerUpdateScreen(void);

// =======================================================

int fib(int n);

int 
grPlot0 ( 
    int z, int x, int y, 
    unsigned int color );

void 
plotLine3d (
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color );

void
rectangleZ (
    int left, int top, 
    int right, int bottom,
    unsigned int color,
    int z );

void 
plotCircleZ ( 
    int xm, 
    int ym, 
    int r, 
    unsigned int color, 
    int z );

void noraDrawingStuff3 (int x, int y, int z);
void demoFred0(void);
void demoFred1(void);
void demo0(void);

void kgws_enable(void);
void kgws_disable(void);


//
// $
// INITIALIZATION
//

int gre_initialize(void);

#endif   

