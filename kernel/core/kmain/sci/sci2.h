
// sci2.h

#ifndef __SYSCALLS_SCI2_H
#define __SYSCALLS_SCI2_H    1

// The handler is sci2().

#define SCI2_NULL  SCI_NULL


// 1 - Set magic (in kernel console)
// 2 - Get magic (in kernel console)
// 3 - Get system metrics
// 4 - ioctl() handler.
// 5 - Business Logic: fcntl() implementation.

// 18 - Business Logic: read() implementation.
// 19 - Business Logic: write() implementation.

// 265 - yield
// Business Logic: 
//  + Yield 
//  + Set a flag that this thread will be preempted.
// See: sched/schedi.c

// 266 - sleep
// Business Logic: 
// Sleep until.
// #todo: Explaint it better here.
// #bugbug
// We cant drastically change the state of a thread,
// we need to schedule that operation.
// given to the ts the opportunity to do that 
// in the right moment. As well as we do for the yield operation.
// Agendando a operação de sleep.
// O ts vai fazer isso quando for seguro.
// IN: tid, ms

// 777 - nice

// 900 - copy process
// Business Logic: 
// Clona e executa o filho dado o nome do filho.
// O filho inicia sua execução do início da imagem.
// #bugbug: Isso às vezes falha na máquina real.
// #todo: Use more arguments.
// See: clone.c
// IN: file name, parent pid, clone flags.
// OUT: Child's PID.

// 8000 - Business Logic: ioctl() implementation.
// See: fs.c
// IN: fd, request, arg

// 8001 - Business Logic: fcntl() implementation. 
// (second time) see: number 5.
// See: sys.c

// 8003
// Business Logic: 
// Clear the fg console background with a given color.
// Do not change the colors.

// 8004
// Business Logic: 
// Change the foreground color of the current console.

// 10000 - sys_set_file_sync
// Business Logic: 
// Configurando sincronização de leitura e escrita em arquivo.
// principalmente socket.
// A estrutura de arquivo contém uma estrutura de sincronização de leitura e escrita.
// #ok: podemos usar ioctl
// See: sys.c

// 10001 - sys_get_file_sync
// Business Logic: 
// Pegando informação sobre sincronização de leitura e escrita de arquivos.
// principalmente para socket.
// A estrutura de arquivo contém uma estrutura de sincronização de leitura e escrita.
// #ok: podemos usar ioctl
// See: sys.c
// IN: fd, request


// Global sync - Not used anymore.
// 10002 - sys_set_global_sync
// 10003 - sys_get_global_sync
// 10004 - sys_create_new_sync
// 10005 - get_saved_sync

// Sync in file.
// 10006 - sys_set_file_sync
// 10007 - sys_get_file_sync


// 10008
// Business Logic:
// Save FAT cache into the disk.
// FAT cache.
// This is the FAT cache for the system disk.
// The boot partition.


// 10010 - Business Logic: Get the tid of the current thread.

// -----------------------------
// 10011
// Business Logic: 
// Set the foreground thread given it's tid.
// #todo: We need a method for that.
// IN: arg2=tid.

// 10020 - get GRAMADO_PID_INIT
// 10021 - get INIT_TID

// 22001 - Set network status.
// 22002 - Get network status.

// 22003 - Test some net component.

// 22011
// Business Logic:
// PS2 full initialization.

// 44000
// Business Logic:
// #important: We're avoiding the callback support.
// Callback support.
// see: ts.c
// see: pit.c
// arg2 = address
// arg3 = pid
// arg4 = signature

#endif   



