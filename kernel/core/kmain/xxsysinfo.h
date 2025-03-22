




// #todo
// Include this file in kernel.h


// sysinfo.h
// Information about the host system used during the compiling time.
//


struct windows_sysinfo_d
{
    int used;
    int magic;

    // ...
};

struct windows_sysinfo_d *windows_sysinfo;


