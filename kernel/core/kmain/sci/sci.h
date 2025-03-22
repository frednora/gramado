
// sci.h
// Here is where the numbers are defines.
// The other files contain just aliases.
// Created by Fred Nora.

// Maybe the SCI_ prefix can be followed by 
// the symbol of the function called by the syscall number.
// ex: SCI_SYS_REBOOT for sys_reboot.

#ifndef __SYSCALLS_SCI_H
#define __SYSCALLS_SCI_H    1

// The handler is sci0().

#define SCI_NULL  0

// Disk
#define SCI_READ_LBA    1
#define SCI_WRITE_LBA   2
#define SCI_READ_FILE   3
#define SCI_WRITE_FILE  4

// kgws - Graphics
#define SCI_SYS_VSYNC  5
#define SCI_BUFFER_PUTPIXEL      6
#define SCI_BUFFER_DRAWCHAR      7
#define SCI_BUFFER_DRAWLINE      8
#define SCI_BUFFER_DRAWRECT      9
// 10 =  Refresh rectangle.
//#define SCI_BUFFER_CREATEWINDOW  10 // (#deprecated)
#define SCI_REFRESHSCREEN        11

// Network
#define SCI_REDE_R1    12
#define SCI_REDE_R2    13
#define SCI_REDE_R3    14
#define SCI_REDE_R4    15

// Posix
#define SCI_SYS_OPEN   16  // open()
#define SCI_SYS_CLOSE  17  // close()
#define SCI_SYS_READ   18  // read()
#define SCI_SYS_WRITE  19  // write()

// Buffers support: 20~23
#define SCI_REFRESH_BUFFER1  20
#define SCI_REFRESH_BUFFER2  21
#define SCI_REFRESH_BUFFER3  22
#define SCI_REFRESHSCREEN2   23

// Surface support: 24~28
#define SCI_24  24
#define SCI_25  25
#define SCI_26  26
#define SCI_27  27
#define SCI_28  28

// kgws - Print string on backbuffer.
#define SCI_MY_BUFFER_PRINTSTRING  29

// kgws - Print pixel on backbuffer.
#define SCI_MY_BUFFER_PUTPIXEL   30
#define SCI_MY_BUFFER2_PUTPIXEL  31
#define SCI_MY_BUFFER3_PUTPIXEL  32

// Not in use.
#define SCI_33  33

// Cursor
// Setup cursor position for the current virtual console.
#define SCI_CALI_SET_CURSOR  34

// Not in use.
#define SCI_35  35

// #deprecated
// O teclado envia essa mensagem para o procedimento ativo.
// #define SCI_KSENDMESSAGE  36  

// #deprecated
// Chama o procedimento padrao do sistema.
// #define SCI_CALLSYSTEMPROCEDURE  37  

// Hostname
#define SCI_GETHOSTNAME  38
#define SCI_SETHOSTNAME  39
// Username
#define SCI_GETUSERNAME  40
#define SCI_SETUSERNAME  41

// #deprecated
// Load bitmap image, size=16x16.
// #define SCI_LOADBMP16X16  42

// Create an empty file
// see: sci0() in sci.c
#define SCI_43  43
// 44 - Create an empty directory.
#define SCI_44  44

// 45 - livre
// #todo: Usar para manipulação de arquivo ou diretório.
#define SCI_45  45

// 46 - livre
// 47 - Show cpu info.
// 48 - livre
// 49 - Show system info.

// ...

// 65 - Put a char in the current virtual console.
#define SCI_KGWS_PUTCHAR  65

//#define SCI_66  66
//#define SCI_67  67
//#define SCI_68  68
//#define SCI_69  69

//
//    Process and Thread 
//


// Process and thread support.
#define  SCI_EXIT             70    // Um thread saiu.
#define  SCI_FORK             71    // fork.
#define  SCI_SYS_CREATE_THREAD   72  // Create thread and execute it.
#define  SCI_SYS_CREATE_PROCESS  73  // Create process and execute it. 
#define  SCI_CLOSEALLPROCESS  74    // FECHA TODOS PROCESSO.
#define  SCI_EXECVE           75    // execve 
#define  SCI_GETNEXTPROCESS   76    // get next process
#define  SCI_SETNEXTPROCESS   77
#define  SCI_GETNEXTTHREAD    78    // get next thread.
#define  SCI_SETNEXTTHREAD    79

// Process support.
#define SCI_CURRENTPROCESSINFO  80  // Informações sobre o processo atual.
#define SCI_GETPPID             81  // get parent process id.

// 82 - Show process information.

// Wait 4 PID.
// Wait for process termination.
// A thread atual vai esperar até que um processo termine.
#define SCI_WAIT4PID  83
#define SCI_WAIT4     83

// Not in use.
#define SCI_84  84

// GET CURRENT PROCESS ID.
#define SCI_GETPID  85

// Not in use.
#define SCI_86  86
#define SCI_87  87

// Test process validation.
#define SCI_88  88

// Not in use.
#define SCI_89  89

// ------------------
// 90~99 Reservado para thread support
#define SCI_DEADTHREADCOLLECTOR  90
#define SCI_ALERTTHREAD  91
#define SCI_92  92
#define SCI_93  93
#define SCI_STARTTHREAD  94 //REAL (coloca a thread em standby para executar pela primeira vez.)
#define SCI_95  95
#define SCI_96  96
#define SCI_97  97
#define SCI_RESUMETHREAD  98  //retomar thread.
#define SCI_99  99
// ------------------

// 100~109: free

//---------------
// Reboot
#define SCI_SYS_REBOOT  110

// #important
// Get system message.
#define SCI_SYS_GET_MESSAGE  111

// Post message to tid.
// Asynchronous.
#define SCI_112  112

// 113~119: free

//------------

// 120
// Get a message given the index.
// With restart support.
#define SCI_SYS_GET_MESSAGE2  120

//#define SCI_121  121
//#define SCI_122  122
//#define SCI_123  123

// #test: a kernel request.
#define SCI_124  124

#define SCI_125  125

// i/o port support
#define SCI_PORTSX86_IN  126
// i/o port support
#define SCI_PORTSX86_OUT  127

// 128~129: free

// --------------------

// 130,131

// 132 - d_draw_char
// Desenha um caractere e pinta o pano de fundo.
#define SCI_132  132
// 133 - d_drawchar_transparent
// Desenha um caractere sem alterar o pano de fundo.
#define SCI_133  133

// 134~136: free

// 137 - #deprecated.
// SUPORTE a getch()
#define SCI_GETCH  137    

// 138 - Get key state.
#define SCI_138           138

#define SCI_GETSCANCODE   139  

// -------------------

//keyboard responder
#define SCI_SET_CURRENT_KEYBOARD_RESPONDER 140
#define SCI_GET_CURRENT_KEYBOARD_RESPONDER 141
//mouse responder
#define SCI_SET_CURRENT_MOUSE_RESPONDER 142
#define SCI_GET_CURRENT_MOUSE_RESPONDER 143
//144-149 Recursos da area do Cliente.
#define SCI_GETCLIENTAREARECT 144
#define SCI_SETCLIENTAREARECT 145
// Janelas principais usadas pelo kgws.
#define SCI_146  146    // retorna o ponteiro para gui->screen 
#define SCI_147  147    // retorna o ponteiro para gui->main
//grid
#define SCI_148 148 //create grid
#define SCI_149 149 //initialize grid.

// -----------------

// 150~156 User and group support.
#define SCI_CREATEUSER         150
#define SCI_SETCURRENTUSERID   151
#define SCI_GETCURRENTUSERID   152
#define SCI_SETCURRENTGROUPID  153
#define SCI_GETCURRENTGROUPID  154
#define SCI_UPDATEUSERINFO     155
#define SCI_SHOWUSERINFO       156

// Security ?
#define SCI_GETCURRENTUSERSESSION    157 // user session
// 158 - free
#define SCI_GETCURRENTDESKTOP        159 // cgroup ?

// ------------

// 160~169: Reserved to network support.
#define SCI_160  160    //create socket (retorna o ponteiro para a estrutura)
#define SCI_161  161    //get socket IP
#define SCI_162  162    //get socket port
#define SCI_163  163    //update socket  
#define SCI_164  164    //netStream
#define SCI_165  165    //netSocket
#define SCI_166  166    //netBuffer
#define SCI_167  167
#define SCI_168  168
#define SCI_169  169 

// -----------------

// Print Working Directory.
// Comand 'pwd'.
#define SCI_PWD  170
// Get current volume id.
#define SCI_GETCURRENTVOLUMEID  171
// Configura o id do volume atual.
#define SCI_SETCURRENTVOLUMEID  172
// Listar os arquivos do diretório. 
// Dados ids de disco, volume e diretório.
#define SCI_LISTFILES  173
// Procurar arquivo. 
#define SCI_SEARCHFILE  174
// 175 - 'cd' command support.
#define SCI_175  175
// 176 - pathname backup string.
#define SCI_176  176
// 'dir' command.
// Listando arquivos em um diretório dado o nome.
#define SCI_177  177
// Get file size.
// Pegando o tamanho de um arquivo.
// bufbug: isso está gastando memória carregando o diretório raiz.
#define SCI_178  178
// Not in use.
#define SCI_179   179

// -----------------

// 180~189: memory support.
// Memory support.
// Precisa de privilágios.
#define SCI_CREATEPAGEDIRECTORY    180  // cria um pagedir.
#define SCI_CREATEPAGETABLE        181  // cria uma pagetable.
#define SCI_SHOWMEMORYSTRUCTS      182  // mostra estruturas de gerencia de memória.
#define SCI_SETCR3                 183  // configura o cr3 do processo atual.
#define SCI_GETPROCESSHEAPPOINTER  184  // pega o heap pointer de um processo.
#define SCI_SETKERNELHEAP          185  // configura o heap do kernel.
#define SCI_ALLOCATEKERNELHEAP     186  // aloca heap do kernel.
#define SCI_FREEKERNELHEAP         187  // Libera heap do kernel.
#define SCI_GETPROCESSDIRECTORY    188  // get process directory.
#define SCI_SETPROCESSDIRECTORY    189  // set process directory.

// 190~199: free

// 200~209: free

// -------------------
// 210~219: terminal/virtual console support.
// #bugbug: 
// Isso provavelmente são rotinas de console virtual
// e não de pseudo terminal
// Terminal emulator support.
#define SCI_CREATETERMINAL          210
#define SCI_GETCURRENTTERMINAL      211
#define SCI_SETCURRENTTERMINAL      212
#define SCI_GETTERMINALINPUTBUFFER  213
#define SCI_SETTERMINALINPUTBUFFER  214
#define SCI_GETTERMINALWINDOW       215
#define SCI_SETTERMINALWINDOW       216
#define SCI_GETTERMINALRECT         217
#define SCI_SETTERMINALRECT         218
#define SCI_DESTROYTERMINAL         219

// -------------------

//
// Get info
//

#define SCI_220  220
#define SCI_222  222
// 223 - Get sys time info.
#define SCI_223  223
//time date
#define SCI_GETTIME 224
#define SCI_GETDATE 225

// Semaphore.
#define SCI_GET_KERNELSEMAPHORE    226  // Get.  
#define SCI_CLOSE_KERNELSEMAPHORE  227  // Close (0).
#define SCI_OPEN_KERNELSEMAPHORE   228  // Open  (1).
#define SCI_229  229  // #todo: use it for semaphore support.

// -------------
// 230~239: Reserved for tty support.

// -------------
// 240~249: Reserved for text editing support.

#define SCI_GETCURSORX  240
#define SCI_GETCURSORY  241
#define SCI_242         242
#define SCI_243         243
#define SCI_244         244
#define SCI_245         245
#define SCI_246  246
#define SCI_247  247
#define SCI_248  248
#define SCI_249  249

// -------------
// (250 ~ 255) - Info support.

// System info support.

#define SCI_SYS_GET_SYSTEM_METRICS  250    // System metrics. 

#define SCI_SHOWDISKINFO      251    // Current disk.            
#define SCI_SHOWVOLUMEINFO    252    // Current volume.
#define SCI_MEMORYINFO        253    // Memory info.
#define SCI_SHOWPCIINFO       254    // PCI info.
#define SCI_SHOWKERNELINFO    255    // Kernel info.

//
// == Extra ============================================
//

// The handler is the worker __extra_services(), 
// called by sci0().

// 260 read.
// 261 write.
// 262 console read
// 263 console write
// 266 process_get_tty
// 272 tty read
// 273 tty write
// 277 console_get_current_virtual_console
// 278 console_set_current_virtual_console
// 288 Returns the current runlevel.
// 289 Serial debug print string.
// 292 newos_get_memory_size_mb
// 293 info_get_boot_info

/*
 350 sys_initialize_component:
// Inicializar ou reinicializar componentes do sistema
// depois da inicialização completa do kernel.
// Isso poderá ser chamado pelo init.bin, pelo shell
// ou qualquer outro.
*/

// 377 - uname command.
// Get info to fill the utsname structure.
#define SCI_SYS_UNAME  377

// 391 - backbuffer_draw_rectangle
// #bugbug
// Falha se tentamos pintar a tela toda.


// ws and wm.

// 512 - SCI_GET_WS_PID
// 512 - Get ws PID for a given cgroup.
// Pega o wm de um dado cgroup.
#define  SCI_GET_WS_PID  512

// 513 - SCI_SET_WS_PID
// 513
// Register the ring3 display server.
// Set ws PID for a given cgroup
// Register a display server.
// gramado_ports[11] = ws_pid
// Called by the window server.
// arg2 = cgroup structure pointer.
// arg3 = The window server PID.
// #todo: 
// We need a helper function for this.
#define SCI_SET_WS_PID  513

// 514 - SCI_GET_WM_PID
// 514 - get wm PID for a given cgroup
// IN: cgroup
#define SCI_GET_WM_PID  514

// 515 - SCI_SET_WM_PID
// 515 - set wm PID for a given cgroup
// Register a ring3 wm.
// IN: cgroup, pid
#define SCI_SET_WM_PID  515

// Ingo for ws and wm.
#define SCI_SHOW_X_SERVER_INFO  516  // show x server info
#define SCI_SHOW_WM_INFO        517  // show wm info

// 519 - CurrentCG
// #bugbug
// This is a ring0 pointer.
// A ring3 process can't handle this thing.
// Get current cgroup

// 521
// network server
// 521 - set ns PID for a given cgroup
// Register a network server.
// gramado_ports[11] = ws_pid

// 600 - dup
// 601 - dup2
// 602 - dup3

// 603 - lseek support.
// See: kunistd.c
// IN: fd, offset, whence.
#define SCI_SYS_LSEEK  603

// 640
// Lock the taskswtiching.
// Only the init thread can call this service.

// 641
// Unlock taskswitching.
// Only the init thread can call this service.

// 642
// Lock the scheduler.
// Only the init thread can call this service.

// 643
// Unlock scheduler.
// Only the init thread can call this service.

// 770 - Show device list.

// 777 - cpu usage for idle thread.

// 801 - get host name
// 802 - set host name
// 803 - Get user name.
// 804 - Set user name.


// 808 - __ptsname
// #todo
// supporting ptsname libc function
// get_ptsname
// #todo: Change the name to sys_ptsname()
// IN: fd do master, buffer em ring3 para o nome, buflen.

// (#bugbug: The same as above?)
// 809 - __ptsname
//#todo
//supporting ptsname_r libc function
// #todo: Change the name to sys_ptsname()
//IN: fd do master, buffer e buflen.

// 880 - Get process stats given pid
// 881 - Get thread stats given tid
// 882 - Get process name
// 883 - Get thread name

// 884 - alarm()
// See: sys.c
#define SCI_SYS_ALARM  884

// 891 - Allocate shared ring3 pages.

// 892 - Setup the thread's surface rectangle.
// 893 - Invalidate the thread's surface rectangle.

// 896 - Invalidate the whole screen
// 897 - Create a rectangle.

// (The kernel console).
// 898 - Enable prompt
// 899 - Desable prompt


// 900 - copy process
#define SCI_COPY_PROCESS  900

// 913 - sys_sleep_if_socket_is_empty
// is the socket full?

// 4444
// Show root files system info.
// Print into the raw kernel console.

// 7000 ~ 7020 for network sockets

// 7000 - socket() 
#define SCI_SYS_SOCKET  7000
// 7001 - connect()
#define SCI_SYS_CONNECT  7001
// 7002 - accept()
#define SCI_SYS_ACCEPT  7002
// 7003 - bind()
#define SCI_SYS_BIND  7003
// 7004 - listen()
#define SCI_SYS_LISTEN  7004

// 7006 - socket_set_gramado_port
// Salvar um pid em uma das portas.
// IN: gramado port, PID

// 7007 - sys_getsockname()

// 7008 - show socket info for a process.
// 7009 - libc: shutdown() IN: fd, how

// 8000 - ioctl() implementation.
#define SCI_SYS_IOCTL  8000
// 8001 - fcntl()
#define SCI_SYS_FCNTL  8001

// 8002 - sys_setup_stdin
// ?? #bugbug
// Setup stdin pointer
// See: kstdio
// IN: fd

// 9100 - ui_get_system_icon
// Pegando o endereço de um buffer de icone.
// queremos saber se ele eh compartilhado.
// shared_buffer_terminal_icon
// #bugbug: Static size for the icons. Static buffer size.
// See: wm.c

#endif   








