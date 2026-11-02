

panic()   → the first worker, console/serial oriented, using printk and PROGRESS.
x_panic() → the second worker, framebuffer/UI oriented, drawing a red box and halting.


/*
 * Panic Workers Documentation
 *
 * Overview:
 * =========
 * The kernel provides two distinct panic routines, each designed for a
 * different stage of system initialization and output capability.
 *
 * 1. panic()
 * ----------
 * - Location: panic.c
 * - Context: Used when the kernel virtual console is already initialized.
 * - Behavior:
 *   + Accepts formatted strings, including '\n' line breaks.
 *   + Prints messages using printk() into the active virtual console.
 *   + Also logs to the serial port if available.
 *   + After printing, halts the system via keDie().
 * - Typical usage:
 *   panic("Scheduler failed to start\n");
 *
 * 2. x_panic()
 * ------------
 * - Location: wink.c
 * - Context: Used *before* the kernel virtual console is initialized.
 * - Behavior:
 *   + Does not rely on printk() or console structures.
 *   + Draws a red rectangle at the top of the framebuffer.
 *   + Prints a short panic message directly into the framebuffer.
 *   + Flushes the screen and halts the system with cli/hlt.
 * - Typical usage:
 *   x_panic("earlyinit failure");
 *
 * Design Rationale:
 * =================
 * - During early boot, console and printk support may not yet be available.
 *   In this case, x_panic() provides a safe visual fallback using raw
 *   framebuffer drawing.
 * - Once the console subsystem is initialized, panic() becomes the preferred
 *   worker, as it supports formatted output, line breaks, and serial logging.
 *
 * Error Code Integration:
 * =======================
 * - Both workers can be wrapped by error-code based helpers:
 *   panic_print_error(Error31_kePhase1);
 *   x_panic_error(Error20_mmPhase0);
 * - This allows standardized error reporting tied to initialization levels,
 *   while still respecting the available output path (console vs framebuffer).
 *
 * Summary:
 * =========
 * - Use x_panic() for early failures (before console init).
 * - Use panic() for later failures (after console init).
 * - Both halt the system after displaying the panic message.
 */
