// grprim.c 
// Primitives.
// 2020 - Created by Fred Nora.

#include "../gwsint.h"

// ??
// Swap two bytes
// It's used by the triangle rasterization o guess.
#define ____SWAP(x,y)  \
    do { (x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y); } while(0)

// See:
// https://wiki.osdev.org/3D_Renderer_Basics
// http://members.chello.at/easyfilter/bresenham.html
// http://math.hws.edu/graphicsbook/c2/s3.html
// http://math.hws.edu/graphicsbook/index.html
// ...

// vanishing point:
// tunel?
// magnification effect.
// two parallel lines convert into a single point.

// ================================
// Viewport 2d:
// A origem normalmente fica em top/left.
// É o 'raster', para fazer rasterização.
// Viewport is a rectangular display area 
// on the application window, which is measured 
// in screen's coordinates 
// (in pixels, with origin at the top-left corner). 
// A viewport defines the size and shape of the display area 
// to map the projected scene captured by the camera 
// onto the application window. 
// It may or may not occupy the entire screen.
// Viewport 3d:
// Então o viewpoint agora possui 3 coordenadas.
// In 3D graphics, a viewport is 3-dimensional 
// to support z-ordering, which is needed for 
// situations such as ordering of overlapping windows.
// Object world:
// O objeto tem seu proprio mundo e a origem fica 
// no centro da tela.
// Each object (or model or avatar) in a 3D scene 
// is typically drawn in its own coordinate system, 
// known as its model space (or local space, or object space). 
// See: https://www3.ntu.edu.sg/home/ehchua/programming/opengl/CG_BasicsTheory.html
// =============================

// #todo
// Create some configuration globals here
// int gUseSomething = TRUE;
// int gUseRaytracing = TRUE;
// ...

// ====
/*
 * Transformations: 
 *     Ex: Scaling, translation and rotation ...
 */

// translation
// x1 = x + e
// y1 = y + f
// translate( e, f )

// Scaling
// inflate, deflate.
// x1 = a * x
// y1 = d * y

// affine transform
// x1 = a*x + b*y + e
// y1 = c*x + d*y + f
// T(x,y) = ( a*x + b*y + e, c*x + d*y + f )

// Rotation
//x1 = cos(r) * x - sin(r) * y
//y1 = sin(r) * x + cos(r) * y

// shearing transform.
// tosquear, podar.
//x1 = x + b * y
//y1 = y
//---------
//x1 = x
//y1 = c * x + y


//Combining Transformations
//translate(4,0)
//rotate(90)

//Window-to-Viewport
//scale( width / (right-left), height / (bottom-top) );
//translate( -left, -top )
//-------
//x1 = width / (right-left) * (x-left)
//y1 = height / (bottom-top) * (y-top)


// =============================================================
// #projection:
// For orthographics projection there is no scaling factor.
// For perspective, we do have scaling.

// Camera and Perspective 
// camera:
// location, pointing at, pointing to top.
// .., lookat(view vector), top (upvector).
// perspective:
// view frustrum
// near, far ...
// from, to

// Device hotspot.
// 'Principle Point'.
static unsigned long HotSpotX=0;
static unsigned long HotSpotY=0;

// Window hotspot.
//static unsigned long WindowHotSpotX=0;
//static unsigned long WindowHotSpotY=0;

static void 
__transform_from_modelspace_to_screespace(
    int *res_x,
    int *res_y,
    int _x, int _y, int _z,
    int use_z_translation );
    
// =============================

/*
 * grInit:
 *     Initialize the 3d support.
 *     Create the hotspot to normalize the screen.
 *     The device context was created in the beginning of
 * of the initialization.
 *     To handle the device context we have the structures:
 * 'display' and 'screen'. The clients will use the data 
 * in these structures.
 */

int grInit(void)
{
    unsigned long deviceWidth  = gws_get_device_width();
    unsigned long deviceHeight = gws_get_device_height();

    server_debug_print("grInit:\n");

    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        //server_debug_print ("grInit: [FAIL] w h\n");
        printf             ("grInit: [FAIL] w h\n");
        exit(1);
    }

// center of the screen.
// #todo: 
// We need the option to put the hotspot 
// at the center of the window.
    HotSpotX = (deviceWidth >> 1);
    HotSpotY = (deviceHeight >> 1);

// == Projection =========
// Initialize the current projection.
// Change the view for the current projection.
    //server_debug_print("grInit: projection\n");
    projection_initialize();
// Changing the view for the current projection.
    gr_depth_range(CurrentProjection,0,40);

// == Camera ==========
// Initialize the current camera.
// Change some attributes for the current camera.
// The projection is a field in the camera's structure.
    //server_debug_print ("grInit: camera\n");
    camera_initialize();
    camera ( 
        -40, -40, 0,     // position vector
        -40,  40, 0,     // upview vector
         10,  10, 10 );  // lookat vector

    // ...

    //server_debug_print ("grInit: done\n");
    return 0;
}

/*
// #bugbug
// Not tested.
// negative numbers ??
// #todo: inclue libc file.
void 
gr_clamp( 
    struct gr_vec3D_d *vect, 
    int min_value, 
    int max_value);
void 
gr_clamp( 
    struct gr_vec3D_d *vect, 
    int min_value, 
    int max_value)
{
    if ( (void*) vect == NULL ){
        return;
    }

    vect->x = max(min_value, vect->x);
    vect->y = max(min_value, vect->y);
    vect->z = max(min_value, vect->z);
        
    vect->x = min(max_value, vect->x);
    vect->y = min(max_value, vect->y);
    vect->z = min(max_value, vect->z);
}
*/


// Configurando o viewport.
// Isso determina os limites onde podemos pintar com 2D. 
// Esse é nosso 'raster'.
// See: projection_initialize
void 
gwsViewport(
    int topleft_x, 
    int topleft_y, 
    int width, 
    int height)
{

// Esses valores não podem ser maiores
// que os valores da tela.

    if (topleft_x<0)
        topleft_x=0;
    if (topleft_y<0)
        topleft_y=0;

    if (width<0)
        width=0;
    if (height<0)
        width=0;

    // ...

    // #todo
    // call that routine.
    //projection_initialize(...)
}

// Limites da profundidade usada pelo raster.
// See: view().
void 
gwsDepthRange(
    int minZ,     // near
    int maxZ)     // far
{
// Changing the view for the current projection
    gr_depth_range(CurrentProjection, minZ, maxZ);
}

/*
//#test
void 
t3(
    int  x1,  int y1,  int z1,
    int *x2, int *y2, int *z2 );
void 
t3(
    int  x1,  int y1,  int z1,
    int *x2, int *y2, int *z2 )
{
    *x2 = (x1*z1);
    *y2 = (y1*z1);
    *z2 = (1 *z1);
}
*/

// worker: Used by grPlot0()
// Projection Transform - Perspective Projection
// Projection Transform - Orthographic Projection
// The x and y coordinates (in the range of -1 to +1) 
// represent its position on the screen, and the z value
// (in the range of 0 to 1) represents its depth, 
// i.e., how far away from the near plane.
// Transforma no 'world space' para o 'view port'.
// The world space, is common to all the objects. 
// But each object can have your own 'local space'.
// #
// Not standard ortographic projection.
static void 
__transform_from_modelspace_to_screespace(
    int *res_x,
    int *res_y,
    int _x, int _y, int _z,
    int use_z_translation )
{

// (0,0) represents the top/left corner in a 2d screen.
// The center of the screen in 2d is the hotspot.
// (0,0,0) represents the center of the screen in 3d
// (0,0,0) in 3d is also the hotspot.

// 3d
// save parameters. (++)
    int x  = (int) _x;  //1
    int y  = (int) _y;  //2
    //int x2 = (int) _y;  //3 #crazy
    int z  = (int) _z;  //4
    int hotspotx = (int) (HotSpotX & 0xFFFFFFFF);
    int hotspoty = (int) (HotSpotY & 0xFFFFFFFF);
// 2d:
// final result.
    int X=0;
    int Y=0;
    //int UseZTranslation = TRUE;
    //int UseZTranslation = FALSE;
    int UseZTranslation = (int) use_z_translation;
    // Register z value into the z buffer.
    //int RegisterZValue=FALSE;

// The world space.
// (HotSpotX,HotSpotY,0)
// This is the origin of the 'world space'.
// model space.
// Been the reference for all the 'object spaces'.

// ===================================================
// X::

// --------------------
// z maior ou igual a zero.
//    |
//    ----
//
    if (z >= 0)
    {
        // x positivo, para direita.
        if (x >= 0 ){
            X = (int) ( hotspotx + x );
        }
        // x negativo, para esquerda.
        if (x < 0 ){ x = abs(x);   
            X = (int) ( hotspotx - x );
        }
        goto done;
    }

// --------------------
// z negativo
//  _
//   |
//
    if (z < 0)
    {
        // x positivo, para direita.
        if (x >= 0){
            X = (int) (hotspotx + x);
        }
        // x negativo, para esquerda.
        if (x < 0){  x = abs(x); 
            X = (int) (hotspotx - x);
        }
        goto done;
    }

done:

// ===================================================
// Y::
     // y positivo, para cima.
     if ( y >= 0 ){
         Y = (int) ( hotspoty - y );
     }
     // y negativo, para baixo
     if ( y < 0 ){ y = abs(y);
         Y = (int) ( hotspoty + y );
     }

// ===================================================
// Z::
    if (UseZTranslation == TRUE)
    {
        // z é positivo para todos os casos 
        // onde z é maior igual a 0.
        if(z >= 0){ 
            X = (X + z); 
            Y = (Y - z); 
        }
        // z é módulo para todos os casos 
        // em que z é menor que 0.
        if(z < 0){ z = abs(z);
            X = (X - z); 
            Y = (Y + z); 
        }
    }

// ===================================================
// Return the values
    if ((void*) res_x != NULL){
        *res_x = (int) X;
    }
    if ((void*) res_y != NULL){
        *res_y = (int) Y;
    }

    return;
}

/*
 * grPlot0:
 * Low leve routine to plot a pixel.
 * Viewport Transformation.
 * Origin at center of the device screen.
 * #todo: Plot into a 'normalized' 2d rater screen.
 * #new: Plotting into a clipping window.
 * History:
 *      2020 - Created by Fred Nora.
 */
// Ortographic view volume.
// Transformation:
// Estamos tranformando de um 'object space' com origem 
// no centro da tela para um viewport que ocupa a tela toda, 
// com origem no canto superior esquerdo.
// left hand orientation
// z+ on top/right corner.
// We use:
// Left-hand System (LHS) 
// LHS is clockwise (CW).
// Same as Microsoft Direct3D.
// See: https://en.wikipedia.org/wiki/Direct3D
// window ?
// Essa rotina pode pintar em qualquer posição 
// da tela do dispositivo. 
// Com origem no centro da tela.
// Aceitamos valores negativos e positivos.
// O limite máximo será modular.
// 3D fullscreen, origin in center.
// ==================================================
// O 'object space' esta no centro da tela. (0,0,0). 
// Essa rotina plota o pixel considerando o viewport com 
// a origem no canto superior esquerdo da tela. (0,0).
// Cada objeto pode ter seu proprio 'object space'
// e o 'world' pode ter seu 'world space' no centro do
// viewport.
// Por enquanto tanto o 'object space' quanto o 'world space'
// estão no centro da tela, em (0,0,0), e o viewport
// é o proprio raster que compreende a tela toda.
// Screen Coordinate System - 
// This 2D coordinate system refers to the physical coordinates 
// of the pixels on the computer screen, based on 
// current screen resolution. ( E.g. 1024x768 )
// Viewport Coordinate System - This coordinate system refers 
// to a subset of the screen space where the model window 
// is to be displayed. Typically the viewport will occupy 
// the entire screen window, or even the entire screen, 
// but it is also possible to set up multiple smaller viewports 
// within a single screen window.
// See:
// https://www.cs.uic.edu/~jbell/CourseNotes/ComputerGraphics/Coordinates.html
// #important
// The main goal here is transforming
// the ortographic view volume into
// a canonical view volume, without 
// using matrices operations.
// See: https://www.youtube.com/watch?v=U0_ONQQ5ZNM
// #todo
// Qual dc estamos usando.
// Se os argumentos não indicarem um, então devemos usar
// o dc default.
// #todo
// Devemos interagir com as rotinas de janela
// e transformarmos tudo em uma coisa só.
// E as rotinas de janela são rotinas 2d,
// mas pode levar em conta apenas o dc padrão
// ou outro destinado a pintar em lugares especificos.
// #bugbug
// Não sei se essa coisa da transformação está certa,
// mas é divertido.
// O z-buffer no dc ou na janela vai nos ajudar a decidirmos
// se devemos ou não pintar o pixel, para economizarmos pixels.
// #
// Not standard ortographic projection.
// Once the camera is positioned and oriented, 
// we need to decide what it can see 
// (analogous to choosing the camera's field of view 
// by adjusting the focus length and zoom factor), and 
// how the objects are projected onto the screen. 
// This is done by selecting a projection mode 
// (perspective or orthographic) and specifying a 
// viewing volume or clipping volume. 
// Objects outside the clipping volume are clipped out 
// of the scene and cannot be seen.
// see:
// https://www3.ntu.edu.sg/home/ehchua/programming/opengl/CG_BasicsTheory.html
// IN:
//   + Clipping window
//   + x, y, z
//   + Color
//   + ROP

int 
grPlot0 ( 
    struct gws_window_d *clipping_window,
    int z, int x, int y, 
    unsigned int color,
    unsigned long rop )
{

// Viewport Transform:
// depth buffer?
// In 3D graphics, a viewport is 3-dimensional 
// to support z-ordering, which is needed for 
// situations such as ordering of overlapping windows.
// #todo
// We can use a 'clipping window' and draw only inside this window.
// If the clipping window is NULL, so we need to use the root window.
// #todo
// This is a work in progress

    int finalx=0;
    int finaly=0;
    // Draw flag.
    int Draw = TRUE;
    int UsingDepthBuffer = FALSE;
    int UsingAlphaBlending = FALSE;
// clipping window support.
    struct gws_window_d *w;
// False. But it turns into TRUE 
// if we have a valid window pointer.
    int UseClipping = FALSE;
// left hand orientation
// z+ on top/right corner.
    int UseZTranslation=TRUE;
    //int UseZTranslation=FALSE;

//-------------------------------------
// tmp: zNear zFar
// Clipping
    int zNear = 0;
    int zFar = 80;
    if ((void*) CurrentProjection != NULL){
        zNear = (int) CurrentProjection->zNear;
        zFar  = (int) CurrentProjection->zFar;
    }
    if (z < zNear)
        goto fail;
    if (z >= zFar)
        goto fail;
//-------------------------------------

//-------------------------------------
// #todo
// We need a z-buffer or (depth buffer)
// it is used to depth testing.
// it discards some unnecessary covered plots. 
// Normalized Device Coordinates (NDC)
// We are using or own ndc style.
// Maybe it is different for diferent systems.
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
    //unsigned long zBaseX=0;
    //unsigned long zBaseY=0;
//
// The clipping window.
//
// #todo
// If the clipping window is invalid, 
// so we're gonna use the root window.
// #todo:
// Maybe we need to use the device context structure,
// or something like that.

// Window validation.
    //UseClipping = FALSE;
    if ((void*) clipping_window != NULL)
    {
        if ( clipping_window->used == TRUE && 
             clipping_window->magic == 1234 )
        {
            UseClipping = TRUE;
            w = (struct gws_window_d *) clipping_window;
        }
    }

//
// Device screen structure
//

// See: screen.h

    // #debug
    if ((void *) DeviceScreen == NULL){
        Draw = FALSE;
        printf("grPlot0: DeviceScreen\n");
        exit(1);
    }

// #todo
// precisamos checar algumas globais, como HotSpotX e HotSpotY.
// Usaremos a janela chamada screen se nenhuma outra foi indicada.
//gui->screen

// Transformation:
// Estamos tranformando de um 'object space' com origem 
// no centro da tela para um viewport que ocupa a tela toda, 
// com origem no canto superior esquerdo.

// #todo
// um worker que faça essa rotina de transformação.
// Chamaremos ele agora e pegaremos os valores ja transformados.
// IN: return x, return y, x, y, z, fix_orientation,

    __transform_from_modelspace_to_screespace( 
        (int *) &finalx,
        (int *) &finaly,
        x, y, z,
        UseZTranslation );

// Draw and clipping.
// #todo
// Talvez possamos retornar '0'
// se a flag indicar que não precisava desenhar.

    if (Draw != TRUE)
        goto fail;

// #todo: 
// We need to check the window limits
// if we are drawing inside a given window.
// Checking the device screen limits.
// #bugbug
// Já fizemos isso logo acima?

    if (finalx<0){ goto fail; }
    if (finaly<0){ goto fail; }
    if ( 0 <= finalx < DeviceScreen->width && 
         0 <= finaly < DeviceScreen->height )
    {
        if ( UsingAlphaBlending == TRUE )
        {
            // #todo
            // Get the color and modify it.
            //color = get??()
        }

        // Se NÃO temos uma janela válida.
        // device screen
        // 2D, No clipping or transformation.
        if ( UseClipping == FALSE )
        {
            // Plot pixel into the raster.
            // The origin is top/left of the viewport. (0,0).
            // IN: color, x, y, rop
            libdisp_backbuffer_putpixel( 
                (unsigned int) color, finalx, finaly, rop ); 

            //return 0;
        }

        // Se temos uma janela válida.
        if ( UseClipping == TRUE )
        {
            if ( finalx >= w->absolute_x  && 
                 finalx <= w->absolute_right &&
                 finaly >= w->absolute_y   && 
                 finaly <= w->absolute_bottom )
                 {
                     // Plot pixel into the raster.
                     // The origin is top/left of the viewport. (0,0).
                     // IN: color, x, y, rop
                     libdisp_backbuffer_putpixel(
                         (unsigned int) color, finalx, finaly, rop ); 

                     //return 0;
                 }
        }

        // #todo
        // This is a work in progress
            
        //if( UsingDepthBuffer == TRUE ){
        //    depth_buffer[ offset ] = Z;
        //}

        // OK
        return 0;
    }

fail:
    return (int) (-1);
}

// Plot with graphics effects.
int 
grPlot1 ( 
    struct gws_window_d *clipping_window,   
    int x, int y, int z, 
    unsigned int color,
    unsigned long rop,
    unsigned long flags )
{

//#todo
//Describe the flags.

    int xValue = 0;
    int yValue = 0;
    int zValue = 0;
    unsigned int colorValue=0;
    int fBlack=FALSE;  // black pixel
    int fNoZBuffer = FALSE;
    // ...

    // No graphics effects;
    if (flags == 0){
        xValue = x;
        yValue = y;
        zValue = z;
        colorValue = (unsigned int) color;
        goto PlotPixel;
    }

    // #todo #test
    // plot pixel using the caes in fx.
    
    if ( flags & 0x00000001 )
    {
        fBlack = TRUE;
        colorValue = (unsigned int) 0x00000000;
    }

    if ( flags & 0x00000020 )
    {
        fNoZBuffer = TRUE;
        zValue = 0;
    }

    // ...

PlotPixel:
    return (int) grPlot0 ( 
                     clipping_window, 
                     zValue, xValue, yValue, 
                     (unsigned int) colorValue,
                     rop );
}

// #todo
// See: gwsProcedure(), service 2040 in main.c

int serviceGrPlot0 (void)
{
    unsigned long *message_address = 
        (unsigned long *) &__buffer[0];

    unsigned long x=0;
    unsigned long y=0;
    unsigned long z=0;
    unsigned long color=0;

    //server_debug_print("serviceGrPlot0: [TODO] \n");

// =================================
// Arguments:
// Sempre começa do 10 para rotinas 3D.
    x      = message_address[10];
    y      = message_address[11];
    z      = message_address[12]; 
    color  = message_address[13];
// =================================

// #todo:
// pegar os argumentos no buffer e chamar a rotina de plotagem de pixel.
// Acho que esse serviço nao retorna uma mensagem ao cliente.
    grPlot0 ( 
        NULL, 
        (int) z, (int) x, (int) y, 
        (unsigned int) color,
        0 );  //#todo: rop

    return 0;
}

/*
void plotLine(int x0, int y0, int x1, int y1);
void plotLine(int x0, int y0, int x1, int y1)
{
    int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;                          // error value e_xy 

    for (;;) {                                    // loop 
        setPixel(x0,y0);
        e2 = 2*err;
        if (e2 >= dy) {                           // e_xy+e_x > 0
            if (x0 == x1) break;
            err += dy; x0 += sx;
        }
        if (e2 <= dx) {                           // e_xy+e_y < 0 
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }
}
*/

// plotLine3d:
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
        grPlot0 ( NULL, z0, x0, y0, color, 0 );
        if (i-- == 0) { 
            break;
        }
        x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; }
        y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; }
        z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; }
    };
}

// plotLine3dEx:
// Bresenham in 3D
// The algorithm could be extended to three (or more) dimensions.
void 
plotLine3dEx (
    struct gws_window_d *window,
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color,
    unsigned long rop )
{

    if ((void*) window == NULL)
        return;
    if (window->magic != 1234)
        return;

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

    for (;;) 
    {
        grPlot0( 
            window, 
            z0, x0, y0, 
            color, 
            rop );

        if (i-- == 0) {
            break;
        }

        x1 -= dx; 
        if (x1 < 0) { 
            x1 += dm; 
            x0 += sx;
        }

        y1 -= dy;
        if (y1 < 0) { 
            y1 += dm; 
            y0 += sy; 
        }

        z1 -= dz;
        if (z1 < 0) {
            z1 += dm; 
            z0 += sz; 
        }
    };
}

// #todo
// plot line given two colors.
// interpolation ?
void 
plotLine3d2 (
    int x0, int y0, int z0, unsigned long color1,
    int x1, int y1, int z1, unsigned long color2,
    int flag )
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

// nothing for now;
// interpolation flag.
    flag=0;

//
// Loop
//

    for (;;) {

        grPlot0 ( NULL, z0, x0, y0, color1, 0 );
        //grPlot0 ( NULL, z0, x0, y0, color2, 0 );

        if (i-- == 0) 
            break;
        x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; } 
        y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; } 
        z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; } 
    };
}

// IN:
// The upper-left corner
// lower-right corner. 
// color
void
rectangle_from_two_points (
    int left, int top, 
    int right, int bottom,
    unsigned int color )
{

    // #todo
    // Check validation
    // if (left<0 ...

// cima
    plotLine3d ( 
        left,top,0, 
        right,top,0, 
        color );

// baixo
    plotLine3d ( 
        left,bottom,0, 
        right,bottom,0, 
        color );

// esquerda
    plotLine3d ( 
        left,top,0, 
        left,bottom,0, 
        color );

// direita
    plotLine3d ( 
        right,top,0, 
        right,bottom,0, 
        color );
}

// __rectangleZZ:
// Worker
// Draw 4 3d lines, not filled rectangle.
void __rectangleZZ(struct gr_rectangle_d *rect)
{
    if ((void*) rect == NULL){
        return;
    }

// #todo
// Check the validation of the values in the structure.
// points
//  0  1
//  3  2
//#bugbug
// We need to create a routine with colors in both points.

//
// Draw 4 lines.
//

// cima
    //plotLine3d2 ( left, top,  z, right, top, z, color, flag );
    plotLine3d2 ( 
        rect->p[0].x, rect->p[0].y, rect->p[0].z, rect->p[0].color,
        rect->p[1].x, rect->p[1].y, rect->p[1].z, rect->p[1].color, 
        0 );

// baixo
     //plotLine3d2 ( left, bottom, z, right,bottom, z, color, flag );
     plotLine3d2 ( 
         rect->p[3].x, rect->p[3].y, rect->p[3].z, rect->p[3].color,
         rect->p[2].x, rect->p[2].y, rect->p[2].z, rect->p[2].color, 
         0 );

// esquerda
     //plotLine3d2 ( left, top, z, left, bottom, z, color, flag );
     plotLine3d2 ( 
         rect->p[0].x, rect->p[0].y, rect->p[0].z, rect->p[0].color,
         rect->p[3].x, rect->p[3].y, rect->p[3].z, rect->p[3].color, 
         0 ); 

// direita
     //plotLine3d2 ( right,  top, z, right, bottom, z, color, flag );
     plotLine3d2 ( 
         rect->p[1].x, rect->p[1].y, rect->p[1].z, rect->p[1].color,
         rect->p[2].x, rect->p[2].y, rect->p[2].z, rect->p[2].color, 
         0 );
}

int grRectangle(struct gr_rectangle_d *rect)
{
    if ((void*) rect == NULL)
        goto fail;

// #todo
// Use options
// Create a flag in the fuction's parameters.

// Option 1:
// worker
// 4 3d lines, not filled.
    __rectangleZZ(rect);

// Option 2:
// ...

    return 0;
fail:
    return (int) -1;
}

// rectangle_ras3D:
// Rectangle rasterization using 3d lines.
// It's not the standard quad rasterization routine
// using the viewport 2d coordenates.
// It uses the model 3d coordenates.
// It applies only on few cases.
// IN:
//   + upper-left corner, 
//   + lower-right corner,
//   + color

void
rectangle_ras3D (
    int left,  int top,    int z0,
    int right, int bottom, int z1,
    unsigned int color )
{
    register int Line=0;

// #todo
// No caso de inclinações da linha,
// precisamos considerar que temos dois 'top' diferentes.
// precisamos receber mais parâmetros de função,
// ou criarmos uma outra funções que tenha tais parâmetros,
// ficando essa somente para retângulos não inclinados.
// #todo
// isso ta bem confuso.
// Estamos no eixo y, para cima, maior que 0.
// Entao top tem um valor maior que bottom.

    if (top  <= bottom){return;}
    if (left >= right) {return;}

// #todo
// device limits.

    // Começa com a linha de cima.
    // Estamos no eixo y, para cima, maior que zero.
    // Enquanto for maior que o bottom.
    for ( Line=top;
          Line >= bottom; 
          Line-- )
    {
        // Pintamos uma linha por vez, horizontalmente.
        // IN: 
        // x0,y0,z0, x1,y1,z1, color.
        plotLine3d ( 
            left,  Line, z0,   // vertex 1 
            right, Line, z1,   // vertex 2 
            color );
    };
}

// Fill a triangle - Bresenham method.
// Original from 
// http://www.sunshine2k.de/\
// coding/java/TriangleRasterization/TriangleRasterization.html
int 
fillTriangle0(
    int x1, int y1,
    int x2, int y2,
    int x3, int y3, 
    unsigned int c,
    unsigned long rop )
{
// Triangle rasterization.
// 2D.

// #suspended.
// We are NOT counting for now.
// Number of changed pixels.
    int npixels=0;

    int t1x=0;
    int t2x=0;
    int y=0;
    int minx=0;
    int maxx=0;
    int t1xp=0;
    int t2xp=0;
    int changed1 = FALSE;
    int changed2 = FALSE;
    int signx1=0;
    int signx2=0;
    int dx1=0;
    int dy1=0;
    int dx2=0;
    int dy2=0;
    int e1=0; 
    int e2=0;

// Sort vertices
    if (y1>y2) { ____SWAP(y1,y2); ____SWAP(x1,x2); }
    if (y1>y3) { ____SWAP(y1,y3); ____SWAP(x1,x3); }
    if (y2>y3) { ____SWAP(y2,y3); ____SWAP(x2,x3); }

// Starting points
    t1x = t2x = x1; 
    y=y1; 

    dx1 = (int)(x2 - x1);
    if(dx1<0) { dx1=-dx1; signx1=-1; } else { signx1=1; }
    dy1 = (int)(y2 - y1);
 
    dx2 = (int)(x3 - x1); 
    if(dx2<0) { dx2=-dx2; signx2=-1; } else { signx2=1; }
    dy2 = (int)(y3 - y1);

// swap values
    if (dy1 > dx1){  ____SWAP(dx1,dy1);  changed1 = TRUE;  }
    if (dy2 > dx2){  ____SWAP(dy2,dx2);  changed2 = TRUE;  }

    e2 = (int)(dx2>>1);

// Flat top, just process the second half.
    if (y1==y2){
        goto next;
    }
    
    e1 = (int)(dx1>>1);

    register int i=0;
    for (i=0; i<dx1;)
    {
        t1xp=0; t2xp=0;
        if(t1x<t2x) { minx=t1x; maxx=t2x; }
        else        { minx=t2x; maxx=t1x; }

        // Process first line until 
        // y value is about to change.
        while (i<dx1) 
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1) 
            {
                e1 -= dx1;
                if (changed1) 
                    t1xp=signx1;//t1x += signx1;
                else 
                    goto next1;
            };

            if (changed1) 
                break;
            else 
                t1x += signx1;
        };

    // Move line
    next1:
        // Process second line 
        // until y value is about to change
        while (1)
        {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp=signx2;  //t2x += signx2;
                else          goto next2;
            };
            if (changed2) break;
            else          t2x += signx2;
        };

    next2:

        if(minx>t1x) minx=t1x; 
        if(minx>t2x) minx=t2x;
        
        if(maxx<t1x) maxx=t1x; 
        if(maxx<t2x) maxx=t2x;
        
        // #test
        //if(minx<0) minx=0; 

        // Draw line from min to max points found on the y
        // #todo: return the number of pixels changed.
        // #todo: The counter is supended.
        //npixels += grBackbufferDrawHorizontalLine( minx, y, maxx, c, rop );
        grBackbufferDrawHorizontalLine( minx, y, maxx, c );

        // Now increase y
        if(!changed1){ t1x += signx1; }
        t1x += t1xp;
        if(!changed2){ t2x += signx2; }
        t2x+=t2xp;
        
        y += 1;
        
        if(y==y2){
            break;
        }
    };

// --------------------------------------------
// Second half

    next:

    dx1 = (int)(x3 - x2); 
    if (dx1<0){ 
        dx1 = -dx1; 
        signx1=-1; 
    }else{ 
        signx1=1; 
    };

    dy1 = (int)(y3 - y2);
    t1x=x2;

// swap values
    if (dy1 > dx1){
        ____SWAP(dy1,dx1);
        changed1=TRUE;
    }else{ 
        changed1=FALSE;
    };

    e1 = (int)(dx1>>1);

    register int ii=0;
    for (ii=0; ii<=dx1; ii++)
    {
        t1xp=0; 
        t2xp=0;
        if(t1x<t2x) { minx=t1x; maxx=t2x; }
        else        { minx=t2x; maxx=t1x; }
        // process first line until y value is about to change
        while(ii<dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1) { t1xp=signx1; break; }//t1x += signx1;
                else goto next3;
            };
            if (changed1) break;
            else          t1x += signx1;
            if (ii<dx1)
                ii++;
        };
    next3:
        // process second line until y value is about to change
        while (t2x!=x3) 
        {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if(changed2) t2xp=signx2;
                else          goto next4;
            };
            if (changed2)  break;
            else           t2x += signx2;
        };
    next4:

        if(minx>t1x) minx=t1x; 
        if(minx>t2x) minx=t2x;

        if(maxx<t1x) maxx=t1x; 
        if(maxx<t2x) maxx=t2x;

        // Draw line from min to max points found on the y
        // #todo: return the number of pixels changed.
        // #todo: the counter is suspended.
        // see: line.c
        //npixels += grBackbufferDrawHorizontalLine( minx, y, maxx, c, rop );
        grBackbufferDrawHorizontalLine( minx, y, maxx, c );
        
        // Now increase y
        if(!changed1) { t1x += signx1; }
        t1x+=t1xp;
        if(!changed2) { t2x += signx2; }
        t2x+=t2xp;
        y += 1;
        if (y>y3){
            return (int) npixels;
        }
    };

    return (int) npixels;
}




// xxxTriangleZ:
// ...
int xxxTriangleZ(struct gr_triangle_d *triangle)
{
    if ((void*) triangle == NULL){
        goto fail;
    }

// Draw:
// Circular, 
// sentido horario, 
// começando pelo ponto de cima.

    plotLine3d (
        triangle->p[0].x, triangle->p[0].y, triangle->p[0].z, 
        triangle->p[1].x, triangle->p[1].y, triangle->p[1].z, 
        triangle->p[1].color );

    plotLine3d (
        triangle->p[1].x, triangle->p[1].y, triangle->p[1].z, 
        triangle->p[2].x, triangle->p[2].y, triangle->p[2].z, 
        triangle->p[2].color );

    plotLine3d (
        triangle->p[2].x, triangle->p[2].y, triangle->p[2].z, 
        triangle->p[0].x, triangle->p[0].y, triangle->p[0].z, 
        triangle->p[0].color );

    return 0;
fail:
    return (int) -1;
}

// grTriangle:
// ...
int grTriangle(struct gr_triangle_d *triangle)
{
    register int Status=0;

    if ((void*) triangle == NULL){
        goto fail;
    }
    Status = (int) xxxTriangleZ(triangle);
    return Status;
fail:
    return (int) -1;
}

// xxxPolygonZ:
// Polyline
// O segundo ponto da linha 
// vira o primeiro ponto da próxima linha.
int xxxPolygonZ(struct gr_polygon_d *polygon)
{
    register int i=0;
    int NumberOfElements=0;
    int Max = 32;
    // list of polygon pointers.
    unsigned long *list = 
        (unsigned long *) polygon->list_address;

    // two vectors to create a line.
    struct gr_vec3D_d *v1;
    struct gr_vec3D_d *v2;

    int PolygonType=0;

// structure
    if ((void*) polygon == NULL){
        goto fail;
    }

// list
    if ((void*) list == NULL){
        goto fail;
    }

// Number of elements
    NumberOfElements = polygon->n;
    if (NumberOfElements > Max){
        goto fail;
    }

// polygon type
    PolygonType = polygon->type;
    switch (PolygonType){
    case POLYGON_POLYPOINT:  goto do_polypoint;  break;
    case POLYGON_POLYLINE:   goto do_polyline;   break;
    default:
        goto fail;
        break;
    };

// Draw polypoint
do_polypoint:

    // #todo
    // Nothing for now

    for ( i=0; i<NumberOfElements; i++ )
    {
        v1 = (struct gr_vec3D_d *) list[i];
        if ((void*) v1 == NULL){ 
            //server_debug_print(">>>> BREAK\n");
            break;
        }

        // draw
        grPlot0 ( 
            NULL, 
            v1->z, v1->x, v1->y, 
            v1->color, 0 );
    };
    return 0;

    // Draw polyline
do_polyline:

    for ( i=0; i<NumberOfElements; i++ )
    {
        // get the first element of two.
        v1 = (struct gr_vec3D_d *) list[i];
        // get the second element of two.
        v2 = (struct gr_vec3D_d *) list[i+1];
    
        if ( (void*) v1 == NULL || 
             (void*) v2 == NULL )
        { 
            //server_debug_print(">>>> BREAK\n");
            break; 
        }

        //server_debug_print(">>>> DRAW LINE\n");
    
        // Plot
        
        plotLine3d (
            v1->x, v1->y, v1->z,  // first point
            v2->x, v2->y, v2->z,  // second point
            v1->color );
    };
    return 0;

fail:
    //server_debug_print("xxxPolygonZ: fail\n");
    return -1;
}

// xxxDrawCubeZ:
// #test
// >>>> wired cube <<<< 
// "with ugly rasterization in some of the fazes"
// This rasterization uses lines and applies only in the faces
// with not inclination.
// #todo
// Create function parameters to select if we will use some
// kind of rasterization or not.
// #todo
// We can create a function only for wired cube, 
// and another one for cube with some kind of rasterization.

int xxxDrawCubeZ(struct gr_cube_d *cube)
{

// #todo: 
// Comment these variables.

    int h=0;
    int d=0;
    int i=0;

// #todo
// Maybe we will receive a function parameter for that.
    int UseRasterization = TRUE;

    if ((void*) cube == NULL){
        goto fail;
    }

// #todo
// Temos que rever a forma em que estamos usado os parâmetros
// para pintarmos o objeto cubo.
// Tudo aqui ainda está bem improvisado.


// ===========
// north
// Primeiro desenha o north porque ele sera sobreposto 
// pelo sul nessa camera.
// -- back face ----------------------------------------
// north - back
// Retângulo com os 4 pontos de trás.

    plotLine3d (  // cima
        cube->p[4].x, cube->p[4].y, cube->p[4].z, 
        cube->p[5].x, cube->p[5].y, cube->p[5].z, cube->p[4].color );
    plotLine3d (  // direita
        cube->p[5].x, cube->p[5].y, cube->p[5].z, 
        cube->p[6].x, cube->p[6].y, cube->p[6].z, cube->p[5].color );
    plotLine3d (  // baixo 
        cube->p[7].x, cube->p[7].y, cube->p[7].z, 
        cube->p[6].x, cube->p[6].y, cube->p[6].z, cube->p[7].color );
    plotLine3d (  // esquerda
        cube->p[4].x, cube->p[4].y, cube->p[4].z, 
        cube->p[7].x, cube->p[7].y, cube->p[7].z, cube->p[4].color );

// #improviso: 
// Isso é um teste de rasterização, não usando triangulo.
// #todo:
// Temos que criar a tasterização via retângulos.
// Isso só funciona para retângulos não inclinados.
// p4 = left top 
// p6 = right bottom
    
    // north rasterization
    if (UseRasterization == TRUE)
    {
        rectangle_ras3D ( 
            cube->p[4].x, cube->p[4].y, cube->p[4].z, 
            cube->p[6].x, cube->p[6].y, cube->p[6].z, 
            (unsigned int) cube->p[4].color  );
    }

//====================
// -- top face ----------------------------------------
    // 1st line 
    plotLine3d (cube->p[0].x, cube->p[0].y, cube->p[0].z, 
                cube->p[4].x, cube->p[4].y, cube->p[4].z, cube->p[0].color );
    // 2nd line
    plotLine3d (cube->p[1].x, cube->p[1].y, cube->p[1].z, 
                cube->p[5].x, cube->p[5].y, cube->p[5].z, cube->p[1].color );

//====================
// -- bottom face -------------------------------------
    // 1rd line
    plotLine3d (cube->p[2].x, cube->p[2].y, cube->p[2].z, 
                cube->p[6].x, cube->p[6].y, cube->p[6].z, cube->p[2].color );
    // 2th line 
    plotLine3d (cube->p[3].x, cube->p[3].y, cube->p[3].z, 
                cube->p[7].x, cube->p[7].y, cube->p[7].z, cube->p[3].color );

//=================================================================

// left 
// Isso só funciona para retângulos não inclinados.
// p0 = left top da frente 
// p7 = left bottom de trás
    //left raterization?
    if (UseRasterization == TRUE)
    {
        rectangle_ras3D ( 
            cube->p[0].x, cube->p[0].y, cube->p[0].z, 
            cube->p[7].x, cube->p[7].y, cube->p[7].z, 
            cube->p[0].color  );

    }

// right
// Isso só funciona para retângulos não inclinados.
// p1 = right top da frente 
// p7 = right bottom de trás
    // right rasterization?
    if (UseRasterization == TRUE)
    {
        rectangle_ras3D( 
            cube->p[1].x, cube->p[1].y, cube->p[1].z, 
            cube->p[6].x, cube->p[6].y, cube->p[6].z, 
            cube->p[1].color  );
    }

    // tampa de baixo
    //bottom fail
    //rectangle_ras3D ( 
    //    cube->p[3].x, cube->p[3].y, cube->p[3].z, 
    //    cube->p[6].x, cube->p[6].y, cube->p[6].z,
    //    cube->p[2].color  );

    // tampa de cima.
    // top fail
    //rectangle_ras3D ( 
    //    cube->p[4].x, cube->p[4].y, cube->p[4].z, 
    //    cube->p[0].x, cube->p[0].y, cube->p[0].z,
    //    cube->p[3].color  );

//==========
// -- front face ----------------------------------------
// south - front
// Retângulo com os 4 pontos da frente.
    plotLine3d (  // cima
        cube->p[0].x, cube->p[0].y, cube->p[0].z, 
        cube->p[1].x, cube->p[1].y, cube->p[1].z, cube->p[0].color );
    plotLine3d (  // direita
        cube->p[1].x, cube->p[1].y, cube->p[1].z, 
        cube->p[2].x, cube->p[2].y, cube->p[2].z, cube->p[1].color );
    plotLine3d (  // baixo
        cube->p[3].x, cube->p[3].y, cube->p[3].z,
        cube->p[2].x, cube->p[2].y, cube->p[2].z, cube->p[3].color );
    plotLine3d (  // esquerda
        cube->p[0].x, cube->p[0].y, cube->p[0].z, 
        cube->p[3].x, cube->p[3].y, cube->p[3].z, cube->p[0].color );

// #improviso: 
// Isso é um teste de rasterização, não usando triangulo.
// #todo:
// Temos que criar a tasterização via retãngulos.
// Isso só funciona para retângulos não inclinados.
// p0 = left top 
// p2 = right bottom

    // south rasterization
    if (UseRasterization == TRUE){
        rectangle_ras3D (  
            cube->p[0].x, cube->p[0].y, cube->p[0].z, 
            cube->p[2].x, cube->p[2].y, cube->p[2].z, 
            cube->p[0].color );
    }

    // ok
    return 0;
fail:
    return (int) -1;
}

// sevice 2041
// Called by gwsProcedure.
int serviceGrCubeZ(void)
{
    //#deprecated
    return -1;
}

// sevice 2042
int serviceGrRectangle(void)
{
    //#deprecated
    return -1;
}

// plotCircle:
// Circle
// This is an implementation of the circle algorithm.
// ?? what means 'm' ???
void 
plotCircle ( 
    int xm, 
    int ym, 
    int r, 
    unsigned int color )
{

    /* II. Quadrant */ 
    //int x = -r, y = 0, err = 2-2*r; 

    // loop
    register int x = -r;

    int y = 0;
    int err = (2-(2*r));

    do {

    //setPixel(xm-x, ym+y); /*   I. Quadrant */
    //setPixel(xm-y, ym-x); /*  II. Quadrant */
    //setPixel(xm+x, ym-y); /* III. Quadrant */
    //setPixel(xm+y, ym+x); /*  IV. Quadrant */

    grPlot0 ( NULL, 0, xm-x, ym+y, color, 0);
    grPlot0 ( NULL, 0, xm-y, ym-x, color, 0);
    grPlot0 ( NULL, 0, xm+x, ym-y, color, 0);
    grPlot0 ( NULL, 0, xm+y, ym+x, color, 0);

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

// plotCircleZ:
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

    grPlot0 ( NULL, z, xm-x, ym+y, color, 0);
    grPlot0 ( NULL, z, xm-y, ym-x, color, 0);
    grPlot0 ( NULL, z, xm+x, ym-y, color, 0);
    grPlot0 ( NULL, z, xm+y, ym+x, color, 0);

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
 //credits: uVGA
void __fillCircle(int xm, int ym, int r, int color)
{
    int x, y;

    for (y = -r; y <= r; y++)
    {
		for (x = -r; x <= r; x++)
		{
			if ((x * x) + (y * y) <= (r * r))
			{
				drawHLine(ym + y, xm + x, xm - x, color);
				if(y != 0)
					drawHLine(ym - y, xm + x, xm - x, color);
				break;
			}
		}
    };
}
*/

/*
void uVGA::fillEllipse(int x0, int y0, int x1, int y1, int color)
{
	int x;
	int y;
	int half_height = abs(y0 - y1) / 2;
	int half_width = abs(x0 - x1) / 2;
	int center_x = (x0 < x1 ? x0 : x1) + half_width;
	int center_y = (y0 < y1 ? y0 : y1) + half_height;

	for(y = -half_height; y <= 0; y++)
	{
		for(x = -half_width; x <= 0; x++)
		{
			if( (x * x * half_height * half_height + y * y * half_width * half_width) <= (half_height * half_height * half_width * half_width))
			{
				drawHLine(center_y + y, center_x + x, center_x - x, color);
				if(y != 0)
					drawHLine(center_y - y, center_x + x, center_x - x, color);
				break;
			}
		}
	}
}
*/


// plotEllipseRect:
// Ellipse
// Plots an ellipse inside a specified rectangle.
void 
plotEllipseRect (
    int x0, int y0, 
    int x1, int y1, 
    unsigned long color )
{
    int a = abs(x1-x0), b = abs(y1-y0), b1 = b&1;  /* values of diameter */
    long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a;      /* error increment */
    long err = dx+dy+b1*a*a, e2;                   /* error of 1.step */

    if (x0 > x1) { x0 = x1; x1 += a; }    /* if called with swapped points */
    if (y0 > y1) y0 = y1;                 /* .. exchange them */
    y0 += (b+1)/2; y1 = y0-b1;            /* starting pixel */
    a *= 8*a; b1 = 8*b*b;

    do {
       grPlot0 ( NULL, 0, x1, y0, color, 0);  //   I. Quadrant
       grPlot0 ( NULL, 0, x0, y0, color, 0);  //  II. Quadrant
       grPlot0 ( NULL, 0, x0, y1, color, 0);  // III. Quadrant
       grPlot0 ( NULL, 0, x1, y1, color, 0);  //  IV. Quadrant

       e2 = (2*err);
       if (e2 <= dy) { y0++; y1--; err += dy += a; }                 /* y step */ 
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; }  /* x step */
    
    } while (x0 <= x1);

/* too early stop of flat ellipses a=1 */
    while (y0-y1 < b) {
        grPlot0 ( NULL, 0, x0-1,    y0, color, 0);  //-> finish tip of ellipse
        grPlot0 ( NULL, 0, x1+1,  y0++, color, 0);
        grPlot0 ( NULL, 0, x0-1,    y1, color, 0);
        grPlot0 ( NULL, 0, x1+1,  y1--, color, 0);
    };
}

// plotEllipseRectZ: 
void 
plotEllipseRectZ (
    int x0, int y0, 
    int x1, int y1, 
    unsigned long color,
    int z )
{
    int a = abs(x1-x0), b = abs(y1-y0), b1 = b&1;  /* values of diameter */
    long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a;      /* error increment */
    long err = dx+dy+b1*a*a, e2;                   /* error of 1.step */

    if (x0 > x1) { x0 = x1; x1 += a; }  /* if called with swapped points */
    if (y0 > y1) y0 = y1;               /* .. exchange them */
    y0 += (b+1)/2; y1 = y0-b1;          /* starting pixel */
    a *= 8*a; b1 = 8*b*b;

    do {
       grPlot0 ( NULL, z, x1, y0, color, 0);  //   I. Quadrant
       grPlot0 ( NULL, z, x0, y0, color, 0);  //  II. Quadrant
       grPlot0 ( NULL, z, x0, y1, color, 0);  // III. Quadrant
       grPlot0 ( NULL, z, x1, y1, color, 0);  //  IV. Quadrant

       e2 = (2*err);
       if (e2 <= dy) { y0++; y1--; err += dy += a; }                 /* y step */ 
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; }  /* x step */

    } while (x0 <= x1);

/* too early stop of flat ellipses a=1 */
    while (y0-y1 < b) {
        grPlot0 ( NULL, z, x0-1,    y0, color, 0);  // -> finish tip of ellipse
        grPlot0 ( NULL, z, x1+1,  y0++, color, 0);
        grPlot0 ( NULL, z, x0-1,    y1, color, 0);
        grPlot0 ( NULL, z, x1+1,  y1--, color, 0);
    };
}


// plotCharBackbufferDrawcharTransparent:
// Desenha um caractere sem alterar o pano de fundo.
// >> no backbuffer.
// #bugbug
// Nessa função estamos usando globais.
// Talvez devamos pegá-las antes e não 
// referenciá-las diretamente.

void 
plotCharBackbufferDrawcharTransparent ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    unsigned long c )
{
    //loop
    register int y2=0;
    register int x2=0;
    char *work_char;
    unsigned char bit_mask = 0x80;
    //int CharWidth;
    //int CharHeight;

/*
 * Get the font pointer.
 * @todo:
 *     usar variavel g8x8fontAddress.
 *     + Criar e usar uma estrutura para fonte.
 *     + Usar o ponteiro para a fonte atual que foi carregada.
 *     + Criar um switch para o tamanho da fonte.
 *     isso deveria estar na inicialização do módulo char.
 *     ...
 */

    if ( FontInitialization.address == 0 || 
         FontInitialization.width <= 0 || 
         FontInitialization.height <= 0 )
    {
        printf("gws_drawchar_transparent: Initialization?\n");
        while (1){
        };
    }

// O caractere sendo trabalhado.
    work_char = 
        (void *) FontInitialization.address + (c * FontInitialization.height);

//
// Draw
//
    for ( y2=0; y2 < FontInitialization.height; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < FontInitialization.width; x2++ )
        {
            // Put pixel. 
            // IN: z,x,y,color,rop
            if ( ( *work_char & bit_mask ) )
            {
                grPlot0 ( 
                    NULL, 
                    0, x + x2, y, 
                    color, 0 ); 
            }

            // Rotate bitmask.
            bit_mask = (bit_mask >> 1);  
        };

        // Próxima linha da 8 linhas do caractere.
        y++; 
        work_char++; 
    };
}

// plotCharBackbufferDrawcharTransparentZ:
// Desenha um caractere sem alterar o pano de fundo.
// >> no backbuffer.
// #bugbug
// Nessa função estamos usando globais.
// Talvez devamos pegá-las antes e não 
// referenciá-las diretamente.
// root surface?

void 
plotCharBackbufferDrawcharTransparentZ ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    unsigned long c,
    int z )
{
    char *work_char;
    unsigned char bit_mask = 0x80;
    int x2=0;
    int y2=0;
    //int CharWidth;
    //int CharHeight;

/*
 * Get the font pointer.
 * @todo:
 *     usar variavel g8x8fontAddress.
 *     + Criar e usar uma estrutura para fonte.
 *     + Usar o ponteiro para a fonte atual que foi carregada.
 *     + Criar um switch para o tamanho da fonte.
 *     isso deveria estar na inicialização do módulo char.
 *     ...
 */

    if ( FontInitialization.address == 0 || 
         FontInitialization.width <= 0 || 
         FontInitialization.height <= 0 )
    {
        printf ("gws_drawchar_transparent : Initialization fail\n");
        while(1){}
    }

// O caractere sendo trabalhado.
    work_char = 
        (void *) FontInitialization.address + (c * FontInitialization.height);

// Draw
// #todo
// We need a flat to invert or not.
// invert
// ok. it works
    for ( y2=0; y2 < FontInitialization.height; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < FontInitialization.width; x2++ )
        {
            // Put pixel. 
            // começa do fim
            // IN: z,x,y,color,rop
            if ( ( *work_char & bit_mask ) )
            {
                grPlot0 ( 
                    NULL, 
                    z, x + x2, (y + FontInitialization.width), 
                    color, 0 ); 
            }

            // Rotate bitmask.
            bit_mask = (bit_mask >> 1);  
        };

        // Próxima linha da 8 linhas do caractere.
        //y++; 
        y--;
        work_char++; 
    };
}

// plotQuadBezierSeg:
// Bézier curve
// This program example plots a quadratic Bézier curve 
// limited to gradients without sign change.
// IN: ?? #todo: explain the input values.
// IN: ponto inicial, ?? , ponto final.
void 
plotQuadBezierSeg ( 
    int x0, int y0, int z0, 
    int x1, int y1, int z1,
    int x2, int y2, int z2, 
    unsigned int color )
{
    int  sx = x2-x1, sy = y2-y1;
    long xx = x0-x1, yy = y0-y1, xy;          /* relative values for checks */
    //double dx, dy, err, cur = xx*sy-yy*sx;  /* curvature */

    //loop
    register long dx=0; 
    register long dy=0;

    long err=0; 
    long cur = xx*sy - yy*sx;

    /* sign of gradient must not change */
    //assert(xx*sx <= 0 && yy*sy <= 0);

    if ( xx*sx <= 0 && 
         yy*sy <= 0 )
    {
        printf("plotQuadBezierSeg: [TODO] assert\n");
        exit(1);
    }

     /* begin with longer part */
    if (sx*(long)sx+sy*(long)sy > xx*xx+yy*yy)
    {  
        //swap P0 P2
        x2 = x0; 
        x0 = sx+x1; 
        y2 = y0; 
        y0 = sy+y1; 
        cur = -cur;  
    }  
    
    if (cur != 0) {                           /* no straight line */

    xx += sx; xx *= sx = x0 < x2 ? 1 : -1;    /* x step direction */
    yy += sy; yy *= sy = y0 < y2 ? 1 : -1;    /* y step direction */
    xy = 2*xx*yy; xx *= xx; yy *= yy;         /* differences 2nd degree */
    if (cur*sx*sy < 0) {                      /* negated curvature? */
      xx = -xx; yy = -yy; xy = -xy; cur = -cur;
    }
    
    //dx = 4.0*sy*cur*(x1-x0)+xx-xy;          /* differences 1st degree */
    //dy = 4.0*sx*cur*(y0-y1)+yy-xy;

    dx = 4*sy*cur*(x1-x0)+xx-xy;              /* differences 1st degree */
    dy = 4*sx*cur*(y0-y1)+yy-xy;
    
    xx += xx; yy += yy; err = dx+dy+xy;       /* error 1st step */    
    do {                              
      
      /* plot curve */
      //setPixel(x0,y0); 
      grPlot0( NULL, x0, y0, z0, color, 0 );

      if (x0 == x2 && y0 == y2) return;  /* last pixel -> curve finished */
      y1 = 2*err < dx;                   /* save value for test of y step */
      if (2*err > dy) { x0 += sx; dx -= xy; err += dy += yy; } /* x step */
      if (    y1    ) { y0 += sy; dy -= xy; err += dx += xx; } /* y step */
    
    } while (dy < dx );           /* gradient negates -> algorithm fails */

    }

/* plot remaining part to end */
    //plotLine(x0,y0, x2,y2);
    plotLine3d (
        x0,y0,z0, 
        x2,y2,z2, 
        color ); 
}

/*
 #todo
 substitui uma cor por outra dentro de uma janela.
void 
xxxFindReplaceColor( 
    struct gws_window_d *window, 
    unsigned long color1, 
    unsigned long color2);
void 
xxxFindReplaceColor( 
    struct gws_window_d *window, 
    unsigned long color1, 
    unsigned long color2)
{
}
*/

// servicepixelBackBufferPutpixel:
// ...
int servicepixelBackBufferPutpixel(void)
{
    unsigned long *message_address = (unsigned long *) &__buffer[0];
    unsigned long x,y,color,rop;

    x = message_address[4];
    y = message_address[5];
    color = (unsigned int) message_address[6];
    //rop = message_address[?];

    if (x<0){ goto fail; }
    if (y<0){ goto fail; }

// #bugbug:
// color is 'unsigned int' i guess.
    libdisp_backbuffer_putpixel(color,x,y,0);

    return 0;
fail:
    return (int) -1;
}

//
// End
//

