// acpi.c
// ACPI support.
// Created by Fred Nora.

// #test
// The routines are running on Qemu.
// The routines are running on virtualbox.

#include <kernel.h>


// see: acpi.h
struct rsdp_d *rsdp;
struct xsdp_d *xsdp;
struct rsdt_d *rsdt;
struct xsdt_d *xsdt;

struct FADT_d *fadt;

static struct FADT_d *__brute_force_probe_fadt(unsigned long base_address);
static int __brute_force_probe_xsdt(unsigned long base_addr);
static int __acpi_rsdp(unsigned long rsdp_pointer);

// =============================================

/*
In Bochs, the RSDT typically includes pointers to:
MADT (Multiple APIC Description Table) – Defines CPU cores and APIC configurations for SMP.
FADT (Fixed ACPI Description Table) – Contains power management and system control details.
DSDT (Differentiated System Description Table) – Holds ACPI-defined device configurations.
SSDT (Secondary System Description Table) – Additional ACPI definitions for devices.
HPET (High Precision Event Timer Table) – Provides high-resolution timing support.
*/

// =============================================

int acpi_check_header(unsigned int *ptr, char *sig)
{
// Get four bytes from these addresses.
	unsigned int sig1 = *(unsigned int*) ptr;
	unsigned int sig2 = *(unsigned int*) sig;

   	if (sig1 == sig2){
   		return 0;
	}	
   	return (int) -1;
}

// Checks for a given header and validates checksum.
// Credits:
// https://forum.osdev.org/viewtopic.php?t=16990
int acpi_check_header02(unsigned int *ptr, char *sig)
{
    char *checkPtr = (char *) ptr;
    int len=0;
    char Sum=0;

    // Match
    if (kstrncmp(ptr, sig, 4) == 0)
    {
        len = *(ptr + 1);
        while (0 < len--)
        {
            Sum += *checkPtr;
            checkPtr++;
        };

        //OK, The sun is not '0'.
        if (Sum == 0)
            return 0;
    }

// Fail
   return (int) (-1);
}

static struct FADT_d *__brute_force_probe_fadt(unsigned long base_address)
{
    unsigned char *p = (unsigned char *)base_address;
    size_t scan_range = 0x1000; // Scan 4KB, adjust as needed
    size_t step = 16;           // ACPI tables are 16-byte aligned
    size_t i=0;

    for (i=0; i < scan_range; i += step) 
    {
        struct FADT_d *hdr = (struct FADT_d *)(p + i);
        if (hdr->h.Signature[0] == 'F' &&
            hdr->h.Signature[1] == 'A' &&
            hdr->h.Signature[2] == 'C' &&
            hdr->h.Signature[3] == 'P') 
        {
            //printk("__brute_force_probe_fadt: Found FACP at offset 0x%zx\n", i);
            printk("FACP was found\n");
            smp_info.fadt_found = TRUE;
            return hdr;
        }
    }

    printk("__brute_force_probe_fadt: Could not find FACP signature near base address\n");
    return NULL;
}

static int __brute_force_probe_xsdt(unsigned long base_addr)
{
    unsigned char *p = (unsigned char *)(base_addr - 256);
    int range = 512;  // Scan 256 bytes before and after
    int i=0;

    for (i=0; i < range; i += 16) 
    {
        if (p[i+0] == 'X' && p[i+1] == 'S' && p[i+2] == 'D' && p[i+3] == 'T') 
        {
            xsdt = (struct xsdt_d *)(p + i);

            //printk("Found XSDT at offset %d\n", i - 256);
            printk("XSDT found: %c\n", 
                xsdt->Signature[0] );
            //refresh_screen();
            //while(1){}

            // Now you can process xsdt as usual.
            return TRUE;
        }
    };

// fail:
    printk("Could not find XSDT signature near expected address\n");
    return FALSE;
}

#define BYTES_TO_READ 256       // Number of bytes to print
void __dump_memory(void *address, size_t size) 
{
    unsigned char *ptr = (unsigned char *)address;
    size_t i;
    for (i = 0; i < size; i++) 
    {
        printk("%c ", ptr[i]);
        if ((i + 1) % 16 == 0) 
            printk("\n"); // Format output nicely
    }
    printk("\n");
}

// Probe rsdp or xrdp.
static int __acpi_rsdp(unsigned long rsdp_pointer)
{

// TRUE when finding the RSDP table.
    int Status = FALSE;

// --------------------------------------------------------------
// To find the RSDT you need first to locate and check the RSDP, 
// then use the RsdtPointer for ACPI Version < 2.0 an XsdtPointer for any other case. 

// The root table.
    rsdp = (struct rsdp_d *) rsdp_pointer;  // 1.0
    xsdp = (struct xsdp_d *) rsdp_pointer;  // 2.0

// #debug: Signature OK.
    printk("RSDP signature: \n");
    printk ("%c %c %c %c \n",
        rsdp->Signature[0],
        rsdp->Signature[1],
        rsdp->Signature[2],
        rsdp->Signature[3] );
    printk ("%c %c %c %c \n",
        rsdp->Signature[4],
        rsdp->Signature[5],
        rsdp->Signature[6],
        rsdp->Signature[7] );

    printk("RSDP: OEMID {%s}\n", rsdp->OEMID);

    // #debug
    // #breakpoint
    //while(1){ asm("hlt"); }

    unsigned long __initial_acpi_address = 0;
    unsigned long __rsdt_address=0;
    unsigned long __xsdt_address=0;

// Mapping the rsdt address.
// Used in both revisions.
    unsigned long address_pa = 0;
    unsigned long address_va = 0;
    int mapStatus = -1;

    // #test
    // This is the initial address where we're gonna probe to find
    // rsdt signature.
    __initial_acpi_address = (unsigned long) (rsdp->RsdtAddress & 0xFFF00000); 

// ------------------------------------------------
// Revision 0x02: Use xsdp
    if (rsdp->Revision == 0x02){
        printk("ACPI: Revision 2.0 {0x02} \n");
        __rsdt_address = (unsigned long) (rsdp->RsdtAddress & 0xFFFFFFFF);
        // #bugbug
        // Here probably the physical address is 64bit long.
        __xsdt_address = (unsigned long) (xsdp->XsdtAddress & 0xFFFFFFFFFFFFFFFF);

        // Mapping the rsdt address.
        // See: x64gva.h
        address_pa = (unsigned long) __xsdt_address;         // 64bit address. right.
        address_va = (unsigned long) (XSDT_VA & 0xFFFFFFFF); // Desired va.

        if (address_pa == 0)
            panic("acpi.c: address_pa\n");

        // #debug
        printk("xsdt pa: %x | xsdt va: %x\n", address_pa, address_va );

        // IN: ps | va
        // OUT: 0=ok | -1=fail
        // see: pages.c
        mapStatus = 
            (int) mm_map_2mb_region(
                (unsigned long) address_pa,    // pa
                (unsigned long) address_va );  // va
        if (mapStatus != 0){
            panic("acpi.c: mm_map_2mb_region\n");
        }

        // #test
        asm ("movq %cr3, %rax");
        asm ("movq %rax, %cr3");

        xsdt = (struct xsdt_d *) (address_va & 0xFFFFFFFFFFFFFFFF);

        // ------------------------------------------
        // Dumping xsdt to check the validation of the mapping.
        //__dump_memory(address_va, BYTES_TO_READ);

        printk ("Signature? %c %c %c %c \n",
            xsdt->Signature[0],
            xsdt->Signature[1],
            xsdt->Signature[2],
            xsdt->Signature[3] );

        // ACPI version indicator (usually 1).
        printk("XSDT: Revision {%c}\n", xsdt->Revision);
        // OEMID[6] – Manufacturer identifier (e.g., "Intel" or "AMI").
        printk("XSDT: OEMID {%s}\n", xsdt->OEMID);
        refresh_screen();

        /*
        // failing
        if (kstrncmp(xsdt->Signature, "XSDT", 4) == 0)
        {
            printk("XSDT ok\n");
            refresh_screen();
            return TRUE;
        }
        if (kstrncmp(xsdt->Signature, "RSDT", 4) == 0) 
        {
            printk("RSDT ok\n");
            refresh_screen();
            return TRUE;
        }
        */


        //#hack
        //return TRUE;

        //#debug
        //refresh_screen();
        //while(1){}

        // ------------------------------------------
        printk("XSDT signature:\n");

        // If the signature is valid, let's print some fields.
        // #todo memcmp(xsdt->Signature, "XSDT", 4) == 0)
        if ( xsdt->Signature[0] == 'X' || 
             xsdt->Signature[0] == 'R' ){

            printk ("%c %c %c %c \n",
                xsdt->Signature[0],
                xsdt->Signature[1],
                xsdt->Signature[2],
                xsdt->Signature[3] );
    
            // ACPI version indicator (usually 1).
            printk("XSDT: Revision {%c}\n", xsdt->Revision);
            // OEMID[6] – Manufacturer identifier (e.g., "Intel" or "AMI").
            printk("XSDT: OEMID {%s}\n", xsdt->OEMID);
            //refresh_screen();
            //while(1){}

        } else {
            printk("XSDT signature: #bugbug Wrong signature\n");

            // #debug
            // Signature failed
            goto fail;
        };

        // #breakpoint
        //panic("x64smp.c: Revision 2.0 #todo\n");
        printk("ACPI: Revision 2.0 #todo\n");

        Status = TRUE;
        goto valid_revision;

// ------------------------------------------------
// Revision 0x00: Use rsdp
    } else if (rsdp->Revision == 0x00){
        printk("ACPI: Revision 1.0 {0x00} \n");
        __rsdt_address = (unsigned long) (rsdp->RsdtAddress & 0xFFFFFFFF);
        __xsdt_address = 0;
        //printk("ACPI: Revision 1.0 #todo\n");

        // Print the address we have.
        //printk("rsdt address: %x \n", __rsdt_address);
        //while(1){ asm("hlt"); }

        // In RSDP Revision 0 (ACPI 1.0), the pointer to the 
        // RSDT (Root System Description Table) is stored in the RsdtAddress 
        // field of the RSDP structure.

        // Mapping the rsdt address.
        // See: x64gva.h
        address_pa = (unsigned long) (__rsdt_address & 0xFFFFFFFF);
        address_va = (unsigned long) (RSDT_VA & 0xFFFFFFFF); // Desired va.

        if (address_pa == 0)
            panic("acpi.c: address_pa\n");

        // #debug
        printk("rsdt pa: %x | rsdt va: %x\n", address_pa, address_va );

        // IN: ps | va
        // OUT: 0=ok | -1=fail
        // see: pages.c
        mapStatus = 
            (int) mm_map_2mb_region(
                (unsigned long) address_pa,    // pa
                (unsigned long) address_va );  // va
        if (mapStatus != 0){
            panic("acpi.c: mm_map_2mb_region\n");
        }

        // #test
        asm ("movq %cr3, %rax");
        asm ("movq %rax, %cr3");
    

        // RSDT:

        // Now we have a valid pointer.
        // #bugbug: Or not. Because the signature is wrong.
        // RSDT helps the system locate other ACPI tables necessary for 
        // configuring processors and power management.
        rsdt = (struct rsdt_d *) (address_va & 0xFFFFFFFF);
        // #debug: Looking for "RSDT".
        // #bugbug: >>>>>>> [FAIL]
        // We cant find a valid signature.
        // Probably out mapping routine is not working well 
        // and giving us a wrong va.

        printk("RSDT signature:\n");

        // If the signature is valid, let's print some fields.
        // #todo memcmp(rsdt->Signature, "RSDT", 4) == 0)
        if (rsdt->Signature[0] == 'R'){
            printk ("%c %c %c %c \n",
                rsdt->Signature[0],
                rsdt->Signature[1],
                rsdt->Signature[2],
                rsdt->Signature[3] );
    
            // ACPI version indicator (usually 1).
            printk("RSDT: Revision {%c}\n", rsdt->Revision);
            // OEMID[6] – Manufacturer identifier (e.g., "Intel" or "AMI").
            printk("RSDT: OEMID {%s}\n", rsdt->OEMID);
        } else {
            printk("RSDT signature: #bugbug Wrong signature\n");

            // #debug
            // Signature failed
            goto fail;
        };

        // #todo
        // Probably this address will fail, and than, 
        // the caller needs to probe for 'fadt' table manually.

        // #bugbug
        // We gotta find this table and check the elements.
        //printk("ACPI: Breakpoint\n");
        //refresh_screen();
        //while(1){}

        Status = TRUE;
        goto valid_revision;

// ------------------------------------------------
// Invalid Resivion:
    } else {
        printk("ACPI: Invalid ACPI revision\n");
        Status = FALSE;
        goto fail;
    };

    panic ("ACPI: Something is wrong\n");

// ------------------------------------------------
// Valid Revision: What is next?
// Multiple APIC Description Table (MADT)
// You should be able to find a MADT table in the RSDT table or in the XSDT table. 
// The table has a list of local-APICs, 
// https://wiki.osdev.org/MADT
// ...

valid_revision:
    if (Status == TRUE){
        return TRUE;
    }
fail:
    printk("ACPI: Failed probing rsdp info\n");
    refresh_screen();
    return FALSE;
}

//
// $
// VIA ACPI
//

// After you've gathered the information, 
// you'll need to disable the PIC and prepare for I/O APIC. 
// You also need to setup BSP's local APIC. 
// Then, startup the APs using SIPIs.
// You should be able to find a MADT table 
// in the RSDT table or in the XSDT table.
// #todo
// Probe the acpi table.
// + 'RSDP signature'
int acpi_probe(void)
{
// Called by x64smp_initialization() in x64smp.c.

// TRUE when finding the RSDP table.
    int Status = FALSE;

// 0x040E - The base address.
// Get a short value.
    unsigned short *bda = (unsigned short*) BDA_BASE;
    unsigned long ebda_address=0;
    register int i=0;
    unsigned char *p;
// Signature elements.
    unsigned char c1=0;
    unsigned char c2=0;
    unsigned char c3=0;
    unsigned char c4=0;
    unsigned char c5=0;
    unsigned char c6=0;
    unsigned char c7=0;
    unsigned char c8=0;

//
// Probe ebda address at bda base.
//

    printk("EBDA short Address: %x\n", bda[0] ); 
    ebda_address = (unsigned long) ( bda[0] << 4 );
    ebda_address = (unsigned long) ( ebda_address & 0xFFFFFFFF );
    printk("EBDA Address: %x\n", ebda_address ); 

// base
// between 0xF0000 and 0xFFFFF.
// #todo: filter
    p = ebda_address;

// The signature was found?
    static int Found = FALSE;


// Probe for the signature. // "RSD PTR "
    int max = (int) (0xFFFFF - ebda_address);
    for (i=0; i<max; i++){
        c1 = p[i+0];
        c2 = p[i+1];
        c3 = p[i+2];
        c4 = p[i+3];
        c5 = p[i+4];
        c6 = p[i+5];
        c7 = p[i+6];
        c8 = p[i+7];

        // "RSD PTR "
        if ( c1 == 'R' && 
             c2 == 'S' && 
             c3 == 'D' && 
             c4 == ' ' &&
             c5 == 'P' &&
             c6 == 'T' &&
             c7 == 'R' &&
             c8 == ' '  )
        {
            printk (":: Found [RSD PTR ] at index %d. :)\n",i);
            Found=TRUE;
            break;
        }
    };

// Signature not found.
    if (Found != TRUE){
        Status = FALSE;
        printk("acpi_probe: [RSD PTR ] wasn't found!\n");
        goto fail;
    }

// Get address
    unsigned long __rsdp_Pointer = (unsigned long) (ebda_address + i);

// Save the address
    smp_info.RSD_PTR_address = (unsigned long) __rsdp_Pointer;

    // Try the normal probe first.
    Status = (int) __acpi_rsdp(__rsdp_Pointer);
    if (Status != TRUE) 
    {
        printk("acpi_probe: failed on __acpi_rsdp()\n");
        goto fail;

        /*
        // If failed, try brute force probe.
        Status = (int) __brute_force_probe_xsdt(__rsdp_Pointer);
        if (Status != TRUE) 
        {
            printk("acpi_probe: failed on __brute_force_probe_xsdt()\n");

            fadt = (struct FADT_d *)__brute_force_probe_fadt(__rsdp_Pointer);
            // #debug
            printk("After __brute_force_probe_fadt()\n");
            refresh_screen();
            
            // Leats fail ... the caller will try MP instead.
            //while(1){}

            goto fail;
        }
        */
    }

    // #debug
    printk("rsdp ok\n");
    refresh_screen();

    unsigned long va = (unsigned long) xsdt;
    fadt = (struct FADT_d *)__brute_force_probe_fadt(va);

    // Lets print the signature
    printk("fadt signature: \n");
    printk ("%c %c %c %c \n",
        fadt->h.Signature[0],
        fadt->h.Signature[1],
        fadt->h.Signature[2],
        fadt->h.Signature[3] );

    refresh_screen();
    //while(1){}

    return TRUE;

fail:
    return FALSE;
}

//
// bytecode of the \_S5 object
// -----------------------------------------
//        | (optional) |    |    |    |   
// NameOP | \          | _  | S  | 5  | _
// 08     | 5A         | 5F | 53 | 35 | 5F
//
// -----------------------------------------------------------------------------------------------------------
//           |           |              | ( SLP_TYPa   ) | ( SLP_TYPb   ) | ( Reserved   ) | (Reserved    )
// PackageOP | PkgLength | NumElements  | byteprefix Num | byteprefix Num | byteprefix Num | byteprefix Num
// 12        | 0A        | 04           | 0A         05  | 0A          05 | 0A         05  | 0A         05
//
//----this-structure-was-also-seen----------------------
// PackageOP | PkgLength | NumElements |
// 12        | 06        | 04          | 00 00 00 00
//
// (Pkglength bit 6-7 encode additional PkgLength bytes [shouldn't be the case here])
//

/*
// Credits:
// https://forum.osdev.org/viewtopic.php?t=16990
int initAcpi(void)
{
   unsigned int *ptr = acpiGetRSDPtr();

   // check if address is correct  ( if acpi is available on this pc )
   if (ptr != NULL && acpiCheckHeader(ptr, "RSDT") == 0)
   {
      // the RSDT contains an unknown number of pointers to acpi tables
      int entrys = *(ptr + 1);
      entrys = (entrys-36) /4;
      ptr += 36/4;   // skip header information

      while (0<entrys--)
      {
         // check if the desired table is reached
         if (acpiCheckHeader((unsigned int *) *ptr, "FACP") == 0)
         {
            entrys = -2;
            struct FACP *facp = (struct FACP *) *ptr;
            if (acpiCheckHeader((unsigned int *) facp->DSDT, "DSDT") == 0)
            {
               // search the \_S5 package in the DSDT
               char *S5Addr = (char *) facp->DSDT +36; // skip header
               int dsdtLength = *(facp->DSDT+1) -36;
               while (0 < dsdtLength--)
               {
                  if ( memcmp(S5Addr, "_S5_", 4) == 0)
                     break;
                  S5Addr++;
               }
               // check if \_S5 was found
               if (dsdtLength > 0)
               {
                  // check for valid AML structure
                  if ( ( *(S5Addr-1) == 0x08 || ( *(S5Addr-2) == 0x08 && *(S5Addr-1) == '\\') ) && *(S5Addr+4) == 0x12 )
                  {
                     S5Addr += 5;
                     S5Addr += ((*S5Addr &0xC0)>>6) +2;   // calculate PkgLength size

                     if (*S5Addr == 0x0A)
                        S5Addr++;   // skip byteprefix
                     SLP_TYPa = *(S5Addr)<<10;
                     S5Addr++;

                     if (*S5Addr == 0x0A)
                        S5Addr++;   // skip byteprefix
                     SLP_TYPb = *(S5Addr)<<10;

                     SMI_CMD = facp->SMI_CMD;

                     ACPI_ENABLE = facp->ACPI_ENABLE;
                     ACPI_DISABLE = facp->ACPI_DISABLE;

                     PM1a_CNT = facp->PM1a_CNT_BLK;
                     PM1b_CNT = facp->PM1b_CNT_BLK;
                     
                     PM1_CNT_LEN = facp->PM1_CNT_LEN;

                     SLP_EN = 1<<13;
                     SCI_EN = 1;

                     return 0;
                  } else {
                     wrstr("\\_S5 parse error.\n");
                  }
               } else {
                  wrstr("\\_S5 not present.\n");
               }
            } else {
               wrstr("DSDT invalid.\n");
            }
         }
         ptr++;
      }
      wrstr("no valid FACP present.\n");
   } else {
      wrstr("no acpi.\n");
   }

   return -1;
}
*/

/*
// Credits:
// https://forum.osdev.org/viewtopic.php?t=16990
void acpiPowerOff(void)
{
   // SCI_EN is set to 1 if acpi shutdown is possible
   if (SCI_EN == 0)
      return;

   acpiEnable();

   // send the shutdown command
   outw((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN );
   if ( PM1b_CNT != 0 )
      outw((unsigned int) PM1b_CNT, SLP_TYPb | SLP_EN );

   wrstr("acpi poweroff failed.\n");
}
*/

/*
// Credits:
// https://wiki.osdev.org/MADT
// The following code snippet detects and parses MADT table 
// to collect Local APIC data on SMP systems. 
// Works with both RSDT and XSDT, and compiles 
// for both protected mode and long mode.

uint8_t lapic_ids[256]={0}; // CPU core Local APIC IDs
uint8_t numcore=0;          // number of cores detected
uint64_t lapic_ptr=0;       // pointer to the Local APIC MMIO registers
uint64_t ioapic_ptr=0;      // pointer to the IO APIC MMIO registers
 
void detect_cores(uint8_t *rsdt)
{
  uint8_t *ptr, *ptr2;
  uint32_t len;
 
  // iterate on ACPI table pointers
  for(len = *((uint32_t*)(rsdt + 4)), ptr2 = rsdt + 36; ptr2 < rsdt + len; ptr2 += rsdt[0]=='X' ? 8 : 4) {
    ptr = (uint8_t*)(uintptr_t)(rsdt[0]=='X' ? *((uint64_t*)ptr2) : *((uint32_t*)ptr2));
    if(!memcmp(ptr, "APIC", 4)) {
      // found MADT
      lapic_ptr = (uint64_t)(*((uint32_t)(ptr+0x24)));
      ptr2 = ptr + *((uint32_t*)(ptr + 4));
      // iterate on variable length records
      for(ptr += 44; ptr < ptr2; ptr += ptr[1]) {
        switch(ptr[0]) {
          case 0: if(ptr[4] & 1) lapic_ids[numcore++] = ptr[3]; break; // found Processor Local APIC
          case 1: ioapic_ptr = (uint64_t)*((uint32_t*)(ptr+4); break;  // found IOAPIC
          case 5: lapic_ptr = *((uint64_t*)(ptr+4); break;             // found 64 bit LAPIC
        }
      }
      break;
    }
  }
}
*/

/*
// detect SMP cores and print out results
detect_cores(rsd_ptr->rsdt_address);
*/

/*
printk("Found %d cores, IOAPIC %lx, LAPIC %lx, Processor IDs:", numcore, ioapic_ptr, lapic_ptr);
for(i = 0; i < numcore; i++)
  printk(" %d", lapic_ids[i]);
printk("\n");
*/

/*
// #test
// Creadits: Nelson Cole.
unsigned long acip_init(void);
unsigned long acip_init(void) 
{

	// // search below the 1mb mark for RSDP signature
	unsigned long ptr = 0x000E0000;
	unsigned long end = ptr + 0x100000;
	
	unsigned int ebda = (unsigned int) (*(unsigned short *) EBDA);
	ebda = ebda << 4;  // convert segment em linear address
	
  	while ( ptr  < end ) 
    {    	
       	unsigned long long signature = *(unsigned long long *) ptr;
       	if (signature == 0x2052545020445352) // "RSD PTR "
   		{
       		return ptr;
      	}        	
        	ptr  += 16;
    	}
 	
   	// search Extended BIOS Data Area for the Root System Description Pointer signature
   	ptr = ebda;
   	end = ptr + 1024;
   	
   	while(ptr  < end){
   	
   		unsigned long long signature = *(unsigned long long *) ptr;

        	if (signature == 0x2052545020445352) // "RSD PTR "
      		{
           		return ptr;
        	}	
        	ptr  += 16;
   	}

	return 0;
}
*/


/*
// Credito: kaworu (https://forum.osdev.org/viewtopic.php?t=16990)
void poweroff() {
    // enviar o comando shutdown
    outportw(fadt->PM1a_CNT_BLK, SLP_TYPa | 1<<13 );
    if ( fadt->PM1b_CNT_BLK != 0 ) {
        outportw(fadt->PM1b_CNT_BLK, SLP_TYPb | 1<<13 );  
    }
}
*/


/*
void reboot()
{
 cli();
 
 if(rsdp->revision >= 0x2) {
     outportb(fadt->ResetRegister.address, fadt->ResetValue);
 }	

 printf("Use the 8042 keyboard controller to pulse the CPU's RESET pin\n");
 
 unsigned char val = 0x02;
     while ( val & 0x02) {
         
           val = inportb(0x64);
     }		
         
     outportb(0x64, 0xFE);
         
     while(1) 
         hlt(); 
}
*/


