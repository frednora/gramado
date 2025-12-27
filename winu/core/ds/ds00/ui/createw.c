// createw.c
// Basically create and destroy a window.
// 2019 - Created by Fred Nora.

// Window design:
// + Absolute coordinates = screen space.
// + Relative coordinates = parent space.
// + Client rect = drawable area after decorations.

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

#include "../ds.h"

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

// Create window structure
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

    // Absolute position relative to the screen (global coordinates).
    window->absolute_x = 0;
    window->absolute_y = 0;
    window->absolute_right = 0;
    window->absolute_bottom = 0;

    // Position and size relative to the parent window (local coordinates).
    window->left = 0;
    window->top = 0;
    window->width = 0;
    window->height = 0;

    // The window can receive input from kbd and mouse.
    window->enabled = TRUE;
    window->style = 0;
    // Validate. This way the compositor can't redraw it.
    window->dirty = FALSE;
    
    return (struct gws_window_d *) window;
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

// Create the controls given the titlebar.
// min, max, close.
int do_create_controls(struct gws_window_d *w_titlebar)
{
// Windows
    struct gws_window_d *w_minimize;
    struct gws_window_d *w_maximize;
    struct gws_window_d *w_close;
    //?
    int id = -1;

// Colors for the button
    unsigned int bg_color = (unsigned int) get_color(csiButton);
    //unsigned int bg_color = (unsigned int) get_color(csiButton);
    // ...

// Parameters:
    if ((void*)w_titlebar == NULL){
        goto fail;
    }
    if (w_titlebar->magic != 1234){
        goto fail;
    }
    //if(window->isTitleBar!=TRUE)
    //    return;

    w_titlebar->Controls.initialized = FALSE;

    w_titlebar->Controls.minimize_wid = -1;
    w_titlebar->Controls.maximize_wid = -1;
    w_titlebar->Controls.close_wid    = -1;

// Width and height.
    unsigned long ButtonWidth = 
        METRICS_TITLEBAR_CONTROLS_DEFAULT_WIDTH;
    unsigned long ButtonHeight = 
        METRICS_TITLEBAR_CONTROLS_DEFAULT_HEIGHT;

    unsigned long LastLeft = 0;

    unsigned long TopPadding=METRICS_TITLEBAR_CONTROLS_TOPPAD; // Top margin
    unsigned long RightPadding=METRICS_TITLEBAR_CONTROLS_RIGHTPAD;  // Right margin
    
    unsigned long SeparatorWidth=METRICS_TITLEBAR_CONTROLS_SEPARATOR_WIDTH;

// #test
// #bugbug
    //Top=1;
    //ButtonWidth  = (unsigned long) (w_titlebar->width -4);
    //ButtonHeight = (unsigned long) (w_titlebar->height -4);

// ================================================
// minimize

    LastLeft = 
        (unsigned long)( 
        w_titlebar->width - 
        (3*ButtonWidth) - 
        (2*SeparatorWidth) - 
        RightPadding );

    // Create a window of button type.
    w_minimize = 
        (struct gws_window_d *) CreateWindow ( 
            WT_BUTTON, 0, 1, 1, 
            "_",           // String  
            LastLeft,      // l 
            TopPadding,    // t 
            ButtonWidth,   // w
            ButtonHeight,  // t
            w_titlebar, 0, bg_color, bg_color );

    if ((void *) w_minimize == NULL){
        //server_debug_print ("xx: minimize fail \n");
        goto fail;
    }
    if (w_minimize->magic != 1234){
        goto fail;
    }
    w_minimize->isControl = TRUE;

    w_minimize->left_offset = 
        (unsigned long) (w_titlebar->width - LastLeft);

    w_minimize->type = WT_BUTTON;
    w_minimize->isMinimizeControl = TRUE;

    id = (int) RegisterWindow(w_minimize);
    if (id<0){
        //server_debug_print("xxx: Couldn't register w_minimize\n");
        goto fail;
    }
    w_titlebar->Controls.minimize_wid = (int) id;

// ================================================
// maximize

    LastLeft = 
        (unsigned long)(
        w_titlebar->width - 
        (2*ButtonWidth) - 
        (1*SeparatorWidth) - 
        RightPadding );

    // Create a window of button type.
    w_maximize = 
        (struct gws_window_d *) CreateWindow ( 
            WT_BUTTON, 0, 1, 1, 
            "M",           // String  
            LastLeft,      // l 
            TopPadding,    // t 
            ButtonWidth,   // w
            ButtonHeight,  // h
            w_titlebar, 0, bg_color, bg_color );

    if ((void *) w_maximize == NULL){
        //server_debug_print ("xx: w_maximize fail \n");
        goto fail;
    }
    if (w_maximize->magic!=1234){
        goto fail;
    }
    w_maximize->isControl = TRUE;
    
    w_maximize->left_offset = 
        (unsigned long) (w_titlebar->width - LastLeft);

    w_maximize->type = WT_BUTTON;
    w_maximize->isMaximizeControl = TRUE;
    id = RegisterWindow(w_maximize);
    if (id<0){
        //server_debug_print ("xxx: Couldn't register w_maximize\n");
        goto fail;
    }
    w_titlebar->Controls.maximize_wid = (int) id;

// ================================================
// close

    LastLeft = 
        (unsigned long)(
        w_titlebar->width - 
        (1*ButtonWidth) - 
        RightPadding );

    // Create a window of button type.
    w_close = 
        (struct gws_window_d *) CreateWindow ( 
            WT_BUTTON, 0, 1, 1, 
            "X",           // String  
            LastLeft,      // l 
            TopPadding,    // t 
            ButtonWidth,   // w
            ButtonHeight,  // h
            w_titlebar, 0, bg_color, bg_color );

    if ((void *) w_close == NULL){
        //server_debug_print ("xx: w_close fail \n");
        goto fail;
    }
    if (w_close->magic!=1234){
        goto fail;
    }
    w_close->isControl = TRUE;
    
    w_close->left_offset = 
        (unsigned long) (w_titlebar->width - LastLeft);

    w_close->type = WT_BUTTON;
    w_close->isCloseControl = TRUE;
    id = RegisterWindow(w_close);
    if (id<0){
        //server_debug_print ("xxx: Couldn't register w_close\n");
        goto fail;
    }
    w_titlebar->Controls.close_wid = (int) id;

    w_titlebar->Controls.initialized = TRUE;

// Done
    return 0;
fail:
    return (int) -1;
}

// Create titlebar and controls.
// It respects the border size of the parent.
struct gws_window_d *do_create_titlebar(
    struct gws_window_d *parent,
    unsigned long tb_height,
    unsigned int color,
    unsigned int ornament_color,
    int has_icon,
    int icon_id,
    int has_string,
    unsigned long string_color )
{
    struct gws_window_d *tbWindow;

// Titlebar position and size depend on parent’s border and style.
    unsigned long TitleBarLeft=0; // X offset inside parent
    unsigned long TitleBarTop=0;  // Y offset inside parent
    unsigned long TitleBarWidth=0; // Width of titlebar
    unsigned long TitleBarHeight = tb_height; // Height passed as parameter

    // Colors
    unsigned int TitleBarColor = color;
    unsigned int StringColor = string_color;
    unsigned long rop=0;
    //unsigned long rop= 0x20;
    int useIcon = FALSE;

// Parameters:
    if ((void*) parent == NULL)
        return NULL;
    if (parent->magic != 1234)
        return NULL;

// Get parameter.
    useIcon = has_icon;

// Overlapped + fullscreen or maximized: no border offset. 
    const int is_fullscreen = (parent->state == WINDOW_STATE_FULL); 
    const int is_maximized = (parent->state == WINDOW_STATE_MAXIMIZED);

// Use parent->border_size (must be set in doCreateWindowFrame before this call) 
    unsigned long b = parent->Border.border_size;

    TitleBarLeft  = b;
    TitleBarTop   = b;
    TitleBarWidth = parent->width - (2 * b);

    if (is_fullscreen || is_maximized) 
    {
        TitleBarLeft  = 0;
        TitleBarTop   = 0;
        TitleBarWidth = parent->width;
    }

    // Saving
    parent->titlebar_height = TitleBarHeight;
    parent->titlebar_width = TitleBarWidth;
    parent->titlebar_color = (unsigned int) TitleBarColor;
    parent->titlebar_text_color = 
        (unsigned int) get_color(csiTitleBarTextColor);
    // ...

    rop = (unsigned long) parent->rop;

//-----------

// #important: 
// WT_SIMPLE with text.
// lembre-se estamos relativos à area de cliente
// da janela mão, seja ela de qual tipo for.

    tbWindow = 
       (void *) doCreateWindow ( 
                WT_TITLEBAR, 0, 1, 1, "TitleBar", 
                TitleBarLeft, 
                TitleBarTop, 
                TitleBarWidth, 
                TitleBarHeight, 
                (struct gws_window_d *) parent, 
                0, 
                TitleBarColor,  //frame 
                TitleBarColor,  //client
                (unsigned long) rop );   // rop_flags from the parent 

    if ((void *) tbWindow == NULL){
        //server_debug_print ("do_create_titlebar: tbWindow\n");
        return NULL;
    }
    tbWindow->type = WT_SIMPLE;
    tbWindow->isTitleBar = TRUE;

// No room drawing more stuff inside the tb window.
    if (tbWindow->width == 0)
        return NULL;

// --------------------------------
// Icon

// O posicionamento em relação
// à janela é consistente por questão de estilo.
// See: bmp.c
// IN: index, left, top.
// Icon ID:
// Devemos receber um argumento do aplicativo,
// na hora da criação da janela.

// Decode the bmp that is in a buffer
// and display it directly into the framebuffer. 
// IN: index, left, top
// see: bmp.c
    unsigned long iL=0;
    unsigned long iT=0;
    unsigned long iWidth = 16;

    parent->titlebarHasIcon = FALSE;

    if (useIcon == TRUE)
    {
        // Icon ID
        if (icon_id < 1 || icon_id > 5)
        {
            //icon_id = 1;
            yellow_status("Invalid icon_id");
            return NULL;
        }
        parent->frame.titlebar_icon_id = icon_id;

        // Draw icon
        iL = (unsigned long) (tbWindow->absolute_x + METRICS_ICON_LEFTPAD);
        iT = (unsigned long) (tbWindow->absolute_y + METRICS_ICON_TOPPAD);
        bmp_decode_system_icon( 
            (int) icon_id, 
            (unsigned long) iL, 
            (unsigned long) iT,
            FALSE );
        parent->titlebarHasIcon = TRUE;
    }

// ---------------------------
// Ornament
    // int useOrnament = TRUE;

// Ornamento:
// Ornamento na parte de baixo da title bar.
// #important:
// O ornamento é pintado dentro da barra, então isso
// não afetará o positionamento da área de cliente.
// border on bottom.
// Usado para explicitar se a janela é ativa ou não
// e como separador entre a barra de títulos e a segunda
// área da janela de aplicativo.
// Usado somente por overlapped window.

    unsigned int OrnamentColor1 = ornament_color;
    unsigned long OrnamentHeight = METRICS_TITLEBAR_ORNAMENT_SIZE;
    parent->frame.ornament_color1   = OrnamentColor1;
    parent->titlebar_ornament_color = OrnamentColor1;

    unsigned long r0_left   = tbWindow->absolute_x;
    unsigned long r0_top    = ( (tbWindow->absolute_y) + (tbWindow->height) - METRICS_TITLEBAR_ORNAMENT_SIZE );
    unsigned long r0_width  = tbWindow->width;
    unsigned long r0_height = OrnamentHeight;
    unsigned long r0_color  = OrnamentColor1;
    unsigned long r0_rop    = 0;

// Draw rectangle
// IN: l, t, w, h, color, rop
    painterFillWindowRectangle( 
        r0_left, r0_top, r0_width, r0_height, r0_color, r0_rop );

//----------------------
// String
// Titlebar string support.
// Using the parent's name.

    int useTitleString = has_string;  //#HACK
    unsigned long StringLeftPad = 0;
    unsigned long StringTopPad = 8;  // char size.
    size_t StringSize = (size_t) strlen( (const char *) parent->name );
    if (StringSize > 64){
        StringSize = 64;
    }

// --------------------------------------
// Left PAD.
// pad | icon | pad | pad
    if (useIcon == FALSE){
        StringLeftPad = (unsigned long) METRICS_ICON_LEFTPAD;
    }
    if (useIcon == TRUE){
        StringLeftPad = 
            (unsigned long) ( METRICS_ICON_LEFTPAD +iWidth +(2*METRICS_ICON_LEFTPAD));
    }

// --------------------------------------
// Top PAD.

    StringTopPad = 
        ( ( (unsigned long) tbWindow->height - FontInitialization.height ) >> 1 );

//
// Text support
//

    // #todo
    // We already did that before.
    //parent->titlebar_text_color = 
        //(unsigned int) get_color(csiTitleBarTextColor);

// #todo
// Temos que gerenciar o posicionamento da string.
// #bugbug: Use 'const char *'

    tbWindow->name = (char *) strdup((const char *) parent->name);
    if ((void*) tbWindow->name == NULL)
    {
        // #todo: Recover
        printf("do_create_titlebar: Invalid name\n");
        return NULL;
    }

    unsigned long sL=0;
    unsigned long sT=0;
    //unsigned int sColor = (unsigned int) parent->titlebar_text_color;
    if (useTitleString == TRUE)
    {
        // Saving relative position.
        parent->titlebar_text_left = StringLeftPad;
        parent->titlebar_text_top = StringTopPad;
        sL = (unsigned long) ((tbWindow->absolute_x) + StringLeftPad);
        sT = (unsigned long) ((tbWindow->absolute_y) + StringTopPad);
        grDrawString ( sL, sT, StringColor, tbWindow->name );
    }

// ---------------------------------
// Controls
    int controls_ok = FALSE;
    controls_ok = (int) do_create_controls(tbWindow);
    // Only if we created the controls.
    if (controls_ok == 0){
        // Invalidade all the controls.
        invalidate_window_by_id(tbWindow->Controls.minimize_wid);
        invalidate_window_by_id(tbWindow->Controls.maximize_wid);
        invalidate_window_by_id(tbWindow->Controls.close_wid);
    }

    // Invalidate the tb window.
    tbWindow->dirty = TRUE;

// Now the parent has a valid ponter to the titlebar.
    parent->titlebar = (struct gws_window_d *) tbWindow;

    return (struct gws_window_d *) tbWindow;
}

// doCreateWindowFrame:
// Called by CreateWindow in createw.c
// #importante:
// Essa rotina será chamada depois que criarmos uma janela básica,
// mas só para alguns tipos de janelas, pois nem todos os tipos 
// precisam de um frame. Ou ainda, cada tipo de janela tem um 
// frame diferente. Por exemplo: Podemos considerar que um checkbox 
// tem um tipo de frame.
// Toda janela criada pode ter um frame.
// Durante a rotina de criação do frame para uma janela que ja existe
// podemos chamar a rotina de criação da caption bar, que vai criar os
// botões de controle ... mas nem toda janela que tem frame precisa
// de uma caption bar (Title bar).
// Estilo do frame:
// Dependendo do estilo do frame, podemos ou nao criar a caption bar.
// Por exemplo: Uma editbox tem um frame mas não tem uma caption bar.
// IN:
// parent = parent window ??
// window = The window where to build the frame.
// x
// y
// width
// height
// style = Estilo do frame.
// OUT:
// 0   = ok, no erros;
// < 0 = not ok. something is wrong.

int 
doCreateWindowFrame ( 
    struct gws_window_d *parent,
    struct gws_window_d *window,
    unsigned long border_size,
    unsigned int border_color1,
    unsigned int border_color2,
    unsigned int border_color3,
    unsigned int ornament_color1,
    unsigned int ornament_color2,
    int frame_style ) 
{
    int useFrame       = FALSE;
    int useTitleBar    = FALSE;
    int useTitleString = FALSE;
    int useIcon        = FALSE;
    int useStatusBar   = FALSE;
    int useBorder      = FALSE;
    // ...

// #bugbug
// os parâmetros 
// parent, 
// x,y,width,height 
// não estão sendo usados.

// Overlapped.
// Janela de aplicativos.

// Title bar and status bar.
    struct gws_window_d  *tbWindow;
    struct gws_window_d  *sbWindow;
    int id=-1;  //usado pra registrar janelas filhas.
    int Type=0;

// Border size
    unsigned long BorderSize = (border_size & 0xFFFF);

// Border color
    unsigned int BorderColor1 = border_color1;  // top/left
    unsigned int BorderColor2 = border_color2;  // right/bottom
    unsigned int BorderColor3 = border_color3;

// Ornament color
    unsigned int OrnamentColor1 = ornament_color1;
    unsigned int OrnamentColor2 = ornament_color2;

// Title bar height
    unsigned long TitleBarHeight = 
        METRICS_TITLEBAR_DEFAULT_HEIGHT;

// Titlebar colors for active window
    unsigned int TitleBarColor = (unsigned int) get_color(csiActiveWindowTitleBar);
    unsigned int TitleBarStringColor = (unsigned int) get_color(csiTitleBarTextColor);

    int icon_id = ICON_ID_APP;  // Default

    //unsigned long X = (x & 0xFFFF);
    //unsigned long Y = (y & 0xFFFF);
    //unsigned long Width = (width & 0xFFFF);
    //unsigned long Height = (height & 0xFFFF);

// #todo
// Se estamos minimizados ou a janela mãe está minimizada,
// então não temos o que pintar.
// #todo
// O estilo de frame é diferente se estamos em full screen ou maximizados.
// não teremos bordas laterais
// #todo
// Cada elemento da frame que incluimos, incrementa
// o w.top do retângulo da área de cliente.

// check parent
    if ((void*) parent == NULL){
        //server_debug_print ("doCreateWindowFrame: [FAIL] parent\n");
        goto fail;
    }
    if (parent->used != TRUE || parent->magic != 1234){
        goto fail;
    }

// check window
    if ((void*) window == NULL){
        //server_debug_print ("doCreateWindowFrame: [FAIL] window\n");
        goto fail;
    }
    if (window->used != TRUE || window->magic != 1234){
        goto fail;
    }

// Save border colors setted by the caller
    window->Border.border_color1 = (unsigned int) BorderColor1;
    window->Border.border_color2 = (unsigned int) BorderColor2;

// Uma overlapped maximizada não tem borda.
    int IsMaximized = FALSE;
    if (window->state == WINDOW_STATE_MAXIMIZED)
    {
        IsMaximized=TRUE;
    }
// Uma overlapped maximizada não tem borda.
    int IsFullscreen = FALSE;
    if (window->state == WINDOW_STATE_FULL){
        IsFullscreen=TRUE;
    }

// Absolute position (left/top)
    window->absolute_x = (window->absolute_x & 0xFFFF);
    window->absolute_y = (window->absolute_y & 0xFFFF);

// (Width/height)
    window->width  = (window->width  & 0xFFFF);
    window->height = (window->height & 0xFFFF);

// #test:
// Defaults:
// Colocamos default, depois muda de acordo com os parametros.
    window->frame.titlebar_icon_id = ICON_ID_DEFAULT;
    // ...

// #todo
// Desenhar o frame e depois desenhar a barra de títulos
// caso esse estilo de frame precise de uma barra.
// Editbox
// EDITBOX NÃO PRECISA DE BARRA DE TÍTULOS.
// MAS PRECISA DE FRAME ... QUE SERÃO AS BORDAS.

// Type
// Qual é o tipo da janela em qual precisamos
// criar o frame. Isso indica o tipo de frame.

    Type = window->type;

    switch (Type){

    case WT_EDITBOX:
    case WT_EDITBOX_MULTIPLE_LINES:
        useFrame=TRUE;
        window->is_frameless = FALSE;
        useIcon=FALSE;
        useBorder=TRUE;
        break;

    // Uma overlapped maximizada não tem borda.
    case WT_OVERLAPPED:
        useFrame=TRUE; 
        window->is_frameless = FALSE;
        useTitleBar=TRUE;  // Normalmente uma janela tem a barra de t[itulos.
        useTitleString=TRUE;
        useIcon=TRUE;
        useBorder=TRUE;
        // Quando a overlapped esta em fullscreen,
        // então não usamos title bar,
        // nem bordas.

        if (window->state == WINDOW_STATE_FULL)
        {
            //useFrame=FALSE;
            useTitleBar=FALSE;
            useTitleString=FALSE;
            useIcon=FALSE;
            useBorder=FALSE;
        }
        
        if (window->style & WS_STATUSBAR){
            useStatusBar=TRUE;
        }
        break;

    case WT_BUTTON:
    case WT_ICON:
        useFrame=TRUE;
        window->is_frameless = FALSE;
        useIcon=FALSE;
        break;

    //default: break;
    };

    if (useFrame == FALSE)
    {
        window->is_frameless = TRUE;
        //server_debug_print ("doCreateWindowFrame: [ERROR] This type does not use a frame.\n");
        goto fail;
    }

// ===============================================
// editbox

    if ( Type == WT_EDITBOX_SINGLE_LINE || 
         Type == WT_EDITBOX_MULTIPLE_LINES )
    {
        window->Border.border_size = BorderSize;
        window->borderUsed = TRUE;
        // Draw the border of an edit box
        __draw_window_border(parent,window);
        return 0;
    }

// ===============================================
// Overlapped?
// Draw border, titlebar and status bar.
// #todo:
// String right não pode ser maior que 'last left' button.

    if (Type == WT_OVERLAPPED)
    {
        // #todo
        // Maybe we nned border size and padding size.
        
        // Consistente para overlapped.
        //BorderSize = METRICS_BORDER_SIZE;
        // ...
        
        // #todo
        // The window structure has a element for border size
        // and a flag to indicate that border is used.
        // It also has a border style.

        // Quatro bordas de uma janela overlapped.
        // Uma overlapped maximizada não tem bordas.
        window->borderUsed = FALSE;
        
        if ( IsMaximized == FALSE && 
             IsFullscreen == FALSE)
        {
            //WindowManager.is_fullscreen = TRUE;
            //WindowManager.fullscreen_window = window;
            
            window->borderUsed = FALSE;
            // Only paint in the case of nor maximized and not full
            __draw_window_border(parent,window);
        }

        // #important:
        // The border in an overlapped window will affect
        // the top position of the client area rectangle.
        window->rcClient.top += window->Border.border_size;

        //
        // Title bar
        //

        // #todo
        // The window structure has a flag to indicate that
        // we are using titlebar.
        // It also has a title bar style.
        // Based on this style, we can setup some
        // ornaments for this title bar.
        // #todo
        // Simple title bar.
        // We're gonna have a wm inside the display server.
        // The title bar will be very simple.
        // We're gonna have a client area.
        // #bugbug
        // Isso vai depender da resolução da tela.
        // Um tamanho fixo pode fica muito fino em uma resolução alta
        // e muito largo em uma resolução muito baixa.
        
        // Title bar
        // Se a janela overlapped tem uma title bar.
        // #todo: Essa janela foi registrada?
        if (useTitleBar == TRUE)
        {
            // This is a application window.
            if (window->style & WS_APP)
                icon_id = ICON_ID_APP;
            // This is a application window.
            if (window->style & WS_DIALOG) 
                icon_id = ICON_ID_FILE;
            // This is a application window.
            if (window->style & WS_TERMINAL) 
                icon_id = ICON_ID_TERMINAL;

            // IN: 
            // parent, border size, height, color, ornament color,
            //  use icon, use string.
            tbWindow = 
                (struct gws_window_d *) do_create_titlebar(
                    window,
                    TitleBarHeight,
                    TitleBarColor,
                    OrnamentColor1,
                    useIcon,
                    icon_id,
                    useTitleString,
                    TitleBarStringColor );

            // Register window
            id = (int) RegisterWindow(tbWindow);
            if (id<0){
                //server_debug_print ("doCreateWindowFrame: Couldn't register window\n");
                goto fail;
            }
            // Add it to the list of childs
            wm_add_child_window(parent,tbWindow);

            // #important:
            // The Titlebar in an overlapped window will affect
            // the top position of the client area rectangle.
            // Depois de pintarmos a titlebar,
            // temos que atualizar o top da área de cliente.
            window->rcClient.top += window->titlebar_height;
        }  //--use title bar.
        // ooooooooooooooooooooooooooooooooooooooooooooooo

        // #todo:
        // nessa hora podemos pintar a barra de menu, se o style
        // da janela assim quiser. Depois disso precisaremos
        // atualizar o top da área de cliente.
        //window->rcClient.top += window->titlebar_height;

        // Status bar
        // (In the bottom)
        // #todo: It turns the client area smaller.
        if (useStatusBar == TRUE)
        {
            //#debug
            //printf ("sb\n");
            //while(1){}

            // #todo
            // Move these variables to the start of the routine.
            unsigned long sbLeft=0;
            unsigned long sbTop=0;
            unsigned long sbWidth=8;
            unsigned long sbHeight=32;

            window->statusbar_height=sbHeight;

            unsigned int sbColor = COLOR_STATUSBAR4;
            window->statusbar_color = (unsigned int) sbColor;

            // ??
            // Se tem uma parent válida?
            // Porque depende da parent?
            //if ( (void*) window->parent != NULL )
            if ((void*) window != NULL)
            {
                // Relative to the app window.
                sbTop = 
                (unsigned long) (window->rcClient.height - window->statusbar_height);
                // #bugbug
                // We're gonna fail if we use
                // the whole width 'window->width'.
                // Clipping?
                sbWidth = 
                (unsigned long) (window->width - 4);
            }

            // Estamos relativos à nossa área de cliente
            // Seja ela do tipo que for.
            // #todo: apos criarmos a janela de status no fim da
            // area de cliente, então precisamos redimensionar a
            // nossa área de cliente.
            
            // #debug
            //printf ("l=%d t=%d w=%d h=%d\n",
            //    sbLeft, sbTop, sbWidth, sbHeight );
            //while(1){}
            
            sbWindow = 
                (void *) doCreateWindow ( 
                             WT_SIMPLE, 
                             0, // Style 
                             1, 
                             1, 
                             "Statusbar", 
                             sbLeft, sbTop, sbWidth, sbHeight,
                             (struct gws_window_d *) window, 
                             0, 
                             window->statusbar_color,  //frame
                             window->statusbar_color,  //client
                             (unsigned long) window->rop );   // rop_flags  
            
            // Depois de pintarmos a status bar, caso o estilo exija,
            // então devemos atualizar a altura da área de cliente.
            window->rcClient.height -= window->statusbar_height;

            if ((void *) sbWindow == NULL){
                //server_debug_print ("doCreateWindowFrame: sbWindow fail \n");
                goto fail;
            }
            sbWindow->type = WT_SIMPLE;
            sbWindow->isStatusBar = TRUE;
            window->statusbar = (struct gws_window_d *) sbWindow;  // Window pointer.
            // Register window
            id = (int) RegisterWindow(sbWindow);
            if (id<0){
                //server_debug_print ("doCreateWindowFrame: Couldn't register window\n");
                goto fail;
            }
            // Add it to the list of childs
            wm_add_child_window(parent,sbWindow);
        }

        return 0;  // OK
    }

// ===============================================
// Icon

    if (Type == WT_ICON)
    {
        window->borderUsed = TRUE;
        __draw_window_border(parent,window);
        //printf("border\n"); while(1){}
        return 0;
    }

    return 0;
fail:
    return (int) (-1);
}

// doCreateWindow:
// Create a new window object and set up its position, size, and frame.
//
// IN:
//   type   - Window type (WT_OVERLAPPED, WT_BUTTON, etc.)
//   style  - Window style 
//   status
//   state
//   title
//   x,y    - Position relative to parent (local coordinates)
//   w,h    - Dimensions (width, height)
//   pWindow - Pointer to parent window (NULL for root)
//   desktop id
//   frame color
//   client area color
//   ROP Raster operations 
// OUT:
//   Returns pointer to new window structure or NULL on failure.
void *doCreateWindow ( 
    unsigned long type, 
    unsigned long style,
    unsigned long status,
    unsigned long state,
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
    register struct gws_window_d *window;
    struct gws_window_d *Parent;

    int fChild = FALSE;

// Colors
    unsigned int FrameColor;
    unsigned int ClientAreaColor;
    if (type == WT_OVERLAPPED){
        FrameColor = (unsigned int) get_color(csiWindow);
        ClientAreaColor = (unsigned int) get_color(csiWindow);
    }else{
        FrameColor = (unsigned int) frame_color;
        ClientAreaColor = (unsigned int) client_color;
    };


    int isDarkTheme = FALSE;
    if ((void *) GWSCurrentColorScheme != NULL)
    {
        if (GWSCurrentColorScheme->magic == 1234)
        {
            if (GWSCurrentColorScheme->is_dark == TRUE)
                isDarkTheme = TRUE;
        }
    }

//
// Internal flags
//

// #todo:
// Receberemos isso via parametro de função.
// Default is FALSE.
// We need to know the parent's bg color.

// Styles (see: windows.h)
    int Maximized=FALSE;
    int Minimized=FALSE;
    int Fullscreen=FALSE;
    // Status bar?
    // locked?
    // clip in client area?
    // ...
    int TitleBar = FALSE;
    // ...
    int Transparent = FALSE;
    // is child?
    //
    int MinimizeButton = FALSE;  // Controls
    int MaximizeButton = FALSE;
    int CloseButton    = FALSE;
    int Shadow        = FALSE;
    int Background    = FALSE;
    int Border        = FALSE;  // Usado no edit box, na overlapped.
    int ClientArea    = FALSE;
    int ButtonDown    = FALSE;  // ??
    int ButtonUp      = FALSE;  // ??
    int ButtonSysMenu = FALSE;  // system menu na barra de títulos.
    // ...


// Desktop support
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
    unsigned long deviceLeft = 0;
    unsigned long deviceTop = 0;
    unsigned long deviceWidth  = (__device_width  & 0xFFFF);
    unsigned long deviceHeight = (__device_height & 0xFFFF);

// Position and dimension.
// Passado via argumento.
// left, top, width, height.
    unsigned long WindowLeft   = (unsigned long) (x & 0xFFFF);
    unsigned long WindowTop    = (unsigned long) (y & 0xFFFF);
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

// ROP (Raster Operations)
// 0 means that there is no ROP. 
// No blending will be applied.
// #bugbug: Only 0 is considered solid?
// No rop flags for now. It depends on the window style.
    unsigned long __rop_flags=0;
    int is_solid = TRUE;
    if (rop_flags != 0){
        is_solid = FALSE;
    }

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

// Maximized: The window occupies the whole desktop working area.
    if (state == WINDOW_STATE_MAXIMIZED)
    {
        Maximized=TRUE;
    }
// Minimized: (Iconic)
    if (state == WINDOW_STATE_MINIMIZED)
    {
        Minimized=TRUE;
    }
// Fullscreen: The client are occupies the whole screen.
    if (state == WINDOW_STATE_FULL){
        Fullscreen=TRUE;
    }

    // ...

    if (style & WS_CHILD)
        fChild = TRUE;

    if (style & WS_TRANSPARENT)
    {
        Transparent=TRUE;
        // Get the given flags
        __rop_flags = rop_flags;
        //#test #hack
        if (__rop_flags == 0)
            __rop_flags = 20;  //gray
    }
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

	//if (FlagChild == 1){
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

// Allocate and initialize base window object.
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

    window->type = (unsigned long) type;

// Style: design-time identity.
// Defines window type and decorations/features.
    window->style = (unsigned long) style;

    // Identity checks
    if (style & WS_MENU)
        window->isMenu = TRUE;

    if (style & WS_MENUITEM)
       window->isMenuItem = TRUE;

    //if (style & (WS_DESKTOPICON | WS_BARICON | WS_TRAYICON | WS_BUTTONICON))
       //window->isIcon = TRUE;


// State: runtime condition.
// Tracks current behavior (minimized, maximized, fullscreen, etc).
    window->state = (int) state;

// Status: interaction/activation.
// Indicates focus, active/inactive, and user engagement.
    window->status = (int) (status & 0xFFFFFFFF);

    // ??
    // The 'window status' is used as 'button state'
    // Status (active or inactive)
    // Status do botao e da janela. (int)
    int ButtonState = (int) (status & 0xFFFFFFFF);

// Colors:
// Background, client-area bg, bg when mouse hover.
    window->bg_color = 
        (unsigned int) FrameColor;
    window->clientarea_bg_color = 
        (unsigned int) ClientAreaColor;

// buffers
    window->depth_buf = NULL;


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

// == Border =============================
// Initializing border info for the first time.
    window->Border.border_color1 = HONEY_COLOR_BORDER_LIGHT_NOFOCUS;
    window->Border.border_color2 = HONEY_COLOR_BORDER_DARK_NOFOCUS;
    window->Border.border_size = METRICS_BORDER_SIZE;
    window->Border.border_style = 0;

// Is it used or not?
    window->borderUsed = FALSE;
    // ## na verdade isse será trabalhado logo abaixo.
    if (window->type == WT_OVERLAPPED){
        window->borderUsed = TRUE;
    }
    unsigned long __BorderSize = window->Border.border_size;

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
    window->mpp.bg_color = 
        (unsigned int) (COLOR_BEIGE + rand());

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
// Se for uma janela filha o posicionamento absoluto 
// deve ser somado às margens da área de cliente da janela 
// que será a janela mãe.
// #bugbug #todo 
// Esses valores de dimensões recebidos via argumento na verdade 
// devem ser os valores para a janela, sem contar o frame, que 
// inclui as bordas e a barra de títulos.

// -------------------------------
// Position and dimension handling
// -------------------------------

// Relative position inside parent.
// These are local coordinates: 
// (0,0) means top-left of parent’s client area in WT_OVERLAPPED.
    window->left   = (unsigned long) (WindowLeft   & 0xFFFF);
    window->top    = (unsigned long) (WindowTop    & 0xFFFF);
// Dimensions of the window itself.
    window->width  = (unsigned long) (WindowWidth  & 0xFFFF);
    window->height = (unsigned long) (WindowHeight & 0xFFFF);

// width in chars
// #todo: We need a variable for char width.
    window->width_in_chars  = 
        (unsigned long) (window->width / 8);   //>>3

// Height in chars
    window->height_in_chars = 
        (unsigned long) (window->height / 8);  //>>3

// =================================

//
// 
//

// #bugbug: Is this the same of absolute values?

// >>> Absolute values <<<
// The compositor needs this information
// #todo: Inside dimensions clipped by parent.
// Initial configuration for the window rectangle.
// Relative values. (l,t,w,h)
    window->rcWindow.left   = (unsigned long) 0; //#todo: absolute
    window->rcWindow.top    = (unsigned long) 0; //#todo: absolute
    window->rcWindow.width  = (unsigned long) WindowWidth;
    window->rcWindow.height = (unsigned long) WindowHeight;

// =================================

// Local pad variables for client area calculation
    unsigned int pad_left   = METRICS_CLIENTAREA_LEFTPAD;
    unsigned int pad_top    = METRICS_CLIENTAREA_TOPPAD;
    unsigned int pad_right  = METRICS_CLIENTAREA_RIGHTPAD;
    unsigned int pad_bottom = METRICS_CLIENTAREA_BOTTOMPAD;


//
// Client area rectangle
//

// #todo:
// We need a variable for border size in the structure.
// #todo:
// We need a variable for title bar height.

// Left margin and top margin.
// The top margin changes if we have a bar.

// >> Relative values <<

// Left
    clientRect.left = (unsigned long) (__BorderSize + pad_left);

// Top
    //clientRect.top  = (unsigned long) 0;
    if (window->type == WT_OVERLAPPED){
        clientRect.top  = (unsigned long) (__TBHeight + (__BorderSize + pad_top));
    } else {
        clientRect.top  = (unsigned long) (__BorderSize + pad_top);
        //or
        //clientRect.top  = (unsigned long) 0;
    }

// Width and height.
// width
// menos bordas laterais

    clientRect.width = 
        (unsigned long) ( window->width - (__BorderSize * 2) - 
        (pad_left + pad_right) );

// height
// menos bordas superior e inferior
    // menos a barra de tarefas.

    clientRect.height = 
        (unsigned long) ( window->height - (__BorderSize * 2) - 
        (pad_top + pad_bottom) - __TBHeight ); 

// If we have scrollbars.
// #todo: Diminuimos as dimensões se o style
// indicar que temos scrollbars.
    //if (window->style & WS_VSCROLLBAR)
    //    clientRect.width -= 24;
    //if (window->style & WS_HSCROLLBAR)
    //    clientRect.height -=24;
    //if (window->style & WS_STATUSBAR)
    //    clientRect.height -=24;


// Saving client rectangle into the window structure.
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
    //window->left = WindowLeft;
    //window->top  = WindowTop;

// If we have a parent window.
// parent + arguments
// Sempre é relativo à janela mãe.
// Se a janela mãe é overlapped,
// então também é relativo à janela de cliente.
// Pois é o lugar padrão para criar janelas cliente.
// Isso só não será válido, se uma flag especial 
// permitir criar uma janela fora da área de cliente.
// >> Not clipped by parent.

//
// -- Absolute position (left and top) --
//

// Set up absolute position where there is no parent.
// Root window: position is already absolute.
    if ((void*) window->parent == NULL)
    {
        window->absolute_x = WindowLeft;
        window->absolute_y = WindowTop;
    }

// Absolute position on screen.
// If parent exists, add parent’s absolute position to local offset.
// >> Not clipped by parent.
    if ((void*) window->parent != NULL)
    {
        // Set up absolute position when the parent is NOT WT_OVERLAPPED
        if (window->parent->type != WT_OVERLAPPED)
        {
            window->absolute_x = 
                (unsigned long) (window->parent->absolute_x + WindowLeft);
            window->absolute_y = 
                (unsigned long) (window->parent->absolute_y + WindowTop);
        }
        
        // Set up absolute position when the parent is WT_OVERLAPPED
        if (window->parent->type == WT_OVERLAPPED)
        {
            // Dentro da área de cliente.
            window->absolute_x = 
                (unsigned long) ( window->parent->absolute_x + 
                window->parent->rcClient.left + 
                WindowLeft );
            window->absolute_y = 
                (unsigned long) ( window->parent->absolute_y +
                window->parent->rcClient.top + 
                WindowTop );

            // Fora da área de cliente.
            if (window->type == WT_TITLEBAR)
            {
                window->absolute_x = 
                    (unsigned long) (window->parent->absolute_x + WindowLeft);
                window->absolute_y = 
                    (unsigned long) (window->parent->absolute_y + WindowTop);
            }
            // ?
        }
    }

// Absolute bottom/right coordinates for clipping and redraw.
// >> Not clipped by parent.
    window->absolute_right = 
        (unsigned long) (window->absolute_x + window->width);
    window->absolute_bottom = 
        (unsigned long) (window->absolute_y + window->height);

// Maximized. OK
// Fit to the desktop working area.
    if (Maximized == TRUE)
    {
        if (WindowManager.initialized == TRUE)
        {
            // Width
            window->width  = WindowManager.wa.width;
            window->height = WindowManager.wa.height;
            //window->rcWindow.width
            //window->rcWindow.height

            // Absolute left/top
            window->absolute_x = WindowManager.wa.left;
            window->absolute_y = WindowManager.wa.top;

            // Absolute right/bottom
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
        // Relative width/height
        window->width = fullWindowWidth;
        window->height = fullWindowHeight;

        // Ansolute left/top
        window->absolute_x = fullWindowX;
        window->absolute_y = fullWindowY;
        // Absolute right/bottom
        window->absolute_right = 
            (unsigned long) (window->absolute_x + window->width);
        window->absolute_bottom = 
            (unsigned long) (window->absolute_y + window->height); 

        // Save fullscreen values
        window->full_left   = window->absolute_x;
        window->full_top    = window->absolute_y;
        window->full_width  = window->width;
        window->full_height = window->height;
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
    window->Border.border_style     = 0;
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

        // #important: Set the taskbar created by the user.
        if (window->style & WS_TASKBAR)
        {
            window->isTaskBar = TRUE;
            taskbar_window = window;
        }
        window->ip_device = IP_DEVICE_NULL;
        window->frame.used = FALSE;
        Background = TRUE;
        window->backgroundUsed = TRUE;
        window->background_style = 0;
        break;

    // Edit box. (Simples + borda preta).
    // Editbox não tem sombra, tem bordas.
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

    // #todo
    if (Maximized == 1)
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

    // #todo: Use color scheme
    if (Shadow == TRUE)
    {
        window->shadowUsed = TRUE;

        // Overlapped window
        if ((unsigned long) type == WT_OVERLAPPED)
        {
            if (window == keyboard_owner){
                __tmp_color = xCOLOR_GRAY1;
                //__tmp_color = (unsigned int) get_color(csiActiveWindowBorder);
            } else if (window != keyboard_owner){
                __tmp_color = xCOLOR_GRAY2;
                //__tmp_color = (unsigned int) get_color(csiInactiveWindowBorder);
            }
            // Saving shadow color value
            window->shadow_color = (unsigned int) __tmp_color;
    
            // Draw rectangle for the shadow for WT_OVERLAPPED.
            // #todo: What is the width for the shadow? 
            // Create variable for this additional area.
            painterFillWindowRectangle( 
                (window->absolute_x +1), 
                (window->absolute_y +1), 
                (window->width  +1 +1),   // #todo 
                (window->height +1 +1),   // #todo 
                __tmp_color, 
                __rop_flags );
        }
        
        // #todo
        // Draw shadow for the other types.
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

        // Select background color for the given type
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
            // #todo: We need a default color for this
            default:
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
        switch (ButtonState){

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
        
        size_t tmp_size = (size_t) strlen((const char *) window->name);
        // #bugbug: The max size also needs to respect 
        // the size of the button's window.
        if (tmp_size > 64){
            tmp_size=64;
        }

        // The label position (left/top)
        // It goes in the center
        unsigned long l_offset = 
            ( ( (unsigned long) window->width - ( (unsigned long) tmp_size * (unsigned long) FontInitialization.width) ) >> 1 );
        unsigned long t_offset = 
            ( ( (unsigned long) window->height - FontInitialization.height ) >> 1 );

        // #bugbug: The button does not have a parent
        if ((void*) Parent == NULL){
            // #bugbug
        }

        // Paint the button only if it has a parent window
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

            window->label_color_when_not_selected = (unsigned int) get_color(csiSystemFontColor);
            if (isDarkTheme == TRUE){
                window->label_color_when_selected = COLOR_WHITE;
            } else {
                window->label_color_when_selected = xCOLOR_GRAY1;
            }


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

//
// More ...
//

// #todo
// Do we have more types to draw here?
// Checkbox, radio box, etc.

// Invalidate window
// #todo: Only if it's not minimized
    window->dirty = TRUE;

// Return the pointer
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
    unsigned long style,   // style of window
    unsigned long status,  // #test Status do botao, e da janela. 
    unsigned long state,   // view: min, max ... 
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

   int fChild = FALSE;

   unsigned long __rop_flags=0;
// This function is able to create some few 
// kinds of windows for now:
// overlapped, editbox, button and simple.
    int ValidType=FALSE;
    size_t text_size = 0;

    //server_debug_print ("CreateWindow:\n");

// The color for the frame and client area.
// If the window is overlapped or button
// we use the color scheme. Otherwise
// we use the colors given by the caller.
    unsigned int FrameColor;
    unsigned int ClientAreaColor;
    if (type == WT_OVERLAPPED){
        FrameColor = (unsigned int) get_color(csiWindowBackground);
        ClientAreaColor = (unsigned int) get_color(csiWindow);
    } else if (type == WT_BUTTON) {
        FrameColor = (unsigned int) get_color(csiButton);
        ClientAreaColor = (unsigned int) get_color(csiWindow);
    // Given by the caller.
    } else {
        FrameColor = (unsigned int) frame_color;
        ClientAreaColor = (unsigned int) client_color;
    };

// Is it a child window?
    if (style & WS_CHILD)
        fChild = TRUE;

/*
// #hack No full screen allowd for now.
    if (state == WINDOW_STATE_FULL)
    {
        printf("server: Invalid WINDOW_STATE_FULL\n");
        exit(0);
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
// See: wt.h
    switch (type){
    case WT_SIMPLE:  // 1
    case WT_EDITBOX_SINGLE_LINE:  // 2 
    case WT_OVERLAPPED:  // 3
    case WT_CHECKBOX:  // 5
    case WT_SCROLLBAR:  // 6
    case WT_EDITBOX_MULTIPLE_LINES:  // 7
    case WT_BUTTON:  // 8
    case WT_ICON:  // 10
    // ...
        ValidType=TRUE;
        break;
    };

    // The application can't create a titlebar alone.
    // It is only for internal use.
    if (type == WT_TITLEBAR){
        goto fail;
    }
    // The application can't create a statusbar alone.
    // It is only for internal use.
    if (type == WT_STATUSBAR){
        goto fail;
    }
// Is it a valid type?
    if (ValidType != TRUE){
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

// ============================
// Types with frame.

// ====
// 1 - Simple
    if (type == WT_SIMPLE)
    {
        //server_debug_print ("CreateWindow: WT_SIMPLE \n");

        __w = 
            (void *) doCreateWindow ( 
                        WT_SIMPLE, style, status, state, (char *) _name,
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

// ====
// 3 - Overlapped
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
                        WT_SIMPLE, style, status, state, (char *) _name,
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
        __w->type = WT_OVERLAPPED;
        //__w->locked = FALSE;
        __w->enabled = TRUE;

        wm_add_window_to_top(__w);
        set_active_window(__w);
        goto draw_frame;
    }

// #todo
// It does not exist by itself. It needs a parent window.

// ====
// 2 and 7 - edit box
// Podemos usar o esquema padrão de cores ...
    if ( type == WT_EDITBOX_SINGLE_LINE || 
         type == WT_EDITBOX_MULTIPLE_LINES )
    {
        //server_debug_print ("CreateWindow: WT_EDITBOX WT_EDITBOX_MULTIPLE_LINES \n");

        //if ( (void*) pWindow == NULL ){ return NULL; }

        if (width < EDITBOX_MIN_WIDTH)  { width=EDITBOX_MIN_WIDTH; }
        if (height < EDITBOX_MIN_HEIGHT){ height=EDITBOX_MIN_HEIGHT; }

        __w = 
            (void *) doCreateWindow ( 
                        WT_SIMPLE, style, status, state, (char *) _name, 
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
// 8 - button
// Podemos usar o esquema padrão de cores ...
    if (type == WT_BUTTON)
    {
        //server_debug_print ("CreateWindow: WT_BUTTON \n");
      
        //if ( (void*) pWindow == NULL ){ return NULL; }

        if (width < BUTTON_MIN_WIDTH)  { width=BUTTON_MIN_WIDTH; }
        if (height < BUTTON_MIN_HEIGHT){ height=BUTTON_MIN_HEIGHT; }

        __w = 
            (void *) doCreateWindow ( 
                        WT_BUTTON, 
                        style,
                        status,  // #bugbug status (Button state)
                        state,
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

// ---------------------
// 10 - Icon 

    if (type == WT_ICON)
    {
        //server_debug_print ("CreateWindow: WT_ICON\n");

        __w = 
            (void *) doCreateWindow ( 
                        WT_SIMPLE, style, status, state, (char *) _name,
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
// #todo:
// Let's create other types here.

    // 5 - Check box
    if (type == WT_CHECKBOX)
    {
        // #todo
        // Call the worker.
        goto fail;
    }

    // 6 - Scroll bar
    // Yes, the application will be able 
    // to create scroll bars, given the parent window.
    if (type == WT_SCROLLBAR)
    {
        // #todo
        // Call the worker.
        // Check if we have a valid parent window.
        goto fail;
    }
    // ...

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

// Border size
    unsigned long BorderSize = METRICS_BORDER_SIZE;

// Border colors.
    unsigned int bc_1 = COLOR_BORDER2;
    unsigned int bc_2 = COLOR_BORDER2;
    unsigned int bc_3 = COLOR_BORDER2;

// Ornament colors
    unsigned int oc_1 = COLOR_ORNAMENT_FG;
    unsigned int oc_2 = COLOR_ORNAMENT_FG;

// Frame style
    int FrameStyle = 1;

// ----

/*
// Border colors for all the types depending on the focus.
    if (__w == keyboard_owner){
        bc_1 = (unsigned int) get_color(csiWWFBorder);
        bc_2 = (unsigned int) get_color(csiWWFBorder);
        //bc_3 = (unsigned int) get_color(csiWWFBorder);
    } else if (__w == active_window) { 
        // Active top-level window 
        bc_1 = get_color(csiActiveWindowBorder); 
        bc_2 = get_color(csiActiveWindowBorder);
    } else if (__w != keyboard_owner){
        bc_1 = (unsigned int) get_color(csiWindowBorder);
        bc_2 = (unsigned int) get_color(csiWindowBorder);
        //bc_3 = (unsigned int) get_color(csiWWFBorder);
    };
*/

    int isKeyboardOwner = FALSE;   // wwf
    int isActiveWindow = FALSE;    // active

    if (__w == keyboard_owner)
        isKeyboardOwner = TRUE;
    if (__w == active_window)
        isActiveWindow = TRUE;


// == Normal windows =================================
// Border Color 1 = top/left      (Light)
// Border Color 2 = right/bottom  (Dark)

    if (isActiveWindow == TRUE){
        bc_1 = HONEY_COLOR_BORDER_LIGHT_ACTIVE;  //get_color(csiActiveWindowBorder); 
        bc_2 = HONEY_COLOR_BORDER_DARK_ACTIVE;   //get_color(csiActiveWindowBorder);
    } else {
        bc_1 = HONEY_COLOR_BORDER_LIGHT_INACTIVE;  //get_color(csiActiveWindowBorder); 
        bc_2 = HONEY_COLOR_BORDER_DARK_INACTIVE;   //get_color(csiActiveWindowBorder);
        // Its a wwf
        if (isKeyboardOwner == TRUE){
            bc_1 = (unsigned int) HONEY_COLOR_BORDER_LIGHT_WWF;  //get_color(csiWWFBorder);
            bc_2 = (unsigned int) HONEY_COLOR_BORDER_DARK_WWF; //get_color(csiWWFBorder);
        } else {
            bc_1 = (unsigned int) HONEY_COLOR_BORDER_LIGHT_NOFOCUS;  //get_color(csiWindowBorder);
            bc_2 = (unsigned int) HONEY_COLOR_BORDER_DARK_NOFOCUS;  //get_color(csiWindowBorder);
        }
    }

// == Editbox windows =================================
// Border Color 1 = top/left      (Dark)
// Border Color 2 = right/bottom  (Light)

    if (type == WT_EDITBOX_SINGLE_LINE || type == WT_EDITBOX_MULTIPLE_LINES)
    {
        // focus
        // Border Color 1 = top/left      (Dark)
        // Border Color 2 = right/bottom  (Light)
        if (isKeyboardOwner == TRUE){
            bc_1 = (unsigned int) HONEY_COLOR_BORDER_DARK_WWF; //get_color(csiWWFBorder);
            bc_2 = (unsigned int) HONEY_COLOR_BORDER_LIGHT_WWF;  //get_color(csiWWFBorder);
        // no focus
        } else {
            bc_1 = (unsigned int) HONEY_COLOR_BORDER_DARK_NOFOCUS;  //get_color(csiWindowBorder);
            bc_2 = (unsigned int) HONEY_COLOR_BORDER_LIGHT_NOFOCUS;  //get_color(csiWindowBorder);
        }
    }


// Ornament color for Overlapped window
    if (type == WT_OVERLAPPED)
    {
        if (__w->status == WINDOW_STATUS_ACTIVE)
        {
            oc_1 = COLOR_ORNAMENT_FG;
            oc_2 = COLOR_ORNAMENT_FG;
        } else {
            oc_1 = COLOR_ORNAMENT_BG;
            oc_2 = COLOR_ORNAMENT_BG;
        }
    }

// IN:
// pwindow, 
// window, 
// border size,
// border color 1, border color 2, border color 3,
// ornament color 1, ornament color 2, ornament color 3,
// style

    if ( type == WT_OVERLAPPED || 
         type == WT_EDITBOX_SINGLE_LINE || 
         type == WT_EDITBOX_MULTIPLE_LINES || 
         type == WT_BUTTON ||
         type == WT_ICON )
    {
        if ((void*) __w != NULL)
        {
            doCreateWindowFrame ( 
                (struct gws_window_d *) pWindow,
                (struct gws_window_d *) __w, 
                BorderSize,
                (unsigned int) bc_1, (unsigned int) bc_2, (unsigned int) bc_3,
                (unsigned int) oc_1, (unsigned int) oc_2,
                FrameStyle );
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
            keyboard_owner = taskbar_window;
        if (window == mouse_owner)
            mouse_owner = taskbar_window;

        set_focus(taskbar_window);
        __set_foreground_tid(taskbar_window->client_tid);
        */

        // #test
        // Send it to the kernel.
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
        keyboard_owner = taskbar_window;
    if (window == mouse_owner)
        mouse_owner = taskbar_window;

    set_focus(taskbar_window);
    __set_foreground_tid(taskbar_window->client_tid);
    */

    // #test
    // Send it to the kernel.
    // This thread will not be the foreground thread anymore.
    sc82(10012, window->client_tid, 0, 0);

    window->magic = 0;
    window->used = FALSE;
    window = NULL;

    if (fRebuildList == TRUE){
        wm_rebuild_list();
    }
    // IN: tile, show
    if (fUpdateDesktop == TRUE){
        wm_update_desktop(TRUE,TRUE);
    }
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
    if (window == taskbar_window)
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

    gws_resize_window( window, (w), (h));
    gwssrv_change_window_position( window, (l), (t) );

    //gws_resize_window( window, (w -4), (h -4));
    //gwssrv_change_window_position( window, (l +2), (t +2) );

/*
    window->width  = (w -4);
    window->height = (h -4);
*/

    if (window->type == WT_OVERLAPPED)
    {
        window->rcClient.width  = 
            window->width  - (window->Border.border_size * 2);
        window->rcClient.height = 
            window->height - (window->titlebar_height + window->Border.border_size * 2);
    }

//
// Redraw and display some windows:
// Root window, taskbar and the maximized window.
//

// Root
    redraw_window(__root_window,TRUE);

// Taskbar
// Send message to the app to repaint all the childs.
    redraw_window(taskbar_window,TRUE);
    window_post_message( taskbar_window->id, GWS_Paint, 0, 0 );

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

