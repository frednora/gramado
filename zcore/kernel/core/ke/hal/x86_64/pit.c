// pit.c
// Programmable Interval Timer (PIT) - (Intel 8253/8254) 
// 2013 - Created by Fred Nora.

// See:
// https://wiki.osdev.org/Programmable_Interval_Timer
// https://en.wikipedia.org/wiki/Intel_8253

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



#include <kernel.h>  

// see: pit.h
struct pit_info_d  PITInfo;

// Total ticks. Global.
unsigned long jiffies=0;
// por quantos segundos o sistema esta rodando
// jiffies/sys_time_hz
unsigned long seconds=0;
// Por quantos ms o sistema esta rodando.
unsigned long sys_time_ms=0;


/*
struct master_timer_d
{
    int initialized;
    pid_t pid;
    unsigned long callback_address;
};
struct master_timer_d  MasterTimer;
*/


//Contador de ticks.
//unsigned long timerTicks;

//??  
//int timerColor;

//??
//unsigned long timerLine;

//??
//unsigned long timerIdleState;

//??
//int timerLock;

//??
//int timerError;

// #deprecated
// Text cursor
static int timerShowTextCursor = FALSE;


//??
//unsigned long timerCountSeconds;  //Count Seconds.
//...

//
// == Private functions: prototypes ===================
//

static int timerTimer(void);

// =============================

static int timerTimer(void)
{
// Total ticks
    jiffies = 0;
// Por quantos segundos o sistema esta rodando
// jiffies/sys_time_hz
    seconds = 0; 
// Por quantos ms o sistema esta rodando.
    sys_time_ms = 0; 
// Pit frequency
    sys_time_hz = 0;
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
 *     Chama o handler do kernel que está no kernel base.
 * #todo: Observar alguns procedimentos antes de chamar a rotina.
 */
// Called by:
// _irq0 in hw.asm
// It is up to the interrupt service routine to reset the latch. 
// It does that by setting bit 7 of port 0x61 (system control port B).

__VOID_IRQ 
irq0_TIMER (void)
{

// Calling the timer routine.
// See: pic.h
    DeviceInterface_PIT();

// Calling the taskswitching routine.
// See: ps/disp/ts.c
// #todo #bugbug
// nesse momento pode acontecer que teremos que
// efetuar spawn de uma thread que esta em standby,
// O nosso contexto ja estara salvo quando formos efetuar
// o spawn, mas precisamos avisar a rotina de spawn
// que precisamos realizar o EOI pois estamos num handler de pit.

// The spawn routine need to make an eoi for pit interrupt.
// Tell the spawn routine that we need an eoi.
// In the case of spawning a new thread.
    spawn_set_eoi_state();

// See: disp/ts.c
    tsTaskSwitch();

// The spawn routine do not need to make a eoi.
// Tell the spawn routine that we do not need eoi anymore.
// The assembly routine will do that for us.
    spawn_reset_eoi_state();
}


void DeviceInterface_PIT(void)
{
// The pit handler.
// Called by irq0_TIMER().

    if (PITInfo.initialized != TRUE)
        return;

// Increment tick counter.
    jiffies++;

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
// GLOBAL VARIABLE.
    set_systime_hz(freq);
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

// set_quantum:
void set_quantum ( unsigned long q)
{
    quantum = (unsigned long) q;
}

// get_quantum:
unsigned long get_quantum (void)
{
    return (unsigned long ) quantum;
}


// set_current_quantum: 
void set_current_quantum (unsigned long q)
{
    current_quantum = (unsigned long) q;
}

// get_current_quantum:
unsigned long get_current_quantum (void)
{
    return (unsigned long ) current_quantum;
}

// set_next_quantum:
void set_next_quantum (unsigned long q)
{
    next_quantum = (unsigned long) q;
}

// get_next_quantum:
unsigned long get_next_quantum (void)
{
    return (unsigned long ) next_quantum;
}  


void set_systime_hz ( unsigned long hz )
{
    sys_time_hz = hz;
}

// systime hz
unsigned long get_systime_hz (void)
{
    return (unsigned long) sys_time_hz;
}

// systime in ms
unsigned long get_systime_ms(void)
{
    return (unsigned long) sys_time_ms;
}

// get_systime_totalticks:
unsigned long get_systime_totalticks(void)
{
    return (unsigned long) jiffies;
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
    case 1: return (unsigned long) get_systime_hz();          break;
    case 2: return (unsigned long) get_systime_ms();          break;
    case 3: return (unsigned long) get_systime_totalticks();  break;
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

void timerEnableTextCursor (void)
{
    timerShowTextCursor = TRUE;
}

void timerDisableTextCursor (void)
{
    timerShowTextCursor = FALSE;
}

// systime in ms
unsigned long now(void)
{
    return (unsigned long) get_systime_ms();
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
    timerTimer();
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
// Quantum
    set_current_quantum (QUANTUM_MIN);
    set_next_quantum (QUANTUM_MIN);
    set_quantum (QUANTUM_MIN);
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
    timerTimer();
    for (i=0; i<32; i++){
        timerList[i] = (unsigned long) 0;
    };

// == Hz ============================================
// Setup the controller.
// Let's setup the variable sys_time_hz.
// And setup the controler.
// We can use the default variable. 
// See: config.h
    timerInit8253(HZ);

// Quantum
// #bugbug
// Is this the right place for that configuration?
// Is it about thread configuration?
    set_current_quantum(QUANTUM_MIN);
    set_next_quantum(QUANTUM_MIN);
    set_quantum(QUANTUM_MIN);

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
    struct process_d *Process;
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
    Process = (struct process_d *) processList[pid];
    if ((void*) Process == NULL){
        debug_print("create_timer: [FAIL] Process\n");
        return NULL;
    }
// --------------
    if (current_thread<0 || current_thread >= THREAD_COUNT_MAX){
        debug_print("create_timer: [FAIL] current_thread\n");
        return NULL;
    }
    // Thread de controle.
    //Thread = (struct thread_d *) Process->control;
    Thread = (struct thread_d *) threadList[current_thread];
    if ((void*) Thread == NULL){
        debug_print("create_timer: [FAIL] Thread\n");
        return NULL;
    }

// limits
// limite de 1 tick.

    // ms
    if (ms < (JIFFY_FREQ/sys_time_hz) )
    {
        printk ("create_timer: Ajust ms\n");
        ms = (JIFFY_FREQ/sys_time_hz);
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

            // ms/(ms por tick)
            Timer->initial_count_down = (unsigned long) ( ms / (JIFFY_FREQ/sys_time_hz) );
            Timer->count_down = Timer->initial_count_down;

            //1 = one shot 
            //2 = intermitent
            Timer->type = (int) type;
            
            // Pegamos logo acima.
            Timer->process = (struct process_d *) Process;
            Timer->thread  = (struct thread_d *)  Thread;
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
    refresh_screen();

    return (struct timer_d *) Timer;

fail:
    debug_print("create_timer: [FAIL]\n");
    printk     ("create_timer: [FAIL]\n");
    refresh_screen();
    return NULL;
}


