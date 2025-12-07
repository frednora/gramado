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
// halls/exec/ke/
#include "../halls/exec/ke/intake/sync.h"

// ===============================
// hal/
#include "../halls/exec/ke/hal/x86_64/gwd.h"  // whatch dogs.
#include "../halls/exec/ke/hal/pints.h"       // profiler

// ===============================
#include "../halls/exec/ke/intake/x64init.h"  // x64 kernel initialization.

// kernel initialization.
#include "../kmain.h"


// ===============================
// kwrap/
#include "../gthouse/kwrap/info.h"
#include "../gthouse/kwrap/request.h"

#include "../gthouse/kwrap/debug.h"

// ==================================
// halls/exec/ke/
// Gramado configuration.
#include "../halls/exec/ke/hal/jiffies.h"

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
// halls/exec/ke/
// Globals. PIDs support.
#include "../halls/exec/ke/intake/kpid.h"

// ==================================
// req/mm/
// Memory management.
#include "../halls/req/mm/mmsize.h"
#include "../halls/req/mm/x86_64/x64gpa.h"
#include "../halls/req/mm/x86_64/x64gva.h"
#include "../halls/req/mm/memmap.h" 
#include "../halls/req/mm/x86_64/intelmm.h"
#include "../halls/req/mm/mmblock.h"
#include "../halls/req/mm/mmusage.h"
#include "../halls/req/mm/x86_64/x64mm.h"
#include "../halls/req/mm/slab.h"
#include "../halls/req/mm/x86_64/paging.h"
#include "../halls/req/mm/mmft.h"
#include "../halls/req/mm/mmpool.h"
#include "../halls/req/mm/mmglobal.h"  // Deve ficar mais acima.
#include "../halls/req/mm/heap.h"      // Heap pointer support.
#include "../halls/req/mm/aspace.h"    // Address Space, (data base account).
#include "../halls/req/mm/bank.h"      // Bank. database
#include "../halls/req/mm/mm.h"

// ==================================
// hal/
#include "../halls/exec/ke/hal/x86_64/ports64.h"
#include "../halls/exec/ke/hal/x86_64/cpu.h"
#include "../halls/exec/ke/hal/x86_64/tss.h"
#include "../halls/exec/ke/hal/x86_64/x64gdt.h"
#include "../halls/exec/ke/hal/x86_64/x64.h"
#include "../halls/exec/ke/hal/detect.h"

// ==================================
// virt/
#include "../gthouse/kwrap/virt/hv.h"

// ==========================================
// hal/arm/
// #include "../halls/exec/ke/hal/arm/archhal.h"

// ==========================================
// hal/x86_64/
#include "../halls/exec/ke/hal/x86_64/cpuid.h"
#include "../halls/exec/ke/hal/x86_64/up/up.h"
#include "../halls/exec/ke/hal/x86_64/smp/mpfloat.h"
#include "../halls/exec/ke/hal/x86_64/smp/acpi.h"
#include "../halls/exec/ke/hal/x86_64/smp/x64smp.h"
#include "../halls/exec/ke/hal/x86_64/pic.h"
#include "../halls/exec/ke/hal/x86_64/smp/apic.h"
#include "../halls/exec/ke/hal/x86_64/smp/apictim.h"
#include "../halls/exec/ke/hal/x86_64/smp/ioapic.h"
#include "../halls/exec/ke/hal/x86_64/pit.h"
#include "../halls/exec/ke/hal/x86_64/rtc.h"
#include "../halls/exec/ke/hal/x86_64/breaker.h"
#include "../halls/exec/ke/hal/x86_64/archhal.h"

// ==========================================
// Architecture-independent HAL interface
#include "../halls/exec/ke/hal/hal.h"

// ==================================
// per/bus/
// PCI bus.
#include "../halls/per/bus/pci/pci.h"
#include "../halls/per/bus/bus.h"

// ==================================
// per/
// io
#include "../halls/per/io.h"

// ==================================
// per/
// Devices
// primeiro char, depois block, depois network.
// tty
#include "../halls/per/chardev/tty/ttyldisc.h"
#include "../halls/per/chardev/tty/ttydrv.h"
#include "../halls/per/chardev/tty/tty.h"
#include "../halls/per/chardev/tty/pty.h"

#include "../halls/per/chardev/console/console.h"

// hw stuff - display device
// display device support.
#include "../halls/per/chardev/display/display.h"
// bootloader display device
#include "../halls/per/chardev/display/bldisp/rop.h"
#include "../halls/per/chardev/display/bldisp/bldisp.h"
//#include "../halls/per/chardev/display/qemudisp/qemudisp.h"

// ==================================
// per/
#include "../halls/per/dev00.h"

// ==================================
// gthouse/wink/ 
// sw - Graphics Engine
#include "../gthouse/wink/gdi/gre/color.h"
#include "../gthouse/wink/gdi/gre/font.h"
#include "../gthouse/wink/gdi/gre/bg.h"

// ==================================
// halls/exec/ke/
// Can we move this up?
#include "../halls/exec/ke/intake/msgcode.h"

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

#include "../halls/exec/ke/intake/disp/callback.h"

// ==================================
// per/

// chardev/
// Serial port. (COM).
#include "../halls/per/chardev/serial/serial.h"

#include "../halls/per/chardev/vk.h"
#include "../halls/per/chardev/kbdabnt2.h"
#include "../halls/per/chardev/kbdmap.h"

// i8042 (PS/2)
#include "../halls/per/chardev/i8042/keyboard.h"
#include "../halls/per/chardev/i8042/ps2kbd.h"
#include "../halls/per/chardev/i8042/mouse.h"
#include "../halls/per/chardev/i8042/ps2mouse.h"
#include "../halls/per/chardev/i8042/i8042.h"

// blkdev/
// Block devices
// ata, sata
#include "../halls/per/blkdev/ata/ata.h"
//#include "../halls/per/blkdev/ahci/ahci.h"
// Storage manager.
#include "../halls/per/blkdev/storage.h"

// netdev/
// Network devices
// primeiro controladoras depois protocolos
// e1000 - nic intel
#include "../halls/per/netdev/e1000/e1000.h"

// ==================================
// per/net/ 
// (network, protocols and socket)
// network
#include "../halls/per/net/mac.h"
#include "../halls/per/net/host.h"
#include "../halls/per/net/in.h"
#include "../halls/per/net/un.h"

//
// Protocols
//

// =================================
// prot/

// Core protocols
#include "../halls/per/net/prot/core/ethernet.h"
#include "../halls/per/net/prot/core/arp.h"
#include "../halls/per/net/prot/core/ip.h"
// Commom protocols
#include "../halls/per/net/prot/tcp.h"
#include "../halls/per/net/prot/udp.h"
#include "../halls/per/net/prot/dhcp.h" 
#include "../halls/per/net/prot/gprot.h"


// Extra protocols
#include "../halls/per/net/prot/icmp.h" 

// per/net/

// Network
#include "../halls/per/net/nports.h"     //(network) Network Ports  (sw)
#include "../halls/per/net/inet.h"

#include "../halls/per/net/socklib.h"     //
#include "../halls/per/net/socket.h"      //last always

#include "../halls/per/net/domain.h"
#include "../halls/per/net/net.h"     //(network) Gerenciamento de rede.  

// ----------------------
// Last:
// Device interface.
// Device manager.
#include "../halls/per/dev.h"

// ==================================
// per/fs/
// File system
// ----------------------
// Depois de devices.
// fs
#include "../halls/per/fs/path.h"      // path.

#include "../halls/per/fs/fat/fatlib.h"    // fat16 library.
#include "../halls/per/fs/fat/fat.h"       // fat16.

#include "../halls/per/fs/inode.h"
#include "../halls/per/fs/exec_elf.h"
#include "../halls/per/fs/pipe.h"
#include "../halls/per/fs/files.h"
#include "../halls/per/fs/fs.h"

// ==================================
#include "../halls/res.h"

// ==================================
// intake/
#include "../halls/exec/ke/intake/prio.h"     // Priority
#include "../halls/exec/ke/intake/quantum.h"  // Quantum
#include "../halls/exec/ke/intake/image.h"
#include "../halls/exec/ke/intake/disp/x86_64/x64cont.h"
#include "../halls/exec/ke/intake/disp/ts.h"
#include "../halls/exec/ke/intake/queue.h"
#include "../halls/exec/ke/intake/intake.h"
#include "../halls/exec/ke/intake/disp/spawn.h"
#include "../halls/exec/ke/intake/disp/dispatch.h"

#include "../halls/exec/ke/intake/thread.h"
#include "../halls/exec/ke/intake/te.h"
#include "../halls/exec/ke/intake/ithread.h"
#include "../halls/exec/ke/intake/clone.h"
#include "../halls/exec/ke/intake/ipc.h"

#include "../halls/exec/ke/intake/sched/sched.h"
#include "../halls/exec/ke/intake/sched/schedq.h"

// Precisa de todos os componentes de ke/
#include "../halls/exec/ke/ke.h"

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
#include "../halls/exec/ke/hal/x86_64/x64sc.h"

// ==================================

#include "../gthouse/kwrap/wrappers.h"
#include "../gthouse/kwrap/panic.h"

// cgroups and namespaces
#include "../gthouse/kwrap/cont/cg.h"
#include "../gthouse/kwrap/cont/ns.h"

// Core module.
// It controls the resorces in halls/.
#include "../gthouse/core.h"

