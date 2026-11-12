
#ifndef _GLOB_H_
#define _GLOB_H_


/*
#ifndef __gl_size_t
#define __gl_size_t	size_t
#endif
#ifndef __gl_stat_t
#define __gl_stat_t	struct stat
#endif

//#todo: The glob_t struct.
*/


#define	GLOB_APPEND	0x0001	/* Append to output from previous call. */
#define	GLOB_DOOFFS	0x0002	/* Use gl_offs. */
#define	GLOB_ERR	0x0004	/* Return on error. */
#define	GLOB_MARK	0x0008	/* Append / to matching directories. */
#define	GLOB_NOCHECK	0x0010	/* Return pattern itself if nothing matches. */
#define	GLOB_NOSORT	0x0020	/* Don't sort. */
#define	GLOB_NOESCAPE	0x1000	/* Disable backslash escaping. */

#define	GLOB_NOSPACE	(-1)	/* Malloc call failed. */
#define	GLOB_ABORTED	(-2)	/* Unignored error. */
#define	GLOB_NOMATCH	(-3)	/* No match, and GLOB_NOCHECK was not set. */
#define	GLOB_NOSYS	(-4)	/* Implementation does not support function. */

#if defined(_NETBSD_SOURCE) || defined(HAVE_NBTOOL_CONFIG_H)
#define	GLOB_ALTDIRFUNC	0x0040	/* Use alternately specified directory funcs. */
#define	GLOB_BRACE	0x0080	/* Expand braces ala csh. */
#define	GLOB_MAGCHAR	0x0100	/* Pattern had globbing characters. */
#define	GLOB_NOMAGIC	0x0200	/* GLOB_NOCHECK without magic chars (csh). */
#define	GLOB_LIMIT	0x0400	/* Limit memory used by matches to ARG_MAX */
#define	GLOB_TILDE	0x0800	/* Expand tilde names from the passwd file. */
/*	GLOB_NOESCAPE	0x1000	above */
#define	GLOB_PERIOD	0x2000	/* Allow metachars to match leading periods. */
#define	GLOB_NO_DOTDIRS	0x4000	/* Make . and .. vanish from wildcards. */
#define	GLOB_STAR	0x8000	/* Use glob ** to recurse directories */
#define	GLOB_QUOTE	0	/* source compatibility */

#define	GLOB_ABEND	GLOB_ABORTED	/* source compatibility */
#endif


#endif /* !_GLOB_H_ */



