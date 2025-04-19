// globals.h
// Created by Fred Nora.

#ifndef __GLOBALS
#define __GLOBALS   1

//
// pool
//

// Tipo array de strings
typedef char ** poll;
typedef char ** POLL;

// Array de unsigned longs contendo ponteiros 
// para pools de strings;
typedef unsigned long  pool_list_t;
typedef unsigned long  POOLLIST;
 
/* Values that can be returned by execute_command (). */
#define EXECUTION_FAILURE  1
#define EXECUTION_SUCCESS  0

/* The list of characters that are quoted in double-quotes with a
   backslash.  Other characters following a backslash cause nothing
   special to happen. */
#define slashify_in_quotes  "\\`\"$"

//
// # env. support # 
//

/*POSIX*/

//colunas e linhas
#define ENV_COLUMNS  "COLUMNS"
#define ENV_LINES  "LINES"
#define ENV_LOGNAME  "LOGNAME"
#define ENV_PATH  "PATH"
#define ENV_SHELL  "SHELL"
//the local time zone
#define ENV_TZ  "TZ"

/*OUTROS*/
//home directory of the present user.
#define ENV_HOME  "HOME"
//login name
#define ENV_USER  "USER"
//the language she would like to see messages
#define ENV_LANG  "LANG"
//the default printer to be used
#define ENV_PRINTER  "PRINTER"
#define ENV_LPDEST  "LPDEST"
//#define FILENAME_LENGTH (8+3) 

//
// Strings
//

#define SHELL_PATHNAME_SEPARATOR  "/"
#define SHELL_PATHNAME_TERMINATOR  "\0"
#define SHELL_STRING_TERMINATOR  "\0" 

// #todo: 
// a versão do sistema operacional 
// deve-se obter através de uma chamada ao sistema. 
#define OS_VERSION     "0.1"  
#define SHELL_VERSION  "0.1"

//prompt
#define SHELL_PROMPT   "$"
#define SHELL_PROMPT2  "#"
#define SHELL_PROMPT3  "%"
#define SHELL_PROMPT4  ">"
//... 

//
// Volume support
//

//Volume list
#define SHELL_ROOT_STRING        "root:"
#define SHELL_VOLUMELIST_STRING  "root:"

//volumes
#define SHELL_UNKNOWNVOLUME_STRING  "unknown-volume"
#define SHELL_VOLUME0_STRING  "volume0"   //vfs
#define SHELL_VOLUME1_STRING  "volume1"   //boot volume
#define SHELL_VOLUME2_STRING  "volume2"   //system volume.
//...

//
// pwd support
//

#define SHELL_UNKNOWNWORKINGDIRECTORY_ID (-1)
#define SHELL_VFSWORKINGDIRECTORY_ID     0
#define SHELL_BOOTWORKINGDIRECTORY_ID    1
#define SHELL_SYSTEMWORKINGDIRECTORY_ID  2

#define SHELL_VFSWORKINGDIRECTORY_STRING     "volume1"
#define SHELL_BOOTWORKINGDIRECTORY_STRING    "volume1"
#define SHELL_SYSTEMWORKINGDIRECTORY_STRING  "volume2"
#define SHELL_UNKNOWNWORKINGDIRECTORY_STRING "unknown-directory"

#define WORKINGDIRECTORY_STRING_MAX  32

//...

#define SHELL_STREAM_STATUS_INPUT   1 //input
#define SHELL_STREAM_STATUS_OUTPUT  2 //output
#define SHELL_STREAM_STATUS_ERROR   3 //error

/*
 * File: globals.h
 * Global variables.
 */

//"global command" stuff 
#define glob_percent_sign  '%' 
#define glob_underscore  '_'
#define glob_square_brackets_left  '['
#define glob_square_brackets_right  ']'
#define glob_caret  '^'
#define glob_asterisk  '*'
#define glob_number_sign  '#'
#define glob_question_mark  '?'
//... 
 
#define SHELL_TERMINAL_COLOR  0
#define SHELL_TERMINAL_COLOR2  0x303030
//...

// #todo:
// devemos criar uma rotina que inicialisa esses buffers.

//
// ## pathname suppport ##
//

#define PATH_SEPARATOR  '/'
#define PATH_TERMINATOR  '\0'

#define PATHNAME_LENGHT (64)
char pathname_buffer[PATHNAME_LENGHT];
int pathname_lenght;  //tamanho do atual pathname.
int pathname_initilized;

//
// ## filename suppport ##
//

#define FILENAME_LENGHT (8+3)
char filename_buffer[FILENAME_LENGHT];
int filename_lenght; //tamanho do nome de arquivo atual.
int filename_initilized;

//...

int EOF_Reached;

char *current_volume_string;
int current_volume_id;

//...

int g_current_disk_id;
int g_current_volume_id;
int g_current_workingdirectory_id;  //??

// a string do diretório de trabalho.
char current_workingdiretory_string[WORKINGDIRECTORY_STRING_MAX];
int pwd_initialized;

/*
    Fluxo padrão: definido em <stdio.h>
    FILE *stdin;
    FILE *stdout; 
    FILE *stderr;
*/

//
// font support.
//

//FILE *font_file;

// Diretórios para o shell comparar 
// os comandos com os nomes dos arquivos lá. 
struct _iobuf  *pwd;
struct _iobuf  *root;
//...

//usado para salvar um comando atual  
//char tmp_command[32];      
 
//
// Banners
//
 
// help 
static const char help_banner[] =
    "\n Commands: \n \n"
    " ?         - This help\n"
    " cd        - Change current directory\n"
    " cls       - Clear screen\n"
    " copy      - Copy file or directory\n"
    " date      - Date\n"
    " del       - Delete file or directory\n"
    " dir       - List\n"
    " echo      - This ---\n"
    " exit      - Exit shell\n"
    " help      - This help\n"
    " mov       - Move file or diretory\n"
    " new       - New file or directory\n"
    " pwd       - Print working directory\n"
    " reboot    - Reboot system\n"
    " rename    - Rename file or directory\n"
    " shutdown  - Shutdown your computer locally or remotely\n"
    " time      - Time\n"
    " version   - Shell version\n"
    "\n";

// experience banner
static const char experience_banner[] =
    "\n Try these commands: \n \n"
    " ?            - This help\n"
    " cls          - Clear screen\n"
    " dir          - List\n"
    " exit         - Exit shell\n"
    " gramcode     - Text editor\n"
    " help         - This help\n"
    " help --tests - Extra commands\n"
    " jackpot      - Game\n"
    " kernel-info  - kernel Info\n"
    " mm-info      - Memory Info\n"
    " disk-info    - Disk Info\n"
    " volume-info  - Volume Info\n"
    " pwd          - Print working directory\n"
    " reboot       - Reboot system\n"
    "\n";

// tests banner 
// mostra os comandos para testes do sistema 
// e aplicativos em deseenvolvimento.
static const char tests_banner[] =
    "\n Try these commands: \n \n"
    " about       - Testing message\n"
    " bmp         - bmp <filename>\n"
    " cd          - cd <dirname>\n"
    " color       - color --humility | color --pride\n"
    " disk-info   - Disk Info\n"
    " exit        - Exit shell\n"
    " fork        - fork\n"
    " heap        - User mode Heap test\n"
    " help        - This help\n"
    " kernel-info - kernel Info\n"
    " mbr         - MBR\n"
    " pci-info    - PCI Info\n"
    " socket-test - Socket test\n"    
    " system-info - System info\n"
    " timer-test  - Timer test\n"
    " ~>          - getpid getppid getuid getgid\n"
    " ~>          - get-usersession get-room get-desktop\n"
    " ~>          - dialog-box edit-box message-box\n"
    " ~>          - mm-info mm-kernelheap mm-processheap\n"
    " ~>          - test-taskman-server\n"
    " ~>          - t1 t2 t3 ... t6 t17\n"
    "\n";

// tree // cancelada.
static const char tree_banner[] = "tree command\n";
//... 

// usage
static const char usage[] =
    "Usage: shell [arguments...]\n"
    "       shell -help             Display this help message and exit.\n"
    "       shell -version          Output version information and exit.\n";
    //...

//...

//Lista de palavras reservadas.
//São as mesmas da gramado language, pois 
//a gramado language é uma linguagem de script.
//*.gra  usar esse >>> (*.gl1)
static char *RESERVED[] = {
	"start",   //início do script
	"finish",  //fim do script
	"super",   //tipo genérico.
	"rules",   //fecha declaração (;)
	"condition",   //
	"open",        //( 
	"close",       //)
	"run",         //{
	"stop",        //} 
	"guarantee",   //while
	"mix",         //done: ?? 
	"here",        //system() command 
	"there",       // fecha (;)
};


static unsigned char SPECIAL[] = {
	'(',
	')',
	'{',
	'}',
	'/',
	'*',
	'-',
	'+',
	'|',
	'\'',
	'@',
	'$',
	'%',
	'&',
	'+',
	//...
}; 

static unsigned char SPECIALOPEN[] = {
	'(',
	'{',
	'[',
	//...
}; 

static unsigned char SPECIALCLOSE[] = {
	')',
	'{',
	']',
	//...
}; 

static unsigned char SPECIALMATH[] = {
	'/',
	'*',
	'-',
	'+',
	'.',
	',',
	//...
}; 

//
// Files (strings).
//

static const char bmp1_file_name[] = "BMP1    BMP";
static const char bmp2_file_name[] = "BMP2    BMP";
static const char bmp3_file_name[] = "BMP3    BMP";
static const char bmp4_file_name[] = "BMP4    BMP";
static const char init_file_name[] = "INIT    TXT";
//... 

//static const char file_bmp1[] = "BMP1.BMP";
//static const char file_bmp2[] = "BMP2.BMP";
//static const char file_bmp3[] = "BMP3.BMP";
//static const char file_bmp4[] = "BMP4.BMP";
//static const char file_init[] = "INIT.TXT";
//... 

#endif   

