// mm/memmap.h 
// Memory map support.
// This file has the main memory addresses used by the system.
// THIS IS A WORK IN PROGRESS, every address can change in the future.
// History:
//     2015 - Created by Fred Nora.

// See:
// x64gpa.h for physical addresses.
// x64gva.h for virtual addresses.

// The base kernel:
// 1MB mark.
// base kernel = 0x00100000pa

// The processes:
// Each process has its own address spaces and its own pages
// used to compose its address space.

/*
  Main virtual addresses for the kernel image.
  It starts at 768MB mark.

			     +====================================+
           |           Kernel land              |
           |                                    | 
           |  Stack = 0x301E0000 ~ 0x301FFFF0   | 
           |  Heap  = 0x30100000 ~ 0x301D0000   | size = 0xD0000.
           |                                    | 			 
           |  Kernel Entry point = 0x30001000   | Entry point
	         |  Kernel Base        = 0x30000000   |	Base 
           |                                    |	 		 
	30000000 |        ( Kernel Mode access )      |	 	   
	         +------------------------------------+
*/

/*   
  Standard addresses for the ring3 processes.
    ============================================================	

             +------------------------------------+
             |             ...                    |  
             +------------------------------------+
    FFFFFFFF |             4GB mark               | Some devices in this area?
             +------------------------------------+ 
             |             ...                    | Some devices in this area? 
             +------------------------------------+
             |           Kernel land              |  
             |             (2MB)                  |  
             |                                    | 
    30400000 |           BackBuffer               | 
             +------------------------------------+	
             +------------------------------------+	
             |           Kernel land              |	 
             |             (2MB)                  |  
             |             ...                    |  
    30200000 |          FrontBuffer(LFB)          | 
             +------------------------------------+
			       +====================================+
             |           Kernel land              |
             |                                    | 
             |  Stack = 0x301E0000 ~ 0x301FFFF0   | 
             |  Heap  = 0x30100000 ~ 0x301D0000   | size = 0xD0000.
             |                                    | 			 
             |  Kernel Entry point = 0x30001000   | Entry point
	           |  Kernel Base        = 0x30000000   |	Base 
             |                                    |	 		 
	  30000000 |        ( Kernel Mode access )      |	 	   
             +------------------------------------+
             |           User Land                |
             |                                    | #todo: Heap and stack
             |                                    |
             | 0x00201000 = Process entry point   | Image entrypoint.
             | 0x00200000 = Process image base    | Image base.
             |       (User Mode access)           |  
             |------------------------------------|
             |           0 ~ 0x001FFFFF           | See bellow.
    00000000 |         (kernel Mode access)       | 
             +====================================+
 
*/ 

/*
 At the bootm of virtual memory we have 2MB in kernel mode.
 It's used by the kernel to access some low physical addresses 
 including VGA, etc.
 See: x64gpa.h and x64gva.h for more information.
*/


#ifndef __MM_MEMMAP_H
#define __MM_MEMMAP_H    1

// #todo

#endif   

//
// End
//

