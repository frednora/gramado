// i8042.h
// Intel 8042 keyboard/mouse controller.
// Created by Fred Nora.


#ifndef __CHARDEV_I8042_H
#define __CHARDEV_I8042_H  1

//Initialising the PS/2 Controller (OSDev Wiki)
//Step 1: Initialise USB Controllers
//Step 2: Determine if the PS/2 Controller Exists
//Step 3: Disable Devices
//Step 4: Flush The Output Buffer
//Step 5: Set the Controller Configuration Byte
//Step 6: Perform Controller Self Test
//Step 7: Determine If There Are 2 Channels
//Step 8: Perform Interface Tests
//Step 9: Enable Devices
//Step 10: Reset Devices


// List of legacy devices.
// 'file->device'
extern struct device_d  *PS2KeyboardDevice;
extern struct device_d  *PS2MouseDevice;
// ...

// Objects
extern file *PS2KeyboardDeviceObject;
extern file *PS2MouseDeviceObject;
// ...

// ttys
extern struct tty_d *PS2KeyboardDeviceTTY;
extern struct tty_d *PS2MouseDeviceTTY;
// ...

// tty drivers
extern struct ttydrv_d *PS2KeyboardDeviceTTYDriver;
extern struct ttydrv_d *PS2MouseDeviceTTYDriver;
// ...

/*
 Wikipedia
 O conector PS/2 é um conector mini-DIN de 6 pinos usado para conectar alguns 
 teclados e mouses a um sistema de computador compatível com PC. 
 Seu nome vem da série de computadores pessoais IBM Personal System/2, 
 com o qual ele foi introduzido em 1987. O conector de mouse PS/2 geralmente 
 substitui antigos conectores de "mouses seriais" DE-9 RS-232, 
 enquanto o conector de teclado PS/2 substituiu o conector DIN mais largo de 5 pinos/180º 
 usado no desenho do IBM PC/AT. Os desenhos PS/2 nas interfaces de teclados e 
 mouses são eletricamente similares e empregam o mesmo protocolo de comunicação. 
 Entretanto, a porta de teclado ou mouse de um determinado sistema pode não ser 
 intercambiável uma vez que os dois dispositivos usam um conjunto de comandos diferentes.
 
A comunicação é serial, síncrona e bidirecional.[1] 
O dispositivo conectado gera o sinal de relógio. O hospedeiro controla a comunicação usando a 
linha de relógio. Quando o hospedeiro puxa o relógio para baixo, a comunicação do dispositivo é inibida.
*/

//
// Definições.
//

#define  PS2_PORT    0x60
#define  PS2_CTRL    0x64
#define  PS2_ACK     0xFA 
//...



// x86
//#define KEYBOARD_VECTOR  1
//#define MOUSE_VECTOR  12

//#define __PS2MOUSE_SET_DEFAULTS    0xF6
//#define __PS2MOUSE_SET_RESOLUTION  0xE8

// #test
// Define the bits in the 8042 status register.
#define I8042_STATUS_OUTPUT_BUFFER_FULL 0x01
#define I8042_STATUS_INPUT_BUFFER_FULL  0x02
#define I8042_STATUS_SELF_TEST_COMPLETE 0x04
#define I8042_STATUS_LAST_WRITE_COMMAND 0x08
#define I8042_STATUS_KEYBOARD_UNLOCK    0x10
#define I8042_STATUS_DATA_FROM_MOUSE    0x20
#define I8042_STATUS_TIMEOUT            0x40
#define I8042_STATUS_PARITY_ERROR       0x80

// #test
// Define bits in the 8042 command byte register.
#define I8042_COMMAND_BYTE_KEYBOARD_INTERRUPT_ENABLED 0x01
#define I8042_COMMAND_BYTE_MOUSE_INTERRUPT_ENABLED    0x02
#define I8042_COMMAND_BYTE_SYSTEM_FLAG                0x04
#define I8042_COMMAND_BYTE_PCAT_INHIBIT               0x08
#define I8042_COMMAND_BYTE_KEYBOARD_DISABLED          0x10
#define I8042_COMMAND_BYTE_MOUSE_DISABLED             0x20
#define I8042_COMMAND_BYTE_TRANSLATION_ENABLED        0x40



#define I8042_BUFFER 0x60
#define I8042_STATUS 0x64

// Espera para ler ou para escrever?
#define I8042_BUFFER_FULL 0x01

#define I8042_KEYBOARD_BUFFER 0x00
#define I8042_MOUSE_BUFFER 0x20
#define I8042_WHICH_BUFFER 0x20


#define I8042_DATAPORT          0x60	//Read/Write
#define I8042_STATUSREGISTER    0x64	//Read
#define I8042_COMMANDREGISTER   0x64	//Write


// i8042 commands.
#define I8042_READ                 0x20
#define I8042_WRITE                0x60
#define I8042_DISABLE_SECOND_PORT  0xA7  // Disable second PS/2 port (only if 2 PS/2 ports supported)
#define I8042_ENABLE_SECOND_PORT   0xA8  // Enable second PS/2 port (only if 2 PS/2 ports supported)
#define I8042_TEST_SECOND_PORT     0xA9  // Test second PS/2 port (only if 2 PS/2 ports supported)
#define I8042_TEST_FIRST_PORT      0xAB  // Test first PS/2 port
#define I8042_DISABLE_FIRST_PORT   0xAD  // Disable first PS/2 port
#define I8042_ENABLE_FIRST_PORT    0xAE  // Enable first PS/2 port
//...

// i8042 responses.
// 0xFA, 0xFE, 0xFC
#define I8042_ACKNOWLEDGE    0xFA        
#define I8042_RESEND         0xFE
//#define I8042_TEST_FAILED    0xFC 

#define I8042_TESTCONTROLLER         0xAA  // Test PS/2 Controller
#define I8042_TESTCONTROLLER_PASSED  0x55  // test passed
#define I8042_TESTCONTROLLER_FAILED  0xFC  // test failed


// Testing first port.
#define I8042_TEST_FIRST_PORT_PASSED      0x00  //test passed
#define I8042_TEST_FIRST_PORT_CLOCK_LOW   0x01  //clock line stuck low
#define I8042_TEST_FIRST_PORT_CLOCK_HIGH  0x02  //clock line stuck high
#define I8042_TEST_FIRST_PORT_DATA_LOW    0x03  //data line stuck low
#define I8042_TEST_FIRST_PORT_DATA_HIGH   0x04  //data line stuck high

// Testing second port.
#define I8042_TEST_SECOND_PORT_PASSED      0x00  //test passed
#define I8042_TEST_SECOND_PORT_CLOCK_LOW   0x01  //clock line stuck low
#define I8042_TEST_SECOND_PORT_CLOCK_HIGH  0x02  //clock line stuck high
#define I8042_TEST_SECOND_PORT_DATA_LOW    0x03  //data line stuck low
#define I8042_TEST_SECOND_PORT_DATA_HIGH   0x04  //data line stuck high

// Configuration Byte
//Bit  Meaning
//0	   First PS/2 port interrupt (1 = enabled, 0 = disabled)
//1	   Second PS/2 port interrupt (1 = enabled, 0 = disabled, only if 2 PS/2 ports supported)
//2	   System Flag (1 = system passed POST, 0 = your OS shouldn't be running)
//3	   Should be zero
//4	   First PS/2 port clock (1 = disabled, 0 = enabled)
//5	   Second PS/2 port clock (1 = disabled, 0 = enabled, only if 2 PS/2 ports supported)
//6	   First PS/2 port translation (1 = enabled, 0 = disabled)
//7	   Must be zero

//Detecting PS/2 Device Types
//Send the "disable scanning" command 0xF5 to the device
//Wait for device to send "ACK" back (0xFA)
//Send the "identify" command 0xF2 to the device
//Wait for device to send "ACK" back (0xFA)
//Wait for device to send up to 2 bytes of reply, with a time-out to determine 
//when it's finished (e.g. in case it only sends 1 byte)

//retorna 1 ou 2 bytes.
#define I8042_DEVICETYPE_STANDARD_PS2_MOUSE       0x00  //Standard PS/2 mouse
#define I8042_DEVICETYPE_MOUSE_WITH_SCROLL_WHELL  0x03  //Mouse with scroll wheel
#define I8042_DEVICETYPE_5BUTTON_MOUSE            0x04  //5-button mouse
//0xAB, 0x41 or 0xAB, 0xC1	MF2 keyboard with translation enabled in the PS/Controller (not possible for the second PS/2 port)
//0xAB, 0x83	MF2 keyboard



// ================================

// The main ps/2 configuration structure.
struct ps2_d
{
    int used;
    int magic;
    
    // #hackhack
    // Use pooling or not.
    // TRUE, FALSE
    // :: 
    // Maybe something is wrong with the 
    // USB/PS2 emulation microcode in my real machine.
    // Maybe the 'interrupts' is not working in the 
    // 32 bit protected mode. 
    // Maybe we can use 'pooling' to aboid this issue. 
    // Just like in Temple OS.
    // ps: Some other OSs are facing the same problem.
    int pooling;

    //flags: TRUE or FALSE,
    int keyboard_initialized;
    int mouse_initialized;
    
    // #todo
    // We can create these two structures.
    // See: ps2kbd.h and ps2mouse.h
    // struct ps2_keyboard_d *ps2_keyboard;
    // struct ps2_mouse_d    *ps2_mouse;
    
    // ...
};

// #important:
// use ps2_ioctl to setup this structure.
// The app needs to open the file /PS2, something like that.

struct ps2_d  PS2;
// ================================





//
// == prototypes ==================
//

void I8042Controller_do_drain(void);

void prepare_for_input (void);
void prepare_for_output (void);

unsigned char wait_then_read (int port);
void wait_then_write ( int port, int data );

void ps2kbd_initialize_device (void);
void ps2mouse_initialize_device (void);

// ================
// This is the full initialization.
// #bugbug This is a test yet.
// It fails in the real machine.
int DDINIT_ps2(void);

// ================
// Early initialization
// Only the keyboard.
// It is working
int DDINIT_ps2_early_initialization(void);

#endif    



