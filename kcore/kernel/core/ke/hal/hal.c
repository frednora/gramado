// hal.c
// HAL interface. Architecture-independent.
// >> This is a good place for 'detection' routines.
// Created by Fred Nora.

#include <kernel.h>    


//
// import
//

//extern unsigned long gdt;
//extern unsigned long idt;
//extern unsigned long tss;
//extern void asm_reboot(void);

// hw interrupt breakers.
// see: 
// breaker.h 
// x86 - hw.asm
// ...

// timer interrupt
// kdrivers/timer.c
int __breaker_timer_initialized=0;

// ps2 keyboard
// x/i8042/ps2kbd.c
int __breaker_ps2keyboard_initialized=0;

// Serial ports.
// See: kdrivers/serial.c
int __breaker_com1_initialized=0;
int __breaker_com2_initialized=0;
int __breaker_com3_initialized=0;
int __breaker_com4_initialized=0;

// real time clock
// kdrivers/rtc.c
int __breaker_rtc_initialized=0;

// ps2 mouse
// x/i8042/ps2mouse.c
int __breaker_ps2mouse_initialized=0;

// ata
// kdrivers/ide/ata.c
int __breaker_ata1_initialized=0;
int __breaker_ata2_initialized=0;

//...


//
// see: pints.h
//

// #todo:
// Create a structure.


//See:  gde_serv.c 
unsigned long g_profiler_ints_gde_services=0;


//
// Legacy hardware interrupts (irqs) (legacy pic)
//

unsigned long g_profiler_ints_irq0=0;  // kdrivers/timer.c
unsigned long g_profiler_ints_irq1=0;  // x/i8042/keyboard.c
unsigned long g_profiler_ints_irq2=0;  // cascade.
unsigned long g_profiler_ints_irq3=0;  //
unsigned long g_profiler_ints_irq4=0;  //
unsigned long g_profiler_ints_irq5=0;  //
unsigned long g_profiler_ints_irq6=0;  //
unsigned long g_profiler_ints_irq7=0;  //

unsigned long g_profiler_ints_irq8=0;   // kdrivers/rtc.c
unsigned long g_profiler_ints_irq9=0;   // kdrivers/network/nicintel.c
unsigned long g_profiler_ints_irq10=0;  //
unsigned long g_profiler_ints_irq11=0;  //
unsigned long g_profiler_ints_irq12=0;  // x/i8042/mouse.c
unsigned long g_profiler_ints_irq13=0;  //
unsigned long g_profiler_ints_irq14=0;  // kdrivers/kdrivers/ide/atairq.c
unsigned long g_profiler_ints_irq15=0;  // kdrivers/kdrivers/ide/atairq.c


// not used?
// processors count
int processors_count=0;
// see: hal.h
unsigned long g_machine_type=0;
// main processor support.
// See: cpu.h
struct UPProcessorBlock_d  UPProcessorBlock;
struct processor_d  *processor;


void hal_io_delay(void)
{
    io_delay();
}

unsigned long hal_get_date(void)
{
    return (unsigned long) rtc_get_date();
}

unsigned long hal_get_time(void)
{
    return (unsigned long)  rtc_get_time();
}


// see: bldisp.c
void hal_refresh_screen(void)
{
    bldisp_flush(0);
}


// Monitor vertical sync.
// See: vsync.c
void hal_vsync(void)
{
    bldisp_vsync();
}


void hal_set_machine_type(unsigned long type)
{
    g_machine_type = (unsigned long) type;
}

unsigned long hal_get_machine_type (void)
{
    return (unsigned long) g_machine_type;
}


// hal_reboot:
// O hal é a camada mis próxima do hardware, 
// não há tratamento para fazer, somente chamar o reboot via teclado. 

void hal_reboot(void)
{
    debug_print("hal_reboot:\n");

    switch (system_state ){
    // Normal situations
    case SYSTEM_BOOTING:
    case SYSTEM_REBOOT:
        debug_print("...\n");
        break;
    default:
        debug_print("Booting on an unexpected situation\n");
        break;
    };

    debug_print("Calling archhal_reboot\n");
    archhal_reboot();

    x_panic("hal_reboot:\n");
}    

/*
 * hal_shutdown: 
 *     Interface usada pelos outros módulos
 *     #todo: Todos os possiveis métodos ficarão nessa rotina.
 *     máquina real e máquina virtual.
 *     #importante: Essa será a única função em hal para chamar shutdown.
 * quanquer outra interfce deve ficar fora do módulo hal.
 APM:
 ===
 This is the basic sequence of APM commands that must be given in 
 order to shut down a computer.
 +Perform an installation check.
 +Disconnect any existing APM interface.
 +Connect the Real Mode interface.
 +Enable power management for all devices.
 +Set the power state for all devices to "Off" (03h).
 ACPI:
 ====
  The ACPI shutdown is technically a really simple thing all 
  that is needed is a 
  outw(PM1a_CNT, SLP_TYPa | SLP_EN ); 
 and the computer is powered off. 
 The problem lies in the gathering of these values 
 especialy since the SLP_TYPa is in the \_S5 object 
 which is in the DSDT and therefore AML encoded.
 */

void hal_shutdown (void)
{
    const char *shutdown_str;

    debug_print ("hal_shutdown:\n");

// Something is wrong.
// We need to be in the right system state.

    if ( system_state != SYSTEM_POWEROFF ){
        panic ("hal_shutdown: system_state\n");
    }

    /*
    if (qemu____){
        //Bochs/QEMU poweroff
        shutdown_str = "Shutdown";
        while (*shutdown_str) out8 (0x8900, *(shutdown_str++));
    }
    */

    x_panic ("hal_shutdown");
}


// Speaker ON. 
// OUT 
// Play sound using built in speaker
void hal_speaker_on(void)
{
// Play the sound using the PC speaker.

    archhal_speaker_on();
}

// Speaker OFF
// IN
void hal_speaker_off (void)
{
// Make it shutup.

    archhal_speaker_off();
}
 
// Testing speaker.
// Beep.
void hal_test_speaker(void)
{
    int i=0;

    debug_print ("Testing speaker ...\n");
// beep, wait and stop.
    hal_speaker_on();
    for (i=0; i<80000; i++)
    {
    };
    hal_speaker_off();
} 

/*
 * hal_init_cpu:
 *     Inicializa a estrutura do processador e as coisas associadas a ele.
 *     Inicializa apenas o que for independente da arquitetura.
 *     @todo: Essa rotina pode ir para outro modulo do /hal. como cpu.c
 */
// #todo 
// O retorno deve ser int, para refletir o status.
    
void hal_init_cpu (void)
{
    debug_print("hal_init_cpu: deprecated\n");
    // See:
    // init() on init.c
    // hal_probe_processor_type() on detect.c
}

/*
 * hal_hardware_detect:
 *     Detecta fabricantes específicos suportados pelo núcleo.
 * 8086, 1237  //PCI & Memory.
 * 8086, 7000  //PIIX3 PCI-to-ISA Bridge (Triton II).
 * 1022, 2000  //Advanced Micro Devices, PCnet LANCE PCI Ethernet Controller.
 * 8086, 7113  //PIIX4/4E/4M Power Management Controller.
 * 8086, 2829  //Intel(R) ICH8M SATA AHCI Controller.
 * //...
 *
 */
// Consumer Chipsets (Z87, H87, H81) Haswell LGA1150. 
// Business Chipsets (Q87, Q85, B85) Haswell LGA1150.

int hal_hardware_detect (void)
{
    debug_print ("hal_hardware_detect: [TODO]\n");
    return 0;    //#todo
}

/*
 * halInitialize:
 * Initialize kernel base hal.
 */
// OUT: TRUE if its ok.
int halInitialize(void)
{
// Called by I_initKernelComponents() in x64init.c

    int Status = FALSE;
    unsigned char ProcessorType=0;


//
// HAL <<<<<<
//

// =========================================
// ::(5)(3)(9)
// Initialize processor information.
// + 'processor' structuture initialization.
// + Probing processor type.
// + Initialize fpu and smp support.
// + Detect the hypervisor.

    //PROGRESS("processor, fpu, smp, hv\n"); 

// --------
// 'processor' structuture initialization.
    processor = (void *) kmalloc( sizeof(struct processor_d) ); 
    if ((void *) processor == NULL){
        printk("halInitialize: processor\n");
        return FALSE;
    }
    memset( processor, 0, sizeof(struct processor_d) );
// Validate the structure
// This is a valid structure,
// but it's not initialized yet.
    processor->objectType = ObjectTypeProcessor;
    processor->objectClass = ObjectClassKernelObject;
    processor->used = TRUE;
    processor->magic = 1234;

// --------
// Probing processor type.
// #todo
// Check if cpuid instruction is available.
// See: _x86_test_cpuid_support on bootmx/headlib.asm
// #todo: extern int x86_test_cpuid_support(void);
// Sonda pra ver qual é a marca do processador.
// #todo: 
// É a segunda vez que fazemos a sondagem ?!
// See: hal/detect.c
// This routine is valid for intel and amd processors.
// Ok.
// Let's make some initialization and 
// get more information about the processor
// using the cpuid instruction.
// See: 
// detect.c
// x86.c
// cpuamd.c

    ProcessorType = (int) hal_probe_processor_type();
    processor->Type = (int) ProcessorType;

// --------
// Initialize fpu and smp support.

    int fpu_status = -1;     // fail
    int smp_status = FALSE;  // fail

    switch (ProcessorType){

    // INTEL:
    // + Get processor information.
    // + Initialize fpu/see support.
    case Processor_INTEL:
    case Processor_AMD:
        // Get processor information.
        x64_init_intel();
        //init_amd(); 
        //Initialize fpu/see support.
        fpu_status = (int) x64_init_fpu_support();
        if (fpu_status<0){
            printk("halInitialize: [FAIL] FPU Initialization fail\n");
            return FALSE;
        }

        // --------
        // Detect the hypervisor.
        // Saved into the processor data structure.
        // Save the option found into a global variable.
        // #todo: We need a structure for that thing.
        // see: virt/hv.c
        // Essa rotina eh valida para AMD e Intel.
        //int hv_return = -1;
        hv_probe_info();

        //#breakpoint
        //printk("#breakpoint in I_init()\n");
        //refresh_screen();
        //while(1){}

        break;
    // ...
    default:
        printk ("halInitialize: [ERROR] default ProcessorType\n");
        return FALSE;
        break;
    };


// #todo:
// Chamaremos essa inicialização básica nesse momento.
// A inicialização completa será chamada pelo processo init.
    early_timer_init();

// Detecta fabricantes específicos suportados pelo núcleo.  
    hal_hardware_detect();

// Initializat RTC device driver.
// hal
    DDINIT_rtc();


// ================================
// DANGER !!!
// :::: GDT ::::
// Setup GDT again.
// We already made this at kernel startup.
// # Caution.
// Lets create a TSS and setup a GDT.
// This way we can use 'current_tss' when we create threads.
// This function creates a TSS and sets up a GDT.
// #todo
// Depois de renovarmos a GDT precisamos
// recarregar os registradores de segmento?
// See: hal/arch/x86/x86.c
// #bugbug
// see: x64.c

    //PROGRESS(":: GDT\n"); 
    x64_init_gdt();

    return TRUE;

fail:
    return FALSE;
}

