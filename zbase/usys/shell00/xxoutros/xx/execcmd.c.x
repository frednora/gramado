// execcmd.c


/* 
 The value returned by the last synchronous command. 
 */
int last_command_exit_value = 0;


/* 
 The list of redirections to preform which will undo the redirections
 that I made in the shell. 
 */
REDIRECT *redirection_undo_list = (REDIRECT *) NULL;



/* Returns 1 if TYPE is a shell control structure type. */
/* credtis: bash 1.05*/
int
shell_control_structure( command_type_t type );
int
shell_control_structure( command_type_t type )
{
    switch (type)
    {
        case cm_for:
        case cm_case:
        case cm_while:
        case cm_until:
        case cm_if:
        case cm_group:
            return (1);
            break;
			
        default:
            return (0);
			break;
    };
};

void
close_pipes( int in, int out );
void
close_pipes( int in, int out )
{
    if( in >= 0 ) 
	    close(in);
	
    if( out >= 0 ) 
	    close(out);
};


/* Make sure that identifier is a valid shell identifier, i.e.
   does not contain a dollar sign, nor is quoted in any way.  Nor
   does it consist of all digits. */
int 
check_identifier(WORD_DESC *word);   
int 
check_identifier(WORD_DESC *word)   
{
    if(word->dollar_present || word->quoted || all_digits (word->word)) 
	{
        //report_error ("`%s' is not a valid identifier", word->word);
		printf("`%s' is not a valid identifier", word->word);
        return (0);
    } else return (1);
};


int
all_digits(char *string);
int
all_digits(char *string)
{
    while( *string )
	{
        if (!digit (*string)) return (0);
            else string++;
    }
    return (1);
};


#define u_mode_bits(x) (((x) & 0000700) >> 6)
#define g_mode_bits(x) (((x) & 0000070) >> 3)
#define o_mode_bits(x) (((x) & 0000007) >> 0)
#define X_BIT(x) (x & 1)


/* Non-zero if the last call to executable_file () found
   the file, but stated that it wasn't executable. */
int file_exists_p = 0;


/* 
 Given a string containing units of information separated by colons,
 return the next one pointed to by INDEX, or NULL if there are no more.
 Advance INDEX to the character after the colon. 
 */
char *
extract_colon_unit( char *string, int *index ); 
char *
extract_colon_unit( char *string, int *index )
{
    int i, start;

    i = *index;

    if( (i >= strlen (string)) || !string )
        return ( (char *) NULL );

    if( string[i] == ':' )
        i++;

    start = i;

    while ( string[i] && string[i] != ':' ) i++;

    *index = i;

    if ( i == start )
    {
        if ( !string[i] )
	        return ( (char *) NULL );

        (*index)++;

        return ( savestring ("") );
    } else {
      
	  char *value;

      value = (char *) xmalloc (1 + (i - start));
      strncpy ( value, &string[start], (i - start) );
      value [i - start] = '\0';

      return (value);
    };
};









