// surface.h
// Created by Fred Nora.

#ifndef __GRAMADO_SURFACE_H
#define __GRAMADO_SURFACE_H    1

// #test
// Um retângulo criado pelo kgws em ring0 
// pode ser representado aqui na forma de surface, 
// e o servidor poderá solicitar ao kgws 
// operações sobre essa surface.

struct gws_surface_d
{
    int used;
    int magic;
    
    // Pointer to the window in kgws.
    // Redirection?
    void *window_object;
    
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

// Só depois de inicializada os valores da janela são validos.
    int initialized;
    int dirty;
    int locked;
    // ...

    struct gws_surface_d *next;
};

extern struct gws_surface_d  *rootSurface;

// ============================================


void demoClearSurface(unsigned int color);
void demoFlushSurface(void);


void *xxxCreateSurfaceWindow( 
    unsigned long type,        // 1, Tipo de janela (popup,normal,...)
    unsigned long status,      // 2, Estado da janela (ativa ou nao)
    unsigned long view,        // 3, (min, max ...)
    char *windowname,          // 4, Título. 
    unsigned long x,           // 5, Deslocamento em relação às margens do Desktop. 
    unsigned long y,           // 6, Deslocamento em relação às margens do Desktop.
    unsigned long width,       // 7, Largura da janela.
    unsigned long height,      // 8, Altura da janela.
    void *pSurfaceWindow,      // 9, Endereço da estrutura da janela mãe.
    unsigned long desktop_id,  //10, Desktop id.
    unsigned long clientcolor, //11, Cor da área de cliente
    unsigned long color );      //12, Color (bg) (para janela simples).

struct gws_surface_d *xxxCreateSurface( 
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    unsigned long color );



#endif   

