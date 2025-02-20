/*
 * File: grprim.c 
 *     Primitives.
 * History:
 *     2020 - Created by Fred Nora.
 */

#include "gram3d.h"


// Swap two bytes
#define ____SWAP(x,y)  \
    do { (x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y); } while(0)

// See:
// https://wiki.osdev.org/3D_Renderer_Basics
// http://members.chello.at/easyfilter/bresenham.html
// http://math.hws.edu/graphicsbook/c2/s3.html
// http://math.hws.edu/graphicsbook/index.html
// ...

// ================================
// Viewport 2d:
// A origem normalmente fica em top/left.
// É o 'raster', para fazer rasterização.
// Viewport is a rectangular display area 
// on the application window, which is measured 
// in screen's coordinates 
// (in pixels, with origin at the top-left corner). 
// A viewport defines the size and shape of the display 
// area to map the projected scene captured by 
// the camera onto the application window. 
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
// See: 
// https://www3.ntu.edu.sg/home/ehchua/programming/opengl/CG_BasicsTheory.html
// =============================
// #todo
// Create some configuration globals here
// int gUseSomething = TRUE;
// int gUseRaytracing = TRUE;
// ...
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
static unsigned long HotSpotX=0;
static unsigned long HotSpotY=0;


// =============================
/*
 * grInit:
 *     Initialize the 3d support.
 *     Create the hotspot to normalize the screen.
 *     The device context was created in the beginning of
 * of the initialization.
 *     To handle the device context we have the structures:
 * 'display' and 'screen'. The clients will use the data in these structures.
 */

int grInit (void)
{
    unsigned long deviceWidth = gws_get_device_width();
    unsigned long deviceHeight = gws_get_device_height();

    gwssrv_debug_print ("grInit:\n");

    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        gwssrv_debug_print ("grInit: w h\n");
        printf ("grInit: w h\n");
        exit(1);
    }

// center of the screen.
// #todo: 
// We need the option to put the hotspot 
// at the center of the window.
    
    HotSpotX = (deviceWidth>>1);
    HotSpotY = (deviceHeight>>1);

// == Projection =========
// Initialize the current projection.
// Change the view for the current projection.
// #test
// Initialize projection matrix. 
// Using float.
// IN: 
// znear, 
// zfar, 
// fov, 
// width, 
// height. 
// Scale factor.

// #todo
// #important
// This is real value used for the scale factor.
// We need to get a better value than this default.
// #todo: use the formula.
    // #ps
    // We're not using this value.
    // We're getting another one from CurrentProjectionF.scale_factor.
    //float scale_factor = (float) 1/( tan(angleA/2) );  // degree
    //float scale_factor = (float) 1/( tan(VerticalFOV/2 * PI/180) );  //rad

// Using float.
    grInitializeProjection( 
        (float) 0.01f, 
        (float) 1000.0f, 
        (float) 90.0f,
        (unsigned long) (deviceWidth & 0xFFFFFFFF),
        (unsigned long) (deviceHeight & 0xFFFFFFFF),
        (float) 0.5f );  //#important: Scale factor.

// Using int.
    projection_initialize();
// Changing the view for the current projection.
    gr_depth_range(CurrentProjection,0,40);
// World
    world_initialize();

// == Camera ==========
// Initialize the current camera.
// Change some attributes for the current camera.
// The projection is a field in the camera's structure.

    //gwssrv_debug_print ("grInit: camera\n");
    camera_initialize();
    cameraF_initialize();
    
    //camera ( 
    //    -40, -40, 0,     // position vector
    //    -40,  40, 0,     // upview vector
    //     10,  10, 10 );  // lookat vector

    camera ( 
        0, 0, 0,     // position vector
        0, 0, 0,     // upview vector
        0, 0, 0 );   // lookat vector

    // ...

    return 0;
}

int 
gr_set_ray_info(
    struct gr_ray_d *r,
    struct gr_vec3D_d *origin,
    struct gr_vec3D_d *direction )
{
    if ( (void*) r == NULL ){
        return -1;
    }
    if (r->magic != 1234){
        return -1;
    }
    if ( (void*) origin == NULL )
        return -1;
    if ( (void*) direction == NULL )
        return -1;

// Origin
    r->p[0].x = (int) origin->x;
    r->p[0].y = (int) origin->y;
    r->p[0].z = (int) origin->z;
// Direction
    r->p[1].x = (int) direction->x;
    r->p[1].y = (int) direction->y;
    r->p[1].z = (int) direction->z;
// ok
    return 0;
}

int gr_reset_ray_info(struct gr_ray_d *r)
{
    if ( (void*) r == NULL ){
        return -1;
    }
    if (r->magic != 1234){
        return -1;
    }

    r->type = 0;
// origin
    r->p[0].x = 0;  r->p[0].y = 0;  r->p[0].z = 0;
// direction
    r->p[1].x = 0;  r->p[1].y = 0;  r->p[1].z = 0;
    r->distance = 0;
    r->used = TRUE;
    r->magic = 1234;
// ok
    return 0;
}

struct gr_ray_d *ray_object(void)
{
    struct gr_ray_d *tmp;

    tmp = (struct gr_ray_d *) malloc( sizeof(struct gr_ray_d) );
    if ( (void*) tmp == NULL ){
        return NULL;
    }
    tmp->used = TRUE;
    tmp->magic = 1234;

    //if(reset)
    //gr_reset_ray_info(tmp);

    return (struct gr_ray_d *) tmp;
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
// Isso determina os limites onde podemos
// pintar com 2d. Esse é nosso 'raster'.
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
    if(topleft_y<0)
        topleft_y=0;

    if (width<0)
        width=0;
    if(height<0)
        width=0;

    // ...
    
    // #todo
    // call that routine.
    //projection_initialize(...)
}


void gr_dc_extents_init(struct dc_d *dc)
{
    if ( (void*) dc == NULL ){
        return;
    }
    if (dc->magic != 1234){
        return;
    }

    //dc->flags |= DCF_RECORD_EXTENTS;
    dc->min_x = ULONG_MIN;
    dc->max_x = ULONG_MAX;
    dc->min_y = ULONG_MIN;
    dc->max_y = ULONG_MAX;
}

int gr_dc_refresh_screen(struct dc_d *dc)
{
    if ( (void*) dc == NULL ){
        return -1;
    }
    if (dc->magic != 1234){
        return -1;
    }
    if (dc->initialized != TRUE){
        return -1;
    }

// Refresh
    gws_refresh_rectangle(
        dc->absolute_x, dc->absolute_y, dc->width, dc->height );

    return 0;
}

int gr_dc_fill( struct dc_d *dc, unsigned int bg_color )
{
    if ( (void*) dc == NULL ){
        return -1;
    }
    if (dc->magic != 1234){
        return -1;
    }
    //if (dc->initialized != TRUE){
    //    return -1;
    //}

    dc->bg_color = (unsigned int) bg_color;

    rectBackbufferDrawRectangle ( 
        dc->absolute_x, 
        dc->absolute_y, 
        dc->width,
        dc->height, 
        dc->bg_color, 
        TRUE,           // fill
        dc->rop );

    return 0;
}


// plot point
// 2d
// not tested!
int 
gr_dc_plot0(
    struct dc_d *dc,
    int x,
    int y )
{
// Print a pixel with the color in the given dc.
// No clipping window.
    if ( (void*) dc == NULL ){
        return -1;
    }
    if (dc->magic != 1234){
        return -1;
    }
    if (dc->initialized != TRUE){
        return -1;
    }

    if (x<0)
        return -1;
    if (y<0)
        return -1;

    unsigned int color = (unsigned int) dc->color;
    unsigned long finalx = (unsigned long) (x & 0xFFFFFFFF);
    unsigned long finaly = (unsigned long) (y & 0xFFFFFFFF);
    unsigned long rop = (unsigned long) dc->rop;

    finalx += dc->absolute_x;
    finaly += dc->absolute_y;

    libdisp_backbuffer_putpixel(
        (unsigned int) color, 
        finalx, 
        finaly, 
        (unsigned long) rop );

    return 0;
}


// Plot a pixel using the top/left coordinates. (0,0).
// Clipping in the screen.
int 
grPlot2D ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop )
{
// #todo: 
// Return the number of changed pixels. '1' or '0'.

// Clipping
// 0 pixels changed.
    if (x<0)
        return 0;
    if (y<0)
        return 0;
    return (int) libdisp_backbuffer_putpixel( (unsigned int) color, x, y, rop ); 
}

/*
 * grPlot0:
 *      plot pixel.
 *      Viewport Transformation.
 *      Low level routine.
 *      Origin at center of the device screen. 
 *      #todo: Plot into a 'normalized' 2d rater screen. 
 *      #new: Plotting into a clipping window.
 * low level plot.
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
// clipping window near: Near window.
// clipping window far: Far window.
// x,y,z
// color

int 
grPlot0 ( 
    struct gws_window_d *clipping_window,
    int z, int x, int y, 
    unsigned int color,
    unsigned long rop )
{
// Viewport Transform: (mapping)
// depth buffer?
// In 3D graphics, a viewport is 3-dimensional 
// to support z-ordering, which is needed for 
// situations such as ordering of overlapping windows.
// #todo
// We can use a 'clipping window' and draw only inside this window.
// If the clipping window is NULL, so we need to use the root window.
// #todo
// This is a work in progress

    // Final 2D screen coordenates.
    int X=0;
    int Y=0;

    unsigned long _rop = rop;

    // Draw flag.
    int Draw = TRUE;

    int UsingDepthBuffer = FALSE;
    int UsingAlphaBlending = FALSE;

// Clipping window support.
    struct gws_window_d *w;
// False. But it turns into TRUE 
// if we have a valid window pointer.
    int UseClipping = FALSE;

    int hotspotx=0;
    int hotspoty=0;

    int UseLeftHand = gUseLeftHand;
    //UseLeftHand = TRUE;   //LEFT HAND
    //UseLeftHand = FALSE;  //RIGHT HAND

// Device screen structure
// See: screen.h

    // #debug
    if ( (void *) DeviceScreen == NULL ){
        Draw = FALSE;
        printf("grPlot0: DeviceScreen\n");
        exit(1);
    }

// #todo
// precisamos checar algumas globais, como HotSpotX e HotSpotY.
// Usaremos a janela chamada screen se nenhuma outra foi indicada.
//gui->screen

//-------------------------------------
// tmp: zNear zFar
// Clipping z
// The projection is an element of the camera structure.
// 'CurrentCamera->CurrentProjection'
// Clipping into the projection field.

     int zNear = 0; 
     int zFar  = 80;

     if ( (void*) CurrentProjection != NULL ){
          zNear = (int) CurrentProjection->zNear;
          zFar  = (int) CurrentProjection->zFar;
     }
     if (z < zNear)
         return 0;
     if (z >= zFar)
         return 0;

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


// The clipping window.
// #todo
// If the clipping window is invalid, 
// so we're gonna use the root window.
// #todo:
// Maybe we need to use the device context structure,
// or something like that.

    if ( (void*) clipping_window != NULL )
    {
        if ( clipping_window->used == TRUE && 
             clipping_window->magic == 1234 )
        {
            UseClipping = TRUE;
            w = (struct gws_window_d *) clipping_window;
        }
    }

// Transformation:
// Estamos tranformando de um 'object space' com origem 
// no centro da tela para um viewport que ocupa a tela toda, 
// com origem no canto superior esquerdo.
// IN: 
// return x, return y, x, y, z, fix_orientation,

// #todo:
// Podemos usar o hotspot do dc pra fazermos a transformações.

// Como não temos uma janela,
// então fazemos a transformação usando o hotspot da tela.
    if (UseClipping==FALSE){
       hotspotx = HotSpotX;
       hotspoty = HotSpotY;
    }

// Se temos uma clipping window válida,
// então usamos o hot spot dela.
// #todo: 
// Precisamos de variaveis de hotspot dentro da estrutura de janela ... 
// o hotspot não fica no centro da janela
// e sim no centro da área de cliente da janela.
// o dc atual deve ser a área de cliente da janela.
    if (UseClipping==TRUE){
        hotspotx = w->absolute_x + (w->width /2);
        hotspoty = w->absolute_y  + (w->height /2);
    }

// transform
    libgr_transform_from_viewspace_to_screespace( 
        (int *) &X, (int *) &Y, 
        x, y, z,
        UseLeftHand,
        hotspotx, hotspoty ); 

// Draw and clipping.
// #todo
// Talvez possamos retornar '0'
// se a flag indicar que não precisava desenhar.
    if (Draw != TRUE){
        return -1;
    }

// #todo: 
// We need to check the window limits
// if we are drawing inside a given window.
// Checking the device screen limits.
// #bugbug
// Já fizemos isso logo acima?

    if (X<0){ return -1; }
    if (Y<0){ return -1; }

// fake depth buffer:

/*
    if (db = dc->depth_buf) 
    {
        // Adiciona o deslocamento do pixel.
        // assim obtemos a posição final.
        db += d;
        // Se a próxima profundiade for menor que
        // a que ja tínhamos salvo, então substituimos.
        // Caso contrário, deixe a profundidade anterior.
        if (0 <= dc->db_z <= *db ){
            // Atualizamos a profundidade salva 
            // numa dada posição do z-buffer.
            *db = dc->db_z;
        }else{
            // não precisamos pintar.
            return TRUE;
        }
    }
*/


    if ( 0 <= X < DeviceScreen->width && 
         0 <= Y < DeviceScreen->height )
    {
        if ( UsingAlphaBlending == TRUE ){
            // #todo
            // Get the color and modify it.
            //color = get??()
        }

        // Se NÃO temos uma janela válida.
        // device screen
        // 2D, No clipping or transformation.
        // Plot pixel into the raster.
        // The origin is top/left of the viewport. (0,0).
        // IN: color, x, y, rop
        // #todo
        // O rop pode vir do dc.
        if (UseClipping == FALSE){
            return (int) libdisp_backbuffer_putpixel( 
                             (unsigned int) color, X, Y, _rop );
        }

        // Se temos uma janela válida.
        // Plot pixel into the raster.
        // The origin is top/left of the viewport. (0,0).
        // IN: color, x, y, rop
        // #todo
        // O rop pode vir do dc.
        // Esta dentro da janela.
        if (UseClipping == TRUE)
        {
            if ( X >= w->absolute_x && X <= w->right &&
                 Y >= w->absolute_y  && Y <= w->bottom )
            {
                return (int) libdisp_backbuffer_putpixel(
                                (unsigned int) color, X, Y, _rop ); 
            }
        }

        // #todo
        // This is a work in progress
            
        //if( UsingDepthBuffer == TRUE )
        //{
        //    depth_buffer[ offset ] = Z;
        //}
            
        return 0; //#todo '-1'
    }

// Fail 
    return (-1);
}

// Plot with graphics effects. use flags.
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

// No graphics effects
    if (flags == 0)
    {
        xValue = x;
        yValue = y;
        zValue = z;
        colorValue = (unsigned int) color;
        goto PlotPixel;
    }

// #todo #test
// plot pixel using the caes in fx.

    if ( flags & 0x00000001 ){
        fBlack = TRUE;
        colorValue = (unsigned int) 0x00000000;
    }

    if ( flags & 0x00000020 ){
        fNoZBuffer = TRUE;
        zValue = 0;
    }

    // ...

PlotPixel:

    return (int) grPlot0 ( 
                     clipping_window, 
                     zValue, xValue, yValue, 
                     (unsigned int) colorValue, rop );
}

// #todo
// See: gwsProcedure(), service 2040 in main.c
int serviceGrPlot0(void)
{
    unsigned long *message_address = (unsigned long *) &__buffer[0];
    unsigned long x=0;
    unsigned long y=0;
    unsigned long z=0;
    unsigned int color=COLOR_PINK;

    //gwssrv_debug_print("serviceGrPlot0: [TODO] \n");

// Arguments:
// Sempre começa do 10 para rotinas 3D.    
    x      = message_address[10];
    y      = message_address[11];
    z      = message_address[12]; 
    color  = message_address[13];

// #todo:
// pegar os argumentos no buffer e chamar a rotina de plotagem de pixel.
// Acho que esse serviço nao retorna uma mensagem ao cliente.
    
    return (int) grPlot0( 
                     NULL, 
                     (int) z, (int) x, (int) y, 
                     (unsigned int) color, 0 );
}


/*
void plotLine(int x0, int y0, int x1, int y1);
void plotLine(int x0, int y0, int x1, int y1)
{
    int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;                       // error value e_xy 

    for (;;) {                                 // loop 
        setPixel(x0,y0);
        e2 = 2*err;
        if (e2 >= dy) {                        // e_xy+e_x > 0
            if (x0 == x1) break;
            err += dy; x0 += sx;
        }
        if (e2 <= dx) {                        // e_xy+e_y < 0 
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }
}
*/


/*
// #todo
// Not tested yet.
// linear interpolation
float lerpF( float x1, float x2, float factor );
float lerpF( float x1, float x2, float factor )
{
    return (float) ( x1 + ((float)(x2-x1) * (float) factor));
}
*/

// plotLine3d:
// Bresenham in 3D
// The algorithm could be extended to three (or more) dimensions.
int 
plotLine3d (
    struct gws_window_d *window,
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color )
{
    int npixels=0;  // Number of pixels changed.
    
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int dz = abs(z1-z0), sz = z0<z1 ? 1 : -1;

/* maximum difference */
    int dm = grMAX3(dx,dy,dz);
    register int i=dm;

/* error offset */
    // x1 = y1 = z1 = dm/2; 

    x1 = (dm >> 1);
    y1 = x1;
    z1 = x1;

    for (;;)
    {
        grPlot0( window, z0, x0, y0, color, 0 );
        npixels++;
     
        if (i-- == 0) { break; }
        
        x1 -= dx; 
        if (x1 < 0) 
        { x1 += dm; x0 += sx; }
        
        y1 -= dy; 
        if (y1 < 0) 
        { y1 += dm; y0 += sy; }
        
        z1 -= dz; 
        if (z1 < 0) 
        { z1 += dm; z0 += sz; }
    };
    
    return (int) npixels;
}

// plotLine3dEx:
// Bresenham in 3D
// The algorithm could be extended to three (or more) dimensions.
void 
plotLine3dEx (
    struct gws_window_d *window,
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    unsigned int color )
{

    if ( (void*) window == NULL )
        return;
    if (window->magic!=1234)
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

    for (;;) {
        grPlot0 ( window, z0, x0, y0, color, 0 );
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
// interpolation?
// #todo: Change the color type to 'unsigned int'.
void 
plotLine3d2 (
    int x0, int y0, int z0, unsigned long color1,
    int x1, int y1, int z1, unsigned long color2, 
    int flag )
{
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
    int dz = abs(z1-z0), sz = z0<z1 ? 1 : -1; 

/* maximum difference */
    int dm = grMAX3(dx,dy,dz);
    register int i = dm;

/* error offset */
    // x1 = y1 = z1 = dm/2; 

    x1 = (dm >> 1);
    y1 = x1;
    z1 = x1;

// nothing for now;
// interpolation flag.

    flag=0;

// Loop 

    for (;;)
    {
        grPlot0 ( NULL, z0, x0, y0, color1, 0 );
      
        if (i-- == 0){ 
            break;
        }
        x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; } 
        y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; } 
        z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; } 
    };
}


// track a given vector.
// OUT: the number of vectors in this line.
int
plotLine3dLT2 (
    struct gws_window_d *window,
    int x0, int y0, int z0, 
    int x1, int y1, int z1,
    int *x2, int *y2, int *z2, 
    int n,   // track this vector.
    unsigned int color,
    int draw )
{
    int ResultNumberOfVectors=0;

    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
    int dz = abs(z1-z0), sz = z0<z1 ? 1 : -1; 

   //#bugbug: This macro is wrong?!
   //int dm = grMAX3 (dx,dy,dz), i = dm; /* maximum difference */
   
   int dm = grMAX3(dx,dy,dz);
   register int i = dm;
   ResultNumberOfVectors = (int) i;

    // x1 = y1 = z1 = dm/2; /* error offset */
 
    x1 = (dm >> 1);
    y1 = x1;
    z1 = x1;

    for (;;) {

        if(draw){
            grPlot0 ( window, z0, x0, y0, color, 0 );
        }
        
        //
        if (i == n)
        {
            if ( (void*) x2 != NULL ){ *x2 = x0; }
            if ( (void*) y2 != NULL ){ *y2 = y0; }
            if ( (void*) z2 != NULL ){ *z2 = z0; }
            // Do not break.
        }

        // End of line.
        if (i == 0){
            break;
        }

        x1 -= dx;
        if (x1 < 0) { x1 += dm; x0 += sx; }
        y1 -= dy; 
        if (y1 < 0) { y1 += dm; y0 += sy; }
        z1 -= dz;
        if (z1 < 0) { z1 += dm; z0 += sz; }

        i--;
    };

    return (int) ResultNumberOfVectors;
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
        NULL,
        left,top,0, 
        right,top,0, 
        color );

// baixo
    plotLine3d ( 
        NULL,
        left,bottom,0, 
        right,bottom,0, 
        color );

// esquerda
    plotLine3d ( 
        NULL,
        left,top,0, 
        left,bottom,0, 
        color );

// direita
    plotLine3d ( 
        NULL,
        right,top,0, 
        right,bottom,0, 
        color );
}


void drawRectangleF(struct gr_rectangleF3D_d *rectangle)
{
    // 'int' values
    struct gr_rectangle_d r;

    if ((void*)rectangle==NULL){
        return;
    }

// Parameters

    float znear = (float) 0.01f;  //default
    float zfar  = (float) 10.0f;  //default
    unsigned long window_width  = 200;
    unsigned long window_height = 200;
    float ar = (float) 1.0f;      //default
    float scale_factor = (float) 0.5f; // % da tela.

    if (CurrentProjectionF.initialized == TRUE)
    {
        znear = (float) CurrentProjectionF.znear;
        zfar  = (float) CurrentProjectionF.zfar;
        window_width  = (unsigned long) CurrentProjectionF.width;
        window_height = (unsigned long) CurrentProjectionF.height;
        ar = 
            (float)((float) window_height / (float) window_width );
        //#todo: hotspot
        scale_factor = (float) CurrentProjectionF.scale_factor;
    }

// --------

// Clipping in z

    if (rectangle->p[0].z < znear){ return; }
    if (rectangle->p[1].z < znear){ return; }
    if (rectangle->p[2].z < znear){ return; }
    if (rectangle->p[3].z < znear){ return; }

    if (rectangle->p[0].z > zfar){ return; }
    if (rectangle->p[1].z > zfar){ return; }
    if (rectangle->p[2].z > zfar){ return; }
    if (rectangle->p[3].z > zfar){ return; }

// #test
// Ficando menor conforma z aumenta.

    if (rectangle->p[0].z != 0.0f)
    {
        rectangle->p[0].x = (float) (rectangle->p[0].x/rectangle->p[0].z);  
        rectangle->p[0].y = (float) (rectangle->p[0].y/rectangle->p[0].z);
    }
    if (rectangle->p[1].z != 0.0f)
    {
        rectangle->p[1].x = (float) (rectangle->p[1].x/rectangle->p[1].z);  
        rectangle->p[1].y = (float) (rectangle->p[1].y/rectangle->p[1].z);
    }
    if (rectangle->p[2].z != 0.0f)
    {
        rectangle->p[2].x = (float) (rectangle->p[2].x/rectangle->p[2].z);  
        rectangle->p[2].y = (float) (rectangle->p[2].y/rectangle->p[2].z);
    }
    if (rectangle->p[3].z != 0.0f)
    {
        rectangle->p[3].x = (float) (rectangle->p[3].x/rectangle->p[3].z);  
        rectangle->p[3].y = (float) (rectangle->p[3].y/rectangle->p[3].z);
    }
//---------

// scale
// Ajustando à tela.

    long x0 = (long) (rectangle->p[0].x *ar * scale_factor * (float) window_width);
    long y0 = (long) (rectangle->p[0].y     * scale_factor * (float) window_height);
    long x1 = (long) (rectangle->p[1].x *ar * scale_factor * (float) window_width);
    long y1 = (long) (rectangle->p[1].y     * scale_factor * (float) window_height);
    long x2 = (long) (rectangle->p[2].x *ar * scale_factor * (float) window_width);
    long y2 = (long) (rectangle->p[2].y     * scale_factor * (float) window_height);
    long x3 = (long) (rectangle->p[3].x *ar * scale_factor * (float) window_width);
    long y3 = (long) (rectangle->p[3].y     * scale_factor * (float) window_height);

    r.p[0].x = (int) ( x0 & 0xFFFFFFFF);
    r.p[0].y = (int) ( y0 & 0xFFFFFFFF);
    r.p[0].z = (int) 0;
    r.p[0].color = rectangle->p[0].color; //COLOR_WHITE;
    
    r.p[1].x = (int) ( x1 & 0xFFFFFFFF);
    r.p[1].y = (int) ( y1 & 0xFFFFFFFF);
    r.p[1].z = (int) 0;
    r.p[1].color = rectangle->p[1].color; // COLOR_WHITE;
    
    r.p[2].x = (int) ( x2 & 0xFFFFFFFF);
    r.p[2].y = (int) ( y2 & 0xFFFFFFFF);
    r.p[2].z = (int) 0;
    r.p[2].color = rectangle->p[2].color; // COLOR_WHITE;

    r.p[3].x = (int) ( x3 & 0xFFFFFFFF);
    r.p[3].y = (int) ( y3 & 0xFFFFFFFF);
    r.p[3].z = (int) 0;
    r.p[3].color = rectangle->p[3].color; // COLOR_WHITE;

//---------------------------------
    __rectangleZZ( (struct gr_rectangle_d *) &r );
}

// worker
// 4 3d lines, not filled.
void __rectangleZZ(struct gr_rectangle_d *rect)
{
    if ( (void*) rect == NULL ){
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

// Top
    //plotLine3d2 ( left, top,  z, right, top, z, color, flag );
    plotLine3d2 ( 
        rect->p[0].x, rect->p[0].y, rect->p[0].z, rect->p[0].color,
        rect->p[1].x, rect->p[1].y, rect->p[1].z, rect->p[1].color, 
        0 );

// Bottom
     //plotLine3d2 ( left, bottom, z, right,bottom, z, color, flag );
     plotLine3d2 ( 
         rect->p[3].x, rect->p[3].y, rect->p[3].z, rect->p[3].color,
         rect->p[2].x, rect->p[2].y, rect->p[2].z, rect->p[2].color, 
         0 );

// Left
     //plotLine3d2 ( left, top, z, left, bottom, z, color, flag );
     plotLine3d2 ( 
         rect->p[0].x, rect->p[0].y, rect->p[0].z, rect->p[0].color,
         rect->p[3].x, rect->p[3].y, rect->p[3].z, rect->p[3].color, 
         0 ); 

// Right
     //plotLine3d2 ( right,  top, z, right, bottom, z, color, flag );
     plotLine3d2 ( 
         rect->p[1].x, rect->p[1].y, rect->p[1].z, rect->p[1].color,
         rect->p[2].x, rect->p[2].y, rect->p[2].z, rect->p[2].color, 
         0 );
}

int grRectangle(struct gr_rectangle_d *rect)
{
    if ( (void*) rect == NULL )
        return -1;

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
}


// Rectangle rasterization using 3d lines.
// It's not the standard quad rasterization routine
// using the viewport 2d coordenates.
// It uses the model 3d coordenates.
// It applies only on few cases.
// IN:
// upper-left corner, 
// lower-right corner,
// color
// #bugbug:
// Essa rotina é boa somente para alguns casos.
// depende da inclinação.

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

//top:     y da linha superior do retangulo
//bottom:  y da linha inferior do retangulo

// Pintamos uma linha por vez, horizontalmente.
// IN: 
// x0,y0,z0, x1,y1,z1, color.
        
    for ( Line = top;
             Line >= bottom; 
             Line-- )
    {
        plotLine3d ( 
            NULL,
            left,  Line, z0,   // vertex 1 (left vertex)
            right, Line, z1,   // vertex 2 (right vertex) 
            color );
    };
}

// Triangle
// #todo: return pixel counter.
int 
grTriangle3(
    struct gws_window_d *window, 
    struct gr_triangle_d *triangle )
{
    int npixels=0;  // Number of pixels changed.

    if ( (void*) triangle == NULL ){
        return (int) npixels;
    }

// Draw:
// Circular, 
// sentido horario, 
// começando pelo ponto de cima.

    npixels += plotLine3d (
        window,
        triangle->p[0].x, triangle->p[0].y, triangle->p[0].z, 
        triangle->p[1].x, triangle->p[1].y, triangle->p[1].z, 
        triangle->p[1].color );
    npixels += plotLine3d (
        window,
        triangle->p[1].x, triangle->p[1].y, triangle->p[1].z, 
        triangle->p[2].x, triangle->p[2].y, triangle->p[2].z, 
        triangle->p[2].color );
    npixels += plotLine3d (
        window,
        triangle->p[2].x, triangle->p[2].y, triangle->p[2].z, 
        triangle->p[0].x, triangle->p[0].y, triangle->p[0].z, 
        triangle->p[0].color );

    return (int) npixels;
}

int grTriangle(struct gr_triangle_d *triangle)
{
    int npixels=0;  // Number of pixels changed.

    if ( (void*) triangle == NULL ){
        return (int) npixels;
    }

// #todo
// something

    npixels += grTriangle3(NULL,triangle);

    return (int) npixels;
}
//---------------------

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
        npixels += grBackbufferDrawHorizontalLine( minx, y, maxx, c, rop );

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
        // see: line.c
        npixels += grBackbufferDrawHorizontalLine( minx, y, maxx, c, rop );

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

//#todo: return pixel counter.
int 
fillTriangle(
    struct gws_window_d *window, 
    struct gr_triangle_d *triangle,
    int hotspotx, int hotspoty,
    unsigned long rop )
{

// Number of changed pixels.
    int npixels=0;
// Transformed values.
// Valid for 2D with 0:0 in the top/left corner.
    int X0=0; int Y0=0;
    int X1=0; int Y1=0;
    int X2=0; int Y2=0;

    //if( (void*) window == NULL )
        //return 0;

    if ( (void*) triangle == NULL ){
        return (int) npixels;
    }

// #
// Trasformation for Cavalier Oblique Drawings.
// (Buz z is always 0).
// It uses full depth.
// See: libgr.c

// #test
// Testing a new routine without z parameter and hand orientation.

    //libgr_transform_from_viewspace_to_screespace( 
    //    (int *) &X0, (int *) &Y0, 
    //    triangle->p[0].x, triangle->p[0].y, triangle->p[0].z,
    //    TRUE, //UseLeftHand,
    //    hotspotx, hotspoty ); 
    libgr_transform_to_screespace( 
        (int *) &X0, (int *) &Y0, 
        triangle->p[0].x, triangle->p[0].y,
        hotspotx, hotspoty ); 

    //libgr_transform_from_viewspace_to_screespace( 
    //    (int *) &X1, (int *) &Y1, 
    //    triangle->p[1].x, triangle->p[1].y, triangle->p[1].z,
    //    TRUE, //UseLeftHand,
    //    hotspotx, hotspoty ); 
    libgr_transform_to_screespace( 
        (int *) &X1, (int *) &Y1, 
        triangle->p[1].x, triangle->p[1].y,
        hotspotx, hotspoty ); 

    //libgr_transform_from_viewspace_to_screespace( 
    //    (int *) &X2, (int *) &Y2, 
    //    triangle->p[2].x, triangle->p[2].y, triangle->p[2].z,
    //    TRUE, //UseLeftHand,
    //    hotspotx, hotspoty ); 
    libgr_transform_to_screespace( 
        (int *) &X2, (int *) &Y2, 
        triangle->p[2].x, triangle->p[2].y,
        hotspotx, hotspoty ); 


// Color
    unsigned int color = (unsigned int) triangle->p[0].color;

// Draw
// Cavalier Oblique Drawings.
// The transformation was made before.

    npixels += fillTriangle0( 
                   (int) (X0 & 0xFFFFFFFF), (int) (Y0 & 0xFFFFFFFF),
                   (int) (X1 & 0xFFFFFFFF), (int) (Y1 & 0xFFFFFFFF),
                   (int) (X2 & 0xFFFFFFFF), (int) (Y2 & 0xFFFFFFFF),
                   (unsigned int) color,
                   rop );

// Return pixel counter.
    return (int) npixels;
}


// -------------------------------------------------
// IN: projected triangle.
// float
int 
plotTriangleF(
    struct gws_window_d *window,
    struct gr_triangleF3D_d *t,
    int fill,
    unsigned long rop )
{

// Number of changed pixels.
    int npixels=0;
// The projected trinagle.
// Engine triangle structure.
// Using 'int',
// because this is what we're gonna paint into a 2D screen.
    struct gr_triangle_d  triProjected;

    if (CurrentProjectionF.initialized != TRUE){
        printf("plotTriangleF: CurrentProjectionF\n");
        return (int) npixels;
    }

// #test
    //unsigned long window_width = 800;
    //unsigned long window_height = 600;
    //unsigned long window_width = gws_get_device_width();
    //unsigned long window_height = gws_get_device_height();

// Check the pointer for the given triangle structure.
    if ((void*)t == NULL){
        return (int) npixels;
    }

// Projection parameters.
// No matrix.

// z
    float znear = (float) 0.01f;  //default
    float zfar  = (float) 10.0f;  //default
// ar - Aspect ratio
    unsigned long window_width  = 200;
    unsigned long window_height = 200;
    float ar = (float) 1.0f;      //default
// Scaling factor.
// % da tela.
// fov scale factor = 1/( tan(a/2) )
// ??: x=afx and y=fy

    // #ps
    // We're not using this value.
    // We're getting another one from CurrentProjectionF.scale_factor.
    //float scale_factor = (float) 1/( tan(angleA/2) );  // degree
    //float scale_factor = (float) 1/( tan(VerticalFOV/2 * PI/180) );  //rad
    float scale_factor = (float) 0.5f;   //default

    // #bugbug
    // #todo: Return if not initialized?

    if (CurrentProjectionF.initialized == TRUE)
    {
        // ----------------
        // :: near and far
        // Simply get the near and far values.
        znear = (float) CurrentProjectionF.znear;
        zfar  = (float) CurrentProjectionF.zfar;

        // ----------------
        // ::ar (Aspect ratio)
        // 600/800 = 0.75
        // 480/640 = 0.75
        // 200/320 = 0.625
        window_width  = (unsigned long) CurrentProjectionF.width;
        window_height = (unsigned long) CurrentProjectionF.height;
        if (window_width == 0){
            printf ("Invalid ar\n");
            exit(1);
        }
        ar = 
            (float)((float) window_height / (float) window_width );
        
        // ----------------
        // scale factor
        // 0.5f
        scale_factor = (float) CurrentProjectionF.scale_factor;

        // #todo: hotspot
    }

// #todo
// Well, z needs to be between 0 and 1.
// the scale factor for z is s.
// z = (sz - sznear)

// #bugbug
// somente depois de aplicarmos o scale factor
// é que podemos realizar o clipping.

// #bugbug
// We have a scale factor do x and y.
// But we do not have a scale factor for z.
// So, z can be any vallur between 0.01f and 1000.0f.

// The 'image space'.
// Our image space is not 1:1:1
// It's something like 2:2:1000
// No z normalization

// #??
// Estamos desistindo do triangulo inteiro porque
// um de seus vetores está fora do frustum no eixo z?
// Nesse caso o triangulo só aparece se todos seus vetores
// estiverem dentro dos limites do frustum no eixo z.

// ------------
// Clipping in z

    // Near limit
    if (t->p[0].z < znear){ return 0; }
    if (t->p[1].z < znear){ return 0; }
    if (t->p[2].z < znear){ return 0; }

    // Far limit
    if (t->p[0].z > zfar){ return 0; }
    if (t->p[1].z > zfar){ return 0; }
    if (t->p[2].z > zfar){ return 0; }


// ------------
// Zoom

// Ficando menor conforme z aumenta.
// Quanto maior for o z, mais próximo do centro da tela
// o vetor ficará. Ou seja, quanto mais longe estão os objetos,
// mais objetos cabem na tela.

// #test
// Testing z factor.

// Bigger z, smaller objects, closer from the center,
    //float factor = 2.0f; 
// Normal
    float factor = 1.0f;
// Smaller z, bigger objects, less closer from the center.
    //float factor = 0.5f;

    if (t->p[0].z != 0.0f)
    {
        t->p[0].z = (float) (t->p[0].z * factor);
        t->p[0].x = (float) (t->p[0].x/t->p[0].z);
        t->p[0].y = (float) (t->p[0].y/t->p[0].z);
    }
    if (t->p[1].z != 0.0f)
    {
        t->p[1].z = (float) (t->p[1].z * factor);
        t->p[1].x = (float) (t->p[1].x/t->p[1].z);
        t->p[1].y = (float) (t->p[1].y/t->p[1].z);
    }
    if (t->p[2].z != 0.0f)
    {
        t->p[2].z = (float) (t->p[2].z * factor);
        t->p[2].x = (float) (t->p[2].x/t->p[2].z);
        t->p[2].y = (float) (t->p[2].y/t->p[2].z);
    }

// -------------------
// Zoom (test)

/*
 //#wrong?
    if (t->p[0].z != 0.0f)
    {
        t->p[0].x = (float) (t->p[0].x / t->p[0].z * tanf(45.5f/2) );
        t->p[0].y = (float) (t->p[0].y / t->p[0].z * tanf(45.5f/2));
    } 
    if (t->p[1].z != 0.0f)
    {
        t->p[1].x = (float) (t->p[1].x/t->p[1].z * tanf(45.5f/2));
        t->p[1].y = (float) (t->p[1].y/t->p[1].z * tanf(45.5f/2));
    }
    if (t->p[2].z != 0.0f)
    {
        t->p[2].x = (float) (t->p[2].x/t->p[2].z * tanf(45.5f/2));
        t->p[2].y = (float) (t->p[2].y/t->p[2].z * tanf(45.5f/2));
    }
*/


// ------------------
// Scaling.

// Ajustando à tela.
// x=afx  y=fx
// So, we're using the 0.5f for scale factor,
// but we gotta use 1/(tan(a/2))


    //#test
    // tan(45)=1
    // tan(90/2)=1
    //scale_factor = (float) ( 1.0f/tanf(90.0f*0.5f)/2.0f);
    //scale_factor = (float) (1/tanf(45.0f/2.0f));  //degree
    //scale_factor = (float) (1/tanf(80.0f * 0.5f / 180.0f * 3.14159f));  // rad

// Se o scale_factor for 0.5f o valor será
// metade da tela.
// então -1 +1 preenche a tela toda.

// Quanto menor for campo de visão, 
// maior sera o scale factor e maior sera o objeto.
// Quanto maior for o campo de visão,
// menor sera o scale factor e menor sera o objeto.

    //#test
    //scale_factor = 0.1f;  // fov grande, scale factor pequeno, objeto pequeno
    //scale_factor = 0.5f;  // fov medio, scale factor medio, objeto normal
    //scale_factor = 1.0f;  // fov pequeno, scale factor grande, objeto grande

    //#test
    //scale_factor = 0.01f;  // very small.
    //scale_factor = 2.0f;

// -----------------------
// Scaling
// Apply the limit for the scale factor.
// We can scale, but not too few, but not too much.

    if ( (float) scale_factor < 0.01f ){
        scale_factor = (float) 0.01f;
    }
    if ( (float) scale_factor > 2.00f ){
        scale_factor = (float) 2.00f;
    }

// --------------------------------
// Scale into view.
// original value * ar * scale factor * width.
// original value * ar * scale factor * height.
// Ex: (0.5 * 0.75 * 0.5 * 800) = 150
// Ex: (0.5 * 0.75 * 0.5 * 600) = 112.5

    long x0 = (long) (t->p[0].x *ar * scale_factor * (float) window_width);
    long y0 = (long) (t->p[0].y     * scale_factor * (float) window_height);
    long x1 = (long) (t->p[1].x *ar * scale_factor * (float) window_width);
    long y1 = (long) (t->p[1].y     * scale_factor * (float) window_height);
    long x2 = (long) (t->p[2].x *ar * scale_factor * (float) window_width);
    long y2 = (long) (t->p[2].y     * scale_factor * (float) window_height);

//
// The projected trinagle.
//

// ----------------------------------------
// The projected triangle. (for 2D screen).
// Final rectangle. Using 'int'.
// Considerando o hotspot no centro da tela,
// o valor tem que ser menor que 800/2 
// ou menor que 600/2.

    triProjected.p[0].x = (int) (x0 & 0xFFFFFFFF);
    triProjected.p[0].y = (int) (y0 & 0xFFFFFFFF);
    triProjected.p[0].z = (int) 0;  // No z.
    triProjected.p[0].color = t->p[0].color; //COLOR_WHITE;
    
    triProjected.p[1].x = (int) (x1 & 0xFFFFFFFF);
    triProjected.p[1].y = (int) (y1 & 0xFFFFFFFF);
    triProjected.p[1].z = (int) 0;  // No z.
    triProjected.p[1].color = t->p[1].color; // COLOR_WHITE;
    
    triProjected.p[2].x = (int) (x2 & 0xFFFFFFFF);
    triProjected.p[2].y = (int) (y2 & 0xFFFFFFFF);
    triProjected.p[2].z = (int) 0;  // No z.
    triProjected.p[2].color = t->p[2].color; // COLOR_WHITE;

    triProjected.used = TRUE;
    triProjected.initialized = TRUE;

    //#debug
    //printf("x0=%d y0=%d | x1=%d y1=%d | x2=%d y2=%d \n",
    //    triProjected.p[0].x, triProjected.p[0].y, 
    //    triProjected.p[1].x, triProjected.p[1].y, 
    //    triProjected.p[2].x, triProjected.p[2].y );

//
// Plot triangle
//

// #alert
// Our engine when painting 3D stuff using 'int'
// has the venter of the screen as a reference,
// So, if we translated the triangle, we did it
// from the center, not from the top/left corner.

// Filled
// We need a valid window.
// #todo: return pixel counter.
// The projected triangle uses 'int'.
    if (fill){
        npixels += fillTriangle( 
                       window, 
                       &triProjected, 
                       HotSpotX, HotSpotY,
                       rop );

        return (int) npixels;
    }

// Not filled.
// we dont need a valid window.
// #todo: return pixel counter.
// #todo: rop parameter.
// The projected triangle uses 'int'.
    if (!fill){
        npixels += grTriangle3( window, &triProjected );
    }

// Return pixel counter.
    return (int) npixels;
}
//------------------


// -------------------------------------------------
// IN: projected vector.
// float
int 
plotPixelF(
    struct gws_window_d *window,
    struct gr_vecF3D_d *vec,
    unsigned long rop )
{
// Plot a single pixel.
// # Not tested yet.
// We have a lot of calculations only for a single pixel.
// This routine is good for some kind of ray tracing.

// Number of changed pixels.
    int npixels=0;

// Engine triangle structure.
// Using 'int',
    //struct gr_triangle_d final_triangle;

    if (CurrentProjectionF.initialized != TRUE){
        printf("plotTriangleF: CurrentProjectionF\n");
        return (int) npixels;
    }

// #test
    //unsigned long window_width = 800;
    //unsigned long window_height = 600;
    //unsigned long window_width = gws_get_device_width();
    //unsigned long window_height = gws_get_device_height();

// Check the 'projected triangle'.
    if ((void*)vec == NULL){
        return (int) npixels;
    }

// Projection parameters.
// No matrix.

    // z
    float znear = (float) 0.01f;  //default
    float zfar  = (float) 10.0f;  //default
    // ar
    unsigned long window_width  = 200;
    unsigned long window_height = 200;
    float ar = (float) 1.0f;      //default
    // scaling factor.
    // % da tela.
    // fov scale factor = 1/( tan(a/2) )
    // x=afx and y=fy
    float scale_factor = (float) 0.5f;   //default
 
    if (CurrentProjectionF.initialized == TRUE)
    {
        znear = (float) CurrentProjectionF.znear;
        zfar  = (float) CurrentProjectionF.zfar;

        window_width  = (unsigned long) CurrentProjectionF.width;
        window_height = (unsigned long) CurrentProjectionF.height;
        if (window_width == 0){
            printf ("Invalid aspect ratio\n");
            exit(1);
        }
        // 600/800 = 0.75
        // 480/640 = 0.75
        // 200/320 = 0.625
        ar = 
            (float)((float) window_height / (float) window_width );
        
        // scale factor
        // 0.5f
        // #todo: hotspot
        scale_factor = (float) CurrentProjectionF.scale_factor;
    }

// #todo
// Well, z needs to be between 0 and 1.
// the scale factor for z is s.
// z = (sz - sznear)

// #bugbug
// somente depois de aplicarmos o scale factor
// é que podemos realizar o clipping.

// #bugbug
// We have a scale factor do x and y.
// But we do not have a scale factor for z.
// So, z can be any vallur between 0.01f and 1000.0f.

// The 'image space'.
// Our image space is not 1:1:1
// It's something like 2:2:1000
// No z normalization

// Clipping in z

    if (vec->z < znear){ return 0; }
    if (vec->z > zfar) { return 0; }

// #test
// Ficando menor conforme z aumenta.

/*
    if (t->p[0].z != 0.0f)
    {
        t->p[0].x = (float) (t->p[0].x/t->p[0].z);
        t->p[0].y = (float) (t->p[0].y/t->p[0].z);
    }
    if (t->p[1].z != 0.0f)
    {
        t->p[1].x = (float) (t->p[1].x/t->p[1].z);
        t->p[1].y = (float) (t->p[1].y/t->p[1].z);
    }
    if (t->p[2].z != 0.0f)
    {
        t->p[2].x = (float) (t->p[2].x/t->p[2].z);
        t->p[2].y = (float) (t->p[2].y/t->p[2].z);
    }
*/

/*
 //#wrong?
    if (t->p[0].z != 0.0f)
    {
        t->p[0].x = (float) (t->p[0].x / t->p[0].z * tanf(45.5f/2) );
        t->p[0].y = (float) (t->p[0].y / t->p[0].z * tanf(45.5f/2));
    } 
    if (t->p[1].z != 0.0f)
    {
        t->p[1].x = (float) (t->p[1].x/t->p[1].z * tanf(45.5f/2));
        t->p[1].y = (float) (t->p[1].y/t->p[1].z * tanf(45.5f/2));
    }
    if (t->p[2].z != 0.0f)
    {
        t->p[2].x = (float) (t->p[2].x/t->p[2].z * tanf(45.5f/2));
        t->p[2].y = (float) (t->p[2].y/t->p[2].z * tanf(45.5f/2));
    }
*/

// scale
// Ajustando à tela.
// x=afx  y=fx
// So, we're using the 0.5f for scale factor,
// but we gotta use 1/(tan(a/2))


    //#test
    // tan(45)=1
    // tan(90/2)=1
    //scale_factor = (float) ( 1.0f/tanf(90.0f*0.5f)/2.0f);
    //scale_factor = (float) (1/tanf(45.0f/2.0f));
    //scale_factor = (float) (1/tanf(80.0f * 0.5f / 180.0f * 3.14159f));

// Se o scale_factor for 0.5f o valor será
// metade da tela.
// então -1 +1 preenche a tela toda.

// Quanto menor for campo de visão, 
// maior sera o scale factor e maior sera o objeto.
// Quanto maior for o campo de visão,
// menor sera o scale factor e menor sera o objeto.

    //#test
    //scale_factor = 0.1f;  // fov grande, scale factor pequeno, objeto pequeno
    //scale_factor = 0.5f;  // fov medio, scale factor medio, objeto normal
    //scale_factor = 1.0f;  // fov pequeno, scale factor grande, objeto grande

    //#test
    //scale_factor = 0.01f;  // very small.
    //scale_factor = 2.0f;

// #tmp
// Scale factor limits
    if ( (float) scale_factor < 0.01f ){
        scale_factor = (float) 0.01f;
    }
    if ( (float) scale_factor > 2.00f ){
        scale_factor = (float) 2.00f;
    }

    // Ex: (0.5 * 0.75 * 0.5 * 800) = 150
    // Ex: (0.5 * 0.75 * 0.5 * 600) = 112.5
    long x0 = (long) (vec->x *ar * scale_factor * (float) window_width);
    long y0 = (long) (vec->y     * scale_factor * (float) window_height);
    //long x1 = (long) (t->p[1].x *ar * scale_factor * (float) window_width);
    //long y1 = (long) (t->p[1].y     * scale_factor * (float) window_height);
    //long x2 = (long) (t->p[2].x *ar * scale_factor * (float) window_width);
    //long y2 = (long) (t->p[2].y     * scale_factor * (float) window_height);

// Final rectangle. Using 'int'.
// Considerando o hotspot no centro,
// o valor tem que ser menor que 800/2 
// ou menor que 600/2.

    int final_x = (int) ( x0 & 0xFFFFFFFF);
    int final_y = (int) ( y0 & 0xFFFFFFFF);
    //int final_z = (int) 0;
    unsigned int final_color = (unsigned int) vec->color;

    //final_triangle.p[0].x = (int) ( x0 & 0xFFFFFFFF);
    //final_triangle.p[0].y = (int) ( y0 & 0xFFFFFFFF);
    //final_triangle.p[0].z = (int) 0;
    //final_triangle.p[0].color = t->p[0].color; //COLOR_WHITE;
    
    //final_triangle.p[1].x = (int) ( x1 & 0xFFFFFFFF);
    //final_triangle.p[1].y = (int) ( y1 & 0xFFFFFFFF);
    //final_triangle.p[1].z = (int) 0;
    //final_triangle.p[1].color = t->p[1].color; // COLOR_WHITE;
    
    //final_triangle.p[2].x = (int) ( x2 & 0xFFFFFFFF);
    //final_triangle.p[2].y = (int) ( y2 & 0xFFFFFFFF);
    //final_triangle.p[2].z = (int) 0;
    //final_triangle.p[2].color = t->p[2].color; // COLOR_WHITE;

    //final_triangle.used = TRUE;
    //final_triangle.initialized = TRUE;

    //#debug
    //printf("x0=%d y0=%d | x1=%d y1=%d | x2=%d y2=%d \n",
    //    final_triangle.p[0].x, final_triangle.p[0].y, 
    //    final_triangle.p[1].x, final_triangle.p[1].y, 
    //    final_triangle.p[2].x, final_triangle.p[2].y );

// Not filled.
// we dont need a valid window.
// #todo: return pixel counter.
    //if (!fill){
    //    npixels += grTriangle3( window, &final_triangle );
    //}

// Filled
// We need a valid window.
// #todo: return pixel counter.
    //if (fill){
    //    npixels += fillTriangle( 
    //                   window, 
    //                   &final_triangle, 
    //                   HotSpotX, HotSpotY, 0 );
    //}

// Plot 2d pixel usiing top/left 0:0.
// IN: color, x, y, rop.
    grPlot2D( 
        (unsigned int) final_color, 
        (int) final_x, 
        (int) final_y, 
        (unsigned long) rop );

// Return pixel counter.
    //return (int) npixels;
    return 1;
}
//------------------



// Polyline
// O segundo ponto da linha 
// vira o primeiro ponto da próxima linha.
int xxxPolygonZ ( struct gr_polygon_d *polygon )
{
    int i=0;
    int NumberOfElements=0;
    int Max = 32;

    // list of polygon pointers.
    unsigned long *list = (unsigned long *) polygon->list_address;

    // two vectors to create a line.
    struct gr_vec3D_d *v1;
    struct gr_vec3D_d *v2;

    int PolygonType=0;

// structure
    if ( (void*) polygon == NULL ){
        goto fail;
    }
// list 
    if ( (void*) list == NULL ){
        goto fail;
    }
// number of elements
    NumberOfElements = polygon->n;
    if (NumberOfElements > Max){
        goto fail;
    }

// polygon type

    PolygonType = polygon->type;

    switch(PolygonType){
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
        if ( (void*) v1 == NULL ){ 
            gwssrv_debug_print(">>>> BREAK\n");
            break; 
        }

        // Draw
        grPlot0 ( NULL, v1->z, v1->x, v1->y, v1->color, 0 );
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
            gwssrv_debug_print(">>>> BREAK\n");
            break; 
        }

        gwssrv_debug_print(">>>> DRAW LINE\n");
    
        // Plot
        
        plotLine3d (
            NULL,
            v1->x, v1->y, v1->z,  // first point
            v2->x, v2->y, v2->z,  // second point
            v1->color );
    };
    return 0;

fail:
    gwssrv_debug_print("xxxPolygonZ: fail\n");
    return -1;
}

// #test
// >>>> wired cube <<<< 
// "with ugly rasterization in some of the faces"
// This rasterization uses lines and applies only in the faces
// with not inclination.
// #todo
// Create function parameters to select if we will use some
// kind of rasterization or not.
// #todo
// We can create a function only for wired cube, 
// and another one for cube with some kind of rasterization.

int xxxDrawCubeZ (struct gr_cube_d *cube)
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
        return -1;
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
        NULL,
        cube->p[4].x, cube->p[4].y, cube->p[4].z, 
        cube->p[5].x, cube->p[5].y, cube->p[5].z, cube->p[4].color );
    plotLine3d (  // direita
        NULL,
        cube->p[5].x, cube->p[5].y, cube->p[5].z, 
        cube->p[6].x, cube->p[6].y, cube->p[6].z, cube->p[5].color );
    plotLine3d (  // baixo 
        NULL,
        cube->p[7].x, cube->p[7].y, cube->p[7].z, 
        cube->p[6].x, cube->p[6].y, cube->p[6].z, cube->p[7].color );
    plotLine3d (  // esquerda
        NULL,
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
    plotLine3d ( NULL,
                cube->p[0].x, cube->p[0].y, cube->p[0].z, 
                cube->p[4].x, cube->p[4].y, cube->p[4].z, cube->p[0].color );
    // 2nd line
    plotLine3d (NULL,
                cube->p[1].x, cube->p[1].y, cube->p[1].z, 
                cube->p[5].x, cube->p[5].y, cube->p[5].z, cube->p[1].color );

//====================
// -- bottom face -------------------------------------
    // 1rd line
    plotLine3d (NULL,
                cube->p[2].x, cube->p[2].y, cube->p[2].z, 
                cube->p[6].x, cube->p[6].y, cube->p[6].z, cube->p[2].color );
    // 2th line 
    plotLine3d (NULL,
                cube->p[3].x, cube->p[3].y, cube->p[3].z, 
                cube->p[7].x, cube->p[7].y, cube->p[7].z, cube->p[3].color );

//=====================================================

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
        NULL,
        cube->p[0].x, cube->p[0].y, cube->p[0].z, 
        cube->p[1].x, cube->p[1].y, cube->p[1].z, cube->p[0].color );
    plotLine3d (  // direita
        NULL,
        cube->p[1].x, cube->p[1].y, cube->p[1].z, 
        cube->p[2].x, cube->p[2].y, cube->p[2].z, cube->p[1].color );
    plotLine3d (  // baixo
        NULL,
        cube->p[3].x, cube->p[3].y, cube->p[3].z,
        cube->p[2].x, cube->p[2].y, cube->p[2].z, cube->p[3].color );
    plotLine3d (  // esquerda
        NULL,
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

//Circle
//This is an implementation of the circle algorithm.
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
   
    //loop
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

// ?? what means 'm' ???
void 
grCircle3 ( 
    struct gws_window_d *window,
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
      
      grPlot0 ( window, z, xm-x, ym+y, color, 0);
      grPlot0 ( window, z, xm-y, ym-x, color, 0);
      grPlot0 ( window, z, xm+x, ym-y, color, 0);
      grPlot0 ( window, z, xm+y, ym+x, color, 0);

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
        };
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

//Ellipse
//This program example plots an ellipse inside a specified rectangle.

// 2D
void 
grEllipse (
    int x0, int y0, 
    int x1, int y1, 
    unsigned int color )
{
   int a = abs(x1-x0), b = abs(y1-y0), b1 = b&1; /* values of diameter */
   long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
   long err = dx+dy+b1*a*a, e2; /* error of 1.step */

   if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
   if (y0 > y1) y0 = y1; /* .. exchange them */
   y0 += (b+1)/2; y1 = y0-b1;   /* starting pixel */
   a *= 8*a; b1 = 8*b*b;

    do {
       grPlot0 ( NULL, 0, x1, y0, color, 0);  //   I. Quadrant
       grPlot0 ( NULL, 0, x0, y0, color, 0);  //  II. Quadrant
       grPlot0 ( NULL, 0, x0, y1, color, 0);  // III. Quadrant
       grPlot0 ( NULL, 0, x1, y1, color, 0);  //  IV. Quadrant
       
       e2 = (2*err);
       if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */ 
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
    
    } while (x0 <= x1);


    /* too early stop of flat ellipses a=1 */
    while (y0-y1 < b) {
        grPlot0 ( NULL, 0, x0-1,    y0, color, 0);  //-> finish tip of ellipse
        grPlot0 ( NULL, 0, x1+1,  y0++, color, 0);
        grPlot0 ( NULL, 0, x0-1,    y1, color, 0);
        grPlot0 ( NULL, 0, x1+1,  y1--, color, 0);
    };
}

void 
grEllipse3 (
    int x0, int y0, 
    int x1, int y1, 
    unsigned int color,
    int z )
{

   int a = abs(x1-x0), b = abs(y1-y0), b1 = b&1; /* values of diameter */
   long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
   long err = dx+dy+b1*a*a, e2; /* error of 1.step */

   if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
   if (y0 > y1) y0 = y1; /* .. exchange them */
   y0 += (b+1)/2; y1 = y0-b1;   /* starting pixel */
   a *= 8*a; b1 = 8*b*b;


    do {
       grPlot0 ( NULL, z, x1, y0, color, 0);  //   I. Quadrant
       grPlot0 ( NULL, z, x0, y0, color, 0);  //  II. Quadrant
       grPlot0 ( NULL, z, x0, y1, color, 0);  // III. Quadrant
       grPlot0 ( NULL, z, x1, y1, color, 0);  //  IV. Quadrant
       
       e2 = (2*err);
       if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */ 
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
    
    } while (x0 <= x1);

/* Too early stop of flat ellipses a=1 */
    while (y0-y1 < b) {
        grPlot0 ( NULL, z, x0-1,    y0, color, 0);  // -> finish tip of ellipse
        grPlot0 ( NULL, z, x1+1,  y0++, color, 0);
        grPlot0 ( NULL, z, x0-1,    y1, color, 0);
        grPlot0 ( NULL, z, x1+1,  y1--, color, 0);
    };
}

/*
 * noraDrawingStuff: 
 *    Lot of dots in the right place. (prime stuff)
 *    2020 - Created by Fred Nora. 
 */

void noraDrawingStuff(void)
{
    register int x=0;
    register int y=0;
    unsigned int color = COLOR_BLACK;

// Colunas.
    for (x=0; x<SavedX; x++)
    {
        for (y=0; y<SavedY; y++)
        {
            if ( x != 0 ){
                if ( (y % x) == 0 ){
                    // IN: color, x, y, rop
                    libdisp_backbuffer_putpixel(color,x,y,0); 
                }
            }
        };
        if ( x >= SavedY ) { break; }
    };
}


/*
 * noraDrawingStuff3: 
 *    Lot of dots in the right place. (prime stuff)
 *    2020 - Created by Fred Nora. 
 */

void noraDrawingStuff3 (int x, int y, int z)
{
    register int _x=0;
    register int _y=0;
    register int _z = z;

    int limitX = (SavedX >> 1);
    int limitY = (SavedY >> 1);

    // colunas.
    for (_x=x; _x<limitX; _x++)    
    {
        for (_y=y; _y<limitY; _y++)
        {
            if (_x != 0)
            {
                if ((_y % _x) == 0){
                    grPlot0 ( NULL, _z, _x, _y, COLOR_BLACK, 0 );
                }
            }
        };
        if ( _x >= limitY) { break; }
    };
}


/* 
// This function multiplies mat1[][] and mat2[][],
// and stores the result in res[][]

void 
matrix_multiply_2x3 ( 
    int mat1[2][3], 
    int mat2[2][3], 
    int res[2][3])
{

    int N1 = 2;
    int N2 = 3;
    
    int i, j, k;
    
    
    for (i = 0; i < N1; i++) 
    {
        for (j = 0; j < N2; j++) 
        {
            res[i][j] = 0;
            
            //for (k = 0; k < N1; k++)
            for (k = 0; k < N2; k++)
                res[i][j] += mat1[i][k] * mat2[k][j];
        }
    }
}
*/

//--------------------------------------------------
// Rotate in x
int 
gr_rotate_x(
    struct gr_triangleF3D_d *in_tri,
    struct gr_triangleF3D_d *out_tri,   // rotated 
    float angle, 
    float fElapsedTime )
{
    struct gr_mat4x4_d matRotX;

    //angle += 1.0f * fElapsedTime;
    angle = (float) angle * (float) fElapsedTime;

// gerando a matriz de tranformação.
// Rotation X
    matRotX.m[0][0] = (float) 1.0f;
    matRotX.m[1][1] =  cosf(angle * 0.5f); //why 0.5f?
    matRotX.m[1][2] =  -sinf(angle * 0.5f);
    matRotX.m[2][1] = sinf(angle * 0.5f);
    matRotX.m[2][2] =  cosf(angle * 0.5f);
    matRotX.m[3][3] = (float) 1.0f;

// ---------------------------------------------------

    if ( (void*) in_tri == NULL )
        return -1;
    if ( (void*) out_tri == NULL )
        return -1;

//-----------------------------    
// Rotate in X-Axis
    gr_MultiplyMatrixVector(
        (struct gr_vecF3D_d *) &in_tri->p[0], 
        (struct gr_vecF3D_d *) &out_tri->p[0], 
        &matRotX);
    gr_MultiplyMatrixVector(
        (struct gr_vecF3D_d *) &in_tri->p[1], 
        (struct gr_vecF3D_d *) &out_tri->p[1], 
        &matRotX);
    gr_MultiplyMatrixVector(
        (struct gr_vecF3D_d *) &in_tri->p[2], 
        (struct gr_vecF3D_d *) &out_tri->p[2], 
        &matRotX);

    return 0;
}

//--------------------------------------------------
// Rotate in y
int 
gr_rotate_y(
    struct gr_triangleF3D_d *in_tri,
    struct gr_triangleF3D_d *out_tri,   // rotated 
    float angle, 
    float fElapsedTime )
{
    struct gr_mat4x4_d matRotY;

    //angle += 1.0f * fElapsedTime;
    angle = (float) angle * (float) fElapsedTime;

// gerando a matriz de tranformação.
// Rotation Y
    matRotY.m[0][0] = cosf(angle);
    matRotY.m[0][2] = sinf(angle);
    matRotY.m[1][1] = (float) 1.0f;
    matRotY.m[2][0] = -sinf(angle);
    matRotY.m[2][2] = cosf(angle);
    matRotY.m[3][3] = (float) 1.0f;

// ---------------------------------------------------
    if ( (void*) in_tri == NULL )
        return -1;
    if ( (void*) out_tri == NULL )
        return -1;

//-----------------------------    
// Rotate in Y-Axis
    gr_MultiplyMatrixVector(
        (struct gr_vecF3D_d *) &in_tri->p[0], 
        (struct gr_vecF3D_d *) &out_tri->p[0], 
        &matRotY);
    gr_MultiplyMatrixVector(
        (struct gr_vecF3D_d *) &in_tri->p[1], 
        (struct gr_vecF3D_d *) &out_tri->p[1], 
        &matRotY);
    gr_MultiplyMatrixVector(
        (struct gr_vecF3D_d *) &in_tri->p[2], 
        (struct gr_vecF3D_d *) &out_tri->p[2], 
        &matRotY);

    return 0;
}

//--------------------------------------------------
// Rotate in z
int 
gr_rotate_z(
    struct gr_triangleF3D_d *in_tri,
    struct gr_triangleF3D_d *out_tri,   // rotated 
    float angle, 
    float fElapsedTime )
{
    struct gr_mat4x4_d matRotZ; 

    //angle += 1.0f * fElapsedTime;
    angle = (float) angle * (float) fElapsedTime;

// gerando a matriz de tranformação.
// Rotation Z
	matRotZ.m[0][0] =  cosf(angle);
	matRotZ.m[0][1] =  -sinf(angle);
	matRotZ.m[1][0] = sinf(angle);
	matRotZ.m[1][1] =  cosf(angle);
	matRotZ.m[2][2] = (float) 1.0f;
	matRotZ.m[3][3] = (float) 1.0f;

// ---------------------------------------------------
    if ( (void*) in_tri == NULL )
        return -1;
    if ( (void*) out_tri == NULL )
        return -1;

// rotação em z,
// o output esta no triRotatedZ.
// os 3 vetores foram modificados.
//-----------------------------    
// Rotate in Z-Axis
    gr_MultiplyMatrixVector(
        (struct gr_vecF3D_d *) &in_tri->p[0], 
        (struct gr_vecF3D_d *) &out_tri->p[0], 
        &matRotZ);
    gr_MultiplyMatrixVector(
        (struct gr_vecF3D_d *) &in_tri->p[1], 
        (struct gr_vecF3D_d *) &out_tri->p[1], 
        &matRotZ);
    gr_MultiplyMatrixVector(
        (struct gr_vecF3D_d *) &in_tri->p[2], 
        (struct gr_vecF3D_d *) &out_tri->p[2], 
        &matRotZ);
//-----------------------------    

    return 0;
}
//--------------------------------------------------


/*
 * plotCharBackbufferDrawcharTransparent:
 *     Desenha um caractere sem alterar o pano de fundo.
 *     >> no backbuffer.
 */
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
// Loop
    register int y2=0;
    register int x2=0;

    char *work_char;
    unsigned char bit_mask = 0x80;

    //int CharWidth;
    //int CharHeight;

/*
 * Get the font pointer.
 * #todo:
 *     usar variavel g8x8fontAddress.
 *     + Criar e usar uma estrutura para fonte.
 *     + Usar o ponteiro para a fonte atual que foi carregada.
 *     + Criar um switch para o tamanho da fonte.
 *     isso deveria estar na inicialização do módulo char.
 */

    if ( gws_currentfont_address == 0 || 
         gcharWidth <= 0 || 
         gcharHeight <= 0 )
    {
        //gws_currentfont_address = (unsigned long) BIOSFONT8X8;  //ROM bios.
        //gcharWidth = DEFAULT_CHAR_WIDTH;    //8.
        //gcharHeight = DEFAULT_CHAR_HEIGHT;  //8.
        // #debug
        // Estamos parando para testes.
        printf ("gws_drawchar_transparent : Initialization fail\n");
        while(1){
        }
    }

// #todo: 
// Criar essas variáveis e definições.

    switch (gfontSize){

		//case FONT8X8:
	        //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //getFontAddress(...)
		    //gcharWidth = 8;
		    //gcharHeight = 8;
		    //set_char_width(8);
			//set_char_height(8);
			//break;
		
		//case FONT8X16:
	        //gws_currentfont_address = (unsigned long) BIOSFONT8X16;    //getFontAddress(...)
		    //gcharWidth = 8;
		    //gcharHeight = 16;
		    //set_char_width(8);
			//set_char_height(16);
		    //break;
		 
		//#todo: 
		//Criar opções
		//...
		
		// #importante:
		// #BUGBUG
		// Se não temos um tamanho selecionado então teremos 
		// que usar o tamanho padrão.
		
        default:
            //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //ROM bios.
            //set_char_width(8);
            //set_char_height(8);
            //gfontSize = FONT8X8;  //#todo: fução para configurar isso.
            break;
    };

// O caractere sendo trabalhado.
    work_char = 
        (void *) gws_currentfont_address + (c * gcharHeight);

// Draw
    for ( y2=0; y2 < gcharHeight; y2++ )
    {
        bit_mask = 0x80;
        for ( x2=0; x2 < gcharWidth; x2++ )
        {
            // Put pixel.
            // IN: z,x,y,color,rop.
            if ( ( *work_char & bit_mask ) ){
                grPlot0 ( NULL, 0, x + x2, y, color, 0 ); 
            }

            // Rotate bitmask.
            bit_mask = (bit_mask >> 1);  
        };
        // Próxima linha da 8 linhas do caractere.
        y++; 
        work_char++; 
    };
}


/*
 * plotCharBackbufferDrawcharTransparentZ:
 *     Desenha um caractere sem alterar o pano de fundo.
 *     >> no backbuffer.
 */
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
 * #todo:
 *     usar variavel g8x8fontAddress.
 *     + Criar e usar uma estrutura para fonte.
 *     + Usar o ponteiro para a fonte atual que foi carregada.
 *     + Criar um switch para o tamanho da fonte.
 *     isso deveria estar na inicialização do módulo char.
 */

    if ( gws_currentfont_address == 0 || 
         gcharWidth <= 0 || 
         gcharHeight <= 0 )
    {
        //gws_currentfont_address = (unsigned long) BIOSFONT8X8;  //ROM bios.
        //gcharWidth = DEFAULT_CHAR_WIDTH;    //8.
        //gcharHeight = DEFAULT_CHAR_HEIGHT;  //8.
        // #debug
        // Estamos parando para testes.
        printf ("gws_drawchar_transparent : Initialization fail\n");
        while(1){}
    }

// #todo: 
// Criar essas variáveis e definições.

    switch (gfontSize){

		//case FONT8X8:
	        //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //getFontAddress(...)
		    //gcharWidth = 8;
		    //gcharHeight = 8;
		    //set_char_width(8);
			//set_char_height(8);
			//break;
		
		//case FONT8X16:
	        //gws_currentfont_address = (unsigned long) BIOSFONT8X16;    //getFontAddress(...)
		    //gcharWidth = 8;
		    //gcharHeight = 16;
		    //set_char_width(8);
			//set_char_height(16);
		    //break;
		 
		//#todo: 
		//Criar opções
		//...
		
		// #importante:
		// #BUGBUG
		// Se não temos um tamanho selecionado então teremos 
		// que usar o tamanho padrão.
		
        default:
		    //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //ROM bios.

			//set_char_width(8);
			//set_char_height(8);
            //gfontSize = FONT8X8;  //#todo: fução para configurar isso.

            break;
    };

// O caractere sendo trabalhado.

    work_char = 
        (void *) gws_currentfont_address + (c * gcharHeight);

// Draw
// #todo
// We need a flat to invert or not.
// invert

    // ok. it works
    for ( y2=0; y2 < gcharHeight; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < gcharWidth; x2++ )
        {
            // Put pixel. 
            // começa do fim
            // IN: z,x,y,color,rop.
            if ( ( *work_char & bit_mask ) ){
                grPlot0 ( NULL, z, x + x2, (y + gcharWidth), color, 0 ); 
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
    long xx = x0-x1, yy = y0-y1, xy;         /* relative values for checks */
    //double dx, dy, err, cur = xx*sy-yy*sx;                    /* curvature */

    //loop
    register long dx=0;
    register long dy=0;
    
    long err=0; 
    long cur = xx*sy-yy*sx;

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
        NULL,
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

int servicepixelBackBufferPutpixel(void)
{
    unsigned long *message_address = (unsigned long *) &__buffer[0];
    unsigned long x,y,color,rop;

    x = message_address[4];
    y = message_address[5];
    color = (unsigned int) message_address[6];
    //rop = message_address[?];

    if (x<0){ return -1; }
    if (y<0){ return -1; }

    libdisp_backbuffer_putpixel(color,x,y,0);

    return 0;
}

//
// End
//


