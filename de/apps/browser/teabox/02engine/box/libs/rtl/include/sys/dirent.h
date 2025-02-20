


#include <sys/cdefs.h>
#include <sys/types.h>

/*
 * The dirent structure defines the format of directory entries returned by
 * the getdents(2) system call.
 *
 * A directory entry has a struct dirent at the front of it, containing its
 * inode number, the length of the entry, and the length of the name
 * contained in the entry.  These are followed by the name padded to 
 * _DIRENT_ALIGN() byte boundary with null bytes.  All names are guaranteed
 * NUL terminated.  The maximum length of a name in a directory is MAXNAMLEN.
 */
struct dirent {
	
	ino_t d_fileno;			/* file number of entry */
	uint16_t d_reclen;		/* length of this record */
	uint16_t d_namlen;		/* length of string in d_name */
	uint8_t  d_type; 		/* file type, see below */

#if defined(_NETBSD_SOURCE)
    #define	MAXNAMLEN	511		/* must be kept in sync with NAME_MAX */
    char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
#else
    char	d_name[511 + 1];	/* name must be no longer than this */
#endif

};

//? incompleta?
typedef struct DIR {
	int fd;
	int cur_entry;
} DIR;


/*
 * File types
 */

/*
#define	DT_UNKNOWN	 0
#define	DT_FIFO		 1
#define	DT_CHR		 2
#define	DT_DIR		 4
#define	DT_BLK		 6
#define	DT_REG		 8
#define	DT_LNK		10
#define	DT_SOCK		12
#define	DT_WHT		14 
 */


/*
 * The _DIRENT_ALIGN macro returns the alignment of struct dirent.
 * struct direct and struct dirent12 used 4 byte alignment but
 * struct dirent uses 8.
 */
#define _DIRENT_ALIGN(dp) (sizeof((dp)->d_fileno) - 1)
