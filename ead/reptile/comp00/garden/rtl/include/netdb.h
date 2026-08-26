/*
 * netdb.h
 * 
 */

// Functions for translating protocol names and 
// host names into numeric addresses. 
// Searches local data as well as name services.
// See:
// https://en.wikipedia.org/wiki/Berkeley_sockets


#ifndef _NETDB_H_
#define  _NETDB_H_


/*
 * Data types
 */


//#ifndef socklen_t
//typedef __socklen_t	socklen_t;
//#define	socklen_t	__socklen_t
//#endif


#ifdef  _BSD_SIZE_T_
typedef  _BSD_SIZE_T_  size_t;
#undef  _BSD_SIZE_T_
#endif



#if defined(_NETBSD_SOURCE)
#ifndef _PATH_HEQUIV
#define _PATH_HEQUIV  "/etc/hosts.equiv"
#endif
#ifndef _PATH_HOSTS
#define _PATH_HOSTS  "/etc/hosts"
#endif
#ifndef _PATH_NETWORKS
#define _PATH_NETWORKS  "/etc/networks"
#endif
#ifndef _PATH_PROTOCOLS
#define _PATH_PROTOCOLS  "/etc/protocols"
#endif
#ifndef _PATH_SERVICES
#define _PATH_SERVICES  "/etc/services"
#endif
#ifndef _PATH_SERVICES_CDB
#define _PATH_SERVICES_CDB  "/var/db/services.cdb"
#endif
#ifndef _PATH_SERVICES_DB
#define _PATH_SERVICES_DB  "/var/db/services.db"
#endif
#endif



struct servent 
{
    char   *s_name;     /*%< official service name */
    char  **s_aliases;  /*%< alias list */
    int     s_port;     /*%< port # */
    char   *s_proto;    /*%< protocol to use */
};


struct protoent 
{
    char   *p_name;     /*%< official protocol name */
    char  **p_aliases;  /*%< alias list */
    int     p_proto;    /*%< protocol # */
};


//extern int h_errno;

/*%
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */

#if defined(_NETBSD_SOURCE)
#define  NETDB_INTERNAL  -1  /*%< see errno */
#define  NETDB_SUCCESS   0   /*%< no problem */
#endif

//#define	NO_ADDRESS	NO_DATA		/* no address, look for MX record */
#define  HOST_NOT_FOUND  1  /*%< Authoritative Answer Host not found */
#define  TRY_AGAIN       2  /*%< Non-Authoritive Host not found, or SERVERFAIL */
#define  NO_RECOVERY     3  /*%< Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define  NO_DATA         4  /*%< Valid name, no data record of requested type */


#if defined(_NETBSD_SOURCE)
//#define	NO_ADDRESS	NO_DATA		/*%< no address, look for MX record */
#endif



/*
 * Error return codes from getaddrinfo()
 */
#if (_POSIX_C_SOURCE - 0) >= 200112L || (_XOPEN_SOURCE - 0) >= 520 || \
    defined(_NETBSD_SOURCE)
#define	EAI_ADDRFAMILY	 1	/*%< address family for hostname not supported */
#define	EAI_AGAIN	 2	/*%< temporary failure in name resolution */
#define	EAI_BADFLAGS	 3	/*%< invalid value for ai_flags */
#define	EAI_FAIL	 4	/*%< non-recoverable failure in name resolution */
#define	EAI_FAMILY	 5	/*%< ai_family not supported */
#define	EAI_MEMORY	 6	/*%< memory allocation failure */
#define	EAI_NODATA	 7	/*%< no address associated with hostname */
#define	EAI_NONAME	 8	/*%< hostname nor servname provided, or not known */
#define	EAI_SERVICE	 9	/*%< servname not supported for ai_socktype */
#define	EAI_SOCKTYPE	10	/*%< ai_socktype not supported */
#define	EAI_SYSTEM	11	/*%< system error returned in errno */
#define	EAI_BADHINTS	12	/* invalid value for hints */
#define	EAI_PROTOCOL	13	/* resolved protocol is unknown */
#define	EAI_OVERFLOW	14	/* argument buffer overflow */
#define	EAI_MAX		15
#endif /* _POSIX_C_SOURCE >= 200112 || _XOPEN_SOURCE >= 520 || _NETBSD_SOURCE */


/*%
 * Flag values for getaddrinfo()
 */
#if (_POSIX_C_SOURCE - 0) >= 200112L || (_XOPEN_SOURCE - 0) >= 520 || \
    defined(_NETBSD_SOURCE)
#define	AI_PASSIVE	0x00000001 /* get address to use bind() */
#define	AI_CANONNAME	0x00000002 /* fill ai_canonname */
#define	AI_NUMERICHOST	0x00000004 /* prevent host name resolution */
#define	AI_NUMERICSERV	0x00000008 /* prevent service name resolution */
#define	AI_ADDRCONFIG	0x00000400 /* only if any address is assigned */
/* valid flags for addrinfo (not a standard def, apps should not use it) */
#ifdef _NETBSD_SOURCE
#define	AI_SRV		0x00000800 /* do _srv lookups */
#define	AI_MASK	\
    (AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV | \
    AI_ADDRCONFIG | AI_SRV)
#else
#define	AI_MASK	\
    (AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV | \
    AI_ADDRCONFIG)
#endif
#endif


#if (_POSIX_C_SOURCE - 0) >= 200112L || (_XOPEN_SOURCE - 0) >= 520 || \
    defined(_NETBSD_SOURCE)
/*%
 * Constants for getnameinfo()
 */
#if defined(_NETBSD_SOURCE)
#define	NI_MAXHOST	1025
#define	NI_MAXSERV	32
#endif


/*%
 * Flag values for getnameinfo()
 */
#define	NI_NOFQDN        0x00000001
#define	NI_NUMERICHOST   0x00000002
#define	NI_NAMEREQD      0x00000004
#define	NI_NUMERICSERV   0x00000008
#define	NI_DGRAM         0x00000010
#define	NI_WITHSCOPEID   0x00000020
#define	NI_NUMERICSCOPE  0x00000040


/*%
 * Scope delimit character
 */
#if defined(_NETBSD_SOURCE)
#define SCOPE_DELIMITER	'%'
#endif
#endif /* (_POSIX_C_SOURCE - 0) >= 200112L || ... */

/*
//#todo
struct hostent *gethostbyname(const char *name);
struct hostent *gethostbyname(const char *name)
{
    debug_print("gethostbyname: todo\n");
    return (struct hostent *) 0;
}
*/

#endif /* !_NETDB_H_ */


