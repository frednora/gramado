
---

# Kernel Initialization Routine (Detailed) -  by Copilot

This document tracks the initialization sequence of the Gramado kernel, 
starting from the assembly bootstrap and continuing through all 
phases until userland is launched. Each stage lists the functions called and their purpose.

---

## Main Map

```text
Levels:
+ [1]   earlyinit()
+ [2:0] mmInitialize(0)
+ [2:1] mmInitialize(1)
+ [3:0] keInitialize(0)
+ [3:1] keInitialize(1)
+ [3:2] keInitialize(2)
+ [4]   archinit()
+ [5]   deviceinit()
+ [6]   lateinit()
```

---

## Initialization Flow

1. **Assembly bootstrap (`head_64.asm`)**  
   Switches CPU into long mode, sets up paging and stack, then calls `I_kmain()`.

2. **`I_kmain()` (kmain.c)**  
   First C entry point. Calls `I_initialize_kernel()` to perform staged initialization.

3. **`I_initialize_kernel()` sequence**

---

### [1] `earlyinit()` (kmain.c)
- `earlyinit_SetupBootblock()` – Reads boot block info (framebuffer, resolution, memory size, Gramado mode).  
- `earlyinit_Globals()` – Resets scheduler, process, and thread globals.  
- `earlyinit_Serial()` – Initializes serial driver for early debug output.  
- `earlyinit_OutputSupport()` – Sets up basic virtual console structures (not full printk yet).  
**Purpose:** Minimal environment setup, early debug output, basic console structures.

---

### [2:0] `mmInitialize(0)` – Memory Manager Phase 0 (mm.c)
- `wink_initialize_video()` – Initializes video support structures.  
- `__init_kernel_heap()` – Defines kernel heap start/end, sets heap pointer.  
- `__init_kernel_stack()` – Defines kernel stack start/end.  
- Clear `mmblockList[]` – Resets memory block tracking.  
- `mmsize_initialize()` – Calculates physical memory size.  
**Purpose:** Establish heap and stack, determine available RAM.

---

### [2:1] `mmInitialize(1)` – Memory Manager Phase 1 (mm.c)
- `initializeFramesAlloc()` – Sets up frame allocator (paged pool).  
- `pagesInitializePaging()` – Builds page tables, maps static areas, enables paging.  
**Purpose:** Enable paging and dynamic allocation (`kmalloc`).

---

### [3:0] `keInitialize(0)` – Debug and Display Support (ke.c)
- `wink_initialize_default_kernel_font()` – Loads default kernel font.  
- `wink_initialize_background()` – Sets up background graphics.  
- `__check_refresh_support()` – Enables refresh/flush support for console.  
- `wink_show_banner(FALSE)` – Prints kernel banner.  
- `__print_resolution_info()` – Displays resolution info.  
- `__check_gramado_mode()` – Validates Gramado mode.  
- `__import_data_from_linker()` – Imports section size data from linker.  
- `displayInitialize()` – Initializes display controller drivers.  
**Purpose:** Console logging, display initialization, banner and resolution info.

---

### [3:1] `keInitialize(1)` – Process and Thread Support (ke.c)
- `init_dispatch()` – Initializes dispatcher.  
- `init_scheduler(0)` – Initializes scheduler.  
- `init_ts()` – Initializes task switching.  
- `init_processes()` – Sets up process structures.  
- `init_threads()` – Sets up thread structures.  
- `I_x64_initialize()` – Architecture‑specific initialization.  
**Purpose:** Process and thread support, scheduler, dispatcher, architecture‑specific setup.

---

### [3:2] `keInitialize(2)` – Graphics and GUI Support (ke.c)
- `gre_initialize()` – Initializes graphics engine.  
- Allocate `gui_d` – Creates GUI structure.  
- `ds_init()` – Initializes display server registration.  
- `callbackInitialize()` – Sets up callback support.  
- `wink_initialize_background()` – Clears background again.  
- `winkLoadGramadoIcons()` – Loads BMP icons.  
**Purpose:** Graphics and GUI support, desktop structures, display server callbacks.

---

### [4] `archinit()` (kmain.c)
- `hal_probe_processor_type()` – Detects processor type.  
- `x64smp_initialization()` – Initializes SMP support.  
- `enable_apic()` – Enables APIC.  
- `Send_STARTUP_IPI_Twice()` – Starts secondary processors.  
**Purpose:** Architecture‑specific setup: GDT, IDT, APIC/IOAPIC, SMP probing, secondary processor startup.

---

### [5] `deviceinit()` (kmain.c)
- `DDINIT_ps2_early_initialization()` – Initializes PS/2 keyboard/mouse.  
- `input_enable_this_input_target()` – Sets input targets (stdin, thread queue).  
- `devInitialize()` – Clears device list.  
- `libk_initialize()` – Initializes libc support, registers consoles (`tty0`–`tty3`).  
**Purpose:** Device manager setup, PS/2 devices, input targets, console registration.

---

### [6] `lateinit()` (kmain.c)
- `tty_initialize_legacy_pty()` – Creates legacy PTYs.  
- `userInitializeStuff()` – Initializes user subsystem.  
- `userCreateRootUser()` – Creates root user.  
- `netInitialize()` – Initializes networking.  
- `__setup_utsname()` – Sets up utsname structure.  
- `mod_initialize()` – Initializes kernel modules.  
- `ke_x64ExecuteInitialProcess()` – Launches `init` process.  
**Purpose:** User subsystem, root user, networking, utsname setup, kernel modules, first userland process.

---

## End of Initialization
- Kernel enters steady state.  
- Userland `init` process runs.  
- Scheduler and multitasking fully enabled.  
- Devices and subsystems are active.

---
