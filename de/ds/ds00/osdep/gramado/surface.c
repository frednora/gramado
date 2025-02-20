// surface.c
// Surfaces support.
// That shared area used to draw things.
// Created by Fred Nora.

#include "../../gwsint.h"

struct gws_surface_d  *rootSurface;

// ===========================================

// Start surface
void demoClearSurface(unsigned int color)
{
    // #todo
    // We can do this for more resolutions. 

    unsigned int fail_color = COLOR_RED;

    if ((void*) gr_dc != NULL)
    {
        if (gr_dc->magic == 1234)
        {
            if (gr_dc->initialized == TRUE)
            {
                rectBackbufferDrawRectangle ( 
                    gr_dc->absolute_x, 
                    gr_dc->absolute_y, 
                    gr_dc->width, 
                    gr_dc->height,
                    color, 1, 0 );

                return;
            }
        }
    }

// Limpa em mostra na resolução 320x200
    rectBackbufferDrawRectangle ( 
           0, 0, 320, 200, 
           fail_color, 1, 0 );
}

// #todo
// We can do this for more resolutions. 
// mostra na resolução 320x200
void demoFlushSurface(void)
{
// Flush the surface.
    
// default dc

    if ((void*) gr_dc != NULL)
    {
        if (gr_dc->magic == 1234)
        {
            if (gr_dc->initialized == TRUE)
            {
                gws_refresh_rectangle(
                    gr_dc->absolute_x, 
                    gr_dc->absolute_y, 
                    gr_dc->width, 
                    gr_dc->height );

                return;
            }
        }
    }

    gws_refresh_rectangle(
        0, 0, 320, 200 );
}

/*
 * xxxCreateSurfaceWindow: 
 *     Create a surface in the kernel.
 *     --------------
 *     Cria uma janela com base em uma struct.
 *     Retorna o endereço da estrutura da janela criada. 
 *     Para que possa ser registrada na lista windowList[].
 *     Essa lista tambem fica em kgws no kernel.
 * Cria a janela dependendo do tipo:  
 * =================================
 * 1 - POP-UP.
 * 2 - EDIT-BOX.
 * 3 - NORMAL ( bg + client area ).
 * 4 - NORMAL_EDIT ( bg + editbox).
 * 5 - MENUBAR.
 * 6 - TOOLBAR.
 * 7 - STATUSBAR.
 * 8 - TASKBAR.
 * 9 - ? 
 * CONTINUA ...
 */
// Create a surface in ring0.
// #todo review that thing.
void *xxxCreateSurfaceWindow( 
    unsigned long type,        // 1, Tipo de janela (popup,normal,...)
    unsigned long status,      // 2, Estado da janela (ativa ou nao)
    unsigned long view,        // 3, (min, max ...)
    char *windowname,          // 4, Título. 
    unsigned long x,           // 5, Deslocamento em relação às margens do Desktop. 
    unsigned long y,           // 6, Deslocamento em relação às margens do Desktop.
    unsigned long width,       // 7, Largura da janela.
    unsigned long height,      // 8, Altura da janela.
    void *pSurfaceWindow,      // 9, Endereço da estrutura da janela mãe.
    unsigned long desktop_id,  //10, Desktop id.
    unsigned long clientcolor, //11, Cor da área de cliente
    unsigned long color )      //12, Color (bg) (para janela simples).
{ 

// opaque
// A ring0 structure pointer.
    void *wObjectPointer;

// Enviando tudo via argumento.
// Esse método dá a possibilidade de enviarmos ainda 
// mais argumentos. 
// #importante: Isso está funcionado, Vamos fazer assim e 
// não do jeito antigo.
    unsigned long message_buffer[12];

    //server_debug_print ("xxxCreateSurfaceWindow: \n");

	message_buffer[0]  = (unsigned long) type;
	message_buffer[1]  = (unsigned long) status;
	message_buffer[2]  = (unsigned long) view;
	message_buffer[3]  = (unsigned long) windowname;
	message_buffer[4]  = (unsigned long) x;
	message_buffer[5]  = (unsigned long) y;
	message_buffer[6]  = (unsigned long) width;
	message_buffer[7]  = (unsigned long) height;
	message_buffer[8]  = (unsigned long) pSurfaceWindow;
	message_buffer[9]  = (unsigned long) desktop_id;
	message_buffer[10] = (unsigned long) clientcolor;
	message_buffer[11] = (unsigned long) color;

// #bugbug
// Maybe we need to use sc82()

    wObjectPointer = 
        (void *) gramado_system_call ( 
                     118, 
                     (unsigned long) &message_buffer[0], 
                     (unsigned long) &message_buffer[0], 
                     (unsigned long) &message_buffer[0] );

    if ( (void *) wObjectPointer == NULL ){
        //server_debug_print ("xxxCreateSurfaceWindow: [FAIL] wObjectPointer\n");
        return NULL;  
    }

// #teste
// Vamos enviar a mensagem MSG_CREATE para o procedimento de janela.
// Assim ele pode terminar de pintar nessa mesma janela.

    return (void *) wObjectPointer;    
}

// not tested yet
struct gws_surface_d *xxxCreateSurface( 
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    unsigned long color )
{
    struct gws_surface_d *surface;
    void *surfaceWindowObject;

// surface

    surface = 
        (struct gws_surface_d *) malloc ( sizeof( struct gws_surface_d ) );

    if ((void*) surface == NULL){
        //todo: message
        return NULL;
    }
    surface->initialized = FALSE;

// window

    surfaceWindowObject = 
        (void *) xxxCreateSurfaceWindow ( 
                     WT_SIMPLE, 1, 1, "Surface",
                     left, top, width, height, 
                     0, 0, COLOR_BLACK, COLOR_BLACK );  

    if ((void*) surfaceWindowObject == NULL){
        //todo: message
        return NULL;
    }

// structure

    surface->window_object = (void *) surfaceWindowObject;

    surface->left   = left;
    surface->top    = top;
    surface->width  = width;
    surface->height = height;

    surface->dirty = FALSE;
    surface->locked = FALSE;
    
    surface->next = NULL;
    surface->initialized = TRUE;

    surface->used = TRUE;
    surface->magic = 1234;

    return (struct gws_surface_d *) surface;
}

