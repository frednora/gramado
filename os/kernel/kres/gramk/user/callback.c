// callback.c
// Created by Fred Nora.

#include <kernel.h>

//--------------------------------------

// #test
// Variáveis usadas em unit3hw.asm para implementar
// a chamada de um callback em ring3.
// O salto para ring3 acontece depois que o cr3 
// do processo alvo sofreu refresh. Para garantir
// que temos o espaço d endereçamento que desejamos.
// #todo: Podemos mudar esses nomes para ficar mais fácil
// para o assembly entender o que esta acontecendo.
// see: psTaskSwitch

// Exported to asm.
unsigned long asmflagDoCallbackAfterCR3=0;
unsigned long ring3_callback_address=0;

// Not exported to asm.
int _callback_status=FALSE;
unsigned long _callback_address=0;
unsigned long _callback_address_saved=0;

struct ws_callback_info_d  ws_callback_info;

// initialize for the first time 
// during the kernel initialization.
void initialize_ws_callback_info(void)
{
    ws_callback_info.ready = FALSE;  // status
    ws_callback_info.callback_address = 0;   //#bugbug Invalid ring3 address.
    ws_callback_info.callback_address_saved = 0;

    ws_callback_info.each_n_ms = 16;
    ws_callback_info.times_per_second = (1000/16); 

// Initialized for the first time.
    ws_callback_info.initialized = TRUE;
}


// service 44000
// Also called by DeviceInterface_PIT in pit.c.
void setup_callback(unsigned long r3_address, unsigned long ms)
{

    // nao foi inicializado pela inicialização do kenrel.
    // see: x64init.c
    if( ws_callback_info.initialized != TRUE ){
        panic("setup_callback: callback support Not initialized\n");
    }

// Esse é o endereço do handler em ring 3.
// passado pelo window server.
    //_callback_address = (unsigned long) r3_address;
    ws_callback_info.callback_address = (unsigned long) r3_address;
    
// Salvamos o endereço original,
// DeviceInterface_PIT() em pit.c vai reconfigurar
// esses valores de tempos em tempos,
// chamando essa função. Para isso ele usa essa variável salva.
    //_callback_address_saved = (unsigned long) r3_address;
    ws_callback_info.callback_address_saved = (unsigned long) r3_address;

    if(ms<1)
        panic("setup_callback: ms<1\n");
    if(ms>1000)
        panic("setup_callback: ms>1000\n");

    ws_callback_info.each_n_ms = ms;  //16;
    ws_callback_info.times_per_second = (1000/ms);  //(1000/16); 

// Indica que temos um callback
// passado pelo window server.
    //_callback_status = TRUE;
    ws_callback_info.ready = TRUE;

}



void prepare_next_ws_callback(void)
{
    if ( ws_callback_info.ready != TRUE ){
        panic("prepare_next_ws_callback: No ready\n");
    }

// acionamos a flag que sera usada pelo assembly.
// atualizamos o endereço que sera usado pelo assembly.
// see: unit3hw.asm
// o assembly precisa disso.

//flag
    asmflagDoCallbackAfterCR3 = 
        (unsigned long) (0x1234 & 0xFFFF);

// handler    
    if ( ws_callback_info.callback_address == 0 )
        panic("prepare_next_ws_callback: Invalid ws_callback_info.callback_address\n");
            
    ring3_callback_address = 
        (unsigned long) ws_callback_info.callback_address;
            
// Dizemos que não ha mais um callback inicializado.
// forçando o timer a atualizar novamente
// usando um endereço salvo.
// #bugbug: Quando o timer libera para usamos o callback,
// o ts.c ainda precisa esperar chegar a vez
// da thread de controle do window server. Isso pode
// demorar um round inteiro.

    ws_callback_info.ready = FALSE;
    ws_callback_info.callback_address = 0;
    //#important: the saved value is still preserved.
}





