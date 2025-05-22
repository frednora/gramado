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

// -------

//#include "../core/arch/arm/arch.h"
#include "../core/arch/x86_64/arch.h"


//
// Core control  kmain/
//

#include "../core/kmain/mode.h"
#include "../core/kmain/state.h"
#include "../core/kmain/system.h"
#include "../core/kmain/klimits2.h"

#include "../core/kmain/globals.h"

#include "../core/kmain/bootblk/bootblk.h"

// ==================================
// ke/
#include "../core/ke/intake/sync.h"

// ===============================
// hal/
#include "../core/ke/hal/x86_64/gwd.h"  // whatch dogs.
#include "../core/ke/hal/pints.h"       // profiler

// ===============================
#include "../core/ke/intake/x64init.h"  // x64 kernel initialization.
#include "../core/kmain/kmain.h"        // kernel initialization.

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
#include "../libk/ktypes.h"
#include "../libk/ktypes2.h"

// #todo: Move this one above?
#include "../libk/ascii.h"

// Kernel objects.
// Can we move this above the libk? Or after it?
#include "../core/kmain/kobject.h"

// libk/
// Legacy stuff.
#include "../libk/kstdarg.h"
#include "../libk/kerrno.h"
#include "../libk/kcdefs.h"
#include "../libk/kstddef.h"
#include "../libk/klimits.h"
#include "../libk/kstdio.h"
#include "../libk/kstdlib.h"
#include "../libk/kstring.h"
#include "../libk/kctype.h"
#include "../libk/kiso646.h"
#include "../libk/ksignal.h"
#include "../libk/kunistd.h"
#include "../libk/kfcntl.h"
#include "../libk/kioctl.h"
#include "../libk/kioctls.h"
#include "../libk/ktermios.h"
#include "../libk/kttydef.h"

// ==================================
// ke/
// Globals. PIDs support.
#include "../core/ke/intake/kpid.h"

// ==================================
// mm/
// Memory management.
#include "../res/mm/mmsize.h"
#include "../res/mm/x86_64/x64gpa.h"
#include "../res/mm/x86_64/x64gva.h"
#include "../res/mm/memmap.h" 
#include "../res/mm/x86_64/intelmm.h"
#include "../res/mm/mmblock.h"
#include "../res/mm/mmusage.h"
#include "../res/mm/x86_64/x64mm.h"
#include "../res/mm/x86_64/paging.h"
#include "../res/mm/mmft.h"
#include "../res/mm/mmglobal.h"  // Deve ficar mais acima.
#include "../res/mm/heap.h"      // Heap pointer support.
#include "../res/mm/aspace.h"    // Address Space, (data base account).
#include "../res/mm/bank.h"      // Bank. database
#include "../res/mm/mm.h"

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
#include "../core/ke/hal/x86_64/smp/mpfloat.h"
#include "../core/ke/hal/x86_64/smp/acpi.h"
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
#include "../res/bus/pci/pci.h"
#include "../res/bus/bus.h"

// ==================================
// dev/
// io
#include "../res/dev/io.h"

// ==================================
// dev/
// Devices
// primeiro char, depois block, depois network.
// tty
#include "../res/dev/chardev/tty/ttyldisc.h"
#include "../res/dev/chardev/tty/ttydrv.h"
#include "../res/dev/chardev/tty/tty.h"
#include "../res/dev/chardev/tty/pty.h"

#include "../res/dev/chardev/console/console.h"

// hw stuff - display device
// display device support.
#include "../res/dev/chardev/display/display.h"
// bootloader display device
#include "../res/dev/chardev/display/bldisp/bldisp.h"
//#include "../res/dev/chardev/display/qemudisp/qemudisp.h"

// ==================================
// dev/
#include "../res/dev/dev00.h"

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

#include "../gramk/gdi/gdi.h"

// Event Interface.
#include "../gramk/evi/obroker.h"
#include "../gramk/evi/output.h"
#include "../gramk/evi/ibroker.h"
#include "../gramk/evi/input.h"
#include "../gramk/evi/callback.h"

// ==================================
// dev/

// chardev/
// Serial port. (COM).
#include "../res/dev/chardev/serial/serial.h"

#include "../res/dev/chardev/vk.h"
#include "../res/dev/chardev/kbdabnt2.h"
#include "../res/dev/chardev/kbdmap.h"

// i8042 (PS/2)
#include "../res/dev/chardev/i8042/keyboard.h"
#include "../res/dev/chardev/i8042/ps2kbd.h"
#include "../res/dev/chardev/i8042/mouse.h"
#include "../res/dev/chardev/i8042/ps2mouse.h"
#include "../res/dev/chardev/i8042/i8042.h"

// blkdev/
// Block devices
// ata, sata
#include "../res/dev/blkdev/ata/ata.h"
//#include "../res/dev/blkdev/ahci/ahci.h"
// Storage manager.
#include "../res/dev/blkdev/storage.h"

// netdev/
// Network devices
// primeiro controladoras depois protocolos
// e1000 - nic intel
#include "../res/dev/netdev/e1000/e1000.h"

// ==================================
// net/ 
// (network, protocols and socket)
// network
#include "../res/net/mac.h"
#include "../res/net/host.h"
#include "../res/net/in.h"
#include "../res/net/un.h"

//
// Protocols
//

// =================================
// prot/

// Core protocols
#include "../res/net/prot/core/ethernet.h"
#include "../res/net/prot/core/arp.h"
#include "../res/net/prot/core/ip.h"
// Commom protocols
#include "../res/net/prot/tcp.h"
#include "../res/net/prot/udp.h"
#include "../res/net/prot/dhcp.h" 
#include "../res/net/prot/gprot.h"


// Extra protocols
#include "../res/net/prot/icmp.h" 

// net/

// Network
#include "../res/net/nports.h"     //(network) Network Ports  (sw)
#include "../res/net/inet.h"

#include "../res/net/socklib.h"     //
#include "../res/net/socket.h"      //last always

#include "../res/net/domain.h"
#include "../res/net/net.h"     //(network) Gerenciamento de rede.  

// ----------------------
// Last:
// Device interface.
// Device manager.
#include "../res/dev/dev.h"

// ==================================
// fs/
// File system
// ----------------------
// Depois de devices.
// fs
#include "../res/fs/path.h"      // path.

#include "../res/fs/fat/fatlib.h"    // fat16 library.
#include "../res/fs/fat/fat.h"       // fat16.

#include "../res/fs/inode.h"
#include "../res/fs/exec_elf.h"
#include "../res/fs/pipe.h"
#include "../res/fs/files.h"
#include "../res/fs/fs.h"

// ==================================
#include "../res/res.h"

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
#include "../core/kmain/panic.h"

// cgroups and namespaces
#include "../core/kmain/cont/cg.h"
#include "../core/kmain/cont/ns.h"

// Core module.
// It controls the resorces in res/.
#include "../core/core.h"

