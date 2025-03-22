// ke.c
// ke subsystem.
// Created by Fred Nora.

#include <kernel.h>

// 
// Imports
//

//--------------------------------------
// Ponteiros para ícones
// Ponteiros para o endereço onde os ícones 
// foram carregados.
// queremos saber se o endereço alocado eh compartilhado ...
// para o window server usar ... entao chamaremos de sharedbufferIcon.

// Icon cache structure.
// see: window.h
struct icon_cache_d  icon_cache;


// ============================
static unsigned long presence_level=32;
static unsigned long flush_fps=30;

// ============================

// Endereços dos charmaps usados:
// Obs: Incluídos na estrutura.
//unsigned long normal_charmap_address;
//unsigned long shift_charmap_address;
//unsigned long control_charmap_address;

// ==========================

//
// Import from linker
//

// Não queremos um tamanho de imagem que
// exceda o tamanho da região de memória mapeada para ela.
// No futuro poderemos usar as informações que estão dentro
// do header ELF.
// See: link.ld
static int ImportDataFromLinker = TRUE;

extern unsigned long start_of_kernel_image(void);
extern unsigned long end_of_kernel_image(void);

extern unsigned long kernel_begin(void);
extern unsigned long kernel_end(void);

extern unsigned long code_begin(void);
extern unsigned long code_end(void);

extern unsigned long rodata_begin(void);
extern unsigned long rodata_end(void);

extern unsigned long data_begin(void);
extern unsigned long data_end(void);

extern unsigned long bss_begin(void);
extern unsigned long bss_end(void);

// ==========================

static unsigned long KernelImageSize=0;

static unsigned long KernelImage_Size=0;
static unsigned long KernelImage_CODE_Size=0;
static unsigned long KernelImage_RODATA_Size=0;
static unsigned long KernelImage_DATA_Size=0;
static unsigned long KernelImage_BSS_Size=0;

// ==========================

//-------------
static void __check_refresh_support(void);
static void __print_resolution_info(void);
static void __check_gramado_mode(void);
static void __import_data_from_linker(void);

static unsigned long __last_tick(void);


//-------------------

static unsigned long __last_tick(void)
{
    return (unsigned long) jiffies;
}

//----------------------------------------------

int sys_signal(void)
{
// #todo
// Create the parameters.

// ...

    return (int) -1;
}

// windowLoadGramadoIcons:
// Carrega alguns ícones do sistema.
// It's a part of the window system's initialization.
int windowLoadGramadoIcons(void)
{
    unsigned long fRet=0;

    icon_cache.initialized = FALSE;

    //#debug
    //printk("windowLoadGramadoIcons:\n");

// ## Icon support ##
// iconSupport:
// Carregando alguns ícones básicos usados pelo sistema.
// ## size ##
// Vamos carregar ícones pequenos.
// @todo checar a validade dos ponteiros.
// #bugbug
// Size determinado, mas não sabemos o tamanho dos ícones.
// 4 pages.
// 16 KB ? Is it enough ?
// Sim, os ícones que estamos usam possuem no máximo 2KB.
// See: base/

    unsigned long tmp_size = (4*4096);
    icon_cache.size_in_bytes = (size_t) tmp_size;

// See: window.h
    icon_cache.app      = (void *) allocPages(4);
    icon_cache.file     = (void *) allocPages(4);
    icon_cache.folder   = (void *) allocPages(4);
    icon_cache.terminal = (void *) allocPages(4);
    icon_cache.cursor   = (void *) allocPages(4);
    // ...

    if ( (void *) icon_cache.app == NULL ){
        printk ("windowLoadGramadoIcons: app\n");
        goto fail;
    }
    if ( (void *) icon_cache.file == NULL ){
        printk ("windowLoadGramadoIcons: file\n");
        goto fail;
    }
    if ( (void *) icon_cache.folder == NULL ){
        printk ("windowLoadGramadoIcons: folder\n");
        goto fail;
    }
    if ( (void *) icon_cache.terminal == NULL ){
        printk ("windowLoadGramadoIcons: terminal\n");
        goto fail;
    }
    if ( (void *) icon_cache.cursor == NULL ){
        printk ("windowLoadGramadoIcons: cursor\n");
        goto fail;
    }

//
// Load .BMP images
//

// Loading from the rootdir, '/'.
// #todo:
// Maybe we can load these files from DE/ folder.

// sdDE struture.
    if (sdDE.initialized != TRUE){
        printk("fsLoadProgramFromDE: sdDE.initialized\n");
        goto fail;
    }
    if (sdDE.address == 0){
        printk("fsLoadProgramFromDE: sdDE.address\n");
        goto fail;
    }


// app icon
    fRet = 
        (unsigned long) fsLoadFile ( 
                            VOLUME1_FAT_ADDRESS,
                            sdDE.address, //VOLUME1_ROOTDIR_ADDRESS, 
                            512,  //FAT16_ROOT_ENTRIES,
                            "APP     BMP", 
                            (unsigned long) icon_cache.app,
                            tmp_size );
    if (fRet != 0){
        printk("windowLoadGramadoIcons: APP.BMP\n");
        goto fail;
    }

// file icon
    fRet = 
        (unsigned long) fsLoadFile ( 
                            VOLUME1_FAT_ADDRESS,
                            sdDE.address, //VOLUME1_ROOTDIR_ADDRESS, 
                            512,  //FAT16_ROOT_ENTRIES, 
                            "FILE    BMP", 
                            (unsigned long) icon_cache.file,
                            tmp_size );
    if (fRet != 0){
        printk("windowLoadGramadoIcons: FILE.BMP\n");
        goto fail;
    }

// folder icon
    fRet = 
        (unsigned long) fsLoadFile ( 
                            VOLUME1_FAT_ADDRESS, 
                            sdDE.address, //VOLUME1_ROOTDIR_ADDRESS, 
                            512,  //FAT16_ROOT_ENTRIES,
                            "FOLDER  BMP", 
                            (unsigned long) icon_cache.folder,
                            tmp_size );
    if (fRet != 0){
        printk("windowLoadGramadoIcons: FOLDER.BMP\n");
        goto fail;
    }

// terminal icon
    fRet = 
        (unsigned long) fsLoadFile ( 
                            VOLUME1_FAT_ADDRESS, 
                            sdDE.address, //VOLUME1_ROOTDIR_ADDRESS, 
                            512,  //FAT16_ROOT_ENTRIES, 
                            "TERMINALBMP", 
                            (unsigned long) icon_cache.terminal,
                            tmp_size );
    if (fRet != 0){
        printk("windowLoadGramadoIcons: TERMINAL.BMP\n");
        goto fail;
    }

// cursor icon
    fRet = 
        (unsigned long) fsLoadFile ( 
                            VOLUME1_FAT_ADDRESS, 
                            sdDE.address, //VOLUME1_ROOTDIR_ADDRESS, 
                            512,  //FAT16_ROOT_ENTRIES,
                            "CURSOR  BMP", 
                            (unsigned long) icon_cache.cursor,
                            tmp_size );
    if (fRet != 0){
        printk("windowLoadGramadoIcons: CURSOR.BMP\n");
        goto fail;
    }

// More?
// Podemos checar se eles foram carregados corretamente,
// conferindo apenas a assinatura em cada um deles.
    icon_cache.initialized = TRUE;
    return 0;
fail:
    icon_cache.initialized = FALSE;
    panic ("windowLoadGramadoIcons: Fail\n");
    return -1;
}

// ============================
// Get a shared buffer to a system icon.
// it is gonna be used by the window server.
// It is a pre allocated buffer containg an bmp icon loaded at it.
// Service 9100
// See: window.h

void *ui_get_system_icon(int n)
{
    if (icon_cache.initialized != TRUE){
        return NULL;
    }

    if (n < 1 || n > 5){
        return NULL;
    }

// #bugbug
// This is very unsafe.
// The app can mess up our memory.
// #todo
// Only the window server can access this routine.

    switch (n){
    case 1: return (void *) icon_cache.app;       break;
    case 2: return (void *) icon_cache.file;      break;
    case 3: return (void *) icon_cache.folder;    break;
    case 4: return (void *) icon_cache.terminal;  break;
    case 5: return (void *) icon_cache.cursor;    break;
    // ...
    };

    return NULL;
}

unsigned long get_update_screen_frequency(void)
{
    return (unsigned long) flush_fps;
}

void set_update_screen_frequency(unsigned long fps)
{
    if (fps==0)   { fps=1; };
    if (fps>=1000){ fps=1000; };

// See: sched.h
    flush_fps = (unsigned long) (fps&0xFFFF);
    presence_level = (unsigned long) (1000/flush_fps);
    presence_level = (unsigned long) (presence_level & 0xFFFF);
}

unsigned long get_presence_level(void)
{
    return (unsigned long) presence_level;
}

void set_presence_level(unsigned long value)
{
    if (value==0)  { value=1; }
    if (value>1000){ value=1000; }
    presence_level = value;
}

// Called by task_switch
void schedulerUpdateScreen(void)
{
    register int i=0;
    struct thread_d *TmpThread;

    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    if ( deviceWidth == 0 || deviceHeight == 0 ){
        debug_print ("schedulerUpdateScreen: w h\n");
        panic       ("schedulerUpdateScreen: w h\n");
    }

// Atualizado pelo timer.
    if (UpdateScreenFlag != TRUE){
        return;
    }

    deviceWidth  = (deviceWidth & 0xFFFF);
    deviceHeight = (deviceHeight & 0xFFFF);

// ============================
// Precisamos apenas validar todos retangulos
// porque fizemos refresh da tela toda.
    
    int validate_all= FALSE;

// Flush the whole screen and exit.
// The whole screen is invalidated.
// Validate the screen
    if (screen_is_dirty == TRUE)
    {
        refresh_screen();
        validate_all = TRUE;
        screen_is_dirty = FALSE;
    }

/*
//=========================
// Blue bar:
    unsigned long fps = get_update_screen_frequency();
    char data[32];
    backbuffer_draw_rectangle( 
        0, 0, deviceWidth, 24, COLOR_KERNEL_BACKGROUND, 0 );
    ksprintf(data,"  FPS %d       ",fps);
    data[12]=0;
    keDrawString(0,8,COLOR_YELLOW,data);
    refresh_rectangle ( 0, 0, deviceWidth, 24 );
//=========================
*/

// Flush a list of dirty surfaces.

    for (i=0; i<THREAD_COUNT_MAX; ++i)
    {
        TmpThread = (void *) threadList[i];

        if ( (void *) TmpThread != NULL )
        {
            if ( TmpThread->used == TRUE && 
                 TmpThread->magic == 1234 && 
                 TmpThread->state == READY )
            {
                // #test 
                //debug_print("  ---- Compositor ----  \n");
                
                if ( (void *) TmpThread->surface_rect != NULL )
                {
                    if ( TmpThread->surface_rect->used == TRUE && 
                         TmpThread->surface_rect->magic == 1234 )
                    {
                        // Como fizemos refresh da tela toda,
                        // então precisamos validar todos os retângulos.
                        
                        if (validate_all == TRUE)
                            TmpThread->surface_rect->dirty = FALSE;

                        // dirty rectangle
                        // Se uma surface está suja de tinta.
                        // Precisamos copiar para o framebuffer.

                        if ( TmpThread->surface_rect->dirty == TRUE )
                        {
                            refresh_rectangle ( 
                                TmpThread->surface_rect->left, 
                                TmpThread->surface_rect->top, 
                                TmpThread->surface_rect->width, 
                                TmpThread->surface_rect->height );

                            // Validate the surface. (Rectangle)
                            TmpThread->surface_rect->dirty = FALSE;
                        }

                    }
                }
            }
        }
    };

// Chamamos o 3d demo do kernel.
// See: kgws.c

    if (DemoFlag == TRUE)
    {
        //demo0();
        DemoFlag=FALSE;
    }

// Atualizado pelo timer.
    UpdateScreenFlag = FALSE;
}

// see: bldisp.c
void keRefreshScreen(void)
{
    bldisp_flush(0);
}

void 
keDrawString ( 
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string )
{
    if ((void*) string == NULL)
        return;
    if (*string == 0)
        return;
    draw_string(x, y, color, string);
}

// Worker
static void __check_refresh_support(void)
{
// Check if we can use refresh_screen or not.

// -------------------------------------------

// No refresh screen yet!
// Ainda nao podemos usar o refresh screen porque a
// flag refresh_screen_enabled ainda nao foi acionada.
// Eh o que vamos fazer agora,
// de acordo com o tamanho da memoria disponivel pra isso.

// ++
// ======================================
// Screen size
// #importante
// Essa rotina calcula a memória que precisamos
// e nos diz se podemos usar o refresh screen.
// Isso habilita o uso do console como verbose para log.

    unsigned long bytes_per_pixel = 0;
    unsigned long pitch = 0;

    refresh_screen_enabled = FALSE;
    screen_size_in_kb = 0;

// #bugbug
// Para os outros casos o pitch será '0'.

    /*
    if ( xBootBlock.bpp != 24 &&  
         xBootBlock.bpp != 32 )
    {
        //panic
    }
    */

    if ( xBootBlock.bpp == 24 || 
         xBootBlock.bpp == 32 )
    {
        bytes_per_pixel = (xBootBlock.bpp / 8); 
        pitch = 
            (unsigned long) (xBootBlock.deviceWidth * bytes_per_pixel);
    }  

// pitch fail,
// don't stop, the real machine needs some kind of
// message support.

    if (pitch == 0){
        refresh_screen_enabled = FALSE;
        debug_print("Screen size fail. pitch\n");
    }

// ---------------

// Saving:
// Screen size in kb.
// Remember: For now we only have 2048KB mapped for LFB.
// Quantos KB vamos precisar para uma tela nessa resoluçao?
    screen_size_in_kb = 
        (unsigned long) ( (pitch * xBootBlock.deviceHeight)/1024 );

    // #debug
    //printk ("Screen size: %d KB\n", screen_size_in_kb);
 
// Se a quantidade usada por uma tela nessa resoluçao
// for maior que o que temos disponivel.
// Entao nao podemos habilitar o refresh screen.

    if (screen_size_in_kb >= 2048)
    {
        refresh_screen_enabled = FALSE;
        debug_print("Screen size fail screen_size_in_kb\n");
    }

// ok
// Ok We can use the refresh routine.
// Because we have memory enough for that.

    if (screen_size_in_kb < 2048){
        refresh_screen_enabled = TRUE;  
    }
    
// ======================================
// --

//
// Full console initialization.
//

// We can't live without this.
// But the real machine needs the message support,
// so, we can't stop here. Let's hack for a now.

/*
    if ( refresh_screen_enabled != TRUE )
    {
        debug_print("kernel_main: refresh_screen_enabled != TRUE\n");
        Initialization.is_console_log_initialized = FALSE;
        while(1){asm("hlt");};
        //die(); //we dont have refresh screen support,
    }
*/

// #hack
// #bugbug
// Nesse caso a rotina de refreshscreen vai usar
// um tamanho falso de tela, bem pequeno, que
// cabe na memoria mapeada disponivel.
// Isso sera usado para debug em ambiente
// que tivermos problemas.
// tudo isso se resolvera quando mapearmos memoria
// o suficiente para resoluçoes grandes.

// #todo
// (Screen Extents)
// #todo: 
// We can use the concept of screen extents in this case. 
// It's similar to virtual screens. :)

    if (refresh_screen_enabled != TRUE)
    {
        // Enough for 320x200x32bpp
        fake_screen_size_in_kb = ( (320*4*200)/1024 );
        g_use_fake_screen_size = TRUE;
        refresh_screen_enabled = TRUE;
    }
// -------------------------------------------
}

// Worker
static void __print_resolution_info(void)
{
// Print device info.
    printk ("Width:%d Height:%d BPP:%d\n",
        xBootBlock.deviceWidth,
        xBootBlock.deviceHeight,
        xBootBlock.bpp );
// ---------------
// Is it supported?
// #temp
// Supported widths. 800, 640, 320.
    if ( xBootBlock.deviceWidth != 800 &&
         xBootBlock.deviceWidth != 640 &&
         xBootBlock.deviceWidth != 320 )
    {
        panic("__print_resolution_info: Unsupported resolution\n");
    }
}

// Worker
// Called by booting_begin().
static void __check_gramado_mode(void)
{
//-----------
// gramado mode.

// Show gramado mode.
    printk("gramado mode: %d\n",current_mode);

    // #debug
    //refresh_screen();
    //while(1){}

    switch (current_mode){
// #temp
// Supported gramado mode.
    case GRAMADO_JAIL:
    case GRAMADO_P1:
    case GRAMADO_HOME:
        // OK
        break;
// #temp
// Unsupported gramado mode. (yet)
    case GRAMADO_P2:
    case GRAMADO_CASTLE:
    case GRAMADO_CALIFORNIA:

        // #bugbug: 
        // panic and x_panic are not working at this point.
        debug_print("__check_gramado_mode: Unsupported gramado mode\n");
        panic("__check_gramado_mode: Unsupported gramado mode\n");
        break;
// Undefined gramado mode.
    default:
        // #bugbug: 
        // panic and x_panic are not working at this point.
        debug_print("__check_gramado_mode: Undefined gramado mode\n");
        panic("__check_gramado_mode: Undefined gramado mode\n");
        break;
    };

    // Breakpoint
    //refresh_screen();
    //while(1){}
}

// ================================

static void __import_data_from_linker(void)
{
// #todo
// Isso deve ter uma flag no aquivo de configuração.
// config.h i guess.

    if (ImportDataFromLinker == TRUE)
    {
        //printk("\n");
 
        //KernelImageSize = (start_of_kernel_image - end_of_kernel_image);
        //printk ("Image Size %d KB \n",KernelImageSize/1024);

        //-------------

        // Não queremos um tamanho de imagem que
        // exceda o tamanho da região de memória mapeada para ela.

        KernelImage_Size = (kernel_end - kernel_begin);
        //printk ("Image Size %d KB \n",KernelImage_Size/1024);

        // .text
        KernelImage_CODE_Size = (code_end - code_begin);
        //printk ("CODE Size %d KB \n",KernelImage_CODE_Size/1024);

        // .rodata
        KernelImage_RODATA_Size = (rodata_end - rodata_begin);
        //printk ("RODATA Size %d KB \n",KernelImage_RODATA_Size/1024);

        // .data
        KernelImage_DATA_Size = (data_end - data_begin);
        //printk ("DATA Size %d KB \n",KernelImage_DATA_Size/1024);

        // .bss
        KernelImage_BSS_Size = (bss_end - bss_begin);
        //printk ("BSS Size %d KB \n",KernelImage_BSS_Size/1024);

        // Limit 1 MB
        // The kernel image is too long.
        if ( KernelImage_Size/1024 > 1024 )
        {
            panic("Error 0x04: Image size\n");
        }
        
        // Address limit for the kernel image.
        // See: x64gva.h
        if (kernel_end > KERNEL_HEAP_START)
        {
            panic("Error 0x04: kernel_end\n");
        }

        // #debug: breakpoint
        //refresh_screen();
        //while(1){}
    }
}

unsigned long keGetSystemMetrics(int index)
{
    if (index <= 0){
        return (unsigned long) 0;
    }
    return (unsigned long) doGetSystemMetrics(index);
}

void keDie(void)
{
    die();
}

void keSoftDie(void)
{
    soft_die();
}

int keIsQemu(void)
{
    return (int) isQEMU();
}

int keCloseInitProcess(void)
{

    if ((void*) InitThread == NULL){
        goto fail;
    }
    if (InitThread->magic != 1234){
        goto fail;
    }

    // #debug
    printk("#test: Sending CLOSE to init.bin\n");
    refresh_screen();

// Send
    ipc_post_message_to_tid(
        (tid_t) 0,                //sender tid. #todo
        (tid_t) InitThread->tid,  //receiver tid.
        (int) MSG_CLOSE,          //msg code
        0,
        0 );

    return 0;
fail:
    return (int) -1;
}

void keReboot(void)
{
// Call a safe implementation of this routine.
    do_reboot(0);
}

void ke_x64ExecuteInitialProcess(void)
{

// See: x64init.c
    I_x64ExecuteInitialProcess();
    panic("keInitialize: phase 3\n");
    //goto InitializeEnd;
}

// Called by I_initKernelComponents().
int keInitializeIntake(void)
{
    //int Status = FALSE;

    //PROGRESS("keInitializeIntake:\n");

// Init dispatcher
    init_dispatch();

// Init scheduler.
// See: sched/sched.c
    init_scheduler(0);

// Init taskswitch
    init_ts();

// Init processes and threads, 
// See: process.c and thread.c
    init_processes();
    init_threads();    

// #todo: Init IPC and Semaphore.
    //ipc_init();
    //create_semaphore(); 

// queue
// #deprecated ?

    queue = NULL;

// #debug 
// A primeira mensagem só aparece após a inicialização da runtime
// por isso não deu pra limpar a tela antes.

#ifdef BREAKPOINT_TARGET_AFTER_MK
    printk ("#breakpoint: after keInitializeIntake");
    die();
#endif

    return TRUE;
}

// --------------------------------
// Initialize ke phase 0.
// + kernel font.
// + background.
// + refresh support.
// + show banner and resolution info.
// + Check gramado mode and grab data from linker.
// + Initialize bootloader display device.
// --------------------------------
// Initialize ke phase 1.
// + Calling I_x64main to 
//   initialize a lot of stuff from the 
//   current architecture.
// + PS2 early initialization.
// --------------------------------
// Initialize ke phase 2.
// + Initialize background.
// + Load BMP icons.
//
// OUT: TRUE or FALSE.
int keInitialize(int phase)
{
// Called by I_kmain() in kmain.c.

    int Status=FALSE;

    // Phase 0: 
    // Architecture independent.
    // >> Debug and display support.
    if (phase == 0){

        //PROGRESS("keInitialize: phase 0\n");
        // kernel font.
        zero_initialize_default_kernel_font();
        // Initializing background for the very first time.
        zero_initialize_background();
        // Setup refresh/flush support.
        // Flush data into the lfb.
        __check_refresh_support();
        // Now we have console debug
        Initialization.is_console_log_initialized = TRUE;

        // Show banner!
        // See: zero.c
        zero_show_banner();

        // Print resolution info
        __print_resolution_info();
        // Check gramado mode
        __check_gramado_mode();
        // Import data from linker.
        __import_data_from_linker();

        // Device driver for bootloader display device.
        // Initialize the structure for the bootloader display device.
        // see: bldisp.c
        DDINIT_bldisp();

        goto InitializeEnd;

    // Phase 1: 
    // It depends on the architecture. 
    // >> Process and thread support.
    } else if (phase == 1) {

        // Starting with some architecture independent stuff.

        // Threads counter
        UPProcessorBlock.threads_counter = 0;

        // Architecture dependent stuff.

        //PROGRESS("keInitialize: phase 1\n");
        // Initialize the current architecture.
        // Change name to I_arch_initialize();
        // See: ke/x86_64/x64init.c
        Status = (int) I_x64_initialize();
        if (Status != TRUE){
            goto fail;
        }

        goto InitializeEnd;

    // Phase 2: 
    // Architecture independent. 
    // >> Device drivers and 3d graphics support.
    } else if (phase == 2){

        // ================================
        // Early ps/2 initialization.
        // Initializing ps/2 controller.
        // #todo: 
        // Essa inicialização deve ser adiada.
        // deixando para o processo init fazer isso.
        // Chamaremos essa inicialização básica nesse momento.
        // A inicialização completa será chamada pelo processo init.
        // See: i8042.c
        // ================
        // Early initialization
        // Only the keyboard.
        // It is working
        // ================
        // This is the full initialization.
        // #bugbug This is a test yet.
        // It fails in the real machine.

        //PROGRESS(":: Early PS2 initialization\n"); 
        DDINIT_ps2_early_initialization();
        //DDINIT_ps2(); // (full initialization)

        //================================
        // Initialize all the kernel graphics support.
        // Initialize all the kernel graphics support.
        // some extra things like virtual terminal and tty.
        // #todo: rever essa inicializaçao.
        // See: graphics.c
        // ================================
        // Initialize window server manager.
        // ws.c
        // #debug:  
        // Esperamos alcaçarmos esse alvo.
        // Isso funcionou gigabyte/intel
        // Vamos avançar
        // Quem chamou essa funçao foi o começo da inicializaçao do kernel.
        // Retornamos para x86main.c para arch x86.
        // See: drivers/ws.c
        // Initialize ws callback support.
        // see: callback.c

        //PROGRESS(":: kgws, ws, ws callback\n"); 
        // Graphics infrastruture.
        // see: gramk/gre/gre.c
        gre_initialize();
        // Desktop stuff.
        // Ring0 components for the display server.
        // see: user.c
        gramkInitialize();
        // ws registration support.
        // (display server)
        ws_init();
        // ws callback support.
        initialize_ws_callback_info();

        // Final message before jumping to init process.
        //PROGRESS("keInitialize: phase 2\n");
        //printk("keInitialize:  phase 2\n");
        //#debug
        //refresh_screen();
        //while(1){}
        // Clear the screen again.
        zero_initialize_background();
        // Loading .BMP icon images.
        //PROGRESS("::(2)(?) Icons\n"); 
        windowLoadGramadoIcons();

        // ==========================
        // Network support.
        // ?? At this moment we already initialized the e1000 driver.
        // See: network.c
        networkInit();

        goto InitializeEnd;
    } else {
        // Wrong phase number.
        // goto fail;
    };

InitializeEnd:
    return TRUE;
fail:
    debug_print("keInitialize: fail\n");
    return FALSE;
}

