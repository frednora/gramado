/*
 * File: in.h 
 *     ring0. socket address for internet style.
 */

#ifndef ____IN_H
#define ____IN_H

//#test
typedef __uint32_t	__in_addr_t;	/* IP(v4) address */
typedef __uint16_t	__in_port_t;	/* "Internet" port number */


#ifndef in_addr_t
typedef __in_addr_t	in_addr_t;
#define	in_addr_t	__in_addr_t
#endif

#ifndef in_port_t
typedef __in_port_t	in_port_t;
#define	in_port_t	__in_port_t
#endif


typedef __uint8_t  __sa_family_t;  /* socket address family */


// #bugbug: 
// It needs to be 16bit.
#ifndef sa_family_t
typedef __sa_family_t  sa_family_t;
#define sa_family_t  __sa_family_t
#endif



/*
 * Internet address (a structure for historical reasons)
 */
 
 
//#todo: checar a tipagem dos elementos 


struct in_addr {
    
    in_addr_t  s_addr;
} __packed;

 
 
//#todo: checar a tipagem dos elementos  
/*
 * Socket address, internet style.
 */


struct sockaddr_in {
    
    // len + family = 16bit
    uint8_t           sin_len;     // 8
    sa_family_t       sin_family;  // 8 

    in_port_t         sin_port;
    struct in_addr    sin_addr;
    
    __int8_t          sin_zero[8];
};



#endif    



