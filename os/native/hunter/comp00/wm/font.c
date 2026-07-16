// font.c
// The font support.
// Created by Fred Nora.

#include "../ds.h"

// See: font.h
struct font_initialization_d  FontInitialization;

struct font_info_d  font00;
struct font_info_d  font01;
struct font_info_d  font02;
struct font_info_d  font03;

unsigned long fontList[FONTLIST_MAX];


// ----------------------------------------------------------

static unsigned char TableBitReverse[] = {

  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};


// The final place for the lt font.
char *font_lt8x8;


// =====================================================================
// Prototypes

static char *__initialize_lt8x8_font(void);

// =====================================================================

static char *__initialize_lt8x8_font(void)
{
    unsigned char *tmp_buffer;
    unsigned char c=0;
    register int i=0;
    const int SizeInBytes = (256*8);

// Tmp buffer
    tmp_buffer = (unsigned char *) malloc(SizeInBytes);
    if ((void *) tmp_buffer == NULL){
        printf ("__initialize_lt8x8_font: tmp_buffer\n");
        goto fail;
    }

// Pega um char da fonte lt e coloca na buffer provisorio.
    for (i=0; i<SizeInBytes; i++)
    {
        c      = (unsigned char) __inverted_font_lt8x8[i];
        tmp_buffer[i] = (unsigned char) TableBitReverse[c];
    }; 

// Done:
// Return the address of the new font.
    return (char*) tmp_buffer;
fail:
    return NULL;
}


void 
fontSetInfo(
    unsigned long address, 
    unsigned long char_width, 
    unsigned long char_height )
{
    FontInitialization.address = (unsigned long) address;
    FontInitialization.width = (unsigned long) char_width;
    FontInitialization.height = (unsigned long) char_height;
}

void fontSetCurrentFontIndex(int index)
{
    if (index < 0)
        return;
    FontInitialization.index_for_current_font = index;
}

//
// $
// INITIALIZATION
//

int font_initialize(void)
{
// Called by gwsInitGUI() in gws.c.

    register int i=0;
    //int UseThisOne = 0;
    int UseThisOne = 2; //8x16

// #todo
// #test
// Maybe we need a structure to handle the font initialization.

    FontInitialization.initialized = FALSE;
    FontInitialization.width = 8;  // Default
	FontInitialization.height = 8;  // Default

// ------------------------------------
// Font list
    for (i=0; i<FONTLIST_MAX; i++){
        fontList[i] = 0;
    };
    font00.initialized = FALSE;
    font01.initialized = FALSE;
    font02.initialized = FALSE;
    font03.initialized = FALSE;


// ------------------------------------
// font00
// Linux 8x8
    font00.address = font_lin8x8;
    font00.width = DEFAULT_FONT_WIDTH;
	font00.height = DEFAULT_FONT_HEIGHT;
    font00.initialized = TRUE;
	fontList[0] = (unsigned long) &font00;

// ------------------------------------
// font01
// Linux 8x16
    font01.address = fontdata_8x16;
    font01.width = 8;
	font01.height = 16;
    font01.initialized = TRUE;
	fontList[1] = (unsigned long) &font01;

// ------------------------------------
// font02
// Linux 8x16
    font02.address = fontdata_8x16;
    font02.width = 8;
	font02.height = 16;
    font02.initialized = TRUE;
	fontList[2] = (unsigned long) &font02;

// ------------------------------------
// font03
// Losethos 8x8
// We need to invert the data.
    font_lt8x8 = (char *) __initialize_lt8x8_font();
    if ((void*) font_lt8x8 == NULL){
        printf("font_initialize: on __initialize_lt8x8_font\n");
        goto fail;
    }
    font03.address = font_lt8x8;
    font03.width = DEFAULT_FONT_WIDTH;
	font03.height = DEFAULT_FONT_HEIGHT;
    font03.initialized = TRUE;
	fontList[3] = (unsigned long) &font03;

// ------------------------------------
// Selecting font type.
// This will be the current font.

	FontInitialization.index_for_current_font = UseThisOne;
	// #todo
	//fontSetCurrentFontIndex(UseThisOne);

    switch (UseThisOne)
    {
        case 0:
            fontSetInfo ( font00.address, font00.width, font00.height );
            break;
        case 1:
            fontSetInfo ( font01.address, font01.width, font01.height );
            break;
        case 2:
            fontSetInfo ( font02.address, font02.width, font02.height );
            break;
        case 3:
            fontSetInfo ( font03.address, font03.width, font03.height );
            break;
        default:
            fontSetInfo ( font00.address, font00.width, font00.height );
            FontInitialization.index_for_current_font = 0;
			//#todo
			//fontSetCurrentFontIndex(0);
			break;
    };

// done:
    
    FontInitialization.initialized = TRUE;
    return 0;

fail:
    // #bugbug: Hang the initialization?
    FontInitialization.initialized = FALSE;
    return (int) -1;
}

