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

#include "../core/kmain/bootinfo.h"
#include "../core/kmain/mode.h"
#include "../core/kmain/state.h"
#include "../core/kmain/system.h"
#include "../core/kmain/klimits2.h"
#include "../core/kmain/gdef.h"

// ==================================
// dev/
#include "../kres/dev/gdevice.h"
#include "../kres/dev/ginput.h"  // input manager support.

// ==================================
// ke/
#include "../core/ke/intake/sync.h"

// ===============================
// hal/
#include "../core/ke/hal/x86_64/gwd.h"  // whatch dogs.
#include "../core/ke/hal/pints.h"       // profiler

// ===============================
#include "../core/ke/kinit.h"   // kernel initialization.

// ===============================
// kmain/
#include "../core/kmain/info.h"
#include "../core/kmain/request.h"

#include "../core/kmain/debug.h"

// ==================================
// ke/
// Gramado configuration.
#include "../core/ke/hal/jiffies.h"

// ==================================
// crt/
// Libc support.
#include "../core/clibs/ktypes.h"
#include "../core/clibs/ktypes2.h"

// #todo: Move this one above?
#include "../core/clibs/ascii.h"

// Kernel objects.
// Can we move this above the clibs? Or after it?
#include "../core/kmain/kobject.h"

// core/clibs/
// Legacy stuff.
#include "../core/clibs/kstdarg.h"
#include "../core/clibs/kerrno.h"
#include "../core/clibs/kcdefs.h"
#include "../core/clibs/kstddef.h"
#include "../core/clibs/klimits.h"
#include "../core/clibs/kstdio.h"
#include "../core/clibs/kstdlib.h"
#include "../core/clibs/kstring.h"
#include "../core/clibs/kctype.h"
#include "../core/clibs/kiso646.h"
#include "../core/clibs/ksignal.h"
#include "../core/clibs/kunistd.h"
#include "../core/clibs/kfcntl.h"
#include "../core/clibs/kioctl.h"
#include "../core/clibs/kioctls.h"
#include "../core/clibs/ktermios.h"
#include "../core/clibs/kttydef.h"

// ==================================
// ke/
// Globals. PIDs support.
#include "../core/ke/intake/kpid.h"

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
#include "../core/ke/hal/x86_64/ports64.h"
#include "../core/ke/hal/x86_64/cpu.h"
#include "../core/ke/hal/x86_64/tss.h"
#include "../core/ke/hal/x86_64/x64gdt.h"
#include "../core/ke/hal/x86_64/x64.h"
#include "../core/ke/hal/detect.h"

// ==================================
// virt/
#include "../core/kmain/virt/hv.h"

// ==========================================
// hal/arm/
// #include "../core/ke/hal/arm/archhal.h"

// ==========================================
// hal/x86_64/
#include "../core/ke/hal/x86_64/cpuid.h"
#include "../core/ke/hal/x86_64/up/up.h"
#include "../core/ke/hal/x86_64/smp/x64smp.h"
#include "../core/ke/hal/x86_64/pic.h"
#include "../core/ke/hal/x86_64/smp/apic.h"
#include "../core/ke/hal/x86_64/smp/apictim.h"
#include "../core/ke/hal/x86_64/smp/ioapic.h"
#include "../core/ke/hal/x86_64/pit.h"
#include "../core/ke/hal/x86_64/rtc.h"
#include "../core/ke/hal/x86_64/breaker.h"
#include "../core/ke/hal/x86_64/archhal.h"

// ==========================================
// Architecture-independent HAL interface
#include "../core/ke/hal/hal.h"

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
//#include "../core/clibs/ascii.h"

// ==================================
// gramk/
// sw - Graphics Engine
#include "../gramk/gdi/gre/color.h"
#include "../gramk/gdi/gre/font.h"
#include "../gramk/gdi/gre/bg.h"

// ==================================
// ke/
// Can we move this up?
#include "../core/ke/intake/msgcode.h"

// ==================================
// gramk/

#include "../gramk/gdi/gre/pixel.h"
#include "../gramk/gdi/gre/char.h"
#include "../gramk/gdi/gre/text.h"
#include "../gramk/gdi/gre/line.h"
#include "../gramk/gdi/gre/rect.h"
#include "../gramk/gdi/gre/bitblt.h"
#include "../gramk/gdi/gre/gre.h"

#include "../gramk/gdi/dispsrv.h"
#include "../gramk/gdi/osshell.h"

#include "../gramk/user/input.h"
#include "../gramk/user/callback.h"

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
#include "../core/ke/intake/prio.h"     // Priority
#include "../core/ke/intake/quantum.h"  // Quantum
#include "../core/ke/intake/image.h"
#include "../core/ke/intake/disp/x86_64/x64cont.h"
#include "../core/ke/intake/disp/ts.h"
#include "../core/ke/intake/tasks.h"
#include "../core/ke/intake/queue.h"
#include "../core/ke/intake/intake.h"
#include "../core/ke/intake/disp/dispatch.h"

#include "../core/ke/intake/thread.h"
#include "../core/ke/intake/process.h"
#include "../core/ke/intake/ithread.h"
#include "../core/ke/intake/clone.h"
#include "../core/ke/intake/ipc.h"

#include "../core/ke/intake/sched/sched.h"
#include "../core/ke/intake/sched/schedq.h"

// Precisa de todos os componentes de ke/
#include "../core/ke/ke.h"

// ==================================
// The user interactions.
#include "../gramk/user/user.h"

// Exporting some gramk functions to the other modules
// inside the base kernel.
#include "../gramk/gramk.h"

// Reboot system.
#include "../core/kmain/reboot.h"
// Ring 0 kernel modules.
#include "../core/kmain/mod/mod.h"
#include "../core/kmain/mod/public.h"

// Kernel layers. (Work in progress)
#include "../core/kmain/layers.h"
// Syscalls: (Called by the interrups 0x80, 0x81, 0x82, 0x83).
#include "../core/kmain/sci/syscalls.h"

// ==================================
// ke/
// syscall support
#include "../core/ke/hal/x86_64/x64sc.h"

// ==================================

#include "../core/kmain/wrappers.h"

// cgroups and namespaces
#include "../core/kmain/cont/cg.h"
#include "../core/kmain/cont/ns.h"

// Main kernel controller header.
#include "../core/kctl.h"

