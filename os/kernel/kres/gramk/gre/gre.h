// gre.h
// Graphics Engine
// Environment: ring 0.
// Created by Fred Nora.

#ifndef __GRAMK_GRE_H
#define __GRAMK_GRE_H    1

#define grMIN2(a, b)  (((a) < (b)) ? (a) : (b))
#define grMAX2(a, b)  (((a) > (b)) ? (a) : (b))
#define grMIN3(x,y,z)     (x < y  ? (x < z ? x : z) : (y < z ? y : z))
#define grMAX3(x,y,z)     ( (x>y) ? ((x>z)?x:z)     : ((y>z)?y:z) )

//  Compositor
extern int DemoFlag;
extern int UpdateScreenFlag;

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
int kgwsRegisterWindowServer(pid_t pid);

int windowLoadGramadoIcons(void);
void *ui_get_system_icon(int n);


//
// $
// INITIALIZATION
//

int gre_initialize(void);

#endif   



