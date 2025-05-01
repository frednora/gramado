// term0.h
// Support for terminal application.
// Created by Fred Nora.

#ifndef __TERM0_H
#define __TERM0_H    1

//
// Define control characters.
//

#define TERMINAL_ESCAPE  0x1B
#define TERMINAL_INTRODUCER  '['
#define TERMINAL_PARAMETER_SEPARATOR  ';'
#define TERMINAL_RUBOUT  0x7F

//
// CSI support
// 

// CSI - Control Sequence Introducer.
#define CSI_BUFFER_SIZE  128
extern char CSI_BUFFER[CSI_BUFFER_SIZE];
extern int __csi_buffer_tail;
extern int __csi_buffer_head;
// ===============================

//
// Structures
//

enum term_mode {
	MODE_WRAP        = 1,
	MODE_INSERT      = 2,
	MODE_APPKEYPAD   = 4,
	MODE_ALTSCREEN   = 8,
	MODE_CRLF        = 16,
	MODE_MOUSEBTN    = 32,
	MODE_MOUSEMOTION = 64,
	MODE_MOUSE       = 32|64,
	MODE_REVERSE     = 128,
	MODE_KBDLOCK     = 256,
	MODE_HIDE        = 512,
	MODE_ECHO        = 1024,
	MODE_APPCURSOR   = 2048,
	MODE_MOUSESGR    = 4096,
};

enum escape_state {
	ESC_START      = 1,
	ESC_CSI        = 2,
	ESC_STR        = 4,   /* DSC, OSC, PM, APC */
	ESC_ALTCHARSET = 8,
	ESC_STR_END    = 16,  /* a final string was encountered */
	ESC_TEST       = 32,  /* Enter in test mode */
};

#define ESC_BUF_SIZ  32  //(128*UTF_SIZ)
#define ESC_ARG_SIZ  16
#define STR_BUF_SIZ  ESC_BUF_SIZ
#define STR_ARG_SIZ  ESC_ARG_SIZ

// 
// Escape sequence
//

/* CSI Escape sequence structs */
/* ESC '[' [[ [<priv>] <arg> [;]] <mode>] */
typedef struct {
    char buf[ESC_BUF_SIZ];  /* raw string */
    int len;                /* raw string length */
    char priv;
    int arg[ESC_ARG_SIZ];
    int narg;               /* nb of args */
    char mode;
} CSIEscape;


/* STR Escape sequence structs */
/* ESC type [[ [<priv>] <arg> [;]] <mode>] ESC '\' */
typedef struct {
	char type;                /* ESC type ... */
	char buf[STR_BUF_SIZ];    /* raw string */
	int len;                  /* raw string length */
	char *args[STR_ARG_SIZ];  // ponteiro duplo.
	int narg;                 /* nb of args */
} STREscape;


/*
 * credits: linux 0.1.
 * this is what the terminal answers to a ESC-Z or csi0c
 * query (= vt100 response).
 */
#define RESPONSE  "\033[?1;2c"
#define VT102_ID  "\033[?6c"

//==============================================================
// Isso será usado em terminal.bin
// principalmente para gerenciamento de caracteres ... 
// linhas é o básico.
// todo: 
// Colocar as rotinas de terminal virtual aqui,
// separadas do shell

//
// SCREEN SUPPORT
//

#define DEFAULT_WINDOW_X  0
#define DEFAULT_WINDOW_Y  0

// Obs: 
// aumentar essas constantes aumenta o tamanho da janela.

// Isso é o máximo que se pode exibir na tela.
#define DEFAULT_MAX_COLUMNS  80   //80
#define DEFAULT_MAX_ROWS     25   //50  
  
//linux 
//#define SCREEN_START 0xb8000
//#define SCREEN_END   0xc0000
//#define LINES 25
//#define COLUMNS 80
//#define NPAR 16

//
// BUFFER SUPPORT
//

// Screen Buffer: 
// Igual a tela do vga, com caracteres e atributos.
// @todo: O buffer deve ser maior, com várias vistas para rolagem.
// mas apenas uma parte do buffer será mostrada na tela por vez.

// Isso é o buffer de arquivo. ele pode ser maior que
// a quantidade de linhas que a área de cliente pode mostrar.
#define DEFAULT_BUFFER_MAX_COLUMNS  DEFAULT_MAX_COLUMNS  //80
#define DEFAULT_BUFFER_MAX_ROWS    32                    //25
#define SCREEN_BUFFER_SIZE  \
( ((DEFAULT_BUFFER_MAX_COLUMNS*DEFAULT_BUFFER_MAX_ROWS)*2) +1 )

#define TAB_SIZE  8

/*
#define BLACK       0
#define RED         1
#define GREEN       2
#define BROWN       3
#define BLUE        4
#define MAGENTA     5
#define CYAN        6
#define LIGHTGRAY   7
#define DARKGRAY    8
#define LIGHTRED    9
#define LIGHTGREEN  10
#define YELLOW      11
#define LIGHTBLUE   12
#define PINK        13
#define LIGHTCYAN   14
#define WHITE       15 
*/ 

//#ifndef whitespace
//#define whitespace(c) (((c) == ' ') || ((c) == '\t'))
//#endif 

//#ifndef digit
//#define digit(c)  ((c) >= '0' && (c) <= '9')
//#endif

//#ifndef isletter
//#define isletter(c) \
//    (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
//#endif

//#ifndef digit_value
//#define digit_value(c) ((c) - '0')
//#endif
 
//#define NEWLINE '\n' 
//#define TAB '\t'
//#define SPACE 0x020
 

extern unsigned long __tmp_x;
extern unsigned long __tmp_y;

// Colors
// #deprecated: using fg_color and bg_color.
//cores do texto. 
//unsigned long backgroung_color;  //pano de fundo.
//unsigned long foregroung_color;  //texto.

//
// ## Text support #@#
//

struct terminal_line
{
    char CHARS[80];
    char ATTRIBUTES[80];
// Início e fim da string dentro da linha.
// O resto é espaço.
    int left;
    int right;
// Posição do cursor dentro da linha.
    int pos;
};

// #importante
// É nesse buffer que estamos escrevendo. É como um arquivo.
// PROVISÓRIO
// O TEXTO TEM 32 LINHAS NO MÁXIMO.
// ESSA É A PARTE DO TEXTO QUE PODERÁ SER MANIPULADA,
// O RESTO DO TEXTO DEVERÁ FICAR ESPERANDO NO BUFFER.
// #IMPORTANTE: 25 DESSAS 32 LINHAS SERÃO VISÍVEIS.
// #importante: O início dessas estruturas pode representar o inpicio de 
// um arquivo.
// Esse arquivo pode ser um buffer
// Um arquivo no servidor de recursos gráficos pode ser usado para escrevermos 
// nele usando as rotinas da libc.

//DEFAULT_BUFFER_MAX_ROWS
extern struct terminal_line  LINES[32];

// ==========

//
// Estrutura para mainpular linhas dentro do screen_buffer[]
//

#define MAGIC_NORMALLINE  1234
//...
#define LINE_SIZE_MAX  80
#define LINE_COUNT_MAX  2048
#define SCREENBUFFER_COUNT_MAX  8

struct line_d
{
// Identificação da linha.
    int id;
    int used;
    int magic;
    int Start;
    int End;
// Deslocamentos em relação ao Start.
    int LeftOffset;    //Onde começa o texto dentro da linha.   
    int RightOffset;   //Onde termina o texto dentro da linha.
    // ...
    int SizeInChars;  // Quantos chars tem na linha.
    int SizeInBytes;  // Quantos bytes tem na linha. (char + atrr)
    struct line_d  *next;
};
typedef struct line_d  line_t;

// Conterá ponteiros para estruturas de linha.
extern unsigned long lineList[LINE_COUNT_MAX];

//
// Estrutura de suporte ao screen buffer.
//

struct screen_buffer_d
{
    int id;
    int used;
    int magic;
    char *name;
    char *description;

// Current line support.

    int current_line_id;          //id
    struct line_d *current_line;  //struct 
    //...

// Lines

    struct line_d  *first_line;
    struct line_d  *last_line;
    // ...

// Número total de linhas no buffer.
    int total_lines;
    struct screen_buffer_d  *next;
};
typedef struct screen_buffer_d  screen_buffer_t;

// Conterá ponteiros para estruturas de linha.
extern unsigned long screenbufferList[8];

//
// ====
//

// Marcador do cursor.
extern unsigned long screen_buffer_pos;    //(offset) 
extern unsigned long screen_buffer_x;      //current col 
extern unsigned long screen_buffer_y;      //current row

//
// =====
//

#define CalculateRowScanLine(WindowY,CharHeight) \
    ( WindowY * CharHeight )

#define CalculateColumnScanLine(WindowX,CharWidth) \
    ( WindowX * CharWidth )

//
// =====
//

// Comunicação Cliente/Servidor:
// Número da mensagem enviada pelo terminal virtual.
// São apenas mensagens usadas pelo terminal virtual 
// em modo texto, relativas a input e output de textos.

typedef enum terminal_api_message_number_d {
    terminalNull,          // 0
    terminalOpenTerminal,  // Inicia a estrutura de terminal virtual.
    terminalCloseTerminal, // Fecha a estrutura de terminal.
    terminalGetWindow,     //
    terminalGetMessage,    //
    terminalGetLong1,      //
    terminalGetLong2,      //
    terminalScreenWidth,
    terminalScreenHeight,
    terminalCursorWidth,
    terminalCursorHeight,
    terminalCharWidth,
    terminalCharHeight
}terminal_api_message_number_t;

//
// Main struct
//

#define TERMINAL_MODE_NORMAL    1000
#define TERMINAL_MODE_EMBEDDED  2000

struct terminal_d
{
    int initialized;
    int client_fd;
    int main_window_id;
    int client_window_id;
    int pid;
    int uid;
    int gid;

// 1000 - normal
// 2000 - embedded
    int _mode;

    unsigned long left;
    unsigned long top;
// In pixels.
    unsigned long width;
    unsigned long height;
// In chars.
    unsigned long width_in_chars;
    unsigned long height_in_chars;
    // ...

    int esc;
};

// The man structure.
// see: main.c
extern struct terminal_d  Terminal;

//
// System Metrics
//

extern int smScreenWidth;                   //1 
extern int smScreenHeight;                  //2
extern unsigned long smCursorWidth;         //3
extern unsigned long smCursorHeight;        //4
extern unsigned long smMousePointerWidth;   //5
extern unsigned long smMousePointerHeight;  //6
extern unsigned long smCharWidth;           //7
extern unsigned long smCharHeight;          //8
//...

//
// ======
//

//
// Window limits
//

// Full screen support
extern unsigned long wlFullScreenLeft;
extern unsigned long wlFullScreenTop;
extern unsigned long wlFullScreenWidth;
extern unsigned long wlFullScreenHeight;

// Limite de tamanho da janela.
extern unsigned long wlMinWindowWidth;
extern unsigned long wlMinWindowHeight;
extern unsigned long wlMaxWindowWidth;
extern unsigned long wlMaxWindowHeight;

//
// Linhas
//

// Quantidade de linhas e colunas na área de cliente.
extern int wlMinColumns;
extern int wlMinRows;
extern int __wlMaxColumns;
extern int __wlMaxRows;

//
//  ## Window size ##
//

extern unsigned long wsWindowWidth;
extern unsigned long wsWindowHeight;
//...

//
//  ## Window position ##
//

extern unsigned long wpWindowLeft;
extern unsigned long wpWindowTop;
//..

//==========================
//  Cursor                //  
//==========================

// #deprecated: using cursor_x and cursor_y
//linha e coluna atuais
//int textCurrentRow;
//int textCurrentCol;

//#importante:
//Linhas visíveis.
//número da linha
//isso será atualizado na hora do scroll.
extern int textTopRow;  //Top nem sempre será '0'.
extern int textBottomRow;

extern int textSavedRow;
extern int textSavedCol;

extern int textWheelDelta;     //delta para rolagem do texto.
extern int textMinWheelDelta;  //mínimo que se pode rolar o texto
extern int textMaxWheelDelta;  //máximo que se pode rolar o texto
//...

//
// ===
//

//
// Bg window
// 
 
extern unsigned long __bgleft;
extern unsigned long __bgtop;
extern unsigned long __bgwidth;
extern unsigned long __bgheight;
 
extern unsigned long __barleft;
extern unsigned long __bartop;
extern unsigned long __barwidth;
extern unsigned long __barheight;


//
// == Prototypes =====================================
//

void terminal_write_char (int fd, int window, int c);

void terminalInsertNextChar (char c);
void terminalInsertNullTerminator (void);
void terminalInsertCR (void);
void terminalInsertLF (void);

void lf(void);
void cr(void);
void ri(void);
void del(void);

void __test_escapesequence(int fd);
void tputstring( int fd, char *s );
void tputc (int fd, int window, int c, int len);

char 
terminalGetCharXY ( 
    unsigned long x, 
    unsigned long y );

void 
terminalInsertCharXY ( 
    unsigned long x, 
    unsigned long y, 
    char c );

static void save_cur (void);
static void restore_cur (void);
void terminalClearBuffer (void);

void textSetTopRow (int number);
int textGetTopRow (void);
void textSetBottomRow (int number);
int textGetBottomRow (void);    
void textSetCurrentRow (int number);          
int textGetCurrentRow (void);           
void textSetCurrentCol (int number);           
int textGetCurrentCol (void);

void move_to( unsigned long x, unsigned long y );
int pad_to(int count, char *string);

// See: terminal.c
int terminal_init(unsigned short flags);

#endif   

