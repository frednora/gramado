// gws.c
// Part of the display server.
// 2019 - Created by Fred Nora.

#include "../gwsint.h"

// Flag for the whole screen.
static int __dirty = FALSE;

// #todo: Explain this structure.
struct gws_graphics_d *Currentgraphics;

// #todo: Explain this structure.
struct engine_d  Engine;

// #todo: Explain this structure.
// See: display.h
struct gws_display_d  *CurrentDisplay;

static int __gwssrv_init_globals(void);
// ==============================================

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
void xxxThread(void)
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
 *     #bugbug ...ja funcionou uma vez, mas agora esta com problemas.
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
// OBS: 
// ISSO JA FUNCIONOU. 
// ESTAMOS SUSPENDENDO PORQUE PRECISAMOS AUMENTAR O 
// TAMANHO DO HEAP USADO PELO PROCESSO PARA 
// ALOCAÇÂO DINAMICA, ELE NAO TA DANDO CONTA 
// DE TODA A DEMANDA POR MEMORIA.

// >>Dessa vez pegaremos o retorno, 
// que deve ser o ponteiro para a estrutura da thread.
// Obs: Nao podemos usar a estrutura porque ela esta 
// em ring0.
// >>Chamaremos a system_call que executa essa thread 
// que temos o ponteiro da estrutura.

    void *ThreadTest1;

// #bugbug: 
// Nao temos mais espaço no heap do preocesso 
// para alocar memoria pois gastamos o heap com 
// a imagem bmp. (isso aconteceu kkk).

    unsigned long *threadstack1;

    //++
    server_enter_critical_section();

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

    ThreadTest1  = 
        (void *) gwssrv_create_thread ( 
                     (unsigned long) &xxxThread, 
                     (unsigned long) (&threadstack1[0] + (2*1024) - 4), 
                     "ThreadTest1" );

    if ( (void *) ThreadTest1 == NULL ){
        printf ("____test_threads: apiCreateThread fail \n");
        printf ("____test_threads: ThreadTest1");
        exit(1);
    }

// # executando #
// #importante:
// La no kernel, isso deve selecionar a thread para 
// execussao colocando ela no estado standby.
// Logo em seguida a rotinad e taskswitch efetua o spawn.

    gwssrv_start_thread (ThreadTest1);
    server_exit_critical_section ();
    //--

    printf ("____test_threads: Tentando executar um thread [ok]..\n");
    //permitir que o shell continue.
}

/*
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
    //server_debug_print ("gwssrv_create_thread:\n");
    return (void *) gramado_system_call ( 
                        72,  //SYSTEMCALL_CREATETHREAD, 
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
    //server_debug_print ("service_drain_input: [TODO]\n");
    //handle_ipc_message();
    return -1;
}

// refresh the whole screen.
// #todo: move to view/
void gws_show_backbuffer(void)
{
    
    // #debug
    // debug_print("gws_show_backbuffer:\n");

    // #debug
    // [Deprecated] Using the kernel service.
    // gwssrv_show_backbuffer();

    // #todo:
    // Create this flag. 
    // if (!paint_ready) return;

    if ((void*) gui == NULL)
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
    // #debug
    // debug_print("gws_show_backbuffer: Calling gws_show_window_rect\n");

    if ((void *) gui->screen_window == NULL)
    {
        debug_print("gws_show_backbuffer: [PANIC] gui->screen_window\n");
        printf     ("gws_show_backbuffer: [PANIC] gui->screen_window\n");
        exit(1);
        //return;
    }

// See: comp.c
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

// #todo
// actually we gotta trust in the graphics device library
// This library knows everything about the device.
// Maybe we can fill the device context structure
// getting values from the library.
// see: libgd.c
static int __gwssrv_init_globals(void)
{
    register int i=0;

// The gate is open.
    Compositor._locked = FALSE;

// Initializing the graphics device library.
// see: libgd.c
    libgd_initialize();

// Gramado mode
// get gramado mode.
// jail, p1, home, p2, castle ...
// Check validation and panic if fail.
 
    os_mode = server_get_system_metrics(130);
    if (os_mode < 0){
        printf ("__gwssrv_init_globals: os_mode\n");
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
    __device_width  = (unsigned long) server_get_system_metrics(1);
    __device_height = (unsigned long) server_get_system_metrics(2);
    __device_bpp    = (unsigned long) server_get_system_metrics(9);


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
        printf("gwssrv_init_globals: [FAIL] Screen properties\n");
        exit(1);
    }

// ==============================

//
// == buffers ======================================
//

// Let's setup the screens[i].
// 0=frontbuffer and 1=backbuffer.
// Clear the list of pointers.
// Setup pointers.

    for (i=0; i<MAX_SCREENS; ++i)
    { 
        screens[i].initialized = FALSE;
        screens[i].base_address = 0; 

        // The screen belongs to an offscreen window.
        screens[i].OffscreeWindow.initialized = FALSE;
        screens[i].OffscreeWindow.pid = -1;
        screens[i].OffscreeWindow.wid = -1;
        screens[i].OffscreeWindow._dirty = FALSE;
    };

// The frontbuffer.
    screens[SCREEN_FRONTBUFFER].base_address = 
        (unsigned long) ____FRONTBUFFER_VA;

// The backbuffer.
// This is where the desktop scene will be composed
// using all the offscreen canvas that belongs to the clients.
// We can register the pointer for the 
// application's canvas into the screens[] array fo structure.
    screens[SCREEN_BACKBUFFER].base_address = 
        (unsigned long) ____BACKBUFFER_VA;

    if ( screens[SCREEN_FRONTBUFFER].base_address == 0 || 
         screens[SCREEN_BACKBUFFER].base_address == 0 )
    {
        printf ("gwssrv_init_globals: screens\n");
        exit(1);
    }

// Flags for refresh
    refresh_device_screen_flag = FALSE;    
    refresh_valid_screen_flag = FALSE;

// Initialize the default color scheme.
// See: wm.c
    int cs_status=-1;
    cs_status = (int) gwssrv_initialize_default_color_scheme();
    if (cs_status){
        printf("gwssrv_init_globals: Color scheme\n");
        exit(1);
    }

    //...

    return 0;
}

// gwsInitGUI:
// Initialize the display server's GUI. 
// Called by initGUI() in main.c
int gwsInitGUI(void)
{
// Let's create:
//  + The current display.
//  + The current screen.
//  + The global device context.
//  + The root window.
//  And register all these structure in 'gui'.
// #??
// When we created the 'gui' structure?

    debug_print("gwsInitGUI:\n");

    //paint_ready = FALSE;

// Initializing global variables.
    __gwssrv_init_globals();

// #todo
// Configurar as estruturas em ordem:
// Current display, current screen, current root window.

// -----------------------------------------------
// display

//
// == Display ===============================================
//

    CurrentDisplay = (void *) malloc (sizeof(struct gws_display_d));
    if ((void*) CurrentDisplay == NULL){
        debug_print("gwsInitGUI: [FAIL] CurrentDisplay\n");
        printf     ("gwsInitGUI: [FAIL] CurrentDisplay\n");
        exit(1); 
    }
    memset( CurrentDisplay, 0, sizeof(struct gws_display_d) );

    CurrentDisplay->used = TRUE;
    CurrentDisplay->magic = 1234;
    CurrentDisplay->id = 0;
    
    // Socket fd for connections.
    CurrentDisplay->fd = 0;
    // Device driver fd.
    // For resolution configurations.
    CurrentDisplay->device_fd = 0;

    //...

// ===================================================
// Screen

//
// == Screen ===============================================
//

    DeviceScreen  = (void *) malloc (sizeof(struct gws_screen_d));
    if ((void*) DeviceScreen == NULL){
        debug_print("gwsInitGUI: [FAIL] DeviceScreen\n");
        printf     ("gwsInitGUI: [FAIL] DeviceScreen\n");
        exit(1);
    }
    memset( DeviceScreen, 0, sizeof(struct gws_screen_d) );

    DeviceScreen->id = 0;
    DeviceScreen->flags = 0;

// #test
// Configuramos algumas variaveis globais quando
// chamamos a rotina de inicializaçao de globais.
// See: gwssrv_init_globals().

    DeviceScreen->width  = (unsigned long) SavedX;
    DeviceScreen->height = (unsigned long) SavedY;
    DeviceScreen->bpp    = (unsigned long) SavedBPP;

    DeviceScreen->pitch =
        (unsigned long) ( SavedX * SavedBPP );

// Checks

    if (DeviceScreen->pitch == 0){
        debug_print("gwsInitGUI: [FAIL] DeviceScreen->pitch \n");
        printf     ("gwsInitGUI: [FAIL] DeviceScreen->pitch \n");
        exit(1);
    }

// #todo: 
// Cuidado, não queremos divisão por zero.

    // #bugbug
    // We need two values, w and h.
    DeviceScreen->font_size = 0;    //todo

// #test
// We're gonna set this values right below in this routine.
    DeviceScreen->char_width = 0;    //todo
    DeviceScreen->char_height = 0;    //todo

// #?
// We simply used server_get_system_metrics() to get these addresses.
// See: gwssrv_init_globals()
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

    if ( (void *) CurrentDisplay != NULL ){
        CurrentDisplay->device_screen = DeviceScreen;
        CurrentDisplay->valid_screen  = DeviceScreen;
    }

// Validation
    DeviceScreen->used = TRUE;
    DeviceScreen->magic = 1234;

// -----------------------------------------------
// Device context
// Default dc.
// This is the default device context structure.

// dc object

    gr_dc = (struct dc_d *) malloc( sizeof(struct dc_d) );
    if ((void*) gr_dc == NULL){
        printf("gwsInitGUI: Couldn't create default dc\n");
        exit(1);
    }
    memset ( gr_dc, 0, sizeof(struct dc_d) );

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

    // virtual window
    gr_dc->absolute_x = 0;
    gr_dc->absolute_y = 0;
    gr_dc->width  = gr_dc->device_width;
    gr_dc->height = gr_dc->device_height;

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
    register int pi=0;
    for (pi=0; pi<16; pi++){
        gr_dc->palette[pi] = 0;
    }

    gr_dc->next = NULL;
    
    gr_dc->initialized = TRUE;
// -------------------

// -----------------------------------------------
// Initialize font support.
// see: font.c
    font_initialize();

// -----------------------------------------------
// Initialize char support.
// see: char.c
    char_initialize();

    if (CharInitialization.initialized == TRUE)
    {
        DeviceScreen->char_width = 
            (unsigned long) (CharInitialization.width & 0xFFFF);
        DeviceScreen->char_height = 
            (unsigned long) (CharInitialization.height & 0xFFFF);
    }

// -----------------------------------------------
// Initialize window support.
// Initialize some basic variables and the window list.
    window_initialize();

// -----------------------------------------------
// gui structure
// First level structure for the GUI.

    gui = (void *) malloc( sizeof(struct gui_d) );
    if ((void *) gui == NULL){
        debug_print("gwsInitGUI: gui\n");
        printf     ("gwsInitGUI: gui\n");
        exit(1);
    }
    memset( gui, 0, sizeof(struct gui_d) );

//
// == Root window ===============================================
//

// See:
// wm.c

    // Pointer, id and color. 
    struct gws_window_d  *rw;
    wid_t rw_wid = -1;
    unsigned int rw_color = (unsigned int) get_color(csiDesktop);

    rw = (struct gws_window_d *) wmCreateRootWindow(rw_color);
    if ((void*) rw == NULL){
        debug_print("gwsInitGUI: rw\n");
        printf     ("gwsInitGUI: rw\n");
        exit(1);
    }
    if ( rw->used != TRUE || rw->magic != 1234 ){
        debug_print("gwsInitGUI: rw validation\n");
        printf     ("gwsInitGUI: rw validation\n");
        exit(1);
    }

// Register
    rw_wid = RegisterWindow(rw);
    if (rw_wid<0 || rw_wid >= WINDOW_COUNT_MAX){
        debug_print("gwsInitGUI: Couldn't register rw\n");
        printf     ("gwsInitGUI: Couldn't register rw\n");
        exit(1);
    }

// ==============

// Let's start our z-order list
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

    if ((void*) gui == NULL){
        debug_print("gwsInitGUI: gui\n");
        printf     ("gwsInitGUI: gui\n");
        exit(1);
    }

// ======================================
// Let's register what we created in this routine.

    if ((void *) gui != NULL)
    {
        gui->_display      = (struct gws_display_d *) CurrentDisplay;
        gui->_screen       = (struct gws_screen_d *)  DeviceScreen;
        gui->screen_window = (struct gws_window_d *)  rw;
        gui->main_window   = (struct gws_window_d *)  rw;
    }

// Display a string in the background.
    if ((void*) gui->screen_window != NULL){
        dtextDrawText ( 
            (struct gws_window_d *) gui->screen_window,
            8, 8, COLOR_RED, "gwsInitGUI: gui ok" );
    }

// #debug
// Let's see the messages above.
// Show the backbuffer into the screen.

    gwssrv_show_backbuffer();

    return 0;
}

// Invalidate the whole frame.
void invalidate(void)
{
    __dirty = TRUE;
}

// Validate the whole frame.
void validate(void)
{
    __dirty = FALSE;
}

// Check the frame validation
int isdirty(void)
{
    return (int) __dirty;
}

// Invalidate all the background.
void invalidate_background(void)
{
    invalidate();
}

void validate_background(void)
{
    validate();
}

// Check the background validation.
int is_background_dirty(void)
{
    return (int) isdirty();
}

void gwssrv_set_keyboard_focus(int window)
{
    if (window<0){
        return;
    }
    set_focus_by_id(window);
}

// Refresh the device screen
void refresh_screen(void)
{
    refresh_device_screen();
}

// Refresh the device screen
void refresh_device_screen(void)
{
    //debug_print ("refresh_device_screen:\n");

    gws_show_backbuffer();
    refresh_device_screen_flag = FALSE; // Invalidate.
}


// Refresh the valid screen of the current display.
// #todo: move to view/
void refresh_valid_screen(void)
{
    //debug_print ("refresh_valid_screen:\n");

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

void gwssrv_broadcast_close(void)
{
// Post message to all the overlapped windows.
// see: window.c
    window_post_message_broadcast( 
        0,           // wid = Ignored
        GWS_Close,   // msg = msg code
        1234,        // long1 = signature 1 for close message
        5678 );      // long2 = signature 2 for close message
}

//
// End
//

