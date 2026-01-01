// pit.c
// Programmable Interval Timer (PIT) - (Intel 8253/8254)
// 2013 - Created by Fred Nora.

// See:
// https://wiki.osdev.org/Programmable_Interval_Timer
// https://en.wikipedia.org/wiki/Intel_8253


#include <kernel.h>  


/*
I/O port     Usage
0x40         Channel 0 data port (read/write)
0x41         Channel 1 data port (read/write)
0x42         Channel 2 data port (read/write)
0x43         Mode/Command register (write only, a read is ignored)

Bits         Usage
 6 and 7      Select channel :
                 0 0 = Channel 0
                 0 1 = Channel 1
                 1 0 = Channel 2
                 1 1 = Read-back command (8254 only)
 4 and 5      Access mode :
                 0 0 = Latch count value command
                 0 1 = Access mode: lobyte only
                 1 0 = Access mode: hibyte only
                 1 1 = Access mode: lobyte/hibyte
 1 to 3       Operating mode :
                 0 0 0 = Mode 0 (interrupt on terminal count)
                 0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                 0 1 0 = Mode 2 (rate generator)
                 0 1 1 = Mode 3 (square wave generator)
                 1 0 0 = Mode 4 (software triggered strobe)
                 1 0 1 = Mode 5 (hardware triggered strobe)
                 1 1 0 = Mode 2 (rate generator, same as 010b)
                 1 1 1 = Mode 3 (square wave generator, same as 011b)
 0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/ 

/*
 PIT info:
 ========
 PIT: mode=2 count=0x48d3 (18643) - 64.00 Hz (ch=0)
 PIT: mode=3 count=0x10000 (65536) - 18.20 Hz (ch=0) (Oracle, Virtualbox).
 PIT: mode=3 count=0x2e9a (11930) - 100.01 Hz (ch=0) (Oracle, Virtualbox).
 PIT: mode=3 count=0x2e9b (11931) - 100.00 Hz (ch=0) (Oracle, Virtualbox).
 */


// see: pit.h
struct pit_info_d  PITInfo;

// Global tick counter (jiffies)
// This is the authority for PIT timer.
// No drift: 
// You don’t risk one counter being updated incorrectly while another stays correct.
// Clarity: 
// Anyone reading the code sees that time is always derived from ticks.
// Flexibility: 
// Works for any timer frequency without special cases.
// Maintainability: 
// Less state to reset or synchronize in your scheduler.

unsigned long jiffies=0;  // Total ticks

//
// == Private functions: prototypes ===================
//

static int __timerTimer(void);

// =================================================

// Return how many ticks running.
unsigned long get_ticks(void)
{
    return (unsigned long) jiffies;
} 

// Returns the number of whole seconds since the first timer interrupt.
unsigned long get_seconds(void) 
{
    unsigned long sc = (jiffies / DEFAULT_JIFFY_FREQ);
    return sc;
}

// Returns the total number of milliseconds since the first timer interrupt.
unsigned long get_milliseconds(void)
{
    unsigned long ms = (unsigned long) ((jiffies * 1000) / DEFAULT_JIFFY_FREQ);
    return ms;
}

// How many ticks per second.
unsigned long get_hz(void)
{
    return (unsigned long) DEFAULT_JIFFY_FREQ;
}

// systime in ms
unsigned long now(void){
    return (unsigned long) get_milliseconds();
}

// =============================

static int __timerTimer(void)
{
// Total ticks
    jiffies = 0;

    UpdateScreenFlag = FALSE;
// Profiler
    profiler_ticks_count = 0;
    profiler_ticks_limit = PROFILER_TICKS_DEFAULT_LIMIT;
    // ...
    return 0;
}



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
void pit_speaker_on (void)
{
// Play the sound using the PC speaker.

    uint8_t tmp=0;

    tmp = in8(0x61);
    if (tmp != (tmp | 3))
    {
        out8(0x61, tmp | 3);
    }
}

// Speaker OFF
// IN
void pit_speaker_off (void)
{
// make it shutup.

    uint8_t tmp=0; 

    tmp = in8(0x61) & 0xFC;
    out8 (0x61, tmp);
}


/*
 * irq0_TIMER:
 *   Timer interrupt handler (IRQ0).
 *   Called by PeripheralHall_irq0 in hw1.asm.
 *
 *   Responsibilities:
 *   - Update timer logic and system counters (via DeviceInterface_PIT).
 *   - Drive periodic scheduling (tsTaskSwitch).
 *   - Optionally poll PS/2 keyboard/mouse if configured.
 *   - Handle special EOI cases when spawning threads inside the PIT handler.
 *
 *   Notes:
 *   - PIT latch reset: bit 7 of port 0x61 (system control port B).
 *     (Maybe it's not done in our implementation yet)
 *   - EOI handling: normally done in the assembly epilogue,
 *     but if a spawn occurs here, the spawn routine must send EOI itself.
 */

__VOID_IRQ 
irq0_TIMER (void)
{

// Calling the timer routine.
    DeviceInterface_PIT();

// Polling mode: 
// If PS/2 devices are configured for polling, 
// manually invoke their handlers here. 
// (1000 times per second).

    int keyboard_pooling_status = (int) i8042_IsPS2KeyboardPooling();
    int mouse_pooling_status = (int) i8042_IsPS2MousePooling(); 
    if (keyboard_pooling_status == TRUE){
        irq1_KEYBOARD();
    }
    if (mouse_pooling_status == TRUE){
        irq12_MOUSE();
    }

// OEI?
// The key idea is that sometimes the spawn routine 
// (when creating a new thread during the timer interrupt) must send the EOI itself, 
// because you’re still inside the PIT handler. Other times, 
// the assembly epilogue will send the EOI automatically when returning from the interrupt.


// --- Task switching and spawn handling --- 
// 
// At this point we may need to spawn a new thread that was in standby. 
// Because we are still inside the PIT interrupt handler, the spawn routine 
// must know whether it is responsible for sending the EOI (End Of Interrupt). 
// 
// If a spawn occurs here, the context is already saved, but we must flag 
// that the spawn routine should issue the EOI for IRQ0 before returning. 
// Otherwise, the assembly epilogue will send the EOI automatically.
// Calling the taskswitching routine.
// See: disp/ts.c

// Tell spawn routine: "we are inside PIT handler, you must EOI"
    spawn_set_eoi_state();

// Perform the actual task switch.
// For now the context is saved only on global variables. That is the moment 
// when the context will be saved into the thread structure.
// See: disp/ts.c
    tsTaskSwitch();

// Reset the EOI state flag. 
// After task switching, the spawn routine no longer needs to send EOI. 
// The assembly return sequence will handle EOI for us.
    spawn_reset_eoi_state();
}


// The PIT handler for timer routines.
// Called by irq0_TIMER().
void DeviceInterface_PIT(void)
{
    if (PITInfo.initialized != TRUE)
        return;

    jiffies++;  // Increment global tick counter

// Update the seed for rand() function.
// Ring0 only
// see: kstdlib.c
    unsigned int new_seed = (unsigned int) (jiffies & 0xFFFFFFFF);
    srand(new_seed);

// The master timer.
// see: grinput.c
    wmTimerEvent(1234);
}

//-------------------------

/*
// read back
unsigned read_pit_count(void);
unsigned read_pit_count(void) 
{
	unsigned count = 0;
 
	// Disable interrupts
	cli();
 
	// al = channel in bits 6 and 7, remaining bits clear
	outb(0x43,0b0000000);
 
	count = inb(0x40);		// Low byte
	count |= inb(0x40)<<8;		// High byte
 
	return count;
}
*/

/*
void set_pit_count(unsigned count);
void set_pit_count(unsigned count) 
{
	// Disable interrupts
	cli();
 
	// Set low byte
	outb(0x40,count&0xFF);		// Low byte
	outb(0x40,(count&0xFF00)>>8);	// High byte
	return;
}
*/


/*
 * timerInit8253:
 *    @todo: Compreender melhor isso.
 * Seta a frequência de funcionamento do 
 * controlador 8253. "3579545 / 3" 
 * instead of 1193182 Hz. 
 * Reprograma o timer 8253 para operar 
 * à uma frequencia de "HZ".
 * Obs: Essa rotina substitui a rotina init_8253.
 */

//ex: 
// 0x36
// 00 | 11 | 011 | 0
// Channel 0 | word | square wave generator | 16bit binary

/*
Bits         Usage
6 and 7      Select channel :
                0 0 = Channel 0
                0 1 = Channel 1
                1 0 = Channel 2
                1 1 = Read-back command (8254 only)
4 and 5      Access mode :
                0 0 = Latch count value command
                0 1 = Access mode: lobyte only
                1 0 = Access mode: hibyte only
                1 1 = Access mode: lobyte/hibyte
1 to 3       Operating mode :
                0 0 0 = Mode 0 (interrupt on terminal count)
                0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                0 1 0 = Mode 2 (rate generator)
                0 1 1 = Mode 3 (square wave generator)
                1 0 0 = Mode 4 (software triggered strobe)
                1 0 1 = Mode 5 (hardware triggered strobe)
                1 1 0 = Mode 2 (rate generator, same as 010b)
                1 1 1 = Mode 3 (square wave generator, same as 011b)
0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/


// timerInit8253:
// VT8237
// Compativel com Intel 8254.
// IN> frequence in HZ.
// see: pit.h
void timerInit8253 (unsigned int freq)
{
    unsigned int clocks_per_sec = 0;
// para 1.1 MHz
// The counter counts down to zero, then 
// sends a hardware interrupt (IRQ 0) to the CPU.
// 3579545/3
    unsigned int period = 0;

    PITInfo.initialized = FALSE;
    clocks_per_sec = (unsigned int) (freq & 0xFFFFFFFF);
    if (clocks_per_sec == 0){
        x_panic("timerInit8253:");
    }
    PITInfo.clocks_per_sec = clocks_per_sec;
    // latch
    period = (unsigned int) (PIT_DEV_FREQ / clocks_per_sec);
    PITInfo.dev_freq = (unsigned int) PIT_DEV_FREQ;
    PITInfo.period = period;

// 0x36
// 00 | 11 | 011 | 0
// Channel 0 | word | square wave generator | 16bit binary
    out8 ( 0x43, (unsigned char) 0x36 );
    io_delay();
// LSB
    out8 ( 0x40, (unsigned char)(period & 0xFF) ); 
    io_delay();
// MSB
    out8 ( 0x40, (unsigned char)(period >> 8) & 0xFF );
    io_delay();

    PITInfo.initialized = TRUE;

    // #debug
    //printk("Dev freq: %d | Clocks per sec: %d HZ| Period: %d\n",
    //    PITInfo.dev_freq,
    //    PITInfo.clocks_per_sec,
    //    PITInfo.period );
    //refresh_screen();
    //while(1){}
}

// set_timeout: #todo 
void set_timeout ( unsigned long ticks )
{
    time_out = (unsigned long) ticks;
}

// get_timeout: #todo
unsigned long get_timeout (void)
{
    return (unsigned long) time_out;
}

/*
 * get_systime_info:
 */
// #todo 
// Criar um enum para isso.
// Comentar uma descrição em cada item.

unsigned long get_systime_info(int n)
{
    switch (n){
    case 1: return (unsigned long) get_hz();            break;
    case 2: return (unsigned long) get_milliseconds();  break;
    case 3: return (unsigned long) get_ticks();         break;
    // ...
    default:  
        return (unsigned long) 0;  
        break;
    };

    // fail.
    return (unsigned long) 0;
}

int new_timer_id(void)
{
    register int i=0;
    unsigned long address=0;
// Probe for an empty spot.
    for (i=0; i<32; i++)
    {
        // Get a structure pointer.
        address = (unsigned long) timerList[i];
        if (address == 0){
            return (int) i;
        }
    };
// fail
    return (int) -1;
}

// fake
void pit_sleep (unsigned long ms)
{
    unsigned long t=1;
    if (ms == 0){
        ms=1;
    }
// #bugbug
// Is there a limit?
    t = (unsigned long) (ms * 512);
    while (t > 0)
    {
        t--;
    };
}

/*
 * timerInit:
 *     Inicializa o driver de timer.
 *     Inicializa as variáveis do timer.
 * (unsigned long CallBackExemplo); 
 */
int timerInit(void)
{
    panic("timerInit: #deprecated");
    return 0;

/*
    int i=0;

    // g_driver_timer_initialized = FALSE;
// Breaker
    __breaker_timer_initialized = FALSE;
    __timerTimer();
    for ( i=0; i<32; i++ ){
        timerList[i] = (unsigned long) 0;
    }
    // timerLock = 0;
    //set handler.
    // @todo: Habilitar esse configuração pois é mais precisa.
    //config frequências...
    //@todo: Isso poderia ser por último.
    //?? Isso pertence a i386 ??
    //?? Quais máquinas possuem esse controlador ??
// #importante
// Começaremos com 100 HZ
// Mas o timer poderá ser atualizado por chamada.
// e isso irá atualizar a variável que inicializamos agora.
    timerInit8253(HZ);
// #todo:
// alocar memoria para a estrutura do timer.
// inicializar algumas variaveis do timer.
// por enquanto estamos usando variaveis globais.
// ?? Não se se ja foi configurado o timer.
// ou devemos chamr init_8253() agora. ou depois.
    //timerCountSeconds = 0;

// Timeout 
    set_timeout(0);
// Whatchdogs
// Initializing whatchdogs.
// Eles serão zerados pelas interrupções dos dipositivos e
// incrementados pelo timer.
// A condição crítica é alcançar um limite, um timeout.

    ____whatchdog_ps2_keyboard = 0;
    ____whatchdog_ps2_mouse = 0;
    //...
// breaker
    __breaker_timer_initialized = TRUE;
// Done
    g_driver_timer_initialized = TRUE;
    return 0;
*/
}


/*
 * early_timer_init:
 *     Inicialização prévia do módulo timer.
 *     Uma outra inicialização mais apurada poderá ser feita
 * posteriormente.
 *     Porém ainda existe uma inicialização feita em Assembly
 * quando o kernel é inicialazado.
 */
// Called by hal.c

int early_timer_init (void)
{
    register int i=0;

    //g_driver_timer_initialized = FALSE;
// Breaker
    __breaker_timer_initialized = FALSE;
    __timerTimer();
    for (i=0; i<32; i++){
        timerList[i] = (unsigned long) 0;
    };

// == Hz ============================================
// Setup the controller.
// And setup the controler.
// We can use the default variable. 
// See: config.h
    timerInit8253(JIFFY_FREQ);

// Timeout
    set_timeout(0);
// Whatchdogs
// Initializing whatchdogs.
// Eles serão zerados pelas interrupções dos dipositivos e
// incrementados pelo timer.
// A condição crítica é alcançar um limite, um timeout.
    ____whatchdog_ps2_keyboard = 0;
    ____whatchdog_ps2_mouse = 0;
    //...
    // Continua...
// Breaker
    __breaker_timer_initialized = TRUE;
// Done
    g_driver_timer_initialized = TRUE;
    return 0;
}


/*
struct timer_d *timerObject();
struct timer_d *timerObject()
{
	//@todo: criar uma estrutura e retornar o ponteiro.
	return (struct timer_d *) x;
}
*/


/*
 * create_timer:
 */
// IN: pid, ms, type

struct timer_d *create_timer ( 
    pid_t pid, 
    unsigned long ms, 
    int type )
{
    struct timer_d   *Timer;
    struct te_d *Process;
    struct thread_d  *Thread;
    int ID = -1;  //erro;

    debug_print("create_timer:\n");
    //printk     ("create_timer: pid=%d ms=%d type=%d\n",
    //    pid,ms,type);

// --------------

    if (pid<0 || pid >= PROCESS_COUNT_MAX){
        debug_print("create_timer: [FAIL] pid\n");
        return NULL;
    }
    Process = (struct te_d *) teList[pid];
    if ((void*) Process == NULL){
        debug_print("create_timer: [FAIL] Process\n");
        return NULL;
    }
// --------------
    if (current_thread<0 || current_thread >= THREAD_COUNT_MAX){
        debug_print("create_timer: [FAIL] current_thread\n");
        return NULL;
    }
    // The flower thread
    //Thread = (struct thread_d *) Process->flower;
    Thread = (struct thread_d *) threadList[current_thread];
    if ((void*) Thread == NULL){
        debug_print("create_timer: [FAIL] Thread\n");
        return NULL;
    }

// Ajust minimum
    if (ms == 0)
    {
        //ms = 1;
        return NULL;
    }

    // type
    if ( type < 1 || type > 10 )
    {
        panic ("create_timer: type fail\n");
        //printk("create_timer: type fail\n");
        //refresh_screen ();
        //return NULL;
    }

// Structure

    Timer = (void *) kmalloc( sizeof(struct timer_d) );
    if ((void *) Timer == NULL){
        panic("create_timer: Timer fail \n");
        //printk ("create_timer: Timer fail \n");
        //refresh_screen ();
        //return NULL; 
    }else{
        
        // ??
        // List?
        ID = (int) new_timer_id();
        // Erro ao obter um novo id.
        if (  ID < 0 || ID > 32 ){
            panic ("create_timer: ID fail \n");
            //printk("create_timer: ID fail \n");
            //refresh_screen ();
            //return NULL;
        }else{

            // #todo
            // memset (Timer, 0, sizeof(struct timer_d) );

            Timer->used = TRUE;
            Timer->magic = 1234;
            Timer->id = ID;

            // #bugbug
            Timer->initial_count_down = ms;
            // #bugbug
            Timer->count_down = Timer->initial_count_down;

            //1 = one shot 
            //2 = intermitent
            Timer->type = (int) type;
            
            // Pegamos logo acima.
            Timer->thread  = (struct thread_d *)  Thread;
            Timer->process = (struct te_d *) Process;
            Timer->pid     = pid;
            Timer->tid     = current_thread;

            //printk("create_timer: done t={%d} :) \n",
            //    Timer->initial_count_down);

            // Coloca na lista.
            timerList[ID] = (unsigned long) Timer;
        };
    };

    // #debug
    debug_print("create_timer: done\n");
    printk("create_timer: done\n");

    return (struct timer_d *) Timer;

fail:
    debug_print("create_timer: FAIL\n");
    printk     ("create_timer: FAIL\n");
    return NULL;
}

