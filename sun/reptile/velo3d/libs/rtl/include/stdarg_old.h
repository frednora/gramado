
// stdarg.h 
// status: Criando uma versão simplificada, que funcione no sistema 
// operacional Gramado, na arquitetura x86.
// Fred Nora


//todo:
//va_start	Start iterating arguments with a va_list C89
//va_arg	Retrieve an argument C89 (recuperar argumento)
//va_end	Free a va_list C89
//va_copy	Copy contents of one va_list to another C99


//cria um tipo 'char *', usado em strings.


typedef char *va_list;

/*
#define va_list   VA_LIST
#define va_start    VA_START
#define va_arg        VA_ARG
#define va_end              VA_END
#define va_copy(s,d)      (s) = (d)
#define __va_copy       va_copy
*/

#ifdef __GRAMADOC__
typedef char *gramado_va_list;
#endif


/*
//#test
#ifndef __GNUC__
typedef va_list __gnuc_va_list;
#define va_start    __builtin_va_start
#define va_arg        __builtin_va_arg
typedef __gnuc_va_list __builtin_va_list;
#define va_end              __builtin_va_end
#define va_copy         __builtin_va_copy
#endif
*/


/*
Data Type: 
va_list
The type va_list is used for argument pointer variables.

Macro: 
void va_start (va_list ap, last-required)
This macro initializes the argument pointer variable ap to point 
to the first of the optional arguments of the current function; 
last-required must be the last required argument to the function.
See section Old-Style Variadic Functions, for an alternate 
definition of va_start found in the header file `varargs.h'.

Macro: 
type va_arg (va_list ap, type)
The va_arg macro returns the value of the next optional argument, 
and modifies the value of ap to point to the subsequent argument. 
Thus, successive uses of va_arg return successive optional arguments.
The type of the value returned by va_arg is type as specified in the call. 
type must be a self-promoting type 
(not char or short int or float) that matches the type of the actual argument.

Macro: 
void va_end (va_list ap)
This ends the use of ap. After a va_end call, 
further va_arg calls with the same ap may not work. 
You should invoke va_end before returning from the 
function in which va_start was invoked with the same ap argument.
*/



//rotina de suporte.
#define __va_rounded_size(TYPE)  \
    ( ( ( sizeof(TYPE) + sizeof(int) - 1) / sizeof(int) ) * sizeof(int) )

	
/* rotina de suporte. (obs: igual a anterior)
 * Amount of space required in an argument list (ie. the stack) for an
 * argument of type t. */
 
#define __va_argsiz(t)	\
	(((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))
	

/* For a simple minded compiler this should work 
 * (it works in GNU too for vararg lists that don't follow shorts and such). */
 
#define va_start(ap, pN)  \
	( (ap) = ( (va_list) (&pN) + __va_argsiz(pN) ) )
	

//#bugbug 
//@todo:
/* Defined in gnulib */
//void va_end(va_list);		
//#define va_end(AP)

/*
 * #bugbug
 * End processing of variable argument list. 
 * In this case we do nothing.
 */
#define va_end(ap)	( (void) 0 )


/*
 ***************************************
 * ## va_arg ##
 *
 * Increment ap to the next argument in the list while returing a
 * pointer to what ap pointed to first, which is of type t.
 *
 * We cast to void* and then to t* because this avoids a warning about
 * increasing the alignment requirement.
 */

#define va_arg( ap, t )  \
    ( ( (ap) = (ap) + __va_argsiz(t) ),  \
    *( (t *) (void *) ( (ap) - __va_argsiz(t) ) ) )


/*
#define va_arg( AP, TYPE )  \
    ( AP += __va_rounded_size(TYPE), \
    *((TYPE *) (AP - __va_rounded_size (TYPE))) )
*/

/*
void __va_end(va_list* ap);
#define va_end(ap) \
    (__va_end(&(ap)))
*/

  
/*
typedef char *va_list;
#define va_start(ap,parmn) (void)((ap) = (char*)(&(parmn) + 1))
#define va_end(ap) (void)((ap) = 0)
#define va_arg(ap, type) \
    (((type*)((ap) = ((ap) + sizeof(type))))[-1])
 */ 
  


