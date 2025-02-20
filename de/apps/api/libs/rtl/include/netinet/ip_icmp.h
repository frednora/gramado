



#ifndef _NETINET_IP_ICMP_H_
#define _NETINET_IP_ICMP_H_



/*
 * Definition of type and code field values.
 */
#define	ICMP_ECHOREPLY		0		/* echo reply */
#define	ICMP_UNREACH		3		/* dest unreachable, codes: */
#define		ICMP_UNREACH_NET	0		/* bad net */
#define		ICMP_UNREACH_HOST	1		/* bad host */
#define		ICMP_UNREACH_PROTOCOL	2		/* bad protocol */
#define		ICMP_UNREACH_PORT	3		/* bad port */
#define		ICMP_UNREACH_NEEDFRAG	4		/* IP_DF caused drop */
#define		ICMP_UNREACH_SRCFAIL	5		/* src route failed */
#define		ICMP_UNREACH_NET_UNKNOWN 6		/* unknown net */
#define		ICMP_UNREACH_HOST_UNKNOWN 7		/* unknown host */
#define		ICMP_UNREACH_ISOLATED	8		/* src host isolated */
#define		ICMP_UNREACH_NET_PROHIB	9		/* prohibited access */
#define		ICMP_UNREACH_HOST_PROHIB 10		/* ditto */
#define		ICMP_UNREACH_TOSNET	11		/* bad tos for net */
#define		ICMP_UNREACH_TOSHOST	12		/* bad tos for host */
#define		ICMP_UNREACH_FILTER_PROHIB 13		/* admin prohib */
#define		ICMP_UNREACH_HOST_PRECEDENCE 14		/* host prec vio. */
#define		ICMP_UNREACH_PRECEDENCE_CUTOFF 15	/* prec cutoff */
#define	ICMP_SOURCEQUENCH	4		/* packet lost, slow down */
#define	ICMP_REDIRECT		5		/* shorter route, codes: */
#define		ICMP_REDIRECT_NET	0		/* for network */
#define		ICMP_REDIRECT_HOST	1		/* for host */
#define		ICMP_REDIRECT_TOSNET	2		/* for tos and net */
#define		ICMP_REDIRECT_TOSHOST	3		/* for tos and host */
#define	ICMP_ALTHOSTADDR	6		/* alternate host address */
#define	ICMP_ECHO		8		/* echo service */
#define	ICMP_ROUTERADVERT	9		/* router advertisement */
#define		ICMP_ROUTERADVERT_NORMAL	 0  /* normal advertisement */
#define		ICMP_ROUTERADVERT_NOROUTE_COMMON 16 /* selective routing */
#define	ICMP_ROUTERSOLICIT	10		/* router solicitation */
#define	ICMP_TIMXCEED		11		/* time exceeded, code: */
#define		ICMP_TIMXCEED_INTRANS	0		/* ttl==0 in transit */
#define		ICMP_TIMXCEED_REASS	1		/* ttl==0 in reass */
#define	ICMP_PARAMPROB		12		/* ip header bad */
#define		ICMP_PARAMPROB_ERRATPTR 0		/* error at param ptr */
#define		ICMP_PARAMPROB_OPTABSENT 1		/* req. opt. absent */
#define		ICMP_PARAMPROB_LENGTH 2			/* bad length */
#define	ICMP_TSTAMP		13		/* timestamp request */
#define	ICMP_TSTAMPREPLY	14		/* timestamp reply */
#define	ICMP_IREQ		15		/* information request */
#define	ICMP_IREQREPLY		16		/* information reply */
#define	ICMP_MASKREQ		17		/* address mask request */
#define	ICMP_MASKREPLY		18		/* address mask reply */
#define	ICMP_TRACEROUTE		30		/* traceroute */
#define	ICMP_DATACONVERR	31		/* data conversion error */
#define	ICMP_MOBILE_REDIRECT	32		/* mobile host redirect */
#define	ICMP_IPV6_WHEREAREYOU	33		/* IPv6 where-are-you */
#define	ICMP_IPV6_IAMHERE	34		/* IPv6 i-am-here */
#define	ICMP_MOBILE_REGREQUEST	35		/* mobile registration req */
#define	ICMP_MOBILE_REGREPLY	36		/* mobile registration reply */
#define	ICMP_SKIP		39		/* SKIP */
#define	ICMP_PHOTURIS		40		/* Photuris */
#define		ICMP_PHOTURIS_UNKNOWN_INDEX	1	/* unknown sec index */
#define		ICMP_PHOTURIS_AUTH_FAILED	2	/* auth failed */
#define		ICMP_PHOTURIS_DECRYPT_FAILED	3	/* decrypt failed */

#define	ICMP_MAXTYPE		40

#define	ICMP_INFOTYPE(type) \
	((type) == ICMP_ECHOREPLY || (type) == ICMP_ECHO || \
	(type) == ICMP_ROUTERADVERT || (type) == ICMP_ROUTERSOLICIT || \
	(type) == ICMP_TSTAMP || (type) == ICMP_TSTAMPREPLY || \
	(type) == ICMP_IREQ || (type) == ICMP_IREQREPLY || \
	(type) == ICMP_MASKREQ || (type) == ICMP_MASKREPLY)

#define ICMP_ERRORTYPE(type) \
	((type) == ICMP_UNREACH || (type) == ICMP_SOURCEQUENCH || \
	(type) == ICMP_REDIRECT || (type) == ICMP_TIMXCEED || \
	(type) == ICMP_PARAMPROB)
	
	
	


#endif 



