
// i8042.c
// Intel 8042 keyboard/mouse controller.
// Created by Fred Nora.

#include <kernel.h>  

// ??
// #todo
// Were was defined the 'PS2' confugyration structure?
// See: input/i8042/ps2.h


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

// ------------------------------------
// #local
// #bugbug?
// Is this the best option for x86_64?
#define __local_out_any_b(p)  \
    asm volatile ("outb %%al, %0" :: "dN"((p)) : "eax")


// List of legacy devices.
// 'file->device'
struct device_d  *PS2KeyboardDevice;
struct device_d  *PS2MouseDevice;
// ...

// Objects
file *PS2KeyboardDeviceObject;
file *PS2MouseDeviceObject;
// ...

// ttys
struct tty_d *PS2KeyboardDeviceTTY;
struct tty_d *PS2MouseDeviceTTY;
// ...

// tty drivers
struct ttydrv_d *PS2KeyboardDeviceTTYDriver;
struct ttydrv_d *PS2MouseDeviceTTYDriver;
// ...

/*
// #todo: Get these info with HAL.
static int i8042_keyboard_irq=0;
static int i8042_mouse_irq=0;
static unsigned long i8042_keyboard_interrupt_handler=0;
static unsigned long i8042_mouse_interrupt_handler=0;
*/

static void __ps2_wait(unsigned char type);

//-------------------------------------

void I8042Controller_do_drain(void)
{
    unsigned char status=0;
    unsigned char byte=0;

    //debug_print("I8042Controller_do_drain:\n");
    while (1){
        status = (unsigned char) in8(0x64);
        // Empty?
        if (!(status & I8042_STATUS_OUTPUT_BUFFER_FULL)){
            return;
        }  
        byte = (unsigned char) in8(0x60);
    };
}

// __ps2_wait:
// Espera por flag de autorização para ler ou escrever.
// #todo: Descreva pra que serve o bit 1. valor 2.
static void __ps2_wait (unsigned char type)
{
    unsigned char StatusChar=0;
    unsigned char Type = (type & 0xFF);
    int i=0;
    int timeout=10000;

// =====================================
// 0 = READ
// Sinalizado que o buffer ta cheio.
// # Somente para mouse.
// See: serenity os.
// Quando for '1' eu saio.
    if (Type==0)
    {
        for (i=0; i<timeout; i++) 
        {
            StatusChar = (unsigned char) in8(0x64);
            if ( (StatusChar & I8042_STATUS_OUTPUT_BUFFER_FULL) != 0 )
            {
               __local_out_any_b (0x80);
               return;
            }
        };
        return;
    }

// =====================================
// 1 = WRITE
// Quando for '0', eu saio.
    if (Type==1)
    {
        for (i=0; i<timeout; i++)
        {
            if ( !(in8(0x64) & I8042_STATUS_INPUT_BUFFER_FULL) )
            {
                __local_out_any_b (0x80);
                return;
            }
        };
        return;
    }
}  

void prepare_for_input(void)
{
// 0 = READ
    __ps2_wait(0);
}

void prepare_for_output(void)
{
// 1 = WRITE
    __ps2_wait(1);
}

// =======================
// wait then ...

unsigned char wait_then_read(int port)
{
    prepare_for_input();
    return (unsigned char) in8(port);
}

void wait_then_write( int port, int data )
{
    prepare_for_output();
    out8( port, data );
}

// ---------------------------------
// DDINIT_ps2:
// PS2 full initialization
// kbd and mouse
// This is called during the kernel initialization.
int DDINIT_ps2(void)
{
// Called by hv_ps2_full_initialization in hv.c
// Called by I_x64main in x64init.c

    unsigned int i=0;

    PROGRESS("DDINIT_ps2:\n");

//
// Keyboard
//

// Initialize the driver.
    ps2kbd_initialize_driver();

// Initialize the device
// Port 1: Keyboard
    ps2kbd_initialize_device();
    PS2.keyboard_initialized = TRUE;

//
// Mouse
//

// Initialize the driver.
    ps2mouse_initialize_driver();

// Initialize the device.
// Port 2: Mouse
    ps2mouse_initialize_device();
    PS2.mouse_initialized = TRUE;

// Drain
    for (i=0; i<100; i++){
        in8(0x60);
    };

    PS2.pooling = FALSE;
    PS2.used = TRUE;
    PS2.magic = 1234;

    return 0;
}

// Early initialization
// + Only keyboard.
// + No mouse initialization.
int DDINIT_ps2_early_initialization(void)
{
    unsigned int i=0;

    PROGRESS ("DDINIT_ps2_early_initialization:\n");

//
// Keyboard
//

// Initialize the driver.
    ps2kbd_initialize_driver();

// Initialize the device
// Port 1: keyboard
    ps2kbd_initialize_device();
    PS2.keyboard_initialized = TRUE;

//
// Mouse
//

// Port 2: Mouse
// We will not call the mouse initialization 
// in the early ps2 initialization routine.
    PS2.mouse_initialized = FALSE;
    PS2Mouse.initialized = FALSE;

// Drain
    for (i=0; i<100; i++){
        in8(0x60);
    };

    PS2.pooling = FALSE;
    PS2.used = TRUE;
    PS2.magic = 1234;

    return 0;
}

//
// End
//

