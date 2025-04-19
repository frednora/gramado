// loader.c
// Routines to load images.
// In this file:
//   + elfLoadKernelImage: Carrega o KERNEL.BIN.
// 2015 - Created by Fred Nora.

#include "../bl.h"


// PE file header support.
//#define IMAGE_FILE_MACHINE_I386   0x014C  // x86
//#define IMAGE_FILE_MACHINE_IA64   0x0200  // Intel Itanium
//#define IMAGE_FILE_MACHINE_AMD64  0x8664  // x64
//Continua...

/*
// Progress bar support.

int total = 1000;
int step = 0;

//prot�tipos de fun��es locais.
size_t blstrlen(const char *s);
void DoProgress( char label[], int step, int total );
void updateProgressBar();
//...
*/

// elfLoadKernelImage: 
// Load /GRAMADO/KERNEL.BIN into the main memory.
// First we try the desired pathname, if it fail then we try 
// the default pathname.
// Address.
// pa = 0x00100000.
// va = 0xC0000000.
// IN:
// The desired pathname, The default pathname.
int elfLoadKernelImage(const char *pathname, const char *default_pathname)
{
// Called by blLoadKernelImage() in main.c.

    int Status = -1;
    unsigned long kernel_pa = KERNEL_ADDRESS;
    //unsigned long kernel_va = KERNEL_VA;
// Buffer. 0x00100000.
    unsigned char *kernel = (unsigned char *) kernel_pa;      
// Path
    char Path[64];
    char DefaultPath[64];

// Name

    char *image_pathname;
    image_pathname = pathname;

    char *image_default_pathname;
    image_default_pathname = default_pathname;


// Validation
    if ((void*) pathname == NULL )
        goto fail;
    if ( *pathname == 0 )
        goto fail; 

// Validation
    if ((void*) default_pathname == NULL )
        goto fail;
    if ( *default_pathname == 0 )
        goto fail; 

// Message

#ifdef BL_VERBOSE
    printf ("elfLoadKernelImage: Loading %s .. PA=%x | VA=%x \n", 
        image_pathname, kernel_pa, kernel_va );
#endif

//
// Load kernel image
//

// Given pathname
    bzero(Path,64);
    strcat(Path, image_pathname);

// Default pathname.
    bzero(DefaultPath,64);
    strcpy(DefaultPath,image_default_pathname);

// ---------------------------------------
// Load KERNEL.BIN on a physical address.
// First we try the desired pathname, if it fail then we try 
// the default pathname.
// See: fs.c


// Try the desired pathname.
    Status = (int) fs_load_path( Path, (unsigned long) kernel_pa );
// Try default pathname if it failed.
    if (Status != 0){
        printf("%s was not found\n",Path);
        printf("Let's try %s\n",DefaultPath);
        refresh_screen();
        Status = (int) fs_load_path( DefaultPath,(unsigned long) kernel_pa );
    }

// Failed again
    if (Status != 0){
        printf("elfLoadKernelImage: Couldn't load the kernel image\n");
        goto fail;
    }

// Check signature.
// Check for .ELF file signature. 
// 0x7f 0x45 0x4c 0x46 (.ELF)

    if ( kernel[0] != 0x7F || 
         kernel[1] != 'E' || kernel[2] != 'L' || kernel[3] != 'F' )
    {
        printf("elfLoadKernelImage: ELF image validation\n"); 
        goto fail;
    }

//WORD Machine.
//WORD NumberOfSections.

// #importante:
// Checando se o kernel base cont�m o header do multiboot.
// Obs: Para o Gramado Boot isso significa apenas
// mais um �tem de seguran�a, pois o Gramado Boot
// far� a inicializa��o do mesmo modo de sempre e enviar� 
// os mesmos argumentos de sempre.
// Por�m se um multiboot carregar o kernel, certamente 
// n�o passar� os mesmos argumentos que o Gramado Boot,
// ent�o o kernel inicializar� de forma diferente,
// provavelmente apenas em modo texto.
// Multiboot magic signature.
// O header est� em 0xC0001000.
// 0x1BADB002
// tem um jmp antes do header.

/*
    if ( kernel[0x1008] != 0x02 ||
         kernel[0x1009] != 0xB0 ||
         kernel[0x100A] != 0xAD || 
         kernel[0x100B] != 0x1B )
    {
        //#debug
        printf ("elfLoadKernelImage: FAIL\n");
        //refresh_screen();
        //while(1){}
    }
*/

// Continua ...

// Done.
// The kernel image was loaded.

    return 0; 

// Couldn't load the kernel image.
// Let's return and give to the main routine the chance to 
// load from another place.
fail:
    printf("elfLoadKernelImage: Fail\n");
    refresh_screen();
    return (int) (-1);
}

/*
// local
// strlen:
//     Tamanho de uma string.
size_t blstrlen(const char *s)
{
    size_t i = 0;
    for(i = 0; s[i] != '\0'; ++i)
    {
    ; 
    };
    return ( (size_t) i );
};
*/

/*
// local
// DoProgress:
//     Credits: Progress bar source code found on 
//     codeproject.com/Tips/537904/Console-simple-progress 
//
void DoProgress( char label[], int step, int total )
{
    //progress width
    const int pwidth = 72;

    //minus label len
    int width = pwidth - blstrlen( label );
    int pos = ( step * width ) / total ;
   
    int percent = ( step * 100 ) / total;

    //set green text color, only on Windows
    //SetConsoleTextAttribute(  GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN );
    printf("%s[", label);

//fill progress bar with =
    int i;
    for( i = 0; i < pos; i++ ){
        printf("%c", '=');
    };

    //fill progress bar with spaces
    printf("% *c", width - pos + 1);
    printf("] %3d%%\r", percent);

//reset text color, only on Windows
    //SetConsoleTextAttribute(  GetStdHandle( STD_OUTPUT_HANDLE ), 0x08 );

    return;
};
*/

/*
void updateProgressBar()
{
    step += 1;
    DoProgress("Loading: ",step,total);
    refresh_screen();
}
*/

//
// End
//

