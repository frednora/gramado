// char.c 
// view: Char support.
// 2019 - Created by Fred Nora.

#include "../gwsint.h"

struct char_initialization_d  CharInitialization;


// ================================================


// IN: left, right, top, bottom.
void
DrawBorder( 
    unsigned long l, unsigned long r, unsigned long t, unsigned long b,
    int solid )
{
    register int i=0;
    register int j=0;
    int CharSize = 8;

    // Chage in this case.
    //if (FontInitialization.initialized == TRUE)
        //CharSize = FontInitialization.width;

    l = (l&0xFFFF);
    r = (r&0xFFFF);
    t = (t&0xFFFF);
    b = (b&0xFFFF);

    unsigned long w = r-l;
    unsigned long h = b-t;

    if (solid) {

      //PlotScreenU8(CH_DBL_TL+attr,l-1,t-1);
      //PlotScreenU8(CH_DBL_TR+attr,r+1,t-1);
      //PlotScreenU8(CH_DBL_BL+attr,l-1,b+1);
      //PlotScreenU8(CH_DBL_BR+attr,r+1,b+1);
      
      grBackbufferDrawCharTransparent( l-CharSize, t-CharSize, COLOR_BLUE, CH_DBL_TL );
      grBackbufferDrawCharTransparent( r,          t-CharSize, COLOR_BLUE, CH_DBL_TR );
      grBackbufferDrawCharTransparent( l-CharSize, b,          COLOR_BLUE, CH_DBL_BL );
      grBackbufferDrawCharTransparent( r,          b,          COLOR_BLUE, CH_DBL_BR );

      // title
      grBackbufferDrawCharTransparent( l,              t-CharSize, COLOR_BLUE, 'M' );
      grBackbufferDrawCharTransparent( l+(CharSize*1), t-CharSize, COLOR_BLUE, 'E' );
      grBackbufferDrawCharTransparent( l+(CharSize*2), t-CharSize, COLOR_BLUE, 'N' );
      grBackbufferDrawCharTransparent( l+(CharSize*3), t-CharSize, COLOR_BLUE, 'U' );

      // Controller. [x]
      // #todo: precisamos salvar a posiçao do controle.
      grBackbufferDrawCharTransparent( r-(CharSize*1), t-CharSize, COLOR_BLUE, ']' );
      grBackbufferDrawCharTransparent( r-(CharSize*2), t-CharSize, COLOR_BLUE, 'x' );
      grBackbufferDrawCharTransparent( r-(CharSize*3), t-CharSize, COLOR_BLUE, '[' );

      j=0;
      for (i=l; i < (l+(w/CharSize)); i++) 
      {
         //PlotScreenU8(CH_DBL_HORZ+attr,i,t-1);
         //PlotScreenU8(CH_DBL_HORZ+attr,i,b+1);

          grBackbufferDrawCharTransparent( i+j+(8*4), t-CharSize, COLOR_BLUE, CH_DBL_HORZ );
          grBackbufferDrawCharTransparent( i+j, b,          COLOR_BLUE, CH_DBL_HORZ );
          j+=4;
      };

      j=0;
      for (i=t; i < (t+(h/CharSize)); i++) 
      {
          //PlotScreenU8(CH_DBL_VERT+attr,l-1,i);
          //PlotScreenU8(CH_DBL_VERT+attr,r+1,i);

         grBackbufferDrawCharTransparent( l-CharSize,i+j, COLOR_BLUE, CH_DBL_VERT );
         grBackbufferDrawCharTransparent( r,         i+j, COLOR_BLUE, CH_DBL_VERT );
         j+=4;
      };

    }else{
  
      //PlotScreenU8(CH_DBL_TL+attr,l-1,t-1);
      //PlotScreenU8(CH_DBL_TR+attr,r+1,t-1);
      //PlotScreenU8(CH_DBL_BL+attr,l-1,b+1);
      //PlotScreenU8(CH_DBL_BR+attr,r+1,b+1);
      
      grBackbufferDrawCharTransparent( l-CharSize, t-CharSize, COLOR_BLUE, CH_DBL_TL );
      grBackbufferDrawCharTransparent( r,          t-CharSize, COLOR_BLUE, CH_DBL_TR );
      grBackbufferDrawCharTransparent( l-CharSize, b,          COLOR_BLUE, CH_DBL_BL );
      grBackbufferDrawCharTransparent( r,          b,          COLOR_BLUE, CH_DBL_BR );

      // title
      grBackbufferDrawCharTransparent( l,              t-CharSize, COLOR_BLUE, 'M' );
      grBackbufferDrawCharTransparent( l+(CharSize*1), t-CharSize, COLOR_BLUE, 'E' );
      grBackbufferDrawCharTransparent( l+(CharSize*2), t-CharSize, COLOR_BLUE, 'N' );
      grBackbufferDrawCharTransparent( l+(CharSize*3), t-CharSize, COLOR_BLUE, 'U' );

      // Controller. [x]
      // #todo: precisamos salvar a posiçao do controle.
      grBackbufferDrawCharTransparent( r-(CharSize*1), t-CharSize, COLOR_BLUE, ']' );
      grBackbufferDrawCharTransparent( r-(CharSize*2), t-CharSize, COLOR_BLUE, 'x' );
      grBackbufferDrawCharTransparent( r-(CharSize*3), t-CharSize, COLOR_BLUE, '[' );

      j=0;
      for (i=l; i < (l+(w/CharSize)); i++) 
      {
         //PlotScreenU8(CH_DBL_HORZ+attr,i,t-1);
         //PlotScreenU8(CH_DBL_HORZ+attr,i,b+1);

          grBackbufferDrawCharTransparent( i+j+(8*4), t-CharSize, COLOR_BLUE, CH_DBL_HORZ );
          grBackbufferDrawCharTransparent( i+j, b,          COLOR_BLUE, CH_DBL_HORZ );
          j+=4;
      };

      j=0;
      for (i=t; i < (t+(h/CharSize)); i++) 
      {
          //PlotScreenU8(CH_DBL_VERT+attr,l-1,i);
          //PlotScreenU8(CH_DBL_VERT+attr,r+1,i);

         grBackbufferDrawCharTransparent( l-CharSize,i+j, COLOR_BLUE, CH_DBL_VERT );
         grBackbufferDrawCharTransparent( r,         i+j, COLOR_BLUE, CH_DBL_VERT );
         j+=4;
      };
    };
}

int char_initialize(void)
{
// Called by gwsInitGUI() in gws.c.

    CharInitialization.initialized = FALSE;

// Char width and height.
    CharInitialization.width = DEFAULT_FONT_WIDTH;
    CharInitialization.height = DEFAULT_FONT_HEIGHT;

    if (FontInitialization.initialized != TRUE)
    {
        FontInitialization.width = DEFAULT_FONT_WIDTH;
        FontInitialization.height = DEFAULT_FONT_HEIGHT;
    }

    CharInitialization.initialized = TRUE;
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

void charSetCharWidth (int width)
{
    FontInitialization.width = (int) width;
}

void charSetCharHeight (int height)
{
    FontInitialization.height = (int) height;
}

int charGetCharWidth (void)
{
    return (int) FontInitialization.width;
}

int charGetCharHeight (void)
{
    return (int) FontInitialization.height;
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
// Called by grDrawString().

void 
grBackbufferDrawCharTransparent ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    int ch )
{
    int Char = (int) (ch & 0xFF);
    //int CharWidth;
    //int CharHeight;
    char *work_char_p;
    unsigned char bit_mask = 0x80;
//loop
    register int y2=0;
    register int x2=0;

/*
 Get the font pointer.
 #todo:
 usar variavel g8x8fontAddress.
 + Criar e usar uma estrutura para fonte.
 + Usar o ponteiro para a fonte atual que foi carregada.
 + Criar um switch para o tamanho da fonte.
   isso deveria estar na inicialização do módulo char.
 */

    if ( FontInitialization.address == 0 || 
         FontInitialization.width <= 0 || 
         FontInitialization.height <= 0 )
    {
        printf ("grBackbufferDrawCharTransparent: Initialization fail\n");
        while (1){
        };
    }

// #todo: 
// Criar essas variáveis e definições.

// O caractere sendo trabalhado.

    //int ascii = (int) (c & 0xFF);
    //if(ascii == 'M'){
    //    printf("M: %d\n",ascii);
    //}

    work_char_p = 
        (void *) FontInitialization.address + (Char * FontInitialization.height);

// Draw char
// Put pixel using the ring3 routine.
// See: libgd.c

    for ( y2=0; y2 < FontInitialization.height; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < FontInitialization.width; x2++ )
        {
            if ( ( *work_char_p & bit_mask ) )
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

        // Próxima linha das 8 linhas do caractere.
        y++; 
        work_char_p++; 
    };
}

// ----
void 
grBackbufferDrawCharTransparent2 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    int ch,
    char *stock_address )
{
    int Char = (int) (ch & 0xFF);
    //int CharWidth;
    //int CharHeight;
    char *work_char_p;
    unsigned char bit_mask = 0x80;
// Where the font is.
    char *base_address;
    base_address = stock_address;
//loop
    register int y2=0;
    register int x2=0;

// Invalid base address
    if ((void *) base_address == NULL)
    {
        //#debug
        //printf("grBackbufferDrawCharTransparent2: base_address\n");
        goto fail;
    }

// Invalid char dimensions.
    if ( FontInitialization.width <= 0 || FontInitialization.height <= 0 )
    {
        //#debug
        //printf("grBackbufferDrawCharTransparent2: base_address\n");
        goto fail;
    }

// The pointer for the working char.
    work_char_p = 
        (void *) base_address + (Char * FontInitialization.height);

// Draw char
// Put pixel using the ring3 routine.
// See: libgd.c

    for ( y2=0; y2 < FontInitialization.height; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < FontInitialization.width; x2++ )
        {
            if ( *work_char_p & bit_mask )
            {
                // IN: color, x, y, rop
                libdisp_backbuffer_putpixel ( 
                    (unsigned int) color, 
                    (x + x2), 
                    y,
                    (unsigned long) 0 );  
            }

            // Rotate bitmask.
            bit_mask = (bit_mask >> 1);
        };

        // Próxima linha das 8 linhas do caractere.
        y++; 
        work_char_p++; 
    };

//done:
    return;
fail:
    printf("grBackbufferDrawCharTransparent2: Fail\n");
    while (1){
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
 
    if ( FontInitialization.address == 0 ||  
         FontInitialization.width <= 0 || 
         FontInitialization.height <= 0 )
    {
        printf ("grBackbufferDrawChar: initialization fail\n");
        while(1){}
    }

// Tentando pintar um espaço em branco.
// Nas rotinas da biblioteca gráfica, quando encontram
//um espaço(32), nem manda para cá, apenas incrementam o cursor.

// O caractere sendo trabalhado.
// Offset da tabela de chars de altura 8 na ROM.

    work_char = 
        (void *) FontInitialization.address + (c * FontInitialization.height);

// Draw:
// Draw a char using a ring3 routine.
// #todo
// Some flag for modification here?
// Put pixel.
// A cor varia de acordo com a mascara de bit.

    for ( y2=0; y2 < FontInitialization.height; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < FontInitialization.width; x2++ )
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

