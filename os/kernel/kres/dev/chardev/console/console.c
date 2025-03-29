// console.c
// This is the virtual console driver, embedded into the kernel base.
// It handles the device-independent part of the console infrastructure.
// + We receive bytes from the ps2 keyboard driver.
// + We print bytes into the screen.
// Created by Fred Nora.

// #todo
// If this code is device independent, 
// so we can move it to another place.
// But at the same

#include <kernel.h>

// #todo
// Why this thing is here.
// Mot it to admin/
#define GCC_VERSION ( __GNUC__ * 10000 \
            + __GNUC_MINOR__ * 100 \
            + __GNUC_PATCHLEVEL__ )


// Fullscreen kernel console.
// Handmade by the kernel at the initialization.
// see: tty.h
struct tty_d  CONSOLE_TTYS[CONSOLETTYS_COUNT_MAX];

// Main consoles.
struct tty_d  *console0_tty;
struct tty_d  *console1_tty;
struct tty_d  *console2_tty;
struct tty_d  *console3_tty;


const char *device_name_nonameconsole = "CON-NONAME";

// 
// Imports
//

// used for handling screen dimensions.
//extern unsigned long gSavedX;
//extern unsigned long gSavedY;

#define __RESPONSE  "\033[?1;2c"

// Se o console esta atuando como um shell comparando palavras.
int ShellFlag=FALSE;

// global
// Foreground console.
int fg_console=0;

// O redirecionador.
// Redirecting console to this pointer.
// see: tty_ioctl: TIOCCONS
struct tty_struct *redirect;

// 0=apaga 1=acende 
int consoleTextCursorStatus=0;
// Cursor
static int saved_x=0;
static int saved_y=0;

// Esse eh o marcador de estagio do escape sequence.
// Usado para manipular csi
static unsigned long __EscapeSequenceStage = 0;

// Array de parâmetros.
#define NPAR  16
static unsigned long par[NPAR];
static unsigned long npar = 0;
static unsigned long ques = 0;
static unsigned char attr = 0x07;


static unsigned long console_interrupt_counter=0;

//
// == Private functions: Prototypes ======================
//

static void __ConsoleDraw_Outbyte (int c, int console_number);

static void __test_path(void);
static void __test_tty(void);
// #todo: Use static modifier.
void __local_ri(void);
void csi_J(int par);
void csi_K(int par);
void csi_m(void);
void csi_M(int nr, int console_number);
void csi_L(int nr, int console_number);


//
// VGA test
//

void DANGER_VGA_clear_screen(void);
void __vga_test1(void);

// =======================

static void __test_path(void)
{
// Test the pathname support.

    int status = -1;
    //unsigned long tmp_size = (512*4096);    // 512*4096 = 2MB
    void *b; //= (void *) allocPages ( 512 );

// ===================================
// #test: 
// Testando carregar usando path.
// #bugbug:
// We have only few pre-allocated buffers,
// so, we only can use four levels for now. (/../../../file.txt)

// Valid shapes
    //char PathAddress[] = "/GRAMADO/TEST.TXT";
    //char PathAddress[] = "/GRAMADO.TXT";
    //char PathAddress[] = "/GRAMADO/FOLDER/TEST.TXT";

// :)
// 4 levels
    char PathAddress[] = "/GRAMADO/FOLDER/FOLDER2/TEST2.TXT";

// Invalid folder name
    //char PathAddress[] = "/GRAMADO///FOLDER/";
    //char PathAddress[] = "/GRAMADO/1234567812345456456/";

// =========================================
// (32*4096) bytes

    //unsigned long BufferSize = (32*4096);
    //b = (void *) allocPages(32); 

// It is a simple text file.
// We don't need too much memory.
// #bugbug: 
// The loader is not getting the file size
// from the fat entry.
    unsigned long BufferSize = (4096);  //One page!

    b = (void *) allocPages(1); 
    if ((void*) b == NULL){
        printk("b fail\n");
        return;
    }

// see: fsload.c
// IN: path, address, buffer size in bytes.
    status = 
        (int) fs_load_path ( 
                  (const char*) PathAddress, 
                  (unsigned long) b,
                  (unsigned long) BufferSize ); 
    if (status<0){
        printk("__test_path: fs_load_path fail\n");
        return;
    }
    if ((void*) b != NULL){
        printk("OUTPUT: {%s}\n",b);
    }
}

void __local_ri(void)
{
    //#todo
}

// goto xy
void 
__local_gotoxy ( 
    int new_x, 
    int new_y, 
    int console_number )
{

// Parameter
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

// Maior que o largura da linha.
    if (new_x > CONSOLE_TTYS[console_number].cursor_right)
    {
        //new_x = CONSOLE_TTYS[console_number].cursor_right;
        return;
    }

// Maior que a altura da coluna.
    if (new_y > CONSOLE_TTYS[console_number].cursor_bottom)
    {
        //new_y = CONSOLE_TTYS[console_number].cursor_bottom;
        return;
    }

// Set x.
    CONSOLE_TTYS[console_number].cursor_x = 
        (unsigned long) (new_x & 0xFFFFFFFF);

// Set y.
    CONSOLE_TTYS[console_number].cursor_y = 
        (unsigned long) (new_y & 0xFFFFFFFF);
}

void __local_save_cur(int console_number)
{
    saved_x = (int) CONSOLE_TTYS[console_number].cursor_x;
    saved_y = (int) CONSOLE_TTYS[console_number].cursor_y;
}

void __local_restore_cur(int console_number)
{

// Parameter:
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

    CONSOLE_TTYS[console_number].cursor_x = 
        (unsigned long) (saved_x & 0xFFFF);

    CONSOLE_TTYS[console_number].cursor_y = 
        (unsigned long) (saved_y & 0xFFFF);
}

void __local_insert_line(int console_number)
{
    int oldtop = 0;
    int oldbottom = 0;

// Parameter:
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

    oldtop    = CONSOLE_TTYS[console_number].cursor_top;
    oldbottom = CONSOLE_TTYS[console_number].cursor_bottom;

    CONSOLE_TTYS[console_number].cursor_top = 
        CONSOLE_TTYS[console_number].cursor_y;

    //#bugbug: apontando par asi mesmo.
    //CONSOLE[console_number].cursor_bottom = CONSOLE[console_number].cursor_bottom;

    //if (console_number<0)
        //return;

    if (CONSOLE_TTYS[console_number].fullscreen_flag == TRUE){
        console_scroll(console_number);
    }

    CONSOLE_TTYS[console_number].cursor_top = oldtop;
    CONSOLE_TTYS[console_number].cursor_bottom = oldbottom;
}

void __local_delete_line(int console_number)
{
    int n=0;
    int oldtop = 0;
    int oldbottom = 0;

// Parameter:
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

    n = console_number;

// save old.
    oldtop    = (int) CONSOLE_TTYS[n].cursor_top;
    oldbottom = (int) CONSOLE_TTYS[n].cursor_bottom;

    CONSOLE_TTYS[n].cursor_top = 
        (int) CONSOLE_TTYS[n].cursor_y;

// #bugbug: apontando para si mesmo.
    //CONSOLE[n].cursor_bottom = CONSOLE[n].cursor_bottom;

//#todo
    //scrup();

    CONSOLE_TTYS[n].cursor_top = oldtop;
    CONSOLE_TTYS[n].cursor_bottom = oldbottom;
}

void csi_J(int par)
{

/*
	long count __asm__("cx");
	long start __asm__("di");

	switch (par) {
		case 0:   //erase from cursor to end of display 
			count = (scr_end-pos)>>1;
			start = pos;
			break;
		case 1:	//erase from start to cursor 
			count = (pos-origin)>>1;
			start = origin;
			break;
		case 2: // erase whole display 
			count = columns*lines;
			start = origin;
			break;
		default:
			return;
	}

    __asm__("cld\n\t"
		"rep\n\t"
		"stosw\n\t"
		::"c" (count),
		"D" (start),"a" (0x0720)
		:"cx","di");

 */
}


// apagar n bytes na mesma linha
void csi_K(int par)
{
}

// Fim da escape sequence.
// isso eh chamado quando encontramos um 'm'.
// O 'm' eh um marcador de fim de escape sequence.
// Entao vamos checar os parametros no buffer.
// Configuramos a variavel de atributo de acordo
// com o parametro encontrado.
// Ficaremos com o ultimo atributo. #bugbug ????

void csi_m(void)
{
// Set some attributes based on the parameters found.
// Sets colors and style of the characters.

    register int i=0;
    int max = (int)(npar & 0xFFFFFFFF);
    int ivalue=0;

// #bugbug
// Check 'npar'

    if (max == 0){
        return;
    }

    for (i=0; i <= max; i++)
    {
        // Get the parameter value.
        ivalue = par[0];
        ivalue = (ivalue & 0xFF);
        ivalue = k_atoi(&ivalue);

        // #todo
        // Call a worker for each of these parameters.
        //console_set_parameter(ivalue);
    };
}

void csi_M (int nr, int console_number)
{

    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

/*
    if ( nr > CONSOLE_TTYS[console_number].cursor_height )
        nr = CONSOLE_TTYS[console_number].cursor_height;
    else if (!nr)
        nr=1;
    while (nr--)
        __local_delete_line(console_number);
*/
}


void csi_L (int nr, int console_number)
{

    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

/*
	if (nr > CONSOLE[console_number].cursor_height)
		nr = CONSOLE[console_number].cursor_height;
	else if (!nr)
		nr=1;
	while (nr--)
		__local_insert_line(console_number);
*/
}

//============================================
// console_interrupt:
// Called by devices that are not block devices.
// Probably keyboard and serial devices.
// See: keyboard.c and serial.c
// Called by DeviceInterface_PS2Keyboard() in ps2kbd.c
// IN:
// #todo: Explain the parameters.

void 
console_interrupt(
    int target_thread, 
    int device_type, 
    int data )
{
// Devece drivers will call this routine to process the data.
// The data is a 'char'.

    // # Not in use.
    tid_t target_tid = (tid_t) target_thread;

    int DeviceType = device_type;
    int Data = data;
    int Status=-1;

    console_interrupt_counter++;

    // # Not in use.
    if (target_tid < 0 || target_tid >= THREAD_COUNT_MAX){
        debug_print ("console_interrupt: target_tid\n");
        goto fail;
    }


    switch (DeviceType){

        // keyboard
        // data =  raw byte.
        // See: kgws.c
        case CONSOLE_DEVICE_KEYBOARD:
            // debug_print("console_interrupt: Input from keyboard device\n");
            // In this case the target tid is the window server.
            // This worker is good for device drivers.
            // IN: 
            // scancode (raw byte), prefix.
            // #bugbug: Where is the prefix?
            //Status =  (int) wmRawKeyEvent( Data, (int) 0 );
            //if (Status<0){
                //goto fail;
            //}
            break;

        // COM port
        case CONSOLE_DEVICE_SERIAL:
            //debug_print("console_interrupt: Input from serial device\n");
            break;
 
        //network device.
        case CONSOLE_DEVICE_NETWORK:
            //debug_print("console_interrupt: Input from network device\n");
            break;

        // ...

        default:
            debug_print("console_interrupt: Undefined input device\n");
            goto fail;
            break;
    };

// done
    return;

fail:
    debug_print ("console_interrupt: Fail\n");
    return;
}

void console_set_current_virtual_console(int console_number)
{
// Set the new fg_console.

    if (console_number == fg_console){
        return;
    }

    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

    fg_console = (int) console_number;
}

int console_get_current_virtual_console(void)
{
    return (int) fg_console;
}


// Called by: 
// __local_ps2kbd_procedure in ps2kbd.c
// VirtualConsole_initialize in console.c
// __initialize_virtual_consoles in kstdio.c
// #todo:
// maybe we can do somo other configuration here.
// Change the foreground console.

void jobcontrol_switch_console(int n)
{
    unsigned int bg_color=COLOR_BLACK;
    unsigned int fg_color=COLOR_WHITE;

    if  (n<0 || 
         n >= CONSOLETTYS_COUNT_MAX )
    {
        return;
    }

// Set the current virtual console.
    console_set_current_virtual_console(n);

// Clear the screen, set bg and fg colors and set the cursor position.
// IN: bg color, fg color, console number.
    bg_color = (unsigned int) CONSOLE_TTYS[n].bg_color;
    fg_color = (unsigned int) CONSOLE_TTYS[n].fg_color;
    console_clear_imp( bg_color, fg_color, n );

// banner
    zero_show_banner();
    consolePrompt();
    //printk ("Console number {%d}\n", n);
}

/*
 * set_up_cursor:
 *     Setup cursor for the current virtual console.
 */
// #todo
// Maybe change to console_set_up_cursor();
// We need a routine to change the cursor inside a wigen window.
// The console has a window ? No ? ... but it has a dimension.
// See the tty structure at hal/dev/tty/tty.h
// We need to know that there is a difference between 
// printing at a given console and printing at a given window.
// The tty has a window, but we are not using it, we are using
// the margins for the console.
// #todo
// Quando estivermos colocando o cursor em uma janela e
// a janela não for válida, então colocamos o cursor na posição 0,0.
// #todo
// Podemos criar uma rotina que mude o cursor de um dado console.
//void set_up_cursor2 ( int console, unsigned long x, unsigned long y );
//void set_up_cursor2 ( int console, unsigned long x, unsigned long y ){}

// Change to setup_fgconsole_cursor(..)
void set_up_cursor ( unsigned long x, unsigned long y )
{

// Console number:
    if (fg_console<0){ 
        return;
    }
    if (fg_console >= CONSOLETTYS_COUNT_MAX){
        return;
    }

// Parameters:
// #todo:
// Check the limits. We don't want to setup an invalid value.

/*
    // #test
    if (x >= CONSOLE_TTYS[fg_console].cursor_right)
        x = (CONSOLE_TTYS[fg_console].cursor_right - 1);
    if (x >= CONSOLE_TTYS[fg_console].cursor_bottom)
        x = (CONSOLE_TTYS[fg_console].cursor_bottom - 1);
*/

    CONSOLE_TTYS[fg_console].cursor_x = (unsigned long) x;
    CONSOLE_TTYS[fg_console].cursor_y = (unsigned long) y;
}

/*
void set_up_cursor2 ( int console_number, unsigned long x, unsigned long y );
void set_up_cursor2 ( int console_number, unsigned long x, unsigned long y )
{
    if (console_number<0){ return; }
    if (console_number>=CONSOLETTYS_COUNT_MAX){ return; }
    CONSOLE_TTYS[console_number].cursor_x = (unsigned long) x;
    CONSOLE_TTYS[console_number].cursor_y = (unsigned long) y;
}
*/

unsigned long get_cursor_x(void)
{
// In the case of called before console initialization.
    if (fg_console<0 || fg_console>=CONSOLETTYS_COUNT_MAX){
        return 0;
    }
    return (unsigned long) CONSOLE_TTYS[fg_console].cursor_x;
}

unsigned long get_cursor_y(void)
{
// In the case of called before console initialization.
    if (fg_console<0 || fg_console>=CONSOLETTYS_COUNT_MAX){
        return 0;
    }
    return (unsigned long) CONSOLE_TTYS[fg_console].cursor_y; 
}

unsigned long get_bg_color(void)
{
    unsigned long DefaultLongWhiteColor = (COLOR_WHITE & 0xFFFFFFFF);

    if (fg_console < 0)
        return DefaultLongWhiteColor;
    if (fg_console >= CONSOLETTYS_COUNT_MAX)
        return DefaultLongWhiteColor;

    return (unsigned long) CONSOLE_TTYS[fg_console].bg_color;
}

unsigned long get_fg_color(void)
{
    unsigned long DefaultLongBlackColor = (COLOR_BLACK & 0xFFFFFFFF);

    if (fg_console < 0)
        return DefaultLongBlackColor;
    if (fg_console >= CONSOLETTYS_COUNT_MAX)
        return DefaultLongBlackColor;

    return (unsigned long) CONSOLE_TTYS[fg_console].fg_color;
}

void console_scroll(int console_number)
{
    register int i=0;
// Salvar cursor
    unsigned long OldX=0;
    unsigned long OldY=0;
// Salvar limites
    unsigned long OldLeft=0;
    unsigned long OldTop=0;
    unsigned long OldRight=0;
    unsigned long OldBottom=0;

    if (VideoBlock.useGui != TRUE){
        panic ("console_scroll: no GUI\n");
    }

// Parameter:
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

// Scroll the screen rectangle.
// See: rect.c
// #todo
// Isso deveria ser apenas scroll_rectangle()
// #bugbug
// Valid only for full screen

    scroll_screen_rect();


// -------------------------------------

// #test
// Draw the rectangle of the last line.

    unsigned long r_bg_color = CONSOLE_TTYS[console_number].bg_color;
    unsigned long CharHeight = 8;

    unsigned long rLeft   = 0;
    unsigned long rTop    = (gSavedY - CharHeight);
    unsigned long rWidth  = gSavedX;
    unsigned long rHeight = CharHeight;

    backbuffer_draw_rectangle(
        rLeft,       // x in pixels
        rTop,        // y in pixels
        rWidth,      // width in pixels
        rHeight,     // height in pixels
        r_bg_color,  // bg color
        0 );         // rop

// -------------------------------------

// Clear the last line.
  
// Salva cursor
    OldX = CONSOLE_TTYS[console_number].cursor_x;
    OldY = CONSOLE_TTYS[console_number].cursor_y;
// Salva cursor limits
    OldLeft   = CONSOLE_TTYS[console_number].cursor_left;
    OldTop    = CONSOLE_TTYS[console_number].cursor_top;
    OldRight  = CONSOLE_TTYS[console_number].cursor_right;
    OldBottom = CONSOLE_TTYS[console_number].cursor_bottom;


// #bugbug
// cursor_bottom Isso não é a última, é o limite.
// A última é cursor_bottom-1.

// Cursor na ultima linha.
// Para podermos limpa-la.
    CONSOLE_TTYS[console_number].cursor_x = 
        CONSOLE_TTYS[console_number].cursor_left; 
    CONSOLE_TTYS[console_number].cursor_y = 
        CONSOLE_TTYS[console_number].cursor_bottom; 

// Limpa a última linha.
// #bugbug: 
// Essa rotina pode sujar alguns marcadores importantes.
// Depois disso precisamos restaurar os valores salvos

/*
   if ( CONSOLE_TTYS[console_number].cursor_left < (CONSOLE_TTYS[console_number].cursor_right-1) )
   {
       for ( i = CONSOLE_TTYS[console_number].cursor_left; 
             i < (CONSOLE_TTYS[console_number].cursor_right-1);
             i++ )
       {
           __ConsoleDraw_Outbyte(' ',console_number);
           //__ConsoleDraw_Outbyte('.',console_number); 
       };
    }
*/

// Restaura limits
    CONSOLE_TTYS[console_number].cursor_left   = (OldLeft   & 0xFFFF);
    CONSOLE_TTYS[console_number].cursor_top    = (OldTop    & 0xFFFF);
    CONSOLE_TTYS[console_number].cursor_right  = (OldRight  & 0xFFFF);
    CONSOLE_TTYS[console_number].cursor_bottom = (OldBottom & 0xFFFF);

// Reposiciona:
// Primeira coluna, última linha.
    CONSOLE_TTYS[console_number].cursor_x = 0; 
    CONSOLE_TTYS[console_number].cursor_y = 
        (CONSOLE_TTYS[console_number].cursor_bottom -1); 

// #todo:
// Essa flag permite que o taskswitch faça o refresh
// da tela.
// see: schedulerUpdateScreen() in kgwm.c
// O certo seria dispatchUpdateScreen()

    //invalidate_screen();  //#bugbug not working

    refresh_screen();
}

// worker
// __ConsoleDraw_Outbyte:
// Outputs a char on the console device;
// Low level function to draw the char into the screen.
// it calls the embedded window server.
// #test
// Tentando pegar as dimensões do char.
// #importante: 
// Não pode ser 0, pois poderíamos ter divisão por zero.
// Called by console_outbyte().

static void __ConsoleDraw_Outbyte (int c, int console_number)
{
// Draw
// Low level.
// This routine is gonna call the function d_draw_char()
// to draw into the screen. this function belongs to the 
// display device driver.
// #todo: We need to change the name of this function
// this way we will know that the function belongs
// to the device driver.  Maybe display_draw_char().

    register int Ch = c;
    //unsigned long Ch = (unsigned long) (c & 0xFF);

// Target console
    int n = (int) console_number;

    int cWidth = get_char_width();
    int cHeight = get_char_height();

    unsigned long screenx=0;
    unsigned long screeny=0;
    unsigned int bg_color = COLOR_BLUE;
    unsigned int fg_color = COLOR_WHITE;

// Parameter:
    if (n < 0)
        return;
    if (n >= CONSOLETTYS_COUNT_MAX)
        return;

    if ( cWidth == 0 || cHeight == 0 )
    {
        x_panic ("__ConsoleDraw_Outbyte: fail w h");
    }

// #bugbug
// Caso estejamos em modo texto.
// Isso ainda não é suportado.
// #Importante: 
// Essa rotina não sabe nada sobre janela, ela escreve na tela como 
// um todo. Só está considerando as dimensões do 'char'.
// Caso estivermos em modo gráfico.
// #importante: 
// Essa rotina de pintura deveria ser exclusiva 
// para dentro do terminal.
// Então essa flag não faz sentido.
// See: char.c

    if (VideoBlock.useGui != TRUE)
    {
        debug_print ("__ConsoleDraw_Outbyte: kernel in text mode\n");
        x_panic     ("__ConsoleDraw_Outbyte: kernel in text mode");
    }

// Screen position.
    screenx = (unsigned long) (cWidth  * CONSOLE_TTYS[n].cursor_x);
    screeny = (unsigned long) (cHeight * CONSOLE_TTYS[n].cursor_y);

// Get the colors for this console.
    bg_color = (unsigned int) CONSOLE_TTYS[n].bg_color; 
    fg_color = (unsigned int) CONSOLE_TTYS[n].fg_color;

// Draw:
// Sempre pinte o bg e o fg.
// see: gre/char.c

    d_draw_char ( screenx, screeny, Ch, fg_color, bg_color );

/*
// ## NÃO TRANPARENTE ##
// Se estamos no modo terminal então usaremos as cores 
// configuradas na estrutura do terminal atual.
// Branco no preto é um padrão para terminal.

    if (stdio_terminalmode_flag == 1){
        // Pinta bg e fg.
        d_draw_char ( screenx, screeny, Ch, fg_color, bg_color );
        return;
    }

// ## TRANSPARENTE ##
// Se não estamos no modo terminal então usaremos
// char transparente.
// Não sabemos o fundo. Vamos selecionar o foreground.     

    // Pinta apenas o fg.
    d_drawchar_transparent ( screenx, screeny, fg_color, Ch );
*/

}

/*
 * console_outbyte:
 *     Trata o caractere a ser imprimido e chama a rotina /_outbyte/
 * para efetivamente colocar o caractere na tela.
 * Essa rotina é chamada pelas funções: /putchar/scroll/.
 * @todo: Colocar no buffer de arquivo.
 */
// This functions calls __ConsoleDraw_Outbyte to draw
// the char into the screen.

void console_outbyte (int c, int console_number)
{
// + Draw char.
// + Do not refresh char.

    register int Ch = c;
    int n = (int) console_number;
    static char prev=0;
// char support.
    unsigned long __cWidth  = gwsGetCurrentFontCharWidth();
    unsigned long __cHeight = gwsGetCurrentFontCharHeight();

    // #debug
    // debug_print ("console_outbyte:\n");

    if (n < 0)
        return;
    if (n >= CONSOLETTYS_COUNT_MAX)
        return;

    if ( __cWidth == 0 || 
         __cHeight == 0 )
    {
        x_panic ("console_outbyte: char size");
    }

// #test
// Tem momento da inicialização em que esse array de estruturas
// não funciona, e perdemos a configuração feita

    if (CONSOLE_TTYS[n].initialized != TRUE)
    {
        debug_print ("console_outbyte: Console not initialized\n");
        //x_panic ("console_outbyte: CONSOLE_TTYS");
        return;
    }

// Obs:
// Podemos setar a posição do curso usando método,
// simulando uma variável protegida.

//checkChar:

    //Opção  
    //switch ?? 

    // type: 'int'.
    if (Ch<0)
        return;

    // form feed - Nova tela.
    if (Ch == '\f')
    {
        CONSOLE_TTYS[n].cursor_y = CONSOLE_TTYS[n].cursor_top;
        CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
        return;
    }

// #obs: #m$. 
// É normal \n retornar sem imprimir nada.

// Início da próxima linha. 
// not used!!!  "...\r\n";

    if ( Ch == '\n' && prev == '\r' ) 
    {
        // #todo: 
        // Melhorar esse limite.
        if (CONSOLE_TTYS[n].cursor_y >= CONSOLE_TTYS[n].cursor_bottom){
            if (CONSOLE_TTYS[n].fullscreen_flag == TRUE ){
                console_scroll(n);
            }
            // Última linha.
            CONSOLE_TTYS[n].cursor_y = ( CONSOLE_TTYS[n].cursor_bottom -1 );
            prev = Ch; 
        }else{
            CONSOLE_TTYS[n].cursor_y++;
            CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
            prev = Ch;
        };
        return;
    }

// Próxima linha no modo terminal.
// "...\n"

    if ( Ch == '\n' && prev != '\r' ) 
    {
        // Se o line feed apareceu quando estamos na ultima linha
        if ( CONSOLE_TTYS[n].cursor_y >= CONSOLE_TTYS[n].cursor_bottom){
            if (CONSOLE_TTYS[n].fullscreen_flag == TRUE ){
                console_scroll(n);
            }
            CONSOLE_TTYS[n].cursor_y = (CONSOLE_TTYS[n].cursor_bottom -1);
            prev = Ch;
        }else{
            CONSOLE_TTYS[n].cursor_y++;
            // Retornaremos mesmo assim ao início da linha 
            // se estivermos imprimindo no terminal.
            if ( stdio_terminalmode_flag == 1 ){
                CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
            } 

            // Verbose mode do kernel.
            // permite que a tela do kernel funcione igual a um 
            // terminal, imprimindo os printk um abaixo do outro.
            // sempre reiniciando x.
            if (stdio_verbosemode_flag == 1){
                CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
            } 

            // Obs: No caso estarmos imprimindo em um editor 
            // então não devemos voltar ao início da linha.

            prev = Ch;
        };

        return;
    }

// TAB
// #todo: Criar a variável 'g_tab_size'.

    if ( Ch == '\t' ) 
    {
        CONSOLE_TTYS[n].cursor_x += (8);
        prev = Ch;
        return; 

        // Não adianta só avançar, tem que apagar o caminho até lá.

		//int tOffset;
		//tOffset = 8 - ( g_cursor_left % 8 );
		//while(tOffset--){
		//	_outbyte(' ');
		//}
		//set_up_cursor( g_cursor_x +tOffset, g_cursor_y );
		//return; 
    }

// Liberando esse limite.
// Permitindo os caracteres menores que 32.

    //if( c <  ' '  && c != '\r' && c != '\n' && c != '\t' && c != '\b' )
    //{
    //    return;
    //};

// Apenas voltar ao início da linha.
    if (Ch == '\r')
    {
        CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
        prev = Ch;
        return; 
    }

// Space
// #bugbug 
// Com isso o ascii 0x20 foi pintado, 
// mas como todos os bits do char na fonte estão desligados, 
// então não pinta coisa alguma.

    if (Ch == 0x20)
    {
        CONSOLE_TTYS[n].cursor_x++;
        prev = Ch;
        return; 
    }

// Backspace

    //if ( Ch == '\b' )
    if (Ch == 0x8)
    {
        CONSOLE_TTYS[n].cursor_x--; 
        prev = Ch;
        return;
    }

//
// == Limits ====
//

//
// Collision
//

// Out of screen
// Sem espaço horizontal.
    if ( CONSOLE_TTYS[n].cursor_left >= CONSOLE_TTYS[n].cursor_right )
    {
        panic ("console_oubyte: l >= r \n");
    }

// Out of screen
// Sem espaço vertical.
    if ( CONSOLE_TTYS[n].cursor_top >= CONSOLE_TTYS[n].cursor_bottom )
    {
        panic ("console_oubyte: t >= b \n");
    }

// Fim da linha.
// Limites para o número de caracteres numa linha.
// Voltamos ao inicio da linha e avançamos uma linha.
// Caso contrario, apenas incrementa a coluna.

    if ( CONSOLE_TTYS[n].cursor_x >= (CONSOLE_TTYS[n].cursor_right -1) ){
        CONSOLE_TTYS[n].cursor_y++;
        CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
    }else{
        CONSOLE_TTYS[n].cursor_x++;
    };

// Número máximo de linhas. (n pixels por linha.)
// #bugbug
// Tem um scroll logo acima que considera um valor
// de limite diferente desse.

    if ( CONSOLE_TTYS[n].cursor_y >= CONSOLE_TTYS[n].cursor_bottom)  
    {
        if (CONSOLE_TTYS[n].fullscreen_flag == TRUE ){
            console_scroll(n);
        }
        CONSOLE_TTYS[n].cursor_x = 0;  //CONSOLE_TTYS[n].cursor_left;
        CONSOLE_TTYS[n].cursor_y = 
            (CONSOLE_TTYS[n].cursor_bottom -1);
    }

// Draw:
// Draw in x*8 | y*8.
// Only if the echo mode is enabled.
// Nesse momento imprimiremos os caracteres.
// Imprime os caracteres normais.
// Atualisa o prev.
// local modes

    tcflag_t c_lflag = (tcflag_t) CONSOLE_TTYS[n].termios.c_lflag;
    if (c_lflag & ECHO){
        __ConsoleDraw_Outbyte(Ch,n);
    }

    prev = Ch;
}

void console_outbyte2 (int c, int console_number)
{
// + Draw char.
// + Refresh char.

    register int Ch = c;
    int n = (int) console_number;
    static char prev=0;
// char support.
    unsigned long __cWidth  = gwsGetCurrentFontCharWidth();
    unsigned long __cHeight = gwsGetCurrentFontCharHeight();

    // #debug
    // debug_print ("console_outbyte2:\n");

    if (n < 0)
        return;
    if (n >= CONSOLETTYS_COUNT_MAX)
        return;

    if ( __cWidth == 0 || 
         __cHeight == 0 )
    {
        x_panic ("console_outbyte2: char size");
    }

// #test
// Tem momento da inicialização em que esse array de estruturas
// não funciona, e perdemos a configuração feita

    if (CONSOLE_TTYS[n].initialized != TRUE)
    {
        debug_print ("console_outbyte2: Console not initialized\n");
        //x_panic ("console_outbyte2: CONSOLE_TTYS");
        return;
    }

// Obs:
// Podemos setar a posição do curso usando método,
// simulando uma variável protegida.

//checkChar:

    //Opção  
    //switch ?? 

    // type: 'int'.
    // #ps: We don't need this.
    // We need the mask at the low part os the integer.
    //if (Ch<0)
        //return;

    // form feed - Nova tela.
    if (Ch == '\f')
    {
        CONSOLE_TTYS[n].cursor_y = CONSOLE_TTYS[n].cursor_top;
        CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
        return;
    }

// #obs: #m$. 
// É normal \n retornar sem imprimir nada.

// Início da próxima linha. 
// not used!!!  "...\r\n";

    if ( Ch == '\n' && prev == '\r' ) 
    {
        // #todo: 
        // Melhorar esse limite.
        
        if (CONSOLE_TTYS[n].cursor_y >= CONSOLE_TTYS[n].cursor_bottom){
            if (CONSOLE_TTYS[n].fullscreen_flag == TRUE ){
                console_scroll(n);
            }
            // Vai para última linha
            CONSOLE_TTYS[n].cursor_y = ( CONSOLE_TTYS[n].cursor_bottom -1 );
            prev = Ch; 
        // Avança uma linha e volta para o começo da linha.
        }else{
            CONSOLE_TTYS[n].cursor_y++;
            CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
            prev = Ch;
        };
        return;
    }

// Próxima linha no modo terminal.
// "...\n"

    if ( Ch == '\n' && prev != '\r' ) 
    {
        // Se o line feed apareceu quando estamos na ultima linha
        if ( CONSOLE_TTYS[n].cursor_y >= CONSOLE_TTYS[n].cursor_bottom){
            if (CONSOLE_TTYS[n].fullscreen_flag == TRUE ){
                console_scroll(n);
            }
            CONSOLE_TTYS[n].cursor_y = (CONSOLE_TTYS[n].cursor_bottom -1);
            prev = Ch;
        }else{
            CONSOLE_TTYS[n].cursor_y++;
            // Retornaremos mesmo assim ao início da linha 
            // se estivermos imprimindo no terminal.
            if ( stdio_terminalmode_flag == 1 ){
                CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
            } 

            // Verbose mode do kernel.
            // permite que a tela do kernel funcione igual a um 
            // terminal, imprimindo os printk um abaixo do outro.
            // sempre reiniciando x.
            if (stdio_verbosemode_flag == 1){
                CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
            } 

            // Obs: No caso estarmos imprimindo em um editor 
            // então não devemos voltar ao início da linha.

            prev = Ch;
        };

        return;
    }

// TAB
// #todo: Criar a variável 'g_tab_size'.

    if ( Ch == '\t' ) 
    {
        CONSOLE_TTYS[n].cursor_x += (8);
        prev = Ch;
        return; 

        // Não adianta só avançar, tem que apagar o caminho até lá.

		//int tOffset;
		//tOffset = 8 - ( g_cursor_left % 8 );
		//while(tOffset--){
		//	_outbyte(' ');
		//}
		//set_up_cursor( g_cursor_x +tOffset, g_cursor_y );
		//return; 
    }

// Liberando esse limite.
// Permitindo os caracteres menores que 32.

    //if( c <  ' '  && c != '\r' && c != '\n' && c != '\t' && c != '\b' )
    //{
    //    return;
    //};

// Apenas voltar ao início da linha.
    if (Ch == '\r')
    {
        CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
        prev = Ch;
        return; 
    }

// Space
// #bugbug 
// Com isso o ascii 0x20 foi pintado, 
// mas como todos os bits do char na fonte estão desligados, 
// então não pinta coisa alguma.

    if (Ch == 0x20)
    {
        CONSOLE_TTYS[n].cursor_x++;
        prev = Ch;
        return; 
    }

// Backspace
    //if (Ch == 0x8)
    if ( Ch == '\b' )
    {
        prev = Ch;
        if (CONSOLE_TTYS[n].cursor_x > 0){
            CONSOLE_TTYS[n].cursor_x--; 
            return;
        }
        if (CONSOLE_TTYS[n].cursor_y > 0)
        {
            CONSOLE_TTYS[n].cursor_y -= 1;
            CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_right-1;
            return;
        }
    }

//
// == Limits ====
//

//
// Collision.
//

// Out of screen
// Sem espaço horizontal.
    if ( CONSOLE_TTYS[n].cursor_left >= CONSOLE_TTYS[n].cursor_right )
    {
        panic ("console_outbyte2: l >= r \n");
    }

// Out of screen
// Sem espaço vertical.
    if ( CONSOLE_TTYS[n].cursor_top >= CONSOLE_TTYS[n].cursor_bottom )
    {
        panic ("console_outbyte2: t >= b \n");
    }

    int Increment = FALSE;

// Fim da linha.
// Limites para o número de caracteres numa linha.
// Voltamos ao inicio da linha e avançamos uma linha.
// Caso contrario, apenas incrementa a coluna.

    if ( CONSOLE_TTYS[n].cursor_x >= (CONSOLE_TTYS[n].cursor_right -1) ){
        CONSOLE_TTYS[n].cursor_y++;
        CONSOLE_TTYS[n].cursor_x = CONSOLE_TTYS[n].cursor_left;
    }else{
        //CONSOLE_TTYS[n].cursor_x++;
        Increment = TRUE;
    };

// Número máximo de linhas. (n pixels por linha.)
// #bugbug
// Tem um scroll logo acima que considera um valor
// de limite diferente desse.

    if ( CONSOLE_TTYS[n].cursor_y >= CONSOLE_TTYS[n].cursor_bottom)  
    {
        if (CONSOLE_TTYS[n].fullscreen_flag == TRUE ){
            console_scroll(n);
        }
        CONSOLE_TTYS[n].cursor_x = 0;  //CONSOLE_TTYS[n].cursor_left;
        CONSOLE_TTYS[n].cursor_y = 
            (CONSOLE_TTYS[n].cursor_bottom -1);
    }

// draw:
// Draw in x*8 | y*8.
// Don't change the position.
// Only if the echo mode is enabled.
// Nesse momento imprimiremos os caracteres.
// Imprime os caracteres normais.

// local modes
    tcflag_t c_lflag = (tcflag_t) CONSOLE_TTYS[n].termios.c_lflag;
    if (c_lflag & ECHO){
        __ConsoleDraw_Outbyte(Ch,n);
    }

// Atualisa o prev.
    prev = Ch;

// Refresh
    unsigned long x=0;
    unsigned long y=0;
    x = (unsigned long) (CONSOLE_TTYS[n].cursor_x * __cWidth);
    y = (unsigned long) (CONSOLE_TTYS[n].cursor_y * __cHeight);
    refresh_rectangle ( x, y, __cWidth, __cHeight );

// Increment
    if (Increment){
        CONSOLE_TTYS[n].cursor_x++;
    }
}

void console_echo(int c, int console_number)
{
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;
    console_outbyte2(c,console_number);
}

/*
 * console_putchar:
 *     Put a char into the screen of a virtual console.
 *     pinta no backbuffer e faz refresh apenas do retangulo do char.
 */
// #importante
// Colocamos um caractere na tela de um console virtual.
// #bugbug: 
// Como essa rotina escreve na memória de vídeo,
// então precisamos, antes de uma string efetuar a
// sincronização do retraço vertical e não a cada char.

void console_putchar(int c, int console_number)
{
// + Draw char.
// + Refresh char.

// Char info and
// parameter for refresh worker.
    unsigned long x=0;
    unsigned long y=0;
    unsigned long cWidth  = get_char_width();
    unsigned long cHeight = get_char_height();

// Parameter
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

    cWidth  = (unsigned long) (cWidth  & 0xFFFF);
    cHeight = (unsigned long) (cHeight & 0xFFFF);
    if (cWidth == 0 || cHeight == 0){
        panic ("console_putchar: char\n");
    }

// flag on.
    stdio_terminalmode_flag = TRUE;

// Draw the char into the backbuffer and
// Copy a small rectangle to the framebuffer.

    //if( c != 0)
    console_outbyte( (int) c, console_number );

// #danger
// We will no be able to refresh if the routine above
// change the cursor position, incrementing or
// comming back to the start of the line.

    x = (unsigned long) (CONSOLE_TTYS[console_number].cursor_x * cWidth);
    y = (unsigned long) (CONSOLE_TTYS[console_number].cursor_y * cHeight);
    refresh_rectangle( x, y, cWidth, cHeight );

// flag off
    stdio_terminalmode_flag = FALSE; 
}

// Print identation.
// Print consecutive spaces.
void console_print_indent(int indent, int console_number)
{
    register int i=0;
    const int Ch = ' ';

// What is the max limit?
    if (indent < 0)
        indent=0;

    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

    for (i=0; i<indent; i++){
        console_putchar(Ch,console_number);
    };
}


// Coloca no prompt[] para ser comarado.
// Talvez o prompt também seja o buffer de stdin
int consoleInputChar(int c)
{
    unsigned long ascii=0;
    ascii = (unsigned long) (c & 0xFF);

    return (int) kinput(ascii);
}


void __dummy_thread(void)
{
    while (1){
    };
}

void __test_process(void)
{
    struct process_d *p;

// Wrapper
// Only ring3 for now.
// See: sys.c
    p = (void*) sys_create_process ( 
            NULL, 
            0,                      // Reserved
            PRIORITY_NORMAL_THRESHOLD,          // priority
            get_current_process(),  // ppid
            "no-name",              // name
            RING3 );                // iopl 

    if ((void*) p == NULL){
        printk("p fail\n");
        return;
    }

    //show_process_information();
        
// ok, it is working.
// Create and initialize a process structure.
    //__create_and_initialize_process_object();
}

// #test
// Creating a ring0 thread.
// #bugbug: 
// We're having problems with ring0 context switching.

void __test_thread(void)
{

// #bugbug: ring0 threads are a huge problem.
// Avoid that for now.

    struct thread_d *t;

    t = 
        (struct thread_d *) create_thread ( 
                                NULL,
                                __dummy_thread,    // init_rip, 
                                PRIORITY_NORMAL_THRESHOLD, 
                                get_current_process(), 
                                "no-name",
                                RING0 ); 
    if ((void*) t == NULL){
        printk("fail\n");
        return;
    }

    //show_slots();
}

void DANGER_VGA_clear_screen(void)
{
/*
    unsigned int x=0;
    unsigned int y=0;

    unsigned char *VGA_address = (unsigned char *) 0xA0000;
    //unsigned char *VGA_address = (unsigned char *) FRONTBUFFER_VA;
    unsigned int VGA_width = 320;
    unsigned int VGA_height = 200;

    //printk ("VGA_clear_screen:\n");
    
    for(y=0; y<VGA_height; y++)
    {
        for (x=0; x<VGA_width; x++)
        {
            VGA_address[ VGA_width*y+x ] = 0x0f;
            //VGA_address[ y+x ] = 0xFF;
        };
    };

    while(1){}
*/

    printk("done\n");
    refresh_screen();
}

void __vga_test1(void)
{
    unsigned char *p = (unsigned char *) 0x000A0000;
    *p = 0x10;
    p++;
    *p = 0x80;
    p++;
    *p = 0xC0;
    p++;
}

static void __test_tty(void)
{
// + Write to the kernel's tty.
// + Read from the kernel's tty.
// + Print the buffer.

//  From
    char data0[8];
    data0[0]='a';  data0[1]='b';  data0[2]='c';  data0[3]=0;

// To (Dirty)
    char data1[8];
    data1[0]='x';  data1[1]='y';  data1[2]='z';  data1[3]=0; //dirty

// Kernel process.
    if ((void*) KernelProcess == NULL){
        return;
    }
    if (KernelProcess->magic != 1234){
        return;
    }

// Write to the kernel's tty.
    __tty_write(KernelProcess->tty,data0,3);
// Read from the kernel's tty.
    __tty_read (KernelProcess->tty,data1,3);

// Print the buffer.
    printk("%c\n",data1[0]);
    printk("%c\n",data1[1]);
    printk("%c\n",data1[2]);

// -------------------------
// #test
// Testing the legacy pty.

// master
    if ((void*) legacy_pty_master == NULL)
        return;
    if (legacy_pty_master->magic == 1234)
        printk("legacy_pty_master OK\n");
    //if (legacy_pty_master->subtype == TTY_SUBTYPE_PTY_MASTER)
        //printk("legacy_pty_master OK\n");
    printk("type %d\n",legacy_pty_master->type);
    printk("subtype %d\n",legacy_pty_master->subtype);

// slave
    if ((void*) legacy_pty_slave == NULL)
        return;
    if (legacy_pty_slave->magic == 1234)
        printk("legacy_pty_slave OK\n");
    //if (legacy_pty_slave->subtype == TTY_SUBTYPE_PTY_SLAVE)
        //printk("legacy_pty_slave OK\n");
    printk("type %d\n",legacy_pty_slave->type);
    printk("subtype %d\n",legacy_pty_slave->subtype);

// 
    printk("Done\n");
}

// consolePrompt:
// Inicializa o prompt.
// Clean prompt buffer.
// Print the prompt string.
void consolePrompt(void)
{
    register int i=0;

//
// Prompt
//

    for ( i=0; i<PROMPT_MAX_DEFAULT; i++ ){ 
        prompt[i] = (char) '\0'; 
    };
    prompt[0] = (char) '\0';
    prompt_pos = 0;
    prompt_status = 0;
    prompt_max = PROMPT_MAX_DEFAULT;  
    printk("\n");
    printk("$ ");
    refresh_screen();
    //invalidate_screen();

//
// Initialize escape sequence.
//
    //for (i=0; i<NPAR; i++){
    //    par[i]=0;
    //};
    //__EscapeSequenceStage=0;
}

// __console_write:
// No escape sequence support.
ssize_t 
__console_write ( 
    int console_number, 
    const void *buf, 
    size_t count )
{

// #todo
// #bugbug
// Precisamos checar os limites para o buffer.
// Não podemos aceitar escrever um numero muito grande
// de bytes num buffer qualquer, correndo o risco
// sujarmos alguma área importante.

// Arguments.
    int n = (int) console_number;
    char *data = (char *) buf;
    size_t MaxSize = (size_t) count;

    register size_t i=0;
    char ch=0; 

// Console number
    if ( n < 0 || n > 3 )
    {
       printk ("__console_write: n\n");
       goto fail;
    }
// Buffer pointer
    if ( (void *) buf == NULL )
    {
        printk ("__console_write: buf\n");
        goto fail;
    }
// Max size.
    //if (count==0 || count > ?)
    if (!count){
        printk ("__console_write: count\n");
        goto fail;
    }

// Write string
    for (i=0; i<MaxSize; i++){
        console_putchar ( (int) data[i], (int) n );
    };
// Return the counter.
    return (ssize_t) MaxSize;
fail:
    refresh_screen();
    return (ssize_t) (-1);
}

// console_read:
// #todo
// Isso é importante.
// Pegar input na estrutura de console do kernel.

ssize_t 
console_read ( 
    int console_number, 
    const void *buf, 
    size_t count )
{
// #todo
// Not fully implemented!

// Console mumber
    if (console_number < 0){
        printk ("console_read: console_number\n");
        goto fail;
    }
    if (console_number >= CONSOLETTYS_COUNT_MAX){
        printk ("console_read: console_number\n");
        goto fail;
    }
// Buffer
    if ((void *) buf == NULL){
        printk ("console_read: buf\n");
        goto fail;
    }
// Count
    if (!count){
        printk ("console_read: count\n");
        goto fail;
    }

    //debug_print ("console_read: [TODO]\n");

/*
// #test
    __tty_read( 
        (struct tty_d *) &CONSOLE_TTYS[console_number],
        buf,
        count );
*/

fail:
    return (int) -1;  //todo
}

// ------------------------------
// console_write:
// Called by sys_write() in fs.c quando o arquivo é do tipo console.
// Also called by sci.c.
// Tem escape sequence.
// IN:
// console number, buffer, size. 

ssize_t 
console_write ( 
    int console_number, 
    const void *buf, 
    size_t count )
{
// Write n bytes of a string into a given console.
// Called by sys_write.

// loop
    register int i=0;

    char *data = (char *) buf;
    unsigned char ch=0; 

    size_t StringSize=0;
    int ivalue=0;
    int ivalue2=0;
    int ivalue3=0;
    
    struct tty_d *tty;
    int DoEcho = FALSE;

    //debug_print ("console_write: [test]\n");

// Parameters:
    if (console_number < 0)
        goto fail;
    if (console_number >= CONSOLETTYS_COUNT_MAX){
        goto fail;
    }
// Buffer
    if ((void *) buf == NULL){
        printk ("console_write: buf\n");
        goto fail;
    }
// Count
    if (!count){
        printk ("console_write: count\n");
        goto fail;
    }

//
// tty
//

// Get the tty pointer
    tty = (struct tty_d *) CONSOLE_GET_TTY_ADDRESS(console_number);
    if ((void*) tty == NULL)
        panic("console_write: tty\n");
    if (tty->magic != 1234)
        panic("console_write: tty validation\n");

//
// Echo?
//

    if (tty->termios.c_lflag & ECHO)
        DoEcho = TRUE;

//
// Write string
//   

/*
// #test
    __tty_write( 
        (struct tty_d *) &CONSOLE_TTYS[console_number],
        buf,
        count );
*/

// Isso eh o contador de estagios do escape sequence.
// Vamos percorret todos os bytes da sequencia quando
// encontrarmos o marcador.
// #todo
// Esse marcador de estagio deve ser especifico para
// cada console. CONSOLE[i].esc_stage

    __EscapeSequenceStage = 0; 

     StringSize = count;

    for (i=0; i<StringSize; i++)
    {
        // Get next char from the string.
        ch = (unsigned char) data[i];

        // Select the stage in the escape sequence.
        switch (__EscapeSequenceStage){

            //================================================
            // Stage 0
            // Nao entramos em uma escape sequence.
            case 0:
                // #debug
                //console_putchar ( '@',console_number);
                //console_putchar ( '0',console_number);
                //console_putchar ( '\n',console_number);
               
                // Is printable?
                // regular ascii printable. Not abnt2.
                //if (ch >= 32 && ch <= 127){
                // #test: Let's print also the extended ascii chars.
                if (ch >= 32 && ch <= 256 && ch != 127){

                    // Draw and refresh.
                    if (DoEcho == TRUE)
                    {

                        // Regular printable
                        if (ch >= 32 && ch <= 127){
                            console_echo(ch,console_number);
                        
                        // Extended printable
                        } else if (ch >= 128 && ch < 256){
                            
                            // #test: 
                            // Testing extended ascii chars. (Not working)
                            // Maybe there is no BIOS font for this.
                            // console_echo(ch,console_number);
                        };
                    }

                // >>>> [ Escape ]
                // Entramos em uma escape sequence,
                // entao o proximo case inicia o tratamento da escape sequence.
                } else if (ch == 27){
                    __EscapeSequenceStage=1;
               
                // ?? \n
                //}else if (ch==10 || ch==11 || ch==12){
                } else if (ch == '\n'){
                    console_putchar(ch,console_number);
                // Enter ? cr \n
                } else if (ch == '\r'){ 
                    console_putchar(ch,console_number); 
                // Backspace
                } else if (ch=='\b') {
                    console_putchar(ch,console_number);
                // Tab.
                } else if (ch=='\t') {
                    console_putchar(ch,console_number); 
                };
               
                break;
            
            //================================================
            // Stage 1
            // Acabamos de entrar na escape sequence.
            // Vamos checar o primeiro char.
            case 1:
            
                // #debug
                //console_putchar ( '@',console_number);
                //console_putchar ( '1',console_number);
                //console_putchar ( '\n',console_number);
            
                // Zerando o marcador de estagio.
                // Isso porque o estagio mudara dependendo do char encontrado agora.
                __EscapeSequenceStage=0; 
                

                // ESC [ -  CSI Control sequence introducer
                // https://man7.org/linux/man-pages/man4/console_codes.4.html
                if (ch=='['){
                    __EscapeSequenceStage = 2;

                // ESC E - NEL  Newline.
                }else if (ch=='E'){ 
                    __local_gotoxy ( 0, (CONSOLE_TTYS[console_number].cursor_y + 1), console_number );
                    //__EscapeSequenceStage = 0;
                
                // ESC M - RI Reverse linefeed.
                }else if (ch=='M'){
                    __local_ri ();   //scroll. deixa pra depois. kkk
                    //__EscapeSequenceStage = 0;
                
                // ESC D - IND Linefeed.
                }else if (ch=='D'){
                    console_putchar ( ch, console_number );  //lf();
                    //__EscapeSequenceStage = 0;
                
                // ESC Z - DECID  DEC private identification.
                // The kernel returns the string  ESC[?6c, 
                // claiming that it is a VT102.
                }else if (ch=='Z'){
                    __respond (console_number);    //test
                    //__EscapeSequenceStage = 0;
                
                //??
                // tivemos um 0x1b e o cursor esta em determinado lugar.
                }else{
                }
                
                break;

            //================================================
            // Stage 2
            // Chegamos aqui depois de encontrarmos um CSI. '0x1b['
            // O que vem apos o '[' sao parametros separados por delimitadores
            // e terminados com um 'm'. 
            case 2:
            
                // #debug
                //console_putchar ( '@',console_number);
                //console_putchar ( '2',console_number);
                //console_putchar ( '\n',console_number);
 
                // Limpando o array de parametros.
                for ( npar=0; npar<NPAR; npar++ ){ 
                    par[npar]=0;
                };
                npar=0;
                //Mudando de estago para checar os parametros.
                __EscapeSequenceStage=3;  // Next state.
                
                //Se o primeiro char apos o '[' for um '?'
                // entao vamos para o proximo estagio, mas quebraremos 
                // par ao for pegar o proximo char.
                // Caso contrario vamos para o proximo estago sem pegarmos o proximo char?
                if ( ques = ( ch == '?' ) ) 
                    break;

            //================================================
            // Stage 3
            case 3:
                // #debug
                //console_putchar ( '@',console_number);
                //console_putchar ( '3',console_number);
                //console_putchar ( '\n',console_number);
            
                // Se encontramos um delimitador
                // e ainda nao acabou o array usado para parametros.
                // entao avançamos para o proximo char nos parametros.
                if ( ch == ';' && npar < NPAR-1 ) {
                    // Avançamos, mas quebramos para que o for pegue
                    // o proximo char da string.
                    //#bugbug: Não moda de state??
                    npar++;  break;  

                // Isso nao eh um delimitador.
                // Checamos se eh um numero.
                } else if ( ch >= '0' && ch <='9'){

                    // Sim eh um numero.
                    // #bugbug?
                    // Multiplicamos o que estava no buffer por 10.
                    // Mas nem sei o que ja estava no buffer??
                    // porque fizemos isso ?
                    // O valor no buffer provavelmente é 0,
                    // no primeiro numero.
                    // Sendo assim quebramos para 
                    // tentarmos pegar mais numeros no for.
                    //par[npar] = (10 * par[npar]) +ch -'0';
                    par[npar] = ch;
                    break;

                 // Nao eh um delimitador nem um numero.
                 // entao vamos para o proximo estagio
                 // porque provavelmente eh uma letra.
                 // Nao precisamos quebrar, pois ja temos um char.
                 } else { 
                     __EscapeSequenceStage = 4;
                 };

            // Stage 4
            case 4:
                // #debug
                //console_putchar ( '@',console_number);
                //console_putchar ( '4',console_number);
                //console_putchar ( '\n',console_number);

                // Zerando o marcador de estagio.
                // Significa que se quebrarmos, vamos voltar para o estagio zero.
                // No estagio zero estamos fora da escape sequence.
 
                __EscapeSequenceStage=0;
                
                switch (ch){

                    // mudamos o cursor e saimos da escape sequence
                    case 'G': case '`':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        __local_gotoxy ( 
                            ivalue, 
                            CONSOLE_TTYS[console_number].cursor_y, 
                            console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    // Mudamos o cursor e saimos da escape sequence
                    // n lines up
                    case 'A':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        __local_gotoxy ( 
                            CONSOLE_TTYS[console_number].cursor_x,  
                            CONSOLE_TTYS[console_number].cursor_y - ivalue, 
                            console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    case 'B': case 'e':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        __local_gotoxy ( 
                            CONSOLE_TTYS[console_number].cursor_x, 
                            CONSOLE_TTYS[console_number].cursor_y + ivalue, 
                            console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    // Cursor right.
                    // mudamos o cursor e saimos da escape sequence 
                    case 'C': case 'a':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        __local_gotoxy ( 
                            CONSOLE_TTYS[console_number].cursor_x + ivalue, 
                            CONSOLE_TTYS[console_number].cursor_y, 
                            console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    // Cursor left.
                    // mudamos o cursor e saimos da escape sequence
                    case 'D':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        __local_gotoxy ( 
                            CONSOLE_TTYS[console_number].cursor_x - ivalue, 
                            CONSOLE_TTYS[console_number].cursor_y, 
                            console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    // mudamos o cursor e saimos da escape sequence
                    case 'E':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        __local_gotoxy ( 
                            0, 
                            CONSOLE_TTYS[console_number].cursor_y + ivalue, 
                            console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    // mudamos o cursor e saimos da escape sequence
                    case 'F':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        __local_gotoxy ( 
                            0, 
                            CONSOLE_TTYS[console_number].cursor_y - ivalue, 
                            console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    // mudamos o cursor e saimos da escape sequence
                    case 'd':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);

                        __local_gotoxy ( 
                            CONSOLE_TTYS[console_number].cursor_x, 
                            ivalue, 
                            console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    // mudamos o cursor e saimos da escape sequence
                    case 'H': case 'f':

                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);

                        ivalue2 = par[1];
                        ivalue2 = (ivalue2 & 0xFF);
                        ivalue2 = k_atoi(&ivalue2);

                        __local_gotoxy ( 
                            ivalue,
                            ivalue2, 
                            console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    // Outros tratadores.
                    case 'J': 
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        csi_J(ivalue);
                        __EscapeSequenceStage = 4;
                        break;
                    case 'K':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        csi_K(ivalue);
                        __EscapeSequenceStage = 4;
                        break;
                    case 'L':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        csi_L( ivalue, console_number );
                        __EscapeSequenceStage = 4;
                        break;
                    case 'M':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        csi_M( ivalue, console_number );
                        __EscapeSequenceStage = 4;
                        break;
                    case 'P':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        csi_P( ivalue, console_number );
                        __EscapeSequenceStage = 4;
                        break;
                    case '@':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);
                        csi_at( ivalue, console_number );
                        __EscapeSequenceStage = 4;
                        break;

                    // FIM.
                    // Isso marca o fim da escape sequence,
                    // vamos quebrar e voltar para a string normal.
                    // Essa rotina cheaca os parametros e configura o atributo
                    // de acordo com o ultimo parametro.
                    case 'm': 
                        //printk ("m found\n");
                        //csi_m(); // Set some attributes based on the parameters found.
                        // Limpando o array de parametros.
                        for ( npar=0; npar<NPAR; npar++ ){ 
                            par[npar]=0;
                        };
                        npar=0;
                        __EscapeSequenceStage = 0;
                        break;

                    // ??  #bugbug
                    // 0x1b[r
                    // Isso ajusta o top e o bottom.
                    case 'r':
                        ivalue = par[0];
                        ivalue = (ivalue & 0xFF);
                        ivalue = k_atoi(&ivalue);

                        ivalue2 = par[1];
                        ivalue2 = (ivalue2 & 0xFF);
                        ivalue2 = k_atoi(&ivalue2);

                        //if (par[0])  { par[0]--; }
                        //if (!par[1]) { par[1] = CONSOLE_TTYS[console_number].cursor_bottom; }  
                        //if (par[0] < par[1] &&
                        //    par[1] <= CONSOLE_TTYS[console_number].cursor_bottom ) 
                        //{
                            // Ajuste feito por 'r'.
                        //    CONSOLE_TTYS[console_number].cursor_top    = par[0];
                         //   CONSOLE_TTYS[console_number].cursor_bottom = par[1];
                        //}
                        
                        __EscapeSequenceStage = 4;
                        break;

                    // Save cursor
                    case 's': 
                        __local_save_cur(console_number);
                        __EscapeSequenceStage = 4;
                        break;

                    // Restore cursor
                    case 'u': 
                        __local_restore_cur(console_number);
                        __EscapeSequenceStage = 4;
                        break;
                };
                break;

            // Stage fail
            default:
                printk ("console_write: default\n");
                goto fail;
                break;
        };
    };  // FOR 

// done:
    return (ssize_t) StringSize;
fail:
    refresh_screen();
    return (ssize_t) -1;
}

ssize_t console_write_string(int console_number, const char *string)
{
// Write a string into a console.
    char *p;
    p = string;
    register size_t i=0;
    size_t StringSize=0;
    size_t Counter = 0;

// Parameters:

    if (console_number < 0)
        goto fail;
    if (console_number >= CONSOLETTYS_COUNT_MAX){
        goto fail;
    }

    if ((void*) string == NULL)
        goto fail;
    if (*string == 0)
        goto fail;
    StringSize = (size_t) strlen(string);
// Limits:
// Is there a maximum limit?
    if (StringSize <= 0){
        goto fail;
    }

// Write
    for (i=0; i<StringSize; i++)
    {
        console_write ( console_number, p, 1 );
        Counter++;
        p++;
    };
    return (ssize_t) Counter;

fail:
    return (ssize_t) 0;
}

void __respond (int console_number)
{
    char *p = __RESPONSE;

    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;
    console_write_string(console_number,p);
}

// See:
// https://en.wikipedia.org/wiki/Control_character
// https://en.wikipedia.org/wiki/C0_and_C1_control_codes#Device_control

void __local_insert_char(int console_number)
{

    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

	//int i=x;
	//unsigned short tmp,old=0x0720;
	//unsigned short * p = (unsigned short *) pos;

/*
	while (i++ < columns) {

		// Salvo o char da posição atual.
		tmp = *p;

        // coloco o char de controle na posição atual
        // ou o caractere salvo anteriomente.
		*p=old;

        // coloco o caractere salvo para uso futuro.
		old=tmp;

        //avança até o fim da linha.
		p++;
	}
*/

    //int i = CONSOLE[console_number].cursor_x;
    //int tmp;
    //int old = 0x20;
    
    // #bugbug
    // Não é possível fazer essa rotina pois não temos um buffer e chars.

    //while (i++ < CONSOLE[console_number].cursor_height ) {};
    
    console_putchar (0x20, console_number);
}


void __local_delete_char(int console_number)
{

// Parameters:
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;


    console_putchar ( ' ', console_number);

/*
	int i;
	unsigned short * p = (unsigned short *) pos;

	if (x >= columns)
		return;

	i = x;
	while (++i < columns) 
	{
		*p = *(p+1);
		p++;
	}
	*p=0x0720;
*/

}

// #todo: Explain the parameters.
void csi_P (int nr, int console_number)
{

// Parameters:
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

    if (nr > CONSOLE_TTYS[console_number].cursor_right -1 ){
        nr = CONSOLE_TTYS[console_number].cursor_right -1 ;
    } else {
        if (!nr){
            nr = 1;
        }
    };

    if (nr<0){
        return;
    }

    while (nr--){
        __local_delete_char(console_number);
    };
}

// #todo: Explain the parameters.
void csi_at (int nr, int console_number)
{

// Parameters:
    if (console_number < 0)
        return;
    if (console_number >= CONSOLETTYS_COUNT_MAX)
        return;

    if (nr > CONSOLE_TTYS[console_number].cursor_right -1 ){
        nr = CONSOLE_TTYS[console_number].cursor_right -1 ;
    }else{
        if (!nr){
            nr=1;
        }
    };

    if (nr<0){
        return;
    }

    while (nr--){
        __local_insert_char(console_number);
    };
}

// Console banner:
// This will be the first message in the screen.
// It is goona be seen at top/left corner of the screen.

void 
console_banner(
    const char *product_string, 
    const char *build_string, 
    unsigned long banner_flags )
{

    unsigned int bg_color = COLOR_BLUE;
    unsigned int fg_color = COLOR_WHITE;

// Serial debug
    if (Initialization.is_serial_log_initialized == TRUE){
        debug_print ("Gramado OS\n");
    }

//
// Kernel console
//

    if (Initialization.is_console_log_initialized != TRUE){
        return;
    }

    if (fg_console < 0)
        return;
    if (fg_console >= CONSOLETTYS_COUNT_MAX)
        return;

    if (CONSOLE_TTYS[fg_console].initialized != TRUE)
    {
        debug_print ("console_banner: fg_console not initialized\n");
        //x_panic("x");
        return;
    }

    bg_color = CONSOLE_TTYS[fg_console].bg_color;
    fg_color = CONSOLE_TTYS[fg_console].fg_color;
    console_clear_imp( bg_color, fg_color, fg_console );
    set_up_cursor(0,0);

    if ((void*) product_string == NULL)
        return;
    if ((void*) build_string == NULL){
        return;
    }
    printk ("%s %s\n", product_string, build_string);
    // #debug Print gcc version
    // printk("gcc: %d\n",GCC_VERSION);
}

// console_ioctl:
// Podemos mudar as características de um console.
// #bugbug: fd and fg_console needs to be the same?
int 
console_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
// + First, the application opens the console and
//   get a file descriptor. it needs to be in the range of 0~31.
// + This API only affects the four kernel consoles.
// + For now fd=1 (stdout) is allowed to manage the fg_console.
//

// Get the linked pair.
    //struct tty_d *tty;
    //struct tty_d *other_tty;

    //pid_t current_process = -1;
    //struct process_d *p;
    //file *f;

    debug_print ("console_ioctl: TODO\n");
    //printk ("console_ioctl: TODO\n");

    if ( fd < 0 || fd >= OPEN_MAX ){
        return (int) (-EBADF);
    }

// Standard stream.
// Only the fd 1 is a console.
// This is the fg_console, always.
    if ( fd != 1 )
    {
        //#debug
        panic ("console_ioctl: fd != 1\n");
        //return (int) -1;
    }

    //#debug
    //printk ("console_ioctl:  fd=1\n");

//--------------------------------------------
// Setup the foreground console.
// We operate only on the fg_console!

    if ( fg_console < 0 || 
         fg_console >= CONSOLETTYS_COUNT_MAX ) 
    {
        panic ("console_ioctl: fg_console\n");
        //return (int) -1;
    }

    switch (request){

// #todo:
// There is no fflush here in ring0.
// The ring3 libc is doing this job using write.
// ??
// The ring3 flush implementation flushes the content 
// that is in a ring 3 buffer.
// The flush in ring 0 maybe can flush the backbuffer
// into the lfb.
// 1
// Flush accumulated input data.
// Flushes data received, but not read.
    case TCIFLUSH:
        debug_print ("console_ioctl: TCIFLUSH\n");
        printk ("console_ioctl: TCIFLUSH\n");
        refresh_screen();  //#test
        break;

// 2
// Flush accumulated output data.
// Flushes data written but not transmitted.
    case TCOFLUSH:
        printk ("console_ioctl: TCOFLUSH\n");
        refresh_screen();  //#test
        break;

// 3
// Flush accumulated input and output data.
// Flushes both data received but not read, and 
// data written but not transmitted.
    case TCIOFLUSH:
        printk ("console_ioctl: TCIOFLUSH\n");
        refresh_screen();  //#test
        break;

// #todo: 
// Yes, we can return data from the console tty termios. 
    // case TCGETS:
    // ...

    // ...

//
// fg color
//

    // set fg color.
    case 400:
        //#deprecated
        return 0;
        break;
    // get fg color.
    case 401:
        return (int) get_fg_color();
        break;

//
// bg color
//

    // set bg color.
    case 402:
        //#deprecated
        return 0;
        break;
    // get bg color.
    case 403:
        return (int) get_bg_color();
        break;

// Clear console.
// IN: bg color, fg color, console number.
    case 440:
        console_clear_imp( 
            (unsigned int) CONSOLE_TTYS[fg_console].bg_color,
            (unsigned int) CONSOLE_TTYS[fg_console].fg_color, 
            fg_console );
        return 0;
        break;

// Is it initialized or not?
    case 500:
        return (int) CONSOLE_TTYS[fg_console].initialized;
        break;

// Is it blocked?
    case 501:
        return (int) CONSOLE_TTYS[fg_console].is_blocked;
        break;

// What process group it belongs to?
    case 502:
        return (int) -1;
        //return (int) CONSOLE_TTYS[fg_console].gid;
        break;

// ??
// Qual eh o modo de operacao do console virtual.
// graphics, text ...
    case 503:
        return (int) CONSOLE_TTYS[fg_console].vc_mode;
        break;

// tty type
    case 504:
        return (int) CONSOLE_TTYS[fg_console].type;
        break;

// tty subtype
    case 505:
        return (int) CONSOLE_TTYS[fg_console].subtype;
        break;

// ??
// Qual terminal virtual esta usando essa tty.
    case 506:
        return (int) CONSOLE_TTYS[fg_console].virtual_terminal_pid;
        break;

// fullscreen or not?
    case 507:
        return (int) CONSOLE_TTYS[fg_console].fullscreen_flag;
        break;

// charset id
    case 508:
        return (int) CONSOLE_TTYS[fg_console].charset_id;
        break;

// charset lang id
    case 509:
        return (int) CONSOLE_TTYS[fg_console].charset_lang_id;
        break;

//
// Margins in bytes
//

    case 510:
        return (int) CONSOLE_TTYS[fg_console].cursor_left;
        break;
    case 511:
        return (int) CONSOLE_TTYS[fg_console].cursor_top;
        break;
    case 512:
        return (int) CONSOLE_TTYS[fg_console].cursor_right;
        break;
    case 513:
        return (int) CONSOLE_TTYS[fg_console].cursor_bottom;
        break;

// In pixels

    case 514:
        return (int) CONSOLE_TTYS[fg_console].cursor_width_in_pixels;
        break;
    case 515:
        return (int) CONSOLE_TTYS[fg_console].cursor_height_in_pixels;
        break;

// ...

    case 999:
        console_scroll(fg_console);
        return 0;
        break;

    //#deprecated.
    case 1000:
        return 0;
        break;

    // Cursor: x position
    case 1001:
        __local_gotoxy(
            arg,
            CONSOLE_TTYS[fg_console].cursor_y, 
            fg_console );
        return 0;
        break;

    // Cursor: y position
    case 1002:
        __local_gotoxy(
            CONSOLE_TTYS[fg_console].cursor_x,
            arg, 
            fg_console );
        return 0;
        break;

    // switching the current virtual console.
    // We have onlu 4 virtual consoles.
    case 1003:
        if ( arg >= 0 && 
             arg < CONSOLETTYS_COUNT_MAX )
        { 
            fg_console = arg;
            return 0; 
        }
        return (int) (-EINVAL);
        break;

// ...

// Goto first line at given col.
    case 1008:
        // IN: x, y, console number
        __local_gotoxy(
            arg,
            0, 
            fg_console );
        break;

// Goto last line at given col.
    case 1009:
        // IN: x, y, console number
        __local_gotoxy(
            arg,
            (CONSOLE_TTYS[fg_console].cursor_bottom -1), 
            fg_console );
        break;

    case 1010:
        // max limit
        if (arg < 70){
            console_print_indent(arg,fg_console);
        }
        break;

// Get cursor x
    case 1011:
        return (int) get_cursor_x();
        break;
// Get cursor y
    case 1012:
        return (int) get_cursor_y();
        break;


// TIOCCONS - redirecting console output
// Redirecionando o output do console para pty slave.
// Return value:
// On success, 0 is returned.  
// On error, -1 is returned, and errno is set to indicate the error.
// Errors:
// EBUSY  Output was redirected already.
// EPERM  Insufficient permission.
// see: 
// redirect
// https://man7.org/linux/man-pages/man2/TIOCCONS.2const.html

    case TIOCCONS:
        // #todo
        printk ("console_ioctl: TIOCCONS #todo\n");
        errno = 0;
        return 0; // ok
        break;

    // ...

    default:
        debug_print ("console_ioctl: [TODO] request\n");
        return (int) (-EINVAL);
        break;
    };

fail:
    return (int) -1;
}

// Implementation
int 
console_clear_imp (
    unsigned int bg_color, 
    unsigned int fg_color, 
    int console_number )
{
// Clear a console with the given colors.

    if (VideoBlock.useGui != TRUE){
        goto fail;
    }

// Parameters:
    if (console_number < 0)
        goto fail;
    if (console_number >= CONSOLETTYS_COUNT_MAX){
        goto fail;
    }

    CONSOLE_TTYS[console_number].bg_color = (unsigned int) bg_color;
    CONSOLE_TTYS[console_number].fg_color = (unsigned int) fg_color;

// Cursor
    __local_gotoxy(0,0,console_number);

// Refreshing the whole screen.
// #todo: Call the faster routine for that job.

// Clear background.
// #todo:
// This worker belongs to the display device driver,
// so we need a name that represents it.
// ex: display_bg_paint().
    // IN: color, refresh
    displayInitializeBackground(bg_color,TRUE);
    
    return 0;
fail:
    return (int) -1;
}

// It affects only the fg_console.
int console_clear(void)
{
    int ConsoleID = fg_console;
    unsigned int bg_color = COLOR_BLUE; 
    unsigned int fg_color = COLOR_WHITE;

    if (ConsoleID < 0)
        goto fail;
    if (ConsoleID >= CONSOLETTYS_COUNT_MAX){
        goto fail;
    }
    bg_color = (unsigned int) CONSOLE_TTYS[ConsoleID].bg_color; 
    fg_color = (unsigned int) CONSOLE_TTYS[ConsoleID].fg_color;
// IN: bg color, fg color, console number.
    console_clear_imp( bg_color, fg_color, ConsoleID );
    return 0;
fail:
    return (int) -1;
}

//
// $
// INITIALIZATION
//

// DDINIT_console:
// Initializar um virtual console.
// Temos uma lista limitada deles.
// Inicializa a estrutura de console virtual
// configura uma entrada no diretorio dev/
// IN: console index, bg color, fg color
void 
DDINIT_console(
    int n, 
    unsigned int bg_color, 
    unsigned int fg_color )
{
// Initialize a given console.
// #todo: We can pass more parameters.

    register int ConsoleIndex = -1;
    register int i=0;

    PROGRESS ("DDINIT_console:\n");

/*
// Char width and height
    int cWidth  = get_char_width();
    int cHeight = get_char_height();
    if ( cWidth == 0 || cHeight == 0 ){
        x_panic ("DDINIT_console: cWidth cHeight");
    }
*/

// #todo #bugbug
// determinado: 8 = char size.
// #todo: Use cWidth and cHeight.
    unsigned long screen_width_in_chars = (unsigned long) (gSavedX/8);
    unsigned long screen_height_in_chars = (unsigned long) (gSavedY/8);

// Limits
    ConsoleIndex = (int) n;
    if (ConsoleIndex < 0 || ConsoleIndex >= CONSOLETTYS_COUNT_MAX){
        debug_print ("DDINIT_console: [FAIL] ConsoleIndex\n");
        x_panic     ("DDINIT_console: [FAIL] ConsoleIndex\n");
    }

//
// Data
//

    CONSOLE_TTYS[ConsoleIndex].initialized = FALSE;

    // Todo virtual console eh uma tty. Os 4.
    CONSOLE_TTYS[ConsoleIndex].objectType = ObjectTypeTTY;
    CONSOLE_TTYS[ConsoleIndex].objectClass = ObjectClassKernelObject;
    CONSOLE_TTYS[ConsoleIndex].used = TRUE;
    CONSOLE_TTYS[ConsoleIndex].magic = (int) 1234;

// Owner tid
// Uninitialized
    CONSOLE_TTYS[ConsoleIndex].__owner_tid = (int) -1;

// tty: Console/Master.
    CONSOLE_TTYS[ConsoleIndex].type = TTY_TYPE_CONSOLE;
    CONSOLE_TTYS[ConsoleIndex].subtype = TTY_SUBTYPE_CONSOLE_MASTER;


    // No thread for now.
    //CONSOLE_TTYS[ConsoleIndex].control = NULL;

    // tty is a terminal, so the user logs on a terminal.
    // No user logged yet.
    // #test
    // We're gonna set this pointer at the end of the 
    // kernel initialization. see: kmain.c
    CONSOLE_TTYS[ConsoleIndex].user_info = NULL;

    // Security stuff.
    // Nao sei se essas estruturas estao prontas para isso nesse momento
    // ou se esses ponteiros sao nulos.
    CONSOLE_TTYS[ConsoleIndex].user_session = NULL;  // Current user session;
    CONSOLE_TTYS[ConsoleIndex].cgroup = NULL;        // Current cgroup

    // file pointer
    // this file handles this tty object
    // CONSOLE[ConsoleIndex]._fp
    
    // tty name
    //CONSOLE_TTYS[ConsoleIndex].name[?] 
    CONSOLE_TTYS[ConsoleIndex].Name_len = 0;  //initialized

    //#todo: Indice do dispositivo.
    // CONSOLE_TTYS[ConsoleIndex].device = 0;   // initialized.

    CONSOLE_TTYS[ConsoleIndex].driver = NULL;  //driver struct
    CONSOLE_TTYS[ConsoleIndex].ldisc  = NULL;  //line discipline struct

    //CONSOLE_TTYS[ConsoleIndex].termios??       //termios struct (not a pointer)

    // process group.
    //CONSOLE_TTYS[ConsoleIndex].gid = current_group;

    // ??
    // Quantos processos estao usando essa tty.
    //CONSOLE_TTYS[ConsoleIndex].pid_count=0;

    CONSOLE_TTYS[ConsoleIndex].type = 0;
    CONSOLE_TTYS[ConsoleIndex].subtype = 0;
        
    CONSOLE_TTYS[ConsoleIndex].flags = 0;
// Not blocked
    CONSOLE_TTYS[ConsoleIndex].is_blocked = FALSE;

    // process
    //CONSOLE_TTYS[ConsoleIndex].process = KernelProcess;
    
    // thread
    //CONSOLE_TTYS[ConsoleIndex].thread  = ?

    // Qual terminal virtual esta usando essa tty.
    CONSOLE_TTYS[ConsoleIndex].virtual_terminal_pid = 0;

    // Window.
    // When we are using the kgws.
    // CONSOLE_TTYS[ConsoleIndex].window = NULL;

//
// == buffers ===========================
//

// #bugbug
// No buffers fo rthe virtual consoles.
// remember: 
// The virtual console is used only in the 'stdout' of a process.
    CONSOLE_TTYS[ConsoleIndex].nobuffers = TRUE;   // No buffers.


//
// Queues
//


// raw queue
    CONSOLE_TTYS[ConsoleIndex].raw_queue.cnt = 0;
    CONSOLE_TTYS[ConsoleIndex].raw_queue.head = 0;
    CONSOLE_TTYS[ConsoleIndex].raw_queue.tail = 0;
    CONSOLE_TTYS[ConsoleIndex].raw_queue.buffer_size = TTY_BUF_SIZE;
    for(i=0; i<TTY_BUF_SIZE; i++){ CONSOLE_TTYS[ConsoleIndex].raw_queue.buf[i] = 0; }
// canonical queue
    CONSOLE_TTYS[ConsoleIndex].canonical_queue.cnt = 0;
    CONSOLE_TTYS[ConsoleIndex].canonical_queue.head = 0;
    CONSOLE_TTYS[ConsoleIndex].canonical_queue.tail = 0;
    CONSOLE_TTYS[ConsoleIndex].canonical_queue.buffer_size = TTY_BUF_SIZE;
    for(i=0; i<TTY_BUF_SIZE; i++){ CONSOLE_TTYS[ConsoleIndex].canonical_queue.buf[i] = 0; }
// output queue
    CONSOLE_TTYS[ConsoleIndex].output_queue.cnt = 0;
    CONSOLE_TTYS[ConsoleIndex].output_queue.head = 0;
    CONSOLE_TTYS[ConsoleIndex].output_queue.tail = 0;
    CONSOLE_TTYS[ConsoleIndex].output_queue.buffer_size = TTY_BUF_SIZE;
    for(i=0; i<TTY_BUF_SIZE; i++){ CONSOLE_TTYS[ConsoleIndex].output_queue.buf[i] = 0; }



// Cursor dimentions in pixel.
// #bugbug: determinado
    CONSOLE_TTYS[ConsoleIndex].cursor_width_in_pixels = 8; 
    CONSOLE_TTYS[ConsoleIndex].cursor_height_in_pixels = 8;

// Cursor position in chars.
    CONSOLE_TTYS[ConsoleIndex].cursor_x = 0;
    CONSOLE_TTYS[ConsoleIndex].cursor_y = 0;

// Cursor limits

// Full screen
    CONSOLE_TTYS[ConsoleIndex].fullscreen_flag = TRUE;
    CONSOLE_TTYS[ConsoleIndex].cursor_left = 0;
    CONSOLE_TTYS[ConsoleIndex].cursor_top  = 0;
    CONSOLE_TTYS[ConsoleIndex].cursor_right = screen_width_in_chars;
    CONSOLE_TTYS[ConsoleIndex].cursor_bottom = screen_height_in_chars;
    //#todo
    // width in pixels
    // height in pixels.

// Everyone has the same color.
// White on black.
    CONSOLE_TTYS[ConsoleIndex].bg_color = bg_color;
    CONSOLE_TTYS[ConsoleIndex].fg_color = fg_color;

/*
// Default colors.
// A different color for each console number.
    if (ConsoleIndex == 0){
        CONSOLE_TTYS[ConsoleIndex].fg_color = COLOR_WHITE; 
    }
    if (ConsoleIndex == 1){
        CONSOLE_TTYS[ConsoleIndex].fg_color = COLOR_YELLOW; 
    }
    if (ConsoleIndex == 2){
        CONSOLE_TTYS[ConsoleIndex].fg_color = COLOR_GREEN; 
    }
    if (ConsoleIndex == 3){
        CONSOLE_TTYS[ConsoleIndex].fg_color = COLOR_CYAN; 
    }
*/

    //#todo
    // Buffers !!!
    //CONSOLE_TTYS[ConsoleIndex]._rbuffer ...
    //CONSOLE_TTYS[ConsoleIndex]._cbuffer ...    

    CONSOLE_TTYS[ConsoleIndex].termios.c_lflag = ECHO;

// The kernel can print string into the display device.
    CONSOLE_TTYS[ConsoleIndex].vc_mode = 
        (int) VC_MODE_KERNEL_VERBOSER;

//
// Charset support.
//

// see:
// https://man7.org/linux/man-pages/man7/charsets.7.html

    CONSOLE_TTYS[ConsoleIndex].charset_lowercase = (void*) map_abnt2;
    CONSOLE_TTYS[ConsoleIndex].charset_uppercase = (void*) shift_abnt2;
    CONSOLE_TTYS[ConsoleIndex].charset_controlcase = (void*) ctl_abnt2;

    CONSOLE_TTYS[ConsoleIndex].charset_size = 
        (size_t) ABNT2_CHARMAP_SIZE;


// Charset name
    size_t name_size = (size_t) strlen(ABNT2_CHARMAP_NAME);
    memset(
        CONSOLE_TTYS[ConsoleIndex].charset_name,
        0,
        64);
    strncpy( 
        CONSOLE_TTYS[ConsoleIndex].charset_name,
        ABNT2_CHARMAP_NAME,
        name_size );
    CONSOLE_TTYS[ConsoleIndex].charset_name_size = 
        (size_t) name_size;

// ascii, unicode ...
    CONSOLE_TTYS[ConsoleIndex].charset_id = 0;

// language id.
    CONSOLE_TTYS[ConsoleIndex].charset_lang_id = 0;

// Font support.
    CONSOLE_TTYS[ConsoleIndex].font_address = 
        (void*) fontGetCurrentAddress();

// #bugbug
// A estrutura tem mais elementos que podem ser inicializados.
// Tivemos problemas ao tentar inicializa-los.
// ...

    switch (ConsoleIndex){
    case 0:
        //console0_tty = (struct tty_d *) &CONSOLE_TTYS[0];
        console0_tty = (struct tty_d *) CONSOLE_GET_TTY_ADDRESS(0);
        console0_tty->link = NULL;  // Not linked yet.
        console0_tty->is_linked = FALSE;
        break;
    case 1:
        //console1_tty = (struct tty_d *) &CONSOLE_TTYS[1];
        console1_tty = (struct tty_d *) CONSOLE_GET_TTY_ADDRESS(1);
        console1_tty->link = NULL;  // Not linked yet.
        console1_tty->is_linked = FALSE;
        break;
    case 2:
        //console2_tty = (struct tty_d *) &CONSOLE_TTYS[2];
        console2_tty = (struct tty_d *) CONSOLE_GET_TTY_ADDRESS(2);
        console2_tty->link = NULL;  // Not linked yet.
        console2_tty->is_linked = FALSE;
        break;
    case 3:
        //console3_tty = (struct tty_d *) &CONSOLE_TTYS[3];
        console3_tty = (struct tty_d *) CONSOLE_GET_TTY_ADDRESS(3);
        console3_tty->link = NULL;  // Not linked yet.
        console3_tty->is_linked = FALSE;
        break;
    };

    CONSOLE_TTYS[ConsoleIndex].initialized = TRUE; 
}

//
// $
// EARLY INITIALIZATION
//

// VirtualConsole_early_initialization:
// Early initialization of the consoles.
// It's gonna be reinitializad by __initialize_virtual_consoles()
// in kstdio.c
// Intialize the support for virtual consoles.
// #todo: Explain it better.
// We have 4 preallocated tty structures for virtual consoles.
int VirtualConsole_early_initialization(void)
{
// Called by zero_initialize_virtual_consoles() in zero.c

    register int i=0;

// Virtual Console:
// The kernel only have four virtual consoles.
// If this initialization fail, it will happen again

    // #debug
    PROGRESS("VirtualConsole_early_initialization: (First time)\n");;

// Console em primeiro plano.
    fg_console = CONSOLE0;
// Redirecionador.
    redirect = NULL;

// No embedded shell for now.
// No input in prompt[].
    ShellFlag = FALSE;

    __EscapeSequenceStage=0;

// Limpa o array de parâmetros usado durante o 
// tratamento de escape sequencies.
    for (i=0; i<NPAR; i++){
        par[i]=0;
    };

    unsigned int bg_colors[CONSOLETTYS_COUNT_MAX];
    unsigned int fg_colors[CONSOLETTYS_COUNT_MAX];

// -----------------------------
// c0 console: --> INIT.BIN
// Default kernel console.
    bg_colors[0] = (unsigned int) COLOR_BLUE;
    fg_colors[0] = (unsigned int) COLOR_WHITE;
// -----------------------------
// c1 console: --> GRAMLAND.BIN
    bg_colors[1] = (unsigned int) COLOR_BLUE;
    fg_colors[1] = (unsigned int) COLOR_YELLOW;
// -----------------------------
// c2 console: --> NICCTLD.BIN
// Warning console.
    bg_colors[2] = (unsigned int) COLOR_ORANGE;
    fg_colors[2] = (unsigned int) COLOR_WHITE;
// -----------------------------
// c3 console: --> NETCTLD.BIN
// Danger console.
    bg_colors[3] = (unsigned int) COLOR_RED;
    fg_colors[3] = (unsigned int) COLOR_YELLOW;


    for (i=0; i<CONSOLETTYS_COUNT_MAX; i++)
    {
        // IN: console index, bg color, fg color
        DDINIT_console(
            i,
            bg_colors[i],
            fg_colors[i] );

        // #todo
        // We need to register all the tty devices.

        // #bugbug
        // At some time in the initialization we're already
        // linking the console with the 'stdout' file.

        //tmp_tty = (struct tty_d *) &CONSOLE_TTYS[i];
        
        // Register tty device.
        //devmgr_register_device ( 
        //    (file *) __file,            // #todo: Create file.
        //    device_name_nonameconsole,  // pathname 
        //    DEVICE_CLASS_CHAR,          // class (char, block, network)
        //    DEVICE_TYPE_LEGACY,         // type (pci, legacy
        //    NULL,    // Not a pci device.
        //    NULL );  // tty device
    };

// Setup foreground console.
    jobcontrol_switch_console(0);

// Cursor for the current console.
// See: system.c
    set_up_cursor(0,0);

// #hackhack
// Esse trabalho não nos pertence, pertence ao stdio,
// mas funciona.
    stdio_terminalmode_flag = TRUE;
    stdio_verbosemode_flag = TRUE;

    return 0;
}

//
// End
//






