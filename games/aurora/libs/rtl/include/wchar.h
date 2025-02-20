
//#if defined(_BSD_WCHAR_T_) && !defined(__cplusplus)
//typedef	_BSD_WCHAR_T_	wchar_t;
//#undef	_BSD_WCHAR_T_
//#endif

//#ifdef	_BSD_MBSTATE_T_
//typedef	_BSD_MBSTATE_T_	mbstate_t;
//#undef	_BSD_MBSTATE_T_
//#endif

//#ifdef	_BSD_WINT_T_
//typedef	_BSD_WINT_T_	wint_t;
//#undef	_BSD_WINT_T_
//#endif

#ifdef	_BSD_SIZE_T_
typedef	_BSD_SIZE_T_	size_t;
#undef	_BSD_SIZE_T_
#endif

#if defined(_POSIX_C_SOURCE)
#ifndef __VA_LIST_DECLARED
typedef __va_list va_list;
#define __VA_LIST_DECLARED
#endif
#endif


//#ifndef WEOF
//#define	WEOF 	((wint_t)-1)
//#endif




