
IDT Castle - IDT is king 

Gatehouse = entry + preparation
Halls     = management + execution

=========================

kernel/
 ├── gthouse/     # Gatehouse: entry point, stubs, init, wrappers, libk, UI
 │    ├── arch/
 │    ├── kwrap/
 │    ├── libk/
 │    └── wink/
 ├── halls/       # Castle halls: execution, requests, peripherals
 │    ├── exec/   (cpu hall)
 │    ├── per/    (irq hall) 
 │    └── req/    (memory hall)
 └── include/     # Castle library: headers, definitions, shared interfaces


=========================

kernel/
 ├── gthouse/     # Gatehouse: entry point, stubs, init, wrappers, libk, UI
 │    ├── arch/   # Architecture-specific boot & interrupt stubs
 │    ├── kwrap/  # Kernel wrappers (high-level abstractions)
 │    ├── libk/   # Kernel library (shared utilities)
 │    └── wink/   # User interface (input events, GDI)
 ├── halls/       # Castle halls: execution, requests, peripherals
 │    ├── exec/   # CPU Hall: threads, dispatcher, scheduler, exceptions
 │    ├── per/    # Peripheral Hall: external devices, drivers
 │    └── req/    # Memory Hall: syscalls, memory management (mm/)
 └── include/     # Castle library: headers, definitions, shared interfaces

=========================

Entry Paths into the Kernel (all in gthouse/)

>> Entry Point (boot/init)
 The very first code that runs when the system starts.
 Sets up the castle before anyone else arrives.

>> Syscalls
 Formal petitions from user space.
 They knock at the gatehouse door, and the stubs decide which hall to send them to.

>> IRQs (Interrupt Requests)
 Hardware events arriving unexpectedly.
 Like messengers rushing to the gatehouse with urgent news.

>> Ring 0 Modules (direct calls)
 Kernel modules already inside the castle walls.
 They bypass the outer world but still enter through the gatehouse interface to stay consistent.

---------------------------------
gthouse/ Layout

arch/
 Architecture‑specific entry code.
 Boot sequences, interrupt stubs, low‑level assembly for x86, ARM, etc.
 This is where the castle’s drawbridge mechanics live — different designs for different lands (architectures).

kwrap/
 Kernel wrappers.
 High‑level abstractions around raw kernel functions.
 Acts like the clerks in the gatehouse who translate raw petitions into proper requests.

libk/
 Kernel library.
 Shared utility functions used across halls.
 The tool chest stored in the gatehouse for common needs (string ops, math helpers, logging).

wink/
 Windows/Interface kernel layer.
 User interface handling: input events (keyboard, mouse) and graphics device interface (GDI).
 This is the literal front door of the castle where users interact with the system.

------------------------------------------------------
 halls/exec/ Responsibilities

Threads
 Data structures and APIs for creating, managing, and destroying threads.
 The “workers” inside the Execution Hall.

Dispatcher
 Logic for context switching and handing control to the right thread.
 The steward who decides which worker steps forward.

Scheduler
 Algorithms for deciding which thread runs next (round‑robin, priority, etc.).
 The steward’s strategy manual for keeping the hall efficient.

Exceptions / Traps
 Handling CPU‑generated events (divide by zero, page faults).
 The guards inside the hall who respond when something goes wrong.

--------------------------------------------------

