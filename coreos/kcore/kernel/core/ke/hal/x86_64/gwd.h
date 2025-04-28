
// gwd.h
// Incrementamos de tempos em tempos pra saber se
// esses eventos não estão ocorrendo.

#ifndef __KE_GWD_H
#define __KE_GWD_H  1
    
    // #importante
    // Essas variáveis são inicializadas pelo timer.
    // See: kdrivers/timer.c

    // Initializing whatchdogs.
    // Eles serão zerados pelas interrupções dos dipositivos e
    // incrementados pelo timer.
    // A condição crítica é alcançar um limite, um timeout. 

int ____whatchdog_ps2_keyboard;
int ____whatchdog_ps2_mouse;
//...
    
#endif    

