/*
 * File: globals.h
 * Global variables.
 * History:
 *     2018 - Created by Fred Nora.
 */

#ifndef __SHELL00_GLOBALS_H
#define __SHELL00_GLOBALS_H    1

//"global command" stuff 
#define glob_percent_sign '%' 
#define glob_underscore '_'
#define glob_square_brackets_left '['
#define glob_square_brackets_right ']'
#define glob_caret '^'
#define glob_asterisk '*'
#define glob_number_sign '#'
#define glob_question_mark '?'
//... 
 
#define SHELL_TERMINAL_COLOR 0
#define SHELL_TERMINAL_COLOR2 0x303030
//...

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
//#define isletter(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
//#endif

//#ifndef digit_value
//#define digit_value(c) ((c) - '0')
//#endif
 
//#define NEWLINE '\n' 
//#define TAB '\t'
//#define SPACE 0x020
 
//
// Estrutura para mainpular linhas dentro do screen_buffer[]
//


#define MAGIC_NORMALLINE 1234
//...

#define LINE_SIZE_MAX 80

typedef struct line_d line_t;
struct line_d
{
	int id;     //identificação da linha.
	
	int used;
	int magic;  //Validade e característica da estrutura.
	
	int Start;
	int End;
	
	//Deslocamentos em relação ao Start.
	int LeftOffset;    //Onde começa o texto dentro da linha.   
	int RightOffset;   //Onde termina o texto dentro da linha.
	
	// Continua
	// ...
	
	int SizeInChars;  // Quantos chars tem na linha.
	int SizeInBytes;  // Quantos bytes tem na linha. (char + atrr)	
	
    struct line_d *next;	
};

#define LINE_COUNT_MAX 2048

//Conterá ponteiros para estruturas de linha.
unsigned long lineList[LINE_COUNT_MAX];


//
// Estrutura de suporte ao screen buffer.
//

typedef struct screen_buffer_d screen_buffer_t;
struct screen_buffer_d
{
	int id;
	
	int used;
	int magic;
	
	char *name;
	char *description;
	
		
    //
    // Current line support.
    //
	
	int current_line_id;              //id.
	struct line_d *current_line;   //struct 
	//...
	
    //
    // lines.
    //	
	
	struct line_d *first_line;
	struct line_d *last_line;
	//...
	
	
	//Continua ...
	
	//Número total de linhas no buffer.
	int total_lines;
	
	struct screen_buffer_d *next;
};


#define SCREENBUFFER_COUNT_MAX 8

//Conterá ponteiros para estruturas de linha.
unsigned long screenbufferList[8];


//
// ## System Metrics ##
//

int smScreenWidth;                   //1 
int smScreenHeight;                  //2
unsigned long smCursorWidth;         //3
unsigned long smCursorHeight;        //4
unsigned long smMousePointerWidth;   //5
unsigned long smMousePointerHeight;  //6
unsigned long smCharWidth;           //7
unsigned long smCharHeight;          //8
//...

//
// ## Window limits ##
//

//full screen support
unsigned long wlFullScreenLeft;
unsigned long wlFullScreenTop;
unsigned long wlFullScreenWidth;
unsigned long wlFullScreenHeight;

//limite de tamanho da janela.
unsigned long wlMinWindowWidth;
unsigned long wlMinWindowHeight;
unsigned long wlMaxWindowWidth;
unsigned long wlMaxWindowHeight;

//quantidade de linhas e colunas na área de cliente.
int wlMinColumns;
int wlMinRows;
int wlMaxColumns;
int wlMaxRows;


//
//  ## Window size ##
//

unsigned long wsWindowWidth;
unsigned long wsWindowHeight;
//...


//
//  ## Window position ##
//

unsigned long wpWindowLeft;
unsigned long wpWindowTop;
//..



//
// ## Text support #@#
//

struct shell_line
{
    char CHARS[80];
    char ATTRIBUTES[80];	
	
	//início e fim da string dentro da linha.
	//o resto é espaço.
	int left;
	int right;
	
	//posição do cursor dentro da linha.
	int pos;
};

//PROVISÓRIO
//O TEXTO TEM 32 LINHAS NO MÁXIMO.
//ESSA É A PARTE DO TEXTO QUE PODERÁ SER MANIPULADA,
//O RESTO DO TEXTO DEVERÁ FICAR ESPERANDO NO BUFFER.
//#IMPORTANTE: 25 DESSAS 32 LINHAS SERÃO VISÍVEIS.
struct shell_line LINES[32]; 


//#importante:
//Linhas visíveis.
//número da linha
//isso será atualizado na hora do scroll.
int textTopRow;  //Top nem sempre será '0'.
int textBottomRow;

//linha e coluna atuais
int textCurrentRow;
int textCurrentCol;

int textSavedRow;
int textSavedCol;


int textWheelDelta; //delta para rolagem do texto.
int textMinWheelDelta;  //mínimo que se pode rolar o texto
int textMaxWheelDelta;  //máximo que se pode rolar o texto
//...


//
// Buffer support.
// 
//
// BUFFER SUPPORT
//

//
// Screen Buffer: 
// Igual a tela do vga, com caracteres e atributos.
// @todo: O buffer deve ser maior, com várias vistas para rolagem.
// mas apenas uma parte do buffer será mostrada na tela por vez.
//

#define DEFAULT_BUFFER_MAX_COLUMNS 80
#define DEFAULT_BUFFER_MAX_ROWS    25
#define SCREEN_BUFFER_SIZE ( ((DEFAULT_BUFFER_MAX_COLUMNS*DEFAULT_BUFFER_MAX_ROWS)*2)+1)


 

//
//  ** SCREEN BUFFER ***
//

//#bugbug: ALGUMAS AQUI NÃO SÃO MAIS USDAS. DELETAR !!!

//O buffer
char screen_buffer[SCREEN_BUFFER_SIZE]; 

//marcador do cursor.
unsigned long screen_buffer_pos;    //(offset) 

unsigned long screen_buffer_x;      //current col 
unsigned long screen_buffer_y;      //current row

static unsigned long screen_buffer_saved_x = 0;
static unsigned long screen_buffer_saved_y = 0;


//...

//
// WINDOW SUPPORT
//

//...

//
// Colors
// 
 
//cores do texto. 
unsigned long backgroung_color;  //pano de fundo.
unsigned long foregroung_color;  //texto.

#endif   


