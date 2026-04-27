/*
 * File: channel.h 
 *
 * Descrição:
 *     Header for channel manager.
 *     Channel manager. 
 *     Gerenciamento de canais Client/Server para troca de mensagens entre 
 * processos.
 *    " Um canal de comunicação é composto de dois soquetes, um para o cliente 
 * e outro para o servidor. "
 *
 * ?? isso não deveria fazer parte do /pc/ipc
 *
 * Versão 1.0, 2015, 2016.    
 */

#ifndef ____CHANNEL_H
#define ____CHANNEL_H


/*
#define  LISTEN_CHANNEL 0x1
#define MESSAGE_CHANNEL 0x2

typedef enum _ECHANNEL_STATE {
    ClientIdle,
    ClientSendWaitReply,
    ClientShutdown,
    ServerIdle,
    ServerReceiveMessage,
    ServerShutdown
} ECHANNEL_STATE;
*/



/*
 * Estrutura para canal.
 * Serve para estabelecer uma conexão,
 * ?? pelo jeito não serve para 
 * trocar mensagens, somente para estabelecer a 
 * conexão.
 * Obs: Essa estrutura é aceitável, está em conformidade 
 * com os padrões encontrados
 */

struct channel_d
{
    object_type_t objectType;
    object_class_t objectClass;


    int id;
    
    int used;
    int magic;

    int type; //??

    int state;    //Aberto ou fechado.

    //...

    //Os dois soquetes do canal de comunicação.
    // acho que soquete tem stream.


    struct socket_d *server_socket;
    struct socket_d *client_socket;

    // Buffer usado pelo dois soquetes na mesma máquina.
    // 

    void *____BUFFER;


    // Quem é o dono do canal?
    // Quem está enviando?
    // Quem está recebendo.
    //struct process_d *OwnerProcess;
    //struct process_d *SenderProcess;
    //struct process_d *ReceiverProcess;

	// usado para comunição entre threads.
	//#importante
	//Quando a comunicação for entre processos,
	//podemos que usar a thread de controle do processo que 
	//está recebendoa mensagem.
    //struct thread_d *SenderThread;
    //struct thread_d *ReceiverThread;



	//#importante:
	//context ??
	//me parece que um ponteiro pra uma estrutura do 
	//tipo context é necessário 
	//context deve conter os elementos necessários 
	//para a transmissão da mensagem. Como o buffer 
	//de memória compartilhada.
	//
    
	//?? O que é isso ???
	//me parece que é necessário.
	//struct channel_d *serverchannel;
};
struct channel_d *CurrentChannel;
//...


unsigned long channelList[32];




//
// Prototypes.
//

//#todo: arguments.
void *CreateChannel (void);

int DestroyChannel (struct channel_d *channel);


//#todo: arguments.
int OpenChannel ( void );


int CloseChannel (struct channel_d *channel);



#endif    


//
// End.
//

