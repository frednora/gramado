
// kpid.h
// Managing the helper processes.

#ifndef __KE_KPID_H
#define __KE_KPID_H    1

//
// Main processes
//

#define GRAMADO_PID_KERNEL  0
#define GRAMADO_PID_INIT    1
// ...
#define GRAMADO_PID_BASE  GRAMADO_PID_KERNEL

// See:
// process.c

// 1) boot/
// This is the pre-kernel stuff.
// Register this folder is gonna be a noop.
// oops. pode existir nessa pasta também um processo
// de configuração da inicialização. why not?!
extern pid_t __gpidBoot;
//int register_bootsrv_process ( pid_t pid );

// 2) include/
extern pid_t __gpidInclude;
//int register_include_process ( pid_t pid ); //Noop!

// 3) init/
// init.bin
extern pid_t __gpidInit;
//int register_init_process ( pid_t pid );

// 4) install/
extern pid_t __gpidInstall;
//??

// 5) logoff
extern pid_t __gpidLogoff;
//int register_logoff_process ( pid_t pid );

// 6) logon/
extern pid_t __gpidLogon;
//int register_logon_process ( pid_t pid );

// 7) net/
// The net server.
// netsrv.bin
extern pid_t __gpidNetServer;
//int register_netsrv_process ( pid_t pid );

// 8) notify/
extern pid_t __gpidNotificationServer;
//int register_notify_process ( pid_t pid );

// 9) sci/
// SCIServer ??
extern pid_t __gpidSCI;
// ??

// 10) security/
extern pid_t __gpidSecurityServer;
// ??

// 11) 
extern pid_t __gpidSysIO;
// ??

// 12)
extern pid_t __gpidSysLib;
// ??

// 13) sysmk/
extern pid_t __gpidSysMK;
// ??

// 14) syssm/ 
//system management
// system server
extern pid_t __gpidSysSM;
// ??

// 15) wm/
// See: kgwm.c
extern pid_t __gpidWindowManager;
//int register_wm_process ( pid_t pid );

// 16) ws/
// See: dev/fb/gr/graphics.c
extern pid_t __gpidWindowServer;
int register_ws_process(pid_t pid);

#endif    


