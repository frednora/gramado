// ps2kbd.h
// Created by Fred Nora.

#ifndef ____PS2KBD_H
#define ____PS2KBD_H    1

#define PS2_KEYBOARD_IRQ  1


// Ports:
// =====
//     The entire range for the keyboard is 60-6F,
//     a total of 16 values (a 16bit range).
//  @todo:
//      As portas do controlador ainda estão subutilizadas.
//      fazer um driver mais completo utilizando melhor o controlador.
//Command Listing:
//================
//Command	Descripton
//0xED	Set LEDs
//0xEE	Echo command. Returns 0xEE to port 0x60 as a diagnostic test
//0xF0	Set alternate scan code set
//0xF2	Send 2 byte keyboard ID code as the next two bytes to be read from port 0x60
//0xF3	Set autrepeat delay and repeat rate
//0xF4	Enable keyboard
//0xF5	Reset to power on condition and wait for enable command
//0xF6	Reset to power on condition and begin scanning keyboard
//0xF7	Set all keys to autorepeat (PS/2 only)
//0xF8	Set all keys to send make code and break code (PS/2 only)
//0xF9	Set all keys to generate only make codes
//0xFA	Set all keys to autorepeat and generate make/break codes
//0xFB	Set a single key to autorepeat
//0xFC	Set a single key to generate make and break codes
//0xFD	Set a single key to generate only break codes
//0xFE	Resend last result
//0xFF	Reset keyboard to power on state and start self test

/*
#define KBC_DEVCMD_ACK		0xfa
#define KBC_DEVCMD_RESEND	0xfe
#define KBC_DEVCMD_BAT_DONE	0xaa
#define KBC_DEVCMD_BAT_FAIL	0xfc
*/

#define KBD_LEDS_SCRLBIT  0x01  // Scroll lock led.
#define KBD_LEDS_NUMSBIT  0x02  // Num lock led.
#define KBD_LEDS_CAPSBIT  0x04  // Caps lock led.
extern unsigned char ps2kbd_led_status;

// keyboard commands
#define  KEYBOARD_SET_LEDS       0xED    // Set keyboard leds.
#define  KEYBOARD_ECHO           0xEE
//#define KEYBOARD_SELECT_SCAN_CODE_SET     0xF0
#define  KEYBOARD_GET_ID         0xF2    // get keyboard ID.
#define  KEYBOARD_SET_RATE       0xF3    // Set typematic rate.
#define  KEYBOARD_ENABLE         0xF4    // Enable scanning.
#define  KEYBOARD_RESET_DISABLE  0xF5    // reset and disable scanning.
#define  KEYBOARD_RESET_ENABLE   0xF6    // reset and enable scanning.
#define  KEYBOARD_RESET          0xFF    // Reset.

// keyboard responses
// 0xAA, 0xFC, 0xF0, 0x37, 0x54

// keyboard responses
#define  KEYBOARD_COMPLETE_SUCCESS  0xAA
#define  KEYBOARD_COMPLETE_FAILURE  0xFC
#define  KEYBOARD_BREAK_CODE        0xF0
//#define  KEYBOARD_DEBUG_HOTKEY_ENH  0x37 
//#define  KEYBOARD_DEBUG_HOTKEY_AT   0x54 
//...

/*
enum KYBRD_CTRL_STATS_MASK {
 
	KYBRD_CTRL_STATS_MASK_OUT_BUF	=	1,		//00000001
	KYBRD_CTRL_STATS_MASK_IN_BUF	=	2,		//00000010
	KYBRD_CTRL_STATS_MASK_SYSTEM	=	4,		//00000100
	KYBRD_CTRL_STATS_MASK_CMD_DATA	=	8,		//00001000
	KYBRD_CTRL_STATS_MASK_LOCKED	=	0x10,		//00010000
	KYBRD_CTRL_STATS_MASK_AUX_BUF	=	0x20,		//00100000
	KYBRD_CTRL_STATS_MASK_TIMEOUT	=	0x40,		//01000000
	KYBRD_CTRL_STATS_MASK_PARITY	=	0x80		//10000000
};
*/

// Enumerating the types of keyboard.
typedef enum {
    KEYBOARD_VIRTUAL,
    KEYBOARD_PS2,
    KEYBOARD_USB,
    // ...
}keyboard_type_t;

// keyboardMessage
//     Estrutura interna para mensagens.
// ??
// #todo: 
// Na verdade todo driver usará estrutura de janela 
// descrita na API que o driver use.

struct keyboardMessage 
{
    unsigned char scancode;

    //? hwnd;  
    int message;
    unsigned long long1;
    unsigned long long2;
};


// ps2 keyboard info.
// this is gonna be used by PS2 structure in ps2.h
struct ps2_keyboard_d
{
    object_type_t objectType;
    object_class_t objectClass;
    file *fp;
    int initialized;
    int irq_is_working;
    int use_polling;
    unsigned long last_jiffy;

    keyboard_type_t type;

    // #todo
    // int control_fd;
    // int input_fd;
    // pid_t pid;
};
// see: ps2kbd.c
extern struct ps2_keyboard_d  PS2Keyboard;

// ======================================================

// Status
// @todo: Status pode ser (int).
// variáveis usadas pelo line discipline para controlar o 
// estado das teclas de controle.
// #todo: talvez isso possa ir pra dentro da estrutura 
// de teclado ps2.
// see: keyboard.c
extern unsigned long key_status;
extern unsigned long escape_status;
extern unsigned long tab_status;
extern unsigned long winkey_status;
extern unsigned long ctrl_status;
extern unsigned long alt_status;
extern unsigned long shift_status;
extern unsigned long capslock_status;
extern unsigned long numlock_status;
extern unsigned long scrolllock_status;
//...

//
// == prototypes ===========================
//

uint8_t i8042_keyboard_read (void);
void i8042_keyboard_write (uint8_t data);
unsigned char i8042_keyboard_read2(void);
void i8042_keyboard_expect_ack (void);

void i8042_keyboard_disable(void);
void i8042_keyboard_enable (void);

void keyboard_set_leds(unsigned char flags);

unsigned long keyboardGetKeyState(int vk);

int get_alt_status (void);
int get_ctrl_status (void);
int get_shift_status (void);

void DeviceInterface_PS2Keyboard(void); 

int 
ps2kbd_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

#endif    

