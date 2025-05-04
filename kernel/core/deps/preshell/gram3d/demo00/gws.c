/*
 * File: gws.c
 *     Part of the Gramado Window Server.
 * History:
 *     2019 - Created by Fred Nora.
 */

// Window server routines.
#include "gram3d.h"

int dirty=0;
int background=0;

struct gws_graphics_d *Currentgraphics;
struct engine_d  Engine;

static int __init_globals(void);


// =======================================


/*
// #test
// from: x-window-system
int
gws_WriteToClient ( 
    struct gws_client_d *who, 
    int count, 
    char *buf );
int
gws_WriteToClient ( 
    struct gws_client_d *who, 
    int count, 
    char *buf )
{
    return -1;
}
*/


// xxxThread:
// Um thread dentro para testes.
void xxxThread (void)
{
    printf("$\n");
    printf("#### This is a thread ####\n");
    printf("$\n");
    gws_show_backbuffer();
    while (1){
        printf("$"); fflush(stdout);
        //asm ( "pause" );
    };
}


/*
 * ____test_threads:
 *     Cria um thread e executa.
 *     #bugbug ...j� funcionou uma vez, mas agora est� com problemas.
 *     @todo: na hora de criar a thread precisamos passar o PID desse processo.
 */

void ____test_threads (void)
{
    void *T;	

// Obs: 
// As threads criadas aqui s�o atribu�das ao processo PID=0.
// @todo: 
// No kernel, quando criar uma thread ela deve ser atribu�da
// ao processo que chamou a rotina de cria��o.	

	printf ("____test_threads: Creating threads..\n");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread1");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread2");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread3");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread4");
	//...
	
	//
	// # Criar e executar #
	//
	
	// Tentando executar um thread.
	// *******************************
    // OBS: 
	// ISSO J� FUNCIONOU. 
	// ESTAMOS SUSPENDENDO PORQUE PRECISAMOS AUMENTAR O 
	// TAMANHO DO HEAP USADO PELO PROCESSO PARA 
	// ALOCA��O DIN�MICA, ELE N�O T� DANDO CONTA 
    // DE TODA A DEMANDA POR MEM�RIA.		  
	
	//>>Dessa vez pegaremos o retorno, 
	// que deve ser o ponteiro para a estrutura da thread.
	// Obs: N�o podemos usar a estrutura porque ela est� 
	// em ring0.
	//>>Chamaremos a system_call que executa essa thread 
	// que temos o ponteiro da estrutura.
    
	void *ThreadTest1;	
	
	//#bugbug: 
	// N�o temos mais espa�o no heap do preocesso 
	// para alocar mem�ria pois gastamos o heap com 
	// a imagem bmp. (isso aconteceu kkk).

	unsigned long *threadstack1;
	

    //++
    gwssrv_enter_critical_section();
	
	// #importante:
	// Como a torina de thread � bem pequena e o 
	// alocador tem pouqu�ssimo heap, vamos alocar o m�nimo.
	// Isso � apenas um teste, vamos var se a thread funciona 
	// com um a pilha bem pequena. 2KB.
	
	threadstack1 = (unsigned long *) malloc (2*1024);
	
	//Ajuste para o in�cio da pilha.
	//threadstack1 = ( threadstack1 + (2*1024) - 4 ); 
	
	//
	// # Criando a thread #
	//
	
//creating:

    printf ("____test_threads: Tentando executar uma thread..\n");	

    //ThreadTest1  = (void *) gde_create_thread ( (unsigned long) &xxxThread, 
    //                            (unsigned long) (&threadstack1[0] + (2*1024) - 4), 
    //                            "ThreadTest1" );

    ThreadTest1  = (void *) gwssrv_create_thread ( 
                                (unsigned long) &xxxThread, 
                                (unsigned long) (&threadstack1[0] + (2*1024) - 4), 
                                "ThreadTest1" );

    if ( (void *) ThreadTest1 == NULL )
    {
        printf ("____test_threads: apiCreateThread fail \n");
        printf ("____test_threads: ThreadTest1");
        exit(1);
    }

	// # executando #
	
	// #importante:
	// L� no kernel, isso deve selecionar a thread para 
	// execuss�o colocando ela no estado standby.
	// Logo em seguida a rotinad e taskswitch efetua o spawn.

    gwssrv_start_thread (ThreadTest1);
    gwssrv_exit_critical_section ();
    //--


	printf ("____test_threads: Tentando executar um thread [ok]..\n");
	
	//permitir que o shell continue.
}


    
/*
 **************************
 * gwssrv_create_thread:
 *     Create a thread.
 *     #todo: 
 *     Precisamos uma função que envie mais argumentos.
 *     Essa será uma rotina de baixo nível para pthreads.
 */

void *gwssrv_create_thread ( 
    unsigned long init_eip, 
    unsigned long init_stack, 
    char *name )
{
    //#define	SYSTEMCALL_CREATETHREAD     72
    gwssrv_debug_print ("gwssrv_create_thread:\n");
    return (void *) gramado_system_call ( 72, //SYSTEMCALL_CREATETHREAD, 
                        init_eip, 
                        init_stack, 
                        (unsigned long) name );
}


/*
 * gwssrv_start_thread:
 *     Coloca no estado standby para executar pela primeira vez
 */

void gwssrv_start_thread (void *thread)
{

    //#define	SYSTEMCALL_STARTTHREAD  94 
    gramado_system_call ( 94, //SYSTEMCALL_STARTTHREAD, 
        (unsigned long) thread, 
        (unsigned long) thread, 
        (unsigned long) thread );
}


int service_drain_input (void)
{
    gwssrv_debug_print ("service_drain_input: [TODO]\n");
    //handle_ipc_message();
    return -1;
}


// #todo
// Sleep if there is no message in the queue.
void gwssrv_wait_message(void)
{
    // #todo
    // block()
    
    // #using this one for now.
    gwssrv_yield();
}


// refresh the whole screen.
// #todo: move to view/
void gws_show_backbuffer(void)
{
    debug_print("gws_show_backbuffer:\n");

    // #debug
    // [Deprecated] Using the kernel service.
    // gwssrv_show_backbuffer();

    // #todo:
    // Create this flag. 
    // if (!paint_ready) return;

    if ( (void*) gui == NULL )
    {
        debug_print("gws_show_backbuffer: [PANIC] gui fail\n");
        printf     ("gws_show_backbuffer: [PANIC] gui fail\n");
        exit(1);
        //return;
    }

//
// Show window rect.
//

// See: wm.c
    debug_print("gws_show_backbuffer: Calling gws_show_window_rect\n");

    if ( (void *) gui->screen_window == NULL )
    {
        debug_print("gws_show_backbuffer: [PANIC] gui->screen_window\n");
        printf     ("gws_show_backbuffer: [PANIC] gui->screen_window\n");
        exit(1);
        //return;
    }

    gws_show_window_rect(gui->screen_window);

    //debug_print("gws_show_backbuffer: done\n");
}


unsigned long gws_get_device_width(void)
{
    return (unsigned long) __device_width;
}


unsigned long gws_get_device_height(void)
{
    return (unsigned long) __device_height;
}


// __init_globals:
// #todo
// actually we gotta trust in the graphics device library
// This library knows everything about the device.
// Maybe we can fill the device context structure
// getting values from the library.
// see: libgd.c
static int __init_globals(void)
{
    register int i=0;

// Initializing the graphics device library.
// see: libgd.c
    libgd_initialize();

// Gramado mode
// get gramado mode.
// jail, p1, home, p2, castle ...
// Check validation and panic if fail.
 
    current_mode = (int) gwssrv_get_system_metrics(130);
    if (current_mode < 0){
        printf ("__init_globals: current_mode\n");
        exit(1);
    }

// framebuffer and backbuffer.
// buffers
// We need to find a better way to get these addresses,
// maybe a library. (direct framebuffer library thing)
// I guess it uses a shared memory allocator.That is why we can
// use these addresses.
// #test:
// We have the rtl included in this project,
// let's give rtl a chance for now.
// #todo
// Is this call using the sc82 syscall ? or the sc80 ?
// We need full access to the kernel structure.

    ____FRONTBUFFER_VA = (unsigned long) rtl_get_system_metrics(11);
    ____BACKBUFFER_VA  = (unsigned long) rtl_get_system_metrics(12);

// Screen
    __device_width  = (unsigned long) gwssrv_get_system_metrics(1);
    __device_height = (unsigned long) gwssrv_get_system_metrics(2);
    __device_bpp    = (unsigned long) gwssrv_get_system_metrics(9);

// ==============================
// Saving

    SavedX   = (unsigned long) __device_width;
    SavedY   = (unsigned long) __device_height;
    SavedBPP = (unsigned long) __device_bpp;
    SavedLFB = (unsigned long) ____FRONTBUFFER_VA;
        
    if ( SavedX == 0 || 
         SavedY == 0 || 
         SavedBPP == 0 || 
         SavedLFB == 0 )
    {
        printf("__init_globals: Screen properties\n");
        exit(1);
    }
// ==============================

//
// == buffers ======================================
//

// Clear the list of pointers.

    for (i=0; i<MAX_SCREENS; ++i){ 
        screens[i] = 0; 
    };

// Setup pointers:

    screens[SCREEN_FRONTBUFFER] = (unsigned long) ____FRONTBUFFER_VA;
    screens[SCREEN_BACKBUFFER]  = (unsigned long) ____BACKBUFFER_VA;

    if ( screens[SCREEN_FRONTBUFFER] == 0 || 
         screens[SCREEN_BACKBUFFER] == 0 )
    {
        printf ("__init_globals: [FAIL] screens\n");
        exit(1);
    }

// Flags for refresh
    refresh_device_screen_flag = FALSE;    
    refresh_valid_screen_flag = FALSE;

// Color scheme
// Color scheme: Humility
    gwssrv_initialize_color_schemes(ColorSchemeHumility);
    gwssrv_select_color_scheme(ColorSchemeHumility);

    //...

    return 0;
}


/*
 * gwsInit:
 *     Initialize the server. 
 */

// Called by initGraphics() in main.c

int gwsInit(void)
{
    debug_print("gwsInit:\n");

    //paint_ready = FALSE;

// Initializing globals.
    __init_globals();

// #todo
// Configurar as estruturas em ordem:
// Current display, current screen, current root window.

//
// == Display ===============================================
//

    CurrentDisplay = (void *) malloc (sizeof(struct gws_display_d));

    if ( (void*) CurrentDisplay == NULL )
    {
        debug_print("gwsInit: [FAIL] CurrentDisplay\n");
        printf     ("gwsInit: [FAIL] CurrentDisplay\n");
        exit(1); 
    }

    memset( CurrentDisplay, 0, sizeof(struct gws_display_d) );

    CurrentDisplay->id = 0;
    CurrentDisplay->fd = 0;
    CurrentDisplay->used  = TRUE;
    CurrentDisplay->magic = 1234;
    //...
// ===================================================


//
// == Screen ===============================================
//

    DeviceScreen  = (void *) malloc (sizeof(struct gws_screen_d));

    if ( (void*) DeviceScreen == NULL )
    {
        debug_print("gwsInit: [FAIL] DeviceScreen\n");
        printf     ("gwsInit: [FAIL] DeviceScreen\n");
        exit(1);
    }

    memset( DeviceScreen, 0, sizeof(struct gws_screen_d) );

    DeviceScreen->id = 0; 
    DeviceScreen->flags = 0;

// #test
// Configuramos algumas variaveis globais quando
// chamamos a rotina de inicializaçao de globais.
// See: __init_globals().

    DeviceScreen->width  = (unsigned long) SavedX;
    DeviceScreen->height = (unsigned long) SavedY;
    DeviceScreen->bpp    = (unsigned long) SavedBPP;

    DeviceScreen->pitch  =  (unsigned long) ( SavedX * SavedBPP );

// Checks

    if( DeviceScreen->pitch == 0 )
    {
        debug_print("gwsInit: [FAIL] DeviceScreen->pitch \n");
        printf     ("gwsInit: [FAIL] DeviceScreen->pitch \n");
        exit(1);
    }


// #todo: 
// Cuidado, não queremos divisão por zero.

    DeviceScreen->font_size   = 0;    //todo
    DeviceScreen->char_width  = 0;    //todo
    DeviceScreen->char_height = 0;    //todo

// #?
// We simply used gwssrv_get_system_metrics() to get these addresses.
// See: __init_globals()
// We need to find a better way to get these addresses,
 // maybe a library. (direct framebuffer library thing)

    DeviceScreen->backbuffer  = (void *) ____BACKBUFFER_VA;
    DeviceScreen->frontbuffer = (void *) ____FRONTBUFFER_VA;


// Hotspot
// Center of the screen

    DeviceScreen->hotspot_x = ( DeviceScreen->width  >> 1 );
    DeviceScreen->hotspot_y = ( DeviceScreen->height >> 1 );


// The new limits 
// when the origin is the center of the screen.
    DeviceScreen->min_x = 0;
    DeviceScreen->min_y = 0;
    DeviceScreen->max_x = (unsigned long) ( DeviceScreen->width  >> 1 );
    DeviceScreen->max_y = (unsigned long) ( DeviceScreen->height >> 1 );

    //...

// The device screen will be the valid screen for now.
// Save the device screen in the diplay structure.

    if ( (void *) CurrentDisplay != NULL )
    {
        CurrentDisplay->device_screen = DeviceScreen;
        CurrentDisplay->valid_screen  = DeviceScreen;
    }

// Validation
    DeviceScreen->used  = TRUE;
    DeviceScreen->magic = 1234;

// =================================================

// -------------------

//
// Default dc.
//

// This is the default device context structure.

    // dc object
    gr_dc = 
        (struct dc_d *) malloc ( sizeof( struct dc_d ) );

    if ( (void*) gr_dc == NULL ){
        printf ("gwsInit: Couldn't create default dc\n");
        exit(1);
    }

    gr_dc->used = TRUE;
    gr_dc->magic = 1234;
    gr_dc->initialized = FALSE;
    
// display and screen.
    gr_dc->display = (struct gws_display_d *) CurrentDisplay;

    //buffers
    gr_dc->data = NULL;
    gr_dc->depth_buf = NULL;

    gr_dc->flags = 0;
    
    // hardware info: w,h,bpp
    gr_dc->device_width = DeviceScreen->width;
    gr_dc->device_height = DeviceScreen->height;
    gr_dc->bpp = DeviceScreen->bpp;


//
// Orthographic projection
//

// virtual window.
    gr_dc->absolute_x   = 0;
    gr_dc->absolute_y    = 0;
    gr_dc->width  = (unsigned long) (gr_dc->device_width & 0xFFFFFFFF);
    gr_dc->height = (unsigned long) (gr_dc->device_height & 0xFFFFFFFF);

    gr_dc->right  = gr_dc->absolute_x + gr_dc->width;
    gr_dc->bottom = gr_dc->absolute_y + gr_dc->height;

// #test
// Usado na estrutura de projeção.
    gr_dc->znear  = (unsigned long) 0;
    gr_dc->zfar   = (unsigned long) (gr_dc->height / 2);
    gr_dc->zrange = (unsigned long) gr_dc->zfar - gr_dc->znear;

// Hotspot
// The center of the orthographic projection plane.
    gr_dc->hotspot_x = DeviceScreen->hotspot_x;
    gr_dc->hotspot_y = DeviceScreen->hotspot_y;

// extents
// The new limits 
// when the origin is the center of the screen.

    gr_dc->min_x = DeviceScreen->min_x;
    gr_dc->min_y = DeviceScreen->min_y;
    gr_dc->max_x = DeviceScreen->max_x;
    gr_dc->max_y = DeviceScreen->max_y;

    // clear the pallete stuff
    // #todo: put some colors here.
    int pi=0;
    for(pi=0; pi<16; pi++){
        gr_dc->palette[pi] = 0;
    }

    gr_dc->next = NULL;
    
    gr_dc->initialized = TRUE;
// -------------------


    // font support.
    gwssrv_init_font();

    // char support
    gwssrv_init_char();

    // windows
    // Inicializamos algumas variáves ...
    // as primeira janelas são criadas logo abaixo.
    gwssrv_init_windows();    


//
// == gui structure ============================================
//

// First level structure for the GUI.
    gui = (void *) malloc( sizeof( struct gui_d) );
    if ((void *) gui == NULL)
    {
        debug_print("gwsInit: gui\n");
        printf     ("gwsInit: gui\n");
        exit(1);
    }
    memset( gui, 0, sizeof(struct gui_d) );

//
// == Root window ===============================================
//

    struct gws_window_d  *tmpRootWindow;
    tmpRootWindow = (struct gws_window_d *) wmCreateRootWindow();
    if ((void*) tmpRootWindow == NULL)
    {
        debug_print("gwsInit: tmpRootWindow\n");
        printf     ("gwsInit: tmpRootWindow\n");
        exit(1);
    }
    if ( tmpRootWindow->used != TRUE || tmpRootWindow->magic != 1234 )
    {
        debug_print("gwsInit: tmpRootWindow validation\n");
        printf     ("gwsInit: tmpRootWindow validation\n");
        exit(1);
    }

// Register
    int root_wid = RegisterWindow(tmpRootWindow);
    if (root_wid<0)
    {
        debug_print ("gwsInit: Couldn't register rootwindow\n");
        printf      ("gwsInit: Couldn't register rootwindow\n");
        exit(1);
    }

//
// Let's start our z-order list;
//

    last_window = NULL;


    //#debug
    //debug_print(" $ \n");
    //asm("int $3");


// ==============

// #bugbug
// Its is not a screen object. It is only a window object.
// It is the main window of the gui structure.
// The 'screen' window is the device screen and the
// main window is the desktop window.
// Where we created this object.
// See: gui.h

// gui structure.

    if ( (void*) gui == NULL )
    {
        debug_print ("gwsInit: gui\n");
        printf      ("gwsInit: gui\n");
        exit(1);
    }

    if ( (void *) gui != NULL )
    {
        gui->_display      = (struct gws_display_d *) CurrentDisplay;
        gui->_screen       = (struct gws_screen_d *)  DeviceScreen;
        gui->screen_window = (struct gws_window_d *)  tmpRootWindow;
        gui->main_window   = (struct gws_window_d *)  tmpRootWindow;
    }

// #todo
    if ( (void*) gui->screen_window != NULL )
    {
        dtextDrawText ( 
            (struct gws_window_d *) gui->screen_window,
            8, 8, COLOR_RED, "gwsInit: Graphics ok" );

        //dtextDrawText ( 
            //(struct gws_window_d *) gui->screen_window,
            //8, 8, COLOR_RED, "Graphics ok" );
    }

// The kernel side will flush the surface rectangle 
// into the framebuffer.

    // invalidate_surface_retangle();

// #debug
// Let's see the messages above.

    gwssrv_show_backbuffer();
    //while(1){}

    //debug_print("gwsInit: done\n");

    return 0;
}


// invalidate the frame.
void invalidate(void)
{
    dirty = TRUE;
}

// validate the frame.
void validate(void)
{
    dirty = FALSE;
}

// Check the frame validation
int isdirty(void)
{
    return (int) dirty;
}

// invalidate all the background.
void invalidate_background(void)
{
    // #bugbug: Use background_dirty ??
    background = TRUE;
}

void validate_background(void)
{
    background = FALSE;
}

// Check the background validation.
int is_background_dirty(void)
{
    return (int) background;
}


void gwssrv_set_keyboard_focus(int window)
{
    if (window<0)
        return;
    set_focus_by_id(window);
}


// Refresh the device screen
// #todo: move to view/
void refresh_screen(void)
{
    refresh_device_screen();
}

// Refresh the device screen
// #todo: move to view/
void refresh_device_screen(void)
{
    debug_print ("refresh_device_screen:\n");

    gws_show_backbuffer();
    refresh_device_screen_flag = FALSE; // Invalidate.
}


// Refresh the valid screen of the current display.
// #todo: move to view/
void refresh_valid_screen(void)
{

    debug_print ("refresh_valid_screen:\n");

    if ( (void*) CurrentDisplay == NULL ){
        printf("refresh_valid_screen: [ERROR] CurrentDisplay\n");
        exit (1);
    }

//
// The valid SCREEN
//

    // Se a valid screen não existe.
    if ( (void*) CurrentDisplay->valid_screen == NULL ){
        debug_print ("refresh_valid_screen: [FAIL] No valid screen\n");
        return;
    }

    // A valid screen é justamente a screen do device.
    if ( CurrentDisplay->valid_screen == CurrentDisplay->device_screen )
    {
        debug_print ("refresh_valid_screen: show the device screen\n");
        gws_show_backbuffer();
    }

    refresh_valid_screen_flag = FALSE; // Invalidate.
}


/*
// Copiamos o lfb no backbuffer.
void 
copy_backbuffer ( 
    void *lfb,             // lfb
    void *target_buffer,   // target buffer
    int ofs,               // offset to walk
    int count);             // how much
void 
copy_backbuffer ( 
    void *lfb,             // lfb
    void *target_buffer,   // target buffer
    int ofs,               // offset to walk
    int count)             // how much
{

    void *front_buffer  = (void *) lfb;
    void *buffer        = (void *) target_buffer;

    if (front_buffer == NULL)
        return;
 
    if (buffer == NULL)
        return;

    memcpy ( 
        buffer       + ofs, 
        front_buffer + ofs, 
        count );
}
*/






/*
 * serverInit
 * 
 */

// Is it used?

int serverInit (void)
{
    int Status = -1;
    
    printf ("serverInit: Initializing gws server ...\n");

    Status = (int) gwsInit();

    if (Status<0){
        printf ("serverInit: fail\n");
    }
    
    return (int) Status;
}



//
// End.
//



