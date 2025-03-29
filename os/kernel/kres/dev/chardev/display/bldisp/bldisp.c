// bldisp.c
// Bootloader display device device driver.
// Created by Fred Nora.

// Rotinas paga gerenciar os controladores de video.
// Crt, Vga ...
// See:
// https://wiki.osdev.org/Accelerated_Graphic_Cards
// # importante
// Essa deve ser a única maneira em que o sistema acessa
// o controlador de vídeo.
// Por aqui o sistema fará configurações no controlador
// e enviará dados para o lfb.

//
// == VGA =============================
//

/*
    This index lists the VGA's I/O ports in numerical order, 
    making looking up a specific I/O port access simpler.
 
    3B4h -- CRTC Controller Address Register
    3B5h -- CRTC Controller Data Register
    3BAh Read -- Input Status #1 Register
    3BAh Write -- Feature Control Register
    3C0h -- Attribute Address/Data Register
    3C1h -- Attribute Data Read Register
    3C2h Read -- Input Status #0 Register
    3C2h Write -- Miscellaneous Output Register
    3C4h -- Sequencer Address Register
    3C5h -- Sequencer Data Register
    3C7h Read -- DAC State Register
    3C7h Write -- DAC Address Read Mode Register
    3C8h -- DAC Address Write Mode Register
    3C9h -- DAC Data Register
    3CAh Read -- Feature Control Register
    3CCh Read -- Miscellaneous Output Register
    3CEh -- Graphics Controller Address Register
    3CFh -- Graphics Controller Data Register
    3D4h -- CRTC Controller Address Register
    3D5h -- CRTC Controller Data Register
    3DAh Read -- Input Status #1 Register
    3DAh Write -- Feature Control Register

See:
    https://web.stanford.edu/class/cs140/projects/pintos/specs/freevga/vga/portidx.htm
 */
// ================================

#include <kernel.h>

// Screen sizes and bpp.
unsigned long g_device_screen_width=0;
unsigned long g_device_screen_height=0;
unsigned long g_device_screen_bpp=0;

int refresh_device_screen_flag=0;
int refresh_valid_screen_flag=0;

unsigned long screen_size_in_kb=0;
// hack: for debuging purpose
int g_use_fake_screen_size=0;
unsigned long fake_screen_size_in_kb=0;

// The refresh is enabled we mapped
// memory enough for that.
int refresh_screen_enabled=0;
// We need to flush the whole screen into the frontbuffer.
int screen_is_dirty=0;

unsigned long gSavedLFB=0;
unsigned long gSavedX=0;
unsigned long gSavedY=0;
unsigned long gSavedBPP=0;


// Private variables.
// Display device support.
static unsigned long screen_width=0;
static unsigned long screen_height=0;
static unsigned long screen_bpp=0;
static unsigned long screen_pitch=0;
//...

// The info comes from the gramado boot loader.
struct display_device_d *bl_display_device;


//@todo: Criar um arquivo para gerenciar fontes.
#define VIDEO_BIOS_FONT8X8_ADDRESS    0x000FFA6E
//#define VIDEO_BIOS_FONT8X16_ADDRESS (0x000FFA6E+??)
//...
#define VIDEO_BIOS_FONT8X8_WIDTH   8
#define VIDEO_BIOS_FONT8X8_HEIGHT  8
//...




// LFB - Esse é o endereço usado pelo driver de vídeo em /x
// para acessar o LFB, ou seja o frontbuffer.
// Lembrando que o driver de vídeo deve ser independente do 
// kernel então o kernel só poderá obter esse endereço se
// solicitar par asse driver.
// Mas o kernel não deve enviar dados para o frontbuffer,
// para isso ele deve usar esse driver.
// #importante: Esse driver poderá ser diferente para
// cada modelo de placa de vídeo.


// private:
// We gotta build methods to get these values
// from outside.

static unsigned long __frontbuffer_va=0;
static unsigned long __frontbuffer_pa=0;
static unsigned long __cga_va=0;
static unsigned long __cga_pa=0;

#define VSYNC_INPORT  0x3DA

//
// == Private functions: Prototypes ========
//

static void __bldisp_flush_into_lfb(unsigned long flags);

static int __videoInit(void);
static char vsync_inb(int port);


// ============================

/*
 * __bldisp_flush_into_lfb:
 * Coloca o conteúdo do BackBuffer no LFB da memória de vídeo.
 * Ok, isso está no lugar certo. Pois somente o driver display device  
 * pode acessar o LFB.
 */

// backup.
// This is working very well.
// Copying long by long.
// ?? 
// Total size is wrong.
// Esse é mais rápido.
// #bugbug
// #danger
// Dependendo do tamanho da tela, 
// essa rotina pode acabar copiando conteúdo 
// para alguma parte não mapeada no lfb.
// Precisamos de limites. Obedecendo a quantidade mapeada.
// #bugbug
// Como temos apenas 2MB de LFB mapeados, 
// então vamos copiar menos dados pra evitar ultrapassar o limite
// e causar PF.
// #todo
// We can also use 16 bytes ... SSE?

static void __bldisp_flush_into_lfb(unsigned long flags)
{

// #option
// Use this one. It is working fine.
    //refresh_rectangle( 0, 0, gSavedX, gSavedY ); 

// Loop
    register int i=0;
// Char. The slow way.
    unsigned char *backbuffer  = (unsigned char *) BACKBUFFER_VA;
    unsigned char *frontbuffer = (unsigned char *) FRONTBUFFER_VA;
// Long. The fast way. (8bytes)
    unsigned long *backbuffer_long  = (unsigned long *) BACKBUFFER_VA;
    unsigned long *frontbuffer_long = (unsigned long *) FRONTBUFFER_VA;

// We can't refresh.
// The buffer wasn't validated yet.
// #todo: Check this into the display device structure.
// see: init.c
    if (refresh_screen_enabled != TRUE){
        debug_print_string("fb_refresh_screen: [FAIL-FIXME] refresh_screen_flag\n");
        return;
    }

// Invalid screen parameters.
// #todo: Check these into the display device structure.
    if ( gSavedX == 0 || gSavedY == 0 || gSavedBPP == 0 )
    {
        panic("fb_refresh_screen: display info\n");
    }


// #
// Provisório, até termos condições de usar mais memória.
// Clipping:
// Podemos recortar, e mesmo que a resoluçao seja alta,
// somente escreveremos nos primeiros 2MB por enquanto.
// #todo
// Também podemos salvar esse tipo de valor na 
// estrutura de display device.


// Total number of bytes in the screen.
// see: init.c
    int TotalInBytes = (int) (screen_size_in_kb * 1024);

/*
// nao pode ser mais que 2mb
    //if ( Total >= (2*1024*1024) )
    if (Total >= 2097152 ){
        debug_print_string("fb_refresh_screen: [FAIL-FIXME] Total\n");
        return;
    }
*/
// Se algo deu errado 
// e realmente precisamos de alguma tela
// para debug na inicializaçao.
// entao vamos limitar o tamanho do refresh screen
// isso ajuda no refresh screen, mas nao em outras rotinas graficas.
    if (g_use_fake_screen_size == TRUE)
    {
        //fake_screen_size_in_kb = (( 320*4*200 )/1024);
        TotalInBytes = (int) (fake_screen_size_in_kb * 1024);
    }

// 800*3*600 = 1440000
// 800*4*600 = 1920000
// Não pode ser mais que 2MB.
// 2048 * 1024 = 2097152.
    if (TotalInBytes >= 2097152){
        debug_print_string("fb_refresh_screen: [FAIL-FIXME] Total\n");
        return;
    }

//
// Refresh
//

// #todo: Suspend this.
// Vertical synchonization?
//  if(flags & 1)
    bldisp_vsync();

// Refresh
    refresh_rectangle( 0, 0, gSavedX, gSavedY );
}   

// Flush the content of the backbuffer
// into the LFB.
void bldisp_flush(unsigned long flags)
{
    __bldisp_flush_into_lfb(flags);
}

// vsync_inb: 
// Pega um byte na porta. 
static char vsync_inb(int port)
{
    char Value = 0;

    Value = (char) in8(port); 

    asm (" nop \n");
    asm (" nop \n");
    asm (" nop \n");
    asm (" nop \n");

	//#todo: io_delay();

    return (char) Value;    
}

// ===============================

// Called by hal_vsync
void bldisp_vsync(void)
{

	// Método 1
	//================================
	
	// while ( ( vsync_inb (0x3DA) & 8 ) != 8 );
	// while ( ( vsync_inb (0x3DA) & 8 ) == 8 );
	// return;
	
	// Método 2
	//================================
	
	// Checar se vsync foi gerado.

    unsigned long MaxDelay=0;

    out8( 0x3c4, 0 );

    if ( vsync_inb(0x3c5) & 0x2 ) 
    {
        MaxDelay = 100000;
        while ( ( ( vsync_inb (0x3DA) & 8 ) == 8 ) && MaxDelay-- );

        MaxDelay = 100000;
        while ( ( ( vsync_inb (0x3DA) & 8 ) == 0  ) && MaxDelay-- );
    }
}

/*
 * bldisp_putpixel0:
 *     Ok. 
 *     O servidor kgws pode acessar um buffer. Mas não tem acesso
 * ao frontbuffer. Para isso ele precisa usasr o diálogo do driver 
 * de vídeo.
 * IN: 
 *     color, x, y, rop_flags
 */
// #todo
// + Change the names of these parameters.
// + Create a parameter for the address of the buffer.
// Colors:
// b,   g,  r,  a = Color from parameter.
// b2, g2, r2, a2 = Color from backbuffer.
// b3, g3, r3, a3 = Color to be stored.

int 
bldisp_putpixel0 ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags,
    unsigned long buffer_va )
{
    unsigned char *where = (unsigned char *) buffer_va;
    unsigned int Color = (unsigned int) (_color & 0xFFFFFFFF);
// ----------------------------
// A cor passada via argumento.
    char b, g, r, a;
    b = (Color & 0xFF);
    g = (Color & 0xFF00) >> 8;
    r = (Color & 0xFF0000) >> 16;
    a = (Color >> 24) + 1;

// The first byte;
// 0 ~ FF
    int Operation = (int) (_rop_flags & 0xFF);

// Positions
    int x = (int) (_x & 0xFFFF);
    int y = (int) (_y & 0xFFFF);
// 3 = 24 bpp
// 2 = 16 bpp
// ...
    int bytes_count=0;

// Buffer address validation.
    if (buffer_va == 0){
        panic("putpixel0: buffer_va\n");
    }

//
// bpp
//

// #danger
// This is a global variable.
// Esse valor foi herdado do bootloader.

    switch (gSavedBPP){
    case 32:  bytes_count=4;  break;
    case 24:  bytes_count=3;  break;
    //#testando
    //case 16:
    //    bytes_count = 2;
    //    break;
    //case 8:
    //    bytes_count = 1;
    //    break;
    default:
        //#todo: Do we have panic ar this moment?
        //panic ("putpixel0: gSavedBPP\n");
        debug_print_string("putpixel0: gSavedBPP\n");
        while(1){}
        break;
    };

// #importante
// Pegamos a largura do dispositivo.
    int width = (int) (gSavedX & 0xFFFF);
    width = (int) (width & 0xFFFF);

    int offset=0;
    //int offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );

// Offset.

// 32bpp
    if (bytes_count==4){
        offset = (int) ( ((width<<2)*y) + (x<<2) );
    }
// 24bpp
    if (bytes_count==3){
        offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );
    }
// 16bpp
    //if (bytes_count==2){
    //    offset = (int) ( ((width<<1)*y) + (x<<1) );
    //}


//
// == Modify ==============================
//

// ------------------------------------------
// A cor que estava no framebuffer.
    unsigned char b2, g2, r2, a2;
// Get yhe color.
    b2 = where[offset];
    g2 = where[offset +1];
    r2 = where[offset +2];
    if (gSavedBPP == 32){ a2 = where[offset +3]; };

// ------------------------------
// A cor transformada.
// A cor a ser gravada.
    unsigned char b3, g3, r3, a3;
// -------------------------
// 0 - Sem modificação
// A cor a ser registrada é a mesma enviada por argumento.
    if (Operation == 0){
        r3=r; g3=g; b3=b; a3=a;
    }
// -------------------------
// 1 = or
    if (Operation == 1)
    {
        r3 = (r2 | r);
        g3 = (g2 | g);
        b3 = (b2 | b);
        a3 = a2;
    }
// -------------------------
// 2 = and
    if (Operation == 2)
    {
        r3 = (r2 & r);
        g3 = (g2 & g);
        b3 = (b2 & b);
        a3 = a2;
    }
// -------------------------
// 3 = xor
    if (Operation == 3)
    {
        r3 = (r2 ^ r);
        g3 = (g2 ^ g);
        b3 = (b2 ^ b);
        a3 = a2;
    }
// -------------------------
// 4 - nand? #text
    if (Operation == 4)
    {
        r2 = ~r2;
        g2 = ~g2;
        b2 = ~b2;
        r3 = (r2 & r);
        g3 = (g2 & g);
        b3 = (b2 & b);
        a3 = a2;
    }

// -------------------------
// 10 - less red
    if (Operation == 10)
    {
        r3 = (r2 & 0xFE);
        g3 = g2;
        b3 = b2; 
        a3 = a2;
    }
// -------------------------
// 11 - less green
    if (Operation == 11)
    {
        r3 = r2;
        g3 = (g2 & 0xFE);
        b3 = b2; 
        a3 = a2;
    }
// -------------------------
// 12 - less blue
    if (Operation == 12)
    {
        r3 = r2;
        g3 = g2;
        b3 = (b2 & 0xFE); 
        a3 = a2;
    }
// -------------------------
// 20 - gray
    if (Operation == 20)
    {
        r3 = (r2 & 0x80);
        g3 = (g2 & 0x80);
        b3 = (b2 & 0x80);
        a3 = a2;
    }
// -------------------------
// 21 - no red 
    if (Operation == 21)
    {
        r3 = (r2 & 0x00);
        g3 = (g2 & 0xFF);
        b3 = (b2 & 0xFF);
        a3 = a2;
    }

// Luminosity
// Gray: luminosity = R*0.3 + G*0.59 + B *0.11

/*
 // #test
 // This is a test yet.
    unsigned char common_gray=0;
    if (Operation == 22)
    {
        r3 = ((r2 * 30 )/100);
        g3 = ((g2 * 59 )/100);
        b3 = ((b2 * 11 )/100);
        common_gray = (unsigned char) (r3+g3+b3);
        r3=(unsigned char)common_gray;
        g3=(unsigned char)common_gray;
        b3=(unsigned char)common_gray;
        a3 = a2;
    }
*/


//
// == Register =====================
// 

// ----------------------------
// BGR and A
    where[offset]    = b3;
    where[offset +1] = g3;
    where[offset +2] = r3;
    if (gSavedBPP == 32){ where[offset +3] = a3; };

// Number of changed pixels.
    return (int) 1;
}




// Qual é o endereço virtual do início de uma dada linha da tela?
unsigned long screen_scanline_va(int scanline)
{
    unsigned long scanline_va=0;
    unsigned long start = FRONTBUFFER_VA;

// pitch
    unsigned long pitch = (unsigned long) screenGetHeight();
    if ( pitch == 0 ){
        panic ("screen_scanline_va: pitch limits");
    }

// height
    unsigned long height = (unsigned long) screenGetHeight();
    if ( height == 0 ){
        panic ("screen_scanline_va: height limits");
    }

    if (scanline >= height){
        panic ("screen_scanline_va: scanline limits");
    }

    scanline_va = (unsigned long) ( start + (pitch * scanline) );

// É o endereço virtual do início de uma dada linha da tela.
    return (unsigned long) scanline_va;
}

// Get screen width
unsigned long screenGetWidth (void)
{
    return (unsigned long) screen_width;
}

// Get screen height
unsigned long screenGetHeight (void)
{
    return (unsigned long) screen_height;
}

// Get screen bpp
unsigned long screenGetBPP (void)
{
    return (unsigned long) screen_bpp;
}

// Get screen pitch
unsigned long screenGetPitch (void)
{
    return (unsigned long) screen_pitch;
}


// screenSetSize:
//     Configura as dimensões da tela.
//     Tamanho do monitor.

void screenSetSize( unsigned long width, unsigned long height )
{
    screen_width  = (unsigned long) width;
    screen_height = (unsigned long) height;
}

// The whole screen is dirty.
// It can be flushed into the framebuffer.
void invalidate_screen(void)
{
    screen_is_dirty = TRUE;
}

/*
 * screenInit:
 *     Inicializando o gerenciamento de tela.
 */ 
// #bugbug
// Screen is a reagion in the display, or in many displays.
// Display is a monitor, or a set o hid in a given host.

int screenInit(void)
{
    debug_print ("screenInit:\n");

// Validate the whole screen
// We don't need to flush the screen yet.
    screen_is_dirty = FALSE;

// Configura globais com base nos valores passados pelo Boot Loader.
    screenSetSize(
        (unsigned long) gSavedX, 
        (unsigned long) gSavedY );

    screen_bpp = gSavedBPP;
// Pitch: bytes per line.
// Ex: 
// bytes per pixel = ((32/8)*800)
    screen_pitch = 
        (unsigned long) ( (gSavedBPP/8) * gSavedX );
    //...
    return 0;
}

void bldisp_show_info(void)
{
// #todo
// We need a generic worker to display information
// about a given display device.

    if ((void*) bl_display_device == NULL)
        return;
    if (bl_display_device->magic != 1234)
        return;
    if (bl_display_device->initialized != TRUE)
        return;
// Print
    printk ("Width: %d | Height: %d | BPP: %d \n",
        bl_display_device->framebuffer_width,
        bl_display_device->framebuffer_height,
        bl_display_device->framebuffer_bpp );
    printk ("Screen size: %d KB\n",
        bl_display_device->framebuffer_size_in_kb );
}

/*
 * videoGetMode: 
 *     Obtem o modo de video atual.
 */
// #todo
// Isso pode ser incluido em 'get system parameters' system call.
unsigned long videoGetMode(void)
{
    return (unsigned long) g_current_video_mode;
}

/*
 * videoSetMode:
 *     Configura o modo de video atual.
 */
void videoSetMode(unsigned long mode)
{
    unsigned long VideoMode=0;
    unsigned long Width=0;
    unsigned long Height=0;
    //continua...(outros parametros)

    debug_print ("videoSetMode: [FIXME] This routine is wrong\n");

    VideoMode = (unsigned long) mode;

// todo: 
// Check limits.

    //if (){}

// ??
// Se estiver nos limites.
    if ( VideoMode > 0 && 
         VideoMode < 9000 )
    {
        //g_current_video_mode = (unsigned long) VideoMode;
        g_video_mode = 
            (unsigned long) VideoMode;
        VideoBlock.vesaMode = 
            (unsigned long) VideoMode; 
        //...
    }

// #todo:
// Organizar isso. Os valores atuais devem ser obtidos em real mode 
// usando os recursos do VESA BIOS.

// #bugbug
// This is is very wrong.
// We need to get these values in the controller.

    switch (VideoMode){

        case 0x110: 
            Width = 800;
            Height = 600;
            break;

        case 0x111:
            Width = 800;
            Height = 600;
            break;

        case 0x112:
            Width = 800;
            Height = 600;
            break;

        case 0x113:
            Width = 800;
            Height = 600;
            break;

        case 0x114:
            Width = 800;
            Height = 600;
            break;

        case 0x115:
            Width = 800;
            Height = 600;
            break;

        default:
            Width = 800;
            Height = 600;
            break;
    };

    screenSetSize (Width,Height);

    // Continua... (outros parametros)

    g_current_video_mode = 
        (unsigned long) VideoMode;
}

/*
 * videoSetupCGAStartAddress:
 *     Configura o endereço inicial da memória de video em modo texto   
 *     fis=b8000  vir=??   #todo
 */
void videoSetupCGAStartAddress (unsigned long address)
{
    g_current_vm = (unsigned long) address;
    //g_current_cga_address
}


// #todo ioctl
int 
bldisp_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    debug_print("bldisp_ioctl: #todo\n");

// Parameters:
    if ( fd < 0 || fd >= OPEN_MAX ){
        return (int) (-EBADF);
    }

    switch (request){
    //case ?:
        //break;
    default:
        return (int) (-EINVAL);
        break;
    };

fail:
    return (int) -1;
}


static int __videoInit(void)
{
    int Status=0;

// Se o modo de video nao esta habilitado
// Can we use the panic function?
// No we can't.
    if (VideoBlock.useGui != TRUE)
    {
        debug_print("__videoInit:\n");
        //panic("videoInit:\n");
        soft_die();
    }
    VideoBlock.useGui = TRUE;


// Frontbuffer (LFB)
    __frontbuffer_pa = (unsigned long) gSavedLFB;
    __frontbuffer_va = (unsigned long) FRONTBUFFER_VA;
    g_frontbuffer_pa = (unsigned long) __frontbuffer_pa; 
    g_frontbuffer_va = (unsigned long) __frontbuffer_va;

// Backbuffer
     g_backbuffer_va  = (unsigned long) BACKBUFFER_VA;

// Device screen sizes. 
// (herdadas do boot loader.)
// See: 
// admin/config/superv/gdevice.h
    g_device_screen_width  = (unsigned long) gSavedX;
    g_device_screen_height = (unsigned long) gSavedY;
    g_device_screen_bpp    = (unsigned long) gSavedBPP;

    // gwsSetCurrentFontAddress ( VIDEO_BIOS_FONT8X8_ADDRESS );

// #todo: 
// Usar a função que configura essas variáveis.
    FontInitialization.width  = VIDEO_BIOS_FONT8X8_WIDTH;
    FontInitialization.height = VIDEO_BIOS_FONT8X8_HEIGHT;
    FontInitialization.font_size = FONT8X8;

    return 0;
    //return (int) Status;    
}

//
// $
// INITIALIZATION
//

int Video_initialize(void)
{
    g_driver_video_initialized = FALSE;
    __videoInit();
    // ...
    g_driver_video_initialized = TRUE;
    return 0;
}

//==============================

//
// $
// INITIALIZATION
//


// DDINIT_bldisp:
// Device driver initialization
// Initialize the device driver for the bootloader display device.
int DDINIT_bldisp(void)
{
// Called by keInitialize() in ke.c.

    PROGRESS("DDINIT_bldisp:\n");

// Memory allocation for Display device structure.
    bl_display_device = 
        (struct display_device_d *) kmalloc( sizeof(struct display_device_d) ); 
    if ((void*) bl_display_device == NULL){
        x_panic ("Error: 0x05");
    }
    memset ( bl_display_device, 0, sizeof(struct display_device_d) );

// xBootBlock was initialized by init.c
    if (xBootBlock.initialized != TRUE){
        x_panic ("DDINIT_bldisp: xBootBlock");
    }

// Structure initialization.
    bl_display_device->initialized = FALSE;
    bl_display_device->owner_pid = (pid_t) GRAMADO_PID_KERNEL;

// framebuffer address.
    bl_display_device->framebuffer_pa = (unsigned long) xBootBlock.lfb_pa;
    bl_display_device->framebuffer_va = (unsigned long) FRONTBUFFER_VA;

// w, h, bpp.
    bl_display_device->framebuffer_width  = 
        (unsigned long) xBootBlock.deviceWidth;
    bl_display_device->framebuffer_height = 
        (unsigned long) xBootBlock.deviceHeight;
    bl_display_device->framebuffer_bpp = 
        (unsigned long) xBootBlock.bpp;
// Pitch
// Ex: ((32/8)*800)
    bl_display_device->framebuffer_pitch = 
        (unsigned long) ( (xBootBlock.bpp/8) * xBootBlock.deviceWidth);

// Screen size in bytes.
    bl_display_device->framebuffer_size_in_bytes =
        (unsigned long) ( bl_display_device->framebuffer_height * 
                          bl_display_device->framebuffer_pitch );

// Screen size in KB.
    bl_display_device->framebuffer_size_in_kb =
        (unsigned long) (bl_display_device->framebuffer_size_in_bytes / 1024);

// #test
// The main virtual screen for this device.
// Relative values.
    bl_display_device->virtual_screen.left = (unsigned long) 0;
    bl_display_device->virtual_screen.top  = (unsigned long) 0;
    bl_display_device->virtual_screen.width = 
        (unsigned long) bl_display_device->framebuffer_width;
    bl_display_device->virtual_screen.height = 
        (unsigned long) bl_display_device->framebuffer_height;

    bl_display_device->next = NULL;

// validation
    bl_display_device->used = TRUE;
    bl_display_device->magic = 1234;
// Structure initialization.
    bl_display_device->initialized = TRUE;

    return 0;
}


//
// End
//

