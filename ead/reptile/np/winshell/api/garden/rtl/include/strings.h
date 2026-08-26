
//for criado por compatibilidade com gcc 0.9 


#ifndef __STRINGS_H__
#define __STRINGS_H__



#ifdef	_BSD_SIZE_T_
typedef	_BSD_SIZE_T_	size_t;
#undef	_BSD_SIZE_T_
#endif


char *index (const char *s, int c);
char *rindex (const char *s, int c);



char *strrev(char *str);



#endif    //__STRINGS_H__



