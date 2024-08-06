
// ps2kbd.c
// ps/2 keyboard support.
// ring0, kernel base.
// Created by Fred Nora.

#include <kernel.h>

// see: ps2kbd.h
struct ps2_keyboard_d  PS2Keyboard;
static int __prefix=0;
unsigned char ps2kbd_led_status=0;

//
// == private functions: prototypes ================
//

static void keyboard_init_lock_keys(void);
static void keyboard_init_modifier_keys(void);
// ...

// =========================


// #todo: Change this name.
static void keyboard_init_lock_keys(void)
{
    capslock_status = 0;
    scrolllock_status = 0;
    numlock_status = 0;
    // ...
}

// #todo: Change this name.
static void keyboard_init_modifier_keys(void)
{
// Modifier keys.
    shift_status  = 0;
    ctrl_status   = 0;
    winkey_status = 0;
    alt_status    = 0;

// Alternate Graphic.
    //altgr_status = 0; //@todo
// Function.
	//fn_status = 0;  //@todo
	//...
}


// #todo: 
// We need a file structure and the function ps2kbd_ioctl();
void ps2kbd_initialize_device (void)
{
    debug_print ("ps2kbd_initialize_device:\n");
    PS2Keyboard.initialized = FALSE;
    PS2Keyboard.irq_is_working = FALSE;
    PS2Keyboard.use_polling = FALSE;
    PS2Keyboard.last_jiffy = jiffies;

//====================================
// #test
// register device
// create file.
    file *fp;
    fp = (file *) kmalloc( sizeof(file) );
    if ((void *) fp == NULL){
        panic("kbd: fp\n");
    }
    memset ( fp, 0, sizeof(file) );
    fp->used = TRUE;
    fp->magic = 1234;
    fp->____object = ObjectTypeFile;
    fp->isDevice = TRUE;
// #todo
    fp->dev_major = 0;
    fp->dev_minor = 0;

// #todo: Initialize the file structure ... buffer ...

// #test
// Registrando o dispositivo.
    devmgr_register_device ( 
        (file *) fp, 
        "PS2KBD",         // pathname 
        DEVICE_CLASS_CHAR,   // class (char, block, network)
        DEVICE_TYPE_LEGACY,  // type (pci, legacy)
        NULL,                // Not a pci device.
        NULL );              // Not a tty device. (not for now)
//====================================

// globals
    keyboard_init_lock_keys();
    keyboard_init_modifier_keys();
    // ...

// Enable keyboard port
    wait_then_write(I8042_STATUS, 0xae);
    i8042_keyboard_expect_ack();

    __prefix=0;

    PS2Keyboard.initialized = TRUE;
}


// #test
// Poll keyboard
void ps2kbd_poll(void)
{
    if (PS2Keyboard.initialized != TRUE)
        return;
    if (PS2Keyboard.irq_is_working == TRUE)
        return;
    if (PS2Keyboard.use_polling == TRUE){
        DeviceInterface_PS2Keyboard();
    }
}


/*
 * DeviceInterface_PS2Keyboard: 
 *     Vamos pegar o raw code.
 *     Keyboard handler for abnt2 keyboard.
 *     fica dentro do driver de teclado.
 *     A interrupção de teclado vai chamar essa rotina.
 *     @todo: Usar keyboardABNT2Handler().
 * void keyboardABNT2Handler() 
 * Esse será o handler do driver de teclado
 * ele pega o scacode e passa para a entrada do line discipline dentro do kernel.
 * #todo: ISSO DEVERÁ IR PARA UM ARQUIVO MENOR ... OU AINDA PARA UM DRIVER.
 * Pega o scacode cru e envia para a disciplina de linhas que deve ficar no kernelbase.
 * Essa é a parte do driver de dispositivo de caractere.
 * #importante:
 * O driver deverá de alguma maneira notificar o kernel sobrea a ocorrência
 * do evento de input. Para que o kernel acorde as trheads que estão esperando 
 * por eventos desse tipo.
 */
// #importante:
// Provavelmente uma interrupção irá fazer esse trabalho de 
// enviar o scancode para o kernel para que ele coloque na fila.
// Nesse momento o kernel de sentir-se alertado sobre o evento de 
// input e acordar a threa que está esperando por esse tipo de evento. 
// #obs: 
// Esse buffer está em gws/user.h 
// Low level keyboard writter.
// Isso poderia usar uma rotina de tty
// O teclado esta lidando no momento com um buffer pequeno, 128 bytes.
// PUT SCANCODE

void DeviceInterface_PS2Keyboard(void)
{
// Make/Break code.
    unsigned char __raw=0;
    unsigned char val=0;
    // Usado nos testes
    //struct process_d *p;
    // Usado pra checar se a foreground thread quer raw input.
    struct thread_d *t;

// =============================================
// #test

// Get status.
// Return if the output buffer is not full.
    unsigned char status = in8(I8042_STATUS);
    if (!(status & I8042_STATUS_OUTPUT_BUFFER_FULL))
        return;

// Which device?
    int is_mouse_device = 
        ((status & I8042_WHICH_BUFFER) == I8042_MOUSE_BUFFER) 
        ? TRUE 
        : FALSE;
// Yes it is a mouse.
    if (is_mouse_device == TRUE)
        return;

// =============================================
// We don't need to control if we can read
// because was an interruption that brought us here.
// #obs:
// The byte may be a response to a command or a scancode.

//sc_again:

    PS2Keyboard.last_jiffy = (unsigned long) jiffies;

//===========================================
// #test
// Testing with ack
// credits: minix
// #define KEYBD   0x60  /* I/O port for keyboard data */
// #define PORT_B  0x61  /* I/O port for 8255 port B (kbd, beeper...) */
// #define KBIT    0x80  /* bit used to ack characters to keyboard */

// Get the rawbyte for the key struck.
// Read the byte out of the controller.
    __raw = (unsigned char) in8(0x60);

//===========================================
// Get
// > Strobe the keyboard to ack the char
// > Send back
// Strobe the bit high
// and then strobe it low.

    val = (unsigned char) in8(0x61); 
    out8(0x61, val | 0x80);  
    out8(0x61, val);
//===========================================

//++
// ===========================================
// #todo
// We have to check if the first byte is an ack or a resend.
// This all happen right after the initialization.
// #todo
// for me looks like that the first byte can be an ack or resend.
// #define ACKNOWLEDGE         0xFA
// #define RESEND              0xFE

// ??
// Assim como no mouse, talvez o ack so seja enviado
// Like a mouse, maybe the ack just be sent
// when we're in the streaming mode.

    switch (__raw){

    case 0:
       goto done;
       break;

    // ACKNOWLEDGE
    case 0xFA:
        //#test
        //printk ("DeviceInterface_PS2Keyboard: ack\n");
        goto done;
        break;
    
    // RESEND
    case 0xFE:
        //#test
        //printk ("DeviceInterface_PS2Keyboard: resend\n");
        goto done;
        break;

    case 0xFF:
       goto done;
       break;

    ScancodeOrPrefix:
    default:
        goto CheckByte;
        break;
    };

// ===========================================
//--

// #bugbug
// [Enter] in the numerical keyboard isn't working.
// teclas do teclado extendido.
// Nesse caso pegaremos dois sc da fila.
// #obs:
// O scancode é enviado para a rotina,
// mas ela precisa conferir ke0 antes de construir a mensagem,
// para assim usar o array certo.
// See: ps2kbd.c
// #bugbug
// Esse tratamento do scancode não faz sentido quando temos um
// window server instalado. Nesse caso deveríamos deixar o
// window server pegar os scancodes.
// Mas por enquanto, essa rotina manda mensagens para o ws
// caso tenha um instalado.

CheckByte:

// Check prefix for extended keyboard sequence.

    if (__raw == 0xE0){
        __prefix = (int) (__raw & 0x000000FF);
        goto done;
    }
    if (__raw == 0xE1){
        __prefix = (int) (__raw & 0x000000FF);
        goto done;
    }

// Process the normal byte
// >>> Posting the message into the windows server queue.
NormalByte:

// #opçao
// Apenas enfileira os raw bytes e nao processa.
    //if ( flag ...
   // put_rawbyte(__raw)
 
// We don't need this.
// The routine bellow is always posting 
// to the windows server.
// see:
// user/input.c
// IN: tid, scancode, prefix.
    wmKeyEvent( 
        (unsigned char) __raw,
        (int) (__prefix & 0xFF) );

// Clean the mess.
    __prefix=0;

done:
    return;
}


// i8042_keyboard_disable:
// Disable keyboard.
// Wait for bit 1 of status reg to be zero.
// Send code for setting disable command.
    
void i8042_keyboard_disable (void)
{
    while ( ( in8(0x64) & 2) != 0 )
    {
         // Nothing.
    };
    out8(0x60,0xF5);
    //sleep(100);
}

// keyboardEnable:
//     Enable keyboard.
void i8042_keyboard_enable(void)
{
    // #bugbug
    // Dizem que isso pode travar o sistema.

// Wait for bit 1 of status reg to be zero.
// Send code for setting Enable command.
    while ( ( in8(0x64) & 2) != 0 )
    {
    };
    out8 (0x60,0xF4);
    //sleep(100);
}

// see: ps2kbd.h
void keyboard_set_leds(unsigned char flags)
{
// Wait for bit 1 of status reg to be zero.
// Send code for setting the flag.
    while ( (in8(0x64) & 2) != 0 )
    {
    };
    out8(0x60,KEYBOARD_SET_LEDS); 
    pit_sleep(100);

// Wait for bit 1 of status reg to be zero.
// Send flag. 
    while ( (in8(0x64) & 2) != 0 )
    {
    };
    out8(0x60,flags);
    pit_sleep(100);
}

// keyboardGetKeyState:
// Pega o status das teclas de modificação.
unsigned long keyboardGetKeyState(int vk)
{
    unsigned long State=0;
    int Lvk = (int) (vk & 0xFF);

    if (Lvk<0){
        return 0;
    }

    switch (Lvk){
    case VK_LSHIFT:    State = shift_status;       break;
    case VK_LCONTROL:  State = ctrl_status;        break;
    case VK_LWIN:      State = winkey_status;      break;
    case VK_LMENU:     State = alt_status;         break;
    case VK_RWIN:      State = winkey_status;      break;
    case VK_RCONTROL:  State = ctrl_status;        break;
    case VK_RSHIFT:    State = shift_status;       break;
    case VK_CAPITAL:   State = capslock_status;    break;
    case VK_NUMLOCK:   State = numlock_status;     break;
    case VK_SCROLL:    State = scrolllock_status;  break;
    // ...
    default:
        return 0;
        break;
    };

// TRUE or FALSE.
    return (unsigned long) (State & 0xFFFFFFFF);
}

// Get alt Status.
int get_alt_status (void)
{
    return (int) alt_status;
}

// Get control status.
int get_ctrl_status (void)
{
    return (int) ctrl_status;
}

// Get shift status.
int get_shift_status (void)
{
    return (int) shift_status;
}

// i8042_keyboard_read:
// Esta função será usada para ler dados do teclado na 
// porta 0x60, fora do IRQ1.

uint8_t i8042_keyboard_read (void)
{
    uint8_t Value=0;

    prepare_for_input();
    Value = in8(0x60);
    wait_ns(400);

    return (uint8_t) Value;
}

// i8042_keyboard_write: 
// Esta função será usada para escrever dados do teclado 
// na porta 0x60, fora do IRQ1.
void i8042_keyboard_write (uint8_t data)
{
    prepare_for_output();
    out8 ( 0x60, data );
    wait_ns(400);
}

/*
 * i8042_keyboard_read2:
 *     Pega um byte na porta 0x60.
 */
unsigned char i8042_keyboard_read2(void)
{
    prepare_for_input();
    return (unsigned char) in8(0x60);
}

void i8042_keyboard_expect_ack (void)
{
    // #bugbug
    // ? loop infinito  
    // while ( xxx_mouse_read() != 0xFA );

    unsigned char ack_value=0;
    int timeout=100;

    while (1)
    {
        timeout--;
        if (timeout <= 0){
            break;
        }

        //ack_value = (unsigned char) i8042_keyboard_read2();
        
        prepare_for_input();
        ack_value = (unsigned char) in8(0x60);
        
        // OK
        if (ack_value == 0xFA){
            return;  
        }
    }; 

// Acabou o tempo, vamos checar o valor.
// Provavelmente esta errado.
    if (ack_value != 0xFA)
    {
        //#debug
        //printk ("expect_ack: not ack\n");
        return;
        //return -1;
    }

    return;
    //return 0;
}


