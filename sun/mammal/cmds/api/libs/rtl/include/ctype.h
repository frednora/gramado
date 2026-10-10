/*
 * File: ctype.h
 * User mode C library for Gramado.
 * History:
 *     2018 - Created by Fred Nora.
 *     2018 - Linux stuff
 */

#ifndef __CTYPE_H
#define __CTYPE_H    1

//linux
#define _U	0x01	// upper 
#define _L	0x02	// lower 
#define _D	0x04	// digit 
#define _C	0x08	// cntrl 
#define _P	0x10	// punct 
#define _S	0x20	// white space (space/lf/tab) 
#define _X	0x40	// hex digit 
#define _SP	0x80	// hard space (0x20) 


//gramado
//Apelidos para as letras.
#define _UPPER   _U  //letra maiúscula
#define _LOWER   _L  //letra minúscula.
#define _DIGIT   _D   //dígito digit[0-9]
#define _SPACE   _S  // \n \r \t
#define _PUNCT   _P  /* punctuation */
#define _CONTROL _C	 /* control character */
#define _HEX     _X  /* hexadecimal digit */
#define _BLANK   _B  /* space char */
//.


//linux
//in lib/libc-src/ctype.c

extern const unsigned char _ctype[];
#define __ismask(x) (_ctype[(int)(unsigned char)(x)])



//partes: básico, intermediário e definitivo.

//
// # básico #
//

//#obs
//estamos usando esse porque a 
//função que criamos não está funcionando.
//#obs: então outras funções aqui podem estar falahndo ... tem que testar todo o ctype.

//#define isalnum(char) ((char >= 'a' && char <= 'z') || (char >= 'A' && char <= 'Z') || (char >= '0' && char <= '9'))

//linux 
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
static inline int isdigit(int c)
{
    return '0' <= c && c <= '9';
}
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
 
//linux 
#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)


//linux

static inline unsigned char __tolower(unsigned char c)
{
	if (isupper(c))
		c -= 'A'-'a';
	return c;
};


static inline unsigned char __toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a'-'A';
	return c;
}


#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c) //#bugbug: isso está redefinido em stdio.c vamos tirar de lá.
//#define toupper(c)  ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))


/* Fast implementation of tolower() for internal usage. 
   Do not use in your code. */
static inline char _tolower(const char c)
{
	return c | 0x20;
};


/* Fast check for octal digit */
static inline int isodigit(const char c)
{
	return c >= '0' && c <= '7';
};


//
// # intermediário #
//


#define _isdigit(c) ((c) >= '0' && (c) <= '9') 
#define _isascii(c) (((unsigned) c) <= 0x7f)
#define _toascii(c) (((unsigned) c) &  0x7f)


//
// # definitivo #
//
 

//Sr.No.	Function & Description
//1	
//This function checks whether the passed character is alphanumeric.
//#bugbug: isso não funciona.
//int isalnum(int c);


//2	
//This function checks whether the passed character is alphabetic.
//int isalpha(int c);


//3	
//This function checks whether the passed character is control character.
//int iscntrl(int c);


//4	
//This function checks whether the passed character is decimal digit.
//int isdigit(int c);


//5	
//This function checks whether the passed character has graphical representation using locale.
//int isgraph(int c);


//6	
//This function checks whether the passed character is lowercase letter.
//int islower(int c);

//7	
//This function checks whether the passed character is printable.
//int isprint(int c);


//8	
//This function checks whether the passed character is a punctuation character.
//int ispunct(int c);


//9	
//This function checks whether the passed character is white-space.
//int isspace(int c);


//10	
//This function checks whether the passed character is an uppercase letter.
//int isupper(int c);


//11	
//This function checks whether the passed character is a hexadecimal digit.
//int isxdigit(int c);




//
// # Explicações #
//



//Sr.No.	Function & Description
//1	
//int tolower(int c);
//This function converts uppercase letters to lowercase.

//2	
//int toupper(int c)
//This function converts lowercase letters to uppercase.



//Character Classes
//Sr.No.	Character Class & Description
//1	
//Digits

//This is a set of whole numbers { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }.

//2	
//Hexadecimal digits

//This is the set of { 0 1 2 3 4 5 6 7 8 9 A B C D E F a b c d e f }.

//3	
//Lowercase letters

//This is a set of lowercase letters { a b c d e f g h i j k l m n o p q r s t u v w x y z }.

//4	
//Uppercase letters

//This is a set of uppercase letters {A B C D E F G H I J K L M N O P Q R S T U V W X Y Z }.

//5	
//Letters

//This is a set of lowercase and uppercase letters.

//6	
//Alphanumeric characters

//This is a set of Digits, Lowercase letters and Uppercase letters.

//7	
//Punctuation characters

//This is a set of ! " # $ % & ' ( ) * + , - . / : ; < = > ? @ [ \ ] ^ _ ` { | } ~

//8	
//Graphical characters

//This is a set of Alphanumeric characters and Punctuation characters.

//9	
//Space characters

//This is a set of tab, newline, vertical tab, form feed, carriage return, and space.

//10	
//Printable characters

//This is a set of Alphanumeric characters, Punctuation characters and Space characters.

//11	
//Control characters

//In ASCII, these characters have octal codes 000 through 037, and 177 (DEL).

//12	
//Blank characters

//These are spaces and tabs.

//13	
//Alphabetic characters

//This is a set of Lowercase letters and Uppercase letters.



/*
' '     (0x20)	space (SPC)
'\t'	(0x09)	horizontal tab (TAB)
'\n'	(0x0a)	newline (LF)
'\v'	(0x0b)	vertical tab (VT)
'\f'	(0x0c)	feed (FF)
'\r'	(0x0d)	carriage return (CR)
*/

#endif    





 








