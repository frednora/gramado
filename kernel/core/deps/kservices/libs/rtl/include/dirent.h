/*credits: bsd (modificado)*/

#if defined(_NETBSD_SOURCE)

/* definitions for library routines operating on directories. */
#define	DIRBLKSIZ	1024

/* structure describing an open directory. */
struct _dirdesc {
	/*
	 * dd_fd should be kept intact to preserve ABI compat.  see dirfd().
	 */
	int	dd_fd;		/* file descriptor associated with directory */
	/*
	 * the rest is hidden from user.
	 */
	long	dd_loc;		/* offset in current buffer */
	long	dd_size;	/* amount of data returned by getdents */
	char	*dd_buf;	/* data buffer */
	int	dd_len;		/* size of data buffer */
	off_t	dd_seek;	/* magic cookie returned by getdents */
	void	*dd_internal;	/* state for seekdir/telldir */
	int	dd_flags;	/* flags for readdir */
	void	*dd_lock;	/* lock for concurrent access */
};

#define	dirfd(dirp)	((dirp)->dd_fd)

/* flags for __opendir2() */
#define DTF_HIDEW	0x0001	/* hide whiteout entries */
#define DTF_NODUP	0x0002	/* don't return duplicate names */
#define DTF_REWIND	0x0004	/* rewind after reading union stack */
#define __DTF_READALL	0x0008	/* everything has been read */
#define __DTF_RETRY_ON_BADCOOKIE 0x0001	/* retry on EINVAL
					(only valid with __DTF_READALL) */

//#include <sys/null.h>

#endif











