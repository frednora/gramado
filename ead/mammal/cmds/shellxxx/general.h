/* 
 general.h 
 -- defines that everybody likes to use. 
 */

#ifndef _GENERAL_
#define _GENERAL_

//#ifndef NULL
//#define NULL 0x0
//#endif

#ifndef savestring
#define savestring(x) (char *) strcpy (xmalloc (1 + strlen (x)), (x))
#endif

#ifndef whitespace
#define whitespace(c) (((c) == ' ') || ((c) == '\t'))
#endif

#ifndef digit
#define digit(c)  ((c) >= '0' && (c) <= '9')
#endif

#ifndef isletter
#define isletter(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#endif

#ifndef digit_value
#define digit_value(c) ((c) - '0')
#endif

//#ifndef member
//char *index ();
//#define member(c, s) (int)((c) ? index ((s), (c)) : 0)
//#endif

/* Function pointers can be declared as (Function *)foo. */
//#ifndef __FUNCTION_DEF
//typedef int Function ();
//#define __FUNCTION_DEF
//#endif

/* The output of `signal' is different on different systems.  Yechh. */
//#if defined(SunOS4) || defined (NeXT) || defined (Ultrix)
//#define sighandler void
//#else
//#define sighandler int
//#endif

//typedef sighandler SigHandler ();

#endif  /* _GENERAL_ */
