// archhal.c
// HAL interface for x86_64 architecture.
// Created by Fred Nora.

#include <kernel.h>


//
// Imports
//

extern void asm_reboot(void);


//  VECTORS 
//  (isr and irq address)

// #todo: Oh boy, oh boy!
// Endereços das rotinas básicas chamadas pelos vetores de interrupções.
// Essas rotinas devem chamar seus handlers específicos.
// 256 interrupções
//unsigned long VECTORS[256];

// HANDLERS 
// (callbacks??) 

// #todo: Oh boy, oh boy!
// Endereços para as rotinas em C que tratam as interrupções.
// Essas rotinas rodam depois da rotina básica em assembly.
// 256 interrupções
// 8 extras para handlers default.
//unsigned long HANDLERS[256+8];


// ===============================

/*
 * Beep support
 * #bugbug: 
 * Não quero comprometer a programação do PIT. 
 */

/*
void ____Beep_tone (int freq){

    if (freq == 0) {
        int i = io_in8(0x61);
        io_out8(0x61, i & 0x0d);
    
    } else {
        int i = 1193180000 / freq;
        io_out8(0x43, 0xb6);
        io_out8(0x42, i & 0xff);
        io_out8(0x42, i >> 8);
        i = io_in8(0x61);
        io_out8(0x61, (i | 0x03) & 0x0f);
    }
}
*/ 



/*
// Called by the idle handler for up.
void hal_up_idle(void);
void hal_up_idle(void)
{
    asm ("sti");
    asm ("hlt");
    return;
}
*/


/*
// Called by the idle handler for mp.
void hal_mp_idle(void);
void hal_mp_idle(void)
{
    asm ("sti");
    return;
}
*/

/*
PC Speaker Control:
Port 0x61 is used to control the PC speaker. 
By setting bit 0 of this port, the output of timer 2 on the PIT 
can be connected directly to the speaker. 
This allows the speaker to produce sound based on the timer’s output1.

PIT Channel 2 Gate Control:
Port 0x61 also controls the gate input for PIT channel 2. 
Specifically, bit 0 of this port can be used to enable or 
disable the input signal to channel 2 of the PIT2.

These functions are crucial for generating sounds and managing timing operations in a PC.
*/


// Speaker ON. 
// OUT 
// Play sound using built in speaker
void archhal_speaker_on (void)
{
// Play the sound using the PC speaker.
    pit_speaker_on();
}

// Speaker OFF
// IN
void archhal_speaker_off (void)
{
// make it shutup.
    pit_speaker_off();
}

void archhal_reboot(void)
{
    asm_reboot();
}
