

//#define syscall_vector  0x80


// Números das systemcalls para o Gramado.
// #obs:
// A segunda definição é para ficar igual ao netBSD.

#define	SYSTEMCALL_NULL  0
#define	SYS_syscall SYSTEMCALL_NULL

// Disk
#define	SYSTEMCALL_READ_LBA    1
#define	SYSTEMCALL_WRITE_LBA   2
#define	SYSTEMCALL_READ_FILE   3
#define	SYSTEMCALL_WRITE_FILE  4

// Graphics
#define	SYSTEMCALL_VSYNC                5
#define	SYSTEMCALL_BUFFER_PUTPIXEL      6
#define	SYSTEMCALL_BUFFER_DRAWCHAR      7
#define	SYSTEMCALL_BUFFER_DRAWLINE      8
#define	SYSTEMCALL_BUFFER_DRAWRECT      9
#define	SYSTEMCALL_BUFFER_CREATEWINDOW  10
#define	SYSTEMCALL_REFRESHSCREEN        11

// Network (reserved)
#define	SYSTEMCALL_REDE_RES1  12
#define	SYSTEMCALL_REDE_RES2  13
#define	SYSTEMCALL_REDE_RES3  14
#define	SYSTEMCALL_REDE_RES4  15

// i/o

#define	SYSTEMCALL_IO_RES1  16  //open
#define	SYS_open SYSTEMCALL_IO_RES1
#define	SYSTEMCALL_IO_RES2  17  //close
#define	SYS_close SYSTEMCALL_IO_RES2
#define	SYSTEMCALL_IO_RES3  18
#define	SYSTEMCALL_IO_RES4  19

// Outros

// Buffer
#define	SYSTEMCALL_REFRESH_BUFFER1  20
#define	SYSTEMCALL_REFRESH_BUFFER2  21
#define	SYSTEMCALL_REFRESH_BUFFER3  22

// Screen
#define	SYSTEMCALL_REFRESH_SCREEN2  23

// 24-28 WINDOW SUPPORT	
#define	SYSTEMCALL_SHOWWINDOW   24
#define	SYSTEMCALL_25   25
#define	SYSTEMCALL_26    26
#define	SYSTEMCALL_27    27
#define	SYSTEMCALL_28    28

// Buffer: Print string
#define	SYSTEMCALL_BUFFER_PRINTSTRING  29

// Buffer: Put pixel on buffer
#define	SYSTEMCALL_BUFFER1_PUTPIXEL  30
#define	SYSTEMCALL_BUFFER2_PUTPIXEL  31
#define SYSTEMCALL_BUFFER3_PUTPIXEL  32

// Edit box
#define	SYSTEMCALL_EDITBOX    33

// Cursor
#define	SYSTEMCALL_SETCURSOR  34

// Message and procedures
#define	SYSTEMCALL_SETPROCEDURE          35
#define	SYSTEMCALL_KSENDMESSAGE          36  //O teclado envia essa mensagem para o procedimento ativo.
#define	SYSTEMCALL_CALL_SYSTEMPROCEDURE  37  //chama o procedimento padrao do sistema.

// Read sector
#define	SYSTEMCALL_READSECTOR  38

// Show buffer
#define	SYSTEMCALL_SHOWBUFFER  39

// Shut down computer
#define	SYSTEMCALL_SHUTDOWN  40    //Desliga o computador.
#define	SYS_shutdown SYSTEMCALL_SHUTDOWN

#define	SYSTEMCALL_41        41

// Bitmap img
#define	SYSTEMCALL_LOAD_BITMAP_16x16  42  //Load bitmap image, size=16x16.
#define	SYSTEMCALL_43                 43

// ?? Next app
#define	SYSTEMCALL_G_NEXT_APP         44

// Message box
#define	SYSTEMCALL_MESSAGE_BOX        45

// Client area
#define	SYSTEMCALL_SET_CLIENT_AREA    46

// Create Window support
#define	SYSTEMCALL_CREATEWINDOW0  47 // envia argumentos de controle. 
#define	SYSTEMCALL_CREATEWINDOW1  48 // envia argumentos de posicionamento.
#define	SYSTEMCALL_CREATEWINDOW2  49 // envia argumentos de dimensões.

//
// (50~59) Window suppot, manipulação de janelas.
//

#define	SYSTEMCALL_RESIZEWINDOW      50  // resize 
#define	SYSTEMCALL_REDRAWWINDOW      51  // redraw
#define	SYSTEMCALL_REPLACEWINDOW     52  // replace
#define	SYSTEMCALL_MAXIMIZEWINDOW    53  // MAXIMIZE
#define	SYSTEMCALL_MINIMIZEWINDOW    54  // MINMIZE
#define	SYSTEMCALL_GETFOREGROUNDWINDOW    55   // GET FOREGROUND
#define	SYSTEMCALL_SETFOREGROUNDWINDOW    56   // SET FOREGROUND
// Register window
#define	SYSTEMCALL_REGISTERWINDOW  57  
// Close window
#define	SYSTEMCALL_CLOSEWINDOW     58
// Destroy window. (Destrói a estrutura e a classe).
#define	SYSTEMCALL_DESTROYWINDOW   59

// Active window support
#define	SYSTEMCALL_SETACTIVEWINDOW  60
#define	SYSTEMCALL_GETACTIVEWINDOW  61

// Focus support
#define	SYSTEMCALL_SETFOCUS   62
#define	SYSTEMCALL_GETFOCUS   63
#define	SYSTEMCALL_KILLFOCUS  64

// Outros. 
// #todo: 
// Reservar próximos para seleção de janela. 
#define	SYSTEMCALL_SYS_PUTCHAR  65
#define	SYSTEMCALL_66  66
#define	SYSTEMCALL_67  67

#define	SYSTEMCALL_68  68
#define	SYSTEMCALL_69  69

// Exit process support.
#define	SYSTEMCALL_EXIT             70
#define	SYS_exit SYSTEMCALL_EXIT

// fork support.
#define	SYSTEMCALL_FORK             71
#define	SYS_fork  SYSTEMCALL_FORK

// Create thread
#define	SYSTEMCALL_CREATETHREAD     72

// Create process
#define	SYSTEMCALL_CREATEPROCESS    73

// Outros 
// #todo: 
// Reservar próximos para manipulação de threads e processos. 
#define	SYSTEMCALL_CLOSEALLPROCESS  74
#define	SYSTEMCALL_75               75 //poderia ser close current process.??
#define	SYSTEMCALL_GETNEXTPROCESS   76
#define	SYSTEMCALL_SETNEXTPROCESS   77
#define	SYSTEMCALL_GETNEXTTHREAD    78
#define	SYSTEMCALL_SETNEXTTHREAD    79

// #todo: 
// Resevar próximos para manipulação de processos.
#define	SYSTEMCALL_CURRENTPROCESSINFO  80  //Informações sobre o processo atual.

#define	SYSTEMCALL_GETPPID  81
#define	SYS_getppid SYSTEMCALL_GETPPID

#define	SYSTEMCALL_SETPPID  82
#define	SYSTEMCALL_WAIT4    83 //wait for process termination
#define	SYSTEMCALL_84       84

#define	SYSTEMCALL_GETPID   85 
#define	SYS_getpid  SYSTEMCALL_GETPID

#define	SYSTEMCALL_SETPID   86     //#bugbug: não sei se isso existe.
#define	SYSTEMCALL_SEMAPHORE_DOWN  87
#define	SYSTEMCALL_88              88  //Em uso. não mudar.
#define	SYSTEMCALL_SEMAPHORE_UP    89

// Reserva próximos para manipulação de threads.
// Dead thread collector.
#define	SYSTEMCALL_DEAD_THREAD_COLLECTOR  90
#define	SYSTEMCALL_ALERTTHREAD  91
#define	SYSTEMCALL_92  92
#define	SYSTEMCALL_93  93
#define	SYSTEMCALL_STARTTHREAD  94 //REAL (coloca a thread em standby para executar pela primeira vez.)
#define	SYSTEMCALL_95  95
#define	SYSTEMCALL_96  96
#define	SYSTEMCALL_97  97
#define	SYSTEMCALL_RESUMETHREAD  98

// Procedure arguments. 
// (window,msg,long1,long2)
#define	SYSTEMCALL_GET_HWINDOW           99
#define	SYSTEMCALL_GET_KEYBOARD_MESSAGE  100
#define	SYSTEMCALL_GET_LONG1             101 
#define	SYSTEMCALL_GET_LONG2             102

// (103~109) Rotinas de mensagens. Channels, Sockets.
#define	SYSTEMCALL_RECEIVEMESSAGE     103  //Pega uma mensagem no PCB de um processo.
#define	SYSTEMCALL_SENDMESSAGE        104  //Envia uma mensagem para o PCB de um processo.
#define	SYSTEMCALL_REGISTERPROCEDURE  105
#define	SYSTEMCALL_CREATECHANNEL      106
#define	SYSTEMCALL_DESTROYCHANNEL     107
#define	SYSTEMCALL_OPENCHANNEL        108
#define	SYSTEMCALL_CLOSECHANNEL       109

// Reboot
#define	SYSTEMCALL_REBOOT  110
#define	SYS_reboot SYSTEMCALL_REBOOT

// Outros 
// #todo: 
// Reservar para gerenciamento de energia. 111- 119
#define	SYSTEMCALL_111  111
#define	SYSTEMCALL_112  112
#define	SYSTEMCALL_113  113 //Envia uma mensagem PAINT para o aplicativo atualizar a área de trabalho.
#define	SYSTEMCALL_114  114
#define	SYSTEMCALL_115  115
#define	SYSTEMCALL_116  116
#define	SYSTEMCALL_117  117
#define	SYSTEMCALL_118  118
#define	SYSTEMCALL_119  119

// ??
// 120-125 gerenciamento de barras
#define	SYSTEMCALL_CREATEMENUBAR    120
#define	SYSTEMCALL_CREATETASKBAR    121
#define	SYSTEMCALL_CREATESTATUSBAR  122
#define	SYSTEMCALL_123              123
#define	SYSTEMCALL_124              124
#define	SYSTEMCALL_125              125

// 126~129 (RESERVADO PARA COMUNICAÇÃO COM DRIVER.)
#define	SYSTEMCALL_126 126
#define	SYSTEMCALL_127 127
#define	SYSTEMCALL_128 128
// 129: Inicializando um driver. Um driver enviou uma systemcall 
// confirmando a inicialização de um driver.
#define	SYSTEMCALL_DRIVERINITIALIZED 129

// 130-139 Manipulação de texto.
#define	SYSTEMCALL_DRAWTEXT  130
#define	SYSTEMCALL_131       131
#define	SYSTEMCALL_132       132
#define	SYSTEMCALL_SET_WINDOW_WITH_TEXT_INPUT 133
#define	SYSTEMCALL_GET_WINDOW_WITH_TEXT_INPUT 134
#define	SYSTEMCALL_135       135
#define	SYSTEMCALL_136       136
#define	SYSTEMCALL_137       137
#define	SYSTEMCALL_138       138    //api_receive_message
#define	SYSTEMCALL_139       139   //get scancode.@todo

// keyboard responder
#define	SYSTEMCALL_SET_CURRENT_KEYBOARD_RESPONDER 140
#define	SYSTEMCALL_GET_CURRENT_KEYBOARD_RESPONDER 141

// mouse responder
#define	SYSTEMCALL_SET_CURRENT_MOUSE_RESPONDER 142
#define	SYSTEMCALL_GET_CURRENT_MOUSE_RESPONDER 143

// 144-149 Recursos da area do Cliente.
#define	SYSTEMCALL_GETCLIENTAREARECT 144    // Get client Area rect.
#define	SYSTEMCALL_SETCLIENTAREARECT 145    // Set Client Area rect.
#define	SYSTEMCALL_146 146
#define	SYSTEMCALL_147 147
#define	SYSTEMCALL_148 148
#define	SYSTEMCALL_149 149

// 150-159 user and group support.
#define	SYSTEMCALL_CREATEUSER         150
#define	SYSTEMCALL_SETCURRENTUSERID   151
#define	SYSTEMCALL_GETCURRENTUSERID   152
#define	SYSTEMCALL_SETCURRENTGROUPID  153
#define	SYSTEMCALL_GETCURRENTGROUPID  154
#define	SYSTEMCALL_UPDATEUSERINFO     155
#define	SYSTEMCALL_SHOWUSERINFO       156
#define	SYSTEMCALL_GETCURRENTUSERSESSION   157
#define	SYSTEMCALL_GETCURRENTWINDOWSTATION 158
#define	SYSTEMCALL_GETCURRENTDESKTOP       159

// 160-169 network support
#define	SYSTEMCALL_160 160   //create socket (retorna o ponteiro para a estrutura)
#define	SYSTEMCALL_161 161   //get socket IP	
#define	SYSTEMCALL_162 162   //get socket port
#define	SYSTEMCALL_163 163   //update socket  
#define	SYSTEMCALL_164 164   //netStream
#define	SYSTEMCALL_165 165   //netSocket
#define	SYSTEMCALL_166 166   //netBuffer

#define	SYSTEMCALL_167 167    //GRAMADOCORE_INIT_EXECVE_BIN
#define	SYSTEMCALL_168 168    //GRAMADOCORE_INIT_EXECVE_EXE
#define	SYSTEMCALL_169 169    //reservado par outro formato

// 170-179 network support
#define	SYSTEMCALL_170 170  // Print Working Directory. (pwd)
#define	SYSTEMCALL_171 171  // Get current volume id.
#define	SYSTEMCALL_172 172  // Listar os arquivos do diretório atual.
#define	SYSTEMCALL_173 173  // Procurar arquivo. 
#define	SYSTEMCALL_174 174
#define	SYSTEMCALL_175 175
#define	SYSTEMCALL_176 176
#define	SYSTEMCALL_177 177
#define	SYSTEMCALL_178 178
#define	SYSTEMCALL_179 179

// 180-189 Memory support
#define	SYSTEMCALL_CREATEPAGEDIRECTORY 180
#define	SYSTEMCALL_CREATEPAGETABLE 181
#define	SYSTEMCALL_SHOWMEMORYSTRUCTS 182
#define	SYSTEMCALL_SETCR3 183
#define	SYSTEMCALL_GETPROCESSHEAPPOINTER 184
#define	SYSTEMCALL_SETKERNELHEAP 185
#define	SYSTEMCALL_ALLOCATEKERNELHEAP 186
#define	SYSTEMCALL_FREEKERNELHEAP 187
#define	SYSTEMCALL_GETPROCESSDIRECTORY 188
#define	SYSTEMCALL_SETPROCESSDIRECTORY 189

// 190-199 memory support
#define	SYSTEMCALL_SYS_GETPAGEDIRVALUE 190
#define	SYSTEMCALL_ALLOCATEVIRTUALMEMORY 191
#define	SYSTEMCALL_192 192
#define	SYSTEMCALL_193 193
#define	SYSTEMCALL_194 194
#define	SYSTEMCALL_195 195
#define	SYSTEMCALL_196 196
#define	SYSTEMCALL_197 197
#define	SYSTEMCALL_198 198
#define	SYSTEMCALL_199 199   //@todo: GC

// Outros. (sinais, alertas, mesagens)
#define	SYSTEMCALL_SENDSIGNAL         200  //??reservar.
#define	SYSTEMCALL_WAIT4OBJECT        201
#define	SYSTEMCALL_WAIT4EVENT         202
#define	SYSTEMCALL_WAIT4DEVICE        203
#define	SYSTEMCALL_WAIT4SIGNAL        204
#define	SYSTEMCALL_GENERICWAIT        205
#define	SYSTEMCALL_TIMERGETTICKCOUNT  206
#define	SYSTEMCALL_TIMERGETTIMEOUT    207
#define	SYSTEMCALL_TIMERSETTIMEOUT    208
#define	SYSTEMCALL_CALLIOSERVICES     209

// tty;
// console;
// terminal - terminal emulator support.
#define	SYSTEMCALL_CREATETERMINAL          210
#define	SYSTEMCALL_GETCURRENTTERMINAL      211
#define	SYSTEMCALL_SETCURRENTTERMINAL      212
#define	SYSTEMCALL_GETTERMINALINPUTBUFFER  213
#define	SYSTEMCALL_SETTERMINALINPUTBUFFER  214
#define	SYSTEMCALL_GETTERMINALWINDOW       215
#define	SYSTEMCALL_SETTERMINALWINDOW       216
#define	SYSTEMCALL_GETTERMINALRECT         217
#define	SYSTEMCALL_SETTERMINALRECT         218
#define	SYSTEMCALL_DESTROYTERMINAL         219

#define	SYSTEMCALL_REBOOT2  220

// program support
#define	SYSTEMCALL_EXECUTEPROGRAM  221  //execute program.

// Create timer
#define	SYSTEMCALL_222             222  
#define	SYS_timer_create SYSTEMCALL_222

#define	SYSTEMCALL_223  223
#define	SYSTEMCALL_224  224
#define	SYSTEMCALL_225  225

// Critical section. (kernel semaphore)
// Um semáforo do kernel para uso geral.
#define	SYSTEMCALL_GET_KERNELSEMAPHORE    226
#define	SYSTEMCALL_CLOSE_KERNELSEMAPHORE  227
#define	SYSTEMCALL_OPEN_KERNELSEMAPHORE   228

// debug stuff
#define	SYSTEMCALL_KERNELDEBUG  229

// 230-239 (Boot support)
// logon logoff support.
#define	SYSTEMCALL_STARTLOGON     230
#define	SYSTEMCALL_STARTLOGOFF    231
#define	SYSTEMCALL_232            232
#define	SYSTEMCALL_233            233
#define	SYSTEMCALL_234            234
#define	SYSTEMCALL_235            235
#define	SYSTEMCALL_236            236
#define	SYSTEMCALL_237            237
#define	SYSTEMCALL_238            238
#define	SYSTEMCALL_SETBOOTOPTIONS 239

// 240-249 Reservar próximos para gerenciamento de cursor e ponteiros.
#define	SYSTEMCALL_GETCURSORX  240
#define	SYSTEMCALL_GETCURSORY  241
#define	SYSTEMCALL_242         242
#define	SYSTEMCALL_243         243
#define	SYSTEMCALL_244         244
#define	SYSTEMCALL_245         245

#define	SYSTEMCALL_246         246

#define	SYSTEMCALL_247         247
#define	SYS_pipe  SYSTEMCALL_247

#define	SYSTEMCALL_248         248
#define	SYSTEMCALL_249         249

// (250 ~ 255) Info support, get info.
// Ou últimos oferecem informações sobre o sistema.
#define	SYSTEMCALL_GETSYSTEMMETRICS  250 
#define	SYSTEMCALL_251               251
#define	SYSTEMCALL_252               252
#define	SYSTEMCALL_MEMORYINFO        253
#define	SYSTEMCALL_SHOWPCIINFO       254
#define	SYSTEMCALL_SHOWKERNELINFO    255

//#define	SYS_MAXSYSCALL	256

