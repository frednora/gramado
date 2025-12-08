
  This is the startup routine for a kernel in the 
ARM architecture.

// -----------------------------------------------------------------------------
// Context notes:
// -----------------------------------------------------------------------------
// AArch64 execution state (64‑bit mode).
//   - We know this because you're using xN registers (64‑bit) and wN (32‑bit).
//
// Privilege level:
//   - Direct MMIO access means you're in a privileged exception level.
//   - Most likely EL1 (bootloader / kernel) or EL3 (secure monitor firmware).
//   - EL0 (user mode) cannot touch device registers directly.
//
// Boot stage:
//   - The SoC's boot ROM has already run (set up clocks, memory, etc.).
//   - Control has been handed to your code (_start).
//   - This is your own first-stage loader, not yet an OS kernel.
//   - You're in "firmware/bootloader land," equivalent to x86 BIOS/bootloader.
// -----------------------------------------------------------------------------


