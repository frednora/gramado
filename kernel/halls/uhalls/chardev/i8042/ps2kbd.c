// ps2kbd.c
// ps/2 keyboard support.
// ring0, kernel base.
// Created by Fred Nora.

#include <kernel.h>

// see: ps2kbd.h
struct ps2_keyboard_d  PS2Keyboard;
unsigned char ps2kbd_led_status=0;

static int __prefix=0;

#define REPORT_MAX 7   // enough for Pause/Break
unsigned char Report[REPORT_MAX];
int reportIndex = 0;

//
// == private functions: prototypes ================
//

static void keyboard_init_lock_keys(void);
static void keyboard_init_modifier_keys(void);
// ...


static void 
__ps2kbd_interpret_and_dispatch(
    unsigned char b0,
    unsigned char b1,
    unsigned char b2,
    unsigned char prefix );

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

// i8042_keyboard_disable:
// Disable keyboard.
// Wait for bit 1 of status reg to be zero.
// Send code for setting disable command.
   
void i8042_keyboard_disable (void)
{
    while ( (in8(0x64) & 2) != 0 )
    {
        // Nothing
    };
    out8(0x60,0xF5);
    //sleep(100);
}

// i8042_keyboard_enable:
// Enable keyboard.
void i8042_keyboard_enable(void)
{
    // #bugbug
    // Dizem que isso pode travar o sistema.

// Wait for bit 1 of status reg to be zero.
// Send code for setting Enable command.
    while ( (in8(0x64) & 2) != 0 )
    {
    };
    out8 (0x60,0xF4);
    //sleep(100);
}

// Setup keyboard LEDs.
// see: ps2kbd.h
void keyboard_set_leds(unsigned char flags)
{
// Wait for bit 1 of status reg to be zero.
// Send code for setting the flag.
    while ( (in8(0x64) & 2) != 0 )
    {
        // Nothing
    };
    out8(0x60,KEYBOARD_SET_LEDS); 
    pit_sleep(100);

// Wait for bit 1 of status reg to be zero.
// Send flag. 
    while ( (in8(0x64) & 2) != 0 )
    {
        // Nothing
    };
    out8(0x60,flags);
    pit_sleep(100);
}

// [API]
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
    case VK_LSHIFT:      State = shift_status;       break;
    case VK_LCONTROL:    State = ctrl_status;        break;
    case VK_LWIN:        State = winkey_status;      break;
    case VK_LALT:        State = alt_status;         break;
    case VK_RWIN:        State = winkey_status;      break;
    case VK_RCONTROL:    State = ctrl_status;        break;
    case VK_RSHIFT:      State = shift_status;       break;
    case VK_CAPSLOCK:    State = capslock_status;    break;
    case VK_NUMLOCK:     State = numlock_status;     break;
    case VK_SCROLLLOCK:  State = scrolllock_status;  break;
    // ...
    default:
        return 0;
        break;
    };

// TRUE or FALSE.
    return (unsigned long) (State & 0xFFFFFFFF);
}

// [API]
// Get alt Status.
int get_alt_status (void)
{
    return (int) alt_status;
}

// [API]
// Get control status.
int get_ctrl_status (void)
{
    return (int) ctrl_status;
}

// [API]
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

// i8042_keyboard_read2:
// Get byte in port 0x60.
unsigned char i8042_keyboard_read2(void)
{
    prepare_for_input();
    return (unsigned char) in8(0x60);
}

void i8042_keyboard_expect_ack (void)
{
    unsigned char ack_value=0;
    int timeout=100;

    // #bugbug
    // ? loop infinito  
    // while ( xxx_mouse_read() != 0xFA );

    while (1)
    {
        timeout--;
        if (timeout <= 0){
            break;
        }

        // #todo: Use a worker like this?
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

static void 
__ps2kbd_interpret_and_dispatch(
    unsigned char b0,
    unsigned char b1,
    unsigned char b2,
    unsigned char prefix )
{
    unsigned char raw_code_0 = b0;
    unsigned char raw_code_1 = b1;
    unsigned char raw_code_2 = b2;

    switch (prefix)
    {

        // No prefix
        case 0x00:
            wmRawKeyEvent(raw_code_0, 0, 0, prefix);
            break;

        // We gotta send 2 or 3 bytes
        case 0xE0:
            // #todo: The worker will accept 3 bytes
            if (raw_code_1 == 0xF0) {
                // Break sequence: E0 F0 xx
                wmRawKeyEvent(raw_code_0, raw_code_1, raw_code_2, prefix);
            } else {
                // Make sequence: E0 xx 
                // Clear the third byte since it's not used
                wmRawKeyEvent(raw_code_0, raw_code_1, 0, prefix);
            }
            break;

        // We gotta send 3 bytes
        case 0xE1:
            // #todo: The worker will accept 3 bytes
            wmRawKeyEvent(raw_code_0, raw_code_1, raw_code_2, prefix);
            break;

        default:
            // Oh boy!
            break;
    };
}

/*
 * DeviceInterface_PS2Keyboard:
 *   Low-level PS/2 keyboard handler for ABNT2 layout.
 *   Called by the IRQ1 handler when a key event occurs.
 *
 * Responsibilities:
 *   - Read raw scancodes from the PS/2 controller (port 0x60).
 *   - Distinguish between keyboard and mouse bytes.
 *   - Handle ACK/RESEND responses from the controller.
 *   - Track extended scancode prefixes (0xE0, 0xE1).
 *   - Forward scancodes to the window server (or line discipline).
 *
 * Notes:
 *   - Currently uses a small 128-byte buffer.
 *   - Should eventually be moved into a dedicated driver file.
 *   - The driver must notify the kernel about input events so
 *     threads waiting for keyboard input can be awakened.
 */

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
    int fSequenceFinished = FALSE;

// Make/Break code.
    unsigned char __raw=0;
    unsigned char val=0;

// Usado pra checar se a foreground thread quer raw input.
    struct thread_d *t;
    //struct te_d *p;

    unsigned char status;
    int is_mouse_device;

    reportIndex = 0;

// OK. The buffer is FULL.
// Get more than one byte
    while (1){

// Get status.
// Return if the output buffer is not full.
    status = in8(I8042_STATUS);
    if (!(status & I8042_STATUS_OUTPUT_BUFFER_FULL))
        return;

// Which device?
    is_mouse_device = 
        ((status & I8042_WHICH_BUFFER) == I8042_MOUSE_BUFFER) 
        ? TRUE 
        : FALSE;
// Yes it is a mouse.
    if (is_mouse_device == TRUE)
        return;

// =============================================
// Não precisamos perguntar para o controlador se
// podemos ler, porque foi uma interrupção que nos trouxe aqui.
// #obs:
// O byte pode ser uma resposta à um comando ou um scancode.

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
// Temos que checar se o primeiro byte é um ack ou um resend.
// isso acontece logo apos a inicialização.
// #todo
// me parece que o primeiro byte pode ser um ack ou resend.
// #define ACKNOWLEDGE         0xFA
// #define RESEND              0xFE

// ??
// Assim como no mouse, talvez o ack so seja enviado
// quando estivermos em modo streaming.

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


    Report[reportIndex] = (char) __raw;

// Check prefix for extended keyboard sequence.
// Or normal bytes.

// Many emulators (including QEMU, Bochs, VirtualBox) don’t 
// generate the full sequence correctly. Some only send the 
// first three bytes (E1 1D 45), some send nothing at all, and 
// some treat Pause as “not implemented.”
// On real hardware, you’ll always see the full 6‑byte sequence.
// On QEMU, you often won’t.

/*
Scancode reference for extended keys (ABNT2, PS/2 Set 2)

pause/break = make: E1 1D 45, break: E1 9D C5
insert      = make: E0 52,    break: E0 F0 52
delete      = make: E0 53,    break: E0 F0 53
home        = make: E0 47,    break: E0 F0 47
end         = make: E0 4F,    break: E0 F0 4F
page up     = make: E0 49,    break: E0 F0 49
page down   = make: E0 51,    break: E0 F0 51
arrow up    = make: E0 48,    break: E0 F0 48
arrow down  = make: E0 50,    break: E0 F0 50
arrow left  = make: E0 4B,    break: E0 F0 4B
arrow right = make: E0 4D,    break: E0 F0 4D
right ctrl  = make: E0 1D,    break: E0 F0 1D
altgr       = make: E0 38,    break: E0 F0 38
sys menu    = make: E0 5D,    break: E0 F0 5D
*/


    // 0xE0 - Extended (1) prefix
    // #todo: If the first byte was 0xE0, the next byte is stored in Report[1].
    // 2 bytes for make and 3 bytes for break
    // Extended (0xE0):
    // + If second byte != 0xF0 >>> dispatch after 2 bytes (make).
    // + If second byte  = 0xF0 >>> wait for third byte, then dispatch (break).
    if (Report[0] == 0xE0){
    //printk("[kbd] 0xE0 prefix detected (reportIndex=%d)\n", reportIndex);

        if (reportIndex == 0)
            fSequenceFinished = FALSE;

        // ---- If its a 2 bytes, we send them, otherwise we wait for the next -----

        // 0xE0 XX
        // At reportIndex == 1 → dispatch make if not F0.
        if (reportIndex == 1)
        {
            // Not a break, so its a make
            // But, in qemu, Release: often just E0 xx again, with no F0 in the middle.
            if (Report[1] != 0xF0){
                //printk("[kbd] 0xE0 prefix detected [Not 0xF0] (reportIndex=%d)\n", reportIndex);
                __ps2kbd_interpret_and_dispatch(Report[0], Report[1], 0, Report[0]);
                fSequenceFinished = TRUE;
            }
            // Not a break, so its a make
            // its not the end of sequence yet ... lets wait the next byte.
            if (Report[1] == 0xF0){
                //printk("[kbd] 0xE0 prefix detected [0xF0] (reportIndex=%d)\n", reportIndex);
                fSequenceFinished = FALSE;
            }
        }

        // ---- the last byte, we send the sequence, otherwise its a byg -------------

        // 0xE0 ** XX
        // At reportIndex == 2 → dispatch break if Report[1] == F0.
        if (reportIndex == 2)
        {
            // If the last was a break sign
            if (Report[1] == 0xF0){
                __ps2kbd_interpret_and_dispatch(Report[0], Report[1], Report[2], Report[0]); 
                fSequenceFinished = TRUE;
            }
            // #bugbug: If the last was a break sign. Oh boy!
            if (Report[1] != 0xF0){
                fSequenceFinished = TRUE;
            }
        }

    // 0xE1 - Extended (2) prefix (used for Pause/Break and a few rare sequences)
    // #todo: If the first byte was 0xE1, the driver expects two more bytes.
    // It stores them in Report[1] and Report[2].
    // The first three (E1 1D 45) are the “make” sequence.
    // The next three (E1 9D C5) are the “break” sequence.
    // 3 bytes for make and 3 bytes for break
    // Extended‑2 (0xE1):
    // + If sequence is E1 1D 45 >>> Pause make, but you correctly keep accumulating 
    //   because you know more bytes are coming.
    // + If sequence is E1 9D C5 >>> Pause break, dispatch after 6 bytes.
    // + If it’s not E1 1D 45, you dispatch after 3 bytes (generic extended‑2).
    } else if (Report[0] == 0xE1){
    //printk("[kbd] 0xE1 prefix detected (reportIndex=%d)\n", reportIndex);

        // ------------ Pause/Break make (press) ----------
        // E1 1D 45

        if (reportIndex == 0)
            fSequenceFinished = FALSE;

        if (reportIndex == 1)
        {
            // We are in the second byte of pause/break key sequence (E1 1D 45)
            if (Report[1] == 0x1D)
                fSequenceFinished = FALSE;
        }

        if (reportIndex == 2)
        {
            // We are in the 3rd byte of pause/break key sequence (E1 1D 45)
            // The sequence is not finished, because we have more 3 bytes.
            if (Report[2] == 0x45){
                __ps2kbd_interpret_and_dispatch(Report[0], Report[1], Report[2], Report[0]);
                fSequenceFinished = FALSE;
            }
            if (Report[2] != 0x45){
                __ps2kbd_interpret_and_dispatch(Report[0], Report[1], Report[2], Report[0]); 
                fSequenceFinished = TRUE;  
            }
        }

        // ------------ Pause/Break break (release): ----------
        // E1 9D C5

        if (reportIndex == 3)
        {
            // We starts the release sequence of pause/break key
            if (Report[3] == 0xE1)
                fSequenceFinished = FALSE;
        }
        if (reportIndex == 4)
        {
            // We starts the release sequence of pause/break key
            if (Report[4] == 0x9D)
                fSequenceFinished = FALSE;
        }
        if (reportIndex == 5)
        {
            // We starts the release sequence of pause/break key
            if (Report[5] == 0xC5){
                if (Report[3] == 0xE1 && Report[4] == 0x9D && Report[5] == 0xC5){
                    __ps2kbd_interpret_and_dispatch(Report[3], Report[4], Report[5], Report[0]);
                }
                fSequenceFinished = TRUE;  
            }
        }

    // Regular case
    // Normal keys:
    // Single byte (xx) → dispatch immediately.
    } else if (Report[0] != 0xE0 && Report[0] != 0xE1) {
        
        // Normal single‑byte case 
        if (reportIndex == 0)
        {
            // Prefix 0
            __ps2kbd_interpret_and_dispatch(Report[0], 0, 0, 0);
            //wmRawKeyEvent(Report[0], 0, 0, 0);
            fSequenceFinished = TRUE;
        }
    };

        reportIndex++;
        if (reportIndex >= REPORT_MAX) {
            reportIndex = 0;
        }

        if (fSequenceFinished == TRUE)
            reportIndex = 0;
    };  // WHILE ends

done:
    return;
}

// #todo ioctl
int 
ps2kbd_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    debug_print("ps2kbd_ioctl: #todo\n");

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


// #todo: 
// We need a file structure and the function ps2kbd_ioctl();
void ps2kbd_initialize_device (void)
{
    debug_print ("ps2kbd_initialize_device:\n");
    PS2Keyboard.initialized = FALSE;
    PS2Keyboard.irq_is_working = FALSE;
    PS2Keyboard.use_polling = FALSE;
    PS2Keyboard.last_jiffy = jiffies;

// globals
    keyboard_init_lock_keys();
    keyboard_init_modifier_keys();
    // ...

// Enable keyboard port
// #ps: Do not expect ACK after controller command. Only from device. 
    wait_then_write(I8042_STATUS, 0xAE);

    __prefix=0;

    PS2Keyboard.initialized = TRUE;
}

//
// End
//
