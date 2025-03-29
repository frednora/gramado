
// bmp.c
// Very basic support for BMP files.
// We get an address calling the kernel given an index
// we're we find a pre-loaded image.
// 2015 - Created by Fred Nora.

#include "gram3d.h"

// Flag que avisa que deve haver alguma mudança nas cores. 
int bmp_change_color_flag = FALSE;
// Salva-se aqui a cor que usaremos para substituir outra.
unsigned int bmp_substitute_color=0; 
// Cor selecionada para ser substituída ou ignorada. 
unsigned int bmp_selected_color=0;

// 4bpp support.
static int nibble_count_16colors = 0;

// Usados temporariamente por cada uma das exibições.
struct gws_bmp_header_d     __Local_bh;
struct gws_bmp_infoheader_d __Local_bi;

// Pointers
//char *gwssrv_bmp_cursor_pointer;
//char *gwssrv_bmp_mouse_pointer;
// ...

//
// Private
//

static void *__get_system_icon(int n);


/*
 * bmpDisplayBMP:
 * (Decode), Draw a pre-loaded image into the backbuffer.
 * IN:
 * address = Address for an undecoded BMP file.
 * x       = Target x position. 
 * y       = Target y position.
 */

int 
bmpDisplayBMP ( 
    char *address, 
    unsigned long x, 
    unsigned long y )
{

// Endereço base do BMP que foi carregado na memoria
    unsigned char *bmp = (unsigned char *) address;

    if ((void*)bmp==NULL){
        return -1;
    }

    //struct gws_bmp_header_d      *bh;
    //struct gws_bmp_infoheader_d  *bi;

//#todo: inicializar.
    int i, j, base, offset;
    unsigned int left, top, bottom;

// Zoom support.
// It is working.
// But we need to work in the 'position' thing
// and accept some function parameters for that effect.

    int useZoom = FALSE;
    int ZoomFactor = 2;     //8;
    int iwZoom=0;
    int ihZoom=0;

    unsigned int X=0;
    unsigned int Y=0;
    unsigned int Width=0;
    unsigned int Height=0;
    
    unsigned int xLimit=0;
    unsigned int yLimit=0;

    unsigned short sig=0;

    unsigned int color=0;
    unsigned int color2=0;
    unsigned long pal_address=0;

// Variável para salvar rgba.
    unsigned char *c  = (unsigned char *) &color;
    unsigned char *c2 = (unsigned char *) &color2;

// Limits

// #todo: 
// #bugbug: Isso é provisório.
// Get system metrics.

    xLimit = 800;
    yLimit = 600;

// #debug:
    //gwssrv_debug_print ("bmpDisplayBMP:\n");

// Limits.
    if ( x > xLimit || 
         y > yLimit )
    {
        gwssrv_debug_print ("bmpDisplayBMP: Limits \n");
        printf             ("bmpDisplayBMP: Limits \n");
        goto fail;
    }

// #todo:
// Testar validade do endereço.
    if (address == 0)
    {
        gwssrv_debug_print ("bmpDisplayBMP: address\n");
        printf             ("bmpDisplayBMP: address\n");
        goto fail;
    }

//
// struct for Info header
//

// #todo: 
// Podemos usar malloc?
// Buffer para as estruturas.
// #todo: Na verdade a estrutura ja está dentro do arquivo.
// não precisamos alocar toda vez que queremos exibir.
// Mas podemos fazer uma cópia local num buffer estatico.

/*
    bh = (struct gws_bmp_header_d *) malloc( sizeof(struct gws_bmp_header_d) );
    if ( (void *) bh == NULL )
    {
        gwssrv_debug_print ("bmpDisplayBMP: bh \n");
        printf             ("bmpDisplayBMP: bh \n");
        goto fail;
    }
*/

// See: https://en.wikipedia.org/wiki/BMP_file_format

// Signature
// 2 bytes
    sig = *(unsigned short *) &bmp[0];
    __Local_bh.bmpType = (unsigned short) sig;
    //printf ("sig={%x}\n",sig);

    if ( bmp[0] != 'B' || bmp[1] != 'M' )
    {
        gwssrv_debug_print ("bmpDisplayBMP: [FAIL] signature \n");
        printf             ("bmpDisplayBMP: [FAIL] signature %c %c\n", 
            bmp[0], bmp[1]);
        goto fail;
    }

// Size.
// 4 bytes	The size of the BMP file in bytes

    unsigned int Size = *(unsigned int *) &bmp[2];
    __Local_bh.bmpSize = (unsigned int) Size;
    //printf ("Size={%x}\n",Size);


// Representa o inicio da paleta
// ou o inicio da area de dados se for 24/32bpp.
// 36 00 00 00 - 54 bytes (14+40)
    unsigned int OffsetForBase = *(unsigned int *) &bmp[10];
    //printf("OffsetForBase %x\n",OffsetForBase);
    //while(1){}

// #hackhack
// Isso porque, para imagens antigas, feitas no Paint do Windows 7,
// esta aparecento o valor 36 04 00 00, ao invez de 36 04 00 00.
    OffsetForBase = (unsigned int) (OffsetForBase & 0xFF);

// ----------------------------


// Palette
    unsigned int *palette = 
        (unsigned int *) (address + OffsetForBase);
    unsigned char *palette_index = 
        (unsigned char *) &pal_address;



//------------------------------

//
// struct for Info header
//

// Windows bmp.

/*
    bi = (struct gws_bmp_infoheader_d *) malloc ( sizeof(struct gws_bmp_infoheader_d) );
    if ( (void *) bi == NULL )
    {
        gwssrv_debug_print ("bmpDisplayBMP: bi fail \n");
        printf             ("bmpDisplayBMP: bi fail \n");
        goto fail;
    }
*/

// The size of this header.
// 4 bytes
    __Local_bi.bmpSize = *( unsigned int * ) &bmp[14];
    //printf ("HeaderSize={%x}\n",bi->bmpSize);

// X and Y.
// #todo: tipagem (type) xxx;
    X = (x & 0xFFFF);
    Y = (y & 0xFFFF);

// Width and height.
// #todo: tipagem (type) xxx;
// Save it into a local structure.
    Width  = *( unsigned int * ) &bmp[18];
    Height = *( unsigned int * ) &bmp[22];
    __Local_bi.bmpWidth  = (unsigned int) (Width  & 0xFFFF);
    __Local_bi.bmpHeight = (unsigned int) (Height & 0xFFFF);
    //printf ("w=%d h=%d\n",Width,Height);

// Number of bits per pixel.
// 1, 4, 8, 16, 24 and 32.
// #todo: tipagem (type) xxx;
    __Local_bi.bmpBitCount = *( unsigned short * ) &bmp[28];
    //printf ("Count={%d}\n", bi->bmpBitCount );
    
//#limits
    //if (bi->bmpBitCount != 24){
    //    printf("bmpDisplayBMP: Count fail\n");
    //    goto fail;
    // }

//
// Compression
//

// 0 = No compression.

    /*
    if ( bi->bmpCompression != 0 ){
        gwssrv_debug_print ("bmpDisplayBMP: bmpCompression fail \n");
        printf             ("bmpDisplayBMP: bmpCompression fail \n");
        goto fail;
    }
    */

//
// Draw
//

    //#debug
    //gwssrv_debug_print ("bmpDisplayBMP: Draw!\n");
    //printf             ("bmpDisplayBMP: Draw!\n");

// Top, Left, Bottom.
// #todo: tipagem.

    left = (x & 0xFFFF);
    top  = (y & 0xFFFF);
    bottom = ( top + __Local_bi.bmpHeight );


//
// Data area
//

// Bits Per Pixel
// The start of the data area depends on the bpp value.
// Estamos falando do tamanho da paleta?
// Wrong?
//1     -  1 bpp (Mono)
//4     -  4 bpp (Indexed)
//8     -  8 bpp (Indexed) bbgggrrr
//16565 - 16 bpp (5:6:5, RGB Hi color)
//16    - 16 bpp (5:5:5:1, RGB Hi color)
//160   - 16 bpp (5:5:5:1, RGBA Hi color)
//24    - 24 bpp (True color)
//32    - 32 bpp (True color, RGB)
//320   - 32 bpp (True color, RGBA)

    int BitsPerPixel = (int) (__Local_bi.bmpBitCount & 0xFFFF);

    switch (BitsPerPixel){

    // 4 bytes pra cada cor, 2 cores. Total 8 bytes.
    // 8/4 = 2 indices
    case 1:
        //base = (0x36 + 8);
        base = (OffsetForBase + 8);
        break;
    // 4 bytes pra cada cor, 16 cores. Total 64 bytes.
    // 64/4 = 16 indices
    case 4:  
        //base = (0x36 + 64); 
        base = (OffsetForBase + 64);
        break; 
    // 4 bytes pra cada cor, 256 cores. Total 1024 bytes.
    // 1024/4 = 256 indices.
    case 8:
        //printf("8\n");
        // 36 00 00 00 - 54 bytes (14+40)
        // base da area de dados = (base da paleta + tamanho da paleta).
        //base = (0x36 + 1024); 
        base = (OffsetForBase + 1024);
        break;
    // 4 bytes pra cada cor, 16384 cores. Total 65536 bytes.
    // 65536/4 = 16384 cores.
    case 16:
        //printf("16\n");
        //base = 0x36 + 65536;
        base = (OffsetForBase + 65536);
        break;
    // It is not present for bitmaps with 24 color bits 
    // because each pixel is represented by 
    // 24-bit red-green-blue (RGB) values in the actual bitmap data area.
    case 24:
        //printf("24\n");
        //base = 0x36 + 0;
        //base = 4 + 40;  // Right after the headers.
        //base = 54;  // Right after the headers.
        base = (OffsetForBase + 0);
        break;

    case 32:
        //printf("32\n");
        //base = 0x36 + 0;
        //base = 0x36 - 64;
        //base = 54;  // Right after the headers.
        base = (OffsetForBase + 0);
        break;

    // #bugbug
    // We need to abort.
    default:  
        //base = 0x36;
        base = (OffsetForBase + 0);
        gwssrv_debug_print ("bmpDisplayBMP: [FAIL] bmpBitCount fail\n"); 
        break;
    };

    //#debug
    //gwssrv_debug_print ("bmpDisplayBMP: for\n");
    //printf             ("bmpDisplayBMP: for\n");

//
// Draw
//

    for ( i=0; i < __Local_bi.bmpHeight; i++ )
    {
        for ( j=0; j < __Local_bi.bmpWidth; j++ )
        {
            // >> 16 cores
            // Um pixel por nibble.
            if (__Local_bi.bmpBitCount == 4)
            {
                offset = base;
                palette_index[0] = bmp[offset];

                // Segundo nibble.
                if (nibble_count_16colors == 2222)
                {
                    palette_index[0] = ( palette_index[0] & 0x0F);  
                    color = (unsigned int) palette[  palette_index[0]  ];

                    nibble_count_16colors = 0;
                    base = (base +1);

                // Primeiro nibble.
                }else{

                    palette_index[0] =  ( (  palette_index[0] >> 4 ) & 0x0F);
                    color = (unsigned int) palette[  palette_index[0] ];

                    nibble_count_16colors = 2222;
                    //base = base + 0;
                };
            }

            // >> 256 cores
            // Próximo pixel para 8bpp
            if (__Local_bi.bmpBitCount == 8)
            {   
                offset = (base +0);
                color = (unsigned int) palette[  bmp[offset] ];

                // Na area de dados comtem os indices para a paleta.
                base = (base +1);
            }

            // >>
            // Proximo pixel para 16bpp
            if (__Local_bi.bmpBitCount == 16)
            {
                // #bugbug: Wrong

                //a
                c[0] = 0;  

                offset = base;

                //b
                c[1] = bmp[offset];
                c[1] = (c[1] & 0xF8);      // '1111 1000' 0000 0000  

                //g
                c2[0] = bmp[offset];
                c2[0] = c2[0] &  0x07;     // '0000 0111' 0000 0000 
                c2[1] = bmp[offset+1];
                c2[1] = c2[1] &  0xE0;     //  0000 0000 '1110 0000' 
                c[2] = ( c2[0] | c2[1] );  

                //r
                c[3] = bmp[offset+1];
                c[3] = c[3] & 0x1F;        // 0000 0000 '0001 1111' 

                base = (base +2); 
            }

            // Próximo pixel para 24bpp.
            if (__Local_bi.bmpBitCount == 24)
            {
                offset = (base +0);  c[0] = bmp[offset];
                offset = (base +1);  c[1] = bmp[offset];
                offset = (base +2);  c[2] = bmp[offset];
                                     c[3] = 0;
                // Na area de dados contem as cores.
                base = (base +3); 
            }

            // Próximo pixel para 32bpp.
            if (__Local_bi.bmpBitCount == 32)
            {
                offset = (base +0);  c[0] = bmp[offset];
                offset = (base +1);  c[1] = bmp[offset];
                offset = (base +2);  c[2] = bmp[offset];
                offset = (base +3);  c[3] = bmp[offset];
                                     c[3] = 0;  // Apagando o 3.

                // Na area de dados contem as cores.
                base = (base +4);    
            }

            // Put pixel.
            // Nesse momento ja temos a cor selecionada 
            // no formato 0xaarrggbb ... 

            // A flag indica que teremos operação 
            // de substituição de cor.
            switch (bmp_change_color_flag)
            {
                // O pixel com a cor selecionada deve ser ignorado.
                case BMP_CHANGE_COLOR_TRANSPARENT:
                    
                    // Se a cor atual for diferente da cor selecionada,
                    // então pintamos.
                    if (color != bmp_selected_color)
                    {
                        // No zoom
                        // IN: color, x, y, rop
                        if (useZoom==FALSE)
                        {
                            libdisp_backbuffer_putpixel ( 
                                (unsigned int) color, 
                                (unsigned long) left, 
                                (unsigned long) bottom,
                                (unsigned long) 0 );
                        }
                        
                        // zoom
                        // IN: color, x, y, rop
                        if (useZoom==TRUE)
                        {
                            for (ihZoom=0; ihZoom < (ZoomFactor+1); ihZoom++){
                            for (iwZoom=0; iwZoom < (ZoomFactor+1); iwZoom++){ 
                            libdisp_backbuffer_putpixel ( 
                                (unsigned int) color, 
                                (unsigned long) left   + (j * ZoomFactor) + iwZoom, 
                                (unsigned long) bottom - (i * ZoomFactor) + ihZoom,
                                (unsigned long) 0 );
                            };};
                        }
                    }
                    
                    // Ignore o pixel.
                    //if ( color == bmp_selected_color )
                    //{}
                    
                    break;

                // Devemos substituir a cor selecionada por outra indicada.
                case BMP_CHANGE_COLOR_SUBSTITUTE:
                
                    // Se a cor encontrada é igual a cor selecionada,
                    // então devemos substituir pela cor indicada.
                    // IN: color, x, y, rop
                    if (color == bmp_selected_color)
                    {
                        libdisp_backbuffer_putpixel ( 
                            (unsigned int) bmp_substitute_color, 
                            (unsigned long) left, 
                            (unsigned long) bottom,
                            (unsigned long) 0 );

                    }
                    // Se a cor encontrada é diferente da
                    // cor selecionada. Então não devemos mudar a cor.
                    // IN: color, x, y, rop
                    if (color != bmp_selected_color)
                    {
                        libdisp_backbuffer_putpixel ( 
                            (unsigned int) color,  //bmp_substitute_color, 
                            (unsigned long) left, 
                            (unsigned long) bottom,
                            (unsigned long) 0 );
                    } 
                    break;

                // ...

                // 0 and default
                // Pintamos normalmente a cor atual.
                // IN: color, x,y,rop
                case BMP_CHANGE_COLOR_NULL:
                default:
                    libdisp_backbuffer_putpixel( 
                        (unsigned int) color,  //bmp_substitute_color, 
                        (unsigned long) left, 
                        (unsigned long) bottom,
                        (unsigned long) 0 );
                    break;
            };

            left++;    // next pixel.
        };

        // Vamos para a linha anterior.
        // Reiniciamos o x.

        bottom = (bottom-1);
        left = x;
    };


// ## test palette 
    //int p;

    //if (bi->bmpBitCount == 8)
    //{
    //    printf("\n");
    //    for ( p=0; p<16; ++p ){
    //        printf("%x\n",palette[p]);
    //    };
    //    printf("\n");
    //};

done:

//#todo
//Invalidate, not show.

// #todo
// Create a flag in the function's parameter.
    //if(flag==TRUE)
    gws_refresh_rectangle (X,Y,Width,Height);

// #debug
    //gwssrv_debug_print ("bmpDisplayBMP: done \n");
    //printf             ("bmpDisplayBMP: done \n");
    //printf("w={%d} h={%d}\n", bi->bmpWidth, bi->bmpHeight );
    
    return 0;

fail:
    gwssrv_debug_print ("bmpDisplayBMP: Fail\n");
    printf             ("bmpDisplayBMP: Fail\n");
    return (int) 1;
    //return (int) -1;
}

/*
 * __get_system_icon:
 *     Get an address to a shared memory buffer
 * where there is an icon previously loaded by the kernel.
 */
// Called by gwssrv_display_system_icon.
// O kernel vai retornar NULL se for fora do limite.
// limits=(1~5)

static void *__get_system_icon (int n)
{

// #bugbug
// #todo: max limit

    //#todo: if (n <= 0){
    if (n<0){
        return NULL;
    }

    return (void *) gramado_system_call(9100,n,n,n);
}

// gwssrv_display_system_icon: 
// Called by createwDrawFrame on createw.c
// >> Called by wmCreateWindowFrame in wm.c

int 
gwssrv_display_system_icon ( 
    int index, 
    unsigned long x, 
    unsigned long y )
{
    int RefreshScreen= FALSE;
// Shared memory
// Um endereço compartilhado onde o ícone
// foi carregado pelo kernel.
    char *sm_buffer;
// #todo: 
// limits for x and y.
    unsigned long bmp_x = (x & 0xFFFF);
    unsigned long bmp_y = (y & 0xFFFF);

// Get buffer address
    sm_buffer = (char *) __get_system_icon(index);
    if ( (void *) sm_buffer == NULL ){
        printf ("gwssrv_display_system_icon: sm_buffer\n");
        return -1;
    }

// Check BM header
    if ( sm_buffer[0] != 'B' || sm_buffer[1] != 'M' )
    {
        // #debug
        gwssrv_debug_print ("gwssrv_display_system_icon: [FAIL] header\n");
        printf             ("gwssrv_display_system_icon: [FAIL] header\n");
        printf ("gwssrv_display_system_icon: %c %c\n", 
            &sm_buffer[0], 
            &sm_buffer[1] );
        // #debug
        // Show the whole screen if fail
        gws_show_backbuffer();
        //return -1;
        while(1){}
    }

// Check BM header. Again.
    if ( sm_buffer[0] == 'B' && sm_buffer[1] == 'M' )
    {
        // #flags
        bmp_change_color_flag = BMP_CHANGE_COLOR_TRANSPARENT;
        //bmp_change_color_flag = BMP_CHANGE_COLOR_SUBSTITUTE;
        //bmp_change_color_flag = BMP_CHANGE_COLOR_NULL;
        bmp_selected_color = COLOR_WHITE;

        // Paint into the backbuffer, but refresh after that.
        
        bmpDisplayBMP( 
            (char *) sm_buffer, 
            (unsigned long) bmp_x, 
            (unsigned long) bmp_y ); 

        //gde_display_bmp((char *)sm_buffer, (unsigned long) 80, (unsigned long) 80); 
    }
    
    //#debug
    //printf("gwssrv_display_system_icon: *hang2\n");

    if (RefreshScreen == TRUE)
    {
        // #bugbug #todo
        // We need to use the routine to refresh the rectangle. 
        invalidate_surface_retangle();
        //gws_show_backbuffer();
    }

    return 0;
}

//
// End
//

