// comp.c
// The purpose of these routines is compose the final frame
// into the backbuffer and display it into the frontbuffer.
// Created by Fred Nora.

#include "../gwsint.h"


// It manages the compositor behavior.
struct compositor_d  Compositor;

// #todo
// Create some configuration globals here
// int gUseSomething = TRUE;
// ...

extern struct gws_window_d *mouse_hover;

//old
static long __old_mouse_x=0;
static long __old_mouse_y=0;
//current
static long __new_mouse_x=0;
static long __new_mouse_y=0;
static int __mouse_initialized = FALSE;

// Apaga mouse pointer.
// Let's clear the area where the mouse was painted.
// Flushing the area from backbuffer to LFB.
// It erases the pointer in the screen.
static int __clear_mousebox = FALSE;


// --------------------------

static void direct_draw_mouse_pointer(void);

// --------------------------

// Refresh screen via kernel.
// Copy the backbuffer in the frontbuffer(lfb).
// #??
// It uses the embedded display server in the kernel.
//#define	SYSTEMCALL_REFRESHSCREEN        11
// #todo
// trocar o nome dessa systemcall.
// refresh screen será associado à refresh all windows.


// ??
// Using the kernel to show the backbuffer.
void gwssrv_show_backbuffer(void)
{
    gramado_system_call(11,0,0,0);
}

int flush_window(struct gws_window_d *window)
{
    //if ((void*) window == NULL)
        //return -1;
    return (int) gws_show_window_rect(window);
}

int flush_window_by_id(int wid)
{
    struct gws_window_d *w;
// wid
    if (wid<0 || wid >= WINDOW_COUNT_MAX){
        goto fail;
    }
// Structure validation
    w = (void*) windowList[wid];
    if ((void*) w == NULL){
        goto fail;
    }
// Flush
    flush_window(w);
    return 0;
    //return (int) flush_window(w);
fail:
    return (int) (-1);
}

// Flush the whole backbuffer.
// see: painter.c
void flush_frame(void)
{
    wm_flush_screen();
}

/*
 * reactRefreshDirtyWindows: 
 */
// Called by wmReactToPaintEvents().
// O compositor deve ser chamado para compor um frame 
// logo após uma intervenção do painter, que reaje às
// ações do usuário.
// Ele não deve ser chamado X vezes por segundo.
// Quem deve ser chamado X vezes por segundo é a rotina 
// de refresh, que vai efetuar refresh dos retângulos sujos e
// dependendo da ação do compositor, o refresh pode ser da tela toda.
// Refresh
// Lookup the main window list.
// #todo: This is very slow. We need a linked list.
// Get next
// It is a valid window and
// it is a dirty window.
// Flush the window's rectangle and invalidate the window.
// see: rect.c

void reactRefreshDirtyWindows(void)
{
// Called by compose.
// + We need to encapsulate the variables used by this routine
//   to prevent about concorrent access problems.
// #bugbug
// This is not a effitient way of doing this.
// We got to refresh folowind the bottom top order.
// + If we refreshed the background window, so,we dont
//   need to refresh any other window when we're not using 
//   individual buffer for the windows in the compositor.

    register int i=0;

// The component.
// It's a window, but we don't care about its type.
// All we need to do is refreshing the window's rectangle.

    struct gws_window_d *w;

// Is the root window a valid window

// Get the window pointer, refresh the windows retangle via KGWS and 
// validate the window.
    for (i=0; i<WINDOW_COUNT_MAX; ++i)
    {
        w = (struct gws_window_d *) windowList[i];
        if ((void*) w != NULL)
        {
            if ( w->used == TRUE && w->magic == 1234 )
            {
                if (w->dirty == TRUE)
                {
                    gws_refresh_rectangle ( 
                        w->absolute_x, 
                        w->absolute_y, 
                        w->width, 
                        w->height );

                    // We're done.
                    // We do the other windows in the next round.
                    if (w == __root_window){
                       validate_window(w);
                       return;
                    }
                    validate_window(w);
                }
            }
        }
    };
}

// wmReactToPaintEvents:
// Refresh only the components that was changed by the painter.
// #todo
// Maybe in the future we can react to 
// changes in other components than windows.
void wmReactToPaintEvents(void)
{
    // Refresh only the components that was changed by the painter.
    // It means that we're reacting to all the paint events.
    reactRefreshDirtyWindows();

    // ...
}

// gws_show_window_rect:
// Show the rectangle of a window that was painted in the main backbuffer.
// Copy from backbuffer to frontbuffer.
// Does it need vsync?
// #todo: criar um define chamado refresh_window.
// ??
// Devemos validar essa janela, para que ela 
// não seja redesenhada sem antes ter sido suja?
// E se validarmos alguma janela que não está pronta?
// #test: validando

int gws_show_window_rect(struct gws_window_d *window)
{
    struct gws_window_d *parent;

    //#debug
    //debug_print("gws_show_window_rect:\n");

// Parameter:
    if ((void *) window == NULL){
        goto fail;
    }
    if (window->used != TRUE){
        goto fail;
    }
    if (window->magic != 1234){
        goto fail;
    }

//#shadow 
// ?? E se a janela tiver uma sombra, 
// então precisamos mostrar a sombra também. 
//#bugbug
//Extranhamente essa checagem atraza a pintura da janela.
//Ou talvez o novo tamanho favoreça o refresh rectangle,
//ja que tem rotinas diferentes para larguras diferentes

    //if ( window->shadowUsed == 1 )
    //{
        //window->width = window->width +4;
        //window->height = window->height +4;
        //refresh_rectangle ( window->left, window->top, 
        //    window->width +2, window->height +2 ); 
        //return (int) 0;
    //}

    //p = window->parent;

// We can't show a minimied window.
// We need to restore it first.
    if (window->state == WINDOW_STATE_MINIMIZED)
        goto fail;

// We can't draw if the parent's type is WT_OVERLAPPED and it's minimized.
    parent = (struct gws_window_d *) window->parent;
    if ((void*) parent != NULL)
    {
        if (parent->magic == 1234)
        {
            if (parent->type == WT_OVERLAPPED)
            {
                if (parent->state == WINDOW_STATE_MINIMIZED)
                    goto fail;
            }
        }
    }

// Refresh rectangle
// See: rect.c   
    gws_refresh_rectangle ( 
        window->absolute_x, 
        window->absolute_y, 
        window->width, 
        window->height ); 

    validate_window(window);
    return 0;

fail:
    // #slow
    //debug_print("gws_show_window_rect: fail\n");
    return (int) -1;
}

// Sinaliza que precisamos apagar o ponteiro do mouse,
// copiando o conteudo do backbuffer no LFB.
void DoWeNeedToEraseMousePointer(int value)
{
    if ( value != FALSE && 
         value != TRUE )
    {
        return;
    }
    __clear_mousebox = (int) value;
}

// Find the new mouse_hover window.
// What is the window where the mouse pointer is inside?
// Compare the global variables for mouse pointer 
// against the windows dimensions to find the perfect match.
// mouse_hover is the pointer for the window with the mouse pointer.
void mouse_at(void)
{
// Compare the mouse position (__new_mouse_x and__new_mouse_y)
// against the windows. 

    struct gws_window_d *new_hover_window;
    register int i=0;

// Check if the mouse pointer is already in mouse_hover.
// Compare the mouse position (__new_mouse_x and__new_mouse_y)
// against the mouse_hover window. 

    if ((void*) mouse_hover != NULL)
    {
        if (mouse_hover->magic == 1234)
        {
            if ( __new_mouse_x > mouse_hover->absolute_x &&
                 __new_mouse_x < mouse_hover->absolute_right &&
                 __new_mouse_y > mouse_hover->absolute_y &&
                 __new_mouse_y > mouse_hover->absolute_bottom )
            {
                // Not the root
                if (mouse_hover != __root_window)
                {
                    //mouse_hover = (void *) w;
                    //redraw_window(w,TRUE);
                    return;
                }
            }
        }
    }

// Find the new mouse_hover window.
// Compare the mouse position (__new_mouse_x and__new_mouse_y)
// against all the mouse_hover window. 

// #bugbug
// We gotta check window inside window.

    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        new_hover_window = (void*) windowList[i];
        if ((void*) new_hover_window != NULL)
        {
            if (new_hover_window->magic == 1234)
            {
                if ( __new_mouse_x > new_hover_window->absolute_x &&
                     __new_mouse_x < new_hover_window->absolute_right &&
                     __new_mouse_y > new_hover_window->absolute_y &&
                     __new_mouse_y > new_hover_window->absolute_bottom )
                {
                    // Not the root. So accept this hover window.
                    if (new_hover_window != __root_window)
                    {
                        mouse_hover = (void *) new_hover_window;
                        //redraw_window(w,TRUE);
                        return;
                    }
                }
            }
        }
    };
}

long comp_get_mouse_x_position(void)
{
    return (long) __new_mouse_x;
}

long comp_get_mouse_y_position(void)
{
    return (long) __new_mouse_y;
}

// #todo
// We need to put this routine in another file. Maybe painter.c
// maybe mouse.c
// #ps: 
// This is a low level routine. 
// Associated with the display device driver.
static void direct_draw_mouse_pointer(void)
{
// #todo:
// + Each window has it's own type of mouse pointer.
//   so, we need to check on the mouse_hover window's structure
//   in order to know how to paint the mouse pointer 
//   for that given window.
//   See: window->mpp structure.

// The rectangle
    unsigned long rectLeft   = __new_mouse_x;
    unsigned long rectTop    = __new_mouse_y;
    unsigned long rectWidth  = 8;
    unsigned long rectHeight = 8;

    unsigned int rectColor = COLOR_RED;
    unsigned long rectROP = 0;

    //int UseBMPImage= TRUE;

    struct gws_window_d *w;


// BMP Image
    //if (UseBMPImage == TRUE){
        // #todo
        // Paint the pointer using a BMP Imange.
    //}

//
// Rectangle
//

// Change maouse pointer parameters.
    w = (struct gws_window_d *) get_mousehover();
    if ((void*)w != NULL){
        if (w->magic == 1234){
            rectColor = (unsigned int) w->mpp.bg_color;
        }
    }

// Printing directly into the LFB.
// #ps: 
// This is a low level routine. 
// Associated with the display device driver.

    frontbuffer_draw_rectangle( 
        (unsigned long) rectLeft, (unsigned long) rectTop, 
        (unsigned long) rectWidth, (unsigned long) rectHeight, 
        (unsigned int) rectColor, 
        (unsigned long) rectROP );
}

// + Apaga o cursor antigo, copiando o conteudo do backbuffer
// + Pinta o novo cursor diretamente no lfb.
void __display_mouse_cursor(void)
{
    unsigned long rWidth = 16;
    unsigned long rHeight = 16;

// Display server not initialized yet.
    if ((void*) display_server == NULL)
        return;
    if (display_server->initialized != TRUE)
        return;
// Mouse not initialized yet.
    if (gUseMouse != TRUE)
        return;

// #todo Limits
// Precisa inicializar os valores sobre o mouse.
// Precisa criar uma estrura pra eles.

    if ( __old_mouse_x<0 ){ __old_mouse_x=0; }
    if ( __old_mouse_y<0 ){ __old_mouse_y=0; }

    if ( __new_mouse_x<0 ){ __new_mouse_x=0; }
    if ( __new_mouse_y<0 ){ __new_mouse_y=0; }

//------
// We need to clear the mousebox,
// refreshing the content from backbuffer to LFB.
// We call it when we receive an 'mouse move' event.
    if (__clear_mousebox == TRUE)
    {
        gws_refresh_rectangle(
            __old_mouse_x, __old_mouse_y, rWidth, rHeight );
        DoWeNeedToEraseMousePointer(FALSE);
    }

// save
    __old_mouse_x = __new_mouse_x;
    __old_mouse_y = __new_mouse_y;

// ---------------------------
// Draw the pointer direcly into the LFB.
// Not printing it into the backbuffer.
// It uses the new values.
    direct_draw_mouse_pointer();
//------ 
}

// Flush
// Display the desktop components without using the compositor.
// Called by ServerLoop() in main.c.
// Called by wmCompose() and callback_compose().
void comp_display_desktop_components(void)
{
    static int Dirty = FALSE;

// fps++
    if (WindowManager.initialized == TRUE){
        WindowManager.frame_counter++;
    }

// Backgroud
// If the background is marked as dirty, 
// we flush it, validate it, show the cursor and return.
    Dirty = (int) isdirty();
    if (Dirty == TRUE)
    {
        gws_show_backbuffer();
        // #bugbug
        // #todo: 
        // We miss the cursor here before refreshing the whole screen.
        if (gUseMouse == TRUE){
            __display_mouse_cursor();
        }
        validate();
        return;
    }

// Refresh only the components that was changed by the painter.
    wmReactToPaintEvents();

// Show the mouse cursor in the screen.
    if (gUseMouse == TRUE){
        __display_mouse_cursor();
    }

// Validate the whole screen.
    validate();

// fps
    //__update_fps();
}

// wmCompose:
// Called by the main routine for now.
// Its gonna be called by the timer.
void 
wmCompose(
    unsigned long jiffies, 
    unsigned long clocks_per_second )
{

// It's locked. Return.
    if (Compositor._locked == TRUE)
        return;

// Lock.
// This way this routine can't be called recursively,
// or for a callback routine or signal.
    Compositor._locked = TRUE;

    // Compositor
    // Every window was painted into private offscreen buffers.
    if (Compositor.__enable_composition == TRUE){
        // compose();

    // Flush
    // Every window was painted into the backbuffer.
    } else {
        comp_display_desktop_components();
    };

    Compositor.counter++;

// Unlock
    Compositor._locked = FALSE;
}

/*
// Marca como 'dirty' todas as janelas filhas,
// dai o compositor faz o trabalho de exibir na tela.
void refresh_subwidnows( struct gws_window_d *w );
void refresh_subwidnows( struct gws_window_d *w )
{
}
*/


// Initialize the mouse support.
// global
// #temporary:
// Mouse is using the limit os 0~800.
void comp_initialize_mouse(void)
{
    int hotspotx=0;
    int hotspoty=0;
    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();

    if (w>=0 && w<=800)
        hotspotx = (w >> 1);
    if (h>=0 && h<=800)
        hotspoty = (h >> 1);

// Save it globally.
    __old_mouse_x = hotspotx;
    __old_mouse_y = hotspoty;
    __new_mouse_x = hotspotx;
    __new_mouse_y = hotspoty;

    __mouse_initialized = TRUE; 

// CONFIG:
    //gUseMouse = TRUE;
    gUseMouse = FALSE;
}

// global
// Set new mouse position.
void comp_set_mouse_position(long x, long y)
{
    unsigned long minw = 0;
    unsigned long maxw = gws_get_device_width();
    unsigned long minh = 0;
    unsigned long maxh = gws_get_device_height();

// Lower limit
    if (x < minw){ x=minw; }
    if (y < minh){ y=minh; }

// Upper limit
// #bugbug: Check if it is '>=' instead.
    if (x > maxw){
        x=maxw;
    }
    if (y > maxh){
        y=maxh;
    }

// Save it globally.
    __new_mouse_x = (long) x;
    __new_mouse_y = (long) y;

    // changed = TRUE;
}

//
// $
// INITIALIZATION
//

// + InitializeCompositor structure.
// + Initialize mouse support.
int compInitializeCompositor(void)
{

    Compositor.initialized = FALSE;

    Compositor.counter = 0;
    Compositor._locked = FALSE;

// >> This flag enables composition for the display server.
// In this case the server will compose a final backbbuffer
// using buffers and the zorder for these buffers. In this case 
// each application window will have it's own buffer.
// >> If this flag is not set, all the windows will be painted in the
// directly in the same backbuffer, and the compositor will just
// copy the backbuffer to the LFB.
    Compositor.__enable_composition = FALSE; 

// The structure is initialized.
    Compositor.used = TRUE;
    Compositor.magic = 1234;
    Compositor.initialized = TRUE;

// Initialize the mouser support.
// Not enabled yet.
    comp_initialize_mouse();

// ...

    return 0;
}

