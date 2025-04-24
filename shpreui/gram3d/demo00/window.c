/*
 * File: window.c 
 *      Creates a window.
 *      It's part of the gws project.
 * History:
 *     2019 - Created by Fred Nora.
 */
// #importante
// O frame de uma janela deve ser parte do Window Manager.
// #bugbug
// #todo
// Para lidarmos com essas estruturas do kernel
// devemos usar a chamada sci2. int 0x82. ??
// todo
// Quando estivermos contrindo as molduras das janelas,
// as rotinas devem retornar suas alturas, para assim
// o posicionamento do 'top' da área de cliente possa ser
// atualizado. (rect)


#include "gram3d.h"

#define OVERLAPPED_MIN_WIDTH    80
#define OVERLAPPED_MIN_HEIGHT   80
#define EDITBOX_MIN_WIDTH    8
#define EDITBOX_MIN_HEIGHT   8
#define BUTTON_MIN_WIDTH    8
#define BUTTON_MIN_HEIGHT   8

static int config_use_transparency=FALSE;

// habilitando/desabilitando globalmente 
// alguns componentes da janela

int gUseShadow = TRUE;
int gUseFrame = TRUE;
//int gUseShadow = TRUE;
// ...

// Windows - (struct)

extern struct gws_window_d  *__root_window; 
extern struct gws_window_d *active_window;
// If the window server has a taskbar.
// maybe we don't need that.
extern struct gws_window_d  *taskbar_window; 
extern struct gws_window_d  *taskbar_startmenu_button_window; 
//char startmenu_string[32];

// z-order ?
// But we can use multiple layers.
// ex-wayland: background, bottom, top, overlay.
extern struct gws_window_d *first_window;
extern struct gws_window_d *last_window;


//
// == private functions: prototypes =============
//

static struct gws_window_d *__create_window_object(void);


//
// =====================================
//

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
 * xxxCreateSurfaceWindow: 
 *     Create a surface in the kernel.
 *     --------------
 *     Cria uma janela com base em uma struct.
 *     Retorna o endereço da estrutura da janela criada. 
 *     Para que possa ser registrada na lista windowList[].
 *     Essa lista tambem fica em kgws no kernel.
 * Cria a janela dependendo do tipo:  
 * =================================
 * 1 - POP-UP.
 * 2 - EDIT-BOX.
 * 3 - NORMAL ( bg + client area ).
 * 4 - NORMAL_EDIT ( bg + editbox).
 * 5 - MENUBAR.
 * 6 - TOOLBAR.
 * 7 - STATUSBAR.
 * 8 - TASKBAR.
 * 9 - ? 
 * CONTINUA ...
 */
// Create a surface in ring0.
// #todo review that thing.
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
    unsigned long color )      //12, Color (bg) (para janela simples).
{ 

    // opaque
    // A ring0 structure pointer.
    void *wObjectPointer;
    
	// Enviando tudo via argumento.
	// Esse método dá a possibilidade de enviarmos ainda 
	// mais argumentos. 
	// #importante: Isso está funcionado, Vamos fazer assim e 
	// não do jeito antigo.

	unsigned long message_buffer[12];


    gwssrv_debug_print ("xxxCreateSurfaceWindow: \n");

	message_buffer[0]  = (unsigned long) type;
	message_buffer[1]  = (unsigned long) status;
	message_buffer[2]  = (unsigned long) view;
	message_buffer[3]  = (unsigned long) windowname;
	message_buffer[4]  = (unsigned long) x;
	message_buffer[5]  = (unsigned long) y;
	message_buffer[6]  = (unsigned long) width;
	message_buffer[7]  = (unsigned long) height;
	message_buffer[8]  = (unsigned long) pSurfaceWindow;
	message_buffer[9]  = (unsigned long) desktop_id;
	message_buffer[10] = (unsigned long) clientcolor;
	message_buffer[11] = (unsigned long) color;

// #bugbug
// Maybe we need to use sc82()

    wObjectPointer = 
        (void *) gramado_system_call ( 
                     118, 
                     (unsigned long) &message_buffer[0], 
                     (unsigned long) &message_buffer[0], 
                     (unsigned long) &message_buffer[0] );

    if ( (void *) wObjectPointer == NULL )
    {
        gwssrv_debug_print ("xxxCreateSurfaceWindow: [FAIL] wObjectPointer\n");
        return NULL;
    }

// #teste
// Vamos enviar a mensagem MSG_CREATE para o procedimento de janela.
// Assim ele pode terminar de pintar nessa mesma janela.

    return (void *) wObjectPointer;
}


// not tested yet
struct gws_surface_d *xxxCreateSurface( 
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    unsigned long color )
{
    struct gws_surface_d *surface;
    void *surfaceWindowObject;

//
// surface
//

    surface = 
        (struct gws_surface_d *) malloc ( sizeof( struct gws_surface_d ) );

    if ( (void*) surface == NULL )
    {
        //todo: message
        return NULL;
    }

    surface->initialized = FALSE;

//
// window
//

    surfaceWindowObject = 
        (void *) xxxCreateSurfaceWindow ( 
                     WT_SIMPLE, 1, 1, "Surface",
                     left, top, width, height, 
                     0, 0, COLOR_BLACK, COLOR_BLACK );  

    if ( (void*) surfaceWindowObject == NULL )
    {
        //todo: message
        return NULL;
    }
    
//
// structure
//

    surface->window_object = (void *) surfaceWindowObject;

    surface->left   = left;
    surface->top    = top;
    surface->width  = width;
    surface->height = height;

    surface->dirty = FALSE;
    surface->locked = FALSE;
    
    surface->next = NULL;
    surface->initialized = TRUE;

    surface->used = TRUE;
    surface->magic = 1234;

    return (struct gws_surface_d *) surface;
}

static struct gws_window_d *__create_window_object(void)
{
    struct gws_window_d *window;

    window = (void *) malloc( sizeof(struct gws_window_d) );
    if ((void *) window == NULL)
    {
        // #fixme
        debug_print ("__create_window_object: [ERROR] window\n");
        printf      ("__create_window_object: [ERROR] window\n");
        
        // #debug
        gwssrv_show_backbuffer();
        while (1){
        }
        // #todo 
        // We need to return.
        //return NULL; 
    }

    memset( window, 0, sizeof(struct gws_window_d) );
    window->used = TRUE;
    window->magic = 1234;

    return (struct gws_window_d *) window;
}

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
    unsigned long status, 
    unsigned long view, 
    char *windowname, 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    struct gws_window_d *pWindow, 
    int desktopid, 
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

// #todo:
// Receberemos isso via parametro de função.
// Default is FALSE.
// We need to know the parent's bg color.
    //int Transparent = FALSE;
    //int Transparent = TRUE;

    int Maximized=0;
    int Minimized=0;
    int Fullscreen=0;

// Bars
// A title bar é criadas pela função
// que cria o frame.

    // Title bar buttons. [v] [^] [X] 
    int MinimizeButton = FALSE;
    int MaximizeButton = FALSE;
    int CloseButton    = FALSE;
    // ...

    // Items.
    int Background    = FALSE;
    int ClientArea    = FALSE;
    int Shadow        = FALSE;
    int ButtonDown    = FALSE; 
    int ButtonUp      = FALSE;  // ??
    int ButtonSysMenu = FALSE;  // system menu na barra de títulos.
    int Border        = FALSE;  // usado no edit box.
    // ...

    // Desktop support.
    int ParentWindowDesktopId;    //Id do desktop da parent window.
    int WindowDesktopId;          //Id do desktop da janela a ser criada.

    //controle de janela
    struct gws_window_d *windowButton1;  // minimize
    struct gws_window_d *windowButton2;  // maximize
    struct gws_window_d *windowButton3;  // close

    // botões na barra de rolagem.
    struct gws_window_d *windowButton4;
    struct gws_window_d *windowButton5;
    struct gws_window_d *windowButton6;

// Structs

// The window object
    struct gws_window_d *window;
    struct gws_window_d *Parent;

    //struct desktop_d *Desktop;  //suspenso.

// The client rectangle.
// Isso é uma estrutura local para gerenciarmos o retangulo
// da área de cliente.
// Depois temos que copiar os valores para window->rcClient 

    struct gws_rect_d   clientRect;

//
// Border
//

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
// passado via argumento.
// left, top, width, height

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

// Button suport
    int buttonFocus    = FALSE;
    int buttonSelected = FALSE;
    unsigned int buttonBorderColor1=0;
    unsigned int buttonBorderColor2=0;
    unsigned int buttonBorderColor2_light=0;
    unsigned int buttonBorder_outercolor=0;  //Essa cor muda de acordo com o foco 

    //debug_print ("doCreateWindow:\n");

    int is_solid = TRUE;
    if (rop_flags != 0){
        is_solid = FALSE;
    }

//
// style
//

// #bugbug
// maximized, minimized and fullscreen are not style features.
// move these things to another flag.

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

// Salvar para depois restaurar os valores originais no fim da rotina.
    //unsigned long saveLeft;
    //unsigned long saveTop;

// Desktop:
// @todo: Configurar desktop antes de tudo. 
// @todo: Quando criamos uma janela temos de definir que ela
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
// @todo: devemos checar used e magic da janela mãe.
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
// No caso aqui, criarmos uma janela no mesmo desktop que a 
// janela mãe.
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

// #todo: 
// A atualização da contagem de janela deve ficar aqui,
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
        if ( CurrentColorScheme->used != 1 || 
             CurrentColorScheme->magic != 1234 )
        {
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

// The type
    window->type = (unsigned long) type;

// buffers:

    //window->DedicatedBuffer = (void*) windowCreateDedicatedBuffer(window);
    //window->BackBuffer = (void *) g_backbuffer_va;
    //window->FrontBuffer = (void *) g_frontbuffer_pa;

    window->dedicated_buf = NULL;
    window->back_buf      = NULL;
    window->front_buf     = NULL;
    window->depth_buf     = NULL;

// Device contexts
// #todo:
// We can create our own device contexts.
    window->window_dc = NULL;
    window->client_dc = NULL;

// A lot of flags
    window->style = (unsigned long) style;

    window->is_solid = (int) is_solid;
    window->rop = (unsigned long) rop_flags;

    window->status = (int) (status & 0xFFFFFFFF );
    window->view   = (int) view;
    window->focus  = FALSE;
    window->dirty  = FALSE;  // Validate
    window->locked = FALSE;

// Event queue.
    register int e=0;
    for (e=0; e<32; e++)
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
    //if (style & 0x8000){
    //    window->locked = TRUE;
    //}
    window->locked = FALSE;

// ===================================
// Input pointer device.
    window->ip_device = IP_DEVICE_NULL;
    window->ip_on = FALSE;  // desligado
// For keyboard
// Given in chars.
    window->ip_x = 0;       // in chars
    window->ip_y = 0;       // in chars
    window->ip_color = COLOR_BLACK;
    //window->ip_type = 0;    // #bugbug #todo
    //window->ip_style = 0;
// For mouse
// In pixel, for mouse pointer ip device.
    window->ip_pixel_x = 0;
    window->ip_pixel_y = 0;
// ===================================

// Id.
// We will get an id when we register the window.

    window->id = -1;

// Name
// This is just a pointer.
// Do we have a finalized string?
// #todo: What is the size?

    if ((void*) windowname == NULL){
        debug_print("doCreateWindow: Invalid windowname\n");
    }
    if (*windowname == 0){
        debug_print ("doCreateWindow: Invalid *windowname\n");
    }
    window->name = (char *) windowname;

// ===================================
// Parent window
    if ((void*) pWindow == NULL){
        debug_print("doCreateWindow: [#warning] Invalid pWindow\n");
    }
    Parent         = (struct gws_window_d *) pWindow;
    window->parent = (struct gws_window_d *) Parent;

// ===================================
// Child list
    window->child_list = NULL;

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

    // Active 
    if ( window->status == WINDOW_STATUS_ACTIVE )
    { 
        set_active_window(window); 
        
        //window->active = WINDOW_STATUS_ACTIVE;
        //window->status = (unsigned long) WINDOW_STATUS_ACTIVE;
        window->relationship_status = (unsigned long) WINDOW_REALATIONSHIPSTATUS_FOREGROUND; 
    
        //#todo
        //window->z = 0;  //z_order_get_free_slot()
        //...
    }

    // Inactive
    if ( status == WINDOW_STATUS_INACTIVE )
    { 
        //window->active = WINDOW_STATUS_INACTIVE;
        //window->status = (unsigned long) WINDOW_STATUS_INACTIVE;
        window->relationship_status = (unsigned long) WINDOW_REALATIONSHIPSTATUS_BACKGROUND;
   
        //todo
        //window->z = 0; //z_order_get_free_slot()
        //...
    }


//
// == Margins and dimensions ======================
//

// #todo:
// Se for uma janela filha o posicionamento deve ser somado às margens 
// da área de cliente da janela que será a janela mãe.
// #bugbug @todo 
// Esses valores de dimensões recebidos via argumento na verdade 
// devem ser os valores para a janela, sem contar o frame, que 
// inclui as bordas e a barra de títulos.

// Dimensions
// Passado via argumento.
    window->width  = (unsigned long) (WindowWidth  & 0xFFFF);
    window->height = (unsigned long) (WindowHeight & 0xFFFF);

// Maximized ?
    if (Maximized == TRUE)
    {
        window->absolute_x = deviceLeft;
        window->absolute_y = deviceTop;
        window->width  = deviceWidth;
        //#todo: Nesse momento temos que ter a altura da barra 
        //registrada em estrutura.
        window->height = (deviceHeight - 40); //menos a barra
    }

// Fullscreen
    if (Fullscreen == TRUE)
    {
        window->absolute_x = 0;//fullWindowX;
        window->absolute_y = 0;//fullWindowY;
        window->width  = deviceWidth;
        window->height = deviceHeight; 
    }

    window->width_in_chars  = 
        (unsigned long) (window->width / 8);  //>>3
    window->height_in_chars = 
        (unsigned long) (window->height / 8); //>>3

// =================================

// The frame rectangle.
// Initial configuration.
    window->rcWindow.left   = (unsigned long) WindowX;       // window left
    window->rcWindow.top    = (unsigned long) WindowY;       // window top
    window->rcWindow.width  = (unsigned long) WindowWidth;   // window width
    window->rcWindow.height = (unsigned long) WindowHeight;  // window height

//
// == Client area ==
//

// #todo
// Os valores da área de cliente são elaborados
// no decorrer da construção da janela.
// Inicialmente ela é do tamanho da janela.
// A área de cliente é relativa à janela.
// #todo: Qual tipo de janela tem área de cliente?
// #todo: Qual é o tamanho da área de cliente?
// é o valor passado via argumentos? e o frame é extra?
// Local

    //old
    //clientRect.left   = 0;
    //clientRect.top    = 0;
    //clientRect.width  = (unsigned long) window->width;
    //clientRect.height = (unsigned long) window->height;

//#test: considerando as bordas.
//#todo: usar os valores da largura das bordas.

// #todo: 
// Precisamos considerar a barra de títulos 
// se formos overlapped e tivermos uma. 
// Ou modificar o top depois de criarmos a barra de títulos.


// left
    clientRect.left = (unsigned long) 2;  // + borda da esq
// top
    clientRect.top  = (unsigned long) 2;  // + borda superior 
    // borda + barra.
    if (window->type == WT_OVERLAPPED){
        clientRect.top  = (unsigned long) 2+32;
    }
// width
// menos bordas laterais
    clientRect.width  = 
        (unsigned long) (window->width -2 -2 );
// height
// menos bordas superior e inferior
    // menos a barra de tarefas.
    clientRect.height = 
        (unsigned long) (window->height -2 -32 -2); 


// If we have scrollbars.
// #todo: Diminuimos as dimensões se o style
// indicar que temos scrollbars.
    //if (window->style & WS_VSCROLLBAR)
    //    clientRect.width -= 24;
    //if (window->style & WS_HSCROLLBAR)
    //    clientRect.height -=24;

// Save
// The original values. They are the same of the window.
    window->rcClient.left   = clientRect.left;    // window left
    window->rcClient.top    = clientRect.top;     // window top
    window->rcClient.width  = clientRect.width;   // window width
    window->rcClient.height = clientRect.height;  // window height

// =================================

// ??
// Deslocamento em relação a janela mãe.
// Passado via argumento.
    window->left = WindowX;
    window->top = WindowY;

//++
// Margens.
// Deslocando em relaçao a janela mae.

// We don't have a parent wiindow.
// If this is the first of all windows.
    window->absolute_x = WindowX;
    window->absolute_y = WindowY; 

// If we have a parent window.
// parent + arguments
    if ((void*) window->parent != NULL)
    {
        // Sempre é relativo à janela mãe.
        window->absolute_x = (window->parent->absolute_x + WindowX); 
        window->absolute_y = (window->parent->absolute_y + WindowY);
        
        // Se a janela mãe é overlapped,
        // então também é relativo à janela de cliente.
        // Pois é o lugar padrão para criar janelas cliente.
        // Isso só não será válido, se uma flag especial 
        // permitir criar uma janela fora da área de cliente.
        if (window->parent->type == WT_OVERLAPPED)
        {
            window->absolute_x += window->parent->rcClient.left;
            window->absolute_y += window->parent->rcClient.top;
        }
    }

// right and bottom.
    window->right  = (unsigned long) ( window->absolute_x + window->width );
    window->bottom = (unsigned long) ( window->absolute_y + window->height );
//--

// Maximized
// #bugbug
// max and full aren't the same thing.
// #todo: use the working area structure.
    if (Maximized == TRUE)
    {
        window->absolute_x = deviceLeft;
        window->absolute_y = deviceTop;
    }

// Fullscreen
    if (Fullscreen == TRUE)
    {
        window->absolute_x = deviceLeft;
        window->absolute_y = deviceTop;
    }

// Full ?
// Margins and dimensions for fullscreen mode.
// #bugbug
// This is valid only for some types of window.
// #bugbug
// left and top needs to be '0'?

    window->full_left   = fullWindowX;
    window->full_top    = fullWindowY;
    window->full_right  = (fullWindowX + fullWindowWidth);
    window->full_bottom = (fullWindowY + fullWindowHeight);
    window->full_width  = fullWindowWidth;
    window->full_height = fullWindowHeight;

// Color: Background and client area.
    window->bg_color            = (unsigned int) frame_color;
    window->clientrect_bg_color = (unsigned int) client_color;

//@todo: As outras características do cursor.
//Características.

//Estrutura para cursor.
//todo
    //window->cursor = NULL;

// #todo: 
// Uma opção é inicializarmos a estrutura de ponteiro depois ...
// pois tem janela que não tem ponteiro. 
// JÁ QUE NO MOMENTO ESTAMOS ENFRENTANDO ALGUNS TRAVAMENTOS.

    //window->cursor = (void*) malloc( sizeof(struct cursor_d) );

    //@@todo: Criar uma função: Inicializarcursor(struct cursor_d *cursor).
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
    //};


//Barras.
//As flags que representam a presença de cada uma das barras
//serão acionadas mais tarde, na hora da pintuda, de acordo com
//o tipo de janela à ser pintada.

// The child list
    window->child_list = NULL;

//
// Client window 
//

// (#importante)
// Client window support.
// Obs: A área de cliente será um retângulo e não uma janela.
// Mas nda impede da estrutra gerenciar uma janela que fique 
// em cima da área de cliente.

    window->client_window = NULL;  // window

//
// Terminal window
//

// #importante
// Terminal support.
// Suporte não tradicional à terminais.
// manipulando variáveis ao invés de uma estrutura.

    window->terminal_used = (int) 0;
    window->terminal_magic = (int) 0;

    //#suspenso
    //window->terminal_tab   = (int) 0;

    //window->terminal_left = (unsigned long) 0;
    //window->terminal_top = (unsigned long) 0;
    //window->terminal_width = (unsigned long) 0;
    //window->terminal_height = (unsigned long) 0;

//
// Desktop support
//

    //window->desktop = (void*) Desktop; //configurado anteriormente.
    //window->desktop_id = Desktop->id;  //@todo: verificar elemento.

    // What is that?
    window->isMenu    = 0;
    window->isButton  = 0;
    window->isEditBox = 0;


// Context menu: right click
// ou clicando no icone.
    window->contextmenu  = NULL;

// menu na menubar.
    window->barMenu      = NULL;


// Selected menu item.
// Caso a janela seja um ítem de menu.
    window->selected = FALSE; 
// Texto, caso a janela seja um ítem de menu.
    // window->text = NULL; 

// Actions
    window->draw   = FALSE;      //@todo: Cuidado com isso.
    window->redraw = FALSE;
    window->show   = TRUE; //Inicialmente presumimos que precisamos mostrar essa janela.
    // Continua ...


// Abaixo, elementos referenciados com menor frequência.

    //window->desktop = NULL; //@todo: Definir à qual desktop a janela perence.
    //window->process = NULL; //@todo: Definir à qual processo a janela perence.

// Linked list.

    //window->linkedlist = NULL;

//
// Navigation
//

// Prev e next.
    window->prev = (void *) Parent;
    window->next = NULL;

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
    window->clientAreaUsed = FALSE;  // 8
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
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = FALSE;
        Background = TRUE;
        window->backgroundUsed = TRUE;
        window->background_style = 0;
        break;

    // Edit box. (Simples + borda preta).
    // Editbox não tem sombra, tem bordas.
    case WT_EDITBOX:
    case WT_EDITBOX_MULTIPLE_LINES:
        window->ip_device = IP_DEVICE_KEYBOARD;
        window->frame.used = TRUE;
        Background = TRUE;
        Border     = TRUE;
        window->backgroundUsed = TRUE;
        window->background_style = 0;
        break;

    // Overlapped. (completa, para aplicativos).
    // Sombra, bg, título + borda, cliente area ...
    // #obs: Teremos recursividade para desenhar outras partes.
    case WT_OVERLAPPED:
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = TRUE;
        Shadow         = TRUE;
        Background     = TRUE;
        ClientArea     = TRUE;
        MinimizeButton = TRUE;
        MaximizeButton = TRUE;
        CloseButton    = TRUE; 
        window->shadowUsed     = TRUE;
        window->backgroundUsed = TRUE;
        window->titlebarUsed   = TRUE;
        window->controlsUsed   = TRUE;
        window->clientAreaUsed = TRUE;
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
    // #todo: Button has borders.
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

    // Ícone da área de trabalho.
    // #todo: icons has borders sometimes.
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
    if (window->view == VIEW_NULL)
    {
        //#bugbug: fail.
        //window->show = 0;
        //window->redraw = 0;
        //return (void*) window;
    }

// Minimized ? (Hide ?)
// Se tiver minimizada, não precisa mostrar a janela, porém
// é necessário pintar a janela no buffer dedicado, se essa técnica 
// estiver disponível.
// Talvez antes de retornarmos nesse caso seja necessário configurar 
// mais elementos da estrutura.
//#bugbug
//se estamos contruindo a janela, então ela não foi registrada 
//não podemos checar as coisas na estrutura ainda,
//mas a estrutura ja existe a algumas coisas foram inicializadas.
// #importante
// Pois retornaremos no caso de janelas minimizadas.
// Provavelmente isso foi usado quando criamos janelas 
// de referência na inicialização da GUI.(root)

    /*
    Minimized = 0;
    Minimized = (int) is_window_minimized (window);

    if ( Minimized == 1 )
    {
        //window->draw = 1; //Devemos pintála no buffer dedicado.
        window->show = 0;
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
        
        window->right = (unsigned long) window->left + window->width;
        window->bottom = (unsigned long) window->top  + window->height;       

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
// #Shadow
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

        if ( (unsigned long) type == WT_OVERLAPPED )
        {
            if (window->focus == TRUE) { __tmp_color = xCOLOR_GRAY1; }
            if (window->focus == FALSE){ __tmp_color = xCOLOR_GRAY2; }

            //ok funciona
            //rectBackbufferDrawRectangle ( window->left +1, window->top +1, 
            //    window->width +1 +1, window->height +1 +1, 
            //    __tmp_color, 1 ); 
         
            //test
            //remeber: the first window do not have a parent.
            if ( (void*) Parent == NULL )
            { 
                // #check
                // This routine is calling the kernel to paint the rectangle.
                rectBackbufferDrawRectangle ( 
                    (window->absolute_x +1),     (window->absolute_y +1), 
                    (window->width +1 +1), (window->height +1 +1), 
                    __tmp_color, TRUE, rop_flags );
            }

            if ( (void*) Parent != NULL )
            {
                // #check
                // This routine is calling the kernel to paint the rectangle.
                rectBackbufferDrawRectangle ( 
                    (window->absolute_x +1),     (window->absolute_y +1), 
                    (window->width +1 +1), (window->height +1 +1), 
                    __tmp_color, TRUE, rop_flags );
            }

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
// ## Background ##
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
            case WT_OVERLAPPED:
            case WT_POPUP:
            case WT_EDITBOX:
            case WT_EDITBOX_MULTIPLE_LINES:
            case WT_CHECKBOX:
            case WT_ICON:
            case WT_BUTTON:
                window->bg_color = (unsigned int) frame_color;
                //if (window->focus == TRUE)
                //    window->bg_color = (unsigned int) COLOR_BLUE;
                break;
            //#fixme
            default:
                // #fail
                window->bg_color = (unsigned int) COLOR_PINK;  //BLACK?
                //window->bg_color = CurrentColorScheme->elements[csiWindowBackground]; 
                break;
        };

        // Se a janela for edibox, vamos colocar ela 
        // dentro da área de cliente da janela mãe.
        // Editbox só pode existir dentro da área de cliente.
        // Seu posicionamento em relação a tela
        // também leva em conta o posicionamento 
        // da área de cliente da janela mãe

        if (type==WT_EDITBOX || type==WT_EDITBOX_MULTIPLE_LINES)
        {
            if ( (void*) window->parent != NULL )
            {
                window->absolute_x = 
                    (window->absolute_x + window->parent->rcClient.left);
                window->absolute_y = 
                    (window->absolute_y  + window->parent->rcClient.top);
            }
        }

        // Draw the background if we do not have a parent window.
        // Remember: The first window do not have a parent.
        if ((void*) Parent == NULL)
        {
            // #check
            // This routine is calling the kernel to paint the rectangle.
            rectBackbufferDrawRectangle ( 
                    window->absolute_x, window->absolute_y, window->width, window->height, 
                    window->bg_color, TRUE, rop_flags );
        }

        // Draw the background if we have a parent window.
        if ((void*) Parent != NULL)
        {
            // #check
            // This routine is calling the kernel to paint the rectangle.
            rectBackbufferDrawRectangle ( 
                window->absolute_x, window->absolute_y, window->width, window->height, 
                window->bg_color, TRUE, rop_flags );
        }
    }

    //#debug
    //asm ("int $3");

// ===================
// Client area rectangle.

    if (ClientArea == TRUE){
        window->rcClient.left   = (unsigned long) 0; //window->absolute_x;
        window->rcClient.top    = (unsigned long) 0; //window->absolute_y;
        window->rcClient.width  = (unsigned long) window->width;
        window->rcClient.height = (unsigned long) window->height;
    }

//
// == Button ====================
//

// Termina de desenhar o botão, mas não é frame
// é só o botão...
// caso o botão tenha algum frame, será alguma borda extra.
// border color:
// #bugbug
// o conceito de status e state
// está meio misturado. ja que estamos usando
// a função de criar janela para criar botão.

    if ((unsigned long) type == WT_BUTTON)
    {
        switch (status)  // ??
        {
            case BS_FOCUS:
                window->focus = TRUE;
                buttonFocus = TRUE;
                buttonBorderColor1       = COLOR_BLUE;
                buttonBorderColor2       = COLOR_BLUE;
                buttonBorderColor2_light = xCOLOR_GRAY5;
                buttonBorder_outercolor  = COLOR_BLUE;
                break;

            case BS_PRESS:
                buttonSelected = TRUE;
                buttonBorderColor1       = COLOR_WHITE; 
                buttonBorderColor2       = xCOLOR_GRAY3;
                buttonBorderColor2_light = xCOLOR_GRAY5; 
                buttonBorder_outercolor  = COLOR_BLACK;
                break;

            case BS_HOVER:
                break;
                    
            case BS_DISABLED:
                buttonFocus = FALSE;
                buttonBorderColor1 = COLOR_GRAY;
                buttonBorderColor2 = COLOR_GRAY;
                buttonBorder_outercolor  = COLOR_GRAY;
                break;

            case BS_PROGRESS:
                break;

            case BS_DEFAULT:
            default: 
                buttonFocus = FALSE;
                buttonSelected = FALSE;
                buttonBorderColor1       = COLOR_WHITE;  // left/top
                buttonBorderColor2       = xCOLOR_GRAY3; // right/bottom
                buttonBorderColor2_light = xCOLOR_GRAY5;
                buttonBorder_outercolor  = COLOR_BLACK;
                break;
        };

        // Label support.
        size_t tmp_size = (size_t) strlen ( (const char *) windowname );
        if (tmp_size>64){
            tmp_size=64;
        }
        unsigned long offset = 
            ( ( (unsigned long) window->width - ( (unsigned long) tmp_size * (unsigned long) gcharWidth) ) >> 1 );

        // #debug: 
        // Se o botão não tem uma parent window.
        if ( (void*) Parent == NULL ){
            gwssrv_debug_print ("doCreateWindow: [WT_BUTTON] Parent NULL\n"); 
        }

        // Se o botão tem uma parent window.
        // Paint button
        if ( (void*) Parent != NULL )
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
            __draw_button_borders(
                (struct gws_window_d *) window,
                (unsigned int) buttonBorderColor1,
                (unsigned int) buttonBorderColor2,
                (unsigned int) buttonBorderColor2_light,
                (unsigned int) buttonBorder_outercolor );

            // Button label

            if (buttonSelected == TRUE){
                grDrawString ( 
                    (window->absolute_x) +offset, 
                    (window->absolute_y) +8, 
                    COLOR_WHITE, window->name );
            }

            if (buttonSelected == FALSE){
                grDrawString ( 
                    (window->absolute_x) +offset,  
                    (window->absolute_y) +8,  
                    COLOR_BLACK, window->name );
            }
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
    debug_print ("doCreateWindow:\n");
    return NULL;
}


// CreateWindow:
// It creates a window.
// Essa será a função que atenderá a chamada a
// esse é o serviço de criação da janela.
// talvez ampliaremos o número de argumentos
// #todo
// Precisamos de uma estrutura de janela que fique aqui
// no servidor.
// Não podemos usar a estrutura de janela da api.
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
    unsigned long status, 
    unsigned long view, 
    char *windowname, 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    struct gws_window_d *pWindow, 
    int desktopid, 
    unsigned int frame_color, 
    unsigned int client_color ) 
{
   struct gws_window_d  *__w;

// #test:
// rop
   unsigned long __rop_flags=0;

// This function is able to create some few 
// kinds of windows for now:
// overlapped, editbox, button and simple.

    int ValidType=FALSE;

//
// name
//

// =================
// Duplicate
    char *name_local_copy;
    name_local_copy = (void*) malloc(256);
    if ((void*) name_local_copy == NULL){
        return NULL;
    }
    memset(name_local_copy,0,256);
    strcpy(name_local_copy,windowname);
// =================

// See:
// config.h, main.c
    if (config_use_transparency == TRUE)
    {
        __rop_flags = 1;     // or
        //__rop_flags = 2;   // and
        //__rop_flags = 3;   // xor
        //__rop_flags = 4;   // nand
        //__rop_flags = 10;  // less red
        //__rop_flags = 12;  // blue
        //__rop_flags = 20;  // gray 
        //__rop_flags = 21;  // no red
    }

// #todo:
// Colocar mascara nos valores passados via parâmetro.

    switch (type){
    case WT_OVERLAPPED:
        ValidType=TRUE;
        break;
    case WT_EDITBOX:
        ValidType=TRUE;
        break;
    case WT_EDITBOX_MULTIPLE_LINES:
        ValidType=TRUE;
        break;
    case WT_BUTTON:
        ValidType=TRUE;
        break;
    case WT_SIMPLE:
        ValidType=TRUE;
        break;
    };

    if (ValidType == FALSE)
    {
        gwssrv_debug_print("CreateWindow: Invalid type\n");
        goto fail;
        //return NULL;
    }

//1. Começamos criando uma janela simples
//2. depois criamos o frame. que decide se vai ter barra de títulos ou nao.

    /*
    __w = (void *) CreateWindow ( type, status, view, (char *) windowname, 
                           x, y, width, height, 
                           (struct window_d *) pWindow, desktopid, clientcolor, color );  
    */

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

// ============================
// Types with frame.

//====
// Overlapped
    if (type == WT_OVERLAPPED)
    {
        //gwssrv_debug_print ("CreateWindow: WT_OVERLAPPED\n");
        
        // #test
        // #todo: precisamos de um request que selecione
        // o modo de operação do window manager.
        // pode ser assincrono.
        
        if( WindowManager.initialized == TRUE)
        {
            // 1 = Tiling mode.
            // Do tamanho da área de trabalho.
            if (WindowManager.mode == 1)
            {
                x=0;
                y=0;
                //#bugbug: Confiando nos valores.
                width  = WindowManager.wa_width;
                height = WindowManager.wa_height;
            }
        }

        if (width < OVERLAPPED_MIN_WIDTH)  { width=OVERLAPPED_MIN_WIDTH; }
        if (height < OVERLAPPED_MIN_HEIGHT){ height=OVERLAPPED_MIN_HEIGHT; }

        __w = 
            (void *) doCreateWindow ( 
                         WT_SIMPLE, style, status, view, (char *) name_local_copy,
                         x, y, width, height, 
                         (struct gws_window_d *) pWindow, 
                         desktopid, 
                         frame_color, client_color, 
                         (unsigned long) __rop_flags ); 

        if ((void *) __w == NULL){
            gwssrv_debug_print("CreateWindow: on doCreateWindow\n");
            goto fail;
        }

        // Pintamos simples, mas a tipagem será overlapped.
        __w->type = WT_OVERLAPPED;
        __w->locked = FALSE;

        wm_add_window_into_the_list(__w);
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
        //gwssrv_debug_print ("CreateWindow: WT_EDITBOX WT_EDITBOX_MULTIPLE_LINES \n");

        //if ( (void*) pWindow == NULL ){ return NULL; }

        if ( width < EDITBOX_MIN_WIDTH )  { width=EDITBOX_MIN_WIDTH; }
        if ( height < EDITBOX_MIN_HEIGHT ){ height=EDITBOX_MIN_HEIGHT; }

        __w = 
            (void *) doCreateWindow ( 
                         WT_SIMPLE, 0, status, view, (char *) name_local_copy, 
                         x, y, width, height, 
                         (struct gws_window_d *) pWindow, 
                         desktopid, 
                         frame_color, client_color, 
                         (unsigned long) __rop_flags ); 

        if ((void *) __w == NULL){
            gwssrv_debug_print("CreateWindow: on doCreateWindow\n");
            goto fail;
        }

        // Pintamos simples, mas a tipagem será  overlapped
        //__w->type = WT_EDITBOX;
        __w->type = type;
        __w->locked = FALSE;
        goto draw_frame;
    }

// #todo
// It does not exist by itself. 
// It needs a parent window.

//====
// button
// Podemos usar o esquema padrão de cores ...
    if (type == WT_BUTTON)
    {
        //gwssrv_debug_print ("CreateWindow: WT_BUTTON \n");

        //if ( (void*) pWindow == NULL ){ return NULL; }

        if ( width < BUTTON_MIN_WIDTH )  { width=BUTTON_MIN_WIDTH; }
        if ( height < BUTTON_MIN_HEIGHT ){ height=BUTTON_MIN_HEIGHT; }

        __w = 
            (void *) doCreateWindow ( 
                         WT_BUTTON, 0, status, view, (char *) name_local_copy, 
                         x, y, width, height, 
                         (struct gws_window_d *) pWindow, 
                         desktopid, 
                         frame_color, client_color, 
                         (unsigned long) __rop_flags );

        if ((void *) __w == NULL){
            gwssrv_debug_print("CreateWindow: on doCreateWindow\n");
            goto fail;
        }

        // Pintamos simples, mas a tipagem será  overlapped
        __w->type = WT_BUTTON;
        __w->locked = FALSE;
        goto draw_frame;
    }

// ============================
// Types with no frame!

//====
//simple
    if (type == WT_SIMPLE)
    {
        //gwssrv_debug_print ("CreateWindow: WT_SIMPLE \n");

        __w = 
            (void *) doCreateWindow ( 
                         WT_SIMPLE, 0, status, view, (char *) name_local_copy,
                         x, y, width, height, 
                         (struct gws_window_d *) pWindow, 
                         desktopid, 
                         frame_color, client_color, 
                         (unsigned long) __rop_flags );

        if ((void *) __w == NULL){
            gwssrv_debug_print("CreateWindow: on doCreateWindow\n");
            goto fail;
        }

        __w->type = WT_SIMPLE;
        __w->locked = FALSE;

        goto draw_frame;
        //return (void *) __w;
    }

//type_fail:
    gwssrv_debug_print("CreateWindow: [FAIL] type\n");
    goto fail;

//
// == Draw frame ===============================
//

// #todo:
// Lembrando que frame é coisa do wm.
// Porém tem algumas coisas que o window server faz,
// como as bordas de um editbox.

draw_frame:

    //gwssrv_debug_print ("CreateWindow: draw_frame \n");

    if ((void*) __w == NULL){
        gwssrv_debug_print("CreateWindow.draw_frame: __w\n");
        goto fail;
    }
    if (__w->magic != 1234){
        gwssrv_debug_print("CreateWindow.draw_frame: __w->magic\n");
        goto fail;
    }

// #IMPORTANTE
// DESENHA O FRAME DOS TIPOS QUE PRECISAM DE FRAME.
// OVERLAPED, EDITBOX, CHECKBOX ...

// draw frame.
// #todo:
// Nessa hora essa rotina podera criar a barra de títulos.
// o wm poderá chamar a rotina de criar frame.
// See: wm.c

    if ( type == WT_OVERLAPPED || 
         type == WT_EDITBOX || 
         type == WT_EDITBOX_MULTIPLE_LINES || 
         type == WT_BUTTON )
    {
        if ( (void*) __w != NULL ){

        // IN: 
        // parent window, target window,
        // border size, border color1, border color2, bordercolor3,
        // ornament color1, ornament color2, 
        // style.
        wmCreateWindowFrame ( 
            (struct gws_window_d *) pWindow,
            (struct gws_window_d *) __w, 
            METRICS_BORDER_SIZE,
            (unsigned int)COLOR_BLACK, 
            (unsigned int)COLOR_BLACK,
            (unsigned int)COLOR_BLACK,
            (unsigned int)xCOLOR_GRAY6,
            (unsigned int)xCOLOR_GRAY3,
            1 );
        }
    }

//
// z order for overlapped.
//

    // Quando criamos uma overlapped, ela deve vicar no topo da pilha.
    if ( type == WT_OVERLAPPED )
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

//
// level
//

// #test
    //gwssrv_debug_print ("CreateWindow.draw_frame: level stuff \n");    

    if ((void*) pWindow != NULL){
        __w->level = (pWindow->level + 1);
    }
    if ((void*) pWindow == NULL){
        __w->level = 0;
    }

    // ===============

// Unlock the window
    __w->locked = FALSE;
// Invalidate the window rectangle.
    __w->dirty = TRUE;

//done:
    //gwssrv_debug_print ("CreateWindow: done\n");
    return (void *) __w;
fail:
    gwssrv_debug_print ("CreateWindow: FAIL\n");
    return NULL;
}

// RegisterWindow: 
//     Register a window.
// OUT:
// < 0 = fail.
// > 0 = Ok. (index)
 
int RegisterWindow(struct gws_window_d *window)
{
    register int __slot=0;
    struct gws_window_d *tmp;

    if ((void *) window == NULL){
        //gws_debug_print ("RegisterWindow: window struct\n");
        return (int) -1;
    }

// Contagem de janelas e limites.
// (é diferente de id, pois id representa a posição
// da janela na lista de janelas).

    windows_count++;

    if (windows_count >= WINDOW_COUNT_MAX){
        //gws_debug_print ("RegisterWindow: Limits\n");
        printf ("RegisterWindow: Limits\n");
        return -1;
    }

// Search for empty slot
    for (__slot=0; __slot<1024; ++__slot)
    {
        tmp = (struct gws_window_d *) windowList[__slot];

        // Found!
        if ((void *) tmp == NULL)
        {
            windowList[__slot] = (unsigned long) window; 
            window->id = (int) __slot;

            return (int) __slot;
        }
    };

// fail
    //gwssrv_debug_print("No more slots\n");
    return (int) (-1);
}

/*
int this_type_has_a_title(int window_type);
int this_type_has_a_title(int window_type)
{
    if(type==WT_OVERLAPPED){
        return TRUE;
    }
    return FALSE;
}
*/

/*
int this_type_can_become_active(int window_type);
int this_type_can_become_active(int window_type)
{
    if(type==WT_OVERLAPPED){
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

//#todo
// get client rect froma given window.
// #todo: All the types has a client window?
// Or is it valid only for overlapped windows?
struct gws_rect_d *clientrect_from_window(struct gws_window_d *window)
{
    struct gws_rect_d *rect;

    if ((void*) window == NULL)
        return NULL;
    if (window->used != TRUE)
        return NULL;
    if (window->magic != 1234)
        return NULL;

    rect = (struct gws_rect_d *) &window->rcClient;

    return (struct gws_rect_d *) rect;
}

//#todo
struct gws_rect_d *rect_from_window(struct gws_window_d *window)
{
    struct gws_rect_d *rect;

    if ((void*) window == NULL)
        return NULL;
    if (window->used != TRUE)
        return NULL;
    if (window->magic != 1234)
        return NULL;

    rect = (struct gws_rect_d *) &window->rcWindow;

    return (struct gws_rect_d *) rect;
}

//
// End
//



