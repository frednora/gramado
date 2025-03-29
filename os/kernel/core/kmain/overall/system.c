// system.c
// Created by Fred Nora.

#include <kernel.h>

// global
unsigned long sys_time_hz=0;
static char *default_user_name = DEFAULT_USERNAME;

static void __a_soft_place_to_fall(void);

// ====================================

static void __a_soft_place_to_fall(void)
{
    while (TRUE)
    { 
        asm ("cli");
        //asm ("mwait");   
        asm ("hlt");
    };
}

void soft_die(void)
{
    __a_soft_place_to_fall();
    soft_die();
}

/*
 * doGetSystemMetrics:
 *     Retorna informações sobre o sistema.
 *     #todo: 
 *     Criam um enum para essa função, aqui mesmo nesse arquivo.
 */
unsigned long doGetSystemMetrics(int index)
{
// Called by sys_get_system_metrics() in sys.c

    // unsigned long ul_value=0;

    // We are trying to have more system calls. sc80 sc81 sc82.
    // These system calls uses different set of selectors.

    //print ("#debug: doGetSystemMetrics: i={%d} \n",index)

    if (index <= 0){
        return (unsigned long) 0;
    }

    switch (index){

        //screen width.
        case 1:
            return (unsigned long) screenGetWidth();
            break;

        //screen height.
        case 2:
            return (unsigned long) screenGetHeight();
            break;

        //cursor width in pixels.
        // #todo: We need a mehtod for this.
        case 3:
            return (unsigned long) CONSOLE_TTYS[fg_console].cursor_width_in_pixels;
            break;

        //cursor height in pixels.
        // #todo: We need a mehtod for this.
        case 4:
            return (unsigned long) CONSOLE_TTYS[fg_console].cursor_height_in_pixels;
            break;

        // mouse pointer width.
        // #todo: We need a mehtod for this.
        case 5:
            return (unsigned long) g_mousepointer_width;
            break;

        // mouse pointer height.
        // #todo: We need a mehtod for this.
        case 6:
            return (unsigned long) g_mousepointer_height;
            break;

        // char width.
        // #bugbug: The method is returning an int.
        case 7:
            return (unsigned long) get_char_width();
            break;

        // char height.
        // #bugbug: The method is returning an int.
        case 8:
            return (unsigned long) get_char_height();
            break;

        // Bits Per Pixel.
        // #todo: We need a method for this.
        case 9:
            return (unsigned long) gSavedBPP;
            break;

        // screen buffers

        // lfb pa
        // #todo: We need a method for this.
        case 10:
            return (unsigned long) g_frontbuffer_pa;
            break;

        // lfb va
        // #todo: We need a method for this.
        case 11:
            return (unsigned long) g_frontbuffer_va;
            break;

        // main backbuffer va
        // #todo: We need a method for this.
        case 12:
            return (unsigned long) g_backbuffer_va;
            break;

        //20
        // mouse x    
        case SM_GETMOUSEX:
            //#bugbug: Pegando informações com o driver de mouse.
            //return (unsigned long) ps2_mouse_get_info (3); 
            panic ("doGetSystemMetrics: SM_GETMOUSEX #todo \n");
            break;

        //21
        //mouse y
        case SM_GETMOUSEY:
           //#bugbug: Pegando informações com o driver de mouse.
            //return (unsigned long) ps2_mouse_get_info (4); 
            panic ("doGetSystemMetrics: SM_GETMOUSEY #todo\n");
            break;

        // Ram support

        // base memory
        // #todo: We need a method for this.
        case 30:
            return (unsigned long) memorysizeBaseMemory;
            break;
            
        // extended - base
        // #todo: We need a method for this.
        case 31:
            return (unsigned long) memorysizeOtherMemory;
            break;

        // extended memory
        // #todo: We need a method for this.
        case 32:
            return (unsigned long) memorysizeExtendedMemory;
            break;

        // total = base + other + extended 
        // #todo: We need a method for this.
        case 33:
            return (unsigned long) memorysizeTotal;
            break;

        // used
        // #todo: We need a method for this.
        case 34:
            return (unsigned long) memorysizeUsed;
            break;

        // free
        // #todo: We need a method for this. 
        case 35:
            return (unsigned long) memorysizeFree;
            break;  

         // Kernel Heap support.

        // #todo: We need a method for this.
        case 40:
            return (unsigned long) kernel_heap_start;
            break;

        // #todo: We need a method for this.
        case 41:
            return (unsigned long) kernel_heap_end;
            break;

        // #todo: We need a method for this.
        case 42:
            return (unsigned long) g_available_heap;
            break;

        // Kernel Stack support.

        // #todo: We need a method for this.
        case 50:
            return (unsigned long) kernel_stack_start;
            break;

        // #todo: We need a method for this.
        case 51:
            return (unsigned long) kernel_stack_end;
            break;

        // #todo: We need a method for this.
        case 52:
            return (unsigned long) kernel_stack_start_pa;
            break; 

        // Used memory support

        // #todo: We need a method for this.
        case 60:
            return (unsigned long) mm_used_ring0_area;
            break;

        // #todo: We need a method for this.
        case 61:
            return (unsigned long) mm_used_ring3_area;
            break;

        // #todo: We need a method for this.
        case 62:
            return (unsigned long) mm_used_backbuffer;
            break;

        // #todo: We need a method for this.
        case 63:
            return (unsigned long) mm_used_pagedpool;
            break;

        // #todo: We need a method for this.
        case 64:
            return (unsigned long) mm_used_heappool;
            break;

        // #todo: We need a method for this.
        case 65:
            return (unsigned long) mm_used_extraheap1;
            break;

        // #todo: We need a method for this.
        case 66:
            return (unsigned long) mm_used_extraheap2 ;
            break;

        // #todo: We need a method for this.
        case 67:
            return (unsigned long) mm_used_extraheap3;
            break;

        // Dispatch critéria.

        // #todo: We need a method for this.
        case 70:
            return (unsigned long) DispatchCountBlock->SelectIdleCount;
            break;

        // #todo: We need a method for this.
        case 71:
            return (unsigned long) DispatchCountBlock->SelectInitializedCount;
            break;

        // #todo: We need a method for this.
        case 72:
            return (unsigned long) DispatchCountBlock->SelectNextCount;
            break;

        // #todo: We need a method for this.
        case 73:
            return (unsigned long) DispatchCountBlock->SelectCurrentCount;
            break;

        // #todo: We need a method for this.
        case 74:
            return (unsigned long) DispatchCountBlock->SelectAnyCount;
            break;

        // #todo: We need a method for this.
        case 75:
            return (unsigned long) DispatchCountBlock->SelectIdealCount;
            break;

        // #todo: We need a method for this.
        case 76:
            return (unsigned long) DispatchCountBlock->SelectDispatcherQueueCount;
            break;

        // System type by memory size;
        // #todo: We need a method for this.
        case 90:
            return (unsigned long) g_mm_system_type;
            break;

        // Profiler ... (reservar vários números.)

        // #todo: We need a method for this.
        case 100:
            return (unsigned long) g_profiler_ints_irq0;
            break;

        // #todo: We need a method for this.
        case 101:
            return (unsigned long) g_profiler_ints_irq1;
            break;

        // #todo: We need a method for this.
        case 102:
            return (unsigned long) g_profiler_ints_irq2;
            break; 

        // #todo: We need a method for this.
        case 103:
            return (unsigned long) g_profiler_ints_irq3;
            break;

        // #todo: We need a method for this.
        case 104:
            return (unsigned long) g_profiler_ints_irq4;
            break;

        // #todo: We need a method for this.
        case 105:
            return (unsigned long) g_profiler_ints_irq5;
            break;

        // #todo: We need a method for this.
        case 106:
            return (unsigned long) g_profiler_ints_irq6;
            break;              

        // #todo: We need a method for this.
        case 107:
            return (unsigned long) g_profiler_ints_irq7;
            break;

        // #todo: We need a method for this.
        case 108:
            return (unsigned long) g_profiler_ints_irq8;
            break;

        // #todo: We need a method for this.
        case 109:
            return (unsigned long) g_profiler_ints_irq9;
            break;

        // #todo: We need a method for this.
        case 110:
            return (unsigned long) g_profiler_ints_irq10;
            break;

        // #todo: We need a method for this.
        case 111:
            return (unsigned long) g_profiler_ints_irq11;
            break;

        // #todo: We need a method for this.
        case 112:
            return (unsigned long) g_profiler_ints_irq12;
            break;

        // #todo: We need a method for this.
        case 113:
            return (unsigned long) g_profiler_ints_irq13;
            break;

        // #todo: We need a method for this.
        case 114:
            return (unsigned long) g_profiler_ints_irq14;
            break;

        // #todo: We need a method for this.
        case 115:
            return (unsigned long) g_profiler_ints_irq15;
            break;

        // Interrupção de serviços do sistema.
        // #todo: We need a method for this.
        case 117:
            return (unsigned long) g_profiler_ints_gde_services;
            break; 

       // PIT
       // See: hal/pit.h

       // jiffies.
       // total ticks
       // #todo: We need a method for this.
       case 118:
           return (unsigned long) jiffies;
           break;

       // seconds
       // por quantos segundos o sistema esta rodando
       // jiffies/sys_time_hz
       // #todo: We need a method for this.
       case 119:
           return (unsigned long) seconds;
           break;

       // Por quantos ms o sistema esta rodando.
       // #todo: We need a method for this.
       case 120:
           return (unsigned long) sys_time_ms;
           break;

       // pit frequency
       // #todo: We need a method for this.
       case 121:
           return (unsigned long) sys_time_hz;
           break;

        // ...

       // gramado mode, gamemode.
       // jail, p1, home ...
       // #todo: We need a method for this.
       case 130:
           return (unsigned long) current_mode;
           break;

        // When a thread is running to call this service,
        // so, this thread is the current thread.
        // But, current for what processor?
        case 140:  
            //return (unsigned long) current_process;
            return (unsigned long) get_current_process(); //#bugbug mask?  
            break;
       
        // #todo: We need a method for this.
        case 141:  
            return (unsigned long) current_thread;   
            break;

        // return the number of processors.
        // #todo: We need a method for this.
        case 150:  
            return (unsigned long) (g_processor_count & 0xFFFFFFFF);
            break;
 
        //case 151:
            //break;

        // 200 ~ 2xx window server info.
        // See: ws.h

        // #bugbug
        // Quando estamos dentro do kernel por entrarmos via interrupçao de teclado,
        // esses valores estao certos,
        // mas quando estamos dentro do kernel, por entrarmos via interrupçao de sistema,
        // esses valores estao errados.
        // #porque
        // ?? seria algum problema de selecao de diretorio de paginas,
        // ou de configuraçao de registrador de segmento.
        // podemos checar se a configuraçao dos segmentos de dados esta
        // certa na hora da entrada da interrupçao de sistema.
        // o problema eh que mudarmos o registrador de segmento, entao
        // o kernel nao podera mais pegar as informaçoes que estao em ring3. certo??
        // See:
        // sw.asm

        // #todo: We need a method for this.
        // #todo: We need a structure for that.
        // and the name needs to be DisplayServer.xxx.
        case 200:
           //printk ("#type: %d\n",WindowServer_type);
           //refresh_screen();
           return (unsigned long) WindowServer_type;
           break;

        // #todo: We need a method for this.
        // #todo: We need a structure for that.
        // and the name needs to be DisplayServer.xxx.
        case 201:
           //printk ("#pid: %d\n",WindowServer_pid);
           //refresh_screen();
           return (unsigned long) WindowServer_pid; 
           break;

        // #todo: We need a method for this.
        // #todo: We need a structure for that.
        // and the name needs to be DisplayServer.xxx.
        case 202:
           //printk ("#virtual_console: %d\n",WindowServer_virtual_console);
           //refresh_screen();
           // #todo
           // What is this ??
           return (unsigned long) WindowServer_virtual_console; 
           break;

        // #todo: We need a method for this.
        // #todo: We need a structure for that.
        // and the name needs to be DisplayServer.xxx.
        case 203:
           //printk ("#initialized: %d\n",WindowServer_initialized);
           //refresh_screen();
           return (unsigned long) WindowServer_initialized; 
           break;

        // Are we running on Qemu hv?
        case 300:
            return (unsigned long) keIsQemu();
            break;

        case 400:
            // console_get_current_virtual_console();
            return (unsigned long) fg_console;
            break;

        // ...

        default:
            goto done;
            break;
    };

done:
    return (unsigned long) 0;
}

// The kernel hangs.
void die(void)
{
    if (Initialization.is_serial_log_initialized == TRUE){
        PROGRESS("die:\n");
    }
    if (Initialization.is_console_log_initialized == TRUE){
        printk("die: System Halted\n");
        refresh_screen();
    }
    __a_soft_place_to_fall();
    die();
}

// Implementation of a safe reboot routine.
// # We need to return when 
// a non-superuser process call this service.
int do_reboot(unsigned long flags)
{

    //int value = FALSE;

// #todo
// Is it the superuser?
// We only trust in superusser for this call.

    debug_print("do_reboot:\n");

/*
    value = (int) is_superuser();
    if(value != TRUE){
        return (-EPERM);
    }
*/

// #todo
// Use MAGIC arguments.

// FAT cache.
// This is the FAT cache for the system disk.
// The boot partition.
    int Status = -1;
    Status = (int) fs_save_fat16_cache();
    if ( Status < 0 || 
         g_fat_cache_saved != FAT_CACHE_SAVED )
    {
        debug_print("do_reboot: Can't reboot without saving the fat cache\n");
        goto fail;
    }

// Reboot
    debug_print("do_reboot: Rebooting\n");
    hal_reboot();
    panic("do_reboot: Unexpected error\n");
fail:
    return (int) -1;
}



/*
 * doSetupVersion:
 *     Setup version info.     
 */
// See:
// config/version.h
// config/u.h
// system.h
void doSetupVersion(void)
{

// Global structure
// #todo:
// Isso deve ser considerado um erro fatal,
// pois existem aplicações que dependem da versão do sistema 
// para funcionarem corretamente.. 

    Version = (void *) kmalloc( sizeof(struct version_d) );
    if ((void *) Version == NULL){
        panic("doSetupVersion: Version\n");
    }
    Version->Major = VERSION_MAJOR;
    Version->Minor = VERSION_MINOR;
    Version->Build = VERSION_BUILD;

// Global structure
// #todo:
// Isso deve ser considerado um erro fatal,
// pois existem aplicações que dependem da versão do sistema 
// para funcionarem corretamente.. 

    VersionInfo = (void *) kmalloc( sizeof(struct version_info_d) );
    if ((void *) VersionInfo == NULL){
        panic ("doSetupVersion: VersionInfo\n");
    }

// Global structure
// Colocando na estrutura System se ela for válida.

    if ((void *) System == NULL){
        panic ("doSetupVersion: System\n");
    }
    if ((void *) System != NULL)
    {
        if ( System->used == TRUE && System->magic == 1234 )
        {
            System->version      = (void *) Version;
            System->version_info = (void *) VersionInfo;
        }
        //Nothing
    }
}

/*
 * zeroInitializeSystemComponents:
 *     Initialize the kernel executive.
 *     Architecture (independent) inicialization. 
 */
// called by I_Init().
// Probe PCI devices.
// see: /dev/pci/
// Initialize rtc support.
// see: /hal/x86_64/rtc.c 
// Initialize ATA support.
// #importante: 
// Só depois de inicializarmos o ata é que 
// podemos carregar algum arquivo.
// #importante
// Nessa hora ja temos as estruturas de disk e volume inicializadas.
// entao as estruturas usadas pelo driver ata, pode
// se registrar em disk ou volume.
// see: /dev/ata/atainit.c
// OUT: return TRUE if its ok.

int zeroInitializeSystemComponents(void)
{
    // #deprecated

    debug_print ("zeroInitializeSystemComponents:\n");
    return TRUE;
}

