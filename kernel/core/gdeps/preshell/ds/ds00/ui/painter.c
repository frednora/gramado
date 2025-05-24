// painter.c
// Painter.
// Drawing the graphical components.
// Created by Fred Nora.

#include "../gwsint.h"

static void __draw_button_mark_by_wid( int wid, int button_number );
//===================================================================

// Paint the rectangle.
int 
painterFillWindowRectangle( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags )
{
    rectBackbufferDrawRectangle ( 
        x, y, width, height, 
        color, 
        TRUE,  // Fill or not? 
        rop_flags );

    return 0; 
}

// Clear the window
// Repaint it using the default background color.
// Only valid for WT_SIMPLE.
// #todo
// A transparent window inherits its parent's background 
// for this operation.
int clear_window_by_id(int wid, unsigned long flags)
{
    struct gws_window_d *w;

// wid
    if (wid<0 || wid>=WINDOW_COUNT_MAX){
        goto fail;
    }
// Structure validation
    w = (void*) windowList[wid];
    if ((void*) w == NULL){
        goto fail;
    }
    if (w->magic != 1234){
        goto fail;
    }

// #todo
// Maybe we can clear more types of window.
    if (w->type != WT_SIMPLE){
        goto fail;
    }

    redraw_window(w,flags);
    return 0;

fail:
    return (int) -1;
}

// pinta um retangulo no botao
// indicando que o app esta rodando.
static void __draw_button_mark_by_wid( int wid, int button_number )
{
    struct gws_window_d *w;
    
//#todo: max limit
    if (wid<0){
        return;
    }
    if ( button_number<0 || button_number>3 ){
        return;
    }
// Window
    w = (struct  gws_window_d *) windowList[wid];
    if ((void*) w == NULL){
        return;
    }
    if (w->magic != 1234){
        return;
    }

    // IN: l, t, w, h, color, fill or not, rop flags.
    rectBackbufferDrawRectangle ( 
        (w->absolute_x +3), 
        (w->absolute_y +4), 
        (w->width -8), 
        2, 
        COLOR_RED, TRUE, 0 );
}

// WORKER
// Paint button borders.
// Called by doCreateWindow
// >>> No checks
// IN: window, color, color, color, color.
// color1: (left, top)
// color2-dark: (right, bottom)
// color2-light
// outer_color
// #check
// This routine is calling the kernel to paint the rectangle.
void 
__draw_button_borders(
    struct gws_window_d *w,
    unsigned int color1,
    unsigned int color2_dark,
    unsigned int color2_light,
    unsigned int outer_color )
{
// #test
// Size in pixels de apenas 1/3 de todo o size.
    unsigned long BorderSize = 1;
// Isso deve ser o total.
    //window->border_size = ?

    //debug_print("__draw_button_borders:\n");

// This is the window for relative positions.
    if ((void*) w == NULL){
        return;
    }
    if (w->magic != 1234){
        return;
    }

// Order:
// top/left ... right/bottom.


//  ____
// |
//
// board1, borda de cima e esquerda.
// Cores, de fora pra dentro:
// outer_color, color1, color1.

// -------------------------------
// :: Top
// top, top+1, top+2
    rectBackbufferDrawRectangle ( 
        w->absolute_x+1, 
        w->absolute_y, 
        w->width-2,
        BorderSize, 
        outer_color, TRUE,0 );
    rectBackbufferDrawRectangle ( 
        w->absolute_x+1, 
        w->absolute_y+1, 
        w->width-2, 
        BorderSize, 
        color1, TRUE,0 );
    rectBackbufferDrawRectangle ( 
        w->absolute_x+1+1, 
        w->absolute_y+1+1,
        w->width-4, 
        BorderSize, 
        color1, TRUE,0 );

// -------------------------------
// :: Left
// left, left+1, left+2
    rectBackbufferDrawRectangle ( 
        w->absolute_x, 
        w->absolute_y+1, 
        BorderSize, 
        w->height-2,
        outer_color, TRUE,0 );
    rectBackbufferDrawRectangle ( 
        w->absolute_x+1, 
        w->absolute_y+1, 
        BorderSize, 
        w->height-2,
        color1, TRUE,0 );
    rectBackbufferDrawRectangle ( 
        w->absolute_x+1+1, 
        w->absolute_y+1+1, 
        BorderSize, 
        w->height-4,
        color1, TRUE,0 );


//  
//  ____|
//
// board2, borda direita e baixo.
// Cores, de fora pra dentro:
// outer_color, color2, color2_light.

// -------------------------------
// :: Right
// right-3, right-2, right-1
    rectBackbufferDrawRectangle ( 
        ((w->absolute_x) + (w->width) -1), 
        w->absolute_y+1, 
        BorderSize, 
        w->height-2, 
        outer_color, TRUE, 0 );
    rectBackbufferDrawRectangle ( 
        ((w->absolute_x) + (w->width) -2), 
        w->absolute_y+1, 
        BorderSize, 
        w->height-2, 
        color2_dark, TRUE, 0 );
    rectBackbufferDrawRectangle ( 
        ((w->absolute_x) + (w->width) -3), 
        w->absolute_y+1+1, 
        BorderSize, 
        w->height-4, 
        color2_light, TRUE, 0 );

// -------------------------------
// :: Bottom
// bottom-1, bottom-2, bottom-3
    rectBackbufferDrawRectangle ( 
        w->absolute_x+1, 
        ((w->absolute_y) + (w->height) -1),  
        w->width-2, 
        BorderSize, 
        outer_color, TRUE, 0 );
    rectBackbufferDrawRectangle ( 
        w->absolute_x+1, 
        ((w->absolute_y) + (w->height) -2),  
        w->width-2, 
        BorderSize, 
        color2_dark, TRUE, 0 );
    rectBackbufferDrawRectangle ( 
        w->absolute_x+1+1, 
        ((w->absolute_y) + (w->height) -3),  
        w->width-4, 
        BorderSize, 
        color2_light, TRUE, 0 );
}

// worker:
// Draw the border of edit box and overlapped windows.
// >> no checks
// #check
// This routine is calling the kernel to paint the rectangle.
void 
__draw_window_border( 
    struct gws_window_d *parent, 
    struct gws_window_d *window )
{
// + We dont have a border size yet.

    if ((void*) parent == NULL){
        return;
    }
    if ((void*) window == NULL){
        return;
    }

// #todo
// Is it possitle to test the validation here?

// --------------------
// + We dont have a border size yet.
// Define border size.
    if (window == active_window){
        window->border_size = 2;
    }else if (window != active_window){
        window->border_size = 1;
    };

// ----------
// Overlapped
    if (window->type == WT_OVERLAPPED)
    {
        //if (window->active == TRUE)
        //    window->border_size=2;
        //if (window->active == FALSE)
        //    window->border_size=1;

        // Absolute values.
        // Relative to the screen.

        // top
        rectBackbufferDrawRectangle( 
            parent->absolute_x + window->left, 
            parent->absolute_y + window->top, 
            window->width, 
            window->border_size, 
            window->border_color1, 
            TRUE, 
            0 );
        // left
        rectBackbufferDrawRectangle( 
            parent->absolute_x + window->left, 
            parent->absolute_y + window->top, 
            window->border_size, 
            window->height, 
            window->border_color1, 
            TRUE,
            0 );
        // right
        rectBackbufferDrawRectangle( 
            (parent->absolute_x + window->left + window->width - window->border_size), 
            (parent->absolute_y + window->top), 
            window->border_size, 
            window->height, 
            window->border_color2, 
            TRUE,
            0 );
        // bottom
        rectBackbufferDrawRectangle ( 
            (parent->absolute_x + window->left), 
            (parent->absolute_y + window->top + window->height - window->border_size), 
            window->width, 
            window->border_size, 
            window->border_color2, 
            TRUE,
            0 );
    
        // #test
        // Subtract border size.
        //window->left   += window->border_size;
        //window->top    += window->border_size;
        //window->right  -= window->border_size;
        //window->bottom -= window->border_size;
        return;
    }

// ------------------
// Editbox
    if ( window->type == WT_EDITBOX || 
         window->type == WT_EDITBOX_MULTIPLE_LINES )
    {
        // border size:
        // #todo: it can't be hardcoded,
        // We're gonna have themes.
        if (window == keyboard_owner){
            window->border_size=2;
            window->border_color1 = COLOR_BLACK;   //#test
            window->border_color2 = COLOR_BLACK;   //#test
        } else if (window != keyboard_owner){
            window->border_size=1;
            window->border_color1 = COLOR_GRAY;   //#test
            window->border_color2 = COLOR_GRAY;   //#test
        }

        // top
        rectBackbufferDrawRectangle( 
            window->absolute_x, 
            window->absolute_y, 
            window->width,         // w
            window->border_size,   // h
            window->border_color1, // color
            TRUE,                  // fill?
            0 );                   // rop
        // left
        rectBackbufferDrawRectangle( 
            window->absolute_x, 
            window->absolute_y, 
            window->border_size,    // w 
            window->height,         // h
            window->border_color1,  // color
            TRUE,                   // fill?
            0 );                    // rop
        // right
        rectBackbufferDrawRectangle( 
            (window->absolute_x + window->width - window->border_size), 
            window->absolute_y,  
            window->border_size,  // w 
            window->height,       // h
            window->border_color2,  // color
            TRUE,                   // fill
            0 );                    // rop
        // bottom
        rectBackbufferDrawRectangle ( 
            window->absolute_x, 
            (window->absolute_y + window->height - window->border_size), 
            window->width,           // w
            window->border_size,     // h
            window->border_color2,   // color
            TRUE,                    // fill
            0 );                     // rop
        
        // #test
        // Subtract border size.
        //window->left   += window->border_size;
        //window->top    += window->border_size;
        //window->right  -= window->border_size;
        //window->bottom -= window->border_size;
        return;
    }
}

// IN: Titlebar window.
int redraw_controls(struct gws_window_d *window)
{
// Paint the controls, but do not show them.

    struct gws_window_d *tb_window;
    register int wid = -1;

// Validation
    if ((void*) window == NULL){
        goto fail;
    }
    if (window->magic != 1234){
        goto fail;
    }

    tb_window = window;

 //#todo
 // Esta funcionando ...
 // mas precisamos considar quando a janela muda de tamanho.

    wid = (int) tb_window->Controls.minimize_wid;
    redraw_window_by_id(wid,FALSE);

    wid = (int) tb_window->Controls.maximize_wid;
    redraw_window_by_id(wid,FALSE);

    wid = (int) tb_window->Controls.close_wid;
    redraw_window_by_id(wid,FALSE);

    return 0;
fail:
    return (int) -1;
}

// IN: Titlebar window.
int redraw_titlebar_window(struct gws_window_d *window)
{
    struct gws_window_d *parent;
    struct gws_window_d *tb_window;
    unsigned long rop=0;

// Validation
    if ( (void*) window == NULL )
        return -1;
    if (window->magic != 1234)
        return -1;

    tb_window = (struct gws_window_d *) window;

// Parent stuff
    parent = (struct gws_window_d *) tb_window->parent;
    if ( (void*) parent == NULL )
        return -1;
    if (parent->magic != 1234)
        return -1;

    rop = (unsigned long) parent->rop;

// ------------------
// Parent is active.
    if (parent == active_window)
    {
        tb_window->bg_color = 
            (unsigned int) get_color(csiActiveWindowTitleBar);

        parent->titlebar_color = 
            (unsigned int) get_color(csiActiveWindowTitleBar);
        parent->titlebar_ornament_color = xCOLOR_BLACK;
    }

// ------------------
// Parent is NOT active.
    if (parent != active_window)
    {
        tb_window->bg_color = 
            (unsigned int) get_color(csiInactiveWindowTitleBar);

        parent->titlebar_color = 
            (unsigned int) get_color(csiInactiveWindowTitleBar);
        parent->titlebar_ornament_color = xCOLOR_GRAY2;
    }

//--------

//
// Paint
//

// bg
    rectBackbufferDrawRectangle ( 
        tb_window->absolute_x, 
        tb_window->absolute_y, 
        tb_window->width, 
        tb_window->height, 
        tb_window->bg_color, 
        TRUE,   // fill
        (unsigned long) rop );  // rop for this window

// Ornament
    rectBackbufferDrawRectangle ( 
        tb_window->absolute_x, 
        ( (tb_window->absolute_y) + (tb_window->height) - METRICS_TITLEBAR_ORNAMENT_SIZE ),  
        tb_window->width, 
        METRICS_TITLEBAR_ORNAMENT_SIZE, 
        parent->titlebar_ornament_color, 
        TRUE,  // fill
        (unsigned long) rop );  // rop_flags

// --------------------
// Icon

    int icon_id = 
        (int) parent->frame.titlebar_icon_id;

// Decode the bmp that is in a buffer
// and display it directly into the framebuffer. 
// IN: index, left, top
// see: bmp.c
    unsigned long iL=0;
    unsigned long iT=0;
    unsigned long iWidth = 16;

    //#hack #todo
    int useIcon = parent->titlebarHasIcon;
    if (useIcon == TRUE)
    {
        iL = (unsigned long) (tb_window->absolute_x + METRICS_ICON_LEFTPAD);
        iT = (unsigned long) (tb_window->absolute_y + METRICS_ICON_TOPPAD);

        bmp_decode_system_icon( 
            (int) icon_id, 
            (unsigned long) iL, 
            (unsigned long) iT,
            FALSE );
    }

// ---------------

//
// Text
//

    unsigned long sL=0;
    unsigned long sT=0;
    unsigned int sColor = 
        (unsigned int) parent->titlebar_text_color;

    
    int useTitleString = TRUE; //#HACK
    if (useTitleString == TRUE)
    {
        // margin + relative position.
        sL = 
            (unsigned long) 
            ( tb_window->absolute_x + parent->titlebar_text_left);
        sT = 
            (unsigned long) 
            ( tb_window->absolute_y + parent->titlebar_text_top);

        /*
        // #debug
        // It shows the height of the parent window. (Overlapped).
        char new_name[64];
        int __h = (int) (parent->height & 0xFFFFFFFF);
        memset(new_name,0,64);
        itoa ( (int) __h, new_name );
        size_t __s = strlen(new_name);
        new_name[__s] = 0;
        grDrawString ( sL, sT, sColor, new_name );
        */
        
        if ((void*) tb_window->name != NULL){
            grDrawString ( sL, sT, sColor, tb_window->name );
        }
    }


// Invalidate
    window->dirty = TRUE;

// IN: titlebar
    redraw_controls(window); 

    return 0;
}

// #test
// IN: Editbox window.
void redraw_text_for_editbox(struct gws_window_d *window)
{
    char *p;
    register int i=0;

// Prepare the caret string (underscore in this case)
    char caret_string[2];
    caret_string[0] = '_';    // Or use '|' or whatever symbol you prefer for caret
    caret_string[1] = 0x00;

// Choose the caret color (e.g., black)
    unsigned int caret_color = COLOR_BLACK;

    if ((void*)window == NULL)
        return;
    if (window->magic != 1234)
        return;
    if ( window->type != WT_EDITBOX_SINGLE_LINE &&
         window->type != WT_EDITBOX_MULTIPLE_LINES )
    {
        return;
    }
// No text
    if ((void*) window->window_text == NULL)
    {
        //window->textbuffer_size_in_bytes = 0;
        //window->text_size_in_bytes = 0;
        return;
    }

// Get the base
    p = window->window_text;

// Total chars
    int total_chars = window->text_size_in_bytes;
    if (total_chars <= 0)
        total_chars = 0;

    unsigned int font_width  = FontInitialization.width  ? FontInitialization.width  : 8;
    unsigned int font_height = FontInitialization.height ? FontInitialization.height : 8;

// Chars per line.
    //int chars_per_line = (window->width - 16) / font_width; // 8px margin left/right
    int chars_per_line = (int) (window->width_in_chars & 0xFFFFFFFF);
    if (chars_per_line < 1) 
        chars_per_line = 1;
    if (chars_per_line > METRICS_MAX_CHARS_PER_LINE) 
        chars_per_line = METRICS_MAX_CHARS_PER_LINE;

// Chars per column.
    //int max_lines = (window->height - 16) / font_height; // 8px margin top/bottom
    int max_lines = (int) (window->height_in_chars & 0xFFFFFFFF);
    if (max_lines < 1) 
        max_lines = 1;

// -------------------------------------------------------
// Draw the string into the window for single line
    if ( window->type == WT_EDITBOX_SINGLE_LINE )
    {
       // Only print up to chars_per_line
        char line_buffer00[chars_per_line + 1];
        memset(line_buffer00, 0, sizeof(line_buffer00));
        int to_copy = (total_chars > chars_per_line) ? chars_per_line : total_chars;
        // Copy
        memcpy(line_buffer00, p, to_copy);
        line_buffer00[to_copy] = 0;
        // Draw
        grDrawString ( 
            (window->absolute_x + METRICS_EDITBOX_MARGIN_LEFT), 
            (window->absolute_y + METRICS_EDITBOX_MARGIN_TOP), 
            COLOR_BLACK, 
            line_buffer00 );

        /*
        // #todo: Update the input pointer position in chars.
        // and draw it at the end of the line.
        window->ip_x = 0;
        window->ip_y = 0;
        // Draw the caret at the input pointer position (in chars, not pixels)
        dtextDrawText2(
            window,
            window->ip_x *8,
            window->ip_y *8,  
            caret_color,
            caret_string,
            TRUE );
        */

        return;
    }

// -------------------------------------
// Let's print multiple lines.
// Draw the string into the window for single line
    int line = 0;
    int col = 0;
    char line_buffer[chars_per_line + 1];
    memset(line_buffer, 0, sizeof(line_buffer));

    for ( i=0; 
          i < total_chars && line < max_lines; 
          i++ )
    {
        char c = p[i];  // Get

        if (c == '\n' || col == chars_per_line) 
        {
            // Print current line
            line_buffer[col] = 0;
            grDrawString(
                window->absolute_x + METRICS_EDITBOX_MARGIN_LEFT,
                window->absolute_y + METRICS_EDITBOX_MARGIN_TOP + line * font_height,
                COLOR_BLACK,
                line_buffer
            );
            line++;
            col = 0;
            memset(line_buffer, 0, sizeof(line_buffer));

            if (c == '\n')
                continue; // do not add '\n' to buffer
        }

        // Fill the buffer
        if ( line < max_lines && 
             col < chars_per_line && 
             c != '\n' ) 
        {
            line_buffer[col++] = c;
        }
    };

    // Print remaining chars if any
    if ( col > 0 && 
         line < max_lines ) 
    {
        line_buffer[col] = 0;
        grDrawString(
            window->absolute_x + METRICS_EDITBOX_MARGIN_LEFT,
            window->absolute_y + METRICS_EDITBOX_MARGIN_TOP + line * font_height,
            COLOR_BLACK,
            line_buffer
        );
    }
}

//------------
// redraw_window:
// Let's redraw the window.
// Called by serviceRedrawWindow().
// #todo
// devemos repintar as janelas filhas, caso existam.
// IN: 
// window pointer, show or not.

int 
redraw_window ( 
    struct gws_window_d *window, 
    unsigned long flags )
{

// #todo
// When redrawing an WT_OVERLAPPED window,
// we can't redraw the frame if the window is in fullscreen mode.
// In this case, we just redraw the client area.

    unsigned int __tmp_color = COLOR_WINDOW;

// Structure validation.
    if ((void *) window == NULL){
        goto fail;
    }
    if (window->used!=TRUE || window->magic!=1234){
        goto fail;
    }

// =======================
// shadowUsed
// A sombra pertence à janela e ao frame.
// A sombra é maior que a própria janela.
// ?? Se estivermos em full screen não tem sombra ??
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
// ??
// E os outros tipos, não tem sombra ??
// Os outros tipos devem ter escolha para sombra ou não ??
// Flat design pode usar sombra para definir se o botão 
// foi pressionado ou não.

// shadow: Not used for now.
    if (window->shadowUsed == TRUE)
    {
        if ((unsigned long) window->type == WT_OVERLAPPED)
        {
            if (window == keyboard_owner){
                __tmp_color = xCOLOR_GRAY1;
            } else if (window != keyboard_owner){
                __tmp_color = xCOLOR_GRAY2;
            }

            // Shadow rectangle.
            rectBackbufferDrawRectangle ( 
                (window->absolute_x +1), 
                (window->absolute_y +1), 
                (window->width +1 +1), 
                (window->height +1 +1), 
                __tmp_color, 
                TRUE,     // fill?
                (unsigned long) window->rop );  // #todo: rop operations for this window.
        }
    }

// =======================
// backgroundUsed
// ## Background ##
// Background para todo o espaço ocupado pela janela e pelo seu frame.
// O posicionamento do background depende do tipo de janela.
// Um controlador ou um editbox deve ter um posicionamento relativo
// à sua janela mãe. Já uma overlapped pode ser relativo a janela 
// gui->main ou relativo à janela mãe.

// Background rectangle.
    if (window->backgroundUsed == TRUE)
    {
        // Redraw the background rectangle.
        rectBackbufferDrawRectangle ( 
                window->absolute_x, 
                window->absolute_y, 
                window->width, 
                window->height, 
                window->bg_color, 
                TRUE,  //fill
                (unsigned long) window->rop );   // ROP for this window.

        // All done for WT_SIMPLE type.
        if (window->type == WT_SIMPLE){
            goto done;
        }
    }

//
// botao ==========================================
//

// =======================
// WT_BUTTON

    int ButtonState = BS_DEFAULT;

    //Termina de desenhar o botão, mas não é frame
    //é só o botão...
    //caso o botão tenha algum frame, será alguma borda extra.
    int Focus=0;    //(precisa de borda)
    int Selected=0;   
    unsigned int label_color = COLOR_BLACK;
    // Change to 'button_border1' and 'button_border2'
    unsigned int border1 = COLOR_GRAY;
    unsigned int border2 = COLOR_GRAY;

    if (window->type == WT_BUTTON)
    {
        ButtonState = (int) (window->status & 0xFFFFFFFF);
 
        //if ( (void*) window->parent == NULL )
            //printf("redraw_window: [FAIL] window->parent\n");

        // Atualiza algumas características da janela.
        switch (ButtonState)
        {
            case BS_FOCUS:
                border1 = COLOR_BLUE;
                border2 = COLOR_BLUE;
                break;

            //case BS_PRESS:
            case BS_PRESSED:
                Selected = 1;
                border1 = xCOLOR_GRAY1;  //GWS_COLOR_BUTTONSHADOW3;
                border2 = COLOR_WHITE;   //GWS_COLOR_BUTTONHIGHLIGHT3;
                break;

            case BS_HOVER:
                //border1 = COLOR_YELLOW;  //#test
                //border2 = COLOR_YELLOW;  //#test
                border1 = COLOR_WHITE;    //GWS_COLOR_BUTTONHIGHLIGHT3;
                border2 = xCOLOR_GRAY1;   //GWS_COLOR_BUTTONSHADOW3;
                break;

            case BS_DISABLED:
                border1 = COLOR_GRAY;
                border2 = COLOR_GRAY;
                break;

            //?
            case BS_PROGRESS:
                border1 = COLOR_GRAY;
                border2 = COLOR_GRAY;
                break;

            case BS_DEFAULT:
            default: 
                Selected = 0;
                border1 = COLOR_WHITE;    //GWS_COLOR_BUTTONHIGHLIGHT3;
                border2 = xCOLOR_GRAY1;   //GWS_COLOR_BUTTONSHADOW3;
                break;
        };

        // name support.
        size_t tmp_size = (size_t) strlen((const char *) window->name);
        // #bugbug: It also depends of the windows size.
        if (tmp_size > 64)
        {
            tmp_size=64;
        }

        // It goes in the center.
        unsigned long l_offset = 
            ( ( (unsigned long) window->width - ( (unsigned long) tmp_size * (unsigned long) FontInitialization.width) ) >> 1 );
        unsigned long t_offset = 
            ( ( (unsigned long) window->height - FontInitialization.height ) >> 1 );


        // redraw the button border.
        // #todo:
        // as cores vao depender do etado do botao.
        // #todo: veja como foi feito na hora da criaçao do botao.
        __draw_button_borders(
            (struct gws_window_d *) window,
            (unsigned int) border1,        // color1: (left and top)
            (unsigned int) border2,        // color2-dark: (right and bottom)
            (unsigned int) xCOLOR_GRAY5,   // color2-light
            (unsigned int) COLOR_BLACK );  // outter color

        // Button label
        //server_debug_print ("redraw_window: [FIXME] Button label\n"); 

        if (Selected == TRUE){
            label_color = window->label_color_when_selected;
        }
        if (Selected == FALSE){
            label_color = window->label_color_when_not_selected;
        }
        // Redraw the label's string.
        // The label is the window's name.
        grDrawString ( 
            (window->absolute_x + l_offset), 
            (window->absolute_y + t_offset), 
            label_color, window->name );

        // ok, repintamos o botao que eh um caso especial
        // nao precisamos das rotinas abaixo,
        // elas serao par aos outros tipos de janela.
        goto done;
    }

// =======================================
// redraw_frame:
// only the boards
// redraw the frame para alguns tipos menos para botao.
// O bg ja fi feito logo acima.
// Remember:
// We can't recreate the windows, just redraw'em.
// #todo
// Precisamos de uma rotina que redesenhe o frame,
// sem alocar criar objetos novos.

// ----------------------------------------
// + Redraws the title bar for WT_OVERLAPPED.
// + Redraws the borders for overlapped and editbox.

    if ( window->type == WT_OVERLAPPED || 
         window->type == WT_EDITBOX_SINGLE_LINE || 
         window->type == WT_EDITBOX_MULTIPLE_LINES )
    {
        // Invalid window
        if ((void*) window == NULL)
            goto fail;
        if (window->magic != 1234)
            goto fail;

        // Invalid parent window
        // # root has no parent, but its type is WT_SIMPLE.
        if ((void*) window->parent == NULL)
            goto fail;
        if (window->parent->magic != 1234)
            goto fail;

        // Title bar
        // Redraw titlebar for overlapped windows.
        // #todo: Not if the window is in fullscreen mode.
        if (window->type == WT_OVERLAPPED)
        {
            // Redraw the title bar and controls.
            if ((void*) window->titlebar != NULL)
            {
                if (window->titlebar->magic == 1234)
                {
                    // #warning: Not recursive. 
                    // Paint, but do not show them.
                    // It also redraw the controls.
                    redraw_titlebar_window( window->titlebar );
                }
            }
        }

        // Borders: 
        // Let's repaint the borders for some types.
        if ( window->type == WT_OVERLAPPED ||
             window->type == WT_EDITBOX_SINGLE_LINE ||
             window->type == WT_EDITBOX_MULTIPLE_LINES )
        {
            __draw_window_border(window->parent, window);
        }

        // Text
        // It's working for single line, but not for multiple lines
        if ( window->type == WT_EDITBOX_SINGLE_LINE ||
             window->type == WT_EDITBOX_MULTIPLE_LINES )
        {
            // #test
            // We're testing it yet.
            redraw_text_for_editbox(window);
        }

        //...
    }

    // ...

done:

    window->dirty = TRUE;

    // If this is a overlapped, 
    // so we need o invalidate a lot of subwindows.
    if (window->type == WT_OVERLAPPED)
    {
        window->dirty == TRUE;  // Validate
        if ( (void*) window->titlebar != NULL )
        {
            window->titlebar->dirty = TRUE;
            invalidate_window_by_id(
                window->titlebar->Controls.minimize_wid );
            invalidate_window_by_id(
                window->titlebar->Controls.maximize_wid );
            invalidate_window_by_id(
                window->titlebar->Controls.close_wid );
        }
    }


// -------------------------
// If we show the window,
// so, we need to validate it.
// Show or not.
    if (flags == TRUE)
    {
        gws_show_window_rect(window);
        window->dirty = FALSE;  // Validate
        if (window->type == WT_OVERLAPPED)
        {
            if ((void*)window->titlebar != NULL)
            {
                window->titlebar->dirty = FALSE;
                //#todo: Validate by id.
                validate_window_by_id(
                    window->titlebar->Controls.minimize_wid );
                validate_window_by_id(
                    window->titlebar->Controls.maximize_wid );
                validate_window_by_id(
                    window->titlebar->Controls.close_wid );
            }
        }
    }
    return 0;
fail:
    return -1;
}

int redraw_window_by_id(int wid, unsigned long flags)
{
    struct gws_window_d *w;

// wid
    if (wid < 0 || wid >= WINDOW_COUNT_MAX){
        goto fail;
    }
// structure validation
    w = (void*) windowList[wid];
    if ((void*) w == NULL){
        goto fail;
    }
    if (w->magic != 1234){
        goto fail;
    }
    redraw_window(w,flags);
    return 0;
fail:
    return (int) (-1);
}


void validate_window_by_id(int wid)
{
    struct gws_window_d *w;

// #todo: 
// Chamar o metodo de validação de janela.

// wid
    if (wid < 0 || wid >= WINDOW_COUNT_MAX){
        return;
    }
// Window structure
    w = (struct gws_window_d *) windowList[wid];
    if ((void*) w == NULL){
        return;
    }
    if (w->used != TRUE){
        return;
    }
    if (w->magic != 1234){
        return;
    }
    validate_window(w);    
}

// validate
void validate_window (struct gws_window_d *window)
{
    if ((void*) window != NULL)
    {
        if ( window->used == TRUE && window->magic == 1234 )
        {
            window->dirty = FALSE;
        }
    }
}


void invalidate_window_by_id(int wid)
{
    struct gws_window_d *w;

// #todo: 
// Chamar o metodo de validação de janela.

// wid
    if (wid < 0 || wid >= WINDOW_COUNT_MAX){
        return;
    }
// Window structure
    w = (struct gws_window_d *) windowList[wid];
    if ((void*) w == NULL){
        return;
    }
    if (w->used != TRUE){
        return;
    }
    if (w->magic != 1234){
        return;
    }
    invalidate_window(w);    
}

// Invalidate
void invalidate_window (struct gws_window_d *window)
{
    if ((void*) window != NULL)
    {
        if ( window->used == TRUE && window->magic == 1234 )
        {
            window->dirty = TRUE;
        }
    }
}

// Invalidate the titlebar window of a given pwindow.
void invalidate_titlebar(struct gws_window_d *pwindow)
{
    if ((void*) pwindow == NULL)
        return;
    if (pwindow->used != TRUE)
        return;
    if (pwindow->magic != 1234)
        return;
    if (pwindow->type != WT_OVERLAPPED)
        return;
    invalidate_window(pwindow->titlebar);
}

// Invalidate the menubar window of a given pwindow.
void invalidate_menubar(struct gws_window_d *pwindow)
{
    if ((void*) pwindow == NULL)
        return;
    if (pwindow->used != TRUE)
        return;
    if (pwindow->magic != 1234)
        return;
    if (pwindow->type != WT_OVERLAPPED)
        return;
    invalidate_window(pwindow->menubar);
}

// Invalidate the toolbar window of a given pwindow.
void invalidate_toolbar(struct gws_window_d *pwindow)
{
    if ((void*) pwindow == NULL)
        return;
    if (pwindow->used != TRUE)
        return;
    if (pwindow->magic != 1234)
        return;
    if (pwindow->type != WT_OVERLAPPED)
        return;
    invalidate_window(pwindow->toolbar);
}

// Invalidate the scrollbar window of a given pwindow.
void invalidate_scrollbar(struct gws_window_d *pwindow)
{
    if ((void*) pwindow == NULL)
        return;
    if (pwindow->used != TRUE)
        return;
    if (pwindow->magic != 1234)
        return;
    if (pwindow->type != WT_OVERLAPPED)
        return;
    invalidate_window(pwindow->scrollbar);
}

// Invalidate the statusbar window of a given pwindow.
void invalidate_statusbar(struct gws_window_d *pwindow)
{
    if ((void*) pwindow == NULL)
        return;
    if (pwindow->used != TRUE)
        return;
    if (pwindow->magic != 1234)
        return;
    if (pwindow->type != WT_OVERLAPPED)
        return;
    invalidate_window(pwindow->statusbar);
}

void invalidate_root_window(void)
{
    invalidate_window ((struct gws_window_d *) __root_window);
}

void invalidate_taskbar_window(void)
{
    //invalidate_window ( (struct gws_window_d *) taskbar_window );
}

// (Output port)
void wm_flush_rectangle(struct gws_rect_d *rect)
{
    if ((void*) rect == NULL){
        return;
    }
    if (rect->magic != 1234){
        return;
    }
    gwssrv_refresh_this_rect(rect);
}

void wm_flush_screen(void)
{
    gwssrv_show_backbuffer();
}

void wm_flush_window(struct gws_window_d *window)
{
    if ((void*) window == NULL){
        return;
    }
    if (window->used != TRUE){
        return;
    }
    if (window->magic != 1234){
        return;
    }
    gws_show_window_rect(window);
}

void begin_paint(struct gws_window_d *window)
{
    if ((void*) window == NULL){
        return;
    }
    validate_window(window);
}

void end_paint(struct gws_window_d *window)
{
    if ((void*) window == NULL){
        return;
    }
    invalidate_window(window);
}

