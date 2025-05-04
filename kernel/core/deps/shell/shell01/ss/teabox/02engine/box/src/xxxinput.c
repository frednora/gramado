
// #todo
// Em kernel mode temos essas mesma rotinas só que aprimoradas.
// corrigidos alguns problemas com tipagem e definições.
// começe olhando para o código em ring0 e deixe esse igual ao que esta la.
// nem seu se devemos colocar isso realmente aqui ...
// mas precisamos ter rotinas como essas em algum lugar que não seja o kernel.

// #test
// Colocando aqui algumas rotinas de input.
// São específicas para o gramado.
// Começaremos com as rotinas de mouse.

#include "gram3d.h"

// =============================================================

// #bugbug
// It was a test.
// Why we still havell all this code here?

// #test
// #bugbug 
// ps2-mouse driver?
// This is not what we wont.


//int MOUSE_WINDOW = -1;

// i8042 mouse status bit.
#define MOUSE_LEFT_BTN    0x01
#define MOUSE_RIGHT_BTN   0x02
#define MOUSE_MIDDLE_BTN  0x04
#define MOUSE_X_SIGN      0x10
#define MOUSE_Y_SIGN      0x20
//#define MOUSE_X_OVERFLOW  0x40
//#define MOUSE_Y_OVERFLOW  0x80

// Generic PS/2 Mouse Packet Bits
#define  MOUSE_LEFT_BUTTON    0x01
#define  MOUSE_RIGHT_BUTTON   0x02
#define  MOUSE_MIDDLE_BUTTON  0x04
#define  MOUSE_X_DATA_SIGN    0x10
#define  MOUSE_Y_DATA_SIGN    0x20
#define  MOUSE_X_OVERFLOW     0x40
#define  MOUSE_Y_OVERFLOW     0x80

long mouse_x = 0;
long mouse_y = 0;

char mouse_packet_data = 0;
char mouse_packet_x = 0;
char mouse_packet_y = 0;
char mouse_packet_scroll = 0;

char saved_mouse_x =0;
char saved_mouse_y =0;

//Estado dos botões do mouse
int mouse_buttom_1;
int mouse_buttom_2;
int mouse_buttom_3;

//Estado anterior dos botões do mouse.
int old_mouse_buttom_1;
int old_mouse_buttom_2;
int old_mouse_buttom_3;

//Ae ouve alguma modificação no estado dos botões.
int mouse_button_action;

//=============================
// mouse control

// habilitar e desabilitar o mouse.
// usada pelos aplicativos.
int ps2_mouse_status;

int ps2_mouse_has_wheel;

// 1=pressionado 0=liberado
int ps2_button_pressed;

int ps2_mouse_moving;
int ps2_mouse_drag_status;
//int ps2_mouse_drop_status;

int window_mouse_over;





// =============================================================




/*
 * =====================================================
 * update_mouse:
 *     Updates the mouse position.
 */

void update_mouse (void)
{

//======== X ==========
// Testando o sinal de x.
// Do the x pos first.

// Pega o delta x
// testa o sinal para x
do_x:
    if ( mouse_packet_data & MOUSE_X_SIGN ) {
        goto x_neg;
    }

// Caso x seja positivo.
x_pos:
    mouse_x += mouse_packet_x;
    goto do_y;

// Caso x seja negativo.
x_neg:
    mouse_x -= ( ~mouse_packet_x + 1 );

    if (mouse_x > 0)
    {
        goto do_y;
    }
    mouse_x = 0;

//======== Y ==========
// Testando o sinal de x.
// Do the same for y position.

// Pega o delta y.
// Testa o sinal para y.
do_y:
    if ( mouse_packet_data & MOUSE_Y_SIGN )
    {
        goto y_neg;
    }

// Caso y seja positivo.
y_pos:
    mouse_y -= mouse_packet_y;

    if ( mouse_y > 0 )
    {
        goto quit;
    }

    mouse_y = 0;
    goto quit;

// Caso y seja negativo. 
y_neg:

    mouse_y += ( ~mouse_packet_y + 1 );

// Quit
quit:
    return;
}

void parse_data_packet ( char data, char x, char y)
{

    //if (fd<0) 
        //return;

	// A partir de agora já temos os três chars.
	// Colocando os três chars em variáveis globais.
	// Isso ficará assim caso não haja overflow.
    // acho que isso sera usado na rotina de update.
    
    mouse_packet_data = (char) data; // Primeiro char
    mouse_packet_x    = (char) x;    // Segundo char.
    mouse_packet_y    = (char) y;    // Terceiro char.

    // Salvando o antigo antes de atualizar.
    // Para poder apagar daqui a pouco.
    // Atualizando.

    saved_mouse_x = mouse_x;
    saved_mouse_y = mouse_y;
    
    //
    // == Update mouse position ====================
    //
    
    update_mouse (); 
    
    // Agora vamos manipular os valores obtidos através da 
    // função de atualização dos valores.
    // A função de atualização atualizou os valores de
    // mouse_x e mouse_y.
    mouse_x = (mouse_x & 0x000003FF );
    mouse_y = (mouse_y & 0x000003FF );

    // Limits
    //if ( mouse_x < 1 ){ mouse_x = 1; }
    //if ( mouse_y < 1 ){ mouse_y = 1; }
    //if ( mouse_x > (SavedX-16) ){ mouse_x = (SavedX-16); }
    //if ( mouse_y > (SavedY-16) ){ mouse_y = (SavedY-16); }

    /*
    // Comparando o novo com o antigo, pra saber se o mouse se moveu.
    // #obs: Pra quem mandaremos a mensagem de moving ??
    if ( saved_mouse_x != mouse_x || saved_mouse_y != mouse_y )
    {
	// flag: o mouse está se movendo.
	// usaremos isso no keydown.
	// >> na hora de enviarmos uma mensagem de mouse se movendo
	// se o botão estiver pressionado então temos um drag (carregar.)
	// um release cancela o carregar.
		
        ps2_mouse_moving = 1;

       // draw

       // Apaga o antigo.
       // + Copia no LFB um retângulo do backbuffer 
       // para apagar o ponteiro antigo.
       refresh_rectangle ( saved_mouse_x, saved_mouse_y, 20, 20 );
                
       // Acende o novo.
       //+ Decodifica o mouse diretamente no LFB.
       // Copiar para o LFB o antigo retângulo  
       // para apagar o ponteiro que está no LFB.
       bmpDisplayMousePointerBMP ( mouseBMPBuffer, mouse_x, mouse_y );   
                         
    }else{
		
	// Não redesenhamos quando o evento for um click, sem movimento.
        ps2_mouse_moving = 0;
    };
    */

    // Apenas obtendo o estado dos botões.
    mouse_buttom_1 = 0;
    mouse_buttom_2 = 0;
    mouse_buttom_3 = 0;

    // ## LEFT ##
    if ( ( mouse_packet_data & MOUSE_LEFT_BUTTON ) == 0 )
    {
        // liberada.
        mouse_buttom_1 = 0;
        ps2_button_pressed = 0;

        //mudamos sempre que pressionar.
        //todo: mudaremos sempre que pressionar numa title bar.
        //refresh_rectangle ( saved_mouse_x, saved_mouse_y, 20, 20 );
        //bmpDisplayMousePointerBMP ( mouseBMPBuffer, mouse_x, mouse_y ); 

    }else if( ( mouse_packet_data & MOUSE_LEFT_BUTTON ) != 0 )
     {
		    //pressionada.
		    //Não tem como pressionar mais de um botão por vez.
            mouse_buttom_1 = 1;
            mouse_buttom_2 = 0;
            mouse_buttom_3 = 0;
            ps2_button_pressed = 1;
            
            //mudamos sempre que pressionar.
            //todo: mudaremos sempre que pressionar numa title bar.
            //refresh_rectangle ( saved_mouse_x, saved_mouse_y, 20, 20 );
            //bmpDisplayMousePointerBMP ( appIconBuffer, mouse_x, mouse_y ); 
     };

    // ## RIGHT ##
    if ( ( mouse_packet_data & MOUSE_RIGHT_BUTTON ) == 0 )
    {
        // liberada.
        mouse_buttom_2 = 0;
        ps2_button_pressed = 0;

    }else if( ( mouse_packet_data & MOUSE_RIGHT_BUTTON ) != 0 )
        {
		    //pressionada.
		    //Não tem como pressionar mais de um botão por vez.
            mouse_buttom_1 = 0;
            mouse_buttom_2 = 1;
            mouse_buttom_3 = 0;
            ps2_button_pressed = 1;
        };

    // ## MIDDLE ##
    if ( ( mouse_packet_data & MOUSE_MIDDLE_BUTTON ) == 0 )
    {
        // liberada.
        mouse_buttom_3 = 0;
        ps2_button_pressed = 0;

    }else if( ( mouse_packet_data & MOUSE_MIDDLE_BUTTON ) != 0 )
        {
	        //pressionada.
	        //Não tem como pressionar mais de um botão por vez.
	        mouse_buttom_1 = 0;
	        mouse_buttom_2 = 0;
	        mouse_buttom_3 = 1;
	        ps2_button_pressed = 1;
        };

    // ===
    // Confrontando o estado atual com
    // o estado anterior para saber se ouve
    // alguma alteração ou não.
    // 1 = ouve alteração.

    if ( mouse_buttom_1 != old_mouse_buttom_1 ||
         mouse_buttom_2 != old_mouse_buttom_2 ||
         mouse_buttom_3 != old_mouse_buttom_3 )
    {
        mouse_button_action = 1;
    }else{
        mouse_button_action = 0;
    };

    // # isso ainda eh um teste.
    old_mouse_buttom_1 = mouse_buttom_1;
    old_mouse_buttom_2 = mouse_buttom_2;
    old_mouse_buttom_3 = mouse_buttom_3;
}

/*
 **************** 
 * top_at: 
 * 
 */

// #todo
// z order not initialized yet.


// pega a janela que está mais ao topo da zorder e que
// corresponda às cordenadas do mouse.
// retorna window id

int top_at ( int x, int y ){

    int z = 0;
    int wID = -1;
    struct gws_window_d *__last_found;
    struct gws_window_d *tmp;

    // max 1024 janelas.
    for ( z=0; z<ZORDER_MAX; z++ )
    {
        //pega a próxima na zorderlist;
        tmp = (struct gws_window_d *) zList[z];
    
        //check
        if ( (void *) tmp != NULL )
        {
            if ( tmp->used == 1 && tmp->magic == 1234 )
            {

                 // #importante
                 // Checando coordenadas.
                  if ( x > (tmp->absolute_x)  && 
                       x < (tmp->absolute_x + tmp->width)  && 
                       y > (tmp->absolute_y)  &&
                       y < (tmp->absolute_y + tmp->height)  )
                 {
					 //printf ("%d",z);
                      // salva essa.
                      __last_found = (struct gws_window_d *) tmp;
                 }
            }
        }

    };

__found:
    window_mouse_over = __last_found->id;
    return (int) __last_found->id;
}


int mouse_scan_windows (void)
{
    struct gws_window_d *Window;
    int wID = -1;

    wID = (int) top_at ( mouse_x, mouse_y );

    //================
    // -1 = Se não temos uma janela.
    if ( wID < -1 ) //( wID == -1 )
    {
        // ...
    }

    //============================ 
    // Se estamos sobre uma janela válida.
    if ( wID >= 0 )  //if ( wID > -1 )
    {
        Window = (struct gws_window_d *) windowList[wID];

        if ( (void *) Window == NULL ) { return -1; }
        if ( Window->used != 1 || Window->magic != 1234){ return -1; }

        //===============================================
        // ***Se houve mudança em relação ao estado anterior.
        // Nesse momento um drag pode terminar
        //if ( mouse_button_action == 1 )
        //{ ... }

        //===============================================
        // *** Se NÃO ouve alteração no estado dos botões, então apenas 
        // enviaremos a mensagem de movimento do mouse e sinalizamos 
        // qual é a janela que o mouse está em cima.
        // Não houve alteração no estado dos botões, mas o mouse
        // pode estar se movendo com o botão pressionado.
        // a não ser que quando pressionamos o botão ele envie várias
        // interrupções, igual no teclado.
        // if ( mouse_button_action == 0 )
        // { ... }
    }

    return -1;
}



void input_dummy(void)
{
	
}



/*
[ Input ]
The Window Server has basically two kinds of input:
The first one is the hardware input that come from the kernel or from the device drivers.
And the second one is the requests that come from the clients.
---
The hardware input will affect the window with focus. So it will affect the client that this window belongs to. So, we need to put this kind of message into the client's queue.
When a client  request the next event, the window server will check the queue that belong to this client.
*/

// char __system_message_buffer[512];


// get system message
// it comes from the kernel.
// it is hardware events.
// send the message to the active client queue.
// the event affects the window with focus and its client.

/*
int 
gwssrv_get_system_message(void);

int 
gwssrv_get_system_message(void)
{
    unsigned long *message_buffer = (unsigned long *) &__system_message_buffer[0];   

    
    int ClientWithFocus;
    int WindowWithFocus;

    ClientWithFocus = gwssrv_get_client_with_focus();
    WindowWithFocus = gwssrv_get_window_with_focus();
    
    //
    // Get system message
    //    

    // Get message from kernel.
    gde_enter_critical_section();
    gramado_system_call ( 111,
        (unsigned long) &message_buffer[0],
        (unsigned long) &message_buffer[0],
        (unsigned long) &message_buffer[0] );
    gde_exit_critical_section();


   //
   // Put the message into the client's queue
   //

    struct gws_client_d *client;
        
    //ponteiro para a estrutura do cliente com o foco
    client = (void*) clientList[ClientWithFocus];
    
    client->window[tail] = message_buffer[0];
    client->msg[tail]    = message_buffer[1];
    client->long1[tail]   = message_buffer[2];
    client->long2[tail] = message_buffer[3];
    //
    
    //#todo
    // circular o offset
    tail++
    if(tail >?)tail=0;
    
}
*/



/*
// #todo
// Select what kind of input a window wants to receive.
int 
gwssrv_select_input(
    struct gwssrv_window_d *window,
    unsigned long mask );
int 
gwssrv_select_input(
    struct gwssrv_window_d *window,
    unsigned long mask )
{
    return -1;
}
*/    




