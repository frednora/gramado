// gre00.h
// Created by Fred Nora.

// Window system support.
// ring0, kernel base.
// #todo
// Tem muita coisa aqui nesse documento para ser deletado.
// Que era de quando existia gui dentro do kernel.
// Talvez alguma coisa sobre janela seja preservado,
// pois vamos dar suporte ao conceito de 'surfaces',
// que serao areas de memoria compartilhadas entre o painter-client side 
// e o compositor, server-side. 

#ifndef __GRE_GRE00_H
#define __GRE_GRE00_H    1

// 
// Window type
//

#define WT_NULL          0 
#define WT_SIMPLE        1
// Igual simples, mais uma bordinha preta.
#define WT_EDITBOX       2
// Sobreposta (completa)(barra de titulo + borda +client area). 
#define WT_OVERLAPPED    3  
// Um tipo especial de sobreposta, usada em dialog ou message box. 
// (com ou sem barra de titulo ou borda)
#define WT_POPUP         4
// Caixa de seleção. Caixa de verificação. 
// Quadradinho.  
#define WT_CHECKBOX      5
// Cria uma scroll bar. 
// Para ser usada como janela filha.  
#define WT_SCROLLBAR     6  
#define WT_EDITBOX_MULTIPLE_LINES 7
#define WT_BUTTON        8   
#define WT_STATUSBAR     9
// Pequeno retângulo com uma imagem bmp e talvez texto.
#define WT_ICON          10
#define WT_TITLEBAR 11
//... 

// window style
#define WINDOW_STYLE_FLOATING  1000
#define WINDOW_STYLE_DOCKING   2000
//...

// Window status
#define WINDOW_STATUS_ACTIVE       1
#define WINDOW_STATUS_INACTIVE     0
//...

/* rgba */
struct tagRGBA
{
    object_type_t  objectType;
    object_class_t objectClass;
   char red;
   char green;
   char blue;
   char alpha;
};

//--------------------------------------

// Isso pode ser útil principalmente
// para passar um retângulo de um ambiente para outro.
// É muito mais didático que a figura do retângulo como objeto.
struct surface_d
{
    int used;
    int magic;
// Surface ID.
    int surface_id;
    struct rect_d rect;
// How many bytes?
    size_t size_in_bytes;
    int bpp;
    unsigned long width;
    unsigned long pitch;
    unsigned long height;

// Window ID.
// This window owns this surface.
// The wid provided by the window server.
    int owner_wid;

    int dirty;

    struct surface_d *next;
};

//--------------------------------------
// Ponteiros para ícones
// Ponteiros para o endereço onde os ícones 
// foram carregados.
// queremos saber se o endereço alocado eh compartilhado ...
// para o window server usar ... entao chamaremos de sharedbufferIcon.
// see: kgwm.c

// Icon cache structure.
// see: kgwm.c
struct icon_cache_d
{
    int initialized;
    size_t size_in_bytes;
// Pointers to shared memory.
    void *app;  //1
    void *file; 
    void *folder;
    void *terminal;
    void *cursor;  //5
};
extern struct icon_cache_d  icon_cache;


// gui:
// 2015 - Created by Fred Nora.
struct gui_d
{
    int initialized;

// redraw
// Flag para repintar todas as janelas.
// #todo: #bugbug, Isso parece estranho. Cada janela
// está pintada em seu buffer dedicado e fica por conta de
// cada janela decidir se repinta ou não apenas ela.

    int redraw;

// refresh
// Flag para enviar do backbuffer para a memória de video.
// Seguindo uma lista linkada, copiaremos o conteúdo do buffer
// dedicado de cada janela da lista no LFB. Primeiro é Backbuffer
// que é o buffer da janela principal, que funcionará como
// Head da lista.

    int refresh;

// Security
    struct usession_d  *user_session;
// #todo: cgroup spport
    // struct cgroup_d  *cgroup;
};
extern struct gui_d  *gui; 

//
// == prototypes ================
//

#endif    

