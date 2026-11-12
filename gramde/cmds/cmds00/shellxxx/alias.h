/* alias.h -- structure definitions. */


#ifndef _ALIAS_
#define _ALIAS_


#ifndef whitespace
#define whitespace(c) (((c) == ' ') || ((c) == '\t'))
#endif


#ifndef savestring
#define savestring(x) (char *)strcpy (xmalloc (1 + strlen (x)), (x))
#endif


typedef struct assoc_d assoc_t;
typedef struct assoc_d ASSOC;
struct assoc_d 
{
    char *name;
    char *value;
};


//#ifndef NULL
//#define NULL 0x0
//#endif


/* The list of known aliases. */
//extern ASSOC **aliases;

/* Scan the list of aliases looking for one with NAME.  Return NULL
   if the alias doesn't exist, else a pointer to the assoc. */
//extern ASSOC *find_alias ();

/* Return the value of the alias for NAME, or NULL if there is none. */
//extern char *get_alias_value ();

/* Make a new alias from NAME and VALUE.  If NAME can be found,
   then replace its value. */
//extern void add_alias ();

/* Remove the alias with name NAME from the alias list.  Returns
   the index of the removed alias, or -1 if the alias didn't exist. */
//extern int remove_alias ();

/* Return a new line, with any aliases substituted. */
//extern char *alias_substitute ();

#endif  /* _ALIAS_ */
