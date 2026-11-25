// timer.c
// Timer support for the boot loader.
// #ps: No plains for this.
// 2015 - Created by Fred Nora.

/*
 * Handler da irq0 para o Boot Loader.
 * 32bit bootloader.
 * O timer no Boot Loader n�o faz task switch.
 * O timer ser� usado principalmente para timeout de driver.
 * O timer pode ser usado para contar tempo de inicializa��o.
 */

#include "../bl.h"


unsigned long timerTicks=0;

// irq handler.
// Increment jiffies.
void timer()
{
    timerTicks++;
    //if ( timerTicks % 100 == 0 )
    //{
        // Nothing ...
    //}
}

// blTimerInit:
// Inicializa o m�dulo timer.
int blTimerInit()
{
    int Status=0;

    timerTicks = 0;
    //...
    return (int) Status;
}

//
// End
//

