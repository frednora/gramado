// pit.h
// Programmable Interval Timer (PIT) - (Intel 8253/8254) 
// 2013 - Created by Fred Nora.


#ifndef __KE_PIT_H
#define __KE_PIT_H    1

// Seconds      (s)  1
// Milliseconds (ms) 1.000
// Microseconds (µs) 1.000.000
// Nanoseconds  (ns) 1.000.000.000

/*
#define MODE_COUNTDOWN 0x00
#define MODE_ONESHOT 0x02
#define MODE_RATE 0x04
#define MODE_SQUARE_WAVE 0x06

#define BASE_FREQUENCY 1193182

*/

//
// Counting variables support.
//

#define PIT_DEV_FREQ  1193182

// Hz
// see: jiffies.h
#define HZ                JIFFY_FREQ
#define PIT_HZ            JIFFY_FREQ
#define DEFAULT_PIT_FREQ  JIFFY_FREQ

// Tranca
// #define LATCH (1193182/HZ)

struct pit_info_d
{
    int initialized;
    unsigned int dev_freq;        //1193180
    unsigned int clocks_per_sec;  //hz
    unsigned int period;
};
//see: pit.c
extern struct pit_info_d  PITInfo;


// see: pit.c
// total ticks
extern unsigned long jiffies;
// por quantos segundos o sistema esta rodando
// jiffies/sys_time_hz
extern unsigned long seconds;
// Por quantos ms o sistema esta rodando.
extern unsigned long sys_time_ms;

// pit frequency
// See: system.c
extern unsigned long sys_time_hz;

//
// Profiler set support.
// 

// Isso será usado pelo profiler para
// medir a quantidade de páginas que possuem frames em um determinado
// período de tempo.
// Também será usado pelo profiler para saber quantas vezes 
// Uma thread rodou num determinado período de tempo.
// Etc.
// Será o tempo de referência para o profiler.

#define PROFILER_TICKS_DEFAULT_LIMIT  200

// Contador do profiler ticks.
// Só vai de zero até o limite configurável.
unsigned long profiler_ticks_count;

// Limite da contagem do profile ticks.
// Quando chegar aqui, então devemos calcular 
// a porcentagem para todas as threads e processos.
unsigned long profiler_ticks_limit;
unsigned long profiler_percentage_all_normal_threads;
unsigned long profiler_percentage_idle_thread;

// Usado no Linux. 
//#define CT_TO_SECS(x)   ((x) / HZ)
//#define CT_TO_USECS(x)  (((x) % HZ) * 1000000/HZ) 
//... 

// Em que tempo estávamos quando iniciamos a contagem;
//#define TIMER_PROFILE_TICKS_DEADLINE 1000
//unsigned long timer_profile_ticks;

//Counting how much ticks the kernel is running.
//unsigned long kernel_tick;
 
// #extra
// Aciona uma flag para que o ts.c realize atividades extras,
// como gc, dead thread collector, request.

//#todo: maybe move these to ts.h or something like that.
int extra; 
//int weGotKeyboardInput;

// timer pathname.
// #define TIMER_DRIVER_PATHNAME  "Embedded"  

/*
040-05F = 8253 or 8254 Programmable Interval Timer (PIT, see ~8253~)
040     = 8253 channel 0, counter divisor
041     = 8253 channel 1, RAM refresh counter
042     = 8253 channel 2, Cassette and speaker functions
043     = 8253 mode control  (see 8253)
044     = 8254 PS/2 extended timer
047     = 8254 Channel 3 control byte
*/

/*
 * Como o linux define um Quantum (time slice) para tasks RR.
 */

/*
 * default timeslice is 100 msecs (used only for SCHED_RR tasks).
 * Timeslices get refilled after they expire.
 */
// #define RR_TIMESLICE    (100 * HZ / 1000)

//a flag indica se o cursor está habilitado ou não.
//int timer_cursor_used;
//status do cursor.
//int timer_cursor_status;
unsigned long time_out;
unsigned long timer_handler_address;    //global _irq0:

/*
 * timer_d:
 * Estrutura do objeto timer que será usado pelos aplicativos.
 * Precisamos identificar quem está usando para podermos enviar 
 * mensagem para quem possui o timer.
 */

struct timer_d 
{
    object_type_t   objectType;
    object_class_t  objectClass;

    int used;
    int magic;
    int id;

//1 = one shot 
//2 = intermitent
    int type;

// Owner
    struct process_d *process;
    struct thread_d  *thread;
    pid_t pid;
    tid_t tid;

// Counter
    int count_down;          // Decrementing
    int initial_count_down;  // base fixa
// Quantas vezes o timer se esgotou.
    unsigned long times;

    // ??
    int status;

    unsigned long flag;
    unsigned long error;

// Navigation
    struct timer_d  *next;
};

// struct timer_d *XXXXXXTimer;

// #todo: 
// Put this in pit.c and put here just an external reference.
// lista de timers.
unsigned long timerList[32];

//
// == prototypes ===========================
//

void DeviceInterface_PIT(void); 
void timerInit8253(unsigned int freq);

void set_timeout(unsigned long ticks);
unsigned long get_timeout(void);

void set_quantum(unsigned long q);
unsigned long get_quantum(void);

void set_current_quantum(unsigned long q);
unsigned long get_current_quantum(void);

void set_next_quantum(unsigned long q);
unsigned long get_next_quantum(void);

void set_systime_hz(unsigned long hz);
unsigned long get_systime_hz(void);

unsigned long get_systime_ms(void);

unsigned long get_systime_totalticks(void);

unsigned long get_systime_info(int n);

int new_timer_id(void);

void timerEnableTextCursor(void);
void timerDisableTextCursor(void);

unsigned long now(void);

void pit_sleep(unsigned long ms);

void pit_speaker_on (void);
void pit_speaker_off (void);


int timerInit(void);
int early_timer_init(void);

struct timer_d *create_timer ( 
    pid_t pid, 
    unsigned long ms, 
    int type );

#endif    

