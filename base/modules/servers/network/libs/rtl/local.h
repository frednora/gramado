//criado por compatibilidade com bsd,


#include <sys/types.h>
#include <sys/cdefs.h>

#include "fileext.h"

/*
 * Test whether the given stdio file has an active ungetc buffer;
 * release such a buffer, without restoring ordinary unread data.
 */
#define	HASUB(fp) (_UB(fp)._base != NULL)
#define	FREEUB(fp) { \
	if (_UB(fp)._base != (fp)->_ubuf) \
		free((char *)_UB(fp)._base); \
	_UB(fp)._base = NULL; \
}

/*
 * test for an fgetln() buffer.
 */
#define	FREELB(fp) { \
	free(_EXT(fp)->_fgetstr_buf); \
	_EXT(fp)->_fgetstr_buf = NULL; \
	_EXT(fp)->_fgetstr_len = 0; \
}



/*
 * Detect if the current file position fits in a long int.
 */

static __inline bool
__long_overflow(off_t pos)
{
	return (pos < LONG_MIN) || (pos > LONG_MAX);
}


