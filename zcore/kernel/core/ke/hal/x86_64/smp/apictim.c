// apictim.c
// APIC support timer.
// History:
// 2023 - Original ported from Sirius OS, created by Nelson Cole.
// ...


#include <kernel.h>

#define APIC_TIMER_NULL  0

#define APIC_TIMER_CONFIG_DATA_LVT(TimerMode,Mask,TriggerMode,Remote,InterruptInput,DeliveryMode,Vector)(\
(((unsigned int)(TimerMode)      &  0x3 ) << 17) |\
(((unsigned int)(Mask)           &  0x1 ) << 16) |\
(((unsigned int)(TriggerMode)    &  0x1 ) << 15) |\
(((unsigned int)(Remote)         &  0x1 ) << 14) |\
(((unsigned int)(InterruptInput) &  0x1 ) << 13) |\
(((unsigned int)(DeliveryMode)   &  0x7 ) <<  8) |\
( (unsigned int)(Vector)         & 0xff )\
)


static unsigned int apic_timer_ticks=0;

static unsigned int __apictim_read_command(unsigned short addr);
static void __apictim_write_command(unsigned short addr,unsigned int val);

// #todo: Move from this document.
static void __pit_prepare_sleep(int hz);
static void __pit_perform_sleep(void);

//---------------------------------

/*
PC Speaker Control:
Port 0x61 is used to control the PC speaker. 
By setting bit 0 of this port, the output of timer 2 on the PIT 
can be connected directly to the speaker. 
This allows the speaker to produce sound based on the timer’s output1.

PIT Channel 2 Gate Control:
Port 0x61 also controls the gate input for PIT channel 2. 
Specifically, bit 0 of this port can be used to enable or disable 
the input signal to channel 2 of the PIT2.

These functions are crucial for generating sounds and managing timing operations in a PC.
*/

static void __pit_prepare_sleep(int hz)
{
    int val = 0;
    unsigned int divisor = 1193182/hz;

// Initialize PIT Ch 2 in one-shot mode
// waiting 1 sec could slow down boot time considerably,
// so we'll wait 1/100 sec, and multiply the counted ticks

// 0xB2
// 10 | 11 | 001 | 0
// Channel 2 | word | mode | 16bit binary

    val = (in8(0x61) & 0xFD) | 0x1;
    out8(0x61, val);
    out8(0x43, 0xB2);

    out8(0x42,(unsigned char)(divisor & 0xFF));		// LSB
    io_delay();
    out8(0x42,(unsigned char)(divisor >> 8) & 0xFF); // MSB
}

static void __pit_perform_sleep(void)
{
    int val = 0;

// Reset PIT one-short counter (start counting)
    val = (in8(0x61) & 0xFE);
    out8(0x61, val); // gate low
    val = val | 1;
    out8(0x61, val); // gate high

// Now wait until PIT counter reaches zero
    while ((in8(0x61) & 0x20) == 0)
    {
    };
}

// #todo: 
// Definir porta 70h usada nesse arquivo. ??
static unsigned int __apictim_read_command(unsigned short addr)
{
    if( (void *) LAPIC.lapic_va == NULL ){
        panic("__apictim_read_command: LAPIC.lapic_va\n");
    }

    return *( (volatile unsigned int *)(LAPIC.lapic_va + addr));
}

static void __apictim_write_command(unsigned short addr,unsigned int val)
{
    if( (void *) LAPIC.lapic_va == NULL ){
        panic("__apictim_write_command: LAPIC.lapic_va\n");
    }

    *( (volatile unsigned int *)(LAPIC.lapic_va + addr)) = val;
}

void apic_initial_count_timer(int value)
{
    __apictim_write_command( 
        LAPIC_INITIAL_COUNT_TIMER, 
        value);
}

void apic_timer_umasked(void)
{
    unsigned int data = 
        __apictim_read_command(LAPIC_LVT_TIMER);

    __apictim_write_command( 
        LAPIC_LVT_TIMER, 
        data & 0xFFFEFFFF );

	//Divide Configuration Register, to divide by 128 0xA divide by 32 0x8
	//local_apic_write_command( APIC_DIVIDE_TIMER, 0xA);

	// Initial Count Register
	//local_apic_write_command( APIC_INITIAL_COUNT_TIMER, 12345);
	
}

void apic_timer_masked(void)
{
	unsigned int data = __apictim_read_command(LAPIC_LVT_TIMER);
	__apictim_write_command( LAPIC_LVT_TIMER, data | 1 << 16);
}

//
// $
// INITIALIZATION
//

int apic_timer(void)
{
// Called by enable_apic() in apic.c.

// #todo #bugbug
// e o timer precisa mudar o vetor 
// pois 32 ja esta sendo usado pela redirection table.

// --------------------------------------------------
// Map APIC timer to an interrupt, and by that 
// enable it in one-shot mode.
    unsigned int val = 
        APIC_TIMER_CONFIG_DATA_LVT(
            0,  // one-shot mode
            0,  // Umasked
            APIC_TIMER_NULL,
            APIC_TIMER_NULL,
            0,
            APIC_TIMER_NULL,
            0x20  // 32. (Vector) 
            );
    __apictim_write_command(LAPIC_LVT_TIMER, val);

// --------------------------------------------------
// Divide Configuration Register, 
// to divide by 16
    __apictim_write_command(LAPIC_DIVIDE_TIMER, 0x3);

// --------------------------------------------------
// Prepare the PIT to sleep for 10ms (10000µs)
// 1193180/100 Hz
    __pit_prepare_sleep(100);
    //__pit_prepare_sleep(1000);

// == RESET ======================================================
// Reset APIC Timer (set counter to -1)
    __apictim_write_command( LAPIC_INITIAL_COUNT_TIMER, 0xFFFFFFFF );
// Right after the RESET
// Get current counter value.
    apic_timer_ticks = __apictim_read_command(LAPIC_CURRENT_COUNT_TIMER);
// It is counted down from -1, make it positive
    apic_timer_ticks = 0xFFFFFFFF - apic_timer_ticks;
    apic_timer_ticks++;
    printk("Current count: {%d}\n", apic_timer_ticks);


// -- SLEEP ------------------------------------------------
// Perform PIT-supported sleep
    __pit_perform_sleep();


// == STOP ======================================================
// Stop APIC Timer
    __apictim_write_command( LAPIC_LVT_TIMER, (1 << 16) );
// Right after the STOP
// Get current counter value.
    apic_timer_ticks = __apictim_read_command(LAPIC_CURRENT_COUNT_TIMER);
// It is counted down from -1, make it positive
    apic_timer_ticks = 0xFFFFFFFF - apic_timer_ticks;
    apic_timer_ticks++;
    printk("Current count: {%d}\n", apic_timer_ticks);

// ========================================================

// --------------------------------------------------
// Finally re-enable timer in periodic mode. But MASKED.
	val = 
	    APIC_TIMER_CONFIG_DATA_LVT(
	        1,  // Periodic mode
	        1,  // Masked
	        APIC_TIMER_NULL,
	        APIC_TIMER_NULL,
	        0,
	        APIC_TIMER_NULL,
	        0x20  // 32. (Vector)
            );
    __apictim_write_command(LAPIC_LVT_TIMER, val);

// --------------------------------------------------
// Setting divide value register again not needed by the manuals
// Although I have found buggy hardware that required it
    __apictim_write_command(LAPIC_DIVIDE_TIMER, 0xa);  // Option: 0x3

// --------------------------------------------------
// Now eax holds appropriate number of ticks, 
// use it as APIC timer counter initializer.
    apic_timer_ticks = 1000;
    __apictim_write_command(LAPIC_INITIAL_COUNT_TIMER, apic_timer_ticks);

    return 0;
}

