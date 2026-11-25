
// string.c
// String support.

#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>


/*
 // ?? not tested
char rtl_convert_char_to_uppercase( char ch );
char rtl_convert_char_to_uppercase( char ch )
{
    if (    (ch >= 'a') && 
            (ch <= 'z') ) 
    {
        return ch + 'A' - 'a';
    }

    return ch;
}
*/

/*
// 2021 - Created by Fred Nora.
// Not tested yet.
char *rtl_new_string(const char *old_string);
char *rtl_new_string(const char *old_string)
{
    size_t Size=0;
    char *tmp;

    if ( (void*) old_string == NULL )
        return NULL;
    
    Size = strlen(old_string);
    Size = Size+1;
    
    if (Size<=0)
        return NULL;
    
    tmp = (char *) malloc(Size);
    
    if ( (void*) tmp == NULL )
        return NULL;
    
    return (char *) strcpy ( tmp, old_string );
}
*/

/*
static char *int2str(int n);
static char *int2str(int n)
{
    int         i;
    static char res[16];

    res[15] = '\0';
    res[14] = (n%10)+'0';
    for (i=13; (n /= 10); i--)
        res[i] = (n%10)+'0';

    return &res[i+1];
}
*/


/*
 //credits: HOPPY OS.
int occurrences (char *src, char ch);
int occurrences (char *src, char ch)
{
    int i=0;
    
    while (*src)
        if (*src++ == ch) i++;
    
    return i;
}
*/


/*
 //credits: HOPPY OS.
char *remove_first_segment(char *src, char marker, char *dst);
char *remove_first_segment(char *src, char marker, char *dst)
{
  char *temp,*temp1,ch;

  temp = src;
  temp1= dst;
  while ( (ch = *src++) && (ch != marker) )
    *dst++=ch;

  *dst = 0;
  if (ch)
    strcpy(temp,src);
  else
    *temp=0;
  return temp1;
}
*/

/*
 //credits: HOPPY OS.
char *remove_last_segment(char *src, char marker, char *dst);
char *remove_last_segment(char *src, char marker, char *dst)
{
  char *temp;

  temp=src+strlen(src);
  while (temp>src && *temp!=marker)
    temp--;
  if (*temp==marker) {
    strcpy(dst,temp+1);
    *temp=0;
  } else {
    strcpy(dst,src);
    *src=0;
  }
  return dst;
}
*/


/*
 //credits: HOPPY OS.
bool wild_card_match(char *wild_st,char *src_st);
bool wild_card_match(char *wild_st,char *src_st)
{
  BYTE ch1,ch2;
  while (true) {
    if ( !(ch1 = *src_st++)) {
      if (*wild_st)
	return false;
      else
	return true;
    } else {
      if ( !(ch2 = *wild_st++))
	return false;
      else {
	if (ch2 == '*') {
	  if (!(ch2 = *wild_st++))
	    return true;
	  while (ch2 != ch1)
	    if (!(ch1 = *src_st++))
	      return false;
	} else
	  if (ch2!='?')
	    if (ch1!=ch2)
	      return false;
      }
    }
  }
}
*/

/*
 //credits: HOPPY OS.
bool wild_filename_list(char *wild_st,char *src_st);
bool wild_filename_list(char *wild_st,char *src_st)
{
  char *buf1=new_string(wild_st),
       *buf2=malloc(strlen(wild_st)+1);
  while (true) {
    remove_first_segment(buf1,';',buf2);
    if (*buf2) {
      if (wild_card_match(buf2,src_st))  {
	free(buf1);
	free(buf2);
	return true;
      }
    } else {
      free(buf1);
      free(buf2);
      return false;
    }
  }
}
*/


/*
// #test
void strupr2(char *s);
void strupr2(char *s)
{
    while (*s){ 
        *s = toupper(*s); 
        s++;
    };
}
*/



int strupr (char *n)
{
    register int i=0;
    
// #bugbug
// Find another way to do this.
    for (i=0; n[i]; i++) 
    {
        n[i] = toupper(n[i]);
    };
    
    return 1;
}


int strlwr (char *n) 
{

    register int i=0;

    // #bugbug
    // Find another way to do this.
    for (i=0; n[i]; i++) 
    {
        n[i] = tolower(n[i]);
    };
    
    return 1;
}


/*
 Credits: Apple chameleon 
void * stosl(void * dst, long val, long len);
void * stosl(void * dst, long val, long len)
{
    asm volatile ( "rep; stosl"
       : "=c" (len), "=D" (dst)
       : "0" (len), "1" (dst), "a" (val)
       : "memory" );

    return dst;
}
*/


int strcoll (const char *s1, const char *s2)
{
    return (int) strcmp ( (char *) s1, (char *) s2);
}



/**
 * linux style.
 * strncasecmp - Case insensitive, length-limited string comparison
 * @s1: One string
 * @s2: The other string
 * @len: the maximum number of characters to compare
 */
/*
int strncasecmp(const char *s1, const char *s2, size_t len); 
int strncasecmp(const char *s1, const char *s2, size_t len)
{
	//Yes, Virginia, it had better be unsigned 
	unsigned char c1, c2;

	if (!len)
		return 0;

	do {
		c1 = *s1++;
		c2 = *s2++;
		if (!c1 || !c2)
			break;
		if (c1 == c2)
			continue;
		c1 = tolower(c1);
		c2 = tolower(c2);
		if (c1 != c2)
			break;
	} while (--len);
	return (int)c1 - (int)c2;
}
*/


// musl
/*
int strncasecmp(const char *_l, const char *_r, size_t n);
int strncasecmp(const char *_l, const char *_r, size_t n)
{
	const unsigned char *l=_l, *r=_r;
	if (!n--) return 0;
	for (; *l && *r && n && (*l == *r || tolower(*l) == tolower(*r)); l++, r++, n--);
	return tolower(*l) - tolower(*r);
}
*/

void *memsetw (void *dst, int val, size_t count)
{
    unsigned short *temp = (unsigned short *) dst;

    //danger!
    for ( ; count != 0; count--)
    {
        *temp++ = val;
    };

    return (void *) dst;
}

// memcmp:
// Compare memory regions.

int memcmp (const void *s1, const void *s2, size_t n)
{
    if (n != 0) 
    {
        const unsigned char *p1 = s1, *p2 = s2;

        do {

            if (*p1++ != *p2++)
                return (*--p1 - *--p2);

        } while (--n != 0);
    }

    return 0;
}


/*
 * strdup:
 *
 * PUBLIC: #ifndef HAVE_STRDUP
 * PUBLIC: char *strdup __P((const char *));
 * PUBLIC: #endif
 */

char *strdup (const char *str)
{
    char *copy;
    size_t len=0;

    //if ( (void*) s == NULL )
       //return (char *) 0;


    len = strlen(str) + 1;

    // ugly!
    if ( !(copy = (char *) malloc ( ( size_t ) len ) ) )
    {
        return NULL;
    }

    memcpy (copy, str, len);
    
    return (copy);
}


char *strndup (const char *s, size_t n)
{
    char *d;

    size_t l = strnlen (s, n);


    // src
    //if ( (void*) s == NULL )
       //return (char *) 0;

    // dst
    d = (char *) malloc (l + 1);

    if (!d){
        return NULL;
    }
    
    memcpy (d, s, l);

    d[l] = '\0';

    return (char *) d;
}


/**
 * skip_spaces - Removes leading whitespace from @str.
 * @str: The string to be stripped.
 *
 * Returns a pointer to the first non-whitespace character in @str.
 */
/* 
char *skip_spaces(const char *str); 
char *skip_spaces(const char *str)
{
	while (isspace(*str))
		++str;
	return (char *)str;
}
*/


/*linux style*/
/**
 * strim - Removes leading and trailing whitespace from @s.
 * @s: The string to be stripped.
 *
 * Note that the first trailing whitespace is replaced with a %NUL-terminator
 * in the given string @s. Returns a pointer to the first non-whitespace
 * character in @s.
 */

//#bugbug
//Talvez o arquivo string.c deva ter sua própria implementação
//de isspace. 
 
/* 
char *strim(char *s); 
char *strim(char *s)
{
	size_t size;
	char *end;

	size = strlen(s);
	if (!size)
		return s;

	end = s + size - 1;
	while (end >= s && isspace(*end))
		end--;
	*(end + 1) = '\0';

	return skip_spaces(s);
}
*/



/*linux-style*/
/**
 * strnchr - Find a character in a length limited string
 * @s: The string to be searched
 * @count: The number of characters to be searched
 * @c: The character to search for
 */

char *strnchr (const char *s, size_t count, int c)
{

    // #ugly
    for (; count-- && *s != '\0'; ++s)
        if (*s == (char)c)
            return (char *) s;

    return NULL;
}


/*
 * strrchr:
 * PUBLIC: #ifndef HAVE_STRRCHR
 * PUBLIC: char *strrchr __P((const char *, int));
 * PUBLIC: #endif
 */
// returns a pointer to the last occurrence
// of the character c in the string s.

char *strrchr (const char *p, int ch)
{
    char *save;
    char c=0;

    c = ch;


    //if ( (void*) p == NULL )
       //return (char) 0;


    // #ugly
    for ( save = NULL;; ++p ) 
    {
        if (*p == c){
            save = (char *) p;
        }

        if (*p == '\0'){
            return (char *) save;
        }
    };
    
    /* NOTREACHED */
}


void rtl_reverse(char s[])
{
    char c=0;
    register int i=0; 
    register int j=0;
    int Size = strlen(s)-1;

    for ( 
        i=0, j=Size; 
        i<j; 
        i++, j--) 
    {
        c = s[i];
        
        s[i] = s[j];
        s[j] = c;
    }
}


//#todo
//definido em inttypes.h
intmax_t strtoimax (const char *str, char **endptr, int base)
{
    debug_print ("strtoimax: [TODO]\n");
    return 0;
}

//#todo
//definido em inttypes.h
uintmax_t strtoumax (const char *str, char **endptr, int base)
{
    debug_print ("strtoumax: [TODO]\n");
    return 0;
}


/*
 * credits: Serenity OS
static char foldcase(char ch);
static char foldcase(char ch)
{
    if (isalpha(ch))
        return tolower(ch);
    return ch;
}
*/


/*
 * credits: Serenity OS
int strcasecmp2(const char* s1, const char* s2);
int strcasecmp2(const char* s1, const char* s2)
{
    for (; foldcase(*s1) == foldcase(*s2); ++s1, ++s2) {
        if (*s1 == 0)
            return 0;
    }
    return foldcase(*(const unsigned char*)s1) < foldcase(*(const unsigned char*)s2) ? -1 : 1;
}
*/


/*
 * credits: Serenity OS
int strncasecmp(const char* s1, const char* s2, size_t n);
int strncasecmp(const char* s1, const char* s2, size_t n)
{
    if (!n)
        return 0;
    do {
        if (foldcase(*s1) != foldcase(*s2++))
            return foldcase(*(const unsigned char*)s1) - foldcase(*(const unsigned char*)--s2);
        if (*s1++ == 0)
            break;
    } while (--n);
    return 0;
}
*/

/*
// credits: chameleon boot loader.
int strncasecmp(const char *s1, const char *s2, size_t len);
int strncasecmp(const char *s1, const char *s2, size_t len)
{
	register int n = len;
	while (--n >= 0 && tolower(*s1) == tolower(*s2++))
		if (*s1++ == '\0')
			return(0);
	return(n<0 ? 0 : tolower(*s1) - tolower(*--s2));
}
*/



/*
 *********************************************************
 * strcasecmp:
 *     Do strcmp(3) in a case-insensitive manner.
 *
 * PUBLIC: #ifndef HAVE_STRCASECMP
 * PUBLIC: int strcasecmp __P((const char *, const char *));
 * PUBLIC: #endif
 */

int strcasecmp (const char *s1, const char *s2){

    unsigned char s1ch=0; 
    unsigned char s2ch=0;


    for (;;){

        s1ch = *s1++;
        s2ch = *s2++;

		//tolower
		
        if (s1ch >= 'A' && s1ch <= 'Z'){ s1ch += 32; }
        if (s2ch >= 'A' && s2ch <= 'Z'){ s2ch += 32; }

        if (s1ch != s2ch){
            return (int) (s1ch - s2ch);
        }

        if (s1ch == '\0'){  return 0;  }
    }
	/* NOTREACHED */
}


/* ANSI sez:
 *   The `strncpy' function copies not more than `n' characters (characters
 *   that follow a null character are not copied) from the array pointed to
 *   by `s2' to the array pointed to by `s1'.  If copying takes place between
 *   objects that overlap, the behavior is undefined.
 *   If the array pointed to by `s2' is a string that is shorter than `n'
 *   characters, null characters are appended to the copy in the array
 *   pointed to by `s1', until `n' characters in all have been written.
 *   The `strncpy' function returns the value of `s1'.  [4.11.2.4]
 */

char *strncpy (char *s1, const char *s2, size_t n)
{
    char *s = s1;

    while (n > 0 && *s2 != '\0')
    {
        *s++ = *s2++;
        n--;
    };

    while (n > 0)
    {
        *s++ = '\0';
        n--;
    };

    return (char *) s1;
}


/*
 * strcmp:
 *     Compare two strings. 
 */

// See:
// http://man7.org/linux/man-pages/man3/strcmp.3.html

int strcmp (const char *s1, const char *s2)
{
    register int i=0;

    for ( i=0; s1[i] == s2[i]; i++ )
    {
        if ( s1[i] == '\0' ){
            return 0;
        }
    };

    return (int) ( s1[i] - s2[i] );
}

// strncmp:
//  Compare two strings
// See:
// http://man7.org/linux/man-pages/man3/strcmp.3.html
int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n > 0)
    {
        n--;
        if (*s1 != *s2){
            return (int) 1;
        }
        s1++;  
        s2++;
    };

    if ( *s1 != '\0' || *s2 != '\0' )
    {
        return (int) 2;
    }

    return 0;
}

void *memset( void *ptr, int value, int size )
{
    register int i=0;

    //if ( (void*) ptr == NULL )
       //return NULL;

    if ( ptr != NULL && size > 0 )
    {
        unsigned char *temp = ptr;

        for ( i=0; i < size; i++ )
        {
            *temp++ = (unsigned char) value;
        };
    }

    return (void *) ptr;
}


//#todo: mover para memory.c ??
void *memoryZeroMemory ( void *ptr, size_t cnt )
{
    volatile char *vptr = (volatile char *) ptr;

    //if ( (void*) ptr == NULL )
       //return NULL;

    while (cnt)
    {
        *vptr = 0;
        
        vptr++;
        cnt--;
    };

    return (void *) ptr;
}


/*
 * memcpy:
 *     ?? #todo: mover para memory.c ?? 
 */

// Simple, byte oriented memcpy. 

void *memcpy ( 
    void       *v_dst, 
    const void *v_src, 
    unsigned long c )
{
    register const char *src = (char *) v_src;
                   char *dst = (char *) v_dst;

    register unsigned long Copy=c;

    while (Copy--)
    {
        *dst++ = *src++; 
    };

    return (void *) v_dst;
}

void *memcpy32 ( void *v_dst, const void *v_src, unsigned long n )
{
    register const int *src = (int *) v_src;
                   int *dst = (int *) v_dst;

    register unsigned long Copy=n;
    
    while (Copy--)
    {
        *dst++ = *src++;
    };

    return v_dst;
}

void *memcpy64 ( void *v_dst, const void *v_src, unsigned long n )
{
    register const long *src = (long *) v_src;
                   long *dst = (long *) v_dst;

    register unsigned long Copy=n;

    while (Copy--)
    {
        *dst++ = *src++;
    };

    return v_dst;
}

// Copy a string. 
char *strcpy ( char *to, const char *from )
{
    register int i=0;

    //if ( (void*) to == NULL )
       //return (char *) 0;

    while ( to[i] = from[i] )
    {
        i += 1;
    };

    return (char *) to;
}


/*
 //#test
char* strcpy(char* to, const char* from);
char* strcpy(char* to, const char* from)
{
    char *dest_ptr = to;
    const char *src_ptr = from;
    while ((*dest_ptr++ = *src_ptr++) != '\0')
        ;
    return to;
}
*/

// 8 bits.
size_t strlcpy (char *dst, const char *src, size_t size)
{
    register const char *p = (char *) src;
                   char *q = (char *) dst;

    register size_t bytes=0;
    char ch=0;

    while ( (ch = *p++) )
    {
        if (bytes + 1 < size){  
            *q++ = ch;  
        }
        bytes++;
    };

// If size == 0 there is no space for a final null... 
    if (size){
        *q = '\0';
    }

    return (size_t) bytes;
}

// strcat:
// Concatenate strings. 
char *strcat ( char *to, const char *from )
{
    char *ret = to;

    //if ( (void*) to == NULL )
       //return (char *) 0;

    while ( *to ){
        to += 1;
    };

    strcpy ( to, from );
    
    return (char *) ret;
}


/*linux style*/
/**
 * strchrnul - Find and return a character in a string, or end of string
 * @s: The string to be searched
 * @c: The character to search for
 * Returns pointer to first occurrence of 'c' in s. If c is not found, then
 * return a pointer to the null byte at the end of s.
 */

// is like strchr() except that if c is not
// found in s, then it returns a pointer to 
// the '0' byte at the end of s, rather than '0'.

char *strchrnul (const char *s, int c)
{
    //if ( (void*) s == NULL )
       //return (char *) 0;

    while (*s && *s != (char)c){  s++;  };

    return (char *) s;
}

// copy and concatenate strings respectively. 
// size-bounded string copying and concatenation.
// see: https://linux.die.net/man/3/strlcat
size_t strlcat (char *dst, const char *src, size_t size)
{
    char *q = dst;
    const char *p = src;

    //register char ch=0;
    //register size_t bytes = 0;
    char ch=0;
    size_t bytes = 0;
    
    //#bugbug
    //E se size for menor que zero.?

    while (bytes < size && *q)
    {
        q++;
        bytes++;
    };

    if (bytes == size){
        return (bytes + strlen(src));
    }

    //ugly
    while ((ch = *p++))
    {
        if (bytes + 1 < size){
            *q++ = ch;
        }

        bytes++;
    };

    *q = '\0';

    return bytes;
}

char *strncat (char *dst, const char *src, size_t n)
{
    char *q = strchr (dst, '\0');
    const char *p = src;
    //register char ch=0;
    char ch=0;
    
// #bugbug
// E se n for menor que zero?
    while (n--)
    {
        //very ugly
        *q++ = ch = *p++;
        
        if (!ch){  return dst;  };
    };

    *q = '\0';

    return (char *) dst;
}


/*
char *strncat2(char *s1, const char *s2, size_t n);
char *strncat2(char *s1, const char *s2, size_t n)
{
	register char *ret = s1;

	while (*s1)
		s1++;

	while (n-- && *s2)
		*s1++ = *s2++;

	*s1 = '\0';

	return ret;
}
*/

/*
char *strcat2(char *s1, const char *s2);
char *strcat2(char *s1, const char *s2)
{
    return (strncat(s1, s2, strlen(s2)));
}
*/

// bcopy: 
void bcopy ( char *from, char *to, int len )
{
    //if ( (void*) from == NULL )
       //return;

    //if ( (void*) to == NULL )
       //return;

    //if ( len < 0 )
    //    return;

    while (len--){
        *to++ = *from++; 
    };
}

// bzero:
void  bzero ( char *cp, int len )
{
    //if ( (void*) cp == NULL )
       //return;

    //if ( len < 0 )
    //    return;

    while (len--){
        *(cp + len) = 0; 
    };
}

// strlen:
// Get the size of a string.
size_t strlen (const char *s)
{
    register size_t i=0;

    //if ( (void*) s == NULL )
       //return -1;

    for ( i=0; s[i] != '\0'; i++ )
    { 
        ; 
    };

    return (size_t) i;
}


/* 
 * strnlen:
 *  Find the length of S, but scan at most MAXLEN characters.  
 *  If no '\0' terminator is found within the first MAXLEN characters, 
 *  return MAXLEN. 
 */

size_t strnlen ( const char *s, size_t maxlen )
{
    const char *e;
    register size_t n=0;

    //if ( (void*) s == NULL )
       //return -1;

    //#bugbug
    // E se maxlen for negativo.

    //if ( maxlen <=0 )
        //return -1;

    //# very ugly  shit
    for ( e=s, n=0; *e && n<maxlen; e++, n++ )
    {
    };

    return (size_t) n;
}

// ?
char *strpbrk (const char *cs, const char *ct)
{
    const char *sc1;
    const char *sc2;

    //if( (void*) cs == NULL )
        //return (char*) 0;

    //if( (void*) ct == NULL )
        //return (char*) 0;

    // #ugly
    for ( sc1 = cs; *sc1 != '\0'; ++sc1 )
    {
        // #ugly
        for ( sc2 = ct; *sc2 != '\0'; ++sc2 )
        {
            if (*sc1 == *sc2){ return (char *) sc1; }
        };
    };

    return NULL;
}


/*linux style*/
/**
 * strsep - Split a string into tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 *
 * strsep() updates @s to point after the token, ready for the next call.
 *
 * It returns empty tokens, too, behaving exactly like the libc function
 * of that name. In fact, it was (*stolen) from glibc2 and de-fancy-fied.
 * Same semantics, slimmer shape. ;)
 */

char *strsep (char **s, const char *ct)
{
    char *sbegin = *s;
    char *end;

    if (sbegin == NULL){
        return NULL;
    }

    end = strpbrk (sbegin, ct);

    if (end){
        *end++ = '\0';
    }
    
    *s = end;

    return (char *) sbegin;
}


/*linux style*/
/**
 * sysfs_streq - return true if strings are equal, modulo trailing newline
 * @s1: one string
 * @s2: another string
 *
 * This routine returns true iff two strings are equal, treating both
 * NUL and newline-then-NUL as equivalent string terminations.  It's
 * geared for use with sysfs input strings, which generally terminate
 * with newlines but are compared against values without newlines.
 */
//mudar esse bool.
/* 
bool sysfs_streq(const char *s1, const char *s2)/ 
bool sysfs_streq(const char *s1, const char *s2)
{
	while (*s1 && *s1 == *s2) {
		s1++;
		s2++;
	}

	if (*s1 == *s2)
		return true;
	if (!*s1 && *s2 == '\n' && !s2[1])
		return true;
	if (*s1 == '\n' && !s1[1] && !*s2)
		return true;
	return false;
}
*/


/*linux stuff*/
/**
 * match_string - matches given string in an array
 * @array:	array of strings
 * @n:		number of strings in the array or -1 for NULL terminated arrays
 * @string:	string to match with
 *
 * Return:
 * index of a @string in the @array if matches, or %-EINVAL otherwise.
 */

// #todo: O valor de retorno. 
 
/*
int match_string(const char * const *array, size_t n, const char *string); 
int match_string(const char * const *array, size_t n, const char *string)
{
	int index;
	const char *item;

	for (index = 0; index < n; index++) {
		item = array[index];
		if (!item)
			break;
		if (!strcmp(item, string))
			return index;
	}

	return -EINVAL;
}
*/


/**
 * memset16() - Fill a memory area with a uint16_t
 * @s: Pointer to the start of the area.
 * @v: The value to fill the area with
 * @count: The number of values to store
 *
 * Differs from memset() in that it fills with a uint16_t instead
 * of a byte.  Remember that @count is the number of uint16_ts to
 * store, not the number of bytes.
 */
 
//#todo: incluir o header que permite usar esse tipo aí.
/* 
void *memset16(uint16_t *s, uint16_t v, size_t count); 
void *memset16(uint16_t *s, uint16_t v, size_t count)
{
	uint16_t *xs = s;

	while (count--)
		*xs++ = v;
	return s;
}
*/


/**
 * memset32() - Fill a memory area with a uint32_t
 * @s: Pointer to the start of the area.
 * @v: The value to fill the area with
 * @count: The number of values to store
 *
 * Differs from memset() in that it fills with a uint32_t instead
 * of a byte.  Remember that @count is the number of uint32_ts to
 * store, not the number of bytes.
 */

//#todo: incluir o header que permite usar esse tipo aí.
/*
void *memset32 (uint32_t *s, uint32_t v, size_t count); 
void *memset32 (uint32_t *s, uint32_t v, size_t count)
{
    uint32_t *xs = s;

	while (count--)
		*xs++ = v;
    return (void *) s;
}
*/


void *check_bytes8 ( 
    const char *start, 
    char value, 
    unsigned int bytes )
{

    //if( (void*) start == NULL )
        //return NULL;

    while (bytes)
    {
        if (*start != value){
            return (void *) start;
        }

        start++;
        bytes--;
    };

    return NULL;
}

/**
 * strreplace - Replace all occurrences of character in string.
 * @s: The string to operate on.
 * @old: The character being replaced.
 * @new: The character @old is replaced with.
 *
 * Returns pointer to the nul byte at the end of @s.
 */

char *strreplace (char *s, char old, char new)
{
    //if( (void*) s == NULL ){
    //    return (char*) 0;
    //}

    //ugly
    for (; *s; ++s)
    {
        if (*s == old)
        {
            *s = new; 
        }
    };

    return (char *) s;
}

/*
 * strcspn:
 *     ??
 * Credits: 
 *     Copyright (c) 2011, 2012 Jonas 'Sortie' Termansen.
 */

size_t strcspn ( const char *str, const char *reject )
{
// The return
    size_t Result=0;
// Iterators
    register int reject_length = 0;
    register int i=0;
    //bool matches = 0;
    int matches=0;

    while ( reject[reject_length] )
    {
        reject_length++;
    };

    for ( Result=0; Result=1; Result++ )
    {
        char c = str[Result];

        if ( !c ){  return (size_t) Result;  }

        for ( i=0; i < reject_length; i++ )
        {
            if ( str[Result] != reject[i] ){ continue; }

            matches = 1;
            break;
        };

        if (matches){
            return (size_t) Result;
        }
    };
}

/*
 * strspn:
 *   Gets the length of a substring.
 * Credits: 
 * Copyright (c) 2011, 2012 Jonas 'Sortie' Termansen.
 */
 
size_t strspn ( const char *str, const char *accept )
{
// The return
    int result=0;
// Iterators
    register int accept_length = 0; 
    register int i=0;
    int matches = 0;

    //if( (void*) str == NULL )
        //return -1;

    while ( accept[accept_length] )
    {
        accept_length++;
    };

    for ( result = 0; result = 1; result++ )
    {
        char c = str[result];

        if ( !c ){
            return (size_t) result;
        }

        for ( i=0; i<accept_length; i++ )
        {
            if ( str[result] != accept[i] ){ continue; }

            matches = 1;
            break;
        };

        if ( !matches ){
            return (size_t) result;
        }
    };
}

/*
 * strtok_r:
 *     Usada por strtok.
 * Credits: 
 *     Apple. (Open Source)
 */

char *strtok_r ( char *s, const char *delim, char **last )
{
    char *spanp;
    char *tok;
    register int c=0; 
    register int sc=0;

    //#ugly
    if ( s == NULL && (s = *last) == NULL )
    {
        return NULL;
    }

// Skip (span) leading delimiters (s += strspn(s, delim), sort of).
cont:
    c = *s++;

    // #ugly
    for ( spanp = (char *) delim; (sc = *spanp++) != 0; )
    {
        if (c == sc){  goto cont;  }
    };

// No non-delimiter characters. 
    if (c == 0){
        *last = NULL;
        return NULL;
    }

    tok = s -1;

// Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
// Note that delim must have one NUL; we stop if we see that, too.
 
    for (;;)
    {
        c = *s++;
        spanp = (char *) delim;

        do
        {
            if ( (sc = *spanp++) == c )
            {
                if ( c == 0 ){
                    s = NULL;
                }else{

                    char *w = s -1;

                    *w = '\0';
                };

                *last = s;

                return tok;
            };

        } while( sc != 0 );

    //Nothing.

    };
    // NOTREACHED 
}


// strtok:
char *strtok ( char *s, const char *delim )
{
    static char *last;

    //if( (void*) s == NULL )
        //return (char *) 0;

    //if( (void*) delim == NULL )
        //return (char *) 0;

    return strtok_r ( s, delim, &last );
}


/*
 * strchr:
 * linux style 
 */
// returns a pointer to the first occurrence
// of the character c in the string s.

char *strchr (const char *s, int c)
{
    //if( (void*) s == NULL )
        //return (char *) 0;

    //#ugly
    for (; *s != (char) c; ++s)
    {
        if (*s == '\0'){
            return NULL;
        }
    };

    return (char *) s;
}


/*linux style*/
void *memmove (void *dest, const void *src, size_t count)
{
    char *tmp;
    const char *s;

    //if(count<0)
        //return NULL;

    if (dest <= src) {
        tmp = dest;
        s = src;
        
        while (count--){
            *tmp++ = *s++;
        };

    } else {
        tmp = dest;
        tmp += count;
        s = src;
        s += count;

        while (count--){
            *--tmp = *--s;
        };
    };

    return dest;
}

/*
 //linux style 
 * memscan - Find a character in an area of memory.
 * IN:
 * @addr: The memory area
 * @c:    The byte to search for
 * @size: The size of the area.
 * OUT:
 *     Returns the address of the first occurrence 
 * of @c, or 1 byte past the area if @c is not found
 */

void *memscan (void *addr, int c, size_t size)
{
    unsigned char *p = addr;

    //if( (void*) addr == NULL ){
    //    return NULL;
    //}

    //if(size<0)
        //return NULL;

    while (size)
    {
        if (*p == c){
            return (void *) p;
        }

        p++;
        size--;
    };

    return (void *) p;
}

/**
 *linux style
 * strstr - Find the first substring in a %NUL terminated string
 * @s1: The string to be searched
 * @s2: The string to search for
 */

char *strstr (const char *s1, const char *s2)
{
    register size_t l1=0; 
    register size_t l2=0;

    l2 = strlen(s2);

    if (!l2){
        return (char *) s1;
    }

    l1 = strlen(s1);

    while (l1 >= l2)
    {
        l1--;
        
        // #ugly
        if ( !memcmp(s1, s2, l2) )
        {
            return (char *)s1;
        }

        s1++;
    };

    return NULL;
}

/*
 ** Concatenate two given string, creating a mark space at the beginning.
 ** Return the new string pointer.
 */
/*
// It's a test.
// Credits: lua 1.0.
static char *__strconc (char *l, char *r);
static char *__strconc (char *l, char *r){

    char *s = calloc ( strlen(l) + strlen(r) +2, sizeof(char) );
 
    if (s == NULL)
    {
        printf ("Not enough memory \n");
        return NULL;
    }

    // Create mark space.
    *s++ = 0;

    return strcat ( strcpy(s,l),r );
}
*/


/*
 ** Duplicate a string,  creating a mark space at the beginning.
 ** Return the new string pointer.
 */
/*
// Credits: lua 1.0.
char *__strdup (char *l);
char *__strdup (char *l){
    
    char *s = calloc ( strlen(l)+2, sizeof(char) );
    
    if (s == NULL)
    {
        printf ("Not enough memory \n");
        return NULL;
    }

    //Create mark space.
    *s++ = 0; 			
    
    return strcpy (s,l);
}
*/

// Retorna o endereço onde o char foi encontrado?
// Primeira ocorrência.
char *index (const char *s, int c)
{
    do {
        if (*s == c){
            return (char *) (s);
        }
    } while (*s++);

    return (NULL);
}

char *rindex (const char *s, int c)
{
    return (char *) strrchr(s,c);
}

/*
 * Transform src, storing the result in dst, such that
 * strcmp() on transformed strings returns what strcoll()
 * on the original untransformed strings would return.
 */
// Credits: apple open source 

size_t strxfrm (char *dst, const char *src, size_t n)
{
// The return.
    size_t srclen=0; 
// work integer.
    register size_t copysize=0;

// Since locales are unimplemented, this is just a copy.
    srclen = strlen (src);

    if (n != 0)
    {
        copysize = srclen < n ? srclen : n - 1;

        //(void) memcpy (dst, src, copysize);
        memcpy (dst, src, copysize);

        dst[copysize] = 0;
    }

    return (size_t) srclen;
}


/*
 * credits: Serenity OS.
size_t strxfrm(char* dest, const char* src, size_t n);
size_t strxfrm(char* dest, const char* src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; ++i)
        dest[i] = src[i];
    for (; i < n; ++i)
        dest[i] = '\0';
    return i;
}
*/

/*
 The swab() function copies n bytes from the array pointed to by from
 to the array pointed to by to, exchanging adjacent even and odd
 bytes.  This function is used to exchange data between machines that
 have different low/high byte ordering.
 See: http://man7.org/linux/man-pages/man3/swab.3.html
 Credits: Apple open source.  
 */
/*
void swab (const void *from, void *to, size_t len);
void swab (const void *from, void *to, size_t len)
{
	register unsigned long temp;
	register int n;
	register char *fp, *tp;

	n = (len >> 1) + 1;
	fp = (char *)from;
	tp = (char *)to;
#define	STEP	temp = *fp++,*tp++ = *fp++,*tp++ = temp
	//round to multiple of 8
	while ((--n) & 07)
		STEP;
	n >>= 3;
	while (--n >= 0) {
		STEP; STEP; STEP; STEP;
		STEP; STEP; STEP; STEP;
	}
}
*/

char *strerror (int errnum)
{
    debug_print ("strerror: [TODO]\n");
    return 0; 
}

/* XSI-compliant */
int 
strerror_r (
    int errnum, 
    char *buf, 
    size_t buflen )
{
    size_t tmp=0;
    char *msg = strerror(errnum);
  
    if ( (void*) buf == NULL ){
        return -1;
    }

    tmp = strlen(msg);

    if (tmp >= buflen){
        return -1;  //return ERANGE;
    }

    strcpy(buf, msg);

    return 0;
}


/* GNU-specific */
/*
char *strerror_r(int errnum, char *buf, size_t buflen)
{ 
    debug_print ("strerror_r: GNU-specific \n");
    return 0; 
}
*/

/*
char *strerror_l(int errnum, locale_t locale);
char *strerror_l(int errnum, locale_t locale)
{ return 0; }
*/

char *strsignal (int sig)
{
    debug_print ("strsignal: [TODO]\n");
    return 0; 
}

//#define tolower(c)	(isupper(c) ? c + 'a' - 'A' : c)
//#define toupper(c)	(islower(c) ? c + 'A' - 'a' : c)
// Converte a string para tudo maiusculo.

void strtoupper (char *src)
{
    char *c;

    if ( (void*) src == NULL ){
        return;
    }
    
    //if( *src == 0 ){ return; }

    c = src;
  
    while ((*c)!='\0')
    {
        (*c) = toupper (*c);

        c++;
    };
}


void rtl_string_to_upper (char *src)
{
    strtoupper(src);
}

/*
// From unix. Not tested yet. hahaha
int nodup (char *l[], char s[]);
int nodup (char *l[], char s[])
{
    char *t, *os, c;
    os = s;

	while(t = *l++) 
	{
		s = os;
		
		while(c = *s++)
			if (c != *t++) goto ll;

        if (*t++ == '\0') 
            return (0);
ll:
    }

    return (1);
}
*/

// #not tested.
char *strrev(char *str)
{
    int   start=0;
    int   end=0;
    char  tmp=0;

    start = 0;

    end = strlen(str);
    end = end-1;

    while (start < end)
    {
        tmp = str[start];
        
        str[start] = str[end];
        str[end] = tmp;

        start++;
        end--;
    };

    return (char *) str;
}


/*
void trim_newline (char *path);
void trim_newline (char *path)
{
    char *end = path + strlen(path) - 1;
    while (*end == '\n')
    {
        *end = '\0';
        end--;
    };
}
*/


//
// End.
//

