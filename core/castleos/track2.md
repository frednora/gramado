---

# Kernel Initialization Routine - by Copilot

This document tracks the initialization sequence of the Gramado kernel, 
starting from the assembly bootstrap and continuing through all phases 
until userland is launched. Each stage is listed with the functions called and 
their responsibilities.

---

## Main Map

```text
// ==================================
// Main functions during the initialization:
// + ... startup_64() in x86_64/head_64.asm
// + ... start_kernel() in kmain.c       (wrapper)
// + ... x86_64_start_kernel in kmain.c  (wrapper)
// + [x] I_kmain() in kmain.c            (real initialization)
//
// Main function called by I_kmain(): in kmain.c
// + [1] I_initialize_kernel()
// + [2] I_x64CreateKernelProcess()
// + [3] I_x64CreateInitialProcess()
// + [4] ke_x64ExecuteInitialProcess()
//
// Main functions called by I_initialize_kernel(): in kmain.c
// + [1:1]   earlyinit()
// + [1:2:0] mmInitialize(0)
// + [1:2:1] mmInitialize(1)
// + [1:3:0] keInitialize(0)
// + [1:3:1] keInitialize(1)
// + [1:3:2] keInitialize(2)
// + [1:4]   archinit()
// + [1:5]   deviceinit()
// + [1:6]   lateinit()
// ==================================
```

---

## Initialization Flow

1. **Assembly bootstrap (`head_64.asm`)**  
   - Switches CPU into long mode.  
   - Sets up paging and stack.  
   - Passes control to `I_kmain()`.

2. **`[x] I_kmain()` (kmain.c)**  
   - First C entry point.  
   - Calls `I_initialize_kernel()` to perform staged initialization.

3. **`[1] I_initialize_kernel()` sequence**

---

### [1:1] `earlyinit()` (kmain.c)
- **Functions called:**
  - `earlyinit_SetupBootblock()` → Reads boot block info (framebuffer, resolution, memory size, Gramado mode).  
  - `earlyinit_Globals()` → Resets scheduler, process, and thread globals.  
  - `earlyinit_Serial()` → Initializes serial driver (`DDINIT_serial`).  
  - `earlyinit_OutputSupport()` → Sets up basic virtual console structures (`wink_initialize_virtual_consoles`).  
- **Purpose:** Minimal environment setup, early debug output, basic console structures.

---

### [1:2:0] `mmInitialize(0)` – Memory Manager Phase 0 (mm.c)
- **Functions called:**
  - `wink_initialize_video()`  
  - `__init_kernel_heap()` → Defines kernel heap start/end, sets heap pointer.  
  - `__init_kernel_stack()` → Defines kernel stack start/end.  
  - Clears `mmblockList[]`.  
  - `mmsize_initialize()` → Calculates physical memory size.  
- **Purpose:** Establish heap and stack, determine available RAM.

---

### [1:2:1] `mmInitialize(1)` – Memory Manager Phase 1 (mm.c)
- **Functions called:**
  - `initializeFramesAlloc()` (mmpool.c) → Sets up frame allocator.  
  - `pagesInitializePaging()` (pages.c) → Builds page tables, maps static areas, enables paging.  
- **Purpose:** Enable paging and dynamic allocation (`kmalloc`).

---

### [1:3:0] `keInitialize(0)` – Debug and Display Support (ke.c)
- **Functions called:**
  - `wink_initialize_default_kernel_font()`  
  - `wink_initialize_background()`  
  - `__check_refresh_support()` → Enables refresh/flush support.  
  - `wink_show_banner(FALSE)`  
  - `__print_resolution_info()`  
  - `__check_gramado_mode()`  
  - `__import_data_from_linker()`  
  - `displayInitialize()` (display.c)  
- **Purpose:** Console logging, display initialization, banner and resolution info.

---

### [1:3:1] `keInitialize(1)` – Process and Thread Support (ke.c)
- **Functions called:**
  - `init_dispatch()`  
  - `init_scheduler(0)`  
  - `init_ts()`  
  - `init_processes()`  
  - `init_threads()`  
  - `I_x64_initialize()` (x64init.c)  
- **Purpose:** Process and thread support, scheduler, dispatcher, architecture‑specific initialization.

---

### [1:3:2] `keInitialize(2)` – Graphics and GUI Support (ke.c)
- **Functions called:**
  - `gre_initialize()` (graphics engine)  
  - Allocate `gui_d` structure  
  - `ds_init()` (display server registration)  
  - `callbackInitialize()`  
  - `wink_initialize_background()` (clear screen again)  
  - `winkLoadGramadoIcons()` (load BMP icons)  
- **Purpose:** Graphics and GUI support, desktop structures, display server callbacks.

---

### [1:4] `archinit()` (kmain.c)
- **Functions called:**
  - `hal_probe_processor_type()`  
  - `x64smp_initialization()` (x64smp.c)  
  - `enable_apic()` (apic.c)  
  - `Send_STARTUP_IPI_Twice()` (AP startup)  
- **Purpose:** Architecture‑specific setup: GDT, IDT, APIC/IOAPIC, SMP probing, secondary processor startup.

---

### [1:5] `deviceinit()` (kmain.c)
- **Functions called:**
  - `DDINIT_ps2_early_initialization()` (i8042.c)  
  - `input_enable_this_input_target()` (input.c)  
  - `devInitialize()` (dev.c) → Clears device list.  
  - `libk_initialize()` (kstdio.c) → Initializes libc support, registers consoles (`tty0`–`tty3`).  
- **Purpose:** Device manager setup, PS/2 devices, input targets, console registration.

---

### [1:6] `lateinit()` (kmain.c)
- **Functions called:**
  - `tty_initialize_legacy_pty()` (pty.c)  
  - `userInitializeStuff()` (user.c)  
  - `userCreateRootUser()` (user.c)  
  - `netInitialize()` (net.c)  
  - `__setup_utsname()` (kmain.c)  
  - `mod_initialize()` (mod.c)  
  - `ke_x64ExecuteInitialProcess()` → Launches `init` process.  
- **Purpose:** User subsystem, root user, networking, utsname setup, kernel modules, first userland process.

---

## End of Initialization
- Kernel enters steady state.  
- Userland `init` process runs.  
- Scheduler and multitasking fully enabled.  
- Devices and subsystems are active.

---

