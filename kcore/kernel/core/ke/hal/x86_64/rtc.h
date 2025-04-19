// rtc.h
// IRQ 8 – real-time clock (RTC)
// Created by Fred Nora.

#ifndef __KE_RTC_H
#define __KE_RTC_H    1

// #todo:
// Podemos criar uma lista para todos os dispositivos em pic.h
#define  RTC_IRQ   8 
// Ports 
#define RTC_ADDRESS_PORT  0x70 
#define RTC_DATA_PORT     0x71
//Time
#define RTC_SEC    0x00  //*RTC seconds. 
#define RTC_MIN    0x02  //*RTC minutes. 
#define RTC_HOUR   0x04  //*RTC hours. 
#define RTC_WDAY   0x06  //RTC weekday. 
#define RTC_MDAY   0x07  //*RTC day of month. 
#define RTC_MON    0x08  //*RTC month. 
#define RTC_YEAR   0x09  //*RTC year. 
//Status
#define RTC_STATUS_A   0x0A  //RTC status register A. 
#define RTC_STATUS_B   0x0B  //RTC status register B. 
#define RTC_STATUS_C   0x0C  //RTC status register C. 
#define RTC_STATUS_D   0x0D  //RTC status register D. 

//Memory
#define RTC_LOWBYTE_BASEMEMORY       0x15  //Low. (Low base memory byte)
#define RTC_HIGHBYTE_BASEMEMORY      0x16  //High. (High base memory byte)
#define RTC_LOWBYTE_EXTENDEDMEMORY   0x30  //Low. (Low extended memory byte)
#define RTC_HIGHBYTE_EXTENDEDMEMORY  0x31  //High. (High extended memory byte)

//0x0e até 0x7f 114 bytes de used ram. ?? (intel) ep0579 us15w

//Outros
#define RTC_STATUS_A_UPDATE_IN_PROGRESS  0x80  //RTC update in progress bit. 
#define RTC_STATUS_B_24_HOUR  0x02  //RTC 24 hour format bit. 
#define RTC_STATUS_B_BINARY   0x04  //RTC binary mode bit. 
#define RTC_STATUS_B_PIE      0x40  //RTC Periodic Interrupt Enabled bit. 
//...

//
// RTC Structure.
//
 
struct rtc_d
{
    object_type_t objectType;
    object_class_t objectClass;
    int used;
    int magic;

    unsigned long flag;
    unsigned long error;
    unsigned long time;
    unsigned long date;

    unsigned long Seconds;
    unsigned long Minutes;
    unsigned long Hours;

    unsigned long DayOfMonth;
    unsigned long Month;
    unsigned long Year;
};
extern struct rtc_d *rtc;


//
// == prototypes =======================
//

unsigned long rtc_get_date(void);
unsigned long rtc_get_time(void);

unsigned short rtcGetBaseMemory(void);
unsigned short rtcGetExtendedMemory(void);

// Device driver initialization.
int DDINIT_rtc(void);

#endif    

