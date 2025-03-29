
// ke.h

#ifndef __KE_KE_H
#define __KE_KE_H    1


int sys_signal(void);

void keRefreshScreen(void);
void 
keDrawString ( 
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string );

unsigned long keGetSystemMetrics(int index);
void keDie(void);
void keSoftDie(void);
int keIsQemu(void);

int keCloseInitProcess(void);
void keReboot(void);
void ke_x64ExecuteInitialProcess(void);
int keInitialize(int phase);

#endif  

