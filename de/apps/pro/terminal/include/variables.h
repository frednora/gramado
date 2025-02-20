/* 
 File: variables.h 
     Data structures for shell variables. 
 History:
    2018 - Ported from bash 1.05.
 */

#ifndef __VARIABLES__
#define __VARIABLES__    1

/* 
 What a shell variable looks like. 
 Credits: bash 10.5
 */
struct variable_d
{
/* Next variable in the list. */
    struct variable_d *next;
/* Symbol that the user types. */
    char *name;
/* Value that is returned. */
    char *value;
/* Function cell. */
    char *function;
/* export, readonly, array, invisible... */
    int attributes;
/* Which context this variable belongs to. */
    int context;
/* Value from previous context or NULL. */
    struct variable_d *prev_context; 
};
typedef struct variable_d  variable; 
typedef struct variable_d  SHELL_VAR;
typedef struct variable_d  variable_t;

/* 
 The various attributes that a given variable can have.
 We only reserve one byte of the INT. 
 */
/* %00000001 (export to environment) */
/* %00000010 (cannot change)         */
/* %00000100 (cannot see)        */
/* %00001000 (value is an array) */
/* %00010000 (cannot unset)      */
/* %00100000 */

#define att_exported   0x01
#define att_readonly   0x02
#define att_invisible  0x04
#define att_array      0x08
#define att_nounset    0x10
//...

/* 
 This is on the way out.  
 It is only here so that 
 set_or_show_variables() has a flag value 
 for the "-f" flag. 
 */

#define att_function  0x20

// # attributes #
#define exported_p(var)  ((((var)->attributes) & (att_exported)))
#define readonly_p(var)  ((((var)->attributes) & (att_readonly)))
#define invisible_p(var) ((((var)->attributes) & (att_invisible)))
#define array_p(var)     ((((var)->attributes) & (att_array)))

// # function #
#define function_p(var)  ((((var)->function) != (char *) NULL))
#define function_cell(var) ((var)->function)

// # value #
#define value_cell(var) ((var)->value)

/* 
 Stuff for hacking variables. 
 */
/* 
extern SHELL_VAR *variable_list, *bind_variable(), *find_variable();
extern SHELL_VAR *copy_variable ();
extern char *get_string_value (), *dollar_vars[];
extern char **export_env;
extern int variable_context;
*/

#endif   

