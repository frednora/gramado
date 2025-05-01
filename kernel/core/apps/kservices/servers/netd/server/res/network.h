/*
 * File: network.h 
 *     header para o gerenciamento de rede.
 * 
 *  History: 
 *      2019 - Created by Fred Nora.
 */


#ifndef ____NETWORK_H
#define ____NETWORK_H



// >> The register at offset 0x00 is the "IOADDR" window. 
// >> The register at offset 0x04 is the "IODATA" window. 


// packet format
// Ethernet IPv4 TCP/UDP DATA FCS


// See: 
// https://en.wikipedia.org/wiki/EtherType
#define ETH_TYPE_IP    0x0800  
#define ETH_TYPE_IPV4  0x0800  
#define ETH_TYPE_ARP   0x0806
#define ETH_TYPE_IPV6  0x86DD
// ...




// little endian ?

#define ToNetByteOrder16(v)  ((v >> 8) | (v << 8))
#define ToNetByteOrder32(v)  (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))

#define FromNetByteOrder16(v) ((v >> 8) | (v << 8))
#define FromNetByteOrder32(v) (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))



/*
// Device
// #test
struct network_device_d 
{
    int id;
	
	void *priv; //??

	char *dev_name;

    unsigned char ipv4_address[4];
    unsigned char mac_address[6];

	//virtual
	//Nope!
	//Void (*send)(PVoid, UIntPtr, PUInt8);
};
*/

struct network_buffer_d
{
    int receive_tail;
    int receive_head;
    unsigned long receive_buffer[32];

    int send_tail;
    int send_head;
    unsigned long send_buffer[8];    
};
struct network_buffer_d NETWORK_BUFFER;



/*
 * network_info_d:
 *     Estrutura de rede.
 */ 

struct network_info_d
{
    int id;

    int used;
    int magic;

    int networkIdentifier;  // Número identificador da rede.
    char *networkName;      // Nome da rede.
    char *networkVersion;   // String mostrando a versão. ex: (1.1.1234)
    unsigned short networkVersionMajor;
    unsigned short networkVersionMinor; 
    unsigned short networkVersionRevision;

	//adaptador de rede.
	//struct intel_nic_info_d *nic;
	
	//user ??
	//struct user_info_d *networkUserinfo;
	
	//..

    struct network_info_d *next;
};
//struct network_info_d *Network;

// Essa flag poderia ir para dentro da estrutura acima,
int ____network_late_flag;



//
// Prototypes.
//
/*

// Init.
int networkInit (void);



unsigned long 
network_procedure ( 
    struct window_d *window,
    int msg,
    unsigned long long1,
    unsigned long long2 );


// Tests.
void network_test(void);
void testNIC (void);


void networkSetstatus (int status);
int networkGetStatus (void);


// Show info.
void show_network_info (void);
void show_current_nic_info (void);


// Manipular o pacote ipv6 recebido pelo handle do e1000.
int handle_ipv6 ( struct ipv6_header_d *header );


// Send arp.
void 
SendARP ( int op,   //operation 
    uint8_t source_ip[4], 
    uint8_t target_ip[4], 
    uint8_t target_mac[6] );


// Send UDP.
int
network_SendIPV4_UDP ( 
    uint8_t source_ip[4], 
    uint8_t target_ip[4], 
    uint8_t target_mac[6], 
    uint8_t data[32],
    unsigned short port );


// Dialogo para o driver de rede.
unsigned long 
network_driver_dialog ( 
    struct window_d *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );


int network_decode_buffer ( unsigned long buffer_address );


// Colocar um buffer numa lista de buffers.
int network_buffer_in( void *buffer, int len );
// Retirar um buffer de uma lista de buffers.
int network_buffer_out ( void *buffer, int len );

//o ns pega o buffer no kernel.
int sys_network_receive(void *ubuf, int size);
//o ns envia um buffer pra ser enviado para rede.
int sys_network_send(void *ubuf, int size);

void network_send_packet(void *ubuffer, int len);

// Handling requests.
int do_ipv4 ( unsigned long buffer );
int do_ipv6 ( unsigned long buffer );
int do_arp ( unsigned long buffer );

*/

#endif    



//
// End.
//

