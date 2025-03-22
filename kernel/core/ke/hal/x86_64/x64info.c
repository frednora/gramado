
// x64info.c
// handle x64 cpu informations.

#include <kernel.h>

static void __show_cpu_intel_parameters(void);

// =====================================================

/*
 * __show_cpu_intel_parameters:
 *     Mostra os parametros da CPU intel.
 * #todo: 
 *     Colocar em janela. 
 *     Criar funções: TerminalShowCPUIntelParameters()
 *                    ShowCPUIntelParameters()
 */
void __show_cpu_intel_parameters(void)
{
    printk("\n");

// #todo
// Check the pointer validation?

    if ((void*) processor == NULL)
        panic("show_cpu_intel_parameters: processor\n");

// Vendor and brand.
    printk("        Vendor: {%s}\n", &processor->Vendor[0] );
    printk("           Cpu: {%s}\n", &processor->BrandName[0] );

// HV - Hypervisor
// #todo: pegamos o vhName apenas para AMD, precisamos
// fazer o mesmo para intel.

    printk("             HV: {%s}\n", &processor->hvName[0] );

    printk("       Stepping: {%d}\n", 
        (unsigned long) processor->Stepping_ID );
    printk("          Model: {%d}\n", 
        (unsigned long) processor->Model );
    printk("         Family: {%d}\n", 
        (unsigned long) processor->Family_ID );
    printk(" Processor Type: {%d}\n", 
        (unsigned long) processor->Processor_Type );
    printk(" Extended Model: {%d}\n", 
        (unsigned long) processor->Extended_Model_ID );
    printk("Extended Family: {%d}\n", 
        (unsigned long) processor->Extended_Family_ID );
    //printk("ApicSupport={%x}\n", processor->isApic);
    printk("    Max feature: {%d}\n", 
        (unsigned long) processor->MaxFeatureId );
    //Bits 0-7: Cache Line Size.
    printk("   L2 line size: {%d Byte}\n", 
        (unsigned long) processor->L2LineSize ); 

// L2 Associativity. 
// Bits 12-15: L2 Associativity.
   
    unsigned long L2Associativity = processor->L2Associativity;

    switch (L2Associativity){
    case 0x00:  printk ("L2 Associativity: {Disabled}\n");           break; 
    case 0x01:  printk ("L2 Associativity: {Direct mapped}\n");      break; 
    case 0x02:  printk ("L2 Associativity: {2-way associative}\n");  break; 
    case 0x04:  printk ("L2 Associativity: {4-way associative}\n");  break; 
    case 0x06:  printk ("L2 Associativity: {8-way associative}\n");  break; 
    case 0x08:  printk ("L2 Associativity: {16-way associative}\n"); break; 
    case 0x0F:  printk ("L2 Associativity: {Fully associative}\n");  break; 
    };

//
// Cache
//

//Bits 16-31: Cache size in 1K units.
    printk("    L2 cache size: {%d KB}\n", 
        (unsigned long) processor->L2Cachesize ); 

//
// Physical and Virtual Address.
//

// Provavelmente é o número máximo de bits na arquitetura.
// tanto em memória física como virtual.
// 36 ou 39 indica memória extendida. normal é 32=(4GB).
// maximum physical address bits  
//maximum linear (virtual) address bits 

    printk("          PA Lim: {%d}\n", 
        (unsigned long) processor->Physical_Address_Size );
    printk("          VA Lim: {%d}\n", 
        (unsigned long) processor->Virtual_Address_Size );
    
    //printk("     Memory Size: {%d}\n",(unsigned long) processor->MemorySize);

// apic
    if (processor->hasAPIC == TRUE){
        printk("It has APIC\n");
    }
    if (processor->hasAPIC != TRUE){
        printk("No APIC!\n");
    }

// x87
    if (processor->hasX87FPU == TRUE){
        printk("It has a x87 FPU\n");
    }
    if (processor->hasX87FPU != TRUE){
        printk("No x87 FPU!\n");
    }

// HTT
    if (processor->hasHTT == TRUE){
        printk("It has a HTT\n");
    }
    if (processor->hasHTT != TRUE){
        printk("No HTT!\n");
    }

// VMX
    if (processor->hasVMX == TRUE){
        printk("It has VMX\n");
    }
    if (processor->hasVMX != TRUE){
        printk("No VMX!\n");
    }

// LAPIC
    if(LAPIC.initialized == TRUE){
        printk("LAPIC.lapic_pa %x \n",LAPIC.lapic_pa);
        printk("LAPIC.lapic_va %x \n",LAPIC.lapic_va);
        printk("LAPIC.local_id %d \n",LAPIC.local_id);
    }else{
        printk("[ERROR] LAPIC not initialized\n");
    };

// IOAPIC
    if(IOAPIC.initialized == TRUE){
        printk("[IOAPIC initialized\n");
    }else{
        printk("[ERROR] IOAPIC not initialized\n");
    };

// smp_info
    if(smp_info.initialized == TRUE){
        printk("[smp_info initialized\n");
    }else{
        printk("[ERROR] smp_info not initialized\n");
    };

    // Continua ...

// Show
// #obs:
// Como não usamos janelas no kernel, 
// então devemos dar refresh na tela todo por enquanto.

    //refresh_screen();
}

void x64_info(void)
{
    printk("\n");
    printk("x64 CPU Info:\n");
    printk("\n");
    printk("Number of processors: {%d}\n", g_processor_count);
    __show_cpu_intel_parameters();
}

//
// End
//
