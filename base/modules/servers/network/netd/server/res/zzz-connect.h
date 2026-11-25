

// Application services can subscribe to one or more port(s) 
// to be notified if a UDP message is sent to that port  

// Quando um pacote chega a uma determinada porta,
// Então devemos (notificar) o aplicativo que está conectado à
// aquela porta.
// E enviar os dados pra ele. Se ele pedir.

#ifndef ____CONNECT_H
#define ____CONNECT_H




#define CONNECTION_MAX 32



// Estrutura para conectar porcessos à portas.
// Conectando um processo à uma porta.
struct connection_d 
{
	//object_type_t objectType;
	//object_class_t objectClass;
	
    int used;
    int magic;

    // Conectando um processo a uma porta.
    unsigned short port;
    struct process_d *process;    

    struct channel_d *channel;
};
struct connection_d *CurrentConnection;


// Colocaremos nesse array os ponteiros para as estruturas
// de coneção. connection_d
unsigned long connectionList[CONNECTION_MAX];


//conectando um processo à uma porta
//int network_connect_process (unsigned short port);
//int network_connect_process (unsigned short port){}


#endif    



