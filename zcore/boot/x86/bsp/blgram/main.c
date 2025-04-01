// main.c
// This is the Gramado bootloader. (BLGRAM.BIN)
// The main goal of this program is loading the Gramado kernel 
// into the memory and pass the command to it.
// This is a 32bit program.
// Jumps to a 64bit kernel.
// Created by Fred Nora.

/*
 * + It loads the kernel image.
 * + ... initializes the paging.
 * + passes the command to the kernel.
 * The boot loader was loaded in the address 0x00020000, and
 * has the entry point in the address 0x00021000.
 * The kernel image will be loaded in the address 0x00100000
 * and  the entry point will be in the address 0x00101000.
 * The virtual addresses are 0xC0000000 for the base and
 * 0xC0001000 for the entry point.
 */

#include "bl.h"  


// This is for debug via verbose on baremetal.
//#define BAREMETAL_VERBOSE    1


// ----------------------------
// GRAMADO/
// + (1000) Headless server.
// + (1001) Headless embedded systems.
// ...

#define GRAMADO_HEADLESS_SERVER           1000
#define GRAMADO_HEADLESS_EMBEDDED_SYSTEM  1001
// ...

// ----------------------------
// DE/
// + (2000) Servers with GUI experience.
// + (2001) Workstations.
// + (2002) Desktop PCs.
// + (2003) Embedded systems with GUI experience.
// ...

#define DE_SERVER       2000
#define DE_WORKSTATION  2001
#define DE_DESKTOP      2002
#define DE_EMBEDDED     2003
// ...

static unsigned int system_type = 0;


//
// GRAMADO/ (Normal system)
//

// Load this one
const char *image_pathname = "/GRAMADO/KERNEL.BIN";
// or try this one. (BACKUP)
const char *image_default_pathname = "/GRAMADO/KRNL.BIN";

//
// DE/ (Desktop Environment)
//

// Load this one
const char *image_pathname_de = "/DE/KERNEL.BIN";
// or try this one. (BACKUP)
const char *image_default_pathname_de = "/DE/KRNL.BIN";

//
// globals
//

struct display_device_d  DisplayDevice;

int ____testing_memory_size_flag=0;
unsigned long __last_valid_address=0;

// main flags.
int gdefLegacyBIOSBoot=0;
int gdefEFIBoot=0;
int gdefSafeBoot=0;
int gdefShowLogo=0;
int gdefShowProgressBar=0;
//...

// See: gdef.h
struct boot_block_d  BootBlock;

int current_mode=0;

int g_initialized=0;
int g_driver_hdd_initialized=0;


unsigned long g_cursor_x=0;        //Cursor
unsigned long g_cursor_y=0;        //Cursor
unsigned long g_system_color=0;    //Color 
unsigned long g_char_attrib=0;     //Color  
unsigned long CURSOR_X=0;
unsigned long CURSOR_Y=0;
unsigned long STATUS_X=0;
unsigned long STATUS_Y=0;
unsigned long EDITBOX_X=0;
unsigned long EDITBOX_Y=0;
unsigned long EDITBOX_LARGURA=0;
unsigned long EDITBOX_ALTURA=0;
unsigned long EDITBOX_BG_COLOR=0;
unsigned long EDITBOX_TEXT_COLOR=0;

// cr3 support
unsigned long LegacyCR3=0;        //Valor existente.
unsigned long BootLoaderCR3=0;    //Valor configurado pelo Boot Loader.

int useGUI=0;

unsigned long g_proc_status=0;
unsigned long g_next_app=0;
unsigned long g_next_proc=0;
unsigned long g_nova_mensagem=0;
unsigned long next_address=0;    //??


unsigned long g_lbf_pa=0;    //Endereço Físico do LFB.
unsigned long g_lbf_va=0;    //Endereço Lógico do LFB.

// See: bootloader.h
system_t system_info;
VideoBlock_t VideoBlock;    //@todo: Usar um ponteiro.
vesa_t vesa_mode;
procedure_t procedure_info;

// menu
// see: bootloader.h
int menu_highlight=0;
struct menuitem_d MENU[8];

// see: bli386.h
int bliTesting=0;

//static char *codename = "Gramado Boot";
//char kernel_file_name[] = "kernel.bin";
//static char **argv = { NULL, NULL, NULL };
//static char **envp = { NULL, NULL, NULL };

//
// == Private functions: Prototypes =======================
//





static void __no_x86_64_was_found(void);


// Memory
static unsigned long bl_test_memory_size(int mb);
static unsigned long bl_setup_physical_memory(void);
static void 
bl_clean_memory( 
    unsigned long start_address, 
    unsigned long end_address );

// Kernel image
static int bl_load_kernel_image(void);
static int bl_load_kernel_image_for_de(void);

// Menu
static void bl_show_menu(void);
static void bl_menu_loop(void);


// =========================================

// bl_abort:
// Rotina para abortar o bootloader em caso de erro grave.
void bl_abort()
{
//@todo: 
//Talvez poderia ter uma interface antes de chamar a rotina abort().
//ex:
    //checks()
    abort(); 
}

// bl_die:
// CLI HLT routine.
// No return!
// See: bootloader.h
void bl_die(void)
{
    printf("BLGRAM.BIN: [DIE] System Halted\n");
    refresh_screen();
    while (1){
        asm ("cli");
        asm ("hlt");
    };
}

// panic:
// Message support for fatal error.
void panic(const char *msg)
{
    if ((void*) msg != NULL)
    {
        if (*msg != 0){
            printf("BL.BIN: [PANIC] %s\n", msg );
        }
    }
    bl_die();
}

static void __no_x86_64_was_found(void)
{
// We probe the info and detected that the x86)64 instructions
// are not supported.
// We can hang, or load some 32bit funny stuff.

    printf("__no_x86_64_was_found: Sorry!\n");
    printf("It seems that the processor\n");
    printf("does not support x86_64 instructions.\n");

// #todo
// Maybe, in this case, we can load a 32bit dialog,
// asking to shutdown the system.

    printf("The boot loader will not execute the kernel.\n");

done:
    //refresh_screen();
    bl_die();
    while (1){
        asm ("cli");
        asm ("hlt");
    };
}


//
// $
// MEMORY
//

//================================================================
// begin - Testing memory size
//================================================================

static unsigned long bl_test_memory_size(int mb)
{
    unsigned char *BASE = (unsigned char *) 0;
    int offset=0; 
    register int i=0;
// Salvando os valores durante o test.
    unsigned char ____value1 = 0;
    unsigned char ____value2 = 0;
//
// Flag
//
    // Acionando flag.
    ____testing_memory_size_flag = 1;

//#debug
/*
    printf ("=========================================\n");
    printf ("bl_test_memory_size: Looking for %d MB base...\n", mb);
    refresh_screen();
*/

// Começamos em 1MB porque o primeiro mega contem coisa do bios.
    for (i=1; i< (mb+1); i++)
    {
        //printf ("i=%d \n", i);
        //refresh_screen();

        offset = 0x00100000 * i;
        
        //printf ("coloca \n");
        //refresh_screen();

         // #bugbug
         // cuidado com ponteiro nulo.

        //coloca.
        BASE[offset +0] = 0xAA;  //1
        BASE[offset +1] = 0x55;  //2
        
        //printf ("retira \n");
        //refresh_screen();

        //retira dois chars.
        ____value1 = BASE[offset +0];
        ____value2 = BASE[offset +1];
        
        // Se retiramos os mesmos bytes que colocamos.
        if (____value1 == 0xAA && ____value2 == 0x55)
        {
            // Salvamos o ultimo endereco de memoria valido.
            __last_valid_address = (unsigned long) &BASE[offset];
        
            // continua sondando.

        // Se nao conseguimos retirar os mesmos bytes que colocamos
        // e nao tivemos uma excecao.
        }else{

            ____testing_memory_size_flag = 0;
            
            printf("__testing_memory_size: Out of range with no exception!\n");
            printf("__testing_memory_size: Last valid address = %x\n", 
                __last_valid_address);
            refresh_screen();

            ____testing_memory_size_flag = 0;
            return (unsigned long) __last_valid_address;
            
            /*
            while (1){
                asm ("cli");
                asm ("hlt");
            }
            */
        }
    };

     ____testing_memory_size_flag = 0;
            
// Ok, temos um endereço de memoria
// tambem sera salvo em uma variavel global 
// para o caso de panic.

    return (unsigned long) __last_valid_address;
}

//================================================================
// end - Testing memory size
//================================================================

// OUT: Return last valid address.
static unsigned long bl_setup_physical_memory(void)
{
//++
// == Memory size ===============================
// #test
// Sondando quanta memoria fisica tem instalada.
// Faremos isso antes de mapearmos qualquer coisa e
// antes de carregarmos qualquer arquivo.
// #todo:
// Temos varias questoes a se considerar, como o fato de sujarmos
// a IVT no inicio da memoria.

    unsigned long __address = 0;

    // ok
    //__address = (unsigned long) bl_test_memory_size (4);
    // ok
    //__address = (unsigned long) bl_test_memory_size (64);
    // ok
    //__address = (unsigned long) bl_test_memory_size (127);  
    // 511
    //__address = (unsigned long) bl_test_memory_size (1024);   

    //para testar na maquina real com 2048 mb instalado.
    __address = (unsigned long) bl_test_memory_size(2048);   
    
// #todo
// Temos que passar esses valores para o kernel,
// juntamente com os valores que passsamos durante a inicialização
// em assembly, através do boot buffer em 0x0000000000090000. 
// See: head.s
// LFB_VA:      0x00090000 + 0
// WIDTH:       0x00090000 + 8
// HEIGHT:      0x00090000 + 16
// BPP:         0x00090000 + 24
// LAST_VALID:  0x00090000 + 32  <<<--------
// LAST_VALID:  0x00090000 + 40 (complement)
// ...

    unsigned long *LastValid = 
        (unsigned long*) (0x00090000 + 32); 
    
    unsigned long *LastValidComplement = 
        (unsigned long*) (0x00090000 + 36); 

    LastValid[0]           = (unsigned long) __address;
    LastValidComplement[0] = (unsigned long) 0;

//#debug
    //printf ("BlMain: Las valid PA = %x \n", __address);
    //refresh_screen();
    //while(1){}
// =======================================
//--

// Return last valid address.
    return (unsigned long) __address;
}

// #bugbug
// Are we ereasing something important?
// Cleaning the RAM.
// Vamos limpar somente a parte que será usada pelos 
// primeiros componentes do sistema.
// Mas mesmo assim precisamos limpar o bss das imagens
// e limpar as áreas alocadas.
// 32 mb, começando de 1mb.
// Is it slow?
static void 
bl_clean_memory( 
    unsigned long start_address, 
    unsigned long end_address )
{
// Actually we're cleaning only 32 MB starting at the 1MB mark;

    register int i=0;
    unsigned long *CLEARBASE = (unsigned char *) start_address;
    int max_in_mb = 32;
    int max = ( (1024 * 1024 * max_in_mb ) / 4 );  // (32mb/4)

// Limpamos somente se está dentro da área
// previamente descoberta.
// 4 bytes per time.
    for ( i=0; i<max; i++ )
    {
        if ( &CLEARBASE[i] < end_address )
        {
            CLEARBASE[i] = (unsigned long) 0;
        }
    };
}


//
// $
// LOAD KERNEL IMAGE
//

/*
 * bl_load_kernel_image_for_de: 
 *     It loads the kernel image at 0x00100000.
 *     The entry point is at 0x00101000.
 */ 
// #todo
// This way can chose the filename from a
// configuration file.
// This routine will try to load the default filename
// if the provide name fail.
// This routine will build the pathname
// to search in the default folder.
static int bl_load_kernel_image_for_de(void)
{
// Called by bl_main().

    int Status = -1;

// #bugbug
// Precisamos que essa rotina retorne
// para termos a change de inicializarmos o
// rescue shell. Mas acontece que por enquanto
// essa função aborta ao primeiro sinal de perigo.
// See: loader.c

    Status = (int) elfLoadKernelImage(
                       image_pathname_de, 
                       image_default_pathname_de );
    if (Status < 0){
        printf ("bl_load_kernel_image_for_de: elfLoadKernelImage fail\n");
        goto fail;
    }

    // OK
    return (int) Status;

fail:
    refresh_screen();
    return (int) (-1);
}

/*
 * bl_load_kernel_image: 
 *     It loads the kernel image at 0x00100000.
 *     The entry point is at 0x00101000.
 */ 
// #todo
// This way can chose the filename from a
// configuration file.
// This routine will try to load the default filename
// if the provide name fail.
// This routine will build the pathname
// to search in the default folder.
static int bl_load_kernel_image(void)
{
// Called by bl_main().

    int Status = -1;

// #bugbug
// Precisamos que essa rotina retorne
// para termos a change de inicializarmos o
// rescue shell. Mas acontece que por enquanto
// essa função aborta ao primeiro sinal de perigo.
// See: loader.c

    Status = (int) elfLoadKernelImage(image_pathname,image_default_pathname);
    if (Status < 0){
        printf ("bl_load_kernel_image: elfLoadKernelImage fail\n");
        goto fail;
    }

    // OK
    return (int) Status;

fail:
    refresh_screen();
    return (int) (-1);
}

//
// $
// MENU - (Emergency menu)
//

// Show emergency menu
static void bl_show_menu(void)
{
    register int i=0;

// Cursor
    g_cursor_x = 0;
    g_cursor_y = 0;

// Clear the screen
    clear_backbuffer();

// Header
    printf ("\n");
    printf ("Gramado Boot Loader: (Emergency menu)\n");
    printf ("#todo: No imput support yet\n");
    printf ("\n");
    printf ("\n");

// Print the menu itens.
    for (i=0; i<8; i++)
    {
        printf ("\n");

        if (MENU[i].used == TRUE)
        {
            if (i == menu_highlight){
                printf("* %s \n", MENU[i].string);
            }else{
                printf("  %s \n", MENU[i].string);
            };
        }
    };

// Flush the backbuffer into the video memory.
    refresh_screen(); 
}

static void bl_menu_loop(void)
{
    char Key=0;

    g_cursor_x=0;
    g_cursor_y=0;
    clear_backbuffer();

    printf("bl_menu_loop: Initializing\n");
    refresh_screen();

// prepara o menu.   
    MENU[0].used = 1;
    MENU[1].used = 1;
    MENU[2].used = 0;
    MENU[3].used = 0;
    MENU[4].used = 0;
    MENU[5].used = 0;
    MENU[6].used = 0;
    MENU[7].used = 0;

    menu_highlight = 1;
        
    sprintf( MENU[0].string, "menu item 0");
    sprintf( MENU[1].string, "menu item 1");    

    //mostra o menu.
    //bl_show_menu(); 

    while (1){
        bl_show_menu();
        Key = keyboard_wait_key();
        switch (Key)
        {
            case 'x': 
                goto ____go; 
                break;

            case '\n':
                //goto do_execute_item;
                break;    
                
            // UP
            case 'e':
                //blMenuUP();
                break;
            
            // DOWN
            case 'd':
                //blMenuDOWN();
                break;    

            default:
                // Nothing
                break;
        };
    };

____go:
    return;
}

//
// $
// MAIN
//

// --------------------------------
// bl_main:
// Called by StartLoader in head.s.
// This is the entrypoint for the C part of the boot loader.
// + Load the kernel image.
// + Setup paging and the base pagetables used by the kernel.
// + Jumps to the kernel image.
void bl_main(void)
{
    int Status = (-1);
    int fTest=FALSE;

// Do we need to initialize the DE/.
    initialize_de = FALSE;
    //initialize_de = TRUE;

// root and fat not loaded yet.
    g_fat16_root_status = FALSE;
    g_fat16_fat_status = FALSE;
// main flags.
    gdefLegacyBIOSBoot  = FALSE;
    gdefEFIBoot         = FALSE;
    gdefSafeBoot        = FALSE;
    gdefShowLogo        = FALSE;
    gdefShowProgressBar = FALSE;
    // ...
// Set GUI.
    // VideoBlock.useGui = 1;
    // VideoBlock.useGui = 0;

// Display device
// Saving some display device info.

    DisplayDevice.width  = (unsigned long) SavedX;
    DisplayDevice.height = (unsigned long) SavedY;
    DisplayDevice.bpp    = (unsigned long) SavedBPP;

// Line in bytes.
    DisplayDevice.pitch = 
        (unsigned long) (SavedBPP * SavedX);
// Screen in bytes.
    DisplayDevice.screen_size_in_bytes = 
        (unsigned long) (SavedY * DisplayDevice.pitch);

    DisplayDevice.initialized = TRUE;

// -------------------------
// Initialize
// In this routine we're gonna fill the bootblock
// with all the data we have to the kernel.
// See: init.c
    Status = (int) init();
    if (Status != 0){
        // #todo
    }

    /*
    // #debug
    // #test
    // Always 0x80 for only one disk.
    printf("Disk number: %x\n",BootBlock.disk_number);
    refresh_screen();
    while(1){}
    */


// -- #bugbug -----------------------------------------------------

// Setup physical memory.
// Get last valid address.
    unsigned long first_address = 0x100000;  // 1MB mark
    unsigned long last_address=0;
    last_address = (unsigned long) bl_setup_physical_memory();

// Clean up memory.
// IN: start address, end address.
// #bugbug
// Are we ereasing something important?
// Actually we're cleaning only 32 MB starting at the 1MB mark;

#if defined(BAREMETAL_VERBOSE)
    printf ("bl: Clear memory\n");
    refresh_screen();
#endif  

    bl_clean_memory(
        (unsigned long) first_address,
        (unsigned long) last_address );
// -------------------------------------------------------

#if defined(BAREMETAL_VERBOSE)
    printf ("bl: Init heap\n");
    refresh_screen();
#endif  

// Initialize bl heap.
// malloc support.
    init_heap();

#if defined(BAREMETAL_VERBOSE)
    printf ("bl: Init hdd\n");
    refresh_screen();
#endif  

// Initialize ATA support.
// ATA/PATA is the main term here.
// IDE is an alias for ATA
// SATA is the evolution of ATA.
    ata_initialize();

// debug
    if (g_initialized != TRUE){
        printf("bl_main: g_initialized\n");
        bl_die();
    }


// -------------------------------------------
// Loading root dir.
#if defined(BAREMETAL_VERBOSE)
    printf ("bl: Load rootdir\n");
    refresh_screen();
#endif  
   fs_load_rootdirEx();
    g_fat16_root_status = TRUE;


// -------------------------------------------
// Loading FAT.
#if defined(BAREMETAL_VERBOSE)
    printf ("bl: Load fat\n");
    refresh_screen();
#endif  
    fs_load_fatEx();
    g_fat16_fat_status = TRUE;


// -------------------------------------------
// Testing if the driver is reading.
#if defined(BAREMETAL_VERBOSE)

    printf ("bl: Testing read operation\n");
    refresh_screen();

    static char MySectorBuffer[512];

    // Read the first sector of rootdir.
    read_lba ( MySectorBuffer, FAT16_ROOTDIR_LBA  );

    MySectorBuffer[31] = 0;

    // Print the first entry.
    printf("Data={%s}\n",MySectorBuffer);
    
    refresh_screen();
    //while(1){ asm("cli"); asm("hlt"); }

#endif  


// -------------------------------------------
// Loading kernel image.

#if defined(BAREMETAL_VERBOSE)
    printf ("bl: Load kernel image\n");
    refresh_screen();
#endif  

// Initialize the de kernel environment.
// #todo:
// At this moment we need to update the bootblock
// to tell the kernel that we're loading him from
// a new perpective.
    if (initialize_de == TRUE){
        printf("blgram: Initializing de kernel\n");
        refresh_screen();
        Status = bl_load_kernel_image_for_de();

// Initialize the normal kernel environment.
    }else{
        Status = bl_load_kernel_image();
    }

    if (Status<0)
    {
        // Calling the emergency shell.
        bl_menu_loop();

        while (1){
            asm("cli");
            asm("hlt");
        };
         //goto run_rescue_shell;
    }

    //printf ("bl: breakpoint\n");
    //refresh_screen();
    //while(1){}

// ok, depois de carregarmos a imagem do kernel de 64bit
// então devemos configurar o long mode e a paginação para
// 64bit ... para por fim saltarmos para o kernel.

/*
  #todo:
 bl.bin é feito em 32bit e ainda em 32bit ele deve carregar
 o kernel que é um programa de 64bit. 
  Ao fim de sua rotina em C 32bit ele faz as configurações
  necessarias para entrar em 64bit. Poderá usar rotinas
 em assembly 64bit. 
 E por fim salta para o kernel de 64bit.
*/

// Nesse momento ja carregamos a imagem do kernel.
// No momento é uma imagem fake.
// #todo
// precisamos limpara a tela,
// para visualizarmos essa mensagem na resolução 320x200

// #test
// background preto.

    g_cursor_x=0;
    g_cursor_y=0;
    clear_backbuffer();

/*
    printf ("\n");
    printf ("\n");
    printf ("Gramado BL.BIN: [main.c-OS_Loader_Main()] \n");
    printf ("The 64bit kernel image is \n");
    printf ("already loaded. So now we will \n");
    printf ("setup the long mode, the paging and\n");
    printf ("jump to the kernel image\n");
    printf ("Good luck\n");
    printf("======================\n");
*/

//#debug
/*
    printf ("OS_Loader_Main:\n");
    printf ("The kernel image is already loaded\n");
    printf ("Let's setup long mode,\n");
    printf ("paging and jump to the kernel.\n");
*/

    printf("bl_main: Kernel image loaded\n");
    
//#breakpoint
    //refresh_screen();
    //while(1){}

#if defined(BAREMETAL_VERBOSE)
    printf("bl_main: Kernel image loaded :)\n");
    refresh_screen();
#endif  


/*
    How do I enable Long Mode ?
    The steps for enabling long mode are:
    + Disable paging
    + Set the PAE enable bit in CR4
    + Load CR3 with the physical address of the PML4
    + Enable long mode by setting the EFER.LME flag in MSR 0xC0000080
    + Enable paging
    Now the CPU will be in compatibility mode, and 
instructions are still 32-bit. To enter long mode, 
+ the D/B bit (bit 22, 2nd 32-bit value) of the GDT code segment 
must be clear (as it would be for a 16-bit code segment), and 
the L bit (bit 21, 2nd 32-bit value) of the GDT code segment 
must be set. Once that is done, the CPU is in 64-bit long mode.
See: https://wiki.osdev.org/X86-64
*/


// ================================
// Check x86_64 support.
// Test LM-bit
// #todo: We can create a local herper for that thing.

    unsigned long a=0;
    unsigned long b=0;
    unsigned long c=0;
    unsigned long d=0;

    cpuid( 0x80000001, a, b, c, d );

    unsigned long data = (unsigned long) (d >> 29);

/*
// x86_64 is supported.
    if ( (data & 1) != 0 ){
        printf("OS_Loader_Main: x86_64 hardware supported\n");
        refresh_screen();
    }
*/

//
// x86_64 is not supported.
//

    if ((data & 1) == 0)
    {
        // Final message and hang
        __no_x86_64_was_found();
        // Hang
        while (1){
            asm ("cli");
            asm ("hlt");
        };
    }

    //printf ("done\n");
    //printf("======================\n");

    // #breakpoint
    //refresh_screen();
    //while(1){}

// No interrupts

    asm ("cli");

// ====
// Paging:
// Depois carregar o kernel e os modulos 
// nos seus endereços fisicos, 
// configura a paginaçao e 
// volta para o assembly para 
// configurar os registradores e 
// passar o comando para o kernel.
// #obs:
// Essa configuraçao basica nao impede
// que o kernel faça uma reconfiguraçao completa.
// #slow
// #debug
// In this document.
// See: pages.c

    //#debug
    //printf ("OS_Loader_Main: Setup paging ...\n");
    //refresh_screen();
    //while(1){}

    SetUpPaging();

// Nao podemos chamar rotina alguma aqui, somente retornar.
// Pois os registradores estao bagunçados.

// Not reached
    while (1){
        asm("cli");
        asm("hlt");
    };
}

//
// End
//


