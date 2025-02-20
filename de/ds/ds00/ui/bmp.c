// bmp.c
// Very basic support for BMP files.
// We get an address calling the kernel given an index
// we're we find a pre-loaded image.
// 2015 - Created by Fred Nora.

#include "gwsint.h"

// Signature. "MB".
#define BMP_TYPE  0x4D42
#define GWS_BMP_TYPE  BMP_TYPE

// Offsets in gws_bmp_infoheader_d structure.
#define GWS_BMP_OFFSET_WIDTH      18
#define GWS_BMP_OFFSET_HEIGHT     22
#define GWS_BMP_OFFSET_BITPLANES  26
#define GWS_BMP_OFFSET_BITCOUNT   28
//...


//
// Color support
//

// Flag que avisa que deve haver alguma mudança nas cores. 
// see: bmp.h
int bmp_change_color_flag = BMP_CHANGE_COLOR_NULL;
// Cor selecionada para ser substituída ou ignorada. 
unsigned int bmp_selected_color=0;
// Salva-se aqui uma cor para substituir outra. 
unsigned int bmp_substitute_color=0; 


// 4bpp support.
static int nibble_count_16colors = 0;
// Usados temporariamente por cada uma das exibições.
struct gws_bmp_header_d      __Local_bh;
struct gws_bmp_infoheader_d  __Local_bi;

//
// Private
//

static void *__get_system_icon(int n);

// ------------------------------------

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

/*
 * bmpDisplayBMP0:
 * (Decode), Draw a pre-loaded image into the backbuffer.
 * IN:
 * address = Address for an undecoded BMP file.
 * x       = Target x position. 
 * y       = Target y position.
 * zoom_factor = Scale.
 * show = Show or not.
 */
// OUT: 0=ok | -1=fail.
int 
bmpDisplayBMP0 ( 
    char *address, 
    unsigned long x, 
    unsigned long y,
    int zoom_factor,
    int show )
{
// (Decode), Draw a pre-loaded image into the backbuffer.
 
// The address validation
// Endereço base do BMP que foi carregado na memoria
    unsigned char *bmp = (unsigned char *) address;
    if ((void*)address==NULL){
        goto fail;
    }

    register int i=0;
    register int j=0;
    int base=0;
    int offset=0;
    unsigned int left=0; 
    unsigned int top=0; 
    unsigned int bottom=0;

    struct gws_rect_d finalRect;

// Zoom support.
// It is working.
// But we need to work in the 'position' thing
// and accept some function parameters for that effect.

    //int useZoom=FALSE;
    int useZoom=TRUE;
    //int ZoomFactor = BMP_DEFAULT_ZOOM_FACTOR;
    int ZoomFactor = zoom_factor;
    //#hack #provisorio
    if (zoom_factor == 1)
        useZoom = FALSE;

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
    //server_debug_print ("bmpDisplayBMP:\n");

// Limits
    if ( x > xLimit || y > yLimit ){
        //server_debug_print ("bmpDisplayBMP0: Limits\n");
        printf ("bmpDisplayBMP0: Limits\n");
        goto fail;
    }

// #todo:
// Testar validade do endereço.
    if (address == 0){
        //server_debug_print ("bmpDisplayBMP0: address\n");
        printf ("bmpDisplayBMP0: address\n");
        goto fail;
    }

// See: 
// https://en.wikipedia.org/wiki/BMP_file_format

// -------------
// 0 - Signature
// (2 bytes)
    sig = *(unsigned short *) &bmp[0];
    __Local_bh.bmpType = (unsigned short) sig;
    //printf ("sig={%x}\n",sig);

    if ( bmp[0] != 'B' || bmp[1] != 'M' )
    {
        //server_debug_print ("bmpDisplayBMP0: [FAIL] signature \n");
        printf  ("bmpDisplayBMP0: [FAIL] signature %c %c\n", 
            bmp[0], bmp[1]);
        goto fail;
    }

// -------------
// 2 - The size of the BMP file in bytes
// (4 bytes)
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



//
// struct for Info header
//

// Windows bmp.

/*
    bi = (struct gws_bmp_infoheader_d *) malloc ( sizeof(struct gws_bmp_infoheader_d) );
    if ( (void *) bi == NULL )
    {
        server_debug_print ("bmpDisplayBMP: bi fail \n");
        printf             ("bmpDisplayBMP: bi fail \n");
        goto fail;
    }
*/

// --------------
// The size of this header.
// 4 bytes
    __Local_bi.bmpSize = *(unsigned int *) &bmp[14];
    //printf ("HeaderSize={%x}\n", __Local_bi.bmpSize);

// X and Y.
// #todo: tipagem (type) xxx;
    X = (x & 0xFFFF);
    Y = (y & 0xFFFF);

// --------------
// 18 and 22.
// Width and height.
// #todo: tipagem (type) xxx;
// Save it into a local structure.
// 4 bytes each.
    Width  = *(unsigned int *) &bmp[18];
    Height = *(unsigned int *) &bmp[22];
    __Local_bi.bmpWidth  = (unsigned int) (Width  & 0xFFFF);
    __Local_bi.bmpHeight = (unsigned int) (Height & 0xFFFF);
    //printf ("w=%d h=%d\n",Width,Height);


// -----------------
// 26 - Number of color planes
// (2 bytes)


// -----------------
// 28 - Number of bits per pixel.
// (2 bytes)
// The number of bits per pixel, 
// which is the color depth of the image.
// Typical values are 1, 4, 8, 16, 24 and 32. 
// #todo: tipagem (type) xxx;
    __Local_bi.bmpBitCount = *(unsigned short *) &bmp[28];
    //printf ("Count={%d}\n", __Local_bi.bmpBitCount );    
// #limits
    //if (__Local_bi.bmpBitCount != 24){
    //    printf("bmpDisplayBMP: Count fail\n");
    //    goto fail;
    // }

// ---------------------
// 30 - The compression method being used. 
// 0 = No compression.

/*
    //__Local_bi.bmpCompression = *(unsigned int *) &bmp[30];
    if (__Local_bi.bmpCompression != 0){
        server_debug_print ("bmpDisplayBMP: bmpCompression fail \n");
        printf             ("bmpDisplayBMP: bmpCompression fail \n");
        goto fail;
    }
*/


// ---------------
// 46 - Number of color used.
// The number of colors in the color palette, 
// or 0 to default to 2^n.
    //__Local_bi.bmpClrUsed = *(unsigned int *) &bmp[46];
    //printf("%d\n",__Local_bi.bmpClrUsed);
    /*
    if (__Local_bi.bmpClrUsed != 256)
    {
        printf("%d\n",__Local_bi.bmpClrUsed);
        while (1){
        };
    }
    */

// ---------------------

//
// Draw
//

    //#debug
    //server_debug_print ("bmpDisplayBMP: Draw!\n");
    //printf             ("bmpDisplayBMP: Draw!\n");

// Top, Left, Bottom.
// #todo: tipagem.

    left = (x & 0xFFFF);
    top  = (y & 0xFFFF);
// Bottom afected by the zoom factor.
    //bottom = ( top + __Local_bi.bmpHeight );
    bottom = 
        ( top + 
          (__Local_bi.bmpHeight * ZoomFactor) );

// Final rect to refresh.
    finalRect.left = left;
    finalRect.top  = top;
    finalRect.width  = (__Local_bi.bmpWidth * ZoomFactor);
    finalRect.height = (bottom-top);

// --------------------------

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
        //server_debug_print ("bmpDisplayBMP0: [FAIL] bmpBitCount fail\n"); 
        break;
    };

    //#debug
    //server_debug_print ("bmpDisplayBMP: for\n");
    //printf             ("bmpDisplayBMP: for\n");

//
// Draw
//

    for (i=0; i < __Local_bi.bmpHeight; i++)
    {
        for (j=0; j < __Local_bi.bmpWidth; j++)
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
                    palette_index[0] = (palette_index[0] & 0x0F);  
                    color = (unsigned int) palette[  palette_index[0]  ];
                    nibble_count_16colors = 0;
                    base = base + 1;
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

                // #todo: This is wrong!

                //a
                c[0] = 0;  

                //b
                offset = base;
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

            // Put pixel
            // Nesse momento ja temos a cor selecionada 
            // no formato 0xaarrggbb ... 
            // Agora se a flag de mascara estiver selecionada,
            // entao devemos ignora o pixel e nao pinta-lo.

            switch (bmp_change_color_flag)
            {
                // 1000
                // flag para ignorarmos a cor selecionada.
                // Nao pinte nada.
                // Devemos pintar caso a cor atual seja  
                // diferente da cor selecionada.

                case BMP_CHANGE_COLOR_TRANSPARENT:
                    // Só pintamos se a cor atual for diferente
                    // da cor selecionada.
                    if (color != bmp_selected_color)
                    {
                        // No scale
                        if (useZoom==FALSE)
                        {
                            // IN: color, x, y, rop
                            libdisp_backbuffer_putpixel ( 
                                (unsigned int) color, 
                                (unsigned long) left, 
                                (unsigned long) bottom,
                                (unsigned long) 0 );
                        }

                        // #test
                        // Testing zoom support.
                        
                        // With scale.
                        if (useZoom==TRUE)
                        {
                            for (ihZoom=0; ihZoom < ZoomFactor; ihZoom++){
                            for (iwZoom=0; iwZoom < ZoomFactor; iwZoom++){ 
                            // IN: color, x, y, rop
                            libdisp_backbuffer_putpixel ( 
                                (unsigned int) color, 
                                (unsigned long) 
                                    left + 
                                    ((j * ZoomFactor) + iwZoom), 
                                (unsigned long) 
                                    bottom - 
                                    ((i * ZoomFactor) + ihZoom),
                                (unsigned long) 0 );
                            };};
                        }
                    }
                    break;

                // 2000
                // Substitua pela cor indicada.
                // Se a cor atual eh igual a cor selecionada,
                // devemos substituir a cor atual pela substituta.
                // Mas se a cor atual for diferente da cor selecionada,
                // pintamos normalmente a cor atual.
                case BMP_CHANGE_COLOR_SUBSTITUTE:
                    // Substituímos se for igual
                    if (color == bmp_selected_color)
                    {
                        // IN: color, x, y, rop
                        libdisp_backbuffer_putpixel ( 
                            (unsigned int) bmp_substitute_color, 
                            (unsigned long) left, 
                            (unsigned long) bottom,
                            (unsigned long) 0 );
                    }
                    // Não substituímos se for diferente.
                    // #bugbug
                    // #todo: Usar a cor atual e não a substituta.
                    if (color != bmp_selected_color)
                    {
                        // IN: color, x, y, rop
                        libdisp_backbuffer_putpixel ( 
                            (unsigned int) color, //bmp_substitute_color, 
                            (unsigned long) left, 
                            (unsigned long) bottom,
                            (unsigned long) 0 );
                    }
                    break;

                // ...

                // 0 and default.
                // Pintamos normalmente a cor atual.
                // #bugbug #todo:
                // Usar a cor atual e não a substituta.
                case BMP_CHANGE_COLOR_NULL:
                default:
                    // IN: color, x, y, rop
                    libdisp_backbuffer_putpixel( 
                        (unsigned int) color,
                        (unsigned long) left, 
                        (unsigned long) bottom,
                        (unsigned long) 0 );
                    break;
            };

            // next pixel.
            // Esse é o repetidor se não estivermos usando zoom.
            // Se estivermos usando zoom, o repetidor é o do for.
            if (useZoom != TRUE){
                left++;
            }
        };

        // Vamos para a linha anterior.
        // Reiniciamos o x.

        // Esse é o repetidor se não estivermos usando zoom.
        // Se estivermos usando zoom, o repetidor é o do for.
        if (useZoom != TRUE){
            bottom = (bottom-1);
            left = x;
        }
    };

// #test: 
// Palette 

    //int p;
    //if (__Local_bi.bmpBitCount == 8)
    //{
    //    printf("\n");
    //    for ( p=0; p<16; ++p ){
    //        printf("%x\n",palette[p]);
    //    };
    //    printf("\n");
    //}

done:

//#todo
//Invalidate, not show.

// #todo
// Create a flag in the function's parameter.
// Final rect to refresh.
    if (show == TRUE){
        gws_refresh_rectangle (
            finalRect.left,
            finalRect.top,
            finalRect.width,
            finalRect.height );
    }

    // #debug
    //server_debug_print ("bmpDisplayBMP0: done \n");
    //printf             ("bmpDisplayBMP0: done \n");
    //printf("w={%d} h={%d}\n", __Local_bi.bmpWidth, __Local_bi.bmpHeight );

    return 0;

fail:
    //server_debug_print ("bmpDisplayBMP0: fail\n");
    printf             ("bmpDisplayBMP0: fail\n");
    return (int) -1;
}

int 
bmpDisplayBMP ( 
    char *address, 
    unsigned long x, 
    unsigned long y,
    int show )
{
// Decode, paint and maybe refresh.

    int res=0;
    res = (int) bmpDisplayBMP0( 
        address, 
        x, 
        y, 
        BMP_DEFAULT_ZOOM_FACTOR,
        show );

    return (int) res;
}

// gwssrv_display_system_icon:
// Called by createwDrawFrame on createw.c
// >> Called by wmCreateWindowFrame in wm.c
int 
bmp_decode_system_icon0 ( 
    int index, 
    unsigned long x, 
    unsigned long y,
    int show,
    int zoom_factor )
{
    //#expensive: Refresh the whole screen.    
    //int RefreshScreen= FALSE;
    int RefreshScreen = show;

// Shared memory
// Um endereço compartilhado onde o ícone
// foi carregado pelo kernel.
    char *sm_buffer;
// #todo: 
// limits for x and y.
    unsigned long bmp_x = (x & 0xFFFF);
    unsigned long bmp_y = (y & 0xFFFF);

// Get buffer address.
// Check pointer validation
    sm_buffer = (char *) __get_system_icon(index);
    if ((void *) sm_buffer == NULL){
        printf ("bmp_decode_system_icon0: sm_buffer\n");
        goto fail;
    }

// Check BM header
    if ( sm_buffer[0] != 'B' || sm_buffer[1] != 'M' )
    {
        // #debug
        //server_debug_print ("gwssrv_display_system_icon: [FAIL] header\n");
        printf             ("gwssrv_display_system_icon: [FAIL] header\n");
        printf ("gwssrv_display_system_icon: %c %c\n", 
            &sm_buffer[0], &sm_buffer[1] );
        // #debug
        // Show the whole screen if fail
        gws_show_backbuffer();
        //return -1;
        while (1){
        };
    }

//
// Draw the BMP image
//

    int draw_status=-1;

// Check BM header. Again.
    if ( sm_buffer[0] == 'B' && sm_buffer[1] == 'M' )
    {
        // #flags
        bmp_change_color_flag = BMP_CHANGE_COLOR_TRANSPARENT;
        //bmp_change_color_flag = BMP_CHANGE_COLOR_SUBSTITUTE;
        //bmp_change_color_flag = BMP_CHANGE_COLOR_NULL;
        bmp_selected_color = COLOR_WHITE;
        // Paint into the backbuffer, but refresh after that.
        draw_status = 
            (int) bmpDisplayBMP0( 
                (char *) sm_buffer, 
                (unsigned long) bmp_x, 
                (unsigned long) bmp_y,
                zoom_factor,
                show ); 
        if (draw_status<0){
            //#todo: error message.
        } 
    }

    //#debug
    //printf("gwssrv_display_system_icon: hang2\n");
     
// #bugbug #todo
// We need to use the routine to refresh the rectangle.

    //if (RefreshScreen == TRUE){
    //    invalidate_surface_retangle();
        //gws_show_backbuffer();
    //}

    return 0;
fail:
    return -1;
}

int 
bmp_decode_system_icon ( 
    int index, 
    unsigned long x, 
    unsigned long y,
    int show )
{
    int res=0;
    res = (int) bmp_decode_system_icon0(
        index,
        x,
        y,
        show, 
        BMP_DEFAULT_ZOOM_FACTOR );
    
    return (int) res;
}

//
// End
//

