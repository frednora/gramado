

   // todo


#ifndef _NETINET_IP_H_
#define _NETINET_IP_H_

/*
 * Definitions for internet protocol version 4.
 * Per RFC 791, September 1981.
 */
#define	IPVERSION	4


/*
 * IP header.
 * Structure of an internet header, naked of options.
 */

struct ip 
{

//#if BYTE_ORDER == LITTLE_ENDIAN
    unsigned int ip_hl:4,		/* header length */
                 ip_v:4;		/* version */
//#endif

//#if BYTE_ORDER == BIG_ENDIAN
//    unsigned int ip_v:4,		/* version */
//                 ip_hl:4;		/* header length */
//#endif

    u_int8_t  ip_tos;		/* type of service */
    u_int16_t ip_len;		/* total length */
    u_int16_t ip_id;		/* identification */
    u_int16_t ip_off;		/* fragment offset field */

#define	IP_RF 0x8000			/* reserved fragment flag */
#define	IP_EF 0x8000			/* evil flag, per RFC 3514 */
#define	IP_DF 0x4000			/* dont fragment flag */
#define	IP_MF 0x2000			/* more fragments flag */
#define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */

    u_int8_t  ip_ttl;		/* time to live */
    u_int8_t  ip_p;			/* protocol */
    u_int16_t ip_sum;		/* checksum */
    
    /* source and dest address */
    struct in_addr ip_src, ip_dst; 

} __packed;

#define IP_MAXPACKET	65535		/* maximum packet size */
#define IP_MINFRAGSIZE	69		/* minumum size that can be fraged */


#endif    



