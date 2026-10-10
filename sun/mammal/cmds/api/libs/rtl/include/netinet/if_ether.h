

#ifndef _NETINET_IF_ETHER_H_
#define _NETINET_IF_ETHER_H_


/*
 * Some basic Ethernet constants.
 */
#define	ETHER_ADDR_LEN	6	/* Ethernet address length		*/
#define ETHER_TYPE_LEN	2	/* Ethernet type field length		*/
#define ETHER_CRC_LEN	4	/* Ethernet CRC length			*/
#define ETHER_HDR_LEN	((ETHER_ADDR_LEN * 2) + ETHER_TYPE_LEN)
#define ETHER_MIN_LEN	64	/* Minimum frame length, CRC included	*/
#define ETHER_MAX_LEN	1518	/* Maximum frame length, CRC included	*/
#define ETHER_MAX_DIX_LEN	1536	/* Maximum DIX frame length	*/

/*
 * Some Ethernet extensions.
 */
#define ETHER_VLAN_ENCAP_LEN	4	/* len of 802.1Q VLAN encapsulation */

/*
 * Mbuf adjust factor to force 32-bit alignment of IP header.
 * Drivers should do m_adj(m, ETHER_ALIGN) when setting up a
 * receive so the upper layers get the IP header properly aligned
 * past the 14-byte Ethernet header.
 */
#define ETHER_ALIGN	2	/* driver adjust for IP hdr alignment */

/*
 * The maximum supported Ethernet length and some space for encapsulation.
 */
#define ETHER_MAX_HARDMTU_LEN	65435


/*
 * Ethernet address - 6 octets
 */

struct ether_addr 
{
    u_int8_t ether_addr_octet[ETHER_ADDR_LEN];
};


/*
 * The length of the combined header.
 */

struct ether_header 
{
    u_int8_t  ether_dhost[ETHER_ADDR_LEN];
    u_int8_t  ether_shost[ETHER_ADDR_LEN];
    u_int16_t ether_type;
};


/*
// Ethernet header length
#define ETHERNET_HEADER_LENGHT 14      

// ethernet header
struct ether_header {
	
	uint8_t dst[6];
	uint8_t src[6];
	uint16_t type;
	
} __attribute__((packed)); 
*/



#endif    


