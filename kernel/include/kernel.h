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

//#include "../garden/arch/arm/arch.h"
#include "../garden/arch/x86_64/arch.h"

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
// halls/dev/exec/ke/
#include "../halls/dev/exec/ke/intake/sync.h"

// ===============================
// hal/
#include "../halls/dev/exec/ke/hal/x86_64/gwd.h"  // whatch dogs.
#include "../halls/dev/exec/ke/hal/pints.h"       // profiler

// ===============================
#include "../halls/dev/exec/ke/intake/x64init.h"  // x64 kernel initialization.

// kernel initialization
#include "../garden/kmain.h"


// ===============================
// kwrap/
#include "../gthouse/kwrap/info.h"
#include "../gthouse/kwrap/request.h"

#include "../gthouse/kwrap/klog/klog.h"

// ==================================
// halls/dev/exec/ke/
// Gramado configuration.
#include "../halls/dev/exec/ke/hal/jiffies.h"

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
// halls/dev/exec/ke/
// Globals. PIDs support.
#include "../halls/dev/exec/ke/intake/kpid.h"

// ==================================
// dev/req/mm/
// Memory management.
#include "../halls/dev/req/mm/mmsize.h"
#include "../halls/dev/req/mm/x86_64/x64gpa.h"
#include "../halls/dev/req/mm/x86_64/x64gva.h"
#include "../halls/dev/req/mm/memmap.h" 
#include "../halls/dev/req/mm/x86_64/intelmm.h"
#include "../halls/dev/req/mm/mmblock.h"
#include "../halls/dev/req/mm/mmusage.h"
#include "../halls/dev/req/mm/x86_64/x64mm.h"
#include "../halls/dev/req/mm/slab.h"
#include "../halls/dev/req/mm/x86_64/paging.h"
#include "../halls/dev/req/mm/mmft.h"
#include "../halls/dev/req/mm/mmpool.h"
#include "../halls/dev/req/mm/mmglobal.h"  // Deve ficar mais acima.
#include "../halls/dev/req/mm/heap.h"      // Heap pointer support.
#include "../halls/dev/req/mm/aspace.h"    // Address Space, (data base account).
#include "../halls/dev/req/mm/bank.h"      // Bank. database
#include "../halls/dev/req/mm/mm.h"

// ==================================
// hal/
#include "../halls/dev/exec/ke/hal/x86_64/ports64.h"
#include "../halls/dev/exec/ke/hal/x86_64/cpu.h"
#include "../halls/dev/exec/ke/hal/x86_64/tss.h"
#include "../halls/dev/exec/ke/hal/x86_64/x64gdt.h"
#include "../halls/dev/exec/ke/hal/x86_64/x64.h"
#include "../halls/dev/exec/ke/hal/detect.h"

// ==================================
// virt/
#include "../gthouse/kwrap/virt/hv.h"

// ==========================================
#include "../halls/platform/acpi/acpi.h"

// ==========================================
// hal/arm/
// #include "../halls/dev/exec/ke/hal/arm/archhal.h"

// ==========================================
// hal/x86_64/
#include "../halls/dev/exec/ke/hal/x86_64/cpuid.h"

#include "../halls/dev/exec/ke/hal/x86_64/up/up.h"

#include "../halls/dev/exec/ke/hal/x86_64/smp/mps.h"
#include "../halls/dev/exec/ke/hal/x86_64/smp/x64smp.h"
#include "../halls/dev/exec/ke/hal/x86_64/smp/apic.h"
#include "../halls/dev/exec/ke/hal/x86_64/smp/apictim.h"
#include "../halls/dev/exec/ke/hal/x86_64/smp/ioapic.h"

#include "../halls/dev/exec/ke/hal/x86_64/pic.h"
#include "../halls/dev/exec/ke/hal/x86_64/pit.h"
#include "../halls/dev/exec/ke/hal/x86_64/rtc.h"

#include "../halls/dev/exec/ke/hal/x86_64/breaker.h"
#include "../halls/dev/exec/ke/hal/x86_64/archhal.h"

// ==========================================
// Architecture-independent HAL interface
#include "../halls/dev/exec/ke/hal/hal.h"

// ==================================
// bus/
#include "../halls/platform/bus/pci/pci.h"
//#include "../halls/platform/bus/usb/usb.h"
//...
#include "../halls/platform/bus/bus.h"

// ==================================
// dev/per/
// io
#include "../halls/dev/per/io.h"

// ==================================
// dev/per/
// Devices
// primeiro char, depois block, depois network.
// tty
#include "../halls/dev/chardev/tty/ttyldisc.h"
#include "../halls/dev/chardev/tty/ttydrv.h"
#include "../halls/dev/chardev/tty/tty.h"
#include "../halls/dev/chardev/tty/pty.h"

#include "../halls/dev/chardev/console/console.h"

// hw stuff - display device

#include "../halls/dev/chardev/display/dc.h"

#include "../halls/dev/chardev/display/rop.h"

// display device support
#include "../halls/dev/chardev/display/display.h"

// bootloader display device
#include "../halls/dev/chardev/display/bldisp/bldisp.h"

// qemu display device
//#include "../halls/dev/chardev/display/qemudisp/qemudisp.h"

// ==================================
// dev/per/
#include "../halls/dev/per/dev00.h"

// ==================================
// gthouse/wink/ 
// sw - Graphics Engine
#include "../gthouse/wink/gdi/gre/color.h"
#include "../gthouse/wink/gdi/gre/font.h"
#include "../gthouse/wink/gdi/gre/bg.h"

// ==================================
// halls/dev/exec/ke/
// Can we move this up?
#include "../halls/dev/exec/ke/intake/msgcode.h"

// ==================================
// gthouse/wink/

#include "../gthouse/wink/gdi/gre/pixel.h"
#include "../gthouse/wink/gdi/gre/char.h"
#include "../gthouse/wink/gdi/gre/text.h"
#include "../gthouse/wink/gdi/gre/line.h"
#include "../gthouse/wink/gdi/gre/rect.h"
#include "../gthouse/wink/gdi/gre/bitblt.h"
#include "../gthouse/wink/gdi/gre/surface.h"
#include "../gthouse/wink/gdi/gre/gre.h"

#include "../gthouse/wink/gdi/dispsrv.h"
#include "../gthouse/wink/gdi/osshell.h"

#include "../gthouse/wink/gdi/wproxy.h"
#include "../gthouse/wink/gdi/gdi.h"

// Event Interface
#include "../gthouse/wink/evi/obroker.h"
#include "../gthouse/wink/evi/output.h"
#include "../gthouse/wink/evi/ibroker.h"
#include "../gthouse/wink/evi/input.h"

// ===========

#include "../halls/dev/exec/ke/intake/disp/callback.h"

// ==================================
// dev/per/

// dev/chardev/
// Serial port. (COM).
#include "../halls/dev/chardev/serial/serial.h"

#include "../halls/dev/chardev/vk.h"
#include "../halls/dev/chardev/kbdmaps/kbdmaps.h"
#include "../halls/dev/chardev/kbdmaps/ptbr/mapabnt2.h"
// ...

// i8042 (PS/2)
#include "../halls/dev/chardev/i8042/keyboard.h"
#include "../halls/dev/chardev/i8042/ps2kbd.h"
#include "../halls/dev/chardev/i8042/mouse.h"
#include "../halls/dev/chardev/i8042/ps2mouse.h"
#include "../halls/dev/chardev/i8042/i8042.h"

// blkdev/
// Block devices
// ata, sata
#include "../halls/dev/per/blkdev/ata/ata.h"
//#include "../halls/dev/per/blkdev/ahci/ahci.h"
// Storage manager.
#include "../halls/dev/per/blkdev/storage.h"

// netdev/
// Network devices
// primeiro controladoras depois protocolos
// e1000 - nic intel
#include "../halls/dev/per/netdev/e1000/e1000.h"

// ==================================
// dev/per/net/ 
// (network, protocols and socket)
// network
#include "../halls/dev/per/net/mac.h"
#include "../halls/dev/per/net/host.h"
#include "../halls/dev/per/net/in.h"
#include "../halls/dev/per/net/un.h"

//
// Protocols
//

// =================================
// prot/

// Core protocols
#include "../halls/dev/per/net/prot/core/ethernet.h"
#include "../halls/dev/per/net/prot/core/arp.h"
#include "../halls/dev/per/net/prot/core/ip.h"
// Commom protocols
#include "../halls/dev/per/net/prot/tcp.h"
#include "../halls/dev/per/net/prot/udp.h"
#include "../halls/dev/per/net/prot/dhcp.h" 
#include "../halls/dev/per/net/prot/gprot.h"


// Extra protocols
#include "../halls/dev/per/net/prot/icmp.h" 

// dev/per/net/

// Network

#include "../halls/dev/per/net/nports.h"     //(network) Network Ports  (sw)
#include "../halls/dev/per/net/inet.h"

#include "../halls/dev/per/net/socklib.h"     //
#include "../halls/dev/per/net/socket.h"      //last always

#include "../halls/dev/per/net/domain.h"

#include "../halls/dev/per/net/ifconfig/netif.h"  // Network interface
#include "../halls/dev/per/net/net.h"     //(network) Gerenciamento de rede.  

// ----------------------
// Last:
// Device interface.
// Device manager.
#include "../halls/dev/per/dev.h"

// ==================================
// dev/per/fs/
// File system
// ----------------------
// Depois de devices.
// fs
#include "../halls/dev/per/fs/path.h"      // path.

#include "../halls/dev/per/fs/fat/fatlib.h"    // fat16 library.
#include "../halls/dev/per/fs/fat/fat.h"       // fat16.

#include "../halls/dev/per/fs/inode.h"
#include "../halls/dev/per/fs/exec_elf.h"
#include "../halls/dev/per/fs/pipe.h"
#include "../halls/dev/per/fs/files.h"
#include "../halls/dev/per/fs/fs.h"

// ==================================
#include "../halls/res.h"

// ==================================
// intake/
#include "../halls/dev/exec/ke/intake/prio.h"     // Priority
#include "../halls/dev/exec/ke/intake/quantum.h"  // Quantum
#include "../halls/dev/exec/ke/intake/image.h"
#include "../halls/dev/exec/ke/intake/disp/x86_64/x64cont.h"
#include "../halls/dev/exec/ke/intake/disp/ts.h"
#include "../halls/dev/exec/ke/intake/queue.h"
#include "../halls/dev/exec/ke/intake/intake.h"
#include "../halls/dev/exec/ke/intake/disp/spawn.h"
#include "../halls/dev/exec/ke/intake/disp/dispatch.h"

#include "../halls/dev/exec/ke/intake/thread.h"
#include "../halls/dev/exec/ke/intake/te.h"
#include "../halls/dev/exec/ke/intake/ithread.h"
#include "../halls/dev/exec/ke/intake/clone.h"
#include "../halls/dev/exec/ke/intake/broker/ipc.h"

#include "../halls/dev/exec/ke/intake/sched/sched.h"
#include "../halls/dev/exec/ke/intake/sched/schedq.h"

// Precisa de todos os componentes de ke/
#include "../halls/dev/exec/ke/ke.h"

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
#include "../halls/dev/exec/ke/hal/x86_64/x64sc.h"

// ==================================

#include "../gthouse/kwrap/wrappers.h"
#include "../gthouse/kwrap/panic.h"

// cgroups and namespaces
#include "../gthouse/kwrap/cont/cg.h"
#include "../gthouse/kwrap/cont/ns.h"

// Core module.
// It controls the resorces in halls/.
#include "../gthouse/core.h"

