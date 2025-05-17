// window.h
// 2020 - Created by Fred Nora.

#ifndef ____WINDOW_H
#define ____WINDOW_H    1

#include "event.h"

typedef int  __wid_t;
typedef int  wid_t;

// Owner for keyboard and mouse.
extern struct gws_window_d *keyboard_owner;
extern struct gws_window_d *mouse_owner;  // captured

extern struct gws_window_d *active_window;  // active

extern struct gws_window_d *first_window;
extern struct gws_window_d *last_window;
extern struct gws_window_d *top_window;     // z-order

// ===============================================================

// Icons
// see: icon_cache in the Gramado OS kernel.
#define ICON_ID_APP  1
#define ICON_ID_FILE  2
#define ICON_ID_FOLDER  3
#define ICON_ID_TERMINAL  4
#define ICON_ID_CURSOR  5
// ...
#define ICON_ID_DEFAULT  ICON_ID_APP

// #test w->view
#define VIEW_NULL       0
#define VIEW_FULL       1000
#define VIEW_MAXIMIZED  1001
#define VIEW_MINIMIZED  1002
#define VIEW_NORMAL     1003  //Normal (restaurada)
// ...

// -------------------
// Aliases
#define WINDOW_STATE_NULL  VIEW_NULL
#define WINDOW_STATE_FULL  VIEW_FULL
//The window is maximized.
#define WINDOW_STATE_MAXIMIZED  VIEW_MAXIMIZED
//The window is minimized.
#define WINDOW_STATE_MINIMIZED  VIEW_MINIMIZED
//The window is restored.
#define WINDOW_STATE_NORMAL     VIEW_NORMAL 
// -------------------

// Button styles (int)
#define BSTYLE_3D  0
#define BSTYLE_FLAT  1
// ...

// Button states (int)
#define BS_NULL      0 
#define BS_DEFAULT   1
#define BS_RELEASED  1
#define BS_FOCUS     2
#define BS_PRESS     3
#define BS_PRESSED   3
#define BS_HOVER     4
#define BS_DISABLED  5
#define BS_PROGRESS  6
// ...

#define WINDOW_LOCKED    1
#define WINDOW_UNLOCKED  0

//
// Window Style
//

#define WS_MAXIMIZED    0x0001
#define WS_MINIMIZED    0x0002  // Iconic
#define WS_FULLSCREEN   0x0004
#define WS_STATUSBAR    0x0008  // In the bottom

#define WS_LOCKED              0x0010  // No input
// 0x0020
#define WS_CLIP_IN_CLIENTAREA  0x0040
// 0x0080

#define WS_TITLEBAR      0x0100
#define WS_TITLEBARICON  0x0200
#define WS_TRANSPARENT   0x0400
// 0x0800

#define WS_HSCROLLBAR   0x1000
#define WS_VSCROLLBAR   0x2000
#define WS_CHILD        0x4000
// 0x8000

#define WS_APP       0x10000
#define WS_DIALOG    0x20000
#define WS_TERMINAL  0x40000
#define WS_TASKBAR   0x80000

//----------

// window status
#define WINDOW_STATUS_ACTIVE       1
#define WINDOW_STATUS_INACTIVE     0


//window relationship status. (seu status em relação as outras janelas.)
//Obs: tem uma estreita ligação com o status da thread que está trabalahndo com ela 
//e com a prioridade dessa thread e do processo que a possui.
// *** RELAÇÃO IMPLICA PRIORIDADE ***
#define WINDOW_REALATIONSHIPSTATUS_FOREGROUND     1000
#define WINDOW_REALATIONSHIPSTATUS_BACKGROUND     2000
#define WINDOW_REALATIONSHIPSTATUS_OWNED          3000  //Possuida por outra janela.
#define WINDOW_REALATIONSHIPSTATUS_ZAXIS_TOP      4000
#define WINDOW_REALATIONSHIPSTATUS_ZAXIS_BOTTOM   6000
//...

extern unsigned long windows_count;

// ...

extern int show_fps_window;

/*
// #deprecated
// Structure for button object.
// #bugbug:
// A button is a kind of window and use the same structure.
struct gws_button_d
{
    //object_type_t   objectType;
    //object_class_t  objectClass;
    int used;
    int magic;
// ??
// Ordem dos botões que pertencam à mesma janela.
// A qual janela o botão pertence.
// Esse índice pode trabalhar junto com 
// a lista encadeada de 'next'.
    //int index;
    struct gws_window_d *window; 
// label
// #todo: mudar o tipo para (char *)
    unsigned char *string; 
// Estilo de design.
// 3D, flat ...
    int style;
// Button states: See BS_XXXXX.
//1. Default
//2. Focus
//3. Expanded/Toggled/Selected
//4. Disabled
//5. Hover and Active
    int state;
//Check Boxes
//Group Boxes
//Push Buttons
//Radio Buttons
    int type;
    int selected;
// Border color
// Isso é para o caso do estilo 3D.
// Ou para causar alguns efito em outros estilos.
    unsigned long border1;
    unsigned long border2;
// Deslocamento em relação ao left da janela
// Deslocamento em relação ao top da janela
    unsigned long x;    
    unsigned long y;   
    unsigned long width; 
    unsigned long height;
// #todo: Use unsigned int.
// Background color.
    unsigned long color;
//More?
//...
    struct gws_button_d *next;  
};
*/


//
// Window Class support.
//

// Enumerando classe de janela.
typedef enum {
    gws_WindowOwnerClassNull,    // 0 null
    gws_WindowOwnerClassServer,  // 1 The display server
    gws_WindowOwnerClassKernel,  // 2 kernel
    gws_WindowOwnerClassClient,  // 3 client
}gws_wc_ownerclass_t;

// '1'
// ex: The window represents a surface
// managed by the kernel.
typedef enum {
    gws_KernelWindowClassNull,
    gws_KernelWindowClassSurface
}gws_kernel_window_classes_t;

// '2'
// Classes de janelas controladas pelo display server;
// Como no caso da taskbar, ja que o wm esta embutido.
typedef enum {
    gws_ServerWindowClassNull,
    gws_ServerWindowClassServerWindow,
    //...
}gws_server_window_classes_t;

// '3'
// Classes de janelas controladas pelos aplicativos.
typedef enum {
    gws_ClientWindowClassNull,
    gws_ClientWindowClassApplicationWindow,
    gws_ClientWindowClassKernelWindow,  //janelas criadas pelo kernel ... coma a "tela azul da morte"
    gws_ClientWindowClassTerminal,      //janela de terminal usada pelos aplicativos que não criam janela e gerenciada pelo kernel	
    gws_ClientWindowClassButton,
    gws_ClientWindowClassComboBox,
    gws_ClientWindowClassEditBox,
    gws_ClientWindowClassListBox,
    gws_ClientWindowClassScrollBar,
    gws_ClientWindowClassMessageOnly,  //essa janela não é visível, serve apenas para troca de mensagens ...
    gws_ClientWindowClassMenu,
    gws_ClientWindowClassDesktopWindow,
    gws_ClientWindowClassDialogBox,
    gws_ClientWindowClassMessageBox,
    gws_ClientWindowClassTaskSwitchWindow,
    gws_ClientWindowClassIcons,
    gws_ClientWindowClassControl,   //??
    gws_ClientWindowClassDialog,
    gws_ClientWindowClassInfo
    //...
}gws_client_window_classes_t;

// #test
// Estrutura para window class
struct gws_window_class_d
{
// Que tipo de window class.
// do sistema, dos processos ...
// tipo de classe.
    gws_wc_ownerclass_t ownerClass; 

    gws_kernel_window_classes_t kernelClass;
    gws_server_window_classes_t serverClass;
    gws_client_window_classes_t clientClass;

// Endereço do procedimento de janela.
// (rip da thread primcipal do app ou no display server)
    int procedure_is_server_side;
    unsigned long procedure;

    // ...
};

// ------------------------------

// Type of input pointer device.
typedef enum {
    IP_DEVICE_NULL,
    IP_DEVICE_KEYBOARD,
    IP_DEVICE_MOUSE,
    IP_DEVICE_TOUCHSCREEN
    // ... 
} gws_ip_device_t;

// The controls for a given window.
// w->Controls.minimize_wid
// #todo:
// maybe we can use the element 
// 'int maximize_or_restore_wid;'
struct windowcontrols_d
{
// Structure initialization.
    int initialized;

    int minimize_wid;
    int maximize_wid;  // Restore/maximize ?
    int close_wid;
};

//
// Frame control
//

#define FRAME_MIN_X    (24 + 24 + 24)
#define FRAME_MIN_Y    (24 + 24 + 24)

// Esses componentes também existem na
// estrutura de janela. Mas eles só serão relevantes
// se as flags aqui indicarem que eles existem.
// #todo: Talvez todos possam vir para dentro da estrutura de frame.

#define FRAME_FLAGS_TITLEBAR   1
#define FRAME_FLAGS_MENUBAR    2
#define FRAME_FLAGS_STATUSBAR  4
#define FRAME_FLAGS_SCROLLBAR  8
#define FRAME_FLAGS_BORDER     16

// Not a pointer.
struct windowframe_d
{
// Se estamos usando ou não frame nessa janela.
    int used;
    int magic;
// width limits in pixel.
    unsigned long min_x;
    unsigned long max_x;
// height limits in pixel.
    unsigned long min_y;
    unsigned long max_y;
// type
// normal application frame
// full screen applications may has a button.
    // int type;
// + 1 - pintamos com retângulos.
// + 2 - expandimos uma imagem.
    int style;
// The elements.
// Um monte de flags pra indicar os elementos usados no frame.
    unsigned long flags;
// Icons:
// #todo: We can handle more information about the icon.
    unsigned int titlebar_icon_id;
// image
// The address of the expandable image 
// used for drawing the frame.
    int image_id;
// Main colors
    unsigned int color1;
    unsigned int color2;
    unsigned int color3;
    unsigned int color4;
// Decoration colors
    unsigned int ornament_color1;
    unsigned int ornament_color2;
    unsigned int ornament_color3;
    unsigned int ornament_color4;
};

#define TEXT_SIZE_FOR_SINGLE_LINE  128
#define TEXT_SIZE_FOR_MULTIPLE_LINE  256

// #todo:
// Mouse properties.
// Each window can have it's own mouse pointer properties.
struct mouse_pointer_properties_d 
{
    int test_value;
    unsigned int bg_color;
    // ...
};

// gws_window_d:
// Server-side window object.
struct gws_window_d 
{
// Structure validation
    int used;
    int magic;

    int id;
    //int wid;

// Offscreen canvas?
// The information about the area where this window paints.
// The compositor needs this to build the final desktop scene.
    struct screen_information_d  s_canvas;

// The input status.
// If the window is disable, it can't receive input from keyboard or mouse.
    int enabled;

    // In the window stack we have two major components:
    // + The frame (top frame and bottom frame).
    // + The Client area.

// The window frame
// Top frame has: title bar, tool bar, menu bar ...
    struct windowframe_d  frame;

// The frame's rectangle.
// #bugbug: Is it relative or absolute?
    struct gws_rect_d  rcWindow;
// The Client area.
// This is the viewport for some applications, just like browsers.
// >> Inside dimensions clipped by parent.
    struct gws_rect_d  rcClient;

// Absolute
// Relativo a tela.
// Not clipped by parent.
    unsigned long absolute_x;
    unsigned long absolute_y;
    unsigned long absolute_right;
    unsigned long absolute_bottom;

// Relative
// This is the window rectangle. (rcWindow)
// Relativo a parent.
// >> Inside dimensions clipped by parent.
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

// Margins and dimensions when this window is in fullscreen mode.
// #todo: Maybe we can use a structure for that.
// Inside dimensions not clipped by parent.
    unsigned long full_left;
    unsigned long full_top;
    unsigned long full_width;
    unsigned long full_height;
    unsigned long full_right; 
    unsigned long full_bottom;

    // #todo    
    //unsigned long border_color;
    //unsigned long border_width;

    // Cliente area in chars.
    //unsigned long client_area_width;
    //unsigned long client_area_height;

// #todo
// Explain this feature.
// Maybe it's all about dock stuff.
    int gravity;

// tipo? ... (editbox, normal, ...) 
// Isso pode ser 'int'
    unsigned long type;

// Window style
    unsigned long style;

// Controls
    struct windowcontrols_d  Controls;

// ===================================
// Name/label support.
    char *name;
// Label: If the window is a button.
    unsigned int label_color_when_selected;
    unsigned int label_color_when_not_selected;
    //unsigned int label_color_when_disabled;

// Uma janela de aplicativo
// poderá ter janelas de aplicativo dentro de sua área de cliente.
    int multiple;
// Used to invalidate the rectangle.
// If the window is dirty, so the whole window rectangle
// needs to be flushed into the framebuffer.
    int dirty;

// Se tem o foco de entrada ou não.
// Isso faz a janela ser pintada ou repintada 
// contendo o indicador de foco.

// 1=solid | 0=transparent
// Solid means that the color is opaque, 
// there is no transparence at all.
// Transparent means that it has a rop 
// associated with this window.

    int is_solid;
    unsigned long rop;

// Hierarquia. 
// parent->level + 1;
// Não é z-order?
// Criamos a janela sempre um level acima do level da sua parent.
// Is the index in a list o childs?
// The top-level windows are exactly the direct 
// subwindows of the root window.
    int level;

// #todo: use this when
// rebuilding some window list.
    // int zorder_index;

// #importante
// Para sabermos quem receberá o reply no caso de eventos.
// Quando um cliente solicitar por eventos em uma dada janela
// com foco, então essa janela deve registrar qual é o fd do
// cliente que receberá o reply com informações sobre o evento.
    int client_fd;
// pid do cliente.
    pid_t client_pid;
// Client's pid and tid.
// tid é usado para saber qual é a thread associada
// com a janela que tem o foco de entrada.
    int client_tid;

//
// Características dessa janela..
//

// Estado: (Full,Maximized,Minimized...)
    //int view; 
    int state;
// Active, inactive.
    int status;

// ======================================
// DOC
    char *window_doc;
    size_t docbuffer_size_in_bytes;
    size_t doc_size_in_bytes;
    int doc_fd;  // File descriptor for the document.

// ======================================
// TEXT
// The text support.
// Used by input devices or just to show the text
// when we dont have input support.
// #todo: Create a substructure for this.

    char *window_text;
    size_t textbuffer_size_in_bytes;
    size_t text_size_in_bytes;
    int text_fd;             // file descriptor for the text
    // color?

// Text support
// This is part of our effort to handle the text inside a
// EDITBOX window.
// Text buffers support 
// Um ponteiro para um array de ponteiros de estruturas de linhas
// Explicando: Nesse endereço teremos um array. 
// Cada ponteiro armazenado nesse array é um ponteiro para uma 
// estrutura de linha.
// Obs: @todo: Todos esses elementos podem formar uma estrutura e 
// ficaria aqui apenas um ponteiro para ela.

    void *LineArray;
    int LineArrayIndexTotal;    //Total de índices presentes nesse array.
    int LineArrayIndexMax;      //Número máximo de índices permitidos no array de linhas.
    int LineArrayTopLineIndex;  //Indice para a linha que ficará no topo da página.
    int LineArrayPointerX;      //Em qual linha o ponteiro está. 	
    int LineArrayPointerY;      //em qual coluna o ponteiro está.
    // ...

//==================================================
// DC - Device context

// #todo
    struct dc_d  *window_dc;
// Maybe we can have a device context only for the client area.
    struct dc_d  *client_dc;

// ==================================================
// STACK - The layers/parts of a window.
// This is a stack of elements to build an application window.
// Some kinds of window do not use all these elements.

// ================
// 1 - Shadow

    unsigned int shadow_color; 
    int shadow_style;
    int shadowUsed;

// ================
// 2 - Background

    unsigned int bg_color; 
    unsigned int bg_color_when_mousehover;
    //unsigned int bg_color_when_mousepressed;
    int background_style;
    int backgroundUsed;

// ================
// 3 - Titlebar

    struct gws_window_d  *titlebar;
    unsigned int titlebar_color;
    //unsigned int titlebar_color_when_active;
    //unsigned int titlebar_color_when_secondplane;
    unsigned int titlebar_ornament_color;

    //#todo:
    //The 'left' needs to change when the titlebar has an icon.
    // Text properties.
    //int titlebar_has_string; //#todo: Create this flag.
    unsigned long titlebar_text_left;
    unsigned long titlebar_text_top;
    unsigned int titlebar_text_color;

    int isMinimizeControl;
    int isMaximizeControl;
    int isCloseControl;
    // The relative position with width,
    // (width - left_offset) = left.
    unsigned long left_offset;

    unsigned long titlebar_width;
    unsigned long titlebar_height;
    int titlebarHasIcon;     // If the title bar uses or not an icon.
    int titlebar_style;
    int titlebarUsed;

// ================
// 4 - Controls

    int minimizebuttonUsed;
    int maximizebuttonUsed;
    int closebuttonUsed;

    int controls_style;
    int controlsUsed;

// ================
// 5 - Borders

    unsigned int border_color1;  // top/left
    unsigned int border_color2;  // right/bottom
    unsigned long border_size;
    int border_style;
    int borderUsed;

// ================
// 6 - Menubar

    struct gws_window_d *menubar;
    struct gws_menu_d  *barMenu;      // Menu da barra de menu.
    unsigned int menubar_color;
    unsigned long menubar_height;
    int menubar_style;
    int menubarUsed; 

// ================
// 7 - Toolbar

    struct gws_window_d *toolbar;
    unsigned int toolbar_color;
    unsigned long toolbar_height;
    int toolbar_style;
    int toolbarUsed;

// ================
// 8 - Client area.
// We're using a rectangle, declared right above
// in the top of the structure.

    unsigned int clientarea_bg_color;
    int clientarea_style;
    int clientareaUsed;

// ================
// 9 - Scrollbar
// vertical scrollbar
// The wm will call the window server to create this kind of control.

    struct gws_window_d *scrollbar;
    struct gws_window_d *scrollbar_button1;
    struct gws_window_d *scrollbar_button2;
    struct gws_window_d *scrollbar_button3;
    unsigned int scrollbar_color;
    int isScrollBarButton1;
    int isScrollBarButton2;
    int isScrollBarButton3;
    unsigned long scrollbar_height;
    // int scrollbarOrientation;  //horizontal or vertical
    int scrollbar_style;
    int scrollbarUsed;

// ================
// 10 - Statusbar

    struct gws_window_d *statusbar;
    unsigned int statusbar_color;
    unsigned long statusbar_height;
    int statusbar_style;
    int statusbarUsed;
    // ...

// =========================================================
// Context menu. It belongs to a window.
    struct gws_menu_d *contextmenu;

//==================================
// Menu item

// The text when the window is a menu item.
// Maybe we need more information about this text.
    char *menuitem_text;

// The windows is a menuitem and it's selected.
    int selected;

// ======================================================
// Flags to tell us if the window is a button, a bar or anything else.
// It helps when we are painting.
// #bugbug
// What if we set as TRUE more than one flat at the same time?

    int isTitleBar;
    int isIcon;
    int isControl;
    int isMenu;
    int isMenuItem;
    int isButton;
    int isEditBox;
    int isCheckBox;
    int isStatusBar;
    int isTaskBar;
    // ...

//
// == Buffers =========================================
//

   // #test
   // This is a test. Not implemented yet.

// Buffer.
// DedicatedBuffer
// DedicatedBuffer --> LFB.
// Endereço de memória onde a janela foi pintada.
// Obs: Toda janela deve ter seu próprio buffer de pintura para poder 
// passar a janela direto de seu próprio buffer para o LFB, sem passar 
// pelo Backbuffer.

    void *dedicated_buf;  //Qual buffer dedicado a janela usa.

    void *back_buf;       //Qual backbuffer a janela usa.
    void *front_buf;      //Qual frontbuffer a janela usa. (LFB).	

// z-buffer for this window.
// #test: sometimes the whole screen 
// do not have a depth buffer, but we can have
// a depth buffer for some windows.
// big one: 800x600x24bpp = 1875 KB.
// We do not have this memory yet.
    unsigned int *depth_buf;

// ==================================================
// z-order
// Ordem na pilha de janelas do eixo z.
// A janela mais ao topo é a janela foreground.
    int zIndex;

// ==================================================
// Desktop support.
// A que desktop a janela pertence??
// Temos um estrutura de desktop dentro do kernel,
// faz parte do subsistema de segurança e gerenciamento de memoria.
    int desktop_id;

// ==================================================
// Seu status de relacionamento com outras janelas.
    unsigned long relationship_status;

// ==================================================
// Actions
// #todo: Explain the actions.

    int draw;
    int redraw;
    int show_when_creating;  // Se precisa ou não mostrar a janela.
    // ...

//
// Text Cursor support.
//

// fica para uma versão estendida da estrutura.
// Estrutura de cursor para a janela.
    
    //struct cursor_d	*cursor;

    //struct button_d *current_button;  //Botão atual.      
    //struct button_d *buttonList;      //Lista encadeada de botões em uma janela.

// Mouse cursor support ???
// Abaixo ficam os elementos referenciados com menor frequência.

// #deprecated
// ?? rever isso 
// Status do puxador da janela.
// Se está aberta ou não.
// HANDLE_STATUS_OPEN ou HANDLE_STATUS_CLOSE
    //int handle_status;


//
// == Input pointer =========================================
//

// #todo
// We can create a substructure to handle the input info.

    // Valido apenas para essa janela.

// Type of input pointer device.
    gws_ip_device_t ip_device;

// Para input do tipo teclado

    // #todo:
    // Use chars, or use only asterisk for password, etc ...
    //int input_glyph_style;

// The state of the input ponter.
// Used to blink the cursor.
    int ip_on;

    unsigned long ip_x;
    unsigned long ip_y;
    unsigned int ip_color;
    unsigned long width_in_chars;
    unsigned long height_in_chars;

    //unsigned long ip_type; //?? algum estilo especifico de cursor?
    //unsigned long ip_style;
    // ...

// para input do tipo teclado
    unsigned long ip_pixel_x;
    unsigned long ip_pixel_y;

// A posiçao do mouse relativa a essa janela.
    unsigned long x_mouse_relative;
    unsigned long y_mouse_relative;

// The pointer is inside this window.
    int is_mouse_hover;

// Each window has it's own mouse pointer style.
// #todo:
// Create a structure to handle the mouse properties
// for the given window.
    struct mouse_pointer_properties_d  mpp;

//
// == Events =========================================
//

// Single event
    struct gws_event_d  single_event;
// Event list.
    int ev_head;
    int ev_tail;
    unsigned long ev_wid[32];
    unsigned long ev_msg[32];
    unsigned long ev_long1[32];
    unsigned long ev_long2[32];
    // ...
// #todo
// Event queue.
     //struct gws_event_d *event_queue;

// Um alerta de que exite uma mensagem para essa janela.
    int msgAlert;  //#todo: int ev_alert;

//==================================================	
// #todo:
// Maybe every windle is gonna have a server side
// window procedure and a client side window procedure.
    unsigned long procedure;

// =========================================================
// Window Class support.
// Who own this window?
// Where is the window procedure?
    struct gws_window_class_d  window_class;

// =========================================================
// Navigation windows:

// We have an associated window when we are minimized.
// Maybe only the overlapped window can have this iconic window.
    struct gws_window_d *_iconic;
// Telling to the world that this is an icon window,
// and we belongs to an overlapped window.
    int is_iconic;

// The owner
    struct gws_window_d  *parent;
// We need to redraw all the child windows.
    struct gws_window_d  *child_list;
// Brother or sister with at least 'one' parent in common.
    struct gws_window_d *subling_list;
// Navigation
    struct gws_window_d  *prev;
    struct gws_window_d  *next;
};

//
// Windows
//

extern struct gws_window_d  *__root_window; 
extern struct gws_window_d  *active_window;
// Taskbar created by the user.
extern struct gws_window_d  *taskbar_window; 
// z-order ?
// But we can use multiple layers.
// ex-wayland: background, bottom, top, overlay.
extern struct gws_window_d *first_window;
extern struct gws_window_d *last_window;
extern struct gws_window_d *keyboard_owner;
extern struct gws_window_d *mouse_owner;

// Window list.
// This is gonna be used to register the windows.
// These indexes will be returned to the caller.
#define WINDOW_COUNT_MAX  1024
extern unsigned long windowList[WINDOW_COUNT_MAX];

// z-order support.
#define ZORDER_MAX 1024
#define ZORDER_TOP (ZORDER_MAX-1)
#define ZORDER_BOTTOM 0
// ...
#define TOP_WINDOW  ZORDER_TOP
#define BOTTOM_WINDOW 0
// ...
unsigned long zList[ZORDER_MAX];

//
// ================================================================
//

//
// == prototypes ===========================
//

//
// WM suppport
//

void __set_foreground_tid(int tid);

void __set_default_background_color(unsigned int color);
unsigned int __get_default_background_color(void);

void __set_custom_background_color(unsigned int color);
unsigned int __get_custom_background_color(void);

int __has_custom_background_color(void);
int __has_wallpaper(void);

void wmInitializeStructure(void);

// Window status
void set_status_by_id( int wid, int status );

// Window background

void set_bg_color_by_id( int wid, unsigned int color );
void set_clientrect_bg_color_by_id( int wid, unsigned int color );

void wm_change_bg_color(unsigned int color, int tile, int fullscreen);

//
// Mouse hover
//

void set_mouseover(struct gws_window_d *window);
struct gws_window_d *get_mousehover(void);

// Transparence
void gws_enable_transparence(void);
void gws_disable_transparence(void);

void 
wm_draw_char_into_the_window(
    struct gws_window_d *window, 
    int ch,
    unsigned int color );

void 
wm_draw_char_into_the_window2(
    struct gws_window_d *window, 
    int ch,
    unsigned int color );

void wm_draw_text_buffer(struct gws_window_d *window);

// Post message to the window. (broadcast)
int 
window_post_message_broadcast( 
    int wid, 
    int event_type, 
    unsigned long long1,
    unsigned long long2 );

// Post message to the window.
int 
window_post_message( 
    int wid, 
    int event_type, 
    unsigned long long1,
    unsigned long long2 );

// Post message to the thread.
int
wmPostMessage(
    struct gws_window_d *window,
    int msg,
    unsigned long long1,
    unsigned long long2 );

struct gws_client_d *wintoclient(int wid); //#todo: not teste yet
void show_client_list(int tag); //#todo: notworking
void show_client( struct gws_client_d *c, int tag );

void set_first_window( struct gws_window_d *window);
struct gws_window_d *get_first_window(void);
void set_last_window( struct gws_window_d *window);
struct gws_window_d *get_last_window(void);
void activate_first_window(void);
void activate_last_window(void);

struct gws_window_d *get_parent(struct gws_window_d *w);

    
//refaz zorder.
void reset_zorder(void);

//
// Root window
//

struct gws_window_d *wmCreateRootWindow(unsigned int bg_color);

//
// Controls
//

void do_create_controls(struct gws_window_d *w_titlebar);

// Create titlebar and controls.
struct gws_window_d *do_create_titlebar(
    struct gws_window_d *parent,
    unsigned long tb_height,
    unsigned int color,
    unsigned int ornament_color,
    int has_icon,
    int icon_id,
    int has_string );

// #important: 
// O frame de uma janela deve fazer parte do window manager
// e não das primitivas do window server.
// Estamos falando da parte do ws que opera como wm,
// ou oferece recursos que serão usados pelo wm.
// In: style = estilo do frame.
int 
wmCreateWindowFrame ( 
    struct gws_window_d *parent,
    struct gws_window_d *window,
    unsigned long border_size,
    unsigned int border_color1,
    unsigned int border_color2,
    unsigned int border_color3,
    unsigned int ornament_color1,
    unsigned int ornament_color2,
    int frame_style );

//
// Create window. (Worker)
//

// Low level
// Called by CreateWindow().
void *doCreateWindow ( 
    unsigned long type, 
    unsigned long style,
    unsigned long status, 
    unsigned long state,  // view: min, max ... 
    char *title, 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    struct gws_window_d *pWindow, 
    int desktop_id, 
    unsigned int frame_color, 
    unsigned int client_color,
    unsigned long rop_flags );

//
// Create window.
//

// Essa será a função que atenderá a interrupção
// esse é o serviço de criação da janela.
// talvez ampliaremos o número de argumentos
// Middle level.
void *CreateWindow ( 
    unsigned long type,
    unsigned long style, 
    unsigned long status, 
    unsigned long state,  // view: min, max ... 
    char *title, 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    struct gws_window_d *pWindow, 
    int desktop_id, 
    unsigned int frame_color, 
    unsigned int client_color ); 

int RegisterWindow(struct gws_window_d *window);

int destroy_window_by_wid(int wid);
void DestroyAllWindows(void);
void MinimizeAllWindows(void);
void MaximizeAllWindows(void);
void RestoreAllWindows(void);

struct gws_window_d *get_window_from_wid(int wid);

struct gws_window_d *get_active_window (void);
void set_active_window (struct gws_window_d *window);
void set_active_by_id(int wid);

void unset_active_window(void);

void enable_window(struct gws_window_d *window);
void disable_window(struct gws_window_d *window);

void change_window_state(struct gws_window_d *window, int state);
void maximize_window(struct gws_window_d *window);
void minimize_window(struct gws_window_d *window);

int dock_active_window(int position);
int dock_window( struct gws_window_d *window, int position );

//
// Focus
//

void set_focus(struct gws_window_d *window);
struct gws_window_d *get_focus(void);
void __switch_active_window(int active_first);
void set_focus_by_id(int wid);
struct gws_window_d *get_window_with_focus(void);
void set_window_with_focus(struct gws_window_d * window);

//
// Top window
//

struct gws_window_d *get_top_window (void);
void set_top_window (struct gws_window_d *window);

int get_zorder ( struct gws_window_d *window );

int 
gws_resize_window ( 
    struct gws_window_d *window, 
    unsigned long cx, 
    unsigned long cy );

int 
gwssrv_change_window_position ( 
    struct gws_window_d *window, 
    unsigned long x, 
    unsigned long y );

//void gwsWindowLock(struct gws_window_d *window);
//void gwsWindowUnlock(struct gws_window_d *window);

int gwsDefineInitialRootWindow(struct gws_window_d *window);


// Hit test
int 
is_within ( 
    struct gws_window_d *window, 
    unsigned long x, 
    unsigned long y );

// Hit test
int 
is_within2 ( 
    struct gws_window_d *window, 
    unsigned long x, 
    unsigned long y );

// Initialize the window support.
int window_initialize(void);

#endif    

//
// End
//

