// kernel.h
// Gramado OS headers.
// Created by Fred Nora.

#define CURRENT_ARCH_X86      1000
#define CURRENT_ARCH_X86_64   1001
// ...

// Order:
// configuration, libc, devices ...

//
// Configuration
//

#include "config/product.h"  // Product type
#include "config/version.h"  // Product version

#include "config/config.h"   // Select components
#include "config/utsname.h"  // System and machine
#include "config/u.h"        // System, machine and user.

//
// Core control  kmain/
//

#include "../kctl/kmain/bootinfo.h"
#include "../kctl/kmain/mode.h"
#include "../kctl/kmain/state.h"
#include "../kctl/kmain/system.h"
#include "../kctl/kmain/klimits2.h"
#include "../kctl/kmain/gdef.h"


// ==================================
// dev/
#include "../kres/dev/gdevice.h"
#include "../kres/dev/ginput.h"  // input manager support.

// ==================================
// ke/
#include "../kctl/ke/intake/gspin.h"

// ===============================
#include "../kctl/ke/hal/x86_64/gwd.h"     // whatch dogs.
#include "../kctl/ke/hal/pints.h"   // profiler


// ===============================
#include "../kctl/ke/kinit.h"   // kernel initialization.

// ===============================
// kmain/
#include "../kctl/kmain/info.h"
#include "../kctl/kmain/request.h"

#include "../kctl/kmain/overall/debug.h"

// ==================================
// ke/
// Gramado configuration.
#include "../kctl/ke/hal/jiffies.h"

// ==================================
// crt/
// Libc support.
#include "../kctl/clibs/ktypes.h"
#include "../kctl/clibs/ktypes2.h"

// #todo: Move this one above?
#include "../kctl/clibs/ascii.h"

// Kernel objects.
// Can we move this above the clibs? Or after it?
#include "../kctl/kmain/kobject.h"

// kctl/clibs/
// Legacy stuff.
#include "../kctl/clibs/kstdarg.h"
#include "../kctl/clibs/kerrno.h"
#include "../kctl/clibs/kcdefs.h"
#include "../kctl/clibs/kstddef.h"
#include "../kctl/clibs/klimits.h"
#include "../kctl/clibs/kstdio.h"
#include "../kctl/clibs/kstdlib.h"
#include "../kctl/clibs/kstring.h"
#include "../kctl/clibs/kctype.h"
#include "../kctl/clibs/kiso646.h"
#include "../kctl/clibs/ksignal.h"
#include "../kctl/clibs/kunistd.h"
#include "../kctl/clibs/kfcntl.h"
#include "../kctl/clibs/kioctl.h"
#include "../kctl/clibs/kioctls.h"
#include "../kctl/clibs/ktermios.h"
#include "../kctl/clibs/kttydef.h"

// ==================================
// ke/
// Globals. PIDs support.
#include "../kctl/ke/intake/kpid.h"

// ==================================
// mm/
// Memory management.
#include "../kres/mm/mmsize.h"
#include "../kres/mm/x86_64/x64gpa.h"
#include "../kres/mm/x86_64/x64gva.h"
#include "../kres/mm/mm.h"
#include "../kres/mm/memmap.h" 
#include "../kres/mm/x86_64/intelmm.h"
#include "../kres/mm/mmblock.h"
#include "../kres/mm/mmusage.h"
#include "../kres/mm/x86_64/x64mm.h"
#include "../kres/mm/mmft.h"
#include "../kres/mm/mmglobal.h"  // Deve ficar mais acima.
#include "../kres/mm/heap.h"      // Heap pointer support.
#include "../kres/mm/aspace.h"    // Address Space, (data base account).
#include "../kres/mm/bank.h"      // Bank. database

// ==================================
// hal/
#include "../kctl/ke/hal/x86_64/ports64.h"
#include "../kctl/ke/hal/x86_64/cpu.h"
#include "../kctl/ke/hal/x86_64/tss.h"
#include "../kctl/ke/hal/x86_64/x64gdt.h"
#include "../kctl/ke/hal/x86_64/x64.h"
#include "../kctl/ke/hal/detect.h"

// ==================================
// virt/
#include "../kctl/kmain/virt/hv.h"

// ==========================================
// hal/arm/
// #include "../kctl/ke/hal/arm/archhal.h"

// ==========================================
// hal/x86_64/
#include "../kctl/ke/hal/x86_64/cpuid.h"
#include "../kctl/ke/hal/x86_64/up/up.h"
#include "../kctl/ke/hal/x86_64/smp/x64smp.h"
#include "../kctl/ke/hal/x86_64/pic.h"
#include "../kctl/ke/hal/x86_64/smp/apic.h"
#include "../kctl/ke/hal/x86_64/smp/apictim.h"
#include "../kctl/ke/hal/x86_64/smp/ioapic.h"
#include "../kctl/ke/hal/x86_64/pit.h"
#include "../kctl/ke/hal/x86_64/rtc.h"
#include "../kctl/ke/hal/x86_64/breaker.h"
#include "../kctl/ke/hal/x86_64/archhal.h"

// ==========================================
// Architecture-independent HAL interface
#include "../kctl/ke/hal/hal.h"

// ==================================
// bus/
// PCI bus.
#include "../kres/bus/pci/pci.h"

// ==================================
// dev/
// io
#include "../kres/dev/io.h"

// ==================================
// dev/
// Devices
// primeiro char, depois block, depois network.
// tty
#include "../kres/dev/chardev/tty/ttyldisc.h"
#include "../kres/dev/chardev/tty/ttydrv.h"
#include "../kres/dev/chardev/tty/tty.h"
#include "../kres/dev/chardev/tty/pty.h"

#include "../kres/dev/chardev/console/console.h"

// hw stuff - display device
// display device support.
#include "../kres/dev/chardev/display/display.h"
// bootloader display device
#include "../kres/dev/chardev/display/bldisp/bldisp.h"
//#include "../kres/dev/chardev/display/qemudisp/qemudisp.h"

// #todo: Move this one above.
//#include "../kctl/clibs/ascii.h"

// ==================================
// gramk/
// sw - Graphics Engine
#include "../kres/gramk/gre/color.h"
#include "../kres/gramk/gre/font.h"
#include "../kres/gramk/gre/bg.h"

// ==================================
// ke/
// Can we move this up?
#include "../kctl/ke/intake/msgcode.h"

// ==================================
// gramk/

#include "../kres/gramk/gre/pixel.h"
#include "../kres/gramk/gre/char.h"
#include "../kres/gramk/gre/text.h"
#include "../kres/gramk/gre/line.h"
#include "../kres/gramk/gre/rect.h"
#include "../kres/gramk/gre/bitblt.h"
#include "../kres/gramk/gre/gre00.h"
#include "../kres/gramk/gre/gre01.h"
#include "../kres/gramk/gre/gre.h"

#include "../kres/gramk/user/dispsrv.h"
#include "../kres/gramk/user/input.h"
#include "../kres/gramk/user/callback.h"

// ==================================
// dev/

// chardev/
// Serial port. (COM).
#include "../kres/dev/chardev/serial/serial.h"

#include "../kres/dev/chardev/vk.h"
#include "../kres/dev/chardev/kbdabnt2.h"
#include "../kres/dev/chardev/kbdmap.h"

// i8042 (PS/2)
#include "../kres/dev/chardev/i8042/keyboard.h"
#include "../kres/dev/chardev/i8042/ps2kbd.h"
#include "../kres/dev/chardev/i8042/mouse.h"
#include "../kres/dev/chardev/i8042/ps2mouse.h"
#include "../kres/dev/chardev/i8042/i8042.h"

// blkdev/
// Block devices
// ata, sata
#include "../kres/dev/blkdev/ata/ata.h"
// Storage manager.
#include "../kres/dev/blkdev/superblk.h"
#include "../kres/dev/blkdev/volume.h"
#include "../kres/dev/blkdev/disk.h"
#include "../kres/dev/blkdev/storage.h"

// netdev/
// Network devices
// primeiro controladoras depois protocolos
// e1000 - nic intel
#include "../kres/dev/netdev/e1000/e1000.h"

// ==================================
// net/ 
// (network, protocols and socket)
// network
#include "../kres/net/mac.h"
#include "../kres/net/host.h"
#include "../kres/net/in.h"
#include "../kres/net/un.h"

//
// Protocols
//

// =================================
// prot/

// Core protocols
#include "../kres/net/prot/core/ethernet.h"
#include "../kres/net/prot/core/arp.h"
#include "../kres/net/prot/core/ip.h"
// Commom protocols
#include "../kres/net/prot/tcp.h"
#include "../kres/net/prot/udp.h"
#include "../kres/net/prot/dhcp.h" 
// Extra protocols
#include "../kres/net/prot/icmp.h" 

// net/

// Network
#include "../kres/net/nports.h"     //(network) Network Ports  (sw)
#include "../kres/net/inet.h"

#include "../kres/net/socklib.h"     //
#include "../kres/net/socket.h"      //last always

#include "../kres/net/domain.h"
#include "../kres/net/network.h"     //(network) Gerenciamento de rede.  

// ----------------------
// Last:
// Device manager.
#include "../kres/dev/devmgr.h"

// ==================================
// fs/
// File system
// ----------------------
// Depois de devices.
// fs
#include "../kres/fs/path.h"      // path.

#include "../kres/fs/fat/fatlib.h"    // fat16 library.
#include "../kres/fs/fat/fat.h"       // fat16.

#include "../kres/fs/inode.h"
#include "../kres/fs/exec_elf.h"
#include "../kres/fs/pipe.h"
#include "../kres/fs/fs.h"

// ==================================
// ke/
// Process structures

// ==================================
// intake/
#include "../kctl/ke/intake/prio.h"     // Priority
#include "../kctl/ke/intake/quantum.h"  // Quantum
#include "../kctl/ke/intake/image.h"
#include "../kctl/ke/intake/disp/x86_64/x64cont.h"
#include "../kctl/ke/intake/disp/ts.h"
#include "../kctl/ke/intake/tasks.h"
#include "../kctl/ke/intake/queue.h"
#include "../kctl/ke/intake/intake.h"
#include "../kctl/ke/intake/disp/dispatch.h"

#include "../kctl/ke/intake/thread.h"
#include "../kctl/ke/intake/process.h"
#include "../kctl/ke/intake/ithread.h"
#include "../kctl/ke/intake/clone.h"
#include "../kctl/ke/intake/ipc.h"

#include "../kctl/ke/intake/sched/sched.h"
#include "../kctl/ke/intake/sched/schedq.h"


// Precisa de todos os componentes de ke/
#include "../kctl/ke/ke.h"


// ==================================
// The user interactions.
#include "../kres/gramk/user/user.h"

// Exporting some gramk functions to the other modules
// inside the base kernel.
#include "../kres/gramk/gramk.h"

// Reboot system.
#include "../kctl/kmain/reboot.h"
// Ring 0 kernel modules.
#include "../kctl/kmain/mod/mod.h"
#include "../kctl/kmain/mod/public.h"

// Kernel layers. (Work in progress)
#include "../kctl/kmain/layers.h"
// Syscalls: (Called by the interrups 0x80, 0x81, 0x82, 0x83).
#include "../kctl/kmain/sci/syscalls.h"

// zero. (Used during the kernel initialization)
#include "../kctl/kmain/overall/zero.h"

// ==================================
// ke/
// syscall support
#include "../kctl/ke/hal/x86_64/x64sc.h"

// ==================================
// virt/
// Project California
#include "../kctl/kmain/overall/cali.h"

// cgroups and namespaces
#include "../kctl/kmain/cont/cg.h"
#include "../kctl/kmain/cont/ns.h"

// Main kernel controller header.
#include "../kctl/kctl.h"

