//====================================================
// sysinf.bin - Simple system information for Gramado OS
//====================================================
// Shows basic system information using the current rtl.
// Place it under: gramde/cmds/cmds00/sysinf/
//====================================================

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>

// Optional (if your libc has it working)
#include <sys/utsname.h>

// ----------------------------------------------------
// Helpers
// ----------------------------------------------------

static void print_separator(void)
{
    printf("----------------------------------------\n");
}

static void print_uptime(unsigned long jiffies)
{
    // Very rough approximation.
    // We treat jiffies as roughly milliseconds for display purposes.
    // Adjust the divisor later when you know the real tick rate.
    unsigned long total_sec = jiffies / 1000;
    unsigned long hours, minutes, seconds;

    hours   = total_sec / 3600;
    minutes = (total_sec % 3600) / 60;
    seconds = total_sec % 60;

    printf("Uptime (approx):     %lu h %lu m %lu s\n", hours, minutes, seconds);
    printf("Jiffies:             %lu\n", jiffies);
}

// ----------------------------------------------------
// main
// ----------------------------------------------------

int main(int argc, char *argv[])
{
    unsigned long mem_kb = 0;
    unsigned long jiffies = 0;
    unsigned long progress = 0;
    pid_t   pid = 0;
    int     tid = 0;
    int     is_qemu = 0;
    struct utsname un;

    (void)argc;
    (void)argv;

    // Clear structure
    memset(&un, 0, sizeof(un));

    // Try to get uname info (may be empty on some builds)
    uname(&un);   // uncomment if your libc supports it reliably

    printf("\n");
    printf("Gramado System Information (sysinf)\n");
    print_separator();

    // ----- Identity -----
    if (un.nodename[0] != '\0') {
        printf("Hostname:            %s\n", un.nodename);
    } else {
        printf("Hostname:            (unknown)\n");
    }

    if (un.sysname[0] != '\0') {
        printf("System:              %s\n", un.sysname);
    } else {
        printf("System:              Gramado OS\n");
    }

    if (un.release[0] != '\0') {
        printf("Release:             %s\n", un.release);
    }

    if (un.version[0] != '\0') {
        printf("Version:             %s\n", un.version);
    }

    if (un.machine[0] != '\0') {
        printf("Machine:             %s\n", un.machine);
    } else {
        printf("Machine:             x86_64\n");
    }

    print_separator();

    // ----- Memory -----
    mem_kb = rtl_memory_size_in_kb();
    printf("Memory size:         %lu KB", mem_kb);
    if (mem_kb > 1024) {
        printf("  (~%lu MB)", mem_kb / 1024);
    }
    printf("\n");

    // ----- Time / Uptime -----
    jiffies  = rtl_jiffies();
    progress = rtl_get_progress_time();   // usually also based on jiffies

    print_uptime(jiffies);
    printf("Progress time:       %lu\n", progress);

    print_separator();

    // ----- Current process / thread -----
    pid = rtl_current_process();
    tid = rtl_current_thread();

    printf("Current PID:         %d\n", (int)pid);
    printf("Current TID:         %d\n", tid);

    print_separator();

    // ----- Environment detection -----
    is_qemu = rtl_is_qemu();
    printf("Running on QEMU:     %s\n", is_qemu ? "yes" : "no");

    // Extra metrics that are known to exist
    // (safe to call even if some return 0)
    printf("Metric[140] (PID):   %lu\n", rtl_get_system_metrics(140));
    printf("Metric[141] (TID):   %lu\n", rtl_get_system_metrics(141));
    printf("Metric[118] (time):  %lu\n", rtl_get_system_metrics(118));
    printf("Metric[300] (QEMU):  %lu\n", rtl_get_system_metrics(300));

    print_separator();
    printf("sysinf: done.\n\n");

    return 0;
}
