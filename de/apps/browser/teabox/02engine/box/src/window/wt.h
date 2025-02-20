
// wt.h
// Window Types

#ifndef ____WT_H
#define ____WT_H    1

#define WT_NULL          0 
#define WT_SIMPLE        1
// Igual simples, mais uma bordinha preta.
#define WT_EDITBOX_SINGLE_LINE  2
#define WT_EDITBOX  WT_EDITBOX_SINGLE_LINE
// Sobreposta(completa)(barra de titulo + borda + client area)  
#define WT_OVERLAPPED    3  
// Um tipo especial de sobreposta,
// usada em dialog ou message box. 
// (com ou sem barra de titulo ou borda)
#define WT_POPUP         4  
// Caixa de seleção. Caixa de verificação. Quadradinho.
#define WT_CHECKBOX      5
// Cria uma scroll bar. Para ser usada como janela filha.
#define WT_SCROLLBAR     6 
#define WT_EDITBOX_MULTIPLE_LINES 7
#define WT_BUTTON     8   
#define WT_STATUSBAR  9
#define WT_ICON       10
// #define WT_SURFACE    11
// #define WT_MAX (WT_SURFACE+1)

#endif    



