
What is a debugger in the context of a kernel?

-------------------------------------------------------
kd/
1. Kernel Debugging via Debugger (Interactive Debugging)
This is the formal debugging mode where the kernel cooperates with a debugger application (like WinDbg, GDB, or KGDB).
The kernel uses breakpoints (e.g., int 3 on x86) to halt execution.
When triggered, the kernel sends the thread’s context (registers, stack, memory state) to the connected debugger.

-------------------------------------------------------
klog/
2. Kernel Debugging via Logging (Diagnostic Debugging)
This is the informal debugging mode where developers instrument the kernel with logs or verbose messages.
The kernel prints diagnostic information to the console, log buffer, or screen.
Commonly done with functions like printk (Linux) or DbgPrint (Windows).
Printing verbose to the screen
Sending data to the serial port
Tracing / verbose modes
