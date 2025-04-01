// ps2mouse.c
// ps/2 mouse support.
// ring0, kernel base.
// Created by Fred Nora.

#include <kernel.h>

// see: ps2mouse.h
struct ps2_mouse_d  PS2Mouse;

/*
 #todo
 enable mouse?
 See: temple os
    if (*0x40E(U16 *) == 0x9FC0)
    {
        _b = 0x9FC00 + 0x30;
        *_b = 1;    
    }
*/

#define I8042_STATUS 0x64
#define I8042_BUFFER_FULL 0x01
#define I8042_WHICH_BUFFER 0x20
#define I8042_KEYBOARD_BUFFER 0x00
#define I8042_MOUSE_BUFFER    0x20

// Flags in the first byte.
#define MOUSE_FLAGS_Y_OVERFLOW     0x80
#define MOUSE_FLAGS_X_OVERFLOW     0x40
#define MOUSE_FLAGS_Y_SIGN         0x20
#define MOUSE_FLAGS_X_SIGN         0x10
#define MOUSE_FLAGS_ALWAYS_1       0x08
#define MOUSE_FLAGS_MIDDLE_BUTTON  0x04
#define MOUSE_FLAGS_RIGHT_BUTTON   0x02
#define MOUSE_FLAGS_LEFT_BUTTON    0x01

// ??
// mouseHandler support
#define MOUSE_DATA_BIT  1
#define MOUSE_SIG_BIT   2
#define MOUSE_F_BIT     0x20
#define MOUSE_V_BIT     0x08

#define __PS2MOUSE_SET_DEFAULTS      0xF6
#define __PS2MOUSE_SET_RESOLUTION    0xE8

/*
// Commands?
#define PS2MOUSE_SET_RESOLUTION            0xE8
#define PS2MOUSE_STATUS_REQUEST            0xE9
#define PS2MOUSE_REQUEST_SINGLE_PACKET     0xEB
#define PS2MOUSE_GET_DEVICE_ID             0xF2
#define PS2MOUSE_SET_SAMPLE_RATE           0xF3
#define PS2MOUSE_ENABLE_PACKET_STREAMING   0xF4
#define PS2MOUSE_DISABLE_PACKET_STREAMING  0xF5
#define PS2MOUSE_SET_DEFAULTS              0xF6
#define PS2MOUSE_RESEND                    0xFE
#define PS2MOUSE_RESET                     0xFF
*/

// ID support:
// #define PS2MOUSE_INTELLIMOUSE_ID 0x03
// ...

// Screen width and height
extern unsigned long SavedX; 
extern unsigned long SavedY;

// packet
static unsigned char mouse_packet_data = 0;  // flags
static char mouse_packet_x = 0;              // delta x
static char mouse_packet_y = 0;              // delta y
static char mouse_packet_scroll = 0;         // z

// packet support
static int mouse_stage = 0;  // stages.
static char buffer_mouse[4];

// mouse buttons support
unsigned int mbuttons_old_state[5];

static long mouse_x=0;
static long mouse_y=0;
static long saved_mouse_x=0;
static long saved_mouse_y=0;

static int ps2_mouse_moving=0;

static void __ps2mouse_parse_data_packet(void);
static void __initialize_mouse_position(void);

// ----------------------------------------------------

// zzz_mouse_read:
// Pega um byte na porta 0x60.
unsigned char i8042_mouse_read (void)
{
    unsigned char Value=0;

    //debug_print ("i8042_mouse_read:\n");

    //debug_print ("[1]:\n");
    prepare_for_input();
    //debug_print ("[2]:\n");
    Value = (unsigned char) in8(0x60);

    //debug_print ("i8042_mouse_read: Done\n");
    return (unsigned char) Value;
}

// Esta função será usada para escrever dados 
// do mouse na porta 0x60, fora do IRQ12
void i8042_mouse_write(unsigned char data)
{
    prepare_for_output();
    out8(0x64,0xD4);
    prepare_for_output();
    out8(0x60,data);
}

void i8042_mouse_expect_ack (void)
{
    // #bugbug
    // ? loop infinito  
    // while ( xxx_mouse_read() != 0xFA );

    unsigned char ack_value=0;
    int timeout=100;

    while(1) 
    {
        timeout--;
        if (timeout<0){  break;  }

        ack_value = (unsigned char) i8042_mouse_read();
        
        // OK
        if (ack_value == 0xFA){
            return;  
        }
    }; 

// Acabou o tempo, vamos checar o valor.
// Provavelmente esta errado.
    if ( ack_value != 0xFA )
    {
        //#debug
        //printk ("expect_ack: not ack\n");
        return;
        //return -1;
    }

    return;
    //return 0;
}

static void __initialize_mouse_position(void)
{
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    deviceWidth  = (unsigned long) (deviceWidth & 0xFFFF);
    deviceHeight = (unsigned long) (deviceHeight & 0xFFFF);
    if (deviceWidth==0 || deviceHeight==0){
        panic("initialize_mouse_position: w h\n");
    }
    mouse_x = (long) (deviceWidth >> 1);
    mouse_y = (long) (deviceHeight >> 1);
    saved_mouse_x = (long) mouse_x;
    saved_mouse_y = (long) mouse_x;
}

// #deprecated!
// local worker
// This is an old routine for parsing the packet values.
// Not used for now.
void __update_mouse (void)
{
// O primeiro byte comtém um monte de flags.
// O segundo byte comtém o delta x
// O terceiro byte comtém o delta y
    unsigned char Flags=0;
    char DeltaX=0;
    char DeltaY=0;
    Flags  = (mouse_packet_data & 0xFF);
    DeltaX = (mouse_packet_x & 0xFF);
    DeltaY = (mouse_packet_y & 0xFF);

//======== X ==========
// Testando o sinal de x.
// Do the x pos first.

//pega o delta x
//testa o sinal para x
do_x:
    if ( Flags & MOUSE_FLAGS_X_SIGN ){  goto x_neg;  }

//Caso x seja positivo.
x_pos:
    mouse_x += DeltaX;
    goto do_y;

//Caso x seja negativo.
x_neg:
    mouse_x -= ( ~DeltaX + 1 );

    if (mouse_x > 0){  goto do_y;  }

    mouse_x = 0;

//======== Y ==========
// Testando o sinal de x. 
// Do the same for y position.

//Pega o delta y.
//Testa o sinal para y.
do_y:
    if ( Flags & MOUSE_FLAGS_Y_SIGN ){  goto y_neg;  }

//Caso y seja positivo.
y_pos:
    mouse_y -= DeltaY;

    if ( mouse_y > 0 ){  goto quit; }

    mouse_y = 0;
    goto quit;

//Caso y seja negativo. 
y_neg:
    mouse_y += ( ~DeltaY + 1 );

// Quit
quit:
    return;
}

// local worker
// This routine is gonna parse the packet and 
// give us an updated x and y values.
// #todo: 
// We need to review the number given to the buttons,
// 1,2,3 or 0,1,2. What is the right standard?

static void __ps2mouse_parse_data_packet(void)
{
// Parse the mouse pacjet and 
// send event to wmMouseEvent in grinput.c.

// The first byte.
    unsigned char Flags=0;
// Overflow support.
    int x_overflow=FALSE; 
    int y_overflow=FALSE;
// Sign support.
    int x_sign=FALSE;
    int y_sign=FALSE;
// The current buttons state.
    unsigned int mbuttons_current_state[5];
// Button changes.
    int button0_changed=FALSE;
    int button1_changed=FALSE;
    int button2_changed=FALSE;
    int Status=-1;

// ------------------------------------
// Step1:
// Dealing with position.

// Save the old values of x and y.
    saved_mouse_x = mouse_x;
    saved_mouse_y = mouse_y;

// Save for local use.
    Flags = buffer_mouse[0];

// Grab the items inside the packet.
    mouse_packet_data = buffer_mouse[0];
    mouse_packet_x    = buffer_mouse[1];
    mouse_packet_y    = buffer_mouse[2];
    // ? = = buffer_mouse[3];  //extra

// Check signal
    if (mouse_packet_data & 0x10){ x_sign=TRUE; }
    if (mouse_packet_data & 0x20){ y_sign=TRUE; }

// Check overflow
    if (mouse_packet_data & 0x40){ x_overflow=TRUE; }
    if (mouse_packet_data & 0x80){ y_overflow=TRUE; }

// Signal changes
    if (mouse_packet_x && x_sign)
    { 
        mouse_packet_x = (char) (mouse_packet_x - 0xFF - 0x01);
    }
    if (mouse_packet_y && y_sign)
    { 
        mouse_packet_y = (char) (mouse_packet_y - 0xFF - 0x01);
    }

// Abort in case of overflow.
    if (x_overflow || y_overflow)
    {
        return;
    }

// Final x and y positions.
    mouse_x = (long) (mouse_x + mouse_packet_x);
    mouse_y = (long) (mouse_y - mouse_packet_y);
    mouse_x = (long) (mouse_x & 0x000003FF );
    mouse_y = (long) (mouse_y & 0x000003FF );

// ------------------------------------
// Step2:
// Dealing with buttons.

// Left button.
    if ( (Flags & MOUSE_LEFT_BUTTON) == 0 ){
        mbuttons_current_state[0] = FALSE;
    }else if( (Flags & MOUSE_LEFT_BUTTON) != 0 ){
        mbuttons_current_state[0] = TRUE;
    };

// Right button.
    if ( (Flags & MOUSE_RIGHT_BUTTON) == 0 ){
        mbuttons_current_state[1] = FALSE;
    }else if( (Flags & MOUSE_RIGHT_BUTTON) != 0 ){
        mbuttons_current_state[1] = TRUE;
    };

// Middle button.
    if ( (Flags & MOUSE_MIDDLE_BUTTON) == 0 ){
        mbuttons_current_state[2] = FALSE;
    }else if( (Flags & MOUSE_MIDDLE_BUTTON) != 0 ){
        mbuttons_current_state[2] = TRUE;
    };

// Buttons state:
// Compare the current state with the old state.

// button 0 changed
    button0_changed = FALSE;
    if (mbuttons_current_state[0] != mbuttons_old_state[0])
    {
        button0_changed = TRUE;
    }
// button 1 changed
    button1_changed = FALSE;
    if (mbuttons_current_state[1] != mbuttons_old_state[1])
    {
        button1_changed = TRUE;
    }
// button 2 changed
    button2_changed = FALSE;
    if (mbuttons_current_state[2] != mbuttons_old_state[2])
    {
        button2_changed = TRUE;
    }

// Save the current states.
    mbuttons_old_state[0] = mbuttons_current_state[0];
    mbuttons_old_state[1] = mbuttons_current_state[1];
    mbuttons_old_state[2] = mbuttons_current_state[2];

// Call the event handler.
// Was it pressed or released?

// 1 (left)
// The button 0 changed the state.
// O botao 0, eh na verdade o botao 1. (left)
    if (button0_changed == TRUE)
    {
        // Presssed
        if (mbuttons_current_state[0] == TRUE){
            wmMouseEvent( MSG_MOUSEPRESSED, 1, 1 );
        }
        // Released
        if (mbuttons_current_state[0] == FALSE){
            wmMouseEvent( MSG_MOUSERELEASED, 1, 1 );
        }
        return;
    }

// 2 (right)
// The button 1 changed the state.
// O botao 1, eh na verdade o botao 2. (right)
    if (button1_changed == TRUE)
    {
        // Pressed
        if (mbuttons_current_state[1] == TRUE){
            wmMouseEvent( MSG_MOUSEPRESSED, 2, 2 );
        }
        // Relesed
        if (mbuttons_current_state[1] == FALSE){
            wmMouseEvent( MSG_MOUSERELEASED, 2, 2 );
        }
        return;
    }

// 3 (middle)
// The button 2 changed the state.
// O botao 2, eh na verdade o botao 3. (middle)
    if (button2_changed == TRUE)
    {
        // Pressed
        if (mbuttons_current_state[2] == TRUE){
            wmMouseEvent( MSG_MOUSEPRESSED, 3, 3 );
        }
        // Released
        if (mbuttons_current_state[2] == FALSE){
            wmMouseEvent( MSG_MOUSERELEASED, 3, 3 );
        }
        return;
    }

// ------------------------------------
// Step3:
// Dealing with movement.

// Is the mouse moving or not?

    // Default: Not.
    ps2_mouse_moving = FALSE;

    // Yes.
    if ( saved_mouse_x != mouse_x || 
         saved_mouse_y != mouse_y )
    {
        ps2_mouse_moving = TRUE;
    }

// Call the event handler if we have an move event.
// IN: event id, x, y.
// see: grinput.c
    if (ps2_mouse_moving == TRUE){
        wmMouseEvent( MSG_MOUSEMOVE, mouse_x, mouse_y );
    }
}

// local
//int __get_device_id(void);
int __get_device_id(void)
{
    i8042_mouse_write (PS2MOUSE_GET_DEVICE_ID);
    i8042_mouse_expect_ack();
    return (int) i8042_mouse_read(); 
}

// Called by irq12_MOUSE in mouse.c.
// See: https://wiki.osdev.org/Mouse_Input
// See: https://wiki.osdev.org/User:Kmtdk
// See: https://wiki.osdev.org/PS/2_Mouse
void DeviceInterface_PS2Mouse(void)
{
    unsigned char _byte=0;
    int posX = 0;
    int posY = 0;

//#debug
//#todo: deletar isso.
    //debug_print ("DeviceInterface_PS2Mouse:\n");

// Not initialized
    if (PS2.mouse_initialized != TRUE){
        debug_print ("DeviceInterface_PS2Mouse: Not initialized yet\n");
        return;
    }

// Get one byte in the controler.
// #todo
// Temos que checar se o primeiro byte é um ack ou um resend.
// isso acontece logo apos a inicialização.
// #define ACKNOWLEDGE         0xFA
// #define RESEND              0xFE

// == bytes =================================
// #todo
// Read this to understand the bytes.
// https://wiki.osdev.org/Mouse_Input

/*
 One of the biggest problems with streaming 
 mode is "alignment" -- the packets were never defined 
 to have an obvious boundary. This means that it is 
 very easy to lose track of which mouse byte is 
 supposed to be the first byte of the next packet. 
 This problem is completely avoided if you specifically 
 request single packets (instead of using streaming mode) 
 because every packet begins with an ACK (0xFA), 
 which is easily recognizable.
*/


// =============================================
// #test
// Get status
    unsigned char status = in8(I8042_STATUS);
// buffer full?
    if (!(status & I8042_BUFFER_FULL)){
        return;
    }
// which device?
// Is it a mouse device?
// Return if it is not a mouse device.
    int is_mouse_device = 
        ((status & I8042_WHICH_BUFFER) == I8042_MOUSE_BUFFER) 
        ? TRUE 
        : FALSE;
    if (is_mouse_device == FALSE)
        return;
// =============================================

    PS2Mouse.last_jiffy = (unsigned long) get_systime_totalticks();

// Get the byte
    _byte = (unsigned char) in8(0x60);

// ACK: 
// Significa início do pacote quando no modo
// 'request single packets'.
// Então vamos reinicializar o contador.

/*
    // O ACK não é enviado se estivermos
    // no modo streaming.
    if (_byte == 0xFA){
        //debug_print ("[0xFA]: ACK\n");
        mouse_stage = 0;
        return;
    }
*/

/*
// RESEND: 
// ??? Resend é comando e não estatus.
    if (_byte == 0xFE){
        //debug_print ("[0xFE]: RESEND\n");
        printk ("DeviceInterface_PS2Mouse: resend\n");
        refresh_screen();
        //mouse_stage = 0;
        return;
    }
*/

// First byte.
// Y overflow, X overflow, Y sign bit, X sign bit
// Always 1, Middle Btn, Right Btn, Left Btn

    // Save
    buffer_mouse[mouse_stage] = (char) _byte;

    switch (mouse_stage){

    case 0:
        if (!(_byte & MOUSE_FLAGS_ALWAYS_1))
        {
            mouse_stage=0;
            break;
        }
        mouse_stage++;
        break;
    case 1:
        mouse_stage++;
        break;
    case 2:
        // If we have wheel, so we got another stage.
        if (PS2Mouse.has_wheel == TRUE)
        {
            mouse_stage++;
            break;
        }
        // If we do not have the wheel.
        // Commit packet.
        __ps2mouse_parse_data_packet();
        mouse_stage = 0;
        break;
    case 3:
        __ps2mouse_parse_data_packet();
        mouse_stage = 0;
        break;
    // Error: drain and clean
    default:
        in8(0x60);
        mouse_stage = 0;
        break;
    };

// #todo
// Coloque os pacotes num arquivo,
// o window server poderá ler depois.
   //write_packet(mousefp,...)
}


// #todo ioctl
int 
ps2mouse_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    debug_print("ps2mouse_ioctl: #todo\n");

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


//
// $
// INITIALIZATION
//

// ps2mouse_initialize_device:
//     Initialize the device.
/*
0xFF ResetMouse reset
0xFE ResendFor serial communications errors
0xF6 Set DefaultsSet default values
0xF5 Disable (Data Reporting)In stream mode, 
     should be sentbefore any other command
0xF4 Enable (Data Reporting)In stream mode only
0xF3 Set Sample RateSets state sampling rate
0xF0 Set Remote modeSend data on request only
0xEB Read DataSend data packet request
0xEA Set Stream ModeSend data on events
0xE9 Status RequestGet mouse configuration (3 bytes)
0xE8 Set Resolution
0xE7 Set Scaling 2:1Accelerationmode
0xE6 Set Scaling 1:1Linear mode
*/
// #todo: 
// We need a file structure and the function ps2mouse_ioctl();

void ps2mouse_initialize_device(void)
{
    unsigned char status=0;
    unsigned char device_id=0;

    debug_print ("ps2mouse_initialize_device:\n");

    PS2Mouse.initialized = FALSE;
    PS2Mouse.irq_is_working = FALSE;
    PS2Mouse.use_polling = FALSE;
    PS2Mouse.last_jiffy = jiffies;


// Pointer
    __initialize_mouse_position();

//++
//======================================================
    // #obs:
    // A rotina abaixo habilita o segundo dispositivo. O mouse.
    // Mas antes da habilitação temos uma rotina que não me lembro
    // pra que serve, haha.
    // #importante: 
    // TALVEZ. Tô achando que essa rotina deva ser feita no 
    // teclado, apos seu reset, e não aqui no mouse.
    
    // #bugbug
    // Essa nao eh uma rotina de habilitaçao do dispositivo secundario,
    // estamos apenas lendo a porta 60 e devolvendo o que lemos o que lemos
    // com alguma modificaçao;
    // #todo: me parece que para habilitar o secundario eh
    // preciso apenas mandar 0xA8 para a porta 0x64.

// ========================================
// Enable the interrupts

// Dizemos para o controlador entrar no modo leitura.
// Esperamos para ler e lemos.
// 0x20 Read Command Byte
// I8042_READ = 0x20

    wait_then_write(0x64,I8042_READ);
    status = (wait_then_read(0x60) | 2);

// Dizemos para o controlador entrar no modo escrita.
// Esperamos para escrever e escrevemos.
// Enable the PS/2 mouse IRQ (12).
// The keyboard uses IRQ 1 (and is enabled by bit 0 in this register).
// 0x60 Write Command Byte

    wait_then_write (0x64,I8042_WRITE);   // I8042_WRITE = 0x60
    wait_then_write (0x60,status);   
    
// ========================================
// Activate mouse port. (2nd port)

// 0x64 <<< 0xA8 ?? enable aux
// 0x64 <<< 0xA9 ?? check for mouse
// #todo: See i8042.h for the commands used in the initialization.

// #test
// Habilitando o dispositivo secundario na forma bruta.
// Reenable mouse port.
// #bugbug: 
// a rotina de inicializaçao da controladora ps2,
// esta fazendo isso ao fim da rotina. nao precisamos fazer isso aqui.
// Talvez atrapalhe a inicializaçao.
// 0xA8 Enable Mouse
// 0xA7 Disable Mouse
// 0xA9 Check Mouse InterfaceReturns 0, if OK

// Enable mouse
    wait_then_write(0x64,0xA8);
    i8042_mouse_expect_ack();

// ========================================
// Reset
// #bugbug: Isso reseta o controlador ps2 inteiro?
// Essa rotina avisa que vai escrever na porta do mouse
// antes de escrever.
// Então reseta somente o mouse e não o teclado.
    i8042_mouse_write(0xFF); 
    i8042_mouse_expect_ack();

// Default settings
// 0xF6 Set default settings.
    i8042_mouse_write (__PS2MOUSE_SET_DEFAULTS);
    i8042_mouse_expect_ack();

// Set sample rate
// Sampling Rate: Packets the mouse can send per second.
    i8042_mouse_write(0xF3);
    i8042_mouse_expect_ack();
    i8042_mouse_write(PS2MOUSE_DEFAULT_SAMPLERATE);  //200
    i8042_mouse_expect_ack();
    PS2Mouse.sample_rate = PS2MOUSE_DEFAULT_SAMPLERATE;

// Set resolution
// Resolution: DeltaX or DeltaY for each millimeter of mouse movement.
    i8042_mouse_write(__PS2MOUSE_SET_RESOLUTION);
    i8042_mouse_expect_ack();
    i8042_mouse_write(PS2MOUSE_RESULUTION);  //0~3
    i8042_mouse_expect_ack();
    PS2Mouse.resolution = PS2MOUSE_RESULUTION;

// Set scaling 1:1
// Apply a simple non-linear distortion to mouse movement.
    i8042_mouse_write(0xE6);
    i8042_mouse_expect_ack();

// Enable the mouse
    i8042_mouse_write(0xF4);
    i8042_mouse_expect_ack();

//++
//=================================================

// wheel
    PS2Mouse.has_wheel = FALSE;
    PS2Mouse.has_five_buttons = FALSE;

// Device id. (Not initialized)
    PS2Mouse.device_id = -1;

//=============
//__enable_wheel:    

// #obs:
// A rotina abaixo habilita a rodinha, se o dispositivo possui.
// Credits: Serenity OS.   

/*
// osdev
// https://wiki.osdev.org/%228042%22_PS/2_Controller#Detecting_PS.2F2_Device_Types 
The full sequence is:
Send the "disable scanning" command 0xF5 to the device
Wait for the device to send "ACK" back (0xFA)
Send the "identify" command 0xF2 to the device
Wait for the device to send "ACK" back (0xFA)
Wait for the device to send up to 2 bytes of reply, with a time-out to determine when it's finished (e.g. in case it only sends 1 byte)
A partial list of responses includes:

Byte/s	Device Type
None	Ancient AT keyboard with translation enabled in the PS/Controller (not possible for the second PS/2 port)
0x00	Standard PS/2 mouse
0x03	Mouse with scroll wheel
0x04	5-button mouse
0xAB, 0x41 or 0xAB, 0xC1	MF2 keyboard with translation enabled in the PS/Controller (not possible for the second PS/2 port)
0xAB, 0x83	MF2 keyboard
*/

//
// Get the Device ID and setup wheel.
//

// Get the device id for the first time.
    device_id = (int) __get_device_id();
    if (device_id == 0xFA){ 
        printk("ps2 mouse: device id fail\n");
    }
    PS2Mouse.device_id = device_id;

    // #debug
    // printk("ps2 mouse: Device id %d\n",PS2Mouse.device_id);

// Intelligent?
// Se ele não é inteligente,
// vamos fazer a inicialização e ver se vira inteligente.

    // 0
    if (device_id != PS2MOUSE_INTELLIMOUSE_ID || 
        device_id != PS2MOUSE_INTELLIMOUSE_EXPLORER_ID )
    {
        // Send magical wheel initiation sequence.
        i8042_mouse_write (PS2MOUSE_SET_SAMPLE_RATE);
        i8042_mouse_expect_ack();
        i8042_mouse_write (200);
        i8042_mouse_expect_ack();
        i8042_mouse_write (PS2MOUSE_SET_SAMPLE_RATE);
        i8042_mouse_expect_ack();
        i8042_mouse_write (100);
        i8042_mouse_expect_ack();
        i8042_mouse_write (PS2MOUSE_SET_SAMPLE_RATE);
        i8042_mouse_expect_ack();
        i8042_mouse_write (80);
        i8042_mouse_expect_ack();

        // Checando novamente pra ver se ficou inteligente.
        device_id = (int) __get_device_id();
        if (device_id != 0xFA){ 
            PS2Mouse.device_id = device_id;
        }
        
        // #debug
        // printk("ps2 mouse: Device id %d\n",PS2Mouse.device_id);
    }

    // Vamos ver se ficou inteligente depois
    // e possivelmente fazermos uma segunda inicialização.
    
    // YES, ele agora é inteligente
    if (PS2Mouse.device_id == PS2MOUSE_INTELLIMOUSE_ID || 
        PS2Mouse.device_id == PS2MOUSE_INTELLIMOUSE_EXPLORER_ID )
    {
        PS2Mouse.has_wheel = TRUE;
        
        // #debug
        // printk ("ps2mouse_initialize_device: Mouse wheel enabled!\n");
    } 

    // NO, ele ainda é burro.
    // nao e' 3 nem 4.
    if (PS2Mouse.device_id != PS2MOUSE_INTELLIMOUSE_ID &&  
        PS2Mouse.device_id != PS2MOUSE_INTELLIMOUSE_EXPLORER_ID )
    {
        PS2Mouse.has_wheel = FALSE;

        // #debug
        // printk ("ps2mouse_initialize_device: No mouse wheel detected!\n");
    }
//=============

// Sim ja temos um mouse de id '3'
// Para habilitarmos o mouse com 5 botoes
// temos que repetir a sequencia.
// See:
// https://wiki.osdev.org/PS/2_Mouse

    // 3
    if (PS2Mouse.device_id == PS2MOUSE_INTELLIMOUSE_ID)
    {
        // Enable five buttons
        i8042_mouse_write (PS2MOUSE_SET_SAMPLE_RATE);
        i8042_mouse_expect_ack();
        i8042_mouse_write (200);
        i8042_mouse_expect_ack();
        i8042_mouse_write (PS2MOUSE_SET_SAMPLE_RATE);
        i8042_mouse_expect_ack();
        i8042_mouse_write (200);
        i8042_mouse_expect_ack();
        i8042_mouse_write (PS2MOUSE_SET_SAMPLE_RATE);
        i8042_mouse_expect_ack();
        i8042_mouse_write (80);
        i8042_mouse_expect_ack();
        // Pega novamente para sabermos se e' mouse de id 4
        // e tem 5 botoes.
        device_id = (int) __get_device_id();
        if (device_id != 0xFA){ 
            PS2Mouse.device_id = device_id;
        }

        // #debug
        // printk("ps2 mouse: Device id %d\n",PS2Mouse.device_id);
    }

    // 4
    if (PS2Mouse.device_id == PS2MOUSE_INTELLIMOUSE_EXPLORER_ID)
    {
        PS2Mouse.has_five_buttons = TRUE;
        
        // #debug
        // printk("ps2mouse_initialize_device: Mouse has five buttons\n");
    }

//=================================================
//--

// Resolution again
    i8042_mouse_write(__PS2MOUSE_SET_RESOLUTION);
    i8042_mouse_expect_ack();
    i8042_mouse_write(PS2MOUSE_RESULUTION);  //0~3
    i8042_mouse_expect_ack();
    PS2Mouse.resolution = PS2MOUSE_RESULUTION;

// OK
    PS2Mouse.initialized = TRUE;
}

//
// End
//




