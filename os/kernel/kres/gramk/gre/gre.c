// gre.c
// Graphics Engine.
// Environment: kernel
// Created by Fred Nora.

#include <kernel.h>  

//  Compositor
int DemoFlag=0;
int UpdateScreenFlag=0;


// see: ws.h
struct color_scheme_d *HumilityColorScheme; // Simples.
struct color_scheme_d *PrideColorScheme;    // Colorido.
struct color_scheme_d *CurrentColorScheme;


// monitor. (hardware)
int current_display=0;

// superficie.
// Um monitor pode ter varias screens
// e uma screen pode estar em mais de um monitor
int current_screen=0;

//Status da Interface gráfica do usuário.
int guiStatus=0;

// Status de ambientes gráficos.
int logonStatus=0;            //Logon status.
int logoffStatus=0;           //Logoff status.
int userenvironmentStatus=0;  //User environment status.

unsigned long g_system_color=0;
unsigned long g_char_attrib=0;

unsigned long g_kernel_lfb=0;

//video mode
unsigned long g_current_vm=0;          //video memory
unsigned long g_current_video_mode=0;  //video mode

//status do cursor.
//se ele deve aparecer e piscar ou não.
int g_show_text_cursor=0;
//status: aceso ou apagado.
//0=apaga 1=acende.
int textcursorStatus=0; 

unsigned long g_mousepointer_x=0;
unsigned long g_mousepointer_y=0;


// Device hotspot.
static unsigned long HotSpotX=0;
static unsigned long HotSpotY=0;

static int EnableKGWS = TRUE;

int kgws_status;
pid_t kgws_ws_PID;
int kgws_ws_status;


//=================================

static int __gre_initialize_globals(void);

//=================================


void kgws_enable(void)
{
    debug_print("kgws_enable: Enable KGWS\n");
         printk("kgws_enable: Enable KGWS\n");
    refresh_screen();

    // Sending event messages to the thread associated with the wwf.
    //current_input_mode = INPUT_MODE_SETUP;
    IOControl.useEventQueue = TRUE;
    
    EnableKGWS = TRUE;
}

void kgws_disable(void)
{
    debug_print("kgws_disable:\n");
         printk("kgws_disable:\n");

    displayInitializeBackground(COLOR_RED,TRUE);

    // Using unix-like TTY mode for input.
    // using the stdin.
    // current_input_mode = INPUT_MODE_TTY;
    
    // #bugbug: This is a test yet.
    //IOControl.useEventQueue = FALSE;
    
    EnableKGWS = FALSE;
}


// Fibonacci Series using Recursion 
int fib (int n)
{ 
    register int Copy = n;
    int a=0;
    int b=0;

    if (Copy <= 1){ return Copy; }
    
    a = fib(Copy - 1); 
    b = fib(Copy - 2);

    return (int) (a+b);
} 

/*
 * grPlot0:
 *      plot pixel.
 *      Low level routine.
 *      Plot into a normalized screen. kinda.
 *      #new: Plotting into a clipping window.
 */
// low level plot.
// History:
//     2020 - Created by Fred Nora.
// window ?
// Essa rotina pode pintar em qualquer posição 
// da tela do dispositivo. 
// Com origem no centro da tela.
// Aceitamos valores negativos e positivos.
// O limite máximo será modular.
// 3D fullscreen, origin in center.
int 
grPlot0 ( 
    int z, int x, int y, 
    unsigned int color )
{

// #todo
// We can use a 'clipping window' and draw only inside this window.
// If the clipping window is NULL, so we need to use the root window.

    // #todo
    // This is a work in progress

    // Draw flag.
    int Draw = TRUE;
    int UseClipping = FALSE;
    int UsingDepthBuffer = FALSE;
    int UsingAlphaBlending = FALSE;
// left hand orientation
// z+ on top/right corner.
    int FixOrientation = TRUE;

// #todo
// We need a z-buffer or (depth buffer)
// it is used to depth testing.
// it discards some unnecessary covered plots. 
// Normalized Device Coordinates (NDC)
// We are using or own ndc style.
// Maybe it is different for diferent systens.
//      +y  +z
// -x   +   +x
// -z   -y
// We use:
// Left-hand System (LHS) 
// LHS is clockwise (CW).
// Same as Microsoft Direct3D.
// See: https://en.wikipedia.org/wiki/Direct3D
// Another way is:
// Right-Hand Coordinate System (RHS).
// RHS is counter-clockwise (CCW).

    unsigned long zBaseX=0;
    unsigned long zBaseY=0;

// #bugbug
// Precisa ser 'int', nao podemos enviar 
// valores negativos para putpixel.
     int X=0;
     int Y=0;

// #todo
// If the clipping window is invalid, 
// so we're gonna use the root window.
// #todo:
// Maybe we need to use the device context structure,
// or something like that.

    UseClipping = FALSE;

// #todo
// Precisamos checar algumas globais, como HotSpotX e HotSpotY.

// Usaremos a janela chamada screen se nenhuma outra foi indicada.
    //gui->screen

    // z negativo
    //  _
    //   |

    if (z < 0)
    {
        // z é módulo para todos os casos em que z é menor que 0.
        z = abs(z);

        // positivo, para direita.
        // Desloca a base x para esquerda, onde fica o z negativo
        if (x >= 0 )
        {
            X = (unsigned long) ( (unsigned long)HotSpotX  + (unsigned long)x);
        }

        // negativo, para esquerda.
        // Desloca a base x para esquerda, onde fica o z negativo
        if (x < 0 )
        {
            x = abs(x); 
            X = (unsigned long) (  (unsigned long)HotSpotX - (unsigned long)x );
        }

        // positivo, para cima.
        // Desloca a base y para baixo, onde fica o z negativo
        if ( y >= 0 )
        {
            Y = (unsigned long) ( (unsigned long)HotSpotY - (unsigned long)y );
        }

        // negativo, para baixo
        // Desloca a base y para baixo, onde fica o z negativo
        if ( y < 0 )
        {
            y = abs(y);
            Y = (unsigned long) ( (unsigned long) HotSpotY + (unsigned long) y );
        }

        if (FixOrientation == TRUE){
            X = ( (unsigned long) X - (unsigned long) z );
            Y = ( (unsigned long) Y + (unsigned long) z );
        }
        
        //if (Draw == FALSE){ return -1; }
        goto draw;
    }


    // z maior ou igual a zero.
    //    |
    //    ----
    //
    if (z >= 0)
    {
        // z é positivo para todos os casos onde z é maior igual a 0.
        
        // positivo, para direita.
        // Desloca a base x para direita, onde fica o z positivo
        if (x >= 0 )
        {
            X = (unsigned long) ( (unsigned long) HotSpotX + (unsigned long) x );
        }
        
        // negativo, para esquerda.
        // Desloca a base x para direita, onde fica o z positivo
        if (x < 0 )
        {
            x = abs(x);   
            X = (unsigned long) ( (unsigned long)HotSpotX - (unsigned long)x  );
        }

        // positivo, para cima.
        // Desloca a base y para cima, onde fica o z positivo
        if ( y >= 0 )
        {
            Y = (unsigned long) ( (unsigned long)HotSpotY - (unsigned long)y );
        }

        // negativo, para baixo
        // Desloca a base y para cima, onde fica o z positivo
        if ( y < 0 )
        {
            y = abs(y);
            Y = (unsigned long) ( (unsigned long)HotSpotY + (unsigned long)y );
        }

        if (FixOrientation == TRUE){
            X = ( (unsigned long) X + (unsigned long) z );
            Y = ( (unsigned long) Y - (unsigned long) z );
        }
        
        //if (Draw == FALSE){ return -1; }
        goto draw;
    }

    // Fail

    Draw = FALSE;

    return (-1);

//
// Draw
//

draw:
    
    //
    // Clipping
    //
    
    // #todo: 
    // We need to check the window limits
    // if we are drawing inside a given window.

    // Checking the device screen limits.
    // #todo: use a display device structure
    if ( 0 <= X < gSavedX && 0 <= Y < gSavedY )
    {
        if (Draw == TRUE)
        {
            // #bugbug
            // Já fizemos isso logo acima.
            
            if (X<0){ return -1; }
            if (Y<0){ return -1; }
            
            if ( UsingAlphaBlending == TRUE )
            {
                // #todo
                // Get the color and modify it.
                
                //color = get??()
            }
            
            // Se NÃO temos uma janela válida.
            if ( UseClipping == FALSE ){
                // device screen
                // 2D, No clipping or transformation.
                //pixelBackBufferPutpixel ( color, X, Y ); 
                backbuffer_putpixel(color,X,Y,0);
            }

             /*
            // Se temos uma janela válida.
            if ( UseClipping == TRUE ){
                if ( X >= w->left  &&
                     X <= w->right &&
                     Y >= w->top   &&
                     Y <= w->bottom )
                 {
                     pixelBackBufferPutpixel(color,X,Y); 
                 }
            }
            */
            
            // #todo
            // This is a work in progress
            
            //if( UsingDepthBuffer == TRUE )
            //{
            //    depth_buffer[ offset ] = Z;
            //}
            
            return 0;
        }
        return -1;
    }

// Fail 

    return (-1);
}

/*
 * plotLine3d: 
 */
// Bresenham in 3D
// The algorithm could be extended to three (or more) dimensions.

void 
plotLine3d (
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color )
{
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
    int dz = abs(z1-z0), sz = z0<z1 ? 1 : -1; 

    //#bugbug: This macro is wrong?!
    //int dm = grMAX3 (dx,dy,dz), i = dm; /* maximum difference */

    int dm = grMAX3(dx,dy,dz);
    register int i = dm;

    // x1 = y1 = z1 = dm/2; /* error offset */
 
    x1 = (dm >> 1);
    y1 = x1;
    z1 = x1;

    for (;;) {
        grPlot0 ( z0, x0, y0, color );
        if (i-- == 0){
            break;
        }
        x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; }
        y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; }
        z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; }
    };
}

// The upper-left corner and lower-right corner. 
void
rectangleZ (
    int left, int top, 
    int right, int bottom,
    unsigned int color,
    int z )
{
    // #todo
    // Check validation
    // if (left<0 ...
    
    // cima
    plotLine3d ( left, top,  z, right, top, z, color );
    // baixo
    plotLine3d ( left, bottom, z, right,bottom, z, color );
    // esquerda
    plotLine3d ( left, top, z, left, bottom, z, color );
    // direita
    plotLine3d ( right,  top, z, right, bottom, z, color );
}


// ?? what means 'm' ???
void 
plotCircleZ ( 
    int xm, 
    int ym, 
    int r, 
    unsigned int color, 
    int z )
{

    /* II. Quadrant */ 
   //int x = -r, y = 0, err = 2-2*r; 
   
    //loop
    register int x = -r;
   
    int y = 0;
    int err =  (2-(2*r));

    do {
      
      //setPixel(xm-x, ym+y); /*   I. Quadrant */
      //setPixel(xm-y, ym-x); /*  II. Quadrant */
      //setPixel(xm+x, ym-y); /* III. Quadrant */
      //setPixel(xm+y, ym+x); /*  IV. Quadrant */
      
      grPlot0 ( z, xm-x, ym+y, color );
      grPlot0 ( z, xm-y, ym-x, color );
      grPlot0 ( z, xm+x, ym-y, color );
      grPlot0 ( z, xm+y, ym+x, color );

      r = err;
      
      // #ugly routine.
      
      /* e_xy+e_y < 0 */
      if (r <= y) 
      { 
           err += ++y * 2 + 1; 
      }           
      
      /* e_xy+e_x > 0 or no 2nd y-step */
      if (r > x || err > y) 
      { 
          err += ++x * 2+1; 
      }
      
    } while (x < 0);
}


/*
 ********************************************************
 * noraDrawingStuff3: 
 *    Lot of dots in the right place. (prime stuff)
 *    2020 - Created by Fred Nora. 
 */

void noraDrawingStuff3 (int x, int y, int z)
{
    register int _x=0;
    register int _y=0;
    register int _z = z;

    int limitX = (gSavedX >> 1);
    int limitY = (gSavedY >> 1);

    // colunas.
    for (_x=x; _x<limitX; _x++)    
    {
        for (_y=y; _y<limitY; _y++)
        {
            if ( _x != 0 )
            {
                if ( _y % _x == 0 ){
                    grPlot0 ( _z, _x, _y,COLOR_GREEN );
                }
            }
        };
        if (_x >= limitY)
        { 
            break;
        }
    };
}

void demoFred0(void)
{
    register int i=0;

    for (i=0; i<100; i++){
            //noraDrawingStuff3 (i,i,0);
            noraDrawingStuff3 (-i,-i,0);
            //rectangle(8,8,i,i,COLOR_BLUE);
            //rectangleZ(i,i,i+20,i+20,COLOR_BLUE,i);
            //plotCircle ( -i % 20, -i % 20, i%20, COLOR_GREEN);
            
            plotCircleZ ( -i % 20, -i % 20, i % 20, COLOR_GREEN, i % 20);  //save this
            //plotCircleZ ( -i % 20, -i % 20, i % 20, COLOR_GREEN, i % 20);   //save this
            //cool
            //plotCircleZ ( -i % fib(20), -i % fib(20), i % fib(20), COLOR_GREEN, i % fib(20) );
            //igual o de cima.
            //plotCircleZ ( -i % fib(20), -i % fib(20), i, COLOR_GREEN, i % fib(20) );
            //plotCircleZ ( -i % fib(7), -i % fib(7), i % fib(7), COLOR_GREEN, i % fib(7) );
            //plotEllipseRect ( i, i, i*5, i*5, COLOR_BLUE);
            //plotEllipseRectZ ( i%20, i%20, i, i, COLOR_BLUE,i%20);
    };
}

void demoFred1(void)
{
    register int i=0;

    for (i=0; i<100; i++){
            //noraDrawingStuff3 (i,i,0);
            noraDrawingStuff3 (-i,-i,0);
            //rectangle(8,8,i,i,COLOR_BLUE);
            //rectangleZ(i,i,i+20,i+20,COLOR_BLUE,i);
            //plotCircle ( -i % 20, -i % 20, i%20, COLOR_GREEN);
            //plotCircleZ ( -i % 20, -i % 20, i % 20, COLOR_GREEN, i % 20);  //save this
            //plotCircleZ ( -i % 20, -i % 20, i % 20, COLOR_GREEN, i % 20);   //save this
            //cool
            plotCircleZ ( -i % fib(20), -i % fib(20), i % fib(20), COLOR_GREEN, i % fib(20) );
            //igual o de cima.
            //plotCircleZ ( -i % fib(20), -i % fib(20), i, COLOR_GREEN, i % fib(20) );
            //plotCircleZ ( -i % fib(7), -i % fib(7), i % fib(7), COLOR_GREEN, i % fib(7) );
            //plotEllipseRect ( i, i, i*5, i*5, COLOR_BLUE);
            //plotEllipseRectZ ( i%20, i%20, i, i, COLOR_BLUE,i%20);
    };
}

void demo0(void)
{
    // #test 
    //grPlot0(0,10,10,COLOR_GREEN);
    //plotCircleZ ( 0, 12, 25, COLOR_GREEN, 0); 
    //plotLine3d ( 4, 3,0, 40, 2,0, COLOR_WHITE);      
    //noraDrawingStuff3(20,20,0);
    //demoFred0();
    //demoFred1();
    //rectangleZ( 10, 10, 10+20,10+20,COLOR_BLUE,0);                  
    //refresh_screen();
}

// ?? Not used yet ??
// Registrar um window server.
// See: register_ws_process
// #todo: Change name to kgwsRegisterDisplayServer()
int kgwsRegisterWindowServer (pid_t pid)
{
    int Status = 0;

//#todo
    //if (pid<0 || pid >= PROCESS_COUNT_MAX)
        //panic();

    // ?? Where ??
    if (kgws_status != 1){
        Status = 1;
        goto fail;
    }else{
        kgws_ws_PID = (pid_t) pid;
        kgws_ws_status = 1;
        goto done;
    };

fail:
    printk("kgwsRegisterWindowServer: fail\n");
done:
    return 0;
}

// Registrando o processo do window server.
// See: kpid.h
int register_ws_process(pid_t pid)
{
    if ( pid<0 || pid >= PROCESS_COUNT_MAX ){
        debug_print("register_ws_process: pid\n");
        return (int) -1;
    }
    if (__gpidWindowServer != 0){
        debug_print("register_ws_process: __gpidWindowServer\n");
        return (int) -1;
    }
    __gpidWindowServer = (pid_t) pid;
    return 0;
}


static int __gre_initialize_globals(void)
{
// Called by gre_initialize().

// Device
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print("__gre_initialize_globals: w h\n");
        // #bugbug
        // We don't know if 'panic' is working at this moment.
        panic      ("__gre_initialize_globals: w h\n");
    }

// Hotspot for 3D stuff.
    HotSpotX = (deviceWidth>>1);
    HotSpotY = (deviceHeight>>1);
    return 0;
}

//
// $
// INITIALIZATION
//

// called by ke.c
// #todo
// Maybe we can use some parametes here.
int gre_initialize(void)
{
    __gre_initialize_globals();
    // ...
    return 0;
}

//
// End
//

