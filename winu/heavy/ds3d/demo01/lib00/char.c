/*
 * File: char.c 
 *     gws - Char support.
 * History:
 *     2019 - Created by Fred Nora.
 */

#include "../gram3d.h"

// Draw char support
int gcharWidth=0;
int gcharHeight=0;

int gwssrv_init_char(void)
{
    //char
    gcharWidth  = 8;   //gde_get_system_metrics (7);
    gcharHeight = 8;   //gde_get_system_metrics (8);
    //...
    return 0;
}

// Constrói um caractere transparente 8x8 no buffer.
void 
charBackbufferCharBlt ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long color, 
    unsigned long c )
{
    grBackbufferDrawCharTransparent ( x, y, color, c );
}

void charSetCharWidth ( int width )
{
    gcharWidth = (int) width;
}

void charSetCharHeight ( int height )
{
    gcharHeight = (int) height;
}

int charGetCharWidth (void)
{
    return (int) gcharWidth;
}

int charGetCharHeight (void)
{
    return (int) gcharHeight;
}

/*
 * charBackbufferDrawcharTransparent:
 *     Desenha um caractere sem alterar o pano de fundo.
 *     >> no backbuffer.
 */
// #bugbug
// Nessa função estamos usando globais.
// Talvez devamos pegá-las antes e não 
// referenciá-las diretamente.

void 
grBackbufferDrawCharTransparent ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    unsigned long c )
{
    //loop
    register int y2=0;
    register int x2=0;
    char *work_char;
    unsigned char bit_mask = 0x80;
    //int CharWidth;
    //int CharHeight;

/*
 Get the font pointer.
 #todo:
 usar variavel g8x8fontAddress.
 + Criar e usar uma estrutura para fonte.
 + Usar o ponteiro para a fonte atual que foi carregada.
 + Criar um switch para o tamanho da fonte.
   isso deveria estar na inicialização do módulo char.
 */

    if ( gws_currentfont_address == 0 || 
         gcharWidth <= 0 || 
         gcharHeight <= 0 )
    {
        //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //ROM bios.
        //gcharWidth = DEFAULT_CHAR_WIDTH;               //8.
        //gcharHeight = DEFAULT_CHAR_HEIGHT;             //8.

        // #debug
        // Estamos parando para testes.

        printf ("grBackbufferDrawCharTransparent : Initialization fail\n");
        while(1){}
    }

    // #todo: 
    // Criar essas variáveis e definições.

    switch (gfontSize){

		//case FONT8X8:
	        //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //getFontAddress(...)
		    //gcharWidth = 8;
		    //gcharHeight = 8;
		    //set_char_width(8);
			//set_char_height(8);
			//break;
		
		//case FONT8X16:
	        //gws_currentfont_address = (unsigned long) BIOSFONT8X16;    //getFontAddress(...)
		    //gcharWidth = 8;
		    //gcharHeight = 16;
		    //set_char_width(8);
			//set_char_height(16);			
		    //break;
		 
		//#todo: 
		//Criar opções
		//...
		
		// #importante:
		// #BUGBUG
		// Se não temos um tamanho selecionado então teremos 
		// que usar o tamanho padrão.
		
        default:
		    //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //ROM bios.
		    
			//set_char_width(8);
			//set_char_height(8);	
            //gfontSize = FONT8X8;  //#todo: fução para configurar isso.

            break;
    };

// O caractere sendo trabalhado.

    work_char = 
        (void *) gws_currentfont_address + (c * gcharHeight);


// Draw char
// Put pixel using the ring3 routine.
// See: bitblt.c

    for ( y2=0; y2 < gcharHeight; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < gcharWidth; x2++ )
        {
            if ( ( *work_char & bit_mask ) )
            {
                // IN: color, x, y,rop
                libdisp_backbuffer_putpixel ( 
                    (unsigned int) color, 
                    (x + x2), 
                    y,
                    (unsigned long) 0 );  
            }

            // Rotate bitmask.
            bit_mask = (bit_mask >> 1);
        };

        // Próxima linha da 8 linhas do caractere.
        y++; 
        work_char++; 
    };
}

/*
 * charBackbufferDrawchar:
 *     Constrói um caractere no backbuffer.
 *     Desenha um caractere e pinta o pano de fundo.
 */ 

void 
grBackbufferDrawChar ( 
    unsigned long x, 
    unsigned long y,  
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor )
{
    register int y2=0;
    register int x2=0;
    char *work_char; 
    unsigned char bit_mask = 0x80;

/*
 Get the font pointer.
 #todo:
 usar variavel g8x8fontAddress.	 
 + Criar e usar uma estrutura para fonte.
 + Usar o ponteiro para a fonte atual que foi carregada.
 + Criar um switch para o tamanho da fonte.
   isso deveria estar na inicialização do módulo char.
 */
 
    if ( gws_currentfont_address == 0 ||  
         gcharWidth <= 0 || 
         gcharHeight <= 0 )
    {
        //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //ROM bios.
        //gcharWidth = DEFAULT_CHAR_WIDTH;               //8.
        //gcharHeight = DEFAULT_CHAR_HEIGHT;             //8.

        //#debug
        //Estamos parando só para testes.

        printf ("gws_draw_char: initialization fail\n");
        while(1){}
    }

// #todo: 
// Criar essas variáveis e definições.

    switch (gfontSize){

		//case FONT8X8:
	        //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //getFontAddress(...)
		    //gcharWidth = 8;
		    //gcharHeight = 8;
		    //set_char_width(8);
			//set_char_height(8);
			//break;
		
		//case FONT8X16:
	        //gws_currentfont_address = (unsigned long) BIOSFONT8X16;    //getFontAddress(...)
		    //gcharWidth = 8;
		    //gcharHeight = 16;
		    //set_char_width(8);
			//set_char_height(16);
		    //break;
		 
		//#todo: 
		//Criar opções
		//...
		
		// #importante:
		// #BUGBUG
		// Se não temos um tamanho selecionado então teremos 
		// que usar o tamanho padrão.

        default:
		    //gws_currentfont_address = (unsigned long) BIOSFONT8X8;    //ROM bios.
		    //set_char_width(8);
			//set_char_height(8);	
            //gfontSize = FONT8X8;  //#todo: fução para configurar isso.			
            break;
    };

// Tentando pintar um espaço em branco.
// Nas rotinas da biblioteca gráfica, quando encontram
//um espaço(32), nem manda para cá, apenas incrementam o cursor.

// O caractere sendo trabalhado.
// Offset da tabela de chars de altura 8 na ROM.

    work_char = 
        (void *) gws_currentfont_address + (c * gcharHeight);

// Draw:
// Draw a char using a ring3 routine.
// #todo
// Some flag for modification here?
// Put pixel.
// A cor varia de acordo com a mascara de bit.

    for ( y2=0; y2 < gcharHeight; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < gcharWidth; x2++ )
        {
            // IN: color, x, y, rop
            libdisp_backbuffer_putpixel ( 
                *work_char & bit_mask ? fgcolor: bgcolor, 
                (x + x2), 
                y,
                (unsigned long) 0 );

            bit_mask = (bit_mask >> 1); 
        };

        // Próxima linha da (y) linhas do caractere.
        y++; 
        work_char++; 
    };
}

//
// End
//

