// kmain.c
// This is the first C file, called after the assembly routines.
// Created by Fred Nora.

// This file contains the I_kmain function,
// called by START in 
// entrance/warden/unit0/x86_64/head_64.asm.
// This file and their folder will be the control panel
// for the initialization routine.
// Please put here data we need.
// fake main.c
// We need a fake KERNEL.BIN ELF file that will be used my the
// boot loader.
// The boot loader will load the fake kernel image before
// setting up the long mode and the paging.

// The main subsystem is the window system GWSSRV.BIN.
// The first usermode application is GWS.BIN.
// See:
// GWSSRV.BIN: prestier/gws/server
// GWS.BIN:    prestier/gws/client

// #bsod
// List of errors:
// Error: 0x00 - Unexpected error.
// Error: 0x01 - CURRENT_ARCH_X86_64 initialization fail.
// Error: 0x02 - generic.
// Error: 0x03 - Undefined 'current arch'.
// Error: 0x04 - The kernel image is too long.
// Error: 0x05 - Memory allocation for Display device structure.
// ...

#include <kernel.h>

// see: utsname.h
struct utsname  kernel_utsname;


typedef enum {
    KERNEL_SUBSYSTEM_INVALID,
    KERNEL_SUBSYSTEM_KMAIN,  // Main routine
    KERNEL_SUBSYSTEM_DEV,    // Device drivers
    KERNEL_SUBSYSTEM_FS,     // File systems
    KERNEL_SUBSYSTEM_KE,     // Kernel executive
    KERNEL_SUBSYSTEM_MM,     // Memory manager
    KERNEL_SUBSYSTEM_NET,    // Network
    KERNEL_SUBSYSTEM_USER    // User manager. (um)
}kernel_subsystem_t;

// Local
static kernel_subsystem_t __failing_kernel_subsystem = 
    KERNEL_SUBSYSTEM_INVALID;


// Not so important
int gSystemEdition=0;
int gSystemStatus=0;

// See: system.h
struct version_d  *Version;
struct version_info_d  *VersionInfo;
struct system_d  *System;


//
// Initialization :)
//

// Global
//unsigned long gInitializationPhase=0;

// The initialization structure.
// See: ke/kinit.h
struct initialization_d  Initialization;


// Drivers support
// Internal modules support.
int g_driver_ps2keyboard_initialized=FALSE;   //ps2 keyboard
int g_driver_ps2mouse_initialized=FALSE;      //ps2 mouse
int g_driver_video_initialized=FALSE;
int g_driver_apic_initialized=FALSE;
int g_driver_pci_initialized=FALSE;
int g_driver_rtc_initialized=FALSE;
int g_driver_timer_initialized=FALSE;
//...

// Internal modules support.
int g_module_shell_initialized=FALSE;
int g_module_debug_initialized=FALSE;
int g_module_disk_initialized=FALSE;
int g_module_volume_initialized=FALSE;
int g_module_fs_initialized=FALSE;
int g_module_gui_initialized=FALSE;
int g_module_logoff_initialized=FALSE;
int g_module_logon_initialized=FALSE;
int g_module_mm_initialized=FALSE;
int g_module_objectmanager_initialized=FALSE;
int g_module_runtime_initialized=FALSE;
int g_module_uem_initialized=FALSE;             //user environment manager.
//...


//se ele esta inicializado ou nao
int dead_thread_collector_status=0;
// Se e' para usalo ou nao
int dead_thread_collector_flag=0;

//pid_t current_dead_process;
//int current_dead_thread;

//char InitialUserProcessName[32] = "INIT.BIN"

// --------------------------------------
// head_64.asm is given us these two values.
// See: head_64.asm.
unsigned long magic=0;
// Saving the bootblock address passed by the blgram.
// But we already know it. 
// See: BootBlockVA in bootblk.h
unsigned long saved_bootblock_base=0;


// ================================

//
// == Private functions: Prototypes ========
//

// Internal
static void __enter_debug_mode(void);
static void __print_final_messages(void);
static void __setup_utsname(void);

// Early init routines
static int earlyinit_SetupBootblock(void);
static void earlyinit_Globals(int arch_type);
static void earlyinit_Serial(void);
static void earlyinit_OutputSupport(void);

static int earlyinit(void);
static int archinit(void);
static int deviceinit(void);
static int lateinit(void);

static int __test_initialize_ap_processor(int apic_id);

static int I_initialize_kernel(int arch_type, int processor_number);


static void panic_at_init(int error_code, kernel_subsystem_t subsystem_id);

//
// =======================================================
//

/*
 * panic_at_init()
 *
 * Purpose:
 *   Specialized panic routine for system initialization failures.
 *   It is called when a critical error occurs during one of the
 *   initialization levels (earlyinit, mmInitialize, keInitialize, etc.).
 *
 * Behavior:
 *   - Formats the error code into a short string: "INIT PANIC 0x000000NN".
 *   - Delegates to x_panic(), which draws a red rectangle at the top
 *     of the framebuffer and prints the message.
 *   - Halts the system immediately.
 *
 * Context:
 *   - Used before the kernel virtual console is fully initialized.
 *   - Provides a guaranteed visual panic message during early boot.
 *
 * Example:
 *   if (mmInitialize(0) != TRUE)
 *       panic_at_init(Error20_mmPhase0,0);
 */

// Panic during system initialization.
// Builds one-line message with function+phase and error code.
// Logs to serial if available, then halts via x_panic().
static void panic_at_init(int error_code, kernel_subsystem_t subsystem_id)
{
    char msg[64];
    const char *funcname = "init";

    // Record failing subsystem
    // #todo: It will be used in the future.
    __failing_kernel_subsystem = subsystem_id;

    // Map error codes directly here
    switch (error_code) {
    case Error10_earlyinit:
        funcname = "earlyinit";
        break;

    case Error20_mmPhase0:
        funcname = "mmInitialize0";
        break;
    case Error21_mmPhase1:
        funcname = "mmInitialize1";
        break;

    case Error30_kePhase0:
        funcname = "keInitialize0";
        break;
    case Error31_kePhase1:
        funcname = "keInitialize1";
        break;
    case Error32_kePhase2:
        funcname = "keInitialize2";
        break;

    case Error40_archinit:
        funcname = "archinit";
        break;

    case Error50_deviceinit:
        funcname = "deviceinit";
        break;

    case Error60_lateinit:
        funcname = "lateinit";
        break;

    case Error90_breakpoint:
        funcname = "Breakpoint";
        break;

    default:
        funcname = "init";
        break;
    }

    memset(msg,0,64);

    // Build final message: "PANIC: <func> Error: 0xXX"
    ksprintf(msg, "%s Error: 0x%x", funcname, (error_code & 0xFF));

    // Serial log first (if available)
    if (Initialization.is_serial_log_initialized == TRUE) {
        PROGRESS(msg);
        PROGRESS("\n");
    }

    // #todo
    // If network stack is ready, send packet

    // Last call: framebuffer panic worker (hangs system)
    x_panic(msg);
}


static void __enter_debug_mode(void)
{
    if (Initialization.is_serial_log_initialized == TRUE){
        debug_print("__enter_debug_mode:\n");
    }

    if (Initialization.is_console_log_initialized != TRUE)
    {
        if (Initialization.is_serial_log_initialized == TRUE){
            debug_print("__enter_debug_mode: can't use the cirtual console\n");
        }
    }

    if (Initialization.is_console_log_initialized == TRUE)
    {
        //printk("kmain.c: The kernel is in debug mode.\n");
        // kgwm_early_kernel_console();  //#deprecated
        //printk("kmain.c: End of debug mode.\n");
        
        // #panic
        printk("__enter_debug_mode: [PANIC] console log ok\n");
        refresh_screen();
        die();
    }

// Die. 
// No refresh and no message.
    die();
}

// ===================
// ::(2)(3)
static void __print_final_messages(void)
{
// Final messages
    if (Initialization.is_serial_log_initialized == TRUE){
        PROGRESS("::(2)(3): [final message] FAILURE\n");
    }
    if (Initialization.is_console_log_initialized == TRUE){
        printk("init: [final message] FAILURE\n");
        refresh_screen();
    }
}

// utsname
// #todo
// Create the methods for the applications
// to setup this structure and 
// and read the information from it.
// see: sys_uname() in sys.c

static void __setup_utsname(void)
{
    memset(&kernel_utsname, 0, sizeof(struct utsname));

    // OS name
    ksprintf(kernel_utsname.sysname,    OS_NAME);       // "Gramado"
    // Release type (Hypervisor, Server, Desktop, etc.)
    ksprintf(kernel_utsname.release,    RELEASE_NAME);  // "Hypervisor"
    // Version string (from version.h)
    ksprintf(kernel_utsname.version,    VERSION_NAME);  // "0.8"
    // Machine architecture
    ksprintf(kernel_utsname.machine,    MACHINE_NAME);  // "x86_64"
    // Hostname / node name
    ksprintf(kernel_utsname.nodename,   NODE_NAME);     // e.g. "gramado-host"
    // Domain name
    ksprintf(kernel_utsname.domainname, DOMAIN_NAME);   // "DOMAIN-NAME"
}

// == Idle thread in ring 0  ===============
// #suspended
// #test
// #bugbug
// This thread will start to run at the moment when
// the init process enable the interrupts.
void keEarlyRing0IdleThread (void)
{
// #danger: Do NOT change this function.
// #bugbug: This thread can't execute complex routine for now.
    //printk("");  //fail

    //debug_print ("keEarlyRing0IdleThread: w h\n");

    /*
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("keEarlyRing0IdleThread: w h\n");
        panic       ("keEarlyRing0IdleThread: w h\n");
    }
    */
Loop:
// acende
    //backbuffer_draw_rectangle( 0, 0, deviceWidth, 28, COLOR_KERNEL_BACKGROUND );
    //winkDrawString(8,8,COLOR_YELLOW," Gramado Operating System ");
    //refresh_screen();
// relax
    asm ("sti");
    asm ("hlt");
// apaga
    //backbuffer_draw_rectangle( 0, 0, deviceWidth, 28, COLOR_KERNEL_BACKGROUND );
    //backbuffer_draw_rectangle( 0, 0, deviceWidth, deviceHeight, COLOR_KERNEL_BACKGROUND );  //#bug
    //refresh_screen();
    goto Loop;
}

void init_globals(void)
{
// Called by I_initKernelComponents() in x64init.c
// Architecture independent?
// Not everything here.
// Order: cpu, ram, devices, etc.
// #todo: maybe we can move this routine to x64init.c

    int Status=FALSE;

// smp
    g_smp_initialized = FALSE;
    g_processor_count = 0;

// Profiler
// See: pints.h
// Intel/AMD
// Legacy hardware interrupts (irqs) (legacy pic)
    g_profiler_ints_irq0  = 0;
    g_profiler_ints_irq1  = 0;
    g_profiler_ints_irq2  = 0;
    g_profiler_ints_irq3  = 0;
    g_profiler_ints_irq4  = 0;
    g_profiler_ints_irq5  = 0;
    g_profiler_ints_irq6  = 0;
    g_profiler_ints_irq7  = 0;
    g_profiler_ints_irq8  = 0;
    g_profiler_ints_irq9  = 0;
    g_profiler_ints_irq10 = 0;
    g_profiler_ints_irq11 = 0;
    g_profiler_ints_irq12 = 0;
    g_profiler_ints_irq13 = 0;
    g_profiler_ints_irq14 = 0;
    g_profiler_ints_irq15 = 0;
    // ...

// Global counter for syscalls
    g_profiler_ints_syscall_counter = 0;

// User and group.
    current_user  = 0;
    current_group = 0;

// Security layers.
    current_usersession = (int) 0;

// Process
    foreground_process = (pid_t) 0;
    //current_process = (pid_t) 0;
    set_current_process(0);  //?

// Thread
    foreground_thread = (tid_t) 0;
    current_thread = (int) 0;

// File system support.
// Type=1 | FAT16.
    g_currentvolume_filesystem_type = FS_TYPE_FAT16;
    g_currentvolume_fatbits = (int) 16;


// Create the device list for all the devices
// in our system, including the hal stuff i guess.
    devInitialize();

// libk/ module.
// + Initialize srtuctures for the stadard streams.
// + It uses the global file table.
// + It also makes the early initialization of the consoles.
    Status = (int) libk_initialize();
    if (Status != TRUE){
        x_panic("init_globals: on libk_initialize()\n");
    }

// ===================

// The kernel request
// See: request.c
    clearDeferredKernelRequest();

// Screen
// Now we can print strings in the screen.
// Reinitializing ... 
// see: bldisp.c
    bldispScreenInit();

    //debug_print("keInitGlobals: [printk] WE HAVE MESSAGES NOW!\n");
    //printk     ("keInitGlobals: [printk] WE HAVE MESSAGES NOW!\n");

// Redirect printk to serial port during syscalls.
// See: config.h and fslib.c
    if ( CONFIG_PRINTK_TO_SERIAL_DURING_SYSCALLS == 1 ){
        g_redirect_printk_to_serial_during_syscalls = TRUE;
    } else {
        g_redirect_printk_to_serial_during_syscalls = FALSE;
    };
}

//
// $
// EARLY INIT
//

// ==============================
// #limitation: No serial debug yet.
// #todo: #bugbug
// We have another BootBlock structure in info.h
static int earlyinit_SetupBootblock(void)
{
// We don't have any print support for now.
// #bugbug
// In head_64.asm we saved the boot address in _saved_bootblock_base.
// But now we're using the address BootBlockVA.

// -------------------------
// base:
// The boot block address. 0x0000000000090000.
// Each entry has 8 bytes.
// virtual = physical.
// #bugbug
// In head_64.asm we saved the boot address in _saved_bootblock_base.
// But now we're using the address BootBlockVA.

// #warning
// This is a version of the boot block with 64bit entries.
// The 64bit entries version was created in the address 0x00090000
// by the bl.bin in head.s. 

    unsigned long *xxxxBootBlock = (unsigned long*) BootBlockVA; 

// -------------------------
// lfb:
// Esse endereço virtual foi configurado pelo bootloader.
// Ainda não configuramos a paginação no kernel.
    unsigned long *fb = (unsigned long *) FRONTBUFFER_VA; 
    fb[0] = 0x00FFFFFF;

// -------------------------
// magic:
// #bugbug: Explain it better.
// We got this number in head_64.asm givem from the boot loader.
    unsigned long bootMagic = 
        (unsigned long) (magic & 0x00000000FFFFFFFF); 

// Check magic
// Paint a white screen if magic is ok.
// Paint a 'colored' screen if magic is not ok.
    register int i=0;
// WHITE
    if (bootMagic == 1234){
        for (i=0; i<320*32; i++){ fb[i] = 0xFFFFFFFFFFFFFFFF; };
    }
// COLORED
    if (bootMagic != 1234){
        for (i=0; i<320*32; i++){ fb[i] = 0xFF00FFFFFFF00FFF; };
        //#debug
        //while(1){}
    }

    //#debug
    //while(1){}

// -------------------------
// info:
// Save the info into a kernel structure
// defined in mm/globals.c.
// See the structure bootblk_d in bootblk.h.
// #bugbug
// Actually this is swrong, because the boot info
// structure is given not only information about 
// the display device, so need to put this structure
// in another document, not in display.h.
// #warning
// This is a version of the boot block with 64bit entries.
// The 64bit entries version was created in the address 0x00090000
// by the bl.bin in head.s. 

// -------------------------
// Display device info
// Boot display device. (VESA)

    bootblk.lfb_pa        = (unsigned long) xxxxBootBlock[bbOffsetLFB_PA];
    bootblk.deviceWidth   = (unsigned long) xxxxBootBlock[bbOffsetX];
    bootblk.deviceHeight  = (unsigned long) xxxxBootBlock[bbOffsetY];
    bootblk.bpp           = (unsigned long) xxxxBootBlock[bbOffsetBPP];

// Saving the resolution info into another place.
// See: kernel.h
    gSavedLFB = (unsigned long) bootblk.lfb_pa;
    gSavedX   = (unsigned long) bootblk.deviceWidth;
    gSavedY   = (unsigned long) bootblk.deviceHeight;
    gSavedBPP = (unsigned long) bootblk.bpp;

// Set up private variables in screen.c
    screenSetSize(gSavedX,gSavedY);

// -------------------------
// Memory info
    bootblk.last_valid_pa = (unsigned long) xxxxBootBlock[bbLastValidPA];
// Last valid physical address.
// Used to get the available physical memory.
    blSavedLastValidAddress = (unsigned long) bootblk.last_valid_pa; 
// Memory size in KB.
    blSavedPhysicalMemoryInKB = 
        (unsigned long) (blSavedLastValidAddress / 1024);

// -------------------------
// System info
    bootblk.gramado_mode = (unsigned long) xxxxBootBlock[bbGramadoMode];
// Gramado mode. (jail, p1, home ...)
// Save global variable.
    current_mode = (unsigned long) bootblk.gramado_mode;

// 48
    bootblk.ide_port_number = (unsigned long) xxxxBootBlock[bb_idePortNumber];


// ---------------------
// #test
// The signature per se.
// Just saving it for now. We're gnna use it later.
    bootblk.disk_signature = 
        (unsigned long) xxxxBootBlock[bbDiskSignature];

// ---------------------
// #note
// Well, we do not have information about the disk.
// + What is the boot disk number given by the BIOS?
// + ...

// Validation
    bootblk.initialized = TRUE;
    return 0;
}

// ==========================
static void earlyinit_Globals(int arch_type)
{
// We don't have any print support for now.

// Scheduler policies
// Early initialization.
// See: 
// sched.h, sched.c.

    //SchedulerInfo.policy = SCHED_POLICY_RR;
    //SchedulerInfo.flags  = (unsigned long) 0;

    InitThread = NULL;   // thread
    TEInitProcess = NULL;  // thread environment

    // Invalidate
    set_current_process(-1);
    SetCurrentTID(-1);

// Initializing the global spinlock.
    __spinlock_ipc = TRUE;

// IO Control
    IOControl.useTTY = FALSE;        // Model not implemented yet.
    IOControl.useEventQueue = TRUE;  // The current model.
    IOControl.initialized = TRUE;    // IO system initialized.
    // ...

//
// Presence level
//

// See:
// sched.h, sched.c

    // (timer ticks / fps) = level

    //presence_level = (1000/1000);  //level 1;    // 1000   fps
    //presence_level = (1000/500);   //level 2;    // 500    fps
    //presence_level = (1000/250);   //level 4;    // 250    fps
    //presence_level = (1000/125);   //level 8;    // 125    fps
    //presence_level = (1000/62);    //level 16;   // 62,20  fps
    //presence_level = (1000/25);    //level 32;   // 31,25  fps
    //presence_level = (1000/15);    //level 64;   // 15,625 fps 
    //presence_level = (1000/10);    //level 100;   // 10 fps
    //presence_level = (1000/5);     //level 200;   // 5  fps
    // presence_level = (1000/4);     //level 250;  // 4  fps
    //presence_level = (1000/2);     //level 500;   // 2  fps
    //presence_level = (1000/1);     //level 1000;  // 1  fps

    //set_update_screen_frequency(30);
    set_update_screen_frequency(60);
    //set_update_screen_frequency(120);
}

// =========================
// see: serial.c
static void earlyinit_Serial(void)
{
// We still don't have any print support yet.
// But at the end of this routine we can use the serial debug.

// Driver initialization
// see: serial.c

    int Status=FALSE;
    Status = DDINIT_serial();
    if (Status != TRUE){
        //#bugbug
        //Oh boy!, We can't use the serial debug.
    }
    PROGRESS("Welcome!\n");
    PROGRESS("earlyinit_Serial: Serial debug initialized\n");
}

// ======================
static void earlyinit_OutputSupport(void)
{
// #important: 
// We do not have all the runtime support yet.
// We can't use printk yet.
// We only initialized some console structures,
// not the full support for printk functions.

    //PROGRESS("earlyinit_OutputSupport:\n");

// O refresh ainda não funciona, 
// precisamos calcular se a quantidade mapeada é suficiente.
    refresh_screen_enabled = FALSE;

    //PROGRESS("earlyinit_OutputSupport: wink_initialize_virtual_consoles\n");
    wink_initialize_virtual_consoles();

    //debug_print("earlyinit_OutputSupport: earlyinit_OutputSupport\n");

// #important: 
// We do not have all the runtime support yet.
// We can't use printk yet.
}

// :: Level 1 
// We don't have any print support for now.
// But here is the moment when we initialize the
// serial debug support.
static int earlyinit(void)
{

// Starting the counter.
    Initialization.current_phase = 0;

// ==================

// Checkpoints
    Initialization.phase1_checkpoint = FALSE;
    Initialization.phase2_checkpoint = FALSE;
    Initialization.phase3_checkpoint = FALSE;

// mm
    Initialization.mm_phase0 = FALSE;
    Initialization.mm_phase1 = FALSE;
    
// ke
    Initialization.ke_phase0 = FALSE;
    Initialization.ke_phase1 = FALSE;
    Initialization.ke_phase2 = FALSE;

// Flags
// We still dont have any log/verbose support.
    Initialization.is_serial_log_initialized = FALSE;
    Initialization.is_console_log_initialized = FALSE;
// The display dirver is not initialized yet,
// but the kernel owns it.
    Initialization.kernel_owns_display_device = TRUE;

// Hack Hack
    VideoBlock.useGui = TRUE;

// first of all
// Getting critical boot information.
    earlyinit_SetupBootblock();

// We do not have serial debug yet.
    earlyinit_Globals(0);  // IN: arch_type

// Serial debug support.
// After that routine we can use the serial debug functions.
    earlyinit_Serial();

// Initialize the virtual console structures.
// We do not have all the runtime support yet.
// We can't use printk yet.
// #important: 
// We do not have all the runtime support yet.
// We can't use printk yet.
// We only initialized some console structures,
// not the full support for printk functions.
    earlyinit_OutputSupport();

    return 0;
}

// Initializing the AP processor.
static int __test_initialize_ap_processor(int apic_id)
{
    // AP base address
    //unsigned char *ap_base = (unsigned char *) 0x00020000; 0x8000;  // AP trampoline base address.
    // AP signature pointer.
    // The AP will change the memory value at this address to
    // 0xA0 when it starts.
    unsigned char *ap_signature_pointer = 
        (unsigned char *) ____DANGER_TRAMPOLINE_CODE_SIGNATURE_AREA;

    // ex: SIPI vector: 0x20000 >> 12 = 0x20, 
    // so you’d send SIPI with vector 0x20.
    unsigned int vector = 
        (unsigned int) (____DANGER_TRAMPOLINE_CODE_BASE >> 12);  // 0x20

    unsigned long BufferSizeInBytes = (2*4096);  // 8KB

//
// How many processors?
//

    // No APs processors yet
    smp_info.nr_ap_running = 0;
    if (CONFIG_INITIALIZE_SECOND_PROCESSOR == 1)
    {
        // (Step 1) Load AP image into memory.
        // Address 0x8000, vector 0x08
        printk("Loading AP image ...\n");

        // #todo: Check return value.
        fsLoadFile ( 
            VOLUME1_FAT_ADDRESS,
            VOLUME1_ROOTDIR_ADDRESS, //sdDE.address,
            FAT16_ROOT_ENTRIES, //512,           // Number of dir entries?
            "APX86   BIN", // AP image file name 
            (unsigned long) ____DANGER_TRAMPOLINE_CODE_BASE, // Address
            BufferSizeInBytes );

        // (Step 2)
        printk("Sending INIT IPI ...\n");
        local_apic_send_startup(1,vector);  // APIC ID 1

        // (Step 3)
        printk("Sending STARTUP IPI twice ...\n");
        refresh_screen(); //wait
        Send_STARTUP_IPI_Twice(1);

        // Check if we have at least one AP running.
        // #todo
        // Well, we don't need to stay in this loop waiting for signature.
        // We can check it later after the kernel full initialization.
        while (1){
            if (ap_signature_pointer[0] == 0x64 && 
                ap_signature_pointer[1] == 0x64 )
            {
                printk("kernel: AP is running in 64bit\n");
                // Our first AP processor is running
                smp_info.nr_ap_running = 1;
                break;
            }
            /*
            if (ap_signature_pointer[0] == 0xA0 && 
                ap_signature_pointer[1] == 0xA0 )
            {
                printk("AP is running in 32bit!\n");
                // Our first AP processor is running
                smp_info.nr_ap_running = 1;
                break;
            }
            */
            if (ap_signature_pointer[0] == 0x63 && 
                ap_signature_pointer[1] == 0x63 )
            {
                panic("kernel: AP Coudn't enter in long mode\n");
                //asm ("cli \n");
                //asm ("hlt \n");
            }
        };

        // #debug
        //printk(">>>> breakpoint\n");
        //refresh_screen();
        //while(1){asm ("cli"); asm ("hlt");}

    } // End of CONFIG_INITIALIZE_SECOND_PROCESSOR

    return 0;
}

// :: Level ?
static int archinit(void)
{

/*
//++
//----------
// #test
// Do we have the file bus.c?
    char *eisa_p;
    int EISA_bus=0;
    eisa_p = 0x000FFFD9;
	if (kstrncmp(eisa_p, "EISA", 4) == 0)
		EISA_bus = 1;
    printk("EISA_bus %d\n",EISA_bus);
    while(1){}
//----------
//--
*/

// ----------------------------------
// Last thing
// + Probing for processor type.
// + Initializing smp for x64 machines.

    int smp_status = FALSE;
    int processor_type = -1;

    if (USE_SMP == 1)
    {
        processor_type = (int) hal_probe_processor_type();
        if ( processor_type == Processor_AMD ||
             processor_type == Processor_INTEL )
        {
            // #test
            // Testing the function serial_printk(),
            // it sends a formated string to the serial port.
            // #ok, it is working at this part, not in the
            // beginning of the routine.
            serial_printk("archinit: processor_type {%d}\n",
                processor_type );

            // k2_ke/x86_64/x64smp.c
            // Probe for smp support and initialize lapic.
            smp_status = (int) x64smp_initialization();
            //if (smp_status != TRUE)
                //panic("smp\n");

            // #warning: See the flags in config.h

            // >> ENABLE APIC
            // Lets enable the apic, only if smp is supported.
            // #warning
            // This routine is gonna disable the legacy PIC.
            // see: apic.c
            if (ENABLE_APIC == 1)
            {
                // Enable apic and timer
                if (LAPIC.initialized == TRUE)
                    enable_apic();
                //printk("kmain: breakpoint on enable_apic()\n");
                //while(1){}
            }

            // >> ENABLE IOAPIC
            // #todo
            // Setup ioapic.
            // see: ioapic.c
            //if (ENABLE_IOAPIC == 1)
            //{
                // #todo
                // Isso configura o timer ...
                // e o timer precisa mudar o vetor 
                // pois 32 ja esta sendo usado pela redirection table.
                //enable_ioapic();
            //}

            // #debug
            // Show cpu info.
            // see: x64info.c
            //x64_info();

            if (CONFIG_INITIALIZE_SECOND_PROCESSOR == 1)
            {
                // Initialize AP processor.
                // #bugbug: g_processor_count if faling
                //if (g_processor_count >= 2)
                if (smp_info.mptable_number_of_processors >= 2)
                {
                    smp_info.nr_ap_running = 0;
                    __test_initialize_ap_processor(1);  // APIC ID 1
                //__test_initialize_ap_processor(2); // APIC ID 2
                // ...
                }

                // #debug
                //printk(">>>> breakpoint\n");
                //refresh_screen();
                //while(1){asm ("cli"); asm ("hlt");}

            } // End of CONFIG_INITIALIZE_SECOND_PROCESSOR
        }
    }

    return 0;
}

static int deviceinit(void)
{

// IN: phase number = 1.
// Chose the keyboad maps
    ibroker_initialize(1);

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

// --------------------------
// Initialize the input targets.
// Select the default desired input targets
// See: 
// config.h input.h input.c ibroker.h ibroker.c

/*
   //#delete
    if (CONFIG_INPUT_TARGET_TTY == 1){
        input_enable_this_input_target(INPUT_TARGET_TTY);
    }else{
        input_disable_this_input_target(INPUT_TARGET_TTY);
    };
*/

    if (CONFIG_INPUT_TARGET_STDIN == 1){
        input_enable_this_input_target(INPUT_TARGET_STDIN);
    }else{
        input_disable_this_input_target(INPUT_TARGET_STDIN);
    };

    if (CONFIG_INPUT_TARGET_THREAD_QUEUE == 1){
        input_enable_this_input_target(INPUT_TARGET_THREAD_QUEUE);
    }else{
        input_disable_this_input_target(INPUT_TARGET_THREAD_QUEUE);
    };


    // #todo
    // usb_initialize();

// ...

    return 0;
}

// :: Level ?
static int lateinit(void)
{
    int ok = 0;
    //int UseDebugMode=TRUE;  //#bugbug
    int UseDebugMode=FALSE;

// -------------------------------------
// #test
// Creating the legacy pty maste and pty slave.
// see: pty.c
    // #debug
    //printk (":: Creating legacy ptys\n");

    //PROGRESS(":: PTY\n");
    tty_initialize_legacy_pty();

// -------------------------------------

    // see: user.c
    int uStatus =  FALSE;
    uStatus = (int) user_initialize();
    if (uStatus != TRUE)
        panic("lateinit: on user_initialize()\n");

/*
 // ok
    // Wait for some problem with the AP that was initialized.
    printk("Waiting ...\n");
    refresh_screen();
    while(1){}
*/

/*
// ok
    // Wait for some problem with the AP that was initialized.
    printk("Waiting ...\n");
    refresh_screen();
    while(1){}
*/

    // ==========================
    // Network support.
    // ?? At this moment we already initialized the e1000 driver.
    // See: net.c
    netInitialize();

/*
    // ok
    // Wait for some problem with the AP that was initialized.
    printk("Waiting ...\n");
    refresh_screen();
    while(1){}
*/

// -------------------------------------
// Setup utsname structure.
    __setup_utsname();

// -------------------------------------
// Runlevel switch:
// Enter into the debug console instead of jumping 
// into the init thread.
// ::: Initialization on debug mode
// Initialize the default kernel virtual console.
// It depends on the run_level.
// #bugbug: The interrupts are not initialized,
// it's done via init process?
// See: kgwm.c
    if (UseDebugMode == TRUE){
        __enter_debug_mode();
    }

/*
    // ok
    // Wait for some problem with the AP that was initialized.
    printk("Waiting ...\n");
    refresh_screen();
    while(1){}
*/


// -------------------------------------
// Initialize support for loadable kernel modules.
// See: mod.c 
    int mod_status = -1;
    mod_status = (int) mod_initialize();
    if (mod_status != TRUE)
        panic("lateinit: on mod_initialize()\n");


// -------------------------------------
// Execute the first ring3 process.
// ireq to init thread.
// See: ke.c

/*
    PROGRESS(":: INITIAL PROCESS <<<\n");

    //#debug
    refresh_screen();
    while (1){ 
        asm volatile ("cli");
        asm volatile ("hlt"); 
    };
*/

    ok = (int) ke_x64ExecuteInitialProcess();
    if (ok < 0){
        panic ("lateinit: Couldn't launch init process\n");
        // #todo:
        // Maybe we can call the kernel console for debuging purpose.
        goto fail;
    }

    return TRUE;

fail:
    return FALSE;
}

// Initializes the kernel, create the kernel process,
// launches the forst ring 0 loadable module, creates the ini process,
// the first thread for the init process and execute the firs thread.
// Called by I_kmain during the initialization of the BSP.
// APs have a different initialization routine.
// We don't have any print support yet.
// See: kernel.h, kmain.h
// ==================================
// This is the sequence of functions called by this function.
// Levels:
// + [1]   earlyinit()
// + [2:0] mmInitialize(0)
// + [2:1] mmInitialize(1)
// + [3:0] keInitialize(0)
// + [3:1] keInitialize(1)
// + [3:2] keInitialize(2)
// + [4]   archinit()
// + [5]   deviceinit()
// + [6]   lateinit()
// ==================================
static int I_initialize_kernel(int arch_type, int processor_number)
{
    int Status = FALSE;

// Early init
// We don't have any print support yet.
// We initialized the serial debug support, and console structures, 
// but we still can't use the printk functions.

    system_state = SYSTEM_PREINIT;
    // [1]
    earlyinit();
    wink_update_progress_bar(5);
    //while(1){}

//
// Booting
//

    PROGRESS(":: BOOTING\n");
    system_state = SYSTEM_BOOTING;

// boot info
    if (bootblk.initialized != TRUE){
        panic("I_kmain: bootblk.initialized\n");
    }

//
// mm subsystem
//

    //PROGRESS(":: Initialize mm subsystem\n");

// -------------------------------
// Initialize mm phase 0.
// See: mm.c
// + Initialize video support.
// + Inittialize heap support.
// + Inittialize stack support. 
// + Initialize memory sizes.
    PROGRESS(":: MM PHASE 0\n");
    // [2:0]
    Status = (int) mmInitialize(0);
    if (Status != TRUE){
        __failing_kernel_subsystem = KERNEL_SUBSYSTEM_MM;
        if (Initialization.is_serial_log_initialized == TRUE){
            debug_print("I_kmain: mmInitialize phase 0 fail\n");
        }
        goto fail;
    }
    Initialization.mm_phase0 = TRUE;
    wink_update_progress_bar(20);
    //while(1){}

// -------------------------------
// Initialize mm phase 1.
// + Initialize framepoll support.
// + Initializing the paging infrastructure.
//   Mapping all the static system areas.
    PROGRESS(":: MM PHASE 1\n");
    // [2:1]
    Status = (int) mmInitialize(1);
    if (Status != TRUE){
        __failing_kernel_subsystem = KERNEL_SUBSYSTEM_MM;
        if (Initialization.is_serial_log_initialized == TRUE){
            debug_print("I_kmain: mmInitialize phase 1 fail\n");
        }
        goto fail;
    }
    Initialization.mm_phase1 = TRUE;
    wink_update_progress_bar(40);
    //while(1){}

    g_module_runtime_initialized = TRUE;

//
// ke subsystem
//

    //PROGRESS(":: Initialize ke subsystem\n");

// -------------------------------
// Initialize ke phase 0.
// See: ke.c
// + kernel font.
// + background.
// + refresh support.
// + show banner and resolution info.
// + Check gramado mode and grab data from linker.
// + Initialize bootloader display device.
    PROGRESS(":: KE PHASE 0\n");
    // [3:0]
    Status = (int) keInitialize(0);
    if (Status != TRUE)
        panic_at_init(Error30_kePhase0,KERNEL_SUBSYSTEM_KE);
    Initialization.ke_phase0 = TRUE;
    wink_update_progress_bar(50);
    //while(1){}

// -------------------------------
// Initialize ke phase 1.
// + Calling I_x64main to 
//   initialize a lot of stuff from the 
//   current architecture.
// + PS2 early initialization.
    PROGRESS(":: KE PHASE 1\n");
    // [3:1]
    Status = (int) keInitialize(1);
    if (Status != TRUE)
        panic_at_init(Error31_kePhase1,KERNEL_SUBSYSTEM_KE);
    Initialization.ke_phase1 = TRUE;
    wink_update_progress_bar(60);
    //while(1){}

// -------------------------------
// Initialize ke phase 2.
// + Initialize background.
// + Load BMP icons.
    PROGRESS(":: KE PHASE 2\n");
    // [3:2]
    Status = (int) keInitialize(2);
    if (Status != TRUE)
        panic_at_init(Error32_kePhase2,KERNEL_SUBSYSTEM_KE);
    Initialization.ke_phase2 = TRUE;
    wink_update_progress_bar(70);
    //while(1){}

// -------------------------------
    PROGRESS(":: archinit\n");
    // [4]
    archinit();
    wink_update_progress_bar(80);
    //while(1){}

// -------------------------------
    PROGRESS(":: deviceinit\n");
    // [5]
    deviceinit();
    wink_update_progress_bar(100);
    //while(1){}

// -------------------------------
    if (Status == TRUE)
    {
        PROGRESS(":: lateinit\n");
        // [6]
        Status = (int) lateinit();
        if (Status != TRUE)
            panic_at_init(Error60_lateinit,KERNEL_SUBSYSTEM_KMAIN);
    }

// Initialization fail
fail:
    system_state = SYSTEM_ABORTED;
    return FALSE;
}

//
// $
// MAIN
//

// --------------------------------
// Called by START in startup/head_64.asm.
// This is the first function in C.
// We don't have any print support yet.
// See: kernel.h, kmain.h
// #todo: Maybe we can change this name to BSP_kmain()
void I_kmain(int arch_type)
{
    int Status = -1;
    static int ProcessorNumber = 0;

// #test
// Initialize variable that makes sense only for the BSP,
// Because APs do not call I_kmain.


// System state.
// It guides the initialization.
    system_state = SYSTEM_PREINIT;

// Runlevel
// Used by init system.
// See: config.h
    wrappers_set_current_runlevel(CONFIG_CURRENT_RUNLEVEL);

// Product type.
// see: config/product.h
    g_product_type = PRODUCT_TYPE;
    // Status Flag and Edition flag.
    gSystemStatus = 1;
    gSystemEdition = 0;
    __failing_kernel_subsystem = KERNEL_SUBSYSTEM_INVALID;

    has_booted = FALSE;

// No APs processors yet
    smp_info.nr_ap_running = 0;

    config_use_progressbar = FALSE;
    if (CONFIG_USE_PROGRESSBAR == 1){
        config_use_progressbar = TRUE;
    }

// Setup debug mode.
// Enable the usage of the serial debug.
// It is not initialized yet.
// #see: debug.c
    disable_serial_debug();
    if (USE_SERIALDEBUG == 1){
        enable_serial_debug();
    }


//
// Config
//

// Config headless mode.
// In headless mode stdout sends data to the serial port.
    Initialization.headless_mode = FALSE;
    Initialization.printk_to_serial = FALSE;
    // ...

// Redirect printk to serial port?
// It affects printk during all the time.
    if (CONFIG_PRINTK_TO_SERIAL == 1){
        Initialization.printk_to_serial = TRUE;
    }
// Headless mode?
    if (CONFIG_HEADLESS_MODE == 1)
    {
        Initialization.headless_mode = TRUE;
        Initialization.printk_to_serial = TRUE;
        // ...
    }

// =============================================
// Input authority
// Who will be able to setup the current foreground thread.
// Funtamental for the input system.
// First initialization.
// The main change is when the display server is registered.
// The first change is when the initialization 
// launches the init process.
// See: net.c

    InputAuthority.used = TRUE;
    InputAuthority.magic = 1234;
    InputAuthority.current_authority = AUTH_KERNEL;
    InputAuthority.initialized = TRUE;

// =============================================

    // #hack
    current_arch = CURRENT_ARCH_X86_64;
    //current_arch = (int) arch_type;


// I_kmain is called only for the BSP. So it needs to be number 0.
    Status = (int) I_initialize_kernel(arch_type, ProcessorNumber);
    if (Status == FALSE){
        PROGRESS("on I_initialize_kernel()\n");
    }
    if (system_state == SYSTEM_ABORTED){
        PROGRESS("SYSTEM_ABORTED\n");
    }

// Die
    PROGRESS("Initialization fail\n");
    x_panic("Error: 0x02");
    die();
// Not reached
    while (1){
        asm ("cli");
        asm ("hlt");
    };
}

// First function called by all the AP processors.
void AP_kmain(void)
{

    //
    // #todo
    //

// Not reached
    while (1){
        asm ("cli");
        asm ("hlt");
    };
}

//
// End
//

