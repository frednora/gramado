// gre01.h
// Created by Fred Nora.

// Window system support.
// Colors and color scheme.

#ifndef __GRE_GRE01_H
#define __GRE_GRE01_H    1


// # Principais variáveis globais #
// This is not the right place for them.

// see: graphics.c
extern int current_display; 
extern int current_screen;
extern int guiStatus;
// Status de ambientes gráficos.
extern int logonStatus;              //Logon status.
extern int logoffStatus;             //Logoff status.
extern int userenvironmentStatus;    //User environment status.

extern unsigned long g_system_color;
extern unsigned long g_char_attrib;
// LFB - address for kernel graphic mode
extern unsigned long g_kernel_lfb; 
//video mode
extern unsigned long g_current_vm;          //video memory
extern unsigned long g_current_video_mode;  //video mode
//status do cursor.
//se ele deve aparecer e piscar ou não.
extern int g_show_text_cursor;
//status: aceso ou apagado.
//0=apaga 1=acende.
extern int textcursorStatus;      
extern unsigned long g_mousepointer_x;
extern unsigned long g_mousepointer_y;


#endif    

