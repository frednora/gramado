// #todo
// bmp decoder for the libgui.
// Inporting the routines from another project

// bmp.c
// Very basic support for BMP files.
// We get an address calling the kernel given an index
// we're we find a pre-loaded image.
// 2015 - Created by Fred Nora.


// rtl (libc)
#include <stdlib.h>

// libgui
#include "include/libgui.h"

// BMP header
#include "include/bmp.h"


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

// static void *__get_system_icon(int n);

static int 
__bmpDisplayBMP0 ( 
    struct dccanvas_d *dc,
    char *address, 
    unsigned long x, 
    unsigned long y,
    int zoom_factor );

static int 
__bmp_decode_system_icon0 ( 
    struct dccanvas_d *dc,
    const char *img_buffer,
    unsigned long x, 
    unsigned long y,
    int zoom_factor );

// ------------------------------------

/*
 * __get_system_icon:
 *     Get an address to a shared memory buffer
 * where there is an icon previously loaded by the kernel.
 */
// Called by gwssrv_display_system_icon.
// O kernel vai retornar NULL se for fora do limite.
// limits=(1~5)

// #deprecated:
// We are removing from the kernel the support for this routine
/*
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
*/

/*
 * __bmpDisplayBMP0:
 * (Decode), Draw a pre-loaded image into the backbuffer.
 * IN:
 * address = Address for an undecoded BMP file.
 * x       = Target x position. 
 * y       = Target y position.
 * zoom_factor = Scale.
 * show = Show or not.
 */
// OUT: 0=ok | -1=fail.

// worker
static int 
__bmpDisplayBMP0 ( 
    struct dccanvas_d *dc,
    char *address, 
    unsigned long x, 
    unsigned long y,
    int zoom_factor )
{
// (Decode), Draw a pre-loaded image into the backbuffer.
 
// Validate context
    if (!dc || dc->initialized != TRUE || !dc->data)
        return -1;

// The address validation
// Endereço base do BMP que foi carregado na memoria
    unsigned char *bmp = (unsigned char *) address;
    if ((void*)address == NULL){
        goto fail;
    }

    register int i=0;
    register int j=0;

    // These are offsets, NOT addresses.
    int base=0;
    int offset=0;

    unsigned int left=0; 
    unsigned int top=0; 
    unsigned int bottom=0;

    // struct gws_rect_d finalRect;

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

// Limits:
// #todo: Get the system metrics

    xLimit = 1024;  //800;
    yLimit = 768;   //600;

// Limits
    if (x > xLimit || y > yLimit)
    {
        printf ("__bmpDisplayBMP0: Limits\n");
        goto fail;
    }

// Address validation
    if (address == 0){
        printf ("__bmpDisplayBMP0: address\n");
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
        printf  ("__bmpDisplayBMP0: [FAIL] signature %c %c\n", 
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
    //finalRect.left = left;
    //finalRect.top  = top;
    //finalRect.width  = (__Local_bi.bmpWidth * ZoomFactor);
    //finalRect.height = (bottom-top);

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

    // --- 1 bpp (Monochrome) ---
    // Each pixel is a single bit (0 or 1).
    // Palette has 2 entries. (2 colors × 4 bytes = 8 bytes).
    // Data starts right after the palette.
    case 1:
        base = (OffsetForBase + 8);
        break;

    // --- 4 bpp (16 colors) ---
    // Each pixel is a 4‑bit nibble (two pixels per byte).
    // Palette has 16 entries (16 × 4 bytes = 64 bytes).
    // Data starts right after the palette.
    case 4:  
        base = (OffsetForBase + 64);
        break; 

    // --- 8 bpp (256 colors) ---
    // Each pixel is one byte (index into palette).
    // Palette has 256 entries (256 × 4 bytes = 1024 bytes).
    // Data starts right after the palette.
    case 8:
        base = (OffsetForBase + 1024);
        break;

    // --- 16 bpp (High color) ---
    // Each pixel is 2 bytes.
    // Two common layouts:
    //   - RGB 5:5:5 (BI_RGB)
    //   - RGB 5:6:5 (BI_BITFIELDS)
    // We assume 5:6:5 (most common).
    // Pixel layout: RRRRRGGGGGGBBBBB
    // Data starts immediately after headers (no palette).
    case 16:
        base = OffsetForBase;
        break;

    // --- 24 bpp (True color) ---
    // Each pixel is 3 bytes (B, G, R).
    // No palette; data starts immediately after headers.
    case 24:
        base = OffsetForBase;
        break;

    // --- 32 bpp (True color with alpha) ---
    // Each pixel is 4 bytes (B, G, R, A).
    // No palette; data starts immediately after headers.
    case 32:
        base = OffsetForBase;
        break;

    // --- Unsupported bpp ---
    // Abort if we encounter an unknown bit depth.
    default:  
        base = OffsetForBase;
        printf("__bmpDisplayBMP0: Unsupported bpp %d\n", BitsPerPixel);
        goto fail;
    };

    //#debug
    //printf ("bmpDisplayBMP: for\n");

//
// Draw
//

//
// Main drawing loop
// Iterate over each row (height) and column (width)
// BMPs are bottom‑up, so we adjust Y later.
//

    for (i=0; i < __Local_bi.bmpHeight; i++)
    {
        for (j=0; j < __Local_bi.bmpWidth; j++)
        {
            // --- Decode pixel depending on bit depth ---

            // ---------------------------------
            // 4 bpp (16 colors, palette indexed)
            // Each byte contains two pixels (high nibble + low nibble).
            // ps: 2222 is simply a signature.
            if (__Local_bi.bmpBitCount == 4)
            {
                offset = base;
                palette_index[0] = bmp[offset];  // Read one byte from the BMP data

                // Second nibble:
                // If we’re on the second nibble → extract the low nibble
                if (nibble_count_16colors == 2222){
                    palette_index[0] = (palette_index[0] & 0x0F);  
                    base = base + 1;
                    nibble_count_16colors = 0;  // signature

                // First nibble:
                // If we’re on the first nibble → extract the high nibble
                } else {
                    palette_index[0] = ( (  palette_index[0] >> 4 ) & 0x0F);
                    // base not advanced yet, still need low nibble from same byte
                    // base = base + 0;
                    nibble_count_16colors = 2222;  // signature
                };

                // Palette lookup happens once, after nibble extraction
                color = (unsigned int) palette[  palette_index[0]  ];
            }

            // ---------------------------------
            // 8 bpp (256 colors, palette indexed)
            // Each byte is a direct index into the palette.
            // Palette size: 256 entries (256 × 4 bytes = 1024 bytes).
            // Each palette entry is a 32‑bit ARGB color.
            if (__Local_bi.bmpBitCount == 8)
            {
                offset = (base +0);
                color = (unsigned int) palette[  bmp[offset] ];

                // In the data are we have the indexes for the pallete
                base = (base +1);  // Advance the base pointer
            }

            // ---------------------------------
            // 16 bpp (High color, 5:6:5 format)
            // Each pixel is 2 bytes: RRRRRGGGGGGBBBBB
            // Two common layouts:
            // RGB 5:5:5 → 5 bits Red, 5 bits Green, 5 bits Blue, 1 unused bit.
            // RGB 5:6:5 → 5 bits Red, 6 bits Green, 5 bits Blue (most common).
            // No palette — colors are stored directly in the pixel data.
            // #todo:
            // Check which layout to use:
            // If compression == BI_RGB → assume 5:5:5.
            // If compression == BI_BITFIELDS → assume 5:6:5 (or read masks if present).
            if (__Local_bi.bmpBitCount == 16)
            {
                // Getting the short value
                unsigned char lo = bmp[base];       // low byte
                unsigned char hi = bmp[base + 1];   // high byte
                unsigned short pixel = (hi << 8) | lo;

                // Extract 5:6:5 channels
                unsigned char r = (pixel & 0xF800) >> 11; // 5 bits red
                unsigned char g = (pixel & 0x07E0) >> 5;  // 6 bits green
                unsigned char b = (pixel & 0x001F);       // 5 bits blue

                // Scale to 8‑bit per channel
                r = (r << 3);
                g = (g << 2);
                b = (b << 3);

                // Compose ARGB color (alpha ignored)
                color = (r << 16) | (g << 8) | b;

                base += 2; // advance two bytes per pixel
            }

            // ---------------------------------
            // 24 bpp (True color, no palette)
            // Each pixel is 3 bytes: B, G, R
            // Stored in BGR order in BMP data.
            // Alpha channel is not present.
            if (__Local_bi.bmpBitCount == 24)
            {
                unsigned char b = bmp[base + 0]; // Blue
                unsigned char g = bmp[base + 1]; // Green
                unsigned char r = bmp[base + 2]; // Red

                // Compose ARGB color (alpha ignored, set to 0)
                color = (r << 16) | (g << 8) | b;

                // advance three bytes per pixel
                base = (base +3);
            }

            // ---------------------------------
            // 32 bpp (True color with alpha)
            // Each pixel is 4 bytes: B, G, R, A
            // Stored in BGRA order in BMP data.
            // Alpha channel may be ignored or used depending on compositor needs.
            if (__Local_bi.bmpBitCount == 32)
            {
                unsigned char b = bmp[base + 0]; // Blue
                unsigned char g = bmp[base + 1]; // Green
                unsigned char r = bmp[base + 2]; // Red
                unsigned char a = bmp[base + 3]; // Alpha

                // Compose ARGB color
                // If you want to ignore alpha, set it to 0xFF (opaque).
                // If you want transparency, keep 'a' as is.
                color = (a << 24) | (r << 16) | (g << 8) | b;

                base += 4; // advance four bytes per pixel
            }

            // ---------------------------------
            // Apply color change rules before drawing pixel
            // bmp_change_color_flag controls how we treat the decoded color:
            //
            //   BMP_CHANGE_COLOR_TRANSPARENT  → skip selected color
            //   BMP_CHANGE_COLOR_SUBSTITUTE   → replace selected color
            //   BMP_CHANGE_COLOR_NULL/default → draw normally
            //

            switch (bmp_change_color_flag)
            {
                // ---------------------------------
                // Transparent mode
                // Skip drawing if the current color matches bmp_selected_color.
                // Otherwise, draw normally.

                case BMP_CHANGE_COLOR_TRANSPARENT:
                    // Só pintamos se a cor atual for diferente
                    // da cor selecionada.
                    if (color != bmp_selected_color)
                    {
                        // No scale
                        if (useZoom == FALSE)
                        {
                            libgui_putpixel0 (
                                dc, 
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
                            libgui_putpixel0 (
                                dc, 
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

                // ---------------------------------
                // Substitute mode
                // If current color == bmp_selected_color → replace with bmp_substitute_color.
                // Otherwise, draw the original color.
                // Zoom logic applied here as well.

                case BMP_CHANGE_COLOR_SUBSTITUTE:
                if (useZoom == FALSE) 
                {
                    // No zoom → draw single pixel
                    if (color == bmp_selected_color) {
                        libgui_putpixel0(dc, bmp_substitute_color, left, bottom, 0);
                    } else {
                        libgui_putpixel0(dc, color, left, bottom, 0);
                    }
                } 
                if (useZoom == TRUE) 
                {
                    // With zoom → draw a block of pixels
                    for (ihZoom = 0; ihZoom < ZoomFactor; ihZoom++) {
                    for (iwZoom = 0; iwZoom < ZoomFactor; iwZoom++) {
                        if (color == bmp_selected_color) {
                            libgui_putpixel0(dc, bmp_substitute_color,
                               left + ((j * ZoomFactor) + iwZoom),
                               bottom - ((i * ZoomFactor) + ihZoom),
                               0);
                        } else {
                            libgui_putpixel0(dc, color,
                                left + ((j * ZoomFactor) + iwZoom),
                                bottom - ((i * ZoomFactor) + ihZoom),
                                0);
                        }
                    }}
                }
                break;

                // ...

                // ---------------------------------
                // Normal mode (default)
                // Draw the decoded color without modification.

                case BMP_CHANGE_COLOR_NULL:
                default:
                    if (useZoom == FALSE)
                    {
                        libgui_putpixel0 (
                            dc, 
                            (unsigned int) color, 
                            (unsigned long) left, 
                            (unsigned long) bottom,
                            (unsigned long) 0 );
                    }
                    if (useZoom == TRUE)
                    {
                        for (ihZoom=0; ihZoom < ZoomFactor; ihZoom++){
                        for (iwZoom=0; iwZoom < ZoomFactor; iwZoom++){                            
                        libgui_putpixel0(
                            dc, 
                            color, 
                            left + ((j * ZoomFactor) + iwZoom), 
                            bottom - ((i * ZoomFactor) + ihZoom), 
                            0);

                        };};
                    }
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
    /*
    if (show == TRUE)
    {
        gws_refresh_rectangle (
            finalRect.left,
            finalRect.top,
            finalRect.width,
            finalRect.height );
    }
    */

    // #debug
    //server_debug_print ("bmpDisplayBMP0: done \n");
    //printf             ("bmpDisplayBMP0: done \n");
    //printf("w={%d} h={%d}\n", __Local_bi.bmpWidth, __Local_bi.bmpHeight );

    return 0;

fail:
    //server_debug_print ("bmpDisplayBMP0: fail\n");
    printf             ("__bmpDisplayBMP0: fail\n");
    return (int) -1;
}

/*
// wrapper
int 
bmpDisplayBMP ( 
    char *address, 
    unsigned long x, 
    unsigned long y,
    int show )
{
// Decode, paint and maybe refresh.

    if ((void*) address == NULL)
        return (int) -1;

    int res=0;
    res = (int) bmpDisplayBMP0( 
        address, 
        x, 
        y, 
        BMP_DEFAULT_ZOOM_FACTOR,
        show );

    return (int) res;
}
*/

// __bmp_decode_system_icon0:
// Called by createwDrawFrame on createw.c
// >> Called by doCreateWindowFrame in wm.c
static int 
__bmp_decode_system_icon0 ( 
    struct dccanvas_d *dc,
    const char *img_buffer,
    unsigned long x, 
    unsigned long y,
    int zoom_factor )
{
    //#expensive: Refresh the whole screen.    
    //int RefreshScreen= FALSE;
    //int RefreshScreen = show;

// Shared memory
// Um endereço compartilhado onde o ícone
// foi carregado pelo kernel.

// #todo: 
// limits for x and y.
    unsigned long bmp_x = (x & 0xFFFF);
    unsigned long bmp_y = (y & 0xFFFF);


// Validate context
    if (!dc || dc->initialized != TRUE || !dc->data)
        return -1;


    if ((void*) img_buffer == NULL){
        printf("bmp_decode_system_icon0: img_buffer\n");
        goto fail;
    }

    // Check BM header
    if ( img_buffer[0] != 'B' || img_buffer[1] != 'M' )
    {
        printf("bmp_decode_system_icon0: [FAIL] header\n");
        printf("bmp_decode_system_icon0: %c %c\n", 
            &img_buffer[0], &img_buffer[1] );
        // #debug
        // Show the whole screen if fail
        //gws_show_backbuffer();
        //return -1;
        //while (1){
        //};
        return (int) -1;
    }


//
// Draw the BMP image
//

    int draw_status=-1;

// Check BM header. Again.
    if ( img_buffer[0] == 'B' && img_buffer[1] == 'M' )
    {
        // #flags
        bmp_change_color_flag = BMP_CHANGE_COLOR_TRANSPARENT;
        //bmp_change_color_flag = BMP_CHANGE_COLOR_SUBSTITUTE;
        //bmp_change_color_flag = BMP_CHANGE_COLOR_NULL;
        bmp_selected_color = 0x00FFFFFF; //COLOR_WHITE;

        // Paint into the backbuffer, but refresh after that.
        draw_status = 
            (int) __bmpDisplayBMP0( 
                dc,
                (char *) img_buffer, 
                (unsigned long) bmp_x, 
                (unsigned long) bmp_y,
                zoom_factor );
 
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
bmp_decode_bmp_image ( 
    struct bmp_cache_d *cache,
    struct dccanvas_d *dc,
    unsigned long x, 
    unsigned long y,
    int zoom_factor )
{

// Validate context
    if (!dc)
        return -1;
    if (dc->initialized != TRUE)
        return -1;
    if (!dc->data)
        return -1;

// cache
    if ((void*) cache == NULL){
        printf("bmp_decode_system_icon: cache\n");
        goto fail;
    }
    if (cache->loaded != TRUE){
        printf("bmp_decode_system_icon: Not loaded\n");
        goto fail;
    }

//
// Buffer
//

    char *buffer_p = (char *) cache->buffer;

    if ((void*) buffer_p == NULL){
        printf("bmp_decode_system_icon: buffer_p\n");
        goto fail;
    }

// worker
// Decode the raw image

    int res=0;
    res = 
        (int) __bmp_decode_system_icon0(
                dc,
                buffer_p,
                x,
                y,
                zoom_factor  //BMP_DEFAULT_ZOOM_FACTOR 
            );

    return (int) res;

fail:
    return (int) -1;
}


struct bmp_cache_d *bmp_load_bmp_image(const char *pathname)
{
    struct bmp_cache_d *cache;
    int ReturnValue = 0;
    int fdRead = -1;
    register int nreads = 0;
    register int nwrites = 0;

// Parameter
    if ((void*) pathname == NULL){
        printf ("bmp_load_bmp_image: Missing pathname parameter\n");
        goto fail;
    }

// ----------------------------
// #test: 
// Allocate memory for the file
// We don't know its size, our limit will be 8KB for now.
    char *buffer_p;
    size_t BufferSize = (1024 *8);  // 8KB

    buffer_p = (char *) malloc(BufferSize);
    if ((void*) buffer_p == NULL){
        printf("bmp_load_bmp_image: buffer_p\n");
        goto fail;
    }

// ----------------------------
// Open
    fdRead = (int) open((char *) pathname, 0, "a+");
    if (fdRead < 0){
        printf ("bmp_load_bmp_image: on open()\n");
        goto fail;
    }

// Read from fd
// Reading 4KB into a 8KB buffer.
// #todo: We need to read 8KB

    nreads = (int) read(fdRead, buffer_p, 1024*4);
    if (nreads <= 0){
        printf ("bmp_load_bmp_image: File {%d} failed on read()\n", fdRead);
        goto fail;
    }


//
// Cache
//

    cache = (struct bmp_cache_d *) malloc( sizeof(struct bmp_cache_d) );
    if ((void*) cache == NULL)
    {
        printf("bmp_load_bmp_image: cache\n");
        goto fail;
    }

    // fill it
    cache->buffer = (char *) buffer_p;
    cache->loaded = TRUE;

    return (struct bmp_cache_d *) cache;

fail:
    return NULL;
}


//
// End
//

