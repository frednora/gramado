
// netgroup.h

#ifndef _NETGROUP_H_
#define	_NETGROUP_H_

#define	_PATH_NETGROUP       "/etc/netgroup"
#define	_PATH_NETGROUP_DB    "/var/db/netgroup.db"
#define	_PATH_NETGROUP_MKDB  "/usr/sbin/netgroup_mkdb"


#define	_NG_KEYBYNAME  '1'  /* stored by name */
#define	_NG_KEYBYUSER  '2'  /* stored by user */
#define	_NG_KEYBYHOST  '3'  /* stored by host */


#define _NG_ERROR	-1
#define _NG_NONE	 0
#define _NG_NAME	 1
#define _NG_GROUP	 2


//bsd-like
//struct netgroup {
//	__aconst char *ng_host;		/* host name */
//	__aconst char *ng_user;		/* user name */
//	__aconst char *ng_domain;	/* domain name */
//	struct	netgroup *ng_next;	/* thread */
//};

#endif /* !_NETGROUP_H_ */


