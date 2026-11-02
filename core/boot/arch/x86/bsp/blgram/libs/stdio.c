// stdio.c
// i/o routines.
// graphics routines.
// vsync
// 2015 - Created by Fred Nora.

#include "../bl.h"  


extern unsigned long SavedBootBlock;
extern unsigned long SavedLFB;
extern unsigned long SavedX;
extern unsigned long SavedY;
extern unsigned long SavedBPP; 
// from assembly i guess.
extern void my_buffer_load_bitmap_16x16();

// == Private functions: Prototypes ======================
static void _outbyte(int c);
static void outbyte(int c);
// ===================================

// _outbyte: 
// Coloca um char na tela. 
// Com opces de modo de video.
// Called by outbyte().
static void _outbyte(int c)
{
    unsigned long i=0;
    unsigned long x=0;
    unsigned long y=0;
    char *vm = (char *) 0x000B8000;  
// Char, attribute.
    char ch = (char) c;
    char ch_atributo = (char) g_char_attrib;

// We are in graphics mode. 
    if (VideoBlock.useGui == TRUE)
    {
        //vsync();

        // #todo: 
        // Listar aqui os modos VESA?
        switch (VideoBlock.vesaMode){
        case 1:
            my_buffer_char_blt( 
                (8*g_cursor_x), 
                (8*g_cursor_y), 
                COLOR_WHITE, 
                c );
            break;
        // ...
        // Modo grafico vesa 640x480 24bpp, 8 pixel por caractere.
        default:
            my_buffer_char_blt( 
                (8*g_cursor_x), 
                (8*g_cursor_y), 
                COLOR_WHITE, 
                c );
            break;
        };
        return;
    }

// We are not in graphics mode.
    if (VideoBlock.useGui == FALSE)
    {
        // Calcula o valor do deslocamento para text mode 80x25.
        y = (unsigned long) (g_cursor_y *80 *2);
        x = (unsigned long) (g_cursor_x *2);
        i = (unsigned long) (y + x);
        
        // Envia o caractere.
        vm[i+0] = ch;             //char.
        vm[i+1] = ch_atributo;    //atributo (foreground,background).
    }
}

// outbyte:
// Trata o caractere antes de por na memoria de video.
static void outbyte(int c)
{
    register int Ch=c;      // Copy
    static char prev = 0;

// Sendo menor que espaço, não pode ser 'tab,return,back...
    if ( Ch <  ' '  && 
         Ch != '\r' && 
         Ch != '\n' && 
         Ch != '\t' && 
         Ch != '\b' )
    {
        return;
    }

// Sendo maior que 'espaco'. 

// Volta ao inicio da linha.
    if ( Ch == '\r' )
    {
        g_cursor_x = 0;
        prev = Ch;
        return;    
    }
 
// Vai pra proxima linha e volta ao inicio da linha.    
    if ( Ch == '\n' && prev != '\r' )
    {
        g_cursor_y++;
        g_cursor_x = 0; 
        prev = Ch;
        return;
    }

    if ( Ch == '\n' && prev == '\r' )
    {
        g_cursor_y++;    //proxima linha
        prev = Ch;
        return; 
    }

//tab
    if ( Ch == '\t' )
    {
        g_cursor_x += (4);    //criar a var -> 'g_tab_size'
        prev = Ch;
        return;
    }

//space 
    if ( Ch == ' ' )
    {
        g_cursor_x++; 
        prev = Ch;
        return;         
    }

//delete 
    if ( Ch == 8 )
    {
        g_cursor_x--; 
        prev = Ch;
        return; 
    }

// Filtra as dimensções da janela onde esta pintando.

// limite horizontal
// 80 = g_coluna_max 
    if (g_cursor_x > 80)  {
        g_cursor_x = 0;
        g_cursor_y++;
    }else{
        g_cursor_x++;                             
    };

// Limite vertical. 
// (@todo: Testando limite maior, ja que estamos em modo grafico.)
//25 = g_linha_max (50*8 pixels) 
    if (g_cursor_y > 74){
        scroll();
        g_cursor_y = 74;  // Osso pode ir para dentro da funcao scroll().
    }

// Imprime os caracteres normais.
    _outbyte(Ch);

// Atualisa o prev.
    prev = Ch;
}

// scroll:
// #bugbug: Is it for text mode? 
void scroll(void)
{
//loop
    register unsigned short i=0;
    register unsigned short j=0;
// inicio da tela
    unsigned short *p1 = (unsigned short *) ScreenStart;
// inicio da segunda linha
    unsigned short *p2 = (unsigned short *) (ScreenStart + 2 * SCREEN_WIDTH) ;

    // 24 vezes
    for (i=0; i < ROWS - 1; i++)
    {
        // 80 vezes
        for (j=0; j < SCREEN_WIDTH; j++)
        {
            *p1++ = *p2++;
        };
    };

    // 80 vezes
    for (i=0; i < SCREEN_WIDTH; i++)
    {
        *p1++ = 0x07*256 + ' '; 
        //*p1++ = REVERSE_ATTRIB*256 + ' ';
    };
}

// bl_clear: 
// Limpa a tela em text mode. 
// #bugbug
// Nao usamos mas esse modo de video. 
int bl_clear (int color)
{
    register unsigned int i=0;
    char *vidmemz = (char *) 0x000B8000;

    while (i < (80*25*2)) 
    { 
        vidmemz[i] = 219; 
        i++;
        vidmemz[i] = color;
        i++;
    };

    g_cursor_x = 0;
    g_cursor_y = 0;

    return 0; 
}

// kprintf:
// Imprime uma string em uma determinada linha. 
// @todo: Mudar para bl_print(...) 
// #bugbug
// Nao usamos mas esse modo de video. 

// #deprecated

int kprintf( char *message, unsigned int line, int color )
{
    //loop
    register unsigned int i = 0;
    char *vidmemp = (char *) 0x000B8000; 

    //if ( (void*) message == NULL ){ return -1; };
    //if ( *message == 0 )          { return -1; };

    i = (unsigned int) (line*80*2); 

    while (*message != 0) 
    { 
        if(*message == '\n')
        { 
            line++; 
            i = (unsigned int) (line*80*2); 
            *message++; 
        }
        else
        { 
            vidmemp[i] = *message; 
            *message++; 
            i++; 
            
            vidmemp[i] = color; 
            i++; 
        };
        // Nothing
    }; 

    return 0; 
} 

/*
 * prints:
 *     Print string.
 *     Parte da funcao printf(). 
 */

static int prints ( 
    char **out, 
    const char *string, 
    int width, 
    int pad )
{
    register int pc = 0, padchar = ' ';

    if (width > 0) 
    {
	    register int len = 0;
		register const char *ptr;
		
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
    }

    if ( !(pad & PAD_RIGHT) ) 
    {
		for ( ; width > 0; --width)
		{
		    printchar (out, padchar);
			++pc;
		};
    }

	for ( ; *string ; ++string )
	{
		printchar (out, *string);
		++pc;
	};

	for ( ; width > 0; --width )
	{
		printchar (out, padchar);
		++pc;
	};
    // Nothing
done:
    return pc;
}

/*
 * printi:
 *     Parte da funcao printf()
 */
static int printi ( 
   char **out, 
   int i, 
   int b, 
   int sg, 
   int width, 
   int pad, 
   int letbase )
{
    char print_buf[PRINT_BUF_LEN];
    register char *s;
    register int t, neg = 0, pc = 0;    
    register unsigned int u = i;

    if (i == 0)
    {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints (out, print_buf, width, pad);
    }

    if ( sg && 
         b == 10 && 
         i < 0 )
    {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    while (u) 
    {
        t = u % b;

        if ( t >= 10 )
            t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    };

    if (neg) 
    {
        if ( width && (pad & PAD_ZERO) ){
            printchar (out, '-');
            ++pc;
            --width;
        }else{
            *--s = '-';
        };
    }

done:
    return pc + prints(out, s, width, pad);
}

/*
 * print:
 *     Parte da funcao printf()
 */
static int print (char **out, int *varg)
{
    register int width, pad;
    register int pc = 0;
    register char *format = (char *)(*varg++);
    char scr[2];

    for (; *format != 0; ++format) 
    {
		if (*format == '%') 
		{
			++format;
			width = pad = 0;

            if (*format == '\0') { break;    }
            if (*format == '%' ) { goto out; }

			if (*format == '-')
			{
				++format;
				pad = PAD_RIGHT;
			}

			while (*format == '0')
			{
				++format;
				pad |= PAD_ZERO;
			};
			
			for ( ; *format >= '0' && *format <= '9'; ++format)
			{
				width *= 10;
				width += *format - '0';
			};
			
			if ( *format == 's' )
			{
				register char *s = *((char **)varg++);
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			
			if ( *format == 'd' ){
				pc += printi (out, *varg++, 10, 1, width, pad, 'a');
				continue;
			}
			
			if ( *format == 'x' ){
				pc += printi (out, *varg++, 16, 0, width, pad, 'a');
				continue;
			}
			
			if ( *format == 'X' ){
				pc += printi (out, *varg++, 16, 0, width, pad, 'A');
				continue;
			}
			
			if ( *format == 'u' ){
				pc += printi (out, *varg++, 10, 0, width, pad, 'a');
				continue;
			}
			
			if ( *format == 'c' ) 
			{
				/* char are converted to int then pushed on the stack */
				scr[0] = *varg++;
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}else{

            out:
                printchar (out, *format);
                 ++pc;
        };
    };

    if (out){ 
        **out = '\0'; 
    }

    return pc;
}

// printf:
// Função printf() da lib C.
// Assuming sizeof(void *) == sizeof(int). 
// #todo
// Change the name to blprintf()
int printf( const char *format, ... )
{
    register int *varg = (int *)(&format);
    // sincronisa.  
    // vsync();
    return print (0, varg);
}

int sprintf(char *out, const char *format, ... )
{
    register int *varg = (int *)(&format);
    // vsync();
    return print (&out, varg);
}

// printchar:
// Print a char.
// extern int putchar(int c);
static void printchar(char **str, int c)
{
    //if (c<0)
        //return;

    if (str){
        **str = c;
        ++(*str);
    } else { 
        putchar(c);
    };
}

// putchar:
// Put a char.
int putchar(int ch)
{
    outbyte(ch);
    return (int) ch; 
}

// Testing printf.
// #deprecated.
int printf_main(void)
{
    return -1;
}

// input:
// Coloca os caracteres digitados em uma string. 
unsigned long input(unsigned long ch)
{
    // Converte.
    char c = (char) ch;    

// Limits
    if (prompt_pos > 250){
        printf("input: Buffer limits\n");
        return (unsigned long) 0; 
    }

// Trata caractere digitado.

	switch (c)
	{
	    //enter
		case KEY_RETURN:
	    //case 0x1C:
		    prompt[prompt_pos] = (char )'\0';
            goto input_done;
		    break;

	    //backspace
		case 0x0E:
            if( prompt_pos <= 0 ){ 
			    prompt[prompt_pos] = (char ) '\0';
				break; 
			};
		    
			//Apaga o anterior no buffer.
			prompt_pos--;
			prompt[prompt_pos] = (char ) '\0';
			
			//Apaga o atual
			printf ("%c",' ');
			//Apaga o anterior
			g_cursor_x--;
			g_cursor_x--;
			printf ("%c",'_');
			g_cursor_x--;
			break;
			
		default:
		    prompt[prompt_pos] = c;
		    prompt_pos++;          //incrementa fila.
			putchar (c);
			printf ("%c",'_');
			g_cursor_x--;
			break;
	};

input_more:
    return 0;
input_done:
    return KEY_RETURN;
}

// my_buffer_horizontal_line:
// Pinta uma linha horinzontal no Back Buffer. 
void 
my_buffer_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned long color )
{

// #todo
// This is graphics routine.
// Move this routine to another file.

    while (x1 < x2)
    {
        my_buffer_put_pixel( color, x1, y, 0 );
        x1++;  
    };
}

/*
 * my_buffer_put_pixel: 
 *     Coloca um pixel o buffer 1.
 * a = cor
 * b = x
 * c = y
 * d = null
 */

extern void gui_buffer_putpixel();

void 
my_buffer_put_pixel ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx )
{

// #todo
// This is graphics routine.
// Move this routine to another file.

    //asm volatile(" \n "
    //    : // no inputs
    //    : "a"(ax), "b"(bx), "c"(cx), "d"(dx) );

//#Warning
//suspenso, vamos tentar nao usar o assembly
    //gui_buffer_putpixel(); 
    //return;

    //SOFTWARELIB_BACKBUFFER EQU (0x1000000 - 0x800000)
    unsigned char *where = (unsigned char *) (0x1000000 - 0x800000);  //0xC0800000;
    unsigned long color = (unsigned long) ax;
    char b, g, r, a;

// Color.
    b = (color & 0xFF);
    g = (color & 0xFF00)   >> 8;
    r = (color & 0xFF0000) >> 16;
    a = (color >> 24) + 1;

// x and y.
    int x = (int) bx;
    int y = (int) cx;
// = 3; 
//24bpp
    int bytes_count=0;

    switch (SavedBPP){
    case 32:
        bytes_count = 4;
        break;
    case 24:
        bytes_count = 3;
        break;
    // ...
    // #bugbug
    default:
        bytes_count = 3;
        break;
    };

// #importante
// Pegamos a largura do dispositivo.
    int width = (int) SavedX; 
// Offset.
    int offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );

// Plot the pixel.
    where[offset]    = b;
    where[offset +1] = g;
    where[offset +2] = r;
    if (SavedBPP == 32){ where[offset +3] = a; }
}

// my_buffer_char_blt:
// Draw a char using ROM BIOS's font.
void 
my_buffer_char_blt ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long color, 
    unsigned long c )
{
// #todo
// This is graphics routine.
// Move this routine to another file.
 
    int x2=0; 
    int y2=0;
    unsigned char bit_mask = 0x80;
// Font address and char address.
    //char height = 8
    char *work_char = (char*) 0x000FFA6E + (c * 8);

    for ( y2=0; y2<8; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2<8; x2++ )
        {
            if ( (*work_char & bit_mask) ){
                my_buffer_put_pixel( color, x + x2, y, 0);
            } 
            bit_mask = (bit_mask >> 1);
        };

        y++;          // Proxima linha.
        work_char++;  // Incrementa 8 bits.
    };
}

/*
 * vsync: 
 *     Sincroniza a pintura com o retraco vertical.
 *     OBS: Talvez deva usar cli e sti 
 *     //#todo: Move this to another place, maybe fb device support.
 */
void vsync()
{

// #todo
// This is graphics routine. (low level)
// Move this routine to another file.

    // Wait until any previous retrace has ended.
    do {
    // nothing.
    }while ( gui_inb(0x3DA) & 8 );

    // Wait until a new retrace has just begun.
    do {
    //nothing.
    } while( !(gui_inb(0x3DA) & 8) );
}

// gui_inb:
// Pega um byte na porta. 
char gui_inb (int port)
{
// #todo: Move this routine to another file.

    char value=0;

    value = in8(port);

    asm (" nop \n");
    asm (" nop \n");
    asm (" nop \n");
    asm (" nop \n"); 

    return value;
}

/*
void refresh_screen2();
void refresh_screen2()
{
    unsigned char *backbuffer  = (unsigned char *) (0x1000000 - 0x800000); 
    unsigned char *frontbuffer = (unsigned char *) g_lbf_pa; 
    int i=0;
    for(i=0; i<(800*600*6); i++)
        frontbuffer[i] = backbuffer[i];
}
*/

// color black
// 800x600x32
void clear_backbuffer(void)
{

// #todo
// This is graphics routine.
// Move this routine to another file.

    register int i=0;
    // Backbuffer address.
    // Is this a good address ?
    // almos 16MB mark?
    // #todo
    // We can use unsigned longs.
    unsigned char *backbuffer = 
        (unsigned char *) (0x1000000 - 0x800000); 

// #bugbug
// Not good for smaller resolutions.

    for (i=0; i<(800*600*4); i++)
    {
        backbuffer[i] = 0;
    };
}

//#bugbug
//@todo: Rever isso.
//    g_cursor_x = 0;
//    g_cursor_y = 0;
unsigned long get_cursor_x ()
{  
// #todo
// This is graphics routine.
// Move this routine to another file.

    //unsigned long *int_args  = (unsigned long *) 0x0090000;
    //return   int_args[4];
    
    return g_cursor_x;
}

//#bugbug
//@todo: Rever isso.
//    g_cursor_x = 0;
//    g_cursor_y = 0;
unsigned long get_cursor_y ()
{ 
// #todo
// This is graphics routine.
// Move this routine to another file.

    //unsigned long *int_args  = (unsigned long *) 0x0090000;
    //return  int_args[8]; 
    
    return g_cursor_y;
}


/*
 * carrega_bitmap_16x16:
 *     Carrega um bitmap de 16x16.
 *     P�e bitmap no buffer.
 * @todo: Mudar o nome, colocar em outro arquivo.
 * a - endere�o da imagem. 
 * b - x
 * c - y 
 * d - null
 */
// #todo
// This is graphics routine.
// Move this routine to another file.
void 
carrega_bitmap_16x16 ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx )
{
    asm volatile (" \n "
        : // no inputs
        : "a"(ax), "b"(bx), "c"(cx), "d"(dx) );

// Coloca no buffer. 
// Não precisa esperar o retraço vertical.
    my_buffer_load_bitmap_16x16(); 
}

//
// End
//

