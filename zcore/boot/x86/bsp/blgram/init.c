// init.c
// Bootloader initialization support.
// 2015 - Created by Fred Nora.

#include "bl.h"  


// Variaveis herdadas do boot manager
// O modo de boot. gui or cli.
extern unsigned long SavedBootMode;
// Endere�o do boot block
extern unsigned long SavedBootBlock;   
// device info
//extern unsigned long SavedLFB;
//extern unsigned long SavedX;
//extern unsigned long SavedY;
//extern unsigned long SavedBPP;

// -----------------------------------------------
static void __fill_the_bootblock_for_32bit_mode(void);

// -----------------------------------------------

static void __fill_the_bootblock_for_32bit_mode(void)
{
// #warning
// Remember, we are still in 32bit mode.

//
// == boot block ===========================
//

// Vamos pegar as informa�oes no boot block passado pelo
// BM e salvarmos na estrutura no BL.
// Eh melhor copiar do que simplesmente fazer referencia.
// Vamos copiar em ordem.
// See: gdef.h
// BootBlock was defined in main.c
// see the structure in gdef.h.

// #warning
// The 64bit entries version was created by head.s
// in the address 0x00090000. The kernel used this one created by head.s.

    unsigned long *base = (unsigned long *) SavedBootBlock;

    // 32bit size.
    BootBlock.lfb                = (unsigned long) base[0];  //  0
    BootBlock.x                  = (unsigned long) base[1];  //  4
    BootBlock.y                  = (unsigned long) base[2];  //  8
    BootBlock.bpp                = (unsigned long) base[3];  // 12
    BootBlock.last_valid_address = (unsigned long) base[4];  // 16
    BootBlock.metafile_address   = (unsigned long) base[5];  // 20
    BootBlock.disk_number        = (unsigned long) base[6];  // 24
    BootBlock.heads              = (unsigned long) base[7];  // 28
    BootBlock.spt                = (unsigned long) base[8];  // 32 
    BootBlock.cylinders          = (unsigned long) base[9];  // 36
    BootBlock.boot_mode          = (unsigned long) base[10]; // 40
    BootBlock.gramado_mode       = (unsigned long) base[11]; // 44

// #debug
// vamos mostrar as informa�oes do boot block

}


// set_up_color:
// Configura cor padr�o para o sistema.
void set_up_color(unsigned long color)
{   
    g_system_color = (unsigned long) color;
}

// ??
void 
set_up_text_color ( 
    unsigned char forecolor, 
    unsigned char backcolor )
{
    g_char_attrib = (backcolor << 4) | (forecolor & 0x0F);
}


/*
 * init_globals:
 *     Inicia vari�veis globais.
 *     @Mudar para BlInitGlobals();
 *     o retorno por ser int.
 */
//void BlInitGlobals() 
void init_globals(void)
{
// Pr�ximo procedimento, status and file system type.
// 1=fat16.
    g_next_proc = (unsigned long) bl_procedure;
    g_proc_status = 0;
    g_file_system_type = 1;
    //...
// Procedure structure. 
// @todo Check.
    procedure_info.next = (unsigned long) bl_procedure;
    procedure_info.status = 0;
    //...
}


/*
 * init:
 *     Fun��o principal do arquivo init.c.
 */
// Called by OS_Loader_Main in main.c
//int init (int ?){ 
int init(void)
{
    g_cursor_x = 0;
    g_cursor_y = 0;

// Inicializando o v�deo.
// #todo: 
// Inicializar estruturas de v�deo.
// Selecionando modo de v�deo.

    if (SavedBootMode == 1){
        VideoBlock.useGui = TRUE;
    }else{
        VideoBlock.useGui = FALSE;
    };

//#ifdef BL_VERBOSE
    //Debug Message.
    //Se estivermos usando modo gr�fico (GUI.)	
    if (VideoBlock.useGui == TRUE)
    {
        // #importante
        // Essa � a primeira mensagem, ela aparece nos modos 0x115 e 0x118
        //printf("init: Using GUI\n");
    }
//#endif

// Se n�o estivermos usando GUI.(text mode).
// Limpa a tela.
// Branco em preto.

    if (VideoBlock.useGui != TRUE)
    {
        bl_clear(0);
        set_up_text_color (0x0F, 0x00);
        printf ("BL.BIN-init: Text Mode\n");
        refresh_screen();
        //#hang
        while(1){
            asm("cli");
        };
    }

//
// Inicializando o Boot Loader.
//

//T�tulo.
//#ifdef BL_VERBOSE
    printf ("BL.BIN: Initializing ...\n");
    // printf ("init: Boot Loader Version %s \n", BL_VERSION );	
    refresh_screen();
//#endif

    //globais.	
//#ifdef BL_VERBOSE	
	//printf("init: Globals..\n");
//#endif	
    init_globals();

// The boot block
// Fill a 32bit version of the boot block into a structure.
// #warning
// The 64bit entries version was created by head.s
// in the address 0x00090000. The kernel used this one created by head.s.

    __fill_the_bootblock_for_32bit_mode();

    // OK
    //printf ("Gramado mode %d\n",BootBlock.gramado_mode);

    //refresh_screen();
    //while(1){}

    fsInit();       // File system.
    shellInit();    // Embedded shell.
    blTimerInit();  // Timer.

// Type:
//     CD, HD, PXE, FLASH, FAT16 ...

/*
	switch(BootType)
	{
	    case BOOT_TYPE1:
            break; 		
		case BOOT_TYPE1:
            break; 		
	    case BOOT_TYPE1:
            break;
        //...
		default:
		    //printf("init: Boot type unknow!\n");
            //BlAbort();
            break;
	};
*/


// Inicializar os segmentos do sistema.(GDT).
// (data e code)

// #todo:
// Pega o valor herdado do Boot Manager.
    // LegacyCR3 = (unsigned long) GetCR3();

// Continua?

    g_initialized = (int) TRUE;
    return 0;  
}

//
// End
//

