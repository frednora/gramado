// sci0.h
// The definitions for the syscall numbers.
// Created by Fred Nora

#ifndef __SCI_SCI0_H
#define __SCI_SCI0_H    1

// The handler is sci0().

#define SCI0_NULL  0

// Disk
#define SCI0_READ_LBA    1
#define SCI0_WRITE_LBA   2
#define SCI0_READ_FILE   3
#define SCI0_WRITE_FILE  4

// ring 0 Graphics
#define SCI0_SYS_VSYNC  5
#define SCI0_BUFFER_PUTPIXEL  6
#define SCI0_BUFFER_DRAWCHAR  7
#define SCI0_BUFFER_DRAWLINE  8
#define SCI0_BUFFER_DRAWRECT  9
// 10 =  Refresh rectangle
#define SCI0_REFRESHSCREEN  11 

// Network (RESERVED)
// 12,13,14,15
#define SCI0_NET_R1    12
#define SCI0_NET_R2    13
#define SCI0_NET_R3    14
#define SCI0_NET_R4    15

// Posix: File operations
#define SCI0_SYS_OPEN   16
#define SCI0_SYS_CLOSE  17
#define SCI0_SYS_READ   18
#define SCI0_SYS_WRITE  19

// Reserved: 20~23 - Buffers support 
#define SCI0_REFRESH_BUFFER1  20
#define SCI0_REFRESH_BUFFER2  21
#define SCI0_REFRESH_BUFFER3  22
#define SCI0_REFRESHSCREEN2   23

// Reserved: 24~28 - Surface support 
#define SCI0_24  24
#define SCI0_25  25
#define SCI0_26  26
#define SCI0_27  27
#define SCI0_28  28

// Reserved: 29 - String stuff
// Print string on backbuffer
#define SCI0_MY_BUFFER_PRINTSTRING  29

// Reserved: 30,31,32 - Putpixel?
// Print pixel on backbuffer
#define SCI0_MY_BUFFER_PUTPIXEL   30
#define SCI0_MY_BUFFER2_PUTPIXEL  31
#define SCI0_MY_BUFFER3_PUTPIXEL  32

// Reserved: 33 - free number
#define SCI0_33  33

// 34: Setup cursor position for the current virtual console
#define SCI0_WINK_SET_CURSOR  34

// 35 - free number
#define SCI0_35  35

// 36 - free number
#define SCI0_36  36

// 37 - free number
#define SCI0_37  37

// 38 - get host name
#define SCI0_GETHOSTNAME  38
// 39 - set host name 
#define SCI0_SETHOSTNAME  39

// 40 - get user name 
#define SCI0_GETUSERNAME  40
// 41 - Set user name 
#define SCI0_SETUSERNAME  41

// 42 - free

// 43 - Create an empty file
#define SCI0_CREATE_EMPTY_FILE  43

// 44 - Create an empty directory
#define SCI0_CREATE_EMPTY_DIRECTORY  44

// 45 - free
// 46 - free
// 47 - free
// 48 - free

// 49 - Show system info
#define SCI0_SHOW_SYSTEM_INFO  49

// ...

// 65 - Put a char in the current virtual console
#define SCI0_KGWS_PUTCHAR  65

// ...

//
//    Process and Thread 
//

// Process and thread support
#define  SCI0_EXIT  70  // 70 - Exit a thread
#define  SCI0_FORK  71  // 71 - sys_fork()
#define  SCI0_SYS_CREATE_THREAD  72  // 72 - Create thread
#define  SCI0_SYS_CREATE_PROCESS  73  // 73 - Create process

// Reserved 74~79
#define  SCI0_CLOSEALLPROCESSES  74
#define  SCI0_EXECVE           75
#define  SCI0_GETNEXTPROCESS   76
#define  SCI0_SETNEXTPROCESS   77
#define  SCI0_GETNEXTTHREAD    78
#define  SCI0_SETNEXTTHREAD    79

// Process support
#define SCI0_CURRENTPROCESSINFO  80  // Informações sobre o processo atual.
#define SCI0_GETPPID             81  // get parent process id.

// 82 - Show information about all the processes

// 83 - Wait for PID
// Wait for process termination.
// A thread atual vai esperar até que um processo termine.
#define SCI0_WAIT4PID  83
#define SCI0_WAIT4     83

// 84 - free
#define SCI0_84  84

// 85 - Get current process ID
#define SCI0_GETPID  85

// 86 - free
#define SCI0_86  86

// 87 - Get current thread id

// 88 - Testing process validation?

// 89 - free
#define SCI0_89  89

// ------------------
// 90~99 Reserved for thread support
#define SCI0_DEADTHREADCOLLECTOR  90
#define SCI0_ALERTTHREAD  91
#define SCI0_92  92
#define SCI0_93  93
#define SCI0_STARTTHREAD  94  // 94 - Start thread
#define SCI0_95  95
#define SCI0_96  96
#define SCI0_97  97
#define SCI0_RESUMETHREAD  98
#define SCI0_99  99
// ------------------

// 100~109: free

//---------------
// 110 - Reboot
#define SCI0_SYS_REBOOT  110

// 111 - Get system message
#define SCI0_SYS_GET_MESSAGE  111

// 112 - Post message to tid - (Asynchronous)
#define SCI0_SYS_POST_MESSAGE_TO_TID  112

// Reserved (113~117)

// 118 - Pop data from network queue
// 119 - Reserved (Push data into the network queue?)

//------------

// 120 - Get a message given the index.
// With restart support.
#define SCI0_SYS_GET_MESSAGE2  120

// 121 - Signal stuff?
// 122 - Signal stuff?

// 123 - free

// 124 - Defered system procedure call.

// 125 - free

// 126 - i/o port in, via ring syscall.
#define SCI0_PORTSX86_IN  126
// 127 - i/o port out, via ring syscall.
#define SCI0_PORTSX86_OUT  127

// 128~129: free

// --------------------

// 130~131: free

// 132 - Draw a char
// Desenha um caractere e pinta o pano de fundo.

// 133 - Draw a transparent char.
// Desenha um caractere sem alterar o pano de fundo.

// 134~137: free

// 138 - Get key state
#define SCI0_GET_KEY_STATE  138

// 139 - Get scancode
#define SCI0_GETSCANCODE   139  

// -------------------

// 140~149 (Keyboard stuff?)
#define SCI0_SET_CURRENT_KEYBOARD_RESPONDER 140
#define SCI0_GET_CURRENT_KEYBOARD_RESPONDER 141
#define SCI0_SET_CURRENT_MOUSE_RESPONDER 142
#define SCI0_GET_CURRENT_MOUSE_RESPONDER 143
#define SCI0_GETCLIENTAREARECT 144
#define SCI0_SETCLIENTAREARECT 145
#define SCI0_146  146    // retorna o ponteiro para gui->screen 
#define SCI0_147  147    // retorna o ponteiro para gui->main
#define SCI0_148 148 //create grid
#define SCI0_149 149 //initialize grid.

// -----------------

// 150~156 User and group support.
#define SCI0_CREATEUSER         150  // 150 - Create user
#define SCI0_SETCURRENTUSERID   151  // 151 - Set current user id
#define SCI0_GETCURRENTUSERID   152
#define SCI0_SETCURRENTGROUPID  153
#define SCI0_GETCURRENTGROUPID  154
#define SCI0_UPDATEUSERINFO     155
#define SCI0_SHOWUSERINFO       156

// 157 - get user session id
#define SCI0_GETCURRENTUSERSESSION  157 // user session
// 158 - free
// 159 - get current cgroup id (#todo: Change name)
#define SCI0_GETCURRENTDESKTOP  159 // cgroup ?

// ------------

// 160 - free

// 161 - get socket IP
// Gramado API socket support. (not libc)

// 162 - get socket port
// Gramado API socket support. (not libc)

// 163 - update socket  
// retorno 0=ok 1=fail
// Gramado API socket support. (not libc)

// 164~169: free

// -----------------

// 170 - sys_pwd() (Print Working Directory)
// Comand 'pwd'.
#define SCI0_PWD  170

// 171 - Get current volume id
#define SCI0_GETCURRENTVOLUMEID  171

// 172 - Set current volume id
#define SCI0_SETCURRENTVOLUMEID  172

// 173 - List files
// Listar os arquivos do diretório. 
// Dados ids de disco, volume e diretório.
#define SCI0_LISTFILES  173

// 174 - Search file
#define SCI0_SEARCHFILE  174

// 175 - 'cd' command support.
// 176 - pathname backup string.
// 177 - 'dir' command.
// 178 - Get file size
// 179 - free

// -----------------
// 180~189: memory support.

// Memory support.
// Precisa de privilágios.
#define SCI0_CREATEPAGEDIRECTORY    180  // cria um pagedir.
#define SCI0_CREATEPAGETABLE        181  // cria uma pagetable.
#define SCI0_SHOWMEMORYSTRUCTS      182  // mostra estruturas de gerencia de memória.
#define SCI0_SETCR3                 183  // configura o cr3 do processo atual.
#define SCI0_GETPROCESSHEAPPOINTER  184  // pega o heap pointer de um processo.
#define SCI0_SETKERNELHEAP          185  // configura o heap do kernel.
#define SCI0_ALLOCATEKERNELHEAP     186  // aloca heap do kernel.
#define SCI0_FREEKERNELHEAP         187  // Libera heap do kernel.
#define SCI0_GETPROCESSDIRECTORY    188  // get process directory.
#define SCI0_SETPROCESSDIRECTORY    189  // set process directory.

//----------
// 190~199: free

//----------
// 200~209: free

// -------------------
// 210~219: terminal/virtual console support.
// #bugbug: 
// Isso provavelmente são rotinas de console virtual
// e não de pseudo terminal
// Terminal emulator support.
#define SCI0_CREATETERMINAL          210
#define SCI0_GETCURRENTTERMINAL      211
#define SCI0_SETCURRENTTERMINAL      212
#define SCI0_GETTERMINALINPUTBUFFER  213
#define SCI0_SETTERMINALINPUTBUFFER  214
#define SCI0_GETTERMINALWINDOW       215
#define SCI0_SETTERMINALWINDOW       216
#define SCI0_GETTERMINALRECT         217
#define SCI0_SETTERMINALRECT         218
#define SCI0_DESTROYTERMINAL         219

// -------------------

//
// Get info
//

// 220
// 221
// 222

// 223 - Get sys time info.

// 224 - Get time
#define SCI0_GETTIME 224

// 225 - Get date
#define SCI0_GETDATE 225

// 226 - Get kernel semaphore
#define SCI0_GET_KERNELSEMAPHORE    226

// 227 - close gate
// Entering critical section.
// Close (0).
#define SCI0_CLOSE_KERNELSEMAPHORE  227

// 228 - open gate
// Exiting critical section.
// Open  (1).
#define SCI0_OPEN_KERNELSEMAPHORE   228 


// 229 - Reserved for semaphore stuff

// -------------
// 236 - Get tty id

// -------------

#define SCI0_GETCURSORX  240

#define SCI0_GETCURSORY  241

#define SCI0_242         242
#define SCI0_243         243
#define SCI0_244         244
#define SCI0_245         245
#define SCI0_246  246
#define SCI0_247  247

// 248 - sys_execve()

#define SCI0_249  249

// -------------

// 250 - Get system metrics
#define SCI0_SYS_GET_SYSTEM_METRICS  250

#define SCI0_SHOWDISKINFO      251    // Current disk.            
#define SCI0_SHOWVOLUMEINFO    252    // Current volume.
#define SCI0_MEMORYINFO        253    // Memory info.
#define SCI0_SHOWPCIINFO       254    // PCI info.
#define SCI0_SHOWKERNELINFO    255    // Kernel info.

//
// == Extra ============================================
//

// The handler is the worker __extra_services(), 
// called by sci0().

// 260 - sys_read()
// 261 - sys_write()
// 262 - Console read
// 263 - Console write

// 266 - Process get tty

// 272 - sys_tty_read
// 273 - sys_tty_write

// 277 - Get current virtual console.
// 278 - Set current cirtual console.

// 288 - Returns the current runlevel.

// 289 - Serial debug printk.

// 292 - Get memory size in MB.

// 293 - Get boot info

/*
// 350 - Initialize system component
// Inicializar ou reinicializar componentes do sistema
// depois da inicialização completa do kernel.
// Isso poderá ser chamado pelo init.bin, pelo shell
// ou qualquer outro.
*/

// 377 - sys_uname()
// Get info to fill the utsname structure.
#define SCI0_SYS_UNAME  377

// 390 (bug)

// 391 - Backbuffer draw rectangle
// #bugbug: Is it failing when we try to draw the whole screen?

// 512 - Get display server PID for a given cgroup ring0 pointer.
// Pega o wm de um dado cgroup.
#define  SCI0_GET_DS_PID  512

// 513 - Register the ring3 display server.
// Set ws PID for a given cgroup
// Register a display server.
// gramado_ports[11] = ws_pid
// Called by the window server.
// arg2 = cgroup structure pointer.
// arg3 = The window server PID.
// #todo: 
// We need a helper function for this.
#define SCI0_SET_DS_PID  513

// #deprecated
// 514 - get wm PID for a given cgroup
// IN: cgroup
#define SCI0_GET_WM_PID  514

// #deprecated
// 515 - set wm PID for a given cgroup
// IN: cgroup, pid
#define SCI0_SET_WM_PID  515

// Ingo for ws and wm.
//#define SCI0_SHOW_X_SERVER_INFO  516  // show x server info
//#define SCI0_SHOW_WM_INFO        517  // show wm info

// 518 - Register the ring3 browser.

// 519 - Get the main cgroup rin 0 pointer.
// #bugbug
// This is a ring0 pointer.
// A ring3 process can't handle this thing.
// Get current cgroup


// 521 - Set ns PID for a given cgroup
// Register a network server.
// gramado_ports[11] = ws_pid

// 600 - dup
// 601 - dup2
// 602 - dup3

// 603 - sys_lseek()
// See: kunistd.c
// IN: fd, offset, whence.
#define SCI0_SYS_LSEEK  603

// 640 - Lock the taskswtiching.
// Only the init thread can call this service.

// 641 - Unlock taskswitching.
// Only the init thread can call this service.

// 642 Lock the scheduler.
// Only the init thread can call this service.

// 643 - Unlock scheduler
// Only the init thread can call this service.

// 770 - Show device list.

// 777 - cpu usage for idle thread.

// 801 - get host name
// 802 - set host name
// 803 - Get user name.
// 804 - Set user name.

// 808 - pts name
// #todo
// supporting ptsname libc function
// get_ptsname
// #todo: Change the name to sys_ptsname()
// IN: fd do master, buffer em ring3 para o nome, buflen.

// 809 - pts name
// (#bugbug: The same as above?)
//#todo
//supporting ptsname_r libc function
// #todo: Change the name to sys_ptsname()
//IN: fd do master, buffer e buflen.

// 880 - Get process stats given pid
// 881 - Get thread stats given tid
// 882 - Get process name
// 883 - Get thread name

// 884 - sys_alarm()
// See: sys.c
#define SCI_SYS_ALARM  884

// 891 - Allocate shared ring3 pages.

// 892 - Setup the thread's surface rectangle.
// 893 - Invalidate the thread's surface rectangle.

// 896 - Invalidate the whole screen
// 897 - ?? (create rectangle?)

// (The kernel console).
// 898 - Enter the kernel console. (enable prompt)
// 899 - Exit the kernel console. (disable prompt)

// 900 - copy process
#define SCI0_COPY_PROCESS  900

// 913 - Sleep if socket is empty
// is the socket full?

// 970 - Create request.

// 4444 - Show root files system info.
// Print into the raw kernel console.

// 7000 - sys_socket() 
#define SCI0_SYS_SOCKET  7000

// 7001 - sys_connect()
#define SCI0_SYS_CONNECT  7001

// 7002 - sys_accept()
#define SCI0_SYS_ACCEPT  7002

// 7003 - sys_bind()
#define SCI0_SYS_BIND  7003

// 7004 - sys_listen()
#define SCI0_SYS_LISTEN  7004

// 7006 - Set socket gramado port
// Salvar um pid em uma das portas.
// IN: gramado port, PID

// 7007 - sys_getsockname()

// 7008 - show socket info for a process.
// 7009 - libc: shutdown() IN: fd, how

// 8000 - sys_ioctl()
#define SCI0_SYS_IOCTL  8000

// 8001 - sys_fcntl()
#define SCI0_SYS_FCNTL  8001

// 8002 - Setup stdin pointer

// 9100 - Get system icon

#endif   


