// font.c
// ring0
// Created by Fred Nora.

#include <kernel.h>

struct font_initialization_d  FontInitialization;


//
// ==================================================
//


int gwsGetCurrentFontCharWidth (void)
{
    return (int) get_char_width();
}

void gwsSetCurrentFontCharWidth (int width)
{
    set_char_width (width);
}

int gwsGetCurrentFontCharHeight (void)
{
    return (int) get_char_height();
}

void gwsSetCurrentFontCharHeight (int height)
{
    set_char_height (height);
}

unsigned long fontGetCurrentAddress(void)
{
    return (unsigned long) FontInitialization.address;
}

// Set the base address for the current font.
void fontSetCurrentAddress(unsigned long address)
{
    FontInitialization.address = (unsigned long) address;
}

// gwsInstallFont:
// Carregando e instalando uma fonte dado o nome.
int gwsInstallFont(char *file_name)
{
// Load a font from the disk.
// # Not in use.

    unsigned long fileret=0;

// #todo #bugbug
// Rever esse tamanho.

    unsigned long tmp_size = (8*4096);
    void *font_buffer = (void *) allocPages(8);

    if ((void *) file_name == NULL){
        panic("gwsInstallFont: file_name\n");
    }
    if ( *file_name == 0 ){
        panic("gwsInstallFont: *file_name\n");
    }
    if ((void *) font_buffer == NULL){
        panic("gwsInstallFont: font_buffer\n");
    }

// Load file from root dir.
// #bugbug: 
// (FAT16_ROOT_ENTRIES) Number of entries.

    unsigned long dir_address = VOLUME1_ROOTDIR_ADDRESS;
    int number_of_entries = FAT16_ROOT_ENTRIES;

    fileret = 
        (unsigned long) fsLoadFile ( 
                            (unsigned long) VOLUME1_FAT_ADDRESS, 
                            (unsigned long) dir_address, 
                            (int) number_of_entries, 
                            (const char *) file_name, 
                            (unsigned long) font_buffer,
                            (unsigned long) tmp_size );

    if (fileret != 0){
        panic ("gwsInstallFont: fileret\n");
    }

// Set the address for the current font.
    unsigned long base_address = (unsigned long) (font_buffer + 0x2000);
    fontSetCurrentAddress(base_address);

    return 0;
}

//
// $
// INITIALIZATION
//

// BIOS 8x8 font.
int font_initialize(void)
{
// Called by zero_initialize_default_kernel_font() in zero.c.

    // Old way (#todo: Delete this)
    fontSetCurrentAddress(BIOSFONT8X8);
    gwsSetCurrentFontCharWidth(8);
    gwsSetCurrentFontCharHeight(8);

    // New way
    // Using our new global structure.
    FontInitialization.address = BIOSFONT8X8;
    FontInitialization.width = 8;
    FontInitialization.height = 8;
    FontInitialization.font_size = FONT8X8;
    FontInitialization.initialized = TRUE;

    return 0;
}

//
// End
//













