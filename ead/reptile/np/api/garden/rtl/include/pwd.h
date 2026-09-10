//password

//esse arquivo está aqui por compatibilidade 

/* The <pwd.h> header defines the items in the password file. */

#ifndef _PWD_H
#define _PWD_H


#if defined(_NETBSD_SOURCE) || defined(HAVE_NBTOOL_CONFIG_H)
#define	_PATH_PASSWD		"/etc/passwd"
#define	_PATH_MASTERPASSWD	"/etc/master.passwd"
#define	_PATH_MASTERPASSWD_LOCK	"/etc/ptmp"

#define	_PATH_PASSWD_CONF	"/etc/passwd.conf"
#define	_PATH_PASSWDCONF	_PATH_PASSWD_CONF	/* XXX: compat */
#define	_PATH_USERMGMT_CONF	"/etc/usermgmt.conf"

#define	_PATH_MP_DB		"/etc/pwd.db"
#define	_PATH_SMP_DB		"/etc/spwd.db"

#define	_PATH_PWD_MKDB		"/usr/sbin/pwd_mkdb"

#define	_PW_KEYBYNAME		'1'	/* stored by name */
#define	_PW_KEYBYNUM		'2'	/* stored by entry in the "file" */
#define	_PW_KEYBYUID		'3'	/* stored by uid */

#define	_PASSWORD_EFMT1		'_'	/* extended DES encryption format */
#define	_PASSWORD_NONDES	'$'	/* non-DES encryption formats */

#define	_PASSWORD_LEN		128	/* max length, not counting NUL */

#define _PASSWORD_NOUID		0x01	/* flag for no specified uid. */
#define _PASSWORD_NOGID		0x02	/* flag for no specified gid. */
#define _PASSWORD_NOCHG		0x04	/* flag for no specified change. */
#define _PASSWORD_NOEXP		0x08	/* flag for no specified expire. */

#define _PASSWORD_OLDFMT	0x10	/* flag to expect an old style entry */
#define _PASSWORD_NOWARN	0x20	/* no warnings for bad entries */

#define _PASSWORD_WARNDAYS	14	/* days to warn about expiry */
#define _PASSWORD_CHGNOW	-1	/* special day to force password
					 * change at next login */
#endif


//bsd
//struct passwd {
//	__aconst char *pw_name;		/* user name */
//	__aconst char *pw_passwd;	/* encrypted password */
//	uid_t	       pw_uid;		/* user uid */
//	gid_t	       pw_gid;		/* user gid */
//	time_t	       pw_change;	/* password change time */
//	__aconst char *pw_class;	/* user login class */
//	__aconst char *pw_gecos;	/* general information */
//	__aconst char *pw_dir;		/* home directory */
//	__aconst char *pw_shell;	/* default shell */
//	time_t 	       pw_expire;	/* account expiration */
//};


//#posix
//#todo: incomplete
struct passwd {
  char *pw_name;		// login name /
  //uid_t pw_uid;			// uid corresponding to the name 
  //gid_t pw_gid;			// gid corresponding to the name 
  char *pw_dir;			// user's home directory 
  char *pw_shell;		// name of the user's shell 
   //??
//...
};




#endif /* _PWD_H */

