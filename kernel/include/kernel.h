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

//#include "../gthouse/arch/arm/arch.h"
#include "../gthouse/arch/x86_64/arch.h"

//
// Core control kwrap/
//

#include "../gthouse/kwrap/mode.h"
#include "../gthouse/kwrap/state.h"
#include "../gthouse/kwrap/system.h"
#include "../gthouse/kwrap/klimits2.h"

#include "../gthouse/kwrap/globals.h"

#include "../gthouse/kwrap/bootblk/bootblk.h"

// ==================================
// halls/apphalls/exec/ke/
#include "../halls/apphalls/exec/ke/intake/sync.h"

// ===============================
// hal/
#include "../halls/apphalls/exec/ke/hal/x86_64/gwd.h"  // whatch dogs.
#include "../halls/apphalls/exec/ke/hal/pints.h"       // profiler

// ===============================
#include "../halls/apphalls/exec/ke/intake/x64init.h"  // x64 kernel initialization.

// kernel initialization.
#include "../kmain.h"


// ===============================
// kwrap/
#include "../gthouse/kwrap/info.h"
#include "../gthouse/kwrap/request.h"

#include "../gthouse/kwrap/klog/klog.h"

// ==================================
// halls/apphalls/exec/ke/
// Gramado configuration.
#include "../halls/apphalls/exec/ke/hal/jiffies.h"

// ==================================
// crt/
// Libc support.
#include "../gthouse/libk/ktypes.h"
#include "../gthouse/libk/ktypes2.h"

// #todo: Move this one above?
#include "../gthouse/libk/ascii.h"

// Kernel objects.
// Can we move this above the libk? Or after it?
#include "../gthouse/kwrap/kobject.h"

// gthouse/libk/
// Legacy stuff.
#include "../gthouse/libk/kstdarg.h"
#include "../gthouse/libk/kerrno.h"
#include "../gthouse/libk/kcdefs.h"
#include "../gthouse/libk/kstddef.h"
#include "../gthouse/libk/klimits.h"

#include "../gthouse/libk/kstdio.h"
#include "../gthouse/libk/printk/printk.h"

#include "../gthouse/libk/kstdlib.h"
#include "../gthouse/libk/kstring.h"
#include "../gthouse/libk/kctype.h"
#include "../gthouse/libk/kiso646.h"
#include "../gthouse/libk/ksignal.h"
#include "../gthouse/libk/kunistd.h"
#include "../gthouse/libk/kfcntl.h"
#include "../gthouse/libk/kioctl.h"
#include "../gthouse/libk/kioctls.h"
#include "../gthouse/libk/ktermios.h"
#include "../gthouse/libk/kttydef.h"

#include "../gthouse/libk/libk.h"


// ==================================
// halls/apphalls/exec/ke/
// Globals. PIDs support.
#include "../halls/apphalls/exec/ke/intake/kpid.h"

// ==================================
// apphalls/req/mm/
// Memory management.
#include "../halls/apphalls/req/mm/mmsize.h"
#include "../halls/apphalls/req/mm/x86_64/x64gpa.h"
#include "../halls/apphalls/req/mm/x86_64/x64gva.h"
#include "../halls/apphalls/req/mm/memmap.h" 
#include "../halls/apphalls/req/mm/x86_64/intelmm.h"
#include "../halls/apphalls/req/mm/mmblock.h"
#include "../halls/apphalls/req/mm/mmusage.h"
#include "../halls/apphalls/req/mm/x86_64/x64mm.h"
#include "../halls/apphalls/req/mm/slab.h"
#include "../halls/apphalls/req/mm/x86_64/paging.h"
#include "../halls/apphalls/req/mm/mmft.h"
#include "../halls/apphalls/req/mm/mmpool.h"
#include "../halls/apphalls/req/mm/mmglobal.h"  // Deve ficar mais acima.
#include "../halls/apphalls/req/mm/heap.h"      // Heap pointer support.
#include "../halls/apphalls/req/mm/aspace.h"    // Address Space, (data base account).
#include "../halls/apphalls/req/mm/bank.h"      // Bank. database
#include "../halls/apphalls/req/mm/mm.h"

// ==================================
// hal/
#include "../halls/apphalls/exec/ke/hal/x86_64/ports64.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/cpu.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/tss.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/x64gdt.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/x64.h"
#include "../halls/apphalls/exec/ke/hal/detect.h"

// ==================================
// virt/
#include "../gthouse/kwrap/virt/hv.h"

// ==========================================
// hal/arm/
// #include "../halls/apphalls/exec/ke/hal/arm/archhal.h"

// ==========================================
// hal/x86_64/
#include "../halls/apphalls/exec/ke/hal/x86_64/cpuid.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/up/up.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/smp/mpfloat.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/smp/acpi.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/smp/x64smp.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/pic.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/smp/apic.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/smp/apictim.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/smp/ioapic.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/pit.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/rtc.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/breaker.h"
#include "../halls/apphalls/exec/ke/hal/x86_64/archhal.h"

// ==========================================
// Architecture-independent HAL interface
#include "../halls/apphalls/exec/ke/hal/hal.h"

// ==================================
// apphalls/per/bus/
// PCI bus.
#include "../halls/apphalls/per/bus/pci/pci.h"
#include "../halls/apphalls/per/bus/bus.h"

// ==================================
// apphalls/per/
// io
#include "../halls/apphalls/per/io.h"

// ==================================
// apphalls/per/
// Devices
// primeiro char, depois block, depois network.
// tty
#include "../halls/uhalls/chardev/tty/ttyldisc.h"
#include "../halls/uhalls/chardev/tty/ttydrv.h"
#include "../halls/uhalls/chardev/tty/tty.h"
#include "../halls/uhalls/chardev/tty/pty.h"

#include "../halls/uhalls/chardev/console/console.h"

// hw stuff - display device
// display device support.
#include "../halls/uhalls/chardev/display/display.h"
// bootloader display device
#include "../halls/uhalls/chardev/display/bldisp/rop.h"
#include "../halls/uhalls/chardev/display/bldisp/bldisp.h"
//#include "../halls/uhalls/chardev/display/qemudisp/qemudisp.h"

// ==================================
// apphalls/per/
#include "../halls/apphalls/per/dev00.h"

// ==================================
// gthouse/wink/ 
// sw - Graphics Engine
#include "../gthouse/wink/gdi/gre/color.h"
#include "../gthouse/wink/gdi/gre/font.h"
#include "../gthouse/wink/gdi/gre/bg.h"

// ==================================
// halls/apphalls/exec/ke/
// Can we move this up?
#include "../halls/apphalls/exec/ke/intake/msgcode.h"

// ==================================
// gthouse/wink/

#include "../gthouse/wink/gdi/gre/pixel.h"
#include "../gthouse/wink/gdi/gre/char.h"
#include "../gthouse/wink/gdi/gre/text.h"
#include "../gthouse/wink/gdi/gre/line.h"
#include "../gthouse/wink/gdi/gre/rect.h"
#include "../gthouse/wink/gdi/gre/bitblt.h"
#include "../gthouse/wink/gdi/gre/gre.h"

#include "../gthouse/wink/gdi/dispsrv.h"
#include "../gthouse/wink/gdi/osshell.h"

#include "../gthouse/wink/gdi/gdi.h"

// Event Interface
#include "../gthouse/wink/evi/obroker.h"
#include "../gthouse/wink/evi/output.h"
#include "../gthouse/wink/evi/ibroker.h"
#include "../gthouse/wink/evi/input.h"

// ===========

#include "../halls/apphalls/exec/ke/intake/disp/callback.h"

// ==================================
// apphalls/per/

// uhalls/chardev/
// Serial port. (COM).
#include "../halls/uhalls/chardev/serial/serial.h"

#include "../halls/uhalls/chardev/vk.h"
#include "../halls/uhalls/chardev/kbdmaps/kbdmaps.h"
#include "../halls/uhalls/chardev/kbdmaps/ptbr/mapabnt2.h"
// ...

// i8042 (PS/2)
#include "../halls/uhalls/chardev/i8042/keyboard.h"
#include "../halls/uhalls/chardev/i8042/ps2kbd.h"
#include "../halls/uhalls/chardev/i8042/mouse.h"
#include "../halls/uhalls/chardev/i8042/ps2mouse.h"
#include "../halls/uhalls/chardev/i8042/i8042.h"

// blkdev/
// Block devices
// ata, sata
#include "../halls/apphalls/per/blkdev/ata/ata.h"
//#include "../halls/apphalls/per/blkdev/ahci/ahci.h"
// Storage manager.
#include "../halls/apphalls/per/blkdev/storage.h"

// netdev/
// Network devices
// primeiro controladoras depois protocolos
// e1000 - nic intel
#include "../halls/apphalls/per/netdev/e1000/e1000.h"

// ==================================
// apphalls/per/net/ 
// (network, protocols and socket)
// network
#include "../halls/apphalls/per/net/mac.h"
#include "../halls/apphalls/per/net/host.h"
#include "../halls/apphalls/per/net/in.h"
#include "../halls/apphalls/per/net/un.h"

//
// Protocols
//

// =================================
// prot/

// Core protocols
#include "../halls/apphalls/per/net/prot/core/ethernet.h"
#include "../halls/apphalls/per/net/prot/core/arp.h"
#include "../halls/apphalls/per/net/prot/core/ip.h"
// Commom protocols
#include "../halls/apphalls/per/net/prot/tcp.h"
#include "../halls/apphalls/per/net/prot/udp.h"
#include "../halls/apphalls/per/net/prot/dhcp.h" 
#include "../halls/apphalls/per/net/prot/gprot.h"


// Extra protocols
#include "../halls/apphalls/per/net/prot/icmp.h" 

// apphalls/per/net/

// Network

#include "../halls/apphalls/per/net/nports.h"     //(network) Network Ports  (sw)
#include "../halls/apphalls/per/net/inet.h"

#include "../halls/apphalls/per/net/socklib.h"     //
#include "../halls/apphalls/per/net/socket.h"      //last always

#include "../halls/apphalls/per/net/domain.h"

#include "../halls/apphalls/per/net/ifconfig/netif.h"  // Network interface
#include "../halls/apphalls/per/net/net.h"     //(network) Gerenciamento de rede.  

// ----------------------
// Last:
// Device interface.
// Device manager.
#include "../halls/apphalls/per/dev.h"

// ==================================
// apphalls/per/fs/
// File system
// ----------------------
// Depois de devices.
// fs
#include "../halls/apphalls/per/fs/path.h"      // path.

#include "../halls/apphalls/per/fs/fat/fatlib.h"    // fat16 library.
#include "../halls/apphalls/per/fs/fat/fat.h"       // fat16.

#include "../halls/apphalls/per/fs/inode.h"
#include "../halls/apphalls/per/fs/exec_elf.h"
#include "../halls/apphalls/per/fs/pipe.h"
#include "../halls/apphalls/per/fs/files.h"
#include "../halls/apphalls/per/fs/fs.h"

// ==================================
#include "../halls/res.h"

// ==================================
// intake/
#include "../halls/apphalls/exec/ke/intake/prio.h"     // Priority
#include "../halls/apphalls/exec/ke/intake/quantum.h"  // Quantum
#include "../halls/apphalls/exec/ke/intake/image.h"
#include "../halls/apphalls/exec/ke/intake/disp/x86_64/x64cont.h"
#include "../halls/apphalls/exec/ke/intake/disp/ts.h"
#include "../halls/apphalls/exec/ke/intake/queue.h"
#include "../halls/apphalls/exec/ke/intake/intake.h"
#include "../halls/apphalls/exec/ke/intake/disp/spawn.h"
#include "../halls/apphalls/exec/ke/intake/disp/dispatch.h"

#include "../halls/apphalls/exec/ke/intake/thread.h"
#include "../halls/apphalls/exec/ke/intake/te.h"
#include "../halls/apphalls/exec/ke/intake/ithread.h"
#include "../halls/apphalls/exec/ke/intake/clone.h"
#include "../halls/apphalls/exec/ke/intake/ipc.h"

#include "../halls/apphalls/exec/ke/intake/sched/sched.h"
#include "../halls/apphalls/exec/ke/intake/sched/schedq.h"

// Precisa de todos os componentes de ke/
#include "../halls/apphalls/exec/ke/ke.h"

// ==================================
// The user interactions
#include "../gthouse/wink/user/user.h"

// Exporting some wink functions to the other modules
// inside the base kernel.
#include "../gthouse/wink/wink.h"

// Reboot system.
#include "../gthouse/kwrap/reboot.h"
// Ring 0 kernel modules.
#include "../gthouse/kwrap/mod/mod.h"
#include "../gthouse/kwrap/mod/public.h"

// Kernel layers. (Work in progress)
#include "../gthouse/kwrap/layers.h"

// The handlers for the services.
#include "../gthouse/kwrap/sci/sys.h"

// The definitions for the syscall numbers.
#include "../gthouse/kwrap/sci/sci0.h"
#include "../gthouse/kwrap/sci/sci1.h"
#include "../gthouse/kwrap/sci/sci2.h"
#include "../gthouse/kwrap/sci/sci3.h"

// The handlers for the four syscalls.
#include "../gthouse/kwrap/sci/sci.h" 

// ==================================
// ke/
// syscall support
#include "../halls/apphalls/exec/ke/hal/x86_64/x64sc.h"

// ==================================

#include "../gthouse/kwrap/wrappers.h"
#include "../gthouse/kwrap/panic.h"

// cgroups and namespaces
#include "../gthouse/kwrap/cont/cg.h"
#include "../gthouse/kwrap/cont/ns.h"

// Core module.
// It controls the resorces in halls/.
#include "../gthouse/core.h"

