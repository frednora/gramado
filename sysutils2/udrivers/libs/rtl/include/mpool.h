#ifndef _MPOOL_H_
#define _MPOOL_H_


/*
 * The memory pool scheme is a simple one.  Each in-memory page is referenced
 * by a bucket which is threaded in up to two of three ways.  All active pages
 * are threaded on a hash chain (hashed by page number) and an lru chain.
 * Inactive pages are threaded on a free chain.  Each reference to a memory
 * pool is handed an opaque MPOOL cookie which stores all of this information.
 */
#define	HASHSIZE	128
#define	HASHKEY(pgno)	((pgno - 1) % HASHSIZE)

//...

/* flags for get/put */
#define	MPOOL_IGNOREPIN		0x01	/* Ignore if the page is pinned. */
/* flags for newf */
#define	MPOOL_PAGE_REQUEST	0x01	/* Allocate a new page with a
					   specific page number. */
#define	MPOOL_PAGE_NEXT		0x02	/* Allocate a new page with the next
					   page number. */


#endif /* _MPOOL_H_ */


