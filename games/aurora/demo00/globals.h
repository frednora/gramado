
// globals.h


#ifndef __GWSSRV_GLOBALS_H
#define __GWSSRV_GLOBALS_H   1


// Host Operating System
#define OS_OTHER    0
#define OS_NATIVE   1
#define OS_LINUX    2
#define OS_WINDOWS  3

// Virtual Machine
#define VM_OTHER       0
#define VM_NATIVE      1
#define VM_VMWARE      2
#define VM_QEMU        3
#define VM_VIRTUALBOX  4


// =========================
// h:d.s
extern char *host_name;
extern char *display_number;
extern char *screen_number;

extern int gUseLeftHand;
extern int gUseCallback;

extern unsigned long starting_tick;

// ============================
// Modes.
#define GRAMADO_JAIL        0x00
#define GRAMADO_P1          0x01
#define GRAMADO_HOME        0x02
#define GRAMADO_P2          0x03
#define GRAMADO_CASTLE      0x04
#define GRAMADO_CALIFORNIA  0x05
extern int current_mode;


// ## button support ##
#define GWS_COLOR_BUTTONFACE            0x00F0F0F0
#define GWS_COLOR_BUTTONFACE2           0x00E0E0E0
#define GWS_COLOR_BUTTONFACE3           0x00D0D0D0
#define GWS_COLOR_BUTTONHIGHLIGHT       0x00FFFFFF
#define GWS_COLOR_BUTTONHIGHLIGHT2      0x00404040
#define GWS_COLOR_BUTTONHIGHLIGHT3      0x00008080
#define GWS_COLOR_BUTTONHIGHLIGHTTEXT   COLOR_WHITE
#define GWS_COLOR_BUTTONHIGHLIGHTTEXT2  COLOR_WHITE 
#define GWS_COLOR_BUTTONSHADOW          0x00A0A0A0
#define GWS_COLOR_BUTTONSHADOW2         0x00303030 
#define GWS_COLOR_BUTTONSHADOW3         0x00202020 
#define GWS_COLOR_BUTTONTEXT            COLOR_TEXT


// #importante
// usaremos essas definições para configurarmos
// os dois esquemas básicos, humility e pride.
// Apenas poucas cores devem ser definidas aqui. Elas
// servirão de base para outros conjuntos. Devem ser definidas
// com números.

#define HUMILITY_COLOR_BACKGROUND                 0x00008080 
#define HUMILITY_COLOR_WINDOW                     0x00FFFFFF 
#define HUMILITY_COLOR_WINDOW_BACKGROUND          0x00202020 
#define HUMILITY_COLOR_ACTIVE_WINDOW_BORDER       0x00404040 
#define HUMILITY_COLOR_INACTIVE_WINDOW_BORDER     0x00606060 
#define HUMILITY_COLOR_ACTIVE_WINDOW_TITLEBAR     0x00404040 
#define HUMILITY_COLOR_INACTIVE_WINDOW_TITLEBAR   0x00606060 
#define HUMILITY_COLOR_MENUBAR                    0x00808080 
#define HUMILITY_COLOR_SCROLLBAR                  0x00FFF5EE 
#define HUMILITY_COLOR_STATUSBAR                  0x0083FCFF 
#define HUMILITY_COLOR_MESSAGEBOX                 0x00404040 
#define HUMILITY_COLOR_SYSTEMFONT                 0x00000000 
#define HUMILITY_COLOR_TERMINALFONT               0x00FFFFFF 
//...

#define PRIDE_COLOR_BACKGROUND                    0x00008080 
#define PRIDE_COLOR_WINDOW                        0x00FFFFFF 
#define PRIDE_COLOR_WINDOW_BACKGROUND             0x00202020 
#define PRIDE_COLOR_ACTIVE_WINDOW_BORDER          0x0080FFFF
#define PRIDE_COLOR_INACTIVE_WINDOW_BORDER        0x0080FFFF
#define PRIDE_COLOR_ACTIVE_WINDOW_TITLEBAR        0x0080FFFF
#define PRIDE_COLOR_INACTIVE_WINDOW_TITLEBAR      0x0080FFFF
#define PRIDE_COLOR_MENUBAR                       0x00808080 
#define PRIDE_COLOR_SCROLLBAR                     0x00FFF5EE 
#define PRIDE_COLOR_STATUSBAR                     0x002EB982
#define PRIDE_COLOR_MESSAGEBOX                    0x00808080  
#define PRIDE_COLOR_SYSTEMFONT                    0x00000000  
#define PRIDE_COLOR_TERMINALFONT                  0x00FFFFFF  
//...


/*
 * ColorSchemeType:
 *     Enumerando os esquemas de cores. 
 *     Esses são esquemas default.
 *     Poderemos carregar outros e cada esquema terá 
 * seu índice.
 *     Esses serão sempre os primeiros.    
 */

typedef enum {

    ColorSchemeNull,      // 0 - Null.
    ColorSchemeHumility,  // 1 - Gray stuff.
    ColorSchemePride,     // 2 - Colorful.
    ColorSchemeUser,      // 3 - Personalizado. Criado ou carregado.
    // ...

}ColorSchemeType;


/*
 * ColorSchemeIndex:
 *     csi - color scheme index. 
 *     Enumerando os elementos gráficos
 *     O esquema de cores se aplica ao servidor 
 * de recursos gráficos. GWS.
 */
typedef enum {

    csiNull,                    //0
    csiDesktop,                 //1 área de trabalho.

    //window
    csiWindow,                  //2
    csiWindowBackground,        //3
    csiActiveWindowBorder,      //4
    csiInactiveWindowBorder,    //5

    //bar
    csiActiveWindowTitleBar,    //6
    csiInactiveWindowTitleBar,  //7
    csiMenuBar,                 //8
    csiScrollBar,               //9
    csiStatusBar,               //10

    csiMessageBox,              //11
    csiSystemFontColor,         //12 BLACK
    csiTerminalFontColor,       //13 WHITE
	//...

	//@todo: focus,
	
    //@todo: Window 'shadow' (black??)

}ColorSchemeIndex;  


/*
 * color_scheme_d:
 *     Estrutura para esquema de cores. 
 *     O esquema de cores se aplica ao servidor 
 * de recursos gráficos. GWS.
 */ 

struct gws_color_scheme_d
{
    //@todo: É preciso ainda definir esse tipo de objeto.
	//definir em gdef.h
	//object_type_t objectType;
	//object_class_t objectClass;	

    int id;

    int used;
    int magic;

	//marcador de estilo de design para 
	//o padrão de cores.
	//cada estilo pode invocar por si um padrão de cores.
	//ou o padrão de cores por si pode representar um estilo.
    int style;

    char *name; 

	//cada índice desse array representará um elemento gráfico,
	//os valores no array são cores correspondentes aos elementos gráficos 
	//representados por índices.
    unsigned long elements[32];

	//...
};

struct gws_color_scheme_d* GWSCurrentColorScheme;
struct gws_color_scheme_d* GWSHumilityColorScheme; // Simples.
struct gws_color_scheme_d* GWSPrideColorScheme;    // Colorido.


//
// == variables ==============================================
//

// #ordem
// hardware, software
extern unsigned long  ____BACKBUFFER_VA;
extern unsigned long  ____FRONTBUFFER_VA;
// Saving
extern unsigned long SavedBootBlock;
extern unsigned long SavedLFB;   // #bugbug? VA ?
extern unsigned long SavedX;
extern unsigned long SavedY;
extern unsigned long SavedBPP; 
// helper.
extern unsigned long __device_width;
extern unsigned long __device_height;
extern unsigned long __device_bpp;

// refresh rate of the whole screen.
//static unsigned long fps;

// refresh rate for all dirty objects. In one round.
//static unsigned long frames_count;
// static unsigned long frames_count_in_this_round;

// Vamos usar ou nao.
extern int use_vsync;
#define VSYNC_YES  1
#define VSYNC_NO   0


// Pointers to screens.
#define MAX_SCREENS    4
#define SCREEN_FRONTBUFFER     0
#define SCREEN_BACKBUFFER      1
//#define SCREEN_BACKBUFFER2   2
//#define SCREEN_BACKBUFFER3   3

extern unsigned long screens[MAX_SCREENS];



/*
struct vid_d
{
    unsigned long width;
    unsigned long height;
    unsigned long bpp;
    unsigned long frontbuffer;  // lfb
    unsigned long backbuffer;   // screen[0]
};
struct vid_d vidConfig;
*/


//
// == Prototypes ===================================================
//

// #todo: move this thing to another place.
void gwssrv_initialize_color_schemes (int selected_type);
int gwssrv_select_color_scheme (int type);

#endif   



