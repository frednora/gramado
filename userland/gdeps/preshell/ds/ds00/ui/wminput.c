// wminput.c
// Input support.
// Created by Fred Nora.


#include "../gwsint.h"


// ===============================================

// Mouse events.
// Quando temos um evento de mouse,
// vamos enviar esse evento para a janela.
// Os aplicativos estao recebendo
// os eventos enviados para as janelas.
// Mas os aplicativos pegam eventos apenas da 'main window'.
void 
wmProcessMouseEvent(
    int event_type, 
    unsigned long x, 
    unsigned long y )
{
// Process mouse events.

// Quando movemos o mouse, então atualizamos o ponteiro que indica
// a janela mouse_hover. Esse ponteiro será usado pelos eventos
// de botão de mouse.

// Window with focus.
    //struct gws_window_d *w;
    unsigned long saved_x = x;
    unsigned long saved_y = y;
    unsigned long in_x=0;
    unsigned long in_y=0;
    register int Tail=0;

    if (ServerState.state != SERVER_STATE_RUNNING)
        return;

    if (gUseMouse != TRUE){
        return;
    }

// Invalid event type.
    if (event_type < 0){
        return;
    }

// --------------------------------
// Move:
// Process but do not send message for now.
// #todo
// Esse eh o momento de exibirmos o cursor do mouse.
// Precisamos fazer refresh para apagar o cursor antigo
// depois pintarmos o cursor novo direto no lfb.
// Mas nao temos aqui a rotina de pintarmos direto no
// lfb.
// #todo: Trazer as rotinas de exibiçao de cursor
// para ca, depois deixar de usar
// as rotinas de pintura de cursor que estao no kernel.
    if (event_type == GWS_MouseMove)
    {
        // #test
        if (gUseMouse != TRUE)
            return;

        // If we already clicked the window
        // and now we're moving it.
        // So, now we're dragging it.
        
        // O botao esta pressionado.
        if (grab_is_active == TRUE){
            is_dragging = TRUE;
        // O botao nao esta pressionado.
        } else if (grab_is_active != TRUE){
            is_dragging = FALSE;
        };

        // Set flag to erease mouse pointer.
        // Não queremos rastro.
        DoWeNeedToEraseMousePointer(TRUE);

        // Update the global mouse position.
        // The compositor is doing its job,
        // painting the pointer in the right position.
        // Lets update the position. See: comp.c
        comp_set_mouse_position(saved_x,saved_y);

        // #test
        __display_mouse_cursor();

        // Check the window we are inside of 
        // and update the mouse_hover pointer.
        wm_hit_test_00(saved_x,saved_y);

        return;
    }

// --------------------------------
// Pressed:
// Process but do not send message for now.

    if (event_type == GWS_MousePressed)
    {
        DoubleClick.is_doubleclick = FALSE;
        DoubleClick.delta =
            (DoubleClick.current - DoubleClick.last);
        DoubleClick.last = DoubleClick.current; 
        // Yes, it's a double click
        if (DoubleClick.delta < DoubleClick.speed)
        {
            DoubleClick.is_doubleclick = TRUE;
            on_doubleclick();            
            return;
        }
        // No, it's a single click.
        on_mouse_pressed();
        return;
    }

// --------------------------------
// Released:
// Process and send message.
    if (event_type == GWS_MouseReleased){
        on_mouse_released();
        return;
    }

    if (event_type == MSG_MOUSE_DOUBLECLICKED)
    {
        yellow_status("MSG_MOUSE_DOUBLECLICKED");
        on_doubleclick();
        return;
    }

    // ...

// Not valid event type
not_valid:
    return;
}


// Keyboard events.
unsigned long 
wmProcessKeyboardEvent(
    int msg,
    unsigned long long1,
    unsigned long long2 )
{
    struct gws_window_d *window;
    //struct gws_window_d *tmp;
    unsigned long Result=0;
    //char name_buffer[64];
    int wid = -1;

// #todo
    unsigned int fg_color = (unsigned int) get_color(csiSystemFontColor);
    //unsigned int bg_color = (unsigned int) get_color(csiSystemFontColor);

    if (ServerState.state != SERVER_STATE_RUNNING)
        return 0;

    if (msg < 0){
        return 0;
    }

/*
// Window with focus. 
// (keyboard_owner)
    window = (struct gws_window_d *) get_focus();
// No keyboard owner
    if ( (void*) window == NULL )
    {
        return 0;
    }
    if (window->magic != 1234)
    {
        // #bugbug
        // Invalid keyboard_owner.
        //keyboard_owner = NULL;
        return 0;
    }
*/

//================================
    if (msg == GWS_KeyDown)
    {
        // We need the keyboard_owner.
        window = (struct gws_window_d *) get_focus();
        if ((void*) window == NULL)
            return 0;
        if (window->magic != 1234)
            return 0;

        // Enter?
        if (long1 == VK_RETURN)
        {
            on_enter();
            //window->ip_on = FALSE;
            return 0;
        }

        // Print a char into the window with focus.
        // It needs to be an editbox?
        //#todo: on printable.

        /*
        // APAGADO: Se esta apagado, ok, apenas pinte.
        if (window->ip_on != TRUE){
            // Pinte
            wm_draw_char_into_the_window(
                window, (int) long1, fg_color );
        // ACESO: Se esta acesa, apague, depois pinte.
        }else if (window->ip_on == TRUE){
            
            // Apague
            // #todo: Create a worker.
            wm_draw_char_into_the_window(
                window, (int) '_',  COLOR_WHITE );
            wm_draw_char_into_the_window(
                window, (int) VK_BACK,  COLOR_WHITE );
            window->ip_on = FALSE;

            // Pinte
            wm_draw_char_into_the_window(
                window, (int) long1, fg_color );        
        }
        */

        // The is the window with focus.
        wm_draw_char_into_the_window(
            window, (int) long1, fg_color );
        
        // The is the window with focus.
        // Enqueue a message into the queue that belongs
        // to the window with focus.
        // The client application is gonna get this message.
        wmPostMessage(
            (struct gws_window_d *) window,
            (int) msg,
            (unsigned long) long1,
            (unsigned long) long2);

        return 0;
    }

//================================
    if (msg == GWS_KeyUp)
    {
        // Nothing for now.
    }

//================================
    if (msg == GWS_SysKeyDown)
    {
        // Se pressionamos alguma tecla de funçao.
        // Cada tecla de funçao aciona um botao da barra de tarefas.

        // Alaways affect the active window.
        // #test: Sometimes it is the taskbar, sometimes not.
        if (long1 == VK_F1 ||
            long1 == VK_F2 ||
            long1 == VK_F3 ||
            long1 == VK_F4 )
        {
            /*
            if ((void*)active_window != NULL)
            {
                if (active_window->magic == 1234){
                    wid = (int) active_window->id;
                    window_post_message( wid, GWS_SysKeyDown, long1, long1 );
                }
            }
            */
            return 0;
        }

        // Always affect the active window.
        // #test: Sometimes it is the taskbar, sometimes not.
        if (long1 == VK_F5 ||
            long1 == VK_F6 ||
            long1 == VK_F7 ||
            long1 == VK_F8 )
        {
            /*
            if ((void*)active_window != NULL)
            {
                if (active_window->magic == 1234){
                    wid = (int) active_window->id;
                    window_post_message( wid, GWS_SysKeyDown, long1, long1 );
                }
            }
            */
            return 0;
        }

        // Always affect the active window.
        // F9  = Minimize button
        // F10 = Maximize button
        // F11 = Close button
        // #todo: Explain it better.
        if (long1 == VK_F9 || 
            long1 == VK_F10 || 
            long1 == VK_F11)
        {
            control_action(msg,long1);
            return 0;
        }

        // #todo
        // Testing the yellow dialog.
        if (long1 == VK_F12)
        {
            //YellowDialogInfo.useYellowDialog = TRUE;
            //YellowDialogInfo.display_dialog = TRUE;
            return 0;
        }

        // Always affect the window with focus.
        // Enfileirar a mensagem na fila de mensagens
        // da janela com foco de entrada.
        // O cliente vai querer ler isso.
        // We need the keyboard_owner.
        window = (struct gws_window_d *) get_focus();
        if ((void*) window == NULL)
            return 0;
        if (window->magic != 1234)
            return 0;
        // Post
        wmPostMessage(
            (struct gws_window_d *) window,
            (int) msg,
            (unsigned long) long1,
            (unsigned long) long2);

        return 0;
    }

//================================
    if (msg == GWS_SysKeyUp)
    {
        // Se liberamos alguma das 4 teclas de funçao.
        // Cada tecla lança um processo filho diferente,
        // todos pre-definidos aqui.

        if (long1 == VK_F1)
        {
            return 0;
        }
        if (long1 == VK_F2)
        {
            return 0;
        }
        if (long1 == VK_F3)
        {
            return 0;
        }
        if (long1 == VK_F4)
        {
            return 0;
        }

        // F5 = Update desktop.
        // Muda o status dos botoes da quick launch area,
        // atualiza a disposiçao das janelas na tela e
        // mostra a tela.
        if (long1 == VK_F5){
            wm_update_desktop(TRUE,TRUE);
            return 0;
        }
        
        if (long1 == VK_F6){
            wm_update_desktop2();
            return 0;
        }
        
        if (long1 == VK_F7){
            // Preserve the same active window.
            wm_update_desktop3(last_window);
            return 0;
        }

        // F6 = Entra ou sai do modo fullscreen.
        if (long1 == VK_F8)
        {

            /*
            //#debug
            wm_tile();
            window_post_message_broadcast( 
                0,           // wid = Ignored
                GWS_Paint,   // msg = msg code
                0,        // long1 = 
                0 );      // long2 = 
            */

            // Enter fullscreen mode
            if (WindowManager.is_fullscreen != TRUE){
                wm_enter_fullscreen_mode();
                return 0;
            }
            // Exit fullscreen mode.
            if (WindowManager.is_fullscreen == TRUE){
                wm_exit_fullscreen_mode(TRUE);
                return 0;
            }

            return 0;
        }

        // Liberamos as teclas de funçao relativas aos controles
        // de janelas.
        // #todo: Explain it better.
        if (long1 == VK_F9 || 
            long1 == VK_F10 || 
            long1 == VK_F11)
        {
            control_action(msg,long1);
            return 0;
        }

        // #todo
        // Testing the yellow dialog.
        if (long1 == VK_F12)
        {
            YellowDialogInfo.useYellowDialog = TRUE;
            YellowDialogInfo.display_dialog = TRUE;

            /*
            // #test OK
            // Testing the copying of a rectangle.
            // Valid only inside the backbuffer.
            __refresh_rectangle1( 
                400, 200,  // w and h
                10, 10, ____BACKBUFFER_VA,      // Destination
                100, 100, ____BACKBUFFER_VA );  // Source
            wm_flush_window(__root_window);     // Flush screen
            */

            /*
            // #test  OK
            // Testing the bitblt() routine
            // #important
            // The copy area is the width and height of the destine.
            struct gws_rect_d r1;
            r1.left = 10;
            r1.top = 10;
            r1.width = 100;     // copy width
            r1.height = 100;    // copy height
            struct gws_rect_d r2;
            r2.left = 200;
            r2.top = 200;
            r2.width = 200;
            r2.height = 200;

            bitblt00 (
                &r1,  // destination rect
                &r2,  // source rect   
                ____BACKBUFFER_VA,         // dst base buffer
                ____BACKBUFFER_VA,         // src base buffer
               0,                          // rop
               0 );                        // op
            wm_flush_window(__root_window);     // Flush screen
            */

            return 0;
        }
        
        // Nothing?
        
        return 0;
    }

// Not a valid msg

    return 0;
}

// Timer events.
void wmProcessTimerEvent(unsigned long long1, unsigned long long2)
{
    struct gws_window_d *window;

    //#debug
    //printf("Tick\n");

    if (ServerState.state != SERVER_STATE_RUNNING)
        return;

// We need the keyboard_owner.
// #todo: Only if the plain is blinking the cursor.
    window = (struct gws_window_d *) get_focus();
    if ((void*) window == NULL)
        return;
    if (window->magic != 1234)
        return;


// Print a char into the window with focus.
// It needs to be an editbox?

    //const int MyChar = '_';
    //const int MyChar = 219;  // Rectangle

   /*
// Acende (Draw black char)
// #todo: Create a worker.
    if (window->ip_on != TRUE){
        wm_draw_char_into_the_window( window, (int)     '_', COLOR_BLACK );
        wm_draw_char_into_the_window( window, (int) VK_BACK, COLOR_WHITE );
        window->ip_on = TRUE;

// Apaga (Draw white char)
// #todo: Create a worker.
    } else if (window->ip_on == TRUE ){
        wm_draw_char_into_the_window( window, (int)     '_', COLOR_WHITE );
        wm_draw_char_into_the_window( window, (int) VK_BACK, COLOR_WHITE );
        window->ip_on = FALSE;
    };
    */
}

// Handle combination code.
int wmProcessCombinationEvent(int msg_code)
{

// Parameter:
    if (msg_code < 0){
        goto fail;
    }

//
// z, x, c, v
//

// Control + z. (Undo)
    if (msg_code == GWS_Undo)
    {
        yellow_status("Undo");
        //demoCat();
        //demoTriangle();
        return 0;
    }

// Control + x. (Cut)
    if (msg_code == GWS_Cut)
    {
        yellow_status("Cut");
        
        // #test
        // gwssrv_quit();

        //demoCat();
        //__switch_active_window(TRUE);  //active first
        // Update desktop respecting the current list.
        //wm_update_desktop2();
        
        // #test
      
        // minimize all windows.
        //show_desktop();

        //

        return 0;
    }

// Control + c. (Copy)
    if (msg_code == GWS_Copy)
    {
        yellow_status("Copy");
        //__switch_active_window(FALSE);  //active NOT FIRST

        //#test

        // restore all windows
        //restore_desktop_windows();

        return 0;
    }

// Control + v. (Paste)
    if (msg_code == GWS_Paste)
    {
        yellow_status("Paste");
        return 0;
    }

// [control + a]
// Select all.
// #test (ok)
// Post message to all the overlapped windows.
// #test:
// Sending the wrong message.  
// This is just a test for now.
    if (msg_code == GWS_SelectAll)
    {
        yellow_status("Control + a");       
        gwssrv_broadcast_close();
        return 0;
    }

// [control+f]
// Find
    if (msg_code == GWS_Find){
        yellow_status("Control + f");
        return 0;
    }

// Control + s
// #test
// Cfor the root window.
// Only refresh if it is already created.
    if (msg_code == GWS_Save){
        yellow_status("Control + s");
        return 0;
    }

/*
// #todo
// Control + w
// Close the active window.
    if (msg_code == GWS_XXXXX)
    {
        yellow_status("Control + w");
        // #todo: Close the active window.
        return 0;
    }
*/

// --------------

// Control + Arrow keys.
    if (msg_code == GWS_ControlArrowUp)
    {
        //yellow_status("Control + up");
        dock_active_window(1);
        return 0;
    }
    if (msg_code == GWS_ControlArrowRight)
    {
        //yellow_status("Control + right");
        dock_active_window(2);
        return 0;
    }
    if (msg_code == GWS_ControlArrowDown)
    {
        //yellow_status("Control + down");
        dock_active_window(3);
        return 0;
    }
    if (msg_code == GWS_ControlArrowLeft)
    {
        //yellow_status("Control + left");
        dock_active_window(4); 
        return 0;
    }

// [shift + f12]
// Enable the ps2 mouse support
// by making the full ps2-initialization.
// Valid only for qemu.
// + Enable mouse.
// + Change bg color.
    // #define SHIFT_F12  88112 
    if (msg_code == 88112)
    {
        yellow_status("Shift + F12: Enable mouse");
        // Calling the kernel to make the full ps2 initialization.
        // #todo: Create a wrapper fot that syscall.
        // #todo: Allow only the ws pid to make this call.
        sc82 ( 22011, 0, 0, 0 );
        // Enable the use of mouse here in the server.
        gUseMouse = TRUE;
        return 0;
    }

fail:
    return (int) (-1);
}


// #todo
// We can carefully move some routine from wm.c to this file.

inline int is_combination(int msg_code)
{
    if (msg_code<0)
        return FALSE;

    switch (msg_code){
    case GWS_ControlArrowUp:
    case GWS_ControlArrowRight:
    case GWS_ControlArrowDown:
    case GWS_ControlArrowLeft:
    case GWS_Cut:
    case GWS_Copy:
    case GWS_Paste:
    case GWS_Undo:
    case GWS_SelectAll:
    case GWS_Find:
    case GWS_Save:
    case 88112:
        return TRUE;
        break;
    //...
    default:
        return FALSE;
        break;
    };

    return FALSE;
}


// wmInputReader:
// (Input port)
// Get the messages in the queue,
// respecting the circular queue.
int wmInputReader(void)
{
// + Get input events from the thread's event queue.
// + React to the events.
// Getting input events from the event queue
// inside the control thread structure.

// #todo
// We can create a libinput/ to handle the low level
// routine for this type of function.
// That function is gonna be used by the compositors/display servers,
// not by the client-side appications.

    int status=0;

    register long i=0;

    long extra_attempts = 4;

    // --------
    // Msg
    int msg=0;
    unsigned long long1=0;
    unsigned long long2=0;
    // --------
    unsigned long long3=0;
    // #todo: Get the button numberfor mouse clicks.

    int IsCombination=FALSE;

//NextEvent:

    //status = (int) rtl_get_event();
    //if (status != TRUE)
    //{
        for (i=0; i<extra_attempts; i++)
        {
            status = (int) rtl_get_event();
            if (status == TRUE)
                goto ProcessEvent;
        };

// No more attempts.
    goto fail;
    //}

// ---------------------
// Only for mouse move events.
GetNextEvent:
    status = (int) rtl_get_event();
    if (status != TRUE)
        goto fail;

// ---------------------
// All types if events.
ProcessEvent:

    msg   = (int) (RTLEventBuffer[1] & 0xFFFFFFFF);
    long1 = (unsigned long) RTLEventBuffer[2];
    long2 = (unsigned long) RTLEventBuffer[3];
// #test
    long3 = (unsigned long) RTLEventBuffer[4];  //jiffie

// Is it time to use the yellow dialog?
    if (YellowDialogInfo.useYellowDialog == TRUE){
        yellow_status_dialog(msg,long1,long2,long2);
        return 0;
    }

// -----------------------
// MOUSE events
    if ( msg == GWS_MouseMove || 
         msg == GWS_MousePressed ||
         msg == GWS_MouseReleased )
    {
        // Get the current event jiffie.
        if (msg == GWS_MousePressed){
            DoubleClick.current = (unsigned long) long3;
        }

        wmProcessMouseEvent(
            (int) msg, (unsigned long) long1, (unsigned long) long2 ); 

        // LOOP: Processamos um evento de movimento,
        // provavelmente teremos outro subsequente.
        if (msg == GWS_MouseMove){
            goto GetNextEvent;
        }
        return 0;
    }

// -----------------------
// Some keyboard events.
// Print char into the keyboard owner window.
    if ( msg == GWS_KeyDown ||
         msg == GWS_SysKeyDown ||
         msg == GWS_SysKeyUp )
    {
        wmProcessKeyboardEvent( 
            (int) msg, (unsigned long) long1, (unsigned long) long2 );
        return 0;
    }

// ---------------------------------
// Master timer
    if (msg == GWS_Timer)
    {
        // OK, it's working
        if (long1 == 1234){
            //printf("Tick %d\n",long2);
            wmProcessTimerEvent(long1,long2);
        }
        return 0;
    }

// ---------------------------------
// Combination
// Is it a combination?
// The keyboard driver process the combination
// and send us the combination index.
    IsCombination = (int) is_combination(msg);
    int ComStatus = -1;
    if (IsCombination)
    {
        // Process the combination and
        // we're done if it's ok.
        ComStatus = (int) wmProcessCombinationEvent(msg);
        if (ComStatus == 0){
            return 0;
        }
        goto fail;
    }

    // Hotkeys 
    // if (msg == GWS_HotKey){}

// Sys commands
    //if (msg == GWS_Command){
        // #todo: Call a worker for that.
    //}

// #test
// Notificando o display server que a resolução mudou.
// #todo
// Muidas estruturas aindapossuem valores que estão condizentes
// com a resolução antiga e precisa ser atualizados.

    if (msg == 800300){
        printf("[800300] w=%d h=%d\n", long1, long2);
        return 0;
    }

// #test: [Control + w] also generate GWS_Close message.
// Close with the active window.
// Actually a combination also can genrate this message.
    if (msg == GWS_Close)
    {
        if ((void*) active_window != NULL)
        {
            if (active_window->magic == 1234)
            {
                yellow_status("Close window");
                window_post_message ( active_window->id, GWS_Close, 0, 0 );        
            }
        }
    }


    //if (msg == GWS_UpdateDesktop)
    //    wm_update_desktop(TRUE,TRUE);

    // #test: Not implemented
    // Shutdown the server.
    // This is a message sent by the kernel,
    // this way we can close all the clients,
    // sending an event, telling them to close.
    //if (msg == 800350)
       //something()

    // The kernel received a gprot message 
    // and redirected it to us.
    if (msg == 800800)
        yellow_status("ds00: 800800");

//Unknown:
    return 0;
fail:
    return (int) (-1);
}


// ------------------------------------------------
// wmInputReader2
// This is basically the low level input support 
// for the Gramado OS when running the Gramado Window System.
// This routine do not pump the messages from a file, just
// like the traditional way. It just get messages in a queue
// in the control thread of the display server process.
// The kernel post all the input messages into this queue for us.
// See: dev/tty in the kernel source code.
// ------------------------------------------------
// Read input from thread's queue.
// Esse nao eh o melhor metodo.
// #todo: precisamos ler de um arquivo que contenha
// um array de estruturas de eventos.
// #todo: Essas rotinas de input de dispositivo
// precisam ficar em bibliotecas. Mas de uma
// biblioteca pode existir no servidor, uma
// pra cada tipo de sistema.
// vamos tentar 32 vezes,
// pois nossa lista tem 32 ou 64 slots.
// Se encontrarmos um evento, entao pegamos ele.
// #bugbug: Isso eh um problema,
// pois quando nao tiver mensagens na fila,
// teremos que executar esse loop.
// #todo
// A mensagem de tecla pressionada
// deve vir com a informação de quanto
// tempo ela permaneceu pressionada.
// processamos ate 32 input válidos.
// isso deve ajudar quando movimentarmos o mouse.
// #importante:
// Se o servidor for portado para outro sistema
// então precisamos converter o formato do eventro
// entregue pelo sistema operacional, em um formato
// que o servidor consegue entender.
// Por enquanto o servidor entende o formato de eventos
// entregues pelo gramado.
// Called by main.c

int wmInputReader2(void)
{
// Getting input events from the event queue
// inside the control thread structure.
// Process all the messages in the queue, 
// starting at the first message.
// Disrespecting the circular input.

    int __Status = -1;
    register int i=0;
// see: event.h
    struct gws_event_d e;

    int IsCombination=FALSE;

// 32 slots
    for (i=0; i<MSG_QUEUE_MAX; i++)
    {
        // Não volte ao inicio da fila
        if(i< (MSG_QUEUE_MAX-1)) { __Status = rtl_get_event2(i,FALSE); }
        // Volte ao inicio da fila.
        if(i==(MSG_QUEUE_MAX-1)){ __Status = rtl_get_event2(i,TRUE);  }

        // #todo
        // Se a mensagem for um input de teclado,
        // então enviamos a mensagem 
        // para a janela com o foco de entrada.
        // Mensagens de outro tipo 
        // podem ir para outras janelas.
        
        if (__Status==TRUE)
        {
            // #test
            e.msg   = (int)           RTLEventBuffer[1];
            e.long1 = (unsigned long) RTLEventBuffer[2];
            e.long2 = (unsigned long) RTLEventBuffer[3];

            // MOUSE events
            // Calling procedure.
            if ( e.msg == GWS_MouseMove || 
                 e.msg == GWS_MousePressed ||
                 e.msg == GWS_MouseReleased )
            {
                wmProcessMouseEvent(
                    (int) e.msg,
                    (unsigned long) e.long1,
                    (unsigned long) e.long2 ); 
            }

            // keyboard
            // mensagens desse tipo
            // devem ir para a janela com o foco de entrada.
            if ( e.msg == GWS_KeyDown ||
                 e.msg == GWS_SysKeyDown ||
                 e.msg == GWS_SysKeyUp )
            {
                // Print char into the keyboard owner window.
                wmProcessKeyboardEvent( 
                    (int) e.msg, 
                    (unsigned long) e.long1, 
                    (unsigned long) e.long2 );
            }

            // Is it a combination?
            IsCombination = (int) is_combination(e.msg);
            if (IsCombination){
                wmProcessCombinationEvent(e.msg);
            }

            if (e.msg == GWS_HotKey)
            {
                // #todo: Call a worker for that.
                
                // Hot key id.
                // Activate the window associated with the given ID.
                if (e.long1 == 1){
                    yellow_status ("GWS_Hotkey 1\n");
                }
                if (e.long1 == 2){
                    yellow_status ("GWS_Hotkey 2\n");
                }
                // ...
            }

            //if (e.msg == GWS_Command){
                // #todo: Call a worker for that.
            //}
        }
    };

    return 0;
}

// Read n bytes from stdin
int wmSTDINInputReader(void)
{
    size_t nreads=0;
    char buffer[512];
    register int i=0;

    nreads = (size_t) read(0,buffer,512);
    if (nreads<=0){
        return -1;
    }

    //printf("%d bytes\n",nreads);

    i=0;
    for (i=0; i<nreads; i++)
    {
            /*
            ?????procedure( 
                0,            // window pointer
                GWS_KeyDown,  // msg code
                buffer[i],    // long1
                buffer[i] );  // long2
            */
    };

    return (int) nreads;
}

int wminputGetAndProcessSystemEvents(void)
{
    return (int) wmInputReader();
}


