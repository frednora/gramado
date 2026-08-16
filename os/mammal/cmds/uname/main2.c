// UNAME.BIN
// 2025 - Fred Nora.
// Minimal, portable uname-like utility for hobby OS.

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

// Optional domain name support (Linux/glibc, some BSDs)
// If your libc provides getdomainname(), define HAVE_GETDOMAINNAME.
//#ifdef __linux__
//#define HAVE_GETDOMAINNAME 0
//#endif

//#ifdef HAVE_GETDOMAINNAME
#include <unistd.h>   // getdomainname()
//#endif

static void usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s [OPTION]\n"
        "Print system information.\n\n"
        "Options:\n"
        "  -a  Print all of the following: -s -r -v -m -n\n"
        "  -s  Print the kernel name (sysname)\n"
        "  -r  Print the kernel release\n"
        "  -v  Print the kernel version\n"
        "  -m  Print the machine hardware name\n"
        "  -n  Print the nodename (network hostname)\n"
        "  -h  Show this help\n",
        prog ? prog : "uname");
}

int main(int argc, char **argv)
{
    struct utsname un;
    int i;

    // Flags
    int fAll = 0;
    int fSysName = 0;
    int fRelease = 0;
    int fVersion = 0;
    int fMachine = 0;
    int fNodeName = 0;

    // Default behavior: print sysname when no args
    if (argc == 1) {
        fSysName = 1;
    } else {
        for (i = 1; i < argc; i++) {
            const char *arg = argv[i];

            // Stop on non-flag
            if (arg[0] != '-') {
                usage(argv[0]);
                return EXIT_FAILURE;
            }

            // Single-letter flags only
            if (strcmp(arg, "-a") == 0) {
                fAll = 1;
            } else if (strcmp(arg, "-s") == 0) {
                fSysName = 1;
            } else if (strcmp(arg, "-r") == 0) {
                fRelease = 1;
            } else if (strcmp(arg, "-v") == 0) {
                fVersion = 1;
            } else if (strcmp(arg, "-m") == 0) {
                fMachine = 1;
            } else if (strcmp(arg, "-n") == 0) {
                fNodeName = 1;
            } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
                usage(argv[0]);
                return EXIT_SUCCESS;
            } else {
                fprintf(stderr, "uname: unknown option: %s\n", arg);
                usage(argv[0]);
                return EXIT_FAILURE;
            }
        }
    }

    // If -a, enable all primary fields
    if (fAll) {
        fSysName = 1;
        fRelease = 1;
        fVersion = 1;
        fMachine = 1;
        fNodeName = 1;
    }

    // Call uname and check
    if (uname(&un) < 0) {
        perror("uname");
        return EXIT_FAILURE;
    }

    // Print selected fields, each on its own line for clarity
    if (fSysName)  printf("sysname:  %s\n", un.sysname);
    if (fRelease)  printf("release:  %s\n", un.release);
    if (fVersion)  printf("version:  %s\n", un.version);
    if (fMachine)  printf("machine:  %s\n", un.machine);
    if (fNodeName) printf("nodename: %s\n", un.nodename);

/*
    // Optional domain name (non-POSIX). Hide behind a feature macro.
#ifdef HAVE_GETDOMAINNAME
    {
        char domain[256];
        if (getdomainname(domain, sizeof(domain)) == 0 && domain[0] != '\0') {
            printf("domainname: %s\n", domain);
        }
    }
#endif
*/

    return EXIT_SUCCESS;
}
