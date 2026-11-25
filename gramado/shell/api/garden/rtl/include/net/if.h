
// File: if.h
// todo

// #todo
// Tem mais coisa no arquivo if.h See: openbsd.



#include <sys/cdefs.h>


/*
 * Structure used to query names of interface cloners.
 */

struct if_clonereq {

    int    ifcr_total;   /* total cloners (out) */
    int    ifcr_count;   /* room for this many in user buffer */
    char  *ifcr_buffer;  /* buffer for cloner names */
};


#define MCLPOOLS  8  /* number of cluster pools */


struct if_rxring {

    int    rxr_adjusted;
    u_int  rxr_alive;
    u_int  rxr_cwm;
    u_int  rxr_lwm;
    u_int  rxr_hwm;
};


struct if_rxring_info {

    char   ifr_name[16];  /* name of the ring */
    u_int  ifr_size;      /* size of the packets on the ring */
    
    struct if_rxring ifr_info;
};


/* Structure used in SIOCGIFRXR request. */
struct if_rxrinfo {

    u_int  ifri_total;
    struct if_rxring_info *ifri_entries;
};


/*
 * Length of interface external name, including terminating '\0'.
 * Note: this is the same size as a generic device's external name.
 */
#define  IF_NAMESIZE    16


struct if_nameindex 
{
    unsigned int  if_index;
    char         *if_name;
};



/*
 * Structure defining statistics and 
 * other data kept regarding a network interface.
 */

struct if_data 
{

	/* generic interface information */
	u_char  ifi_type;		/* ethernet, tokenring, etc. */
	u_char  ifi_addrlen;		/* media address length */
	u_char  ifi_hdrlen;		/* media header length */
	u_char  ifi_link_state;		/* current link state */
	//u_int32_t	ifi_mtu;		/* maximum transmission unit */
	//u_int32_t	ifi_metric;		/* routing metric (external only) */
	//u_int32_t	ifi_rdomain;		/* routing instance */


	//
	// #bugbug #todo   64bit ??
	//
	
	//u_int64_t	ifi_baudrate;		/* linespeed */
	/* volatile statistics */
	//u_int64_t	ifi_ipackets;		/* packets received on interface */
	//u_int64_t	ifi_ierrors;		/* input errors on interface */
	//u_int64_t	ifi_opackets;		/* packets sent on interface */
	//u_int64_t	ifi_oerrors;		/* output errors on interface */
	//u_int64_t	ifi_collisions;		/* collisions on csma interfaces */
	//u_int64_t	ifi_ibytes;		/* total number of octets received */
	//u_int64_t	ifi_obytes;		/* total number of octets sent */
	//u_int64_t	ifi_imcasts;		/* packets received via multicast */
	//u_int64_t	ifi_omcasts;		/* packets sent via multicast */
	//u_int64_t	ifi_iqdrops;		/* dropped on input, this interface */
	//u_int64_t	ifi_oqdrops;		/* dropped on output, this interface */
	//u_int64_t	ifi_noproto;		/* destined for unsupported protocol */
	
	//u_int32_t	ifi_capabilities;	/* interface capabilities */
	//struct	timeval ifi_lastchange;	/* last operational state change */
};



#define IFQ_NQUEUES	8
#define IFQ_MINPRIO	0
#define IFQ_MAXPRIO	IFQ_NQUEUES - 1
#define IFQ_DEFPRIO	3
#define IFQ_PRIO2TOS(_p) ((_p) << 5)
#define IFQ_TOS2PRIO(_t) ((_t) >> 5)

/*
 * Values for if_link_state.
 */
#define LINK_STATE_UNKNOWN	0	/* link unknown */
#define LINK_STATE_INVALID	1	/* link invalid */
#define LINK_STATE_DOWN		2	/* link is down */
#define LINK_STATE_KALIVE_DOWN	3	/* keepalive reports down */
#define LINK_STATE_UP		4	/* link is up */
#define LINK_STATE_HALF_DUPLEX	5	/* link is up and half duplex */
#define LINK_STATE_FULL_DUPLEX	6	/* link is up and full duplex */

#define LINK_STATE_IS_UP(_s)	\
		((_s) >= LINK_STATE_UP || (_s) == LINK_STATE_UNKNOWN)
		
		
		
		
/*
 * Status bit descriptions for the various interface types.
 */
struct if_status_description 
{
	u_char      ifs_type;
	u_char      ifs_state;
	const char *ifs_string;
};


#define LINK_STATE_DESC_MATCH(_ifs, _t, _s)				\
	(((_ifs)->ifs_type == (_t) || (_ifs)->ifs_type == 0) &&		\
	    (_ifs)->ifs_state == (_s))
	    
	    
	    
	    
#define LINK_STATE_DESCRIPTIONS {					\
	{ IFT_ETHER, LINK_STATE_DOWN, "no carrier" },			\
									\
	{ IFT_IEEE80211, LINK_STATE_DOWN, "no network" },		\
									\
	{ IFT_PPP, LINK_STATE_DOWN, "no carrier" },			\
									\
	{ IFT_CARP, LINK_STATE_DOWN, "backup" },			\
	{ IFT_CARP, LINK_STATE_UP, "master" },				\
	{ IFT_CARP, LINK_STATE_HALF_DUPLEX, "master" },			\
	{ IFT_CARP, LINK_STATE_FULL_DUPLEX, "master" },			\
									\
	{ 0, LINK_STATE_UP, "active" },					\
	{ 0, LINK_STATE_HALF_DUPLEX, "active" },			\
	{ 0, LINK_STATE_FULL_DUPLEX, "active" },			\
									\
	{ 0, LINK_STATE_UNKNOWN, "unknown" },				\
	{ 0, LINK_STATE_INVALID, "invalid" },				\
	{ 0, LINK_STATE_DOWN, "down" },					\
	{ 0, LINK_STATE_KALIVE_DOWN, "keepalive down" },		\
	{ 0, 0, NULL }							\
}

/* Traditional BSD name for length of interface external name. */
#define	IFNAMSIZ	IF_NAMESIZE

/*
 * Length of interface description, including terminating '\0'.
 */
#define	IFDESCRSIZE	64


/*
 * Interface flags can be either owned by the stack or the driver.  The
 * symbols below document who is toggling which flag.
 *
 *	I	immutable after creation
 *	N	written by the stack (upon user request)
 *	d	written by the driver
 *	c	for userland compatibility only
 */
#define	IFF_UP		0x1		/* [N] interface is up */
#define	IFF_BROADCAST	0x2		/* [I] broadcast address valid */
#define	IFF_DEBUG	0x4		/* [N] turn on debugging */
#define	IFF_LOOPBACK	0x8		/* [I] is a loopback net */
#define	IFF_POINTOPOINT	0x10		/* [I] is point-to-point link */
#define	IFF_STATICARP	0x20		/* [N] only static ARP */
#define	IFF_RUNNING	0x40		/* [d] resources allocated */
#define	IFF_NOARP	0x80		/* [N] no address resolution protocol */
#define	IFF_PROMISC	0x100		/* [N] receive all packets */
#define	IFF_ALLMULTI	0x200		/* [d] receive all multicast packets */
#define	IFF_OACTIVE	0x400		/* [c] transmission in progress */
#define	IFF_SIMPLEX	0x800		/* [I] can't hear own transmissions */
#define	IFF_LINK0	0x1000		/* [N] per link layer defined bit */
#define	IFF_LINK1	0x2000		/* [N] per link layer defined bit */
#define	IFF_LINK2	0x4000		/* [N] per link layer defined bit */
#define	IFF_MULTICAST	0x8000		/* [I] supports multicast */


/* flags set internally only: */
#define	IFF_CANTCHANGE \
	(IFF_BROADCAST|IFF_POINTOPOINT|IFF_RUNNING|IFF_OACTIVE|\
	    IFF_SIMPLEX|IFF_MULTICAST|IFF_ALLMULTI)

#define	IFXF_MPSAFE		0x1	/* [I] if_start is mpsafe */
#define	IFXF_CLONED		0x2	/* [I] pseudo interface */
#define	IFXF_INET6_NOPRIVACY	0x4	/* [N] don't autoconf privacy */
#define	IFXF_MPLS		0x8	/* [N] supports MPLS */
#define	IFXF_WOL		0x10	/* [N] wake on lan enabled */
#define	IFXF_AUTOCONF6		0x20	/* [N] v6 autoconf enabled */
#define IFXF_INET6_NOSOII	0x40	/* [N] don't do RFC 7217 */
#define	IFXF_AUTOCONF4		0x80	/* [N] v4 autoconf (aka dhcp) enabled */

#define	IFXF_CANTCHANGE \
	(IFXF_MPSAFE|IFXF_CLONED)
	
/*
 * Some convenience macros used for setting ifi_baudrate.
 */
#define	IF_Kbps(x)	((x) * 1000ULL)			/* kilobits/sec. */
#define	IF_Mbps(x)	(IF_Kbps((x) * 1000ULL))	/* megabits/sec. */
#define	IF_Gbps(x)	(IF_Mbps((x) * 1000ULL))	/* gigabits/sec. */

/* Capabilities that interfaces can advertise. */
#define	IFCAP_CSUM_IPv4		0x00000001	/* can do IPv4 header csum */
#define	IFCAP_CSUM_TCPv4	0x00000002	/* can do IPv4/TCP csum */
#define	IFCAP_CSUM_UDPv4	0x00000004	/* can do IPv4/UDP csum */
#define	IFCAP_VLAN_MTU		0x00000010	/* VLAN-compatible MTU */
#define	IFCAP_VLAN_HWTAGGING	0x00000020	/* hardware VLAN tag support */
#define	IFCAP_CSUM_TCPv6	0x00000080	/* can do IPv6/TCP checksums */
#define	IFCAP_CSUM_UDPv6	0x00000100	/* can do IPv6/UDP checksums */
#define	IFCAP_WOL		0x00008000	/* can do wake on lan */

#define IFCAP_CSUM_MASK		(IFCAP_CSUM_IPv4 | IFCAP_CSUM_TCPv4 | \
    IFCAP_CSUM_UDPv4 | IFCAP_CSUM_TCPv6 | IFCAP_CSUM_UDPv6)

/* symbolic names for terminal (per-protocol) CTL_IFQ_ nodes */
#define IFQCTL_LEN 1
#define IFQCTL_MAXLEN 2
#define IFQCTL_DROPS 3
#define IFQCTL_CONGESTION 4
#define IFQCTL_MAXID 5


/* sysctl for ifq (per-protocol packet input queue variant of ifqueue) */
#define CTL_IFQ_NAMES  { \
	{ 0, 0 }, \
	{ "len", CTLTYPE_INT }, \
	{ "maxlen", CTLTYPE_INT }, \
	{ "drops", CTLTYPE_INT }, \
	{ "congestion", CTLTYPE_INT }, \
}


/*
 * Message format for use in obtaining information about interfaces
 * from sysctl and the routing socket.
 */
struct if_msghdr {
	u_short	ifm_msglen;	/* to skip over non-understood messages */
	u_char	ifm_version;	/* future binary compatibility */
	u_char	ifm_type;	/* message type */
	u_short ifm_hdrlen;	/* sizeof(if_msghdr) to skip over the header */
	u_short	ifm_index;	/* index for associated ifp */
	u_short	ifm_tableid;	/* routing table id */
	u_char	ifm_pad1;
	u_char	ifm_pad2;
	int	ifm_addrs;	/* like rtm_addrs */
	int	ifm_flags;	/* value of if_flags */
	int	ifm_xflags;
	
	//#todo
	//struct	if_data ifm_data;  /* statistics and other data about if */
};

/*
 * Message format for use in obtaining information about interface addresses
 * from sysctl and the routing socket.
 */
struct ifa_msghdr {
	u_short	ifam_msglen;	/* to skip over non-understood messages */
	u_char	ifam_version;	/* future binary compatibility */
	u_char	ifam_type;	/* message type */
	u_short ifam_hdrlen;	/* sizeof(ifa_msghdr) to skip over the header */
	u_short	ifam_index;	/* index for associated ifp */
	u_short	ifam_tableid;	/* routing table id */
	u_char	ifam_pad1;
	u_char	ifam_pad2;
	int	ifam_addrs;	/* like rtm_addrs */
	int	ifam_flags;	/* value of ifa_flags */
	int	ifam_metric;	/* value of ifa_metric */
};


/*
 * Message format announcing the arrival or departure of a network interface.
 */
struct if_announcemsghdr {
	u_short	ifan_msglen;	/* to skip over non-understood messages */
	u_char	ifan_version;	/* future binary compatibility */
	u_char	ifan_type;	/* message type */
	u_short ifan_hdrlen;	/* sizeof(if_announcemsghdr) to skip header */
	u_short	ifan_index;	/* index for associated ifp */
	u_short	ifan_what;	/* what type of announcement */
	char	ifan_name[IFNAMSIZ];	/* if name, e.g. "en0" */
};

#define IFAN_ARRIVAL	0	/* interface arrival */
#define IFAN_DEPARTURE	1	/* interface departure */


// #todo
// Tem mais coisa no arquivo if.h See: openbsd.


/* message format used to pass interface name to index mappings */
struct if_nameindex_msg {
	unsigned int	if_index;
	char		if_name[IFNAMSIZ];
};


/*
 * interface groups
 */

#define IFG_ALL     "all"     /* group contains all interfaces */
#define IFG_EGRESS  "egress"  /* if(s) default route(s) point to */

struct ifg_req {
	union {
		char			 ifgrqu_group[IFNAMSIZ];
		char			 ifgrqu_member[IFNAMSIZ];
	} ifgrq_ifgrqu;
#define	ifgrq_group	ifgrq_ifgrqu.ifgrqu_group
#define	ifgrq_member	ifgrq_ifgrqu.ifgrqu_member
};

struct ifg_attrib {
	int	ifg_carp_demoted;
};

/*
 * Used to lookup groups for an interface
 */
struct ifgroupreq {
	char	ifgr_name[IFNAMSIZ];
	u_int	ifgr_len;
	union {
		char			 ifgru_group[IFNAMSIZ];
		struct	ifg_req		*ifgru_groups;
		struct	ifg_attrib	 ifgru_attrib;
	} ifgr_ifgru;
#define ifgr_group	ifgr_ifgru.ifgru_group
#define ifgr_groups	ifgr_ifgru.ifgru_groups
#define ifgr_attrib	ifgr_ifgru.ifgru_attrib
};


// #todo
// Tem mais coisa no arquivo if.h See: openbsd.


#define IF_HDRPRIO_MIN		IFQ_MINPRIO
#define IF_HDRPRIO_MAX		IFQ_MAXPRIO
#define IF_HDRPRIO_PACKET	-1	/* use mbuf prio */
#define IF_HDRPRIO_PAYLOAD	-2	/* copy payload prio */
#define IF_HDRPRIO_OUTER	-3	/* use outer prio */

#define IF_PWE3_ETHERNET	1	/* ethernet or ethernet tagged */
#define IF_PWE3_IP		2	/* IP layer 2 */



