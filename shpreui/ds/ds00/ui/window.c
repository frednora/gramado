// window.c
// Create and destroy a window.
// Some other routines for windows.
// 2019 - Created by Fred Nora.

// #importante
// O frame de uma janela deve ser parte do Window Manager.
// #bugbug
// #todo
// Para lidarmos com essas estruturas do kernel
// devemos usar a chamada sci2. int 0x82. ??
// #todo
// Quando estivermos construindo as molduras das janelas,
// as rotinas devem retornar suas alturas, para assim
// o posicionamento do 'top' da área de cliente 
// possa ser atualizado. (rect)

#include "../gwsint.h"

#define OVERLAPPED_MIN_WIDTH    80
#define OVERLAPPED_MIN_HEIGHT   80
#define EDITBOX_MIN_WIDTH    8
#define EDITBOX_MIN_HEIGHT   8
#define BUTTON_MIN_WIDTH    8
#define BUTTON_MIN_HEIGHT   8

static int config_use_transparency=FALSE;

unsigned long windows_count=0;

int show_fps_window = FALSE;

// Habilitando/desabilitando globalmente 
// alguns componentes da janela.
// #bugbug: Não confie nessas inicializações.
int gUseShadow = TRUE;
int gUseFrame = TRUE;
//int gUseShadow = TRUE;
// ...

// Windows - (struct)
extern struct gws_window_d  *__root_window; 
extern struct gws_window_d *active_window;
// If the display server has a taskbar.
// maybe we don't need that.
//extern struct gws_window_d  *taskbar_window; 
//extern struct gws_window_d  *taskbar_startmenu_button_window; 
//char startmenu_string[32];

// z-order ?
// But we can use multiple layers.
// ex-wayland: background, bottom, top, overlay.
extern struct gws_window_d *first_window;
extern struct gws_window_d *last_window;

static const char *default_window_name = "Untitled window";

//
// == private functions: prototypes =============
//

static struct gws_window_d *__create_window_object(void);

//
// =====================================
//

// Create window structure.
static struct gws_window_d *__create_window_object(void)
{
    struct gws_window_d *window;

    window = (void *) malloc( sizeof(struct gws_window_d) );
    if ((void *) window == NULL){
        return NULL;
    }
    memset( window, 0, sizeof(struct gws_window_d) );
    window->used = TRUE;
    window->magic = 1234;
    // The window can receive input from kbd and mouse.
    window->enabled = TRUE;
    window->style = 0;
    // Validate. This way the compositor can't redraw it.
    window->dirty = FALSE;
    
    return (struct gws_window_d *) window;
}

// Post message to the window. (broadcast).
// Return the number of sent messages.
int 
window_post_message_broadcast( 
    int wid, 
    int event_type, 
    unsigned long long1,
    unsigned long long2 )
{
    int return_value=-1;
    register int i=0;
    int Counter=0;
    struct gws_window_d *wReceiver;
    int target_wid = -1;

// Invalid message code.
    if (event_type < 0)
        goto fail;

// Probe for valid Overlapped windows and
// send a close message.
    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        wReceiver = (void*) windowList[i];
        if ((void*) wReceiver != NULL)
        {
            if (wReceiver->magic == 1234)
            {
                if (wReceiver->type == WT_OVERLAPPED)
                {
                    target_wid = (int) wReceiver->id;
                    window_post_message( 
                        target_wid, event_type, long1, long2 );
                    Counter++;
                }
            }
        }
    };

    return (int) Counter;
fail:
    return (int) -1;
}

// Post message to the window.
int 
window_post_message( 
    int wid, 
    int event_type, 
    unsigned long long1,
    unsigned long long2 )
{
    struct gws_window_d *w;

// Parameters
    if (wid < 0)
        goto fail;
    if (wid >= WINDOW_COUNT_MAX)
        goto fail;
    if (event_type < 0){
        goto fail;
    }

// Window
    w = (void*) windowList[wid];
    if ((void*) w == NULL)
        goto fail;
    if (w->magic != 1234){
        goto fail;
    }

//
// Event
//

// Get offset.
    register int Tail = (int) w->ev_tail;
// Post
    w->ev_wid[Tail] = 
        (unsigned long) (wid & 0xFFFFFFFF);
    w->ev_msg[Tail] = 
        (unsigned long) (event_type & 0xFFFFFFFF);
    w->ev_long1[Tail] = (unsigned long) long1;
    w->ev_long2[Tail] = (unsigned long) long2;
// Circula
    w->ev_tail++;
    if (w->ev_tail >= 32){
        w->ev_tail=0;
    }

   return 0;

fail:
    return (int) -1;
}

void gws_enable_transparence(void)
{
    config_use_transparency=TRUE;
}

void gws_disable_transparence(void)
{
    config_use_transparency=FALSE;
}

// #todo
// Essas rotina serão chamada pelo request assincrono sem resposta.

/*
void useShadow( int value );
void useShadow( int value )
{
    // Ativando.
    if ( value == TRUE ){
        gUseShadow = TRUE;
        return;
    }

    gUseShadow = FALSE;
}
*/

/*
void useFrame( int value );
void useFrame( int value )
{
    // Ativando.
    if ( value == TRUE ){
        gUseFrame = TRUE;
        return;
    }

    // No frames.
    // It means that the loadable window manager 
    // will create some kind of frames for all the windows, or not.
    
    gUseFrame = FALSE;
}
*/

/*
 * doCreateWindow:  
 *     Chamada por CreateWindow.
 *     Cria uma janela com base em uma struct. 
 * Retorna o endereço da estrutura 
 * da janela criada, para que possa ser registrada na lista windowList[].
 * Obs: A contagem é feita quando registra a janela.
 * @todo: É necessário definir claramente os conceitos de window e window frame...
 *        A construção dos elementos gráficos precisam de organização e hierarquia.
 * Obs: Essa rotian pode criar tanto uma janela, quanto um frame ou um botão.
 * @todo: Corrigir as dimensões da janela: 
 * *Importante: OS ARGUMENTOS DE DIMENSÕES SÃO PARA O RETÂNGULO QUE FORMA UMA ÁREA
 * QUE INCLUI DA ÁREA DO CLIENTE + BARRA DE FERRAMENTAS + BARRA DE MENU.
 * a barra de títulos faz parte da moldura.
 * O AS DIMENSÕES DO FRAME PODEM SER VARIADOS, DEPENDENDO DO ESTILO DA JANELA.
 * Cria a janela dependendo do tipo:
 * =================================
 * WT_NULL          0 
 * WT_SIMPLE        1
 * WT_EDITBOX       2  // igual simples, mais uma bordinha preta.
 * WT_OVERLAPPED    3  // sobreposta(completa)(barra de titulo + borda +client area)
 * WT_POPUP         4  // um tipo especial de sobreposta,  //usada em dialog ou 
 *                        message box. (com ou sem barra de titulo ou borda)
 * WT_CHECKBOX      5  // Caixa de seleção. Caixa de verificação. Quadradinho.
 * WT_SCROLLBAR     6  // Cria uma scroll bar. Para ser usada como janela filha.
 * CONTINUA ...
 */
//1  - Tipo de janela (popup,normal,...) 
//2  - style
//3  - Estado da janela. (poderia ter vários bits ??)
//4  - (min, max ...)
//5  - Título.
//6  - Deslocamento em relação às margens do Desktop.
//7  - Deslocamento em relação às margens do Desktop.
//8  - Largura da janela.
//9  - Altura da janela.
//10  - Endereço da estrutura da janela mãe.
//11 - desktop ID. (# Para levarmos em conta as características de cada desktop).
//12 - Client Area Color.
//13 - color (bg) (para janela simples)

void *doCreateWindow ( 
    unsigned long type, 
    unsigned long style,
    unsigned long status,  // #test Status do botao e da janela.
    unsigned long state,   //view: min, max ... 
    char *title, 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    struct gws_window_d *pWindow, 
    int desktop_id, 
    unsigned int frame_color, 
    unsigned int client_color,
    unsigned long rop_flags ) 
{

// #todo
// Essa função deve chamar helpers que pintem sem criar objetos
// gráficos que alocam memória. Dessa forma eles
// poderão serem reusados nas funções de 'redraw'.
// #todo: 
// O argumento style está faltando.
// Cada tipo de tanela poderá ter vários estilos.
// Obs: 
// Podemos ir usando apenas um estilo padrão por enquanto.
// #todo
// We need different colors for the text inside the buttons.

// The window object.
    register struct gws_window_d *window;
    struct gws_window_d *Parent;

    unsigned int FrameColor;
    unsigned int ClientAreaColor;
    if (type == WT_OVERLAPPED){
        FrameColor = (unsigned int) get_color(csiWindow);
        ClientAreaColor = (unsigned int) get_color(csiWindow);
    }else{
        FrameColor = (unsigned int) frame_color;
        ClientAreaColor = (unsigned int) client_color;
    };

//
// Internal flags.
//

// #todo:
// Receberemos isso via parametro de função.
// Default is FALSE.
// We need to know the parent's bg color.
    int Transparent = FALSE;
    int Maximized=FALSE;
    int Minimized=FALSE;
    int Fullscreen=FALSE;
// Bars
// A title bar é criadas pela função
// que cria o frame.
// Title bar buttons. [v] [^] [X] 
    int MinimizeButton = FALSE;
    int MaximizeButton = FALSE;
    int CloseButton    = FALSE;
// Items.
    int Shadow        = FALSE;
    int Background    = FALSE;
    int TitleBar      = FALSE;
    int Border        = FALSE;  // Usado no edit box, na overlapped.
    int ClientArea    = FALSE;
    int ButtonDown    = FALSE;  // ??
    int ButtonUp      = FALSE;  // ??
    int ButtonSysMenu = FALSE;  // system menu na barra de títulos.
    // ...

// Desktop support.
    int ParentWindowDesktopId;    //Id do desktop da parent window.
    int WindowDesktopId;          //Id do desktop da janela a ser criada.

// Controle de janela
    struct gws_window_d *windowButton1;  // minimize
    struct gws_window_d *windowButton2;  // maximize
    struct gws_window_d *windowButton3;  // close

// Botões na barra de rolagem.
    struct gws_window_d *windowButton4;
    struct gws_window_d *windowButton5;
    struct gws_window_d *windowButton6;

// Structs.
	//struct desktop_d *Desktop;  //suspenso.

// The client rectangle.
// Isso é uma estrutura local para gerenciarmos o retangulo
// da área de cliente.
// Depois temos que copiar os valores para window->rcClient
    struct gws_rect_d  clientRect;

// Border
// #
// Improvisando uma largura de borda.
// Talvez devamos receber isso via parâmetros.
// Ou ser baseado no estilo.

    //unsigned int border_size = METRICS_BORDER_SIZE;
    //unsigned int border_color = COLOR_BORDER;
    unsigned int __tmp_color=0;

// Device context
    unsigned long deviceLeft   = 0;
    unsigned long deviceTop    = 0;
    unsigned long deviceWidth  = (__device_width  & 0xFFFF);
    unsigned long deviceHeight = (__device_height & 0xFFFF);

// Position and dimension.
// Passado via argumento.
// left, top, width, height.
    unsigned long WindowX = (unsigned long) (x & 0xFFFF);
    unsigned long WindowY = (unsigned long) (y & 0xFFFF);
    unsigned long WindowWidth  = (unsigned long) (width  & 0xFFFF);
    unsigned long WindowHeight = (unsigned long) (height & 0xFFFF);

// #todo: right and bottom.

// Full ?
// Position and dimension for fullscreen mode.
// Initial configuration.
// It will change.
// #bugbug
// left and top needs to be '0'?

/*
    unsigned long fullWindowX      = (unsigned long) (WindowX + border_size);
    unsigned long fullWindowY      = (unsigned long) (WindowY + border_size);
    unsigned long fullWindowWidth  = (unsigned long) WindowWidth;
    unsigned long fullWindowHeight = (unsigned long) WindowHeight;
    // #todo: right and bottom.
*/

// Fullscreen support
    unsigned long fullWindowX      = (unsigned long) deviceLeft;
    unsigned long fullWindowY      = (unsigned long) deviceTop;
    unsigned long fullWindowWidth  = (unsigned long) deviceWidth;
    unsigned long fullWindowHeight = (unsigned long) deviceHeight;

// Style
// Button suport
    int buttonFocus    = FALSE;
    int buttonSelected = FALSE;
    unsigned int buttonBorderColor1=0;
    unsigned int buttonBorderColor2=0;
    unsigned int buttonBorderColor2_light=0;
    unsigned int buttonBorder_outercolor=0;  //Essa cor muda de acordo com o foco 

    //debug_print ("doCreateWindow:\n");

// Style (rop)
// #todo: 
// What is this? Explain it better.
    int is_solid = TRUE;
    if (rop_flags != 0){
        is_solid = FALSE;
    }

// No rop flags for now. It depends on the window style.
    unsigned long __rop_flags=0;

//---------------------------------------------------------
// Position

    /*
    if (type == WT_OVERLAPPED)
    {
        if (WindowX > (deviceWidth/4)){
            WindowX = (deviceWidth/8);
        }
        if (WindowY > (deviceHeight/4)){
            WindowY = (deviceHeight/8);
        }
    }
    */

//---------------------------------------------------------

//
// style
//

// #bugbug
// maximized, minimized and fullscreen are not style features.
// move these things to another flag.
// see:
// https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
// see: border, captions, scrobar ...

// Maximized
// #todo:
// The window occupy the whole desktop working area.
    if (style & WS_MAXIMIZED){
        Maximized=TRUE;
    }
// Minimized
// (Iconic)
    if (style & WS_MINIMIZED){
        Minimized=TRUE;
    }
// Fullscreen
// Paint only the client area.
    if (style & WS_FULLSCREEN){
        Fullscreen=TRUE;
    }

    if (style & WS_TRANSPARENT){
        Transparent=TRUE;
        // Get the given flags.
        __rop_flags = rop_flags;
        //#test #hack
        if ( __rop_flags == 0 )
            __rop_flags = 20;  //gray
    }

    //int IsTaskbar = FALSE;
    //if (style & WS_TASKBAR)
        //IsTaskbar = TRUE;

//---------------------------------------------------------

// Salvar para depois restaurar os valores originais no fim da rotina.
	//unsigned long saveLeft;
	//unsigned long saveTop;

// Desktop:
// #todo: Configurar desktop antes de tudo. 
// #todo: Quando criamos uma janela temos de definir que ela
// pertence ao desktop atual se não for enviado por argumento 
// o desktop que desejamos que a janela pertença.
// O argumento onde:
// Indica onde devemos pintar a janela. Serve para indicar as janelas 
// principais. Ex: Se o valor do argumento for 0, indica que devemos 
// pintar na tela screen(background) etc...
// full screen mode ??
// Se a janela a ser criada estiver no modo full screen, ela não deve ter
// um frame, então as dimensões da janela serão as dimensões do retângulo
// que forma a janela. Talvez chamado de Client Area.

// Parent window.
// Se a parent window enviada por argumento for inválida, 
// então usaremos a janela gui->screen. ?? 
// Talvez o certo fosse retornar com erro.
// ?? Qual deve ser a janela mãe ? Limites ?
// #todo: devemos checar used e magic da janela mãe.
// #bugbug: 
// E quando formos criar a gui->screen, quem será a janela mãe?

/*
	if ( (void *) pWindow == NULL ){
		Parent = (void *) gui->screen;
	} else {
		Parent = (void *) pWindow;
	};
 */

// Devemos checar se a janela está no mesmo desktop 
// que a ajnela mãe.
// No caso aqui, criarmos uma janela no mesmo desktop que a janela mãe.
// Devemos setar uma flag que indique que essa 
// é uma janela filha, caso seja uma. Essa flag 
// deve ser passada via argumento @todo.
// @todo: Checar se é uma janela filha, 
// se for uma janela filha e não tiver uma janela mãe associada a ela, 
// não permita e encerre a função.

	//if(FlagChild == 1){
		//if(pWindow = NULL) 
        //    return NULL;
	//}

// #todo: A atualização da contagem de janela deve ficar aqui,
// mas me parece que está em outro lugar, ou não tem. ainda.
// @todo: Se essa não for uma janela filha, então temos que resetar 
// as informações sobre a janela mãe. porque não procedem.	
// ms. e se essa é uma janela filha de uma janela mãe que pertence à
// outra thread e não está no desktop ??

// Importante: 
// Checando se o esquema de cores está funcionando.

/*
	if ( (void *) CurrentColorScheme == NULL ){
		panic ("CreateWindow: CurrentColorScheme");
	}else{
		if ( CurrentColorScheme->used != 1 || CurrentColorScheme->magic != 1234 ){
		    panic ("CreateWindow: CurrentColorScheme validation");
		}
		// Nothing
	}
 */

// Create the window object.
    window = (struct gws_window_d *) __create_window_object();
    if ((void*) window == NULL){
        return NULL;
    }

// Window class. #test
    window->window_class.ownerClass = gws_WindowOwnerClassNull;
    window->window_class.kernelClass = gws_KernelWindowClassNull;
    window->window_class.serverClass = gws_ServerWindowClassNull;
    window->window_class.clientClass = gws_ClientWindowClassNull;
    window->window_class.procedure_is_server_side = 0;
    window->window_class.procedure = 0;

// Gravity
    window->gravity = DefaultGravity;

// Type
    window->type = (unsigned long) type;
// Style
    window->style = (unsigned long) style;
// Status (active or inactive)
// Status do botao e da janela. (int)
    window->status = (int) (status & 0xFFFFFFFF);
    // The 'window status' is used as 'button state'
    int ButtonState = (int) (status & 0xFFFFFFFF);

// View
// The window state: minimized, maximized.
    //window->view = (int) view;
    window->state = (int) view;

// Colors
    window->bg_color = 
        (unsigned int) FrameColor;
    window->clientarea_bg_color = 
        (unsigned int) ClientAreaColor;
// Default color for 'when mouse hover'.
    window->bg_color_when_mousehover = 
        (unsigned int) get_color(csiWhenMouseHover);

// buffers
    window->dedicated_buf = NULL;
    window->back_buf = NULL;
    window->front_buf = NULL;
    window->depth_buf = NULL;
    //window->DedicatedBuffer = 
    //    (void*) windowCreateDedicatedBuffer(window);
    //window->BackBuffer = (void *) g_backbuffer_va;
    //window->FrontBuffer = (void *) g_frontbuffer_pa;
// Device contexts
// #todo:
// We can create our own device contexts.
    window->window_dc = NULL;
    window->client_dc = NULL;
    window->is_solid = (int) is_solid;
    window->rop = (unsigned long) rop_flags;
    //window->focus  = FALSE;
    // We already validated it when we create the object.
    //window->dirty  = FALSE;  // Validate
    //window->locked = FALSE;

// Initializing border stuff
// #todo: use get_color(x)
    window->border_color1 = COLOR_WHITE;
    window->border_color2 = COLOR_WHITE;
    window->border_size = 2;
    window->border_style = 0;
    window->borderUsed = FALSE;
    // ## na verdade isse será trabalhado logo abaixo.
    if (window->type == WT_OVERLAPPED)
    {
        window->borderUsed = TRUE;
        window->border_style = 1;  // Minimum
    }
    unsigned long __BorderSize = window->border_size;

// Title bar height.
// #todo: use metrics.
    unsigned long __TBHeight = METRICS_TITLEBAR_DEFAULT_HEIGHT;

// Event queue
    register int e=0;
    static int Max=32;
    for (e=0; e<Max; e++)
    {
        window->ev_wid[e]=0;
        window->ev_msg[e]=0;
        window->ev_long1[e]=0;
        window->ev_long2[e]=0;
    };
    window->ev_head=0;
    window->ev_tail=0;

// Lock or unlock the window.
    //window->locked = FALSE;
    //if (style & WS_LOCKED){
    //    window->locked = TRUE;
    //}
// Can't lock,
// We need permitions to do our work.
    //window->locked = FALSE;
    window->enabled = TRUE;

// ===================================
// Input support:
// The buffer and the input pointers.

// Input pointer device.
    window->ip_device = IP_DEVICE_NULL;
    window->ip_on = FALSE;  // desligado
// For keyboard
// Given in chars.
    window->ip_x = 0;       // in chars
    window->ip_y = 0;       // in chars
    window->ip_color = (unsigned int) get_color(csiSystemFontColor);
    //window->ip_type = 0;    // #bugbug #todo
    //window->ip_style = 0;
// For mouse
// In pixel, for mouse pointer ip device.
    window->ip_pixel_x = 0;
    window->ip_pixel_y = 0;

// A posiçao do mouse relativa a essa janela.
    window->x_mouse_relative=0;
    window->y_mouse_relative=0;

// The pointer is inside this window.
    window->is_mouse_hover = FALSE;

// #test
// #todo
// The properties for the mouse pointer.
// Valid only for this window.
// See: widnow.h
    window->mpp.test_value = 1000;

// ===================================

// Id
// We will get an id when we register the window.
// #bugbug: So, we can't use the id in this routine yet.
    window->id = -1;

// ===================================
// Title: Just a pointer.
    if ((void*) title != NULL){
        if (*title != 0){
            window->name = (char *) title;
        }
        if (*title == 0){
            window->name = (char *) default_window_name;
        }
    } else if ((void*) title == NULL){
        window->name = (char *) default_window_name;
    };

// ===================================
// Parent

    if ((void*) pWindow == NULL){
        window->parent = NULL;
    }
    if ((void*) pWindow != NULL){
        window->parent = (struct gws_window_d *) pWindow;
    }
    Parent = (struct gws_window_d *) window->parent;

// Navigation
// #todo: Put these at the end of the routine.
    window->prev = (void *) Parent;
    window->next = NULL;

// Default: We still do not have an iconic window associated with us.
    window->_iconic = NULL;
// Default: We're not the icon for another window.
    window->is_iconic = FALSE;

// ===================================
// Sublings
    //window->subling_list = NULL;

// ===================================
// Childs
    window->child_list = NULL;

// ===================================
// #todo: 
// é importante definir o procedimento de janela desde já.
// senão dá problemas quando chamá-lo e ele naõ estiver pronto.
// Procedure support.
// #todo: Devemos receber esses parâmetros e configurar aqui.
// #bugbug: Talvez isso será configurado na estrutura
// de window class. Se é que termos uma.

    //window->procedure = (unsigned long) &system_procedure;
    //window->wProcedure = NULL;  //Estrutura.

//
// == Status ============================
//

// Qual é o status da janela, se ela é a janela ativa ou não.
// ?? Devemos definir quais são os status possíveis da janela.

//
// Window status - (Not a button)
// 

    if (window->type == WT_OVERLAPPED)
    {
        // Active 
        if (window->status == WINDOW_STATUS_ACTIVE)
        { 
            set_active_window(window); 
            //window->active = WINDOW_STATUS_ACTIVE;
            //window->status = (unsigned long) WINDOW_STATUS_ACTIVE;
            window->relationship_status = 
                (unsigned long) WINDOW_REALATIONSHIPSTATUS_FOREGROUND; 
            //#todo
            //window->z = 0;  //z_order_get_free_slot()
           //...
        }
        // Inactive
        if (window->status == WINDOW_STATUS_INACTIVE)
        { 
            //window->active = WINDOW_STATUS_INACTIVE;
            window->relationship_status = 
                (unsigned long) WINDOW_REALATIONSHIPSTATUS_BACKGROUND;
            //todo
            //window->z = 0; //z_order_get_free_slot()
            //...
        }
    }

//
// == Margins and dimensions ======================
//

// #todo:
// Se for uma janela filha o posicionamento deve ser somado às margens 
// da área de cliente da janela que será a janela mãe.
// #bugbug #todo 
// Esses valores de dimensões recebidos via argumento na verdade 
// devem ser os valores para a janela, sem contar o frame, que 
// inclui as bordas e a barra de títulos.

// Dimensions
// Passado via argumento.
    window->width  = (unsigned long) (WindowWidth  & 0xFFFF);
    window->height = (unsigned long) (WindowHeight & 0xFFFF);

// #todo
// We need a variable for char width.
    window->width_in_chars  = 
        (unsigned long) (window->width / 8);   //>>3
    window->height_in_chars = 
        (unsigned long) (window->height / 8);  //>>3

// =================================

//
// Window area.
//

// #todo: Inside dimensions clipped by parent.
// Initial configuration for the window rectangle.
// Relative values. (l,t,w,h)
    window->rcWindow.left   = (unsigned long) 0;
    window->rcWindow.top    = (unsigned long) 0;
    window->rcWindow.width  = (unsigned long) WindowWidth;
    window->rcWindow.height = (unsigned long) WindowHeight;

// =================================

//
// Client area.
//

// #todo:
// We need a variable for border size in the structure.
// #todo:
// We need a variable for title bar height.

// Left margin and top margin.
// The top margin changes if we have a bar.

    //clientRect.left = (unsigned long) __BorderSize;
    //clientRect.top  = (unsigned long) __BorderSize;

    // #test
    clientRect.left = (unsigned long) __BorderSize;
    clientRect.top  = (unsigned long) 0;

    if (window->type == WT_OVERLAPPED){
        clientRect.top  = (unsigned long) (__TBHeight + __BorderSize);
    }

// Width and height.
// width
// menos bordas laterais
// height
// menos bordas superior e inferior
    // menos a barra de tarefas.

    clientRect.width  = 
        (unsigned long) ( 
            window->width -
            __BorderSize -
            __BorderSize );

    clientRect.height = 
        (unsigned long) ( 
            window->height -
            __BorderSize -
            __TBHeight -
            __BorderSize); 

// If we have scrollbars.
// #todo: Diminuimos as dimensões se o style
// indicar que temos scrollbars.
    //if (window->style & WS_VSCROLLBAR)
    //    clientRect.width -= 24;
    //if (window->style & WS_HSCROLLBAR)
    //    clientRect.height -=24;
    //if (window->style & WS_STATUSBAR)
    //    clientRect.height -=24;


// Saving.
// Client rectangle:
// The Client area.
// This is the viewport for some applications, just like browsers.
// >> Inside dimensions clipped by parent.
    window->rcClient.left   = clientRect.left;
    window->rcClient.top    = clientRect.top;
    window->rcClient.width  = clientRect.width;
    window->rcClient.height = clientRect.height;

// =================================
//++
// Margens.
// Deslocando em relaçao a janela mãe.

// We don't have a parent wiindow.
// If this is the first of all windows.

// Relative
// >> Inside dimensions clipped by parent.
    window->left = WindowX;
    window->top  = WindowY;

// If we have a parent window.
// parent + arguments
// Sempre é relativo à janela mãe.
// Se a janela mãe é overlapped,
// então também é relativo à janela de cliente.
// Pois é o lugar padrão para criar janelas cliente.
// Isso só não será válido, se uma flag especial 
// permitir criar uma janela fora da área de cliente.
// >> Not clipped by parent.
    if ((void*) window->parent == NULL)
    {
        window->absolute_x = WindowX;
        window->absolute_y = WindowY;
    }

// Calcula o absoluto
// >> Not clipped by parent.
    if ((void*) window->parent != NULL)
    {
        if (window->parent->type != WT_OVERLAPPED)
        {
            window->absolute_x = 
                (unsigned long) (window->parent->absolute_x + WindowX);
            window->absolute_y = 
                (unsigned long) (window->parent->absolute_y + WindowY);
        }
        
        if (window->parent->type == WT_OVERLAPPED)
        {
            // Dentro da área de cliente.
            window->absolute_x = 
                (unsigned long) ( window->parent->absolute_x + 
                window->parent->rcClient.left + 
                WindowX );
            window->absolute_y  = 
                (unsigned long) ( window->parent->absolute_y +
                window->parent->rcClient.top + 
                WindowY );

            // Fora da área de cliente.
            if ( window->type == WT_TITLEBAR)
            {
                window->absolute_x = 
                    (unsigned long) ( window->parent->absolute_x + WindowX );
                window->absolute_y  = 
                    (unsigned long) ( window->parent->absolute_y + WindowY );
            }
                    
            // Fora da área de cliente.
                    
        }
    }

// Right and bottom.
// >> Not clipped by parent.
    window->absolute_right = 
        (unsigned long) (window->absolute_x + window->width);
    window->absolute_bottom = 
        (unsigned long) (window->absolute_y + window->height);
//--

// Maximized. OK
// Fit to the desktop working area.
    if (Maximized == TRUE)
    {
        if (WindowManager.initialized == TRUE)
        {
            window->absolute_x = WindowManager.wa.left;
            window->absolute_y = WindowManager.wa.top;
            window->width  = WindowManager.wa.width;
            window->height = WindowManager.wa.height;

            // Right and bottom.
            window->absolute_right = 
                (unsigned long) (window->absolute_x + window->width);
            window->absolute_bottom = 
                (unsigned long) (window->absolute_y + window->height);
        }
    }

// Fullscreen OK
// Inside dimensions not clipped by parent.
    if (Fullscreen == TRUE)
    {
        window->absolute_x = fullWindowX;
        window->absolute_y = fullWindowY;
        // #todo: Relative? window->left, window->top?
        window->width = fullWindowWidth;
        window->height = fullWindowHeight;

        // Right and bottom.
        window->absolute_right = 
            (unsigned long) (window->absolute_x + window->width);
        window->absolute_bottom = 
            (unsigned long) (window->absolute_y + window->height); 

        window->full_left   = window->absolute_x;
        window->full_top    = window->absolute_y;
        window->full_width  = window->width;
        window->full_height = window->height;

        // Fullscreen Right and bottom.
        window->full_right  = window->absolute_right;
        window->full_bottom = window->absolute_bottom;

        if (WindowManager.initialized == TRUE)
        {
            WindowManager.fullscreen_window = 
                (struct gws_window_d *) window;
            WindowManager.is_fullscreen = TRUE;
        }
    }

// Colors: 
// Background and client area background.
// #todo
// If this window is overlapped, so, we need to respect the theme.
    // Already did above.
    //window->bg_color = (unsigned int) FrameColor;
    //window->clientarea_bg_color = (unsigned int) ClientAreaColor;

// #todo: As outras características do cursor.
// Características.

// Estrutura para cursor.
// todo
    //window->cursor = NULL;

// #todo: 
// Uma opção é inicializarmos a estrutura de ponteiro depois ...
// pois tem janela que não tem ponteiro. 
// JÁ QUE NO MOMENTO ESTAMOS ENFRENTANDO ALGUNS TRAVAMENTOS.

    //window->cursor = (void*) malloc( sizeof(struct cursor_d) );

// #todo: 
// Criar uma função: Inicializarcursor(struct cursor_d *cursor).
    //if(window->cursor != NULL)
    //{
    //    window->cursor->objectType = ObjectTypeCursor;
    //    window->cursor->objectClass = ObjectClassGuiObjects;
    //	window->cursor->x = 0;
    //	window->cursor->y = 0;
    //	window->cursor->imageBuffer = NULL;
    //	window->cursor->imagePathName = NULL;
    //window->cursor->cursorFile = ??; //@todo: Difícil definir o tipo.
    //	window->cursor->cursorType = cursorTypeDefault;
    //}

// Barras.
// As flags que representam a presença de cada uma das barras
// serão acionadas mais tarde, na hora da pintuda, 
// de acordo com o tipo de janela à ser pintada.

// Desktop support
    //window->desktop = (void*) Desktop; //configurado anteriormente.
    //window->desktop_id = Desktop->id;  //@todo: verificar elemento.

// What kind of component is it?
    window->isMenu = FALSE;
    window->isMenu = FALSE;
    window->isButton = FALSE;
    window->isEditBox = FALSE;
    window->isTaskBar = FALSE;
    // ...

// Context menu: right click
// ou clicando no icone.
    window->contextmenu = NULL;

// menu na menubar
    window->barMenu = NULL;

// Selected menu item.
// Caso a janela seja um ítem de menu.
    window->selected = FALSE; 
// Texto, caso a janela seja um ítem de menu.
    // window->text = NULL; 

// Actions
    window->draw = FALSE;  // #todo: Cuidado com isso.
    window->redraw = FALSE;
    window->show_when_creating = TRUE;   // Inicialmente presumimos que precisamos mostrar essa janela.

    // Continua ...

    //window->desktop = NULL; //@todo: Definir à qual desktop a janela perence.
    //window->process = NULL; //@todo: Definir à qual processo a janela perence.

//==========

// Exemplos de tipos de janelas, segundo MS.
// Overlapped Windows
// Pop-up Windows
// Child Windows
// Layered Windows
// Message-Only Windows

// Preparando os parametros da pintura de acordo com o tipo.
// De acordo com o tipo de janela, preparamos a configuração
// e a própria rotina create window está pintando.
// Porém nesse momento o 'case' pode chamar rotinas de pintura em 
// draw.c e retornar. 
// CreateWindow é para criar a moldura principal ...
// para so outros tipos de janelas podemos usar draw.c
// pois quando chamarmos draw.c a estrutura de janela ja deve estar 
// inicializada.
// Rotinas grandes como pintar um barra de rolagem podem ficar em draw.c
// #importante
// Deveria ter uma variável global indicando o tipo de 
// design atual, se é 3D ou flat.
// Configurando os elementos de acordo com o tipo.
// @todo: Salvar as flags para os elementos presentes
// na estrutura da janela.
// Flags
// Initializing the flag for all the elements.
// not used by default.

    window->shadowUsed     = FALSE;  // 1
    window->backgroundUsed = FALSE;  // 2
    window->titlebarUsed   = FALSE;  // 3
    window->controlsUsed   = FALSE;  // 4
    window->borderUsed     = FALSE;  // 5
    window->menubarUsed    = FALSE;  // 6
    window->toolbarUsed    = FALSE;  // 7
    window->clientareaUsed = FALSE;  // 8
    window->scrollbarUsed  = FALSE;  // 9
    window->statusbarUsed  = FALSE;  // 10

// Element style
// Initialize style indicators.

    window->shadow_style     = 0;
    window->background_style = 0;
    window->titlebar_style   = 0;
    window->controls_style   = 0;
    window->border_style     = 0;
    window->menubar_style    = 0;
    window->toolbar_style    = 0;
    window->clientarea_style = 0;
    window->scrollbar_style  = 0;
    window->statusbar_style  = 0;

// Elements
// Selecting the elements given the type.
// Each type has it's own elements.

    switch (type){

    // Simple window. (Sem barra de títulos).
    case WT_SIMPLE:
    case WT_TITLEBAR:
        if (window->style & WS_TASKBAR)
        {
            window->isTaskBar = TRUE;
            
            // #important
            // Set the taskbar created by the user.
            taskbar2_window = window;
            // No more access to the embedded taskbar.
            //TaskBar.initialized = FALSE;
            // No more access to the QuickLaunch resources.
            //QuickLaunch.initialized = FALSE;
        }
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = FALSE;
        Background = TRUE;
        window->backgroundUsed = TRUE;
        window->background_style = 0;
        break;

    // Edit box. (Simples + borda preta).
    // Editbox não tem sombra, tem bordas.
    //case WT_EDITBOX:
    case WT_EDITBOX_SINGLE_LINE:
    case WT_EDITBOX_MULTIPLE_LINES:
        window->ip_device = IP_DEVICE_KEYBOARD;
        window->frame.used = TRUE;
        Background = TRUE;
        Border = TRUE;
        window->backgroundUsed = TRUE;
        window->background_style = 0;
        break;

    // Overlapped. (completa, para aplicativos).
    // Sombra, bg, título + borda, cliente area ...
    // #obs: Teremos recursividade para desenhar outras partes.
    // + Sempre tem barra de títulos.
    // + Sempre tem borda.
    case WT_OVERLAPPED:
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = TRUE;
        // Internal flag.
        Shadow         = TRUE;
        Background     = TRUE;
        ClientArea     = TRUE;
        MinimizeButton = TRUE;  //Depends on the style.
        MaximizeButton = TRUE;  //Depends on the style.
        CloseButton    = TRUE;  //Depends on the style.
        // Always.
        window->shadowUsed     = TRUE;
        window->backgroundUsed = TRUE;
        window->titlebarUsed   = TRUE;
        window->controlsUsed   = TRUE;
        window->borderUsed     = TRUE;
        window->clientareaUsed = TRUE;
        window->background_style = 0;
        break;

    // Popup. (um tipo de overlapped mais simples).
    case WT_POPUP:
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = FALSE;
        Shadow     = TRUE;
        Background = TRUE;
        window->shadowUsed     = TRUE;
        window->backgroundUsed = TRUE;
        window->background_style = 0;
        break;

    // Check box. (Simples + borda preta).
    // Caixa de seleção. Caixa de verificação. Quadradinho.
    // #todo: checkbox has borders.
    case WT_CHECKBOX:
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = FALSE;
        Background = TRUE;
        Border     = TRUE;
        window->backgroundUsed = TRUE;
        // #todo: structure element for 'border'
        window->background_style = 0;
        break;

    //case WT_SCROLLBAR:
        // Nothing for now.
        //break;

    // Only the bg for now.
    // #todo: Button has borders?
    // #todo: BUtton has icon?
    case WT_BUTTON:
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = TRUE;
        Background = TRUE;
        window->backgroundUsed = TRUE;
        window->background_style = 0;
        break;

    // Status bar.
    // #todo: checkbox has borders sometimes.
    case WT_STATUSBAR:
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = FALSE;
        Background = TRUE;
        window->backgroundUsed = TRUE;
        window->background_style = 0;
        break;

    // Ícone na área de trabalho.
    // #todo: Icons has borders sometimes.
    // #todo: Icon window has an icon in it.
    case WT_ICON:
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = FALSE;
        Background = TRUE;
        window->backgroundUsed = TRUE;
        window->background_style = 0;
        break;

    // barra de rolagem
    // botões de radio 
    // ...

    // #todo
    // #bugbug
    // We need to work on this case.

    default:
        debug_print("doCreateWindow: [DEBUG] default\n");
             printf("doCreateWindow: [DEBUG] default\n");
        while (1){
        };
        //return NULL;
        break;
    };

    // #debug
    // while(1){}

//
// == Draw ========
//

// Hora de pintar. 
// Os elementos serão incluídos se foram 
// selecionados anteriormente.
// Obs: 
// Se for uma janela, pintaremos apenas a janela.
// Se for um frame, pintaremos todos os elementos
// presentes nesse frame de acordo com as flags.
// Obs:
// A janela precisa ser pintada em seu buffer dedicado.
// Nesse momento o buffer dedicado da janela já está na estutura
// da janela. Rotinas de pintura que tenham acesso à estrutura da
// janela devem utilizar o buffer dedicado que for indicado na estrutura.
// Para que seja possível pintar uma janela em seu buffer dedicado,
// devemos passar um ponteiro para a estrutura da janela em todas
// as rotinas de pintura chamadas daqui pra baixo.
// #todo: 
// Passar estrutura de janela via argumento, para a rotina
// de pintura ter acesso ao buffer dedicado.

    //if(DedicatedBuffer == 1){};


// Se o view for igual NULL talvez signifique não pintar.
// The window state: minimized, maximized.
    if (window->state == WINDOW_STATE_NULL)
    {
        //#bugbug: fail.
        //window->show_when_creating = FALSE;
        //window->redraw = 0;
        //return (void*) window;
    }

// Minimized ? (Hide ?)
// Se tiver minimizada, não precisa mostrar a janela, porém
// é necessário pintar a janela no buffer dedicado, se essa técnica 
// estiver disponível.
// Talvez antes de retornarmos nesse caso seja necessário configurar 
// mais elementos da estrutura.
// #bugbug
// se estamos contruindo a janela, então ela não foi registrada 
// não podemos checar as coisas na estrutura ainda,
// mas a estrutura ja existe a algumas coisas foram inicializadas.
// #importante
// Pois retornaremos no caso de janelas minimizadas.
// Provavelmente isso foi usado quando criamos janelas 
// de referência na inicialização da GUI.(root)

/*
    Minimized = 0;
    Minimized = (int) is_window_minimized (window);
    if (Minimized == 1)
    {
        //window->draw = 1; //Devemos pintála no buffer dedicado.
        window->show_when_creating = FALSE;
        window->redraw = 0;
        //...
        //@todo: Não retornar. 
        //como teste estamos retornando.
        goto done;
        //return (void *) window;
    }
 */

// #todo: 
// Maximized ?
// Para maximizar devemos considerar as dimensões 
// da área de cliente da janela mãe.
// Se a jenela estiver maximizada, então deve ter o tamanho da área de 
// cliente da janela main.
// Essa área de cliente poderá ser a área de trabalho, caso a
// janela mãe seja a janela principal.
// Obs: se estiver maximizada, devemos usar as dimensão e coordenadas 
// da janela gui->main.
// #bugbug
// Temos um problema com essa limitação.
// Não conseguimos pintar janelas simples além do height da janela gui->main
// para janelas overlapped funciona.

/*
    Maximized = 0;
    Maximized = (int) is_window_maximized (window);

    if ( Maximized == 1 )
    {
        //#debug
        printf("file: createw.c: #debug\n");
        printf ("original: l=%d t=%d w=%d h=%d \n", 
            window->left, gui->main->top, 
            window->width, window->height );

        //Margens da janela gui->main
        window->left = gui->main->left;    
        window->top  = gui->main->top;

        //Dimensões da janela gui->main.
        window->width  = gui->main->width;
        window->height = gui->main->height; 
        
        window->absolute_right = (unsigned long) window->left + window->width;
        window->absolute_bottom = (unsigned long) window->top  + window->height;       

        // ??
        // Deslocamentos em relação às margens.
        // Os deslocamentos servem para inserir elementos na janela, 
        // como barras, botões e textos.
        window->x = 0;
        window->y = 0;

        //#debug
        printf ("corrigido: l=%d t=%d w=%d h=%d \n", 
            window->left, gui->main->top, 
            window->width, window->height );

        //#debug
        refresh_screen ();
        while (1){}
    }
 */

// =================================
// ## Shadow ## (Shadow for the frame)
// A sombra pertence à janela e ao frame.
// A sombra é maior que a própria janela.
// Se estivermos em full screen não tem sombra?
// ========
// 1

    if (Shadow == TRUE)
    {
        window->shadowUsed = TRUE;

        //CurrentColorScheme->elements[??]

        //@todo: 
        // ?? Se tiver barra de rolagem a largura da 
        // sombra deve ser maior. ?? Não ...
        //if()

        // @todo: Adicionar a largura das bordas verticais 
        // e barra de rolagem se tiver.
        // @todo: Adicionar as larguras das 
        // bordas horizontais e da barra de títulos.
        // Cinza escuro.  CurrentColorScheme->elements[??] 
        // @TODO: criar elemento sombra no esquema. 

        if ((unsigned long) type == WT_OVERLAPPED)
        {
            if (window == keyboard_owner){
                __tmp_color = xCOLOR_GRAY1;
            }else if (window != keyboard_owner){
                __tmp_color = xCOLOR_GRAY2;
            }

            painterFillWindowRectangle( 
                (window->absolute_x +1), (window->absolute_y +1), 
                (window->width +1 +1), (window->height +1 +1), 
                __tmp_color, __rop_flags );
            //#todo
            window->shadow_color = (unsigned int) __tmp_color;
        }

        // E os outros tipos, não tem sombra ??
        // Os outros tipos devem ter escolha para sombra ou não ??
        // Flat design pode usar sombra para definir se o botão 
        // foi pressionado ou não.
        // ...
    }

// ===============================================
// ## Background ## (Background for the frame)
// Background para todo o espaço ocupado pela janela e pelo seu frame.
// O posicionamento do background depende do tipo de janela.
// Um controlador ou um editbox deve ter um posicionamento relativo
// à sua janela mãe. Já uma overlapped pode ser relativo a janela 
// gui->main ou relativo à janela mãe.
// ========
// 2

    if (Background == TRUE)
    {
        window->backgroundUsed = TRUE;

        // Select background color.
        switch (type){
            case WT_SIMPLE:
            case WT_TITLEBAR:
            case WT_OVERLAPPED:
            case WT_POPUP:
            case WT_EDITBOX:
            case WT_EDITBOX_MULTIPLE_LINES:
            case WT_CHECKBOX:
            case WT_ICON:
            case WT_BUTTON:
                window->bg_color = (unsigned int) FrameColor;
                break;
            default:
                // #todo
                window->bg_color = (unsigned int) COLOR_PINK;
                break;
        };

        // Paint the background.
        // This routine is calling the kernel to paint the rectangle.
        // Absolute values.
        painterFillWindowRectangle( 
            window->absolute_x, 
            window->absolute_y, 
            window->width, 
            window->height, 
            window->bg_color, 
            __rop_flags );
    
        // #todo
        // Could we return now if its type is WT_SIMPLE?
    }


// #todo
// Nothing to do here.
    //if (ClientArea == TRUE){
    //}


//
// == Button ====================
//

// Termina de desenhar o botão, mas não é frame
// é só o botão...
// caso o botão tenha algum frame, será alguma borda extra.
// border color:

    unsigned int label_color = COLOR_BLACK;  // default

// #todo
// Use color scheme in this routine.
// #todo: Call get_color() to get the standard color 
// for all this button components.
    if ((unsigned long) type == WT_BUTTON)
    {
        // #ps: ButtonState = window status.
        switch (ButtonState)
        {
            case BS_FOCUS:
                //window->focus = TRUE;
                buttonFocus = TRUE;
                buttonBorderColor1       = COLOR_BLUE;
                buttonBorderColor2       = COLOR_BLUE;
                buttonBorderColor2_light = xCOLOR_GRAY5;
                buttonBorder_outercolor  = COLOR_BLUE;
                break;

            case BS_PRESS:
                //printf("BS_PRESS\n"); exit(0);
                buttonSelected = TRUE;
                buttonBorderColor1       = xCOLOR_GRAY1; 
                buttonBorderColor2       = COLOR_WHITE;
                buttonBorderColor2_light = xCOLOR_GRAY5; 
                buttonBorder_outercolor  = COLOR_BLACK;
                break;

            case BS_HOVER:
                buttonBorderColor1       = COLOR_WHITE; 
                buttonBorderColor2       = xCOLOR_GRAY1;
                buttonBorderColor2_light = xCOLOR_GRAY5; 
                buttonBorder_outercolor  = COLOR_BLACK;
                break;
                    
            case BS_DISABLED:
                buttonFocus = FALSE;
                buttonBorderColor1 = COLOR_GRAY;
                buttonBorderColor2 = COLOR_GRAY;
                buttonBorder_outercolor  = COLOR_GRAY;
                break;

            case BS_PROGRESS:
                buttonBorderColor1 = COLOR_GRAY;
                buttonBorderColor2 = COLOR_GRAY;
                buttonBorder_outercolor  = COLOR_GRAY;
                break;

            case BS_DEFAULT:
            default: 
                buttonFocus = FALSE;
                buttonSelected = FALSE;
                buttonBorderColor1       = COLOR_WHITE;   // left/top
                buttonBorderColor2       = xCOLOR_GRAY1;  // right/bottom
                buttonBorderColor2_light = xCOLOR_GRAY5;
                buttonBorder_outercolor  = COLOR_BLACK;
                break;
        };

        //
        // Label support
        //
        
        // #todo
        // It depends on the string style.
        // If the buttton's window has an icon,
        // the string goes after the icon are.
        // If it doesn't have an icon, so the buttons goes
        // in the center.
        
        size_t tmp_size = 
            (size_t) strlen( (const char *) window->name );
        // #bugbug
        // The max size also need to respect 
        // the size of the button's window.
        if (tmp_size > 64)
        {
            tmp_size=64;
        }

        // It goes in the center.
        unsigned long l_offset = 
            ( ( (unsigned long) window->width - ( (unsigned long) tmp_size * (unsigned long) FontInitialization.width) ) >> 1 );
        unsigned long t_offset = 
            ( ( (unsigned long) window->height - FontInitialization.height ) >> 1 );

        // #debug: 
        // Se o botão não tem uma parent window.
        if ((void*) Parent == NULL){
            //server_debug_print ("doCreateWindow: [WT_BUTTON] Parent NULL\n"); 
        }

        // Paint the button only if it has a parent window.
        if ((void*) Parent != NULL)
        {
            // #todo
            // Esses valores precisam estar na estrutura para
            // podermos chamar a rotina redraw para repintar 
            // as bordas do botao.
            // See: wm.c
            // color1: left/top
            // color2: right/bottom
            // #check
            // This routine is calling the kernel to paint the rectangle.
            // #todo
            // We can register these colors inside the windows structure.
            __draw_button_borders(
                (struct gws_window_d *) window,
                (unsigned int) buttonBorderColor1,
                (unsigned int) buttonBorderColor2,
                (unsigned int) buttonBorderColor2_light,
                (unsigned int) buttonBorder_outercolor );

            //#todo: Use the color scheme.
            window->label_color_when_selected = xCOLOR_GRAY1;
            window->label_color_when_not_selected = xCOLOR_BLACK;

            // Setup the label's properties.
            if (buttonSelected == TRUE){
                label_color = window->label_color_when_selected;  
            }
            if (buttonSelected == FALSE){
                label_color = window->label_color_when_not_selected;  
            }
            // Draw the label's string.
            // The label is the window's name.
            grDrawString ( 
                (window->absolute_x + l_offset), 
                (window->absolute_y + t_offset), 
                (unsigned int) label_color, 
                window->name );

            // #test
            // Testing the possibility of painting an icon inside the button.
            //bmp_decode_system_icon( 
                //(int) 2,            //ID
                //(unsigned long) 4,  //left 
                //(unsigned long) 4,  //top
                //FALSE );
        }

      //todo
      // configurar a estrutura de botão 
      // e apontar ela como elemento da estrutura de janela.
      //window->button->?
    
    } //button

// Invalidate the window.
// #todo: Only if it is not minimized.
    window->dirty = TRUE;
// Return the pointer.
    return (void *) window;
fail:
    debug_print ("doCreateWindow: Fail\n");
    return NULL;
}

/*
 * CreateWindow:
 *     It creates a window
 */
// Essa será a função que atenderá a chamada a
// esse é o serviço de criação da janela.
// talvez ampliaremos o número de argumentos
// #todo: change name to 'const char *'
// Called by serviceCreateWindow() in main.c.
// #test
// Uma janela que é cliente, será criada
// com deslocamento relativo à area de cliente.
// Para criar janelas filhas com deslocamento relativo
// a janela do aplicativo, tem que ativar uma flag.

void *CreateWindow ( 
    unsigned long type, 
    unsigned long style,
    unsigned long status,  // #test Status do botao, e da janela. 
    unsigned long state,  // view: min, max ... 
    char *title,
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    struct gws_window_d *pWindow, 
    int desktop_id, 
    unsigned int frame_color, 
    unsigned int client_color ) 
{
   register struct gws_window_d *__w;
   unsigned long __rop_flags=0;
// This function is able to create some few 
// kinds of windows for now:
// overlapped, editbox, button and simple.
    int ValidType=FALSE;
    size_t text_size = 0;

    //server_debug_print ("CreateWindow:\n");

    unsigned int FrameColor;
    unsigned int ClientAreaColor;
    if (type == WT_OVERLAPPED){
        FrameColor = (unsigned int) get_color(csiWindowBackground);
        ClientAreaColor = (unsigned int) get_color(csiWindow);
    } else if (type == WT_BUTTON) {
        FrameColor = (unsigned int) get_color(csiButton);
        ClientAreaColor = (unsigned int) get_color(csiWindow);
    } else {
        FrameColor = (unsigned int) frame_color;
        ClientAreaColor = (unsigned int) client_color;
    };



    /*
    // #debug
    int mystate = (status & 0xFFFFFFFF);
    if (mystate == BS_PRESS){
        printf("BS_PRESS\n"); exit(0);
    }
    */

    /*
    // #debug
    int myview = (view & 0xFFFFFFFF);
    if (myview == WINDOW_STATE_MAXIMIZED){
        printf("WINDOW_STATE_MAXIMIZED\n"); exit(0);
    }
    */


// =================
// name
// Duplicate
    char *_name;
    _name = (void*) malloc(256);
    if ((void*) _name == NULL){
        return NULL;
    }
    memset(_name,0,256);
    if ((void*) title != NULL){
        strcpy(_name,title);
    }
    if ((void*) title == NULL){
        strcpy(_name,"No title");
    }
// =================

// See:
// config.h, main.c
    if (config_use_transparency == TRUE)
    {
        __rop_flags = 1;       // or
        //__rop_flags = 2;     // and
        //__rop_flags = 3;     // xor
        //__rop_flags = 4;     // nand
        //__rop_flags = 10;    // less red
        //__rop_flags = 12;    // blue
        //__rop_flags = 20;    // gray 
        //__rop_flags = 21;    // no red
    }

// #todo: 
// Colocar mascara nos valores passados via parâmetro.
    // #todo: ValidType=FALSE;
    switch (type){
    case WT_OVERLAPPED:
    case WT_EDITBOX: 
    case WT_EDITBOX_MULTIPLE_LINES:
    case WT_BUTTON:
    case WT_SIMPLE:
    case WT_ICON:
        ValidType=TRUE;
        break;
    };

    // #todo: if (ValidType != TRUE){
    if (ValidType == FALSE){
        //server_debug_print ("CreateWindow: Invalid type\n");
        goto fail;
    }

// 1. Começamos criando uma janela simples
// 2. depois criamos o frame. que decide se vai ter barra de títulos ou nao.
// No caso dos tipos com moldura então criaremos em duas etapas.
// no futuro todas serão criadas em duas etapas e 
// CreateWindow será mais imples.

// #todo
// Check parent window validation.
// APPLICATION window uses the screen margins for relative positions.
    //if ( (void*) pWindow == NULL ){}

// #todo
// check window name validation.
    //if ( (void*) windowname == NULL ){}
    //if ( *windowname == 0 ){}

    //if (style & WS_MAXIMIZED){
    //    printf("MAX 1\n"); 
    //}

// ============================
// Types with frame.

//====
// Overlapped
    if (type == WT_OVERLAPPED)
    {
        //server_debug_print ("CreateWindow: WT_OVERLAPPED\n");
        
        // #test
        // #todo: precisamos de um request que selecione
        // o modo de operação do window manager.
        // pode ser assincrono.
        
        if (WindowManager.initialized == TRUE)
        {
            // 1 = Tiling mode.
            // Fit into the working area.
            if (WindowManager.mode == 1)
            {
                // #bugbug: 
                // Estamos confiando nos valores. 
                /*
                 //#suspended #debug
                x      = WindowManager.wa.left;
                y      = WindowManager.wa.top;
                width  = WindowManager.wa.width;
                height = WindowManager.wa.height;
                */
            }
        }

        if (width < OVERLAPPED_MIN_WIDTH)  { width=OVERLAPPED_MIN_WIDTH; }
        if (height < OVERLAPPED_MIN_HEIGHT){ height=OVERLAPPED_MIN_HEIGHT; }

        // #debug:  Fake name: The parent id.
        // #bugbug: 0 for everyone.
        // itoa( (int) pWindow->id, _name );
        
        __w = 
            (void *) doCreateWindow ( 
                         WT_SIMPLE, 
                         style, 
                         status,  //#test 
                         state,  // view: min, max ...
                         (char *) _name,
                         x, y, width, height, 
                         (struct gws_window_d *) pWindow, 
                         desktop_id, 
                         FrameColor, 
                         ClientAreaColor, 
                         (unsigned long) __rop_flags ); 

        if ((void *) __w == NULL){
            //server_debug_print ("CreateWindow: doCreateWindow fail\n");
            goto fail;
        }

        //if (__w->style & WS_MAXIMIZED){
        //    printf("MAX2\n"); 
        //}
        //printf ("overlapped: breakpoint\n");
        //while(1){}

        // Pintamos simples, mas a tipagem será overlapped.
        __w->type = WT_OVERLAPPED;
        //__w->locked = FALSE;
        __w->enabled = TRUE;

        wm_add_window_to_top(__w);
        set_active_window(__w);
        goto draw_frame;
    }

// #todo
// It does not exist by itself. It needs a parent window.

//====
// edit box
// Podemos usar o esquema padrão de cores ...
    if ( type == WT_EDITBOX || type == WT_EDITBOX_MULTIPLE_LINES )
    {
        //server_debug_print ("CreateWindow: WT_EDITBOX WT_EDITBOX_MULTIPLE_LINES \n");

        //if ( (void*) pWindow == NULL ){ return NULL; }

        if (width < EDITBOX_MIN_WIDTH)  { width=EDITBOX_MIN_WIDTH; }
        if (height < EDITBOX_MIN_HEIGHT){ height=EDITBOX_MIN_HEIGHT; }

        __w = 
            (void *) doCreateWindow ( 
                         WT_SIMPLE, 
                         style, 
                         status, 
                         state,  // view: min, max ... 
                         (char *) _name, 
                         x, y, width, height, 
                         (struct gws_window_d *) pWindow, 
                         desktop_id, 
                         FrameColor, ClientAreaColor, 
                         (unsigned long) __rop_flags ); 

        if ((void *) __w == NULL){
            //server_debug_print ("CreateWindow: doCreateWindow fail\n");
            goto fail;
        }

        //--------------------
        // Let's setup the buffer for the text.
        if (type == WT_EDITBOX)
            text_size = TEXT_SIZE_FOR_SINGLE_LINE; //128;
        if (type == WT_EDITBOX_MULTIPLE_LINES)
            text_size = TEXT_SIZE_FOR_MULTIPLE_LINE; //256;
        __w->textbuffer_size_in_bytes = 0;
        __w->text_size_in_bytes = 0;
        __w->window_text = (void*) malloc(text_size);
        if ((void*) __w->window_text != NULL)
        {
            memset(__w->window_text, 0, text_size);  // Clean
            __w->textbuffer_size_in_bytes = (size_t) text_size;
            __w->text_size_in_bytes = 0;
        }
        __w->text_fd = 0;  // No file for now.
        //--------------------

        // Pintamos simples, mas o tipo sera editbox.
        __w->type = type;  // Editbox.
        //__w->locked = FALSE;
        __w->enabled = TRUE;
        goto draw_frame;
    }

// #todo
// It does not exist by itself. 
// It needs a parent window.

// =======
// button
// Podemos usar o esquema padrão de cores ...
    if (type == WT_BUTTON)
    {
        //server_debug_print ("CreateWindow: WT_BUTTON \n");
      
        //if ( (void*) pWindow == NULL ){ return NULL; }

        if (width < BUTTON_MIN_WIDTH)  { width=BUTTON_MIN_WIDTH; }
        if (height < BUTTON_MIN_HEIGHT){ height=BUTTON_MIN_HEIGHT; }

        __w = 
            (void *) doCreateWindow ( 
                        WT_BUTTON,  // type 
                        style,      // style
                        status,     // status (Button state)
                        state,      // view: min, max ...
                        (char *) _name, 
                        x, y, width, height, 
                        (struct gws_window_d *) pWindow, 
                        desktop_id, 
                        FrameColor, ClientAreaColor, 
                        (unsigned long) __rop_flags );

         if ((void *) __w == NULL){
            //server_debug_print ("CreateWindow: doCreateWindow fail\n");
            goto fail;
         }

        // Pintamos simples, mas a tipagem será overlapped.
        __w->type = WT_BUTTON;
        //__w->locked = FALSE;
        __w->enabled = TRUE;
        goto draw_frame;
    }

// ============================

//====
// Simple
    if (type == WT_SIMPLE)
    {
        //server_debug_print ("CreateWindow: WT_SIMPLE \n");

        __w = 
            (void *) doCreateWindow ( 
                         WT_SIMPLE, 
                         style, 
                         status, 
                         state,  // view: min, max ... 
                         (char *) _name,
                         x, y, width, height, 
                         (struct gws_window_d *) pWindow, 
                         desktop_id, 
                         FrameColor, ClientAreaColor, 
                         (unsigned long) __rop_flags );  

        if ((void *) __w == NULL){
            //server_debug_print ("CreateWindow: doCreateWindow fail\n");
            goto fail;
        }

        __w->type = WT_SIMPLE;
        //__w->locked = FALSE;
        __w->enabled = TRUE;
        goto draw_frame;
    }

// ---------------------
// Icon 

    if (type == WT_ICON)
    {
        //server_debug_print ("CreateWindow: WT_ICON\n");

        __w = 
            (void *) doCreateWindow ( 
                         WT_SIMPLE, 
                         style, 
                         status, 
                         state,  // view: min, max ... 
                         (char *) _name,
                         x, y, width, height, 
                         (struct gws_window_d *) pWindow, 
                         desktop_id, 
                         FrameColor, ClientAreaColor, 
                         (unsigned long) __rop_flags );  

        if ((void *) __w == NULL){
            //server_debug_print("CreateWindow: doCreateWindow fail\n");
            goto fail;
        }

        __w->type = WT_ICON;
        //__w->locked = FALSE;
        __w->enabled = TRUE;
        goto draw_frame;
    }


// ---------------------

//type_fail:
    //server_debug_print ("CreateWindow: [FAIL] type\n");
    goto fail;

//
// == Draw frame ===============================
//

draw_frame:
// (Borders for the frame)
// We already have the shadow and 
// the background for the frame.
// These were created by doCreateWindow.
// #todo:
// Lembrando que frame é coisa do wm.
// Porém tem algumas coisas que o display server faz,
// como as bordas de um editbox.

    if ((void*) __w == NULL){
        //server_debug_print ("CreateWindow.draw_frame: __w\n");
        goto fail;
    }
    if (__w->magic != 1234){
        //server_debug_print ("CreateWindow.draw_frame: __w->magic\n");
        goto fail;
    }

// #importante:
// DESENHA O FRAME DOS TIPOS QUE PRECISAM DE FRAME.
// OVERLAPED, EDITBOX, CHECKBOX ...

// draw frame.
// #todo:
// Nessa hora essa rotina podera criar a barra de títulos.
// o wm poderá chamar a rotina de criar frame.
// See: wm.c
// IN: 
// parent window, target window,
// border size, border color1, border color2, bordercolor3,
// ornament color1, ornament color2, 
// style.
// #todo: We need the style dependent variables for these colors.

// #todo: use color scheme here.
    if ( type == WT_OVERLAPPED || 
         type == WT_EDITBOX_SINGLE_LINE || 
         type == WT_EDITBOX_MULTIPLE_LINES || 
         type == WT_BUTTON ||
         type == WT_ICON )
    {
        if ((void*) __w != NULL)
        {
            wmCreateWindowFrame ( 
                (struct gws_window_d *) pWindow,
                (struct gws_window_d *) __w, 
                METRICS_BORDER_SIZE,
                (unsigned int) COLOR_BORDER2,  //COLOR_BLACK,  // border color 1
                (unsigned int) COLOR_BORDER2,  //COLOR_BLACK,  // border color 2
                (unsigned int) COLOR_BORDER2,  //COLOR_BLACK,  // border color 3
                (unsigned int) COLOR_ORNAMENT,  //0x00C3C3C3,   // ornament color 1
                (unsigned int) COLOR_ORNAMENT,  //0x00C3C3C3,   // ornament color 2
                1 );  // style
        }
    }

// z order for overlapped.
// Quando criamos uma overlapped, ela deve vicar no topo da pilha.
    if (type == WT_OVERLAPPED)
    {
        // #bugbug
        // refaz a lista de zorder...
        // somente com overlalled
        //reset_zorder();
        // #bugbug isso nao eh bom.
        //invalidate parent, if present
        //invalidate_window(__w->parent);
        // coloca a nova janela no topo.
        __w->zIndex = ZORDER_TOP;
        zList[ZORDER_TOP] = (unsigned long) __w;
    }

// ===============

// level
// #test
    //server_debug_print ("CreateWindow.draw_frame: level stuff \n");    

    if ((void*) pWindow != NULL){
        __w->level = (pWindow->level + 1);
    }
    if ((void*) pWindow == NULL){
        __w->level = 0;
    }


// ================================

    /*
    if (type == WT_OVERLAPPED)
    {
    }
    */

// ===============
// Unlock the window
// Only at the end of this routine.
// #bugbug: We cant create the parts of the window
// if the window is locked.
// So, we can't lock it at the beginning.
    //__w->locked = FALSE;
    __w->enabled = TRUE;
// Invalidate the window rectangle.
// Only at the end of this routine.
    __w->dirty = TRUE;
    return (void *) __w;
fail:
    //server_debug_print ("CreateWindow: Fail\n");
    return NULL;
}

// RegisterWindow: 
// Register a window.
// OUT:
// < 0 = fail.
// > 0 = Ok. (index)
int RegisterWindow(struct gws_window_d *window)
{
    register int Slot=0;
    struct gws_window_d *tmp; 

// Parameter
    if ((void *) window == NULL){
        //gws_debug_print ("RegisterWindow: window struct\n");
        goto fail;
    }
// #todo ?
    //if (window->magic != 1234)
        //goto fail;

// Contagem de janelas e limites.
// (é diferente de id, pois id representa a posição
// da janela na lista de janelas).

    windows_count++;
    if (windows_count >= WINDOW_COUNT_MAX){
        printf ("RegisterWindow: windows_count\n");
        goto fail;
    }

// Probe for an empty slot.
// When found, the slot number becomes the WID.
    for (Slot=0; Slot<WINDOW_COUNT_MAX; ++Slot)
    {
        tmp = (struct gws_window_d *) windowList[Slot];
        if ((void *) tmp == NULL)
        {
            windowList[Slot] = (unsigned long) window;
            window->id = (int) Slot;
            return (int) Slot;
        }
    };
// After the loop.
fail:
    //server_debug_print("No more slots\n");
    return (int) (-1);
}

int destroy_window_by_wid(int wid)
{
    struct gws_window_d *window;
    struct gws_window_d *tmpw;
    int fRebuildList = FALSE;
    int fUpdateDesktop = FALSE;
    register int i=0;

// Parameter
    if (wid < 0){
        goto fail;
    }
    if (wid >= WINDOW_COUNT_MAX){
        goto fail;
    }

// Window structure.
    window = (struct gws_window_d *) get_window_from_wid(wid);
    if ((void*) window == NULL)
        goto fail;
    if (window->magic != 1234)
        goto fail;

// We can't destroy the root window.
// #todo
// The shutdown routine weill destroy it manually.
    if (window == __root_window){
        goto fail;
    }

// --------------------------------------
// Not an overlapped window.

    if (window->type != WT_OVERLAPPED)
    {
        if ( window->isMinimizeControl == TRUE ||
             window->isMaximizeControl == TRUE ||
             window->isCloseControl == TRUE )
        {
            // #todo:
            // Yes, we also need to destroy this kind of window.
            goto fail;
        }

        // Remove it from the list
        for (i=0; i<WINDOW_COUNT_MAX; i++)
        {
            tmpw = (void*) windowList[i];
            if (tmpw == window)
                windowList[i] = 0;
        };
        // Why not?!
        // windowList[window->id] = 0;

        /*
        // #test
        if (window == keyboard_owner)
            keyboard_owner = taskbar2_window;
        if (window == mouse_owner)
            mouse_owner = taskbar2_window;

        set_focus(taskbar2_window);
        __set_foreground_tid(taskbar2_window->client_tid);
        */

        // #test
        // This thread will not be the foreground thread anymore.
        sc82(10012, window->client_tid, 0, 0);

        window->magic = 0;
        window->used = FALSE;
        window = NULL;

        return 0;
    }

// --------------------------------------
// Overlapped
// In this case we need to rebuild the list of window frames 
// and update the desktop.
    if (window->type == WT_OVERLAPPED)
    {
        fRebuildList = TRUE;
        fUpdateDesktop = TRUE;
    }

// ---------------
// Titlebar
// Get the WIDs for the controls and 
// destroy the titlebar window.
    int wid_min = -1;
    int wid_max = -1;
    int wid_clo = -1;
    tmpw = (void *) window->titlebar;
    if ((void*) tmpw != NULL)
    {
        if (tmpw->magic == 1234)
        {
            // Get wids.
            wid_min = (int) tmpw->Controls.minimize_wid;
            wid_max = (int) tmpw->Controls.maximize_wid;
            wid_clo = (int) tmpw->Controls.close_wid;
            // Destroy titlebar window.
            tmpw->magic = 0;
            tmpw->used = FALSE;
            tmpw = NULL;
        }
    }
// ---------------
// Destroy min control
    tmpw = (struct gws_window_d *) get_window_from_wid(wid_min);
    if ((void*) tmpw != NULL)
    {
        if (tmpw->magic == 1234)
        {
            tmpw->magic = 0;
            tmpw->used = FALSE;
            tmpw = NULL;
        }
    }
// ---------------
// Destroy max control
    tmpw = (struct gws_window_d *) get_window_from_wid(wid_max);
    if ((void*) tmpw != NULL)
    {
        if (tmpw->magic == 1234)
        {
            tmpw->magic = 0;
            tmpw->used = FALSE;
            tmpw = NULL;
        }
    }
// ---------------
// Destroy clo control
    tmpw = (struct gws_window_d *) get_window_from_wid(wid_clo);
    if ((void*) tmpw != NULL)
    {
        if (tmpw->magic == 1234)
        {
            tmpw->magic = 0;
            tmpw->used = FALSE;
            tmpw = NULL;
        }
    }
// ---------------
// Destroy the overlapped window.
// #todo
// + Destroy the child list.
// +...

    // Remove it from the list
    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        tmpw = (void*) windowList[i];
        if (tmpw == window)
            windowList[i] = 0;
    };
    // Why not?!
    // windowList[window->id] = 0;

    /*
    // #test
    if (window == keyboard_owner)
        keyboard_owner = taskbar2_window;
    if (window == mouse_owner)
        mouse_owner = taskbar2_window;

    set_focus(taskbar2_window);
    __set_foreground_tid(taskbar2_window->client_tid);
    */

    // #test
    // This thread will not be the foreground thread anymore.
    sc82(10012, window->client_tid, 0, 0);

    window->magic = 0;
    window->used = FALSE;
    window = NULL;

    if (fRebuildList == TRUE){
        wm_rebuild_list();
    }
    if (fUpdateDesktop == TRUE)
    {
        // IN: tile, show
        wm_update_desktop(TRUE,TRUE);
    }
// Done
    return 0;
fail:
    return (int) -1;
}

void DestroyAllWindows(void)
{
    register int i=0;
    struct gws_window_d *tmp;
    int wid = -1;

    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        tmp = (void*) windowList[i];
        if (tmp != NULL)
        {
            if (tmp->used == TRUE)
            {
                if (tmp->magic == 1234)
                {
                    wid = (int) tmp->id;
                    destroy_window_by_wid(wid);
                }
            }
        }
    };
}

void MinimizeAllWindows(void)
{
    register int i=0;
    struct gws_window_d *tmp;
    int wid = -1;

    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        tmp = (void*) windowList[i];
        if (tmp != NULL)
        {
            if (tmp->used == TRUE)
            {
                if (tmp->magic == 1234)
                {
                    if (tmp->type == WT_OVERLAPPED)
                    {
                        tmp->state = WINDOW_STATE_MINIMIZED;
                        //tmp->enabled = FALSE;
                    }
                }
            }
        }
    };
}

void MaximizeAllWindows(void)
{
    register int i=0;
    struct gws_window_d *tmp;
    int wid = -1;

    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        tmp = (void*) windowList[i];
        if (tmp != NULL)
        {
            if (tmp->used == TRUE)
            {
                if (tmp->magic == 1234)
                {
                    if (tmp->type == WT_OVERLAPPED)
                    {
                        tmp->state = WINDOW_STATE_MAXIMIZED;
                        //tmp->enabled = TRUE;
                    }
                }
            }
        }
    };
}

void RestoreAllWindows(void)
{
// Back to normal state

    register int i=0;
    struct gws_window_d *tmp;
    int wid = -1;

    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        tmp = (void*) windowList[i];
        if (tmp != NULL)
        {
            if (tmp->used == TRUE)
            {
                if (tmp->magic == 1234)
                {
                    if (tmp->type == WT_OVERLAPPED)
                    {
                        tmp->state = WINDOW_STATE_NORMAL;
                        //tmp->enabled = TRUE;
                    }
                }
            }
        }
    };
}

/*
int this_type_has_a_title(int window_type);
int this_type_has_a_title(int window_type)
{
    if (type == WT_OVERLAPPED){
        return TRUE;
    }
    return FALSE;
}
*/

/*
int this_type_can_become_active(int window_type);
int this_type_can_become_active(int window_type)
{
    if (type == WT_OVERLAPPED){
        return TRUE;
    }
    return FALSE;
}
*/

/*
// #todo
// Get the handle given the wid.
struct gws_window_d *get_window_object(int wid);
struct gws_window_d *get_window_object(int wid)
{
}
*/

void enable_window(struct gws_window_d *window)
{
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

    window->enabled = TRUE;
    if (window->type == WT_BUTTON)
        window->status = BS_DEFAULT;
}

void disable_window(struct gws_window_d *window)
{
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

    window->enabled = FALSE;
    if (window->type == WT_BUTTON)
        window->status = BS_DISABLED;
}

// Valid states only.
void change_window_state(struct gws_window_d *window, int state)
{
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

// Is it a valid state?
// #todo: We can create a worker for this routine.
    int is_valid = FALSE;
    switch (state){
        case WINDOW_STATE_FULL:
        case WINDOW_STATE_MAXIMIZED:
        case WINDOW_STATE_MINIMIZED:
        case WINDOW_STATE_NORMAL:
            is_valid = TRUE;
            break;
        default:
            is_valid = FALSE;
            break;
    };

    if (is_valid == TRUE){
        window->state = state;
    }
}

void maximize_window(struct gws_window_d *window)
{

// Parameter:
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

// We only maximize application windows.
    if (window->type != WT_OVERLAPPED)
        return;

// Can't maximize root or taskbar
// They are not overlapped, but anyway.
    if (window == __root_window)
        return;
    if (window == taskbar2_window)
        return;

// Enable input for overlapped window.
    window->enabled = TRUE;

// Maximize it.
    change_window_state( window, WINDOW_STATE_MAXIMIZED );

// Initialization: 
// Using the working area by default.
    unsigned long l = WindowManager.wa.left;
    unsigned long t = WindowManager.wa.top;
    unsigned long w = WindowManager.wa.width;
    unsigned long h = WindowManager.wa.height;

    if (MaximizationStyle.initialized != TRUE)
    {
        //MaximizationStyle.style = 1;  // full
        //MaximizationStyle.style = 2;  // partial 00
        MaximizationStyle.style = 3;  // partial 01
        MaximizationStyle.initialized = TRUE;
    }

    // Based on style
    int Style = MaximizationStyle.style;
    switch (Style)
    {
        // full
        case 1:
            l = WindowManager.wa.left;
            t = WindowManager.wa.top;
            w = WindowManager.wa.width;
            h = WindowManager.wa.height;
            break;
        // partial 01
        case 2:
            l = (WindowManager.wa.left + 24);
            t = (WindowManager.wa.top  + 24);
            w = (WindowManager.wa.width  -24 -24);
            h = (WindowManager.wa.height -24 -24);
            break;
        // partial 02
        case 3:
            l = (WindowManager.wa.left);
            t = (WindowManager.wa.top);
            w = (WindowManager.wa.width);
            h = (WindowManager.wa.height -24);
            break;
        // full
        default:
            l = WindowManager.wa.left;
            t = WindowManager.wa.top;
            w = WindowManager.wa.width;
            h = WindowManager.wa.height;
            break;
    };

// --------------
    if ( w == 0 || h == 0 )
    {
        return;
    }
    gws_resize_window( window, 
        (w -4), 
        (h -4));
    gwssrv_change_window_position( window, 
        (l +2), 
        (t +2) );

//
// Redraw and display some windows:
// Root window, taskbar and the maximized window.
//

// Root
    redraw_window(__root_window,TRUE);

// Taskbar
// Send message to the app to repaint all the childs.
    redraw_window(taskbar2_window,TRUE);
    window_post_message( taskbar2_window->id, GWS_Paint, 0, 0 );

// Our window
// Set focus
// Redraw and show window

    set_focus(window);
    redraw_window(window,TRUE);

// Send message to the app to repaint all the childs.
    int target_wid = window->id;
    window_post_message( target_wid, GWS_Paint, 0, 0 );
}

// #test
// Minimize a window
// #ps: We do not have support for iconic window yet.
void minimize_window(struct gws_window_d *window)
{

// Parameter:
    if ((void*)window == NULL)
        return;
    if (window->magic != 1234){
        return;
    }

// We only minimize application windows.
    if (window->type != WT_OVERLAPPED)
        return;

// The minimized window can't receive input.
// The iconic window that belongs to the minimized window
// will be able to receive input.
// To restore this window, we need to do it via the iconic window.
    window->enabled = FALSE;

// Minimize it.
    change_window_state( window, WINDOW_STATE_MINIMIZED );

// Maybe we're still the active window,
// even minimized.
    //if (window == active_window)
    // ...

// Focus?
// Is the wwf one of our childs?
// Change the falg to 'not receiving input'.
    struct gws_window_d *wwf;
    wwf = (struct gws_window_d *) keyboard_owner;
    if ((void*) wwf != NULL)
    {
        if (wwf->magic == 1234)
        {
            if (wwf->parent == window){
                wwf->enabled = FALSE; // Can't receive input anymore.
            }
        }
    }

// Update the desktop respecting the current zorder.
    wm_update_desktop2();
}

// window_initialize:
// Initialize the window support.
// Called by gwsInitGUI() in gws.c.
int window_initialize(void)
{
    register int i=0;

// see: window.h
    windows_count = 0;

// Basic windows.
// At this moment we didn't create any window yet.

// Active window
    active_window = NULL;
// Input
    keyboard_owner = NULL;
    mouse_owner = NULL;
// Stack
    first_window = NULL;
    last_window = NULL;
    top_window = NULL;

    //...
    show_fps_window = FALSE;

// Window list
    for (i=0; i<WINDOW_COUNT_MAX; ++i){
        windowList[i] = 0;
    };

// z order list
// #bugbug
// z-order is gonna be a linked list.
    for (i=0; i<ZORDER_MAX; ++i){
        zList[i] = 0;
    };

    // ...

    return 0;

//fail:
    //return (int) -1;
}

//
// End
//

