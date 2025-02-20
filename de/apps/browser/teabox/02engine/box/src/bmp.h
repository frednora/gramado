
// bmp.h
// BMP file support.

#ifndef __ENG_BMP_H
#define __ENG_BMP_H    1

// Signature "MB" 
#define BMP_TYPE 0x4D42        

// Não há operação com cor.
#define  BMP_CHANGE_COLOR_NULL  0
// O pixel com a cor selecionada deve ser ignorado.
#define  BMP_CHANGE_COLOR_TRANSPARENT  1000
// Devemos substituir a cor selecionada por outra indicada.
#define  BMP_CHANGE_COLOR_SUBSTITUTE   2000
// ...


// Flag que avisa que deve haver alguma mudança nas cores. 
extern int bmp_change_color_flag; 
// Salva-se aqui uma cor para substituir outra. 
extern unsigned int bmp_substitute_color; 
// Cor selecionada para ser substituída ou ignorada. 
extern unsigned int bmp_selected_color; 


//
// ## BMP support ##
//

// Signature "MB"
#define GWS_BMP_TYPE  0x4D42

// Offsets
#define GWS_BMP_OFFSET_WIDTH      18
#define GWS_BMP_OFFSET_HEIGHT     22
#define GWS_BMP_OFFSET_BITPLANES  26
#define GWS_BMP_OFFSET_BITCOUNT   28
//...

// See: https://en.wikipedia.org/wiki/BMP_file_format
struct gws_bmp_header_d                     
{
    unsigned short bmpType;       //  0  - Magic number for file
    unsigned int   bmpSize;       //  2  - The size of the BMP file in bytes
    unsigned short bmpReserved1;  //  6  - Reserved
    unsigned short bmpReserved2;  //  8  - ...
    unsigned int   bmpOffBits;    // 10  - Offset to bitmap data
};

// See: https://en.wikipedia.org/wiki/BMP_file_format   
struct gws_bmp_infoheader_d                     
{
    unsigned int  bmpSize;           // 14 - Size of info header
    unsigned int  bmpWidth;          // 18 - Width of image
    unsigned int  bmpHeight;         // 22 - Height of image
    unsigned short bmpPlanes;        // 26 - Number of color planes
    unsigned short bmpBitCount;      // 28 - Number of bits per pixel
    unsigned int  bmpCompression;    // 30 - Type of compression to use
    unsigned int  bmpSizeImage;      // 34 - Size of image data
    unsigned int  bmpXPelsPerMeter;  // 38 - X pixels per meter
    unsigned int  bmpYPelsPerMeter;  // 42 - Y pixels per meter
    unsigned int  bmpClrUsed;        // 46 - Number of colors used
    unsigned int  bmpClrImportant;   // 50 - Number of important colors
};

// -------------------------------------------

int 
bmpDisplayBMP ( 
    char *address, 
    unsigned long x, 
    unsigned long y );

int 
gwssrv_display_system_icon( 
    int index, 
    unsigned long x, 
    unsigned long y );

#endif    

//
// End
//


