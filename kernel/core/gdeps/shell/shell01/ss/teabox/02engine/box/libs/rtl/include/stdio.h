/*
 * File: stdio.h
 *     Standard IO facilities.
 *     I/O routines support.
 *     c99 - ISO/IEC 9899:1999
 * Environment: ring3.
 * History:
 *     2015 - Created by Fred Nora.
 *     2020 - A lot of new functions.
 */

#ifndef __STDIO_H__
#define __STDIO_H__    1

#include <sys/types.h>
#include <sys/cdefs.h>
#include <stddef.h>
#include <stdarg.h> 

#define CONSOLE_FILE_DESCRIPTOR  1


// tty id desse processo.
// Usado na inicialização da biblioteca.
// See: stdio.c:

//int __libc_tty_id;
extern int __libc_tty_id;



//
// # Modes #
//


// #test
//Os caracteres são colocados em stdout.
#define LIBC_NORMAL_MODE 1
//Os caracteres são pintados na tela.
#define LIBC_DRAW_MODE 2
// ...

//printf using draw mode.
int printf_draw ( const char *fmt, ... );

//setup libc mode
void libc_set_output_mode ( int mode );


#ifdef _BSD_SIZE_T_
typedef _BSD_SIZE_T_  size_t;
#undef _BSD_SIZE_T_
#endif

#ifdef _BSD_SSIZE_T_
typedef _BSD_SSIZE_T_  ssize_t;
#undef _BSD_SSIZE_T_
#endif

#if defined(_POSIX_C_SOURCE)
#ifndef __VA_LIST_DECLARED
typedef __va_list va_list;
#define __VA_LIST_DECLARED
#endif
#endif


// bsd-like
#define _IOFBF  0  // setvbuf should set fully buffered 
#define _IOLBF  1  // setvbuf should set line buffered 
#define _IONBF  2  // setvbuf should set unbuffered 

// _flags
// BSD-sytle
#define __SLBF  0x0001    /* line buffered */
#define __SNBF  0x0002    /* unbuffered */
#define __SRD   0x0004    /* OK to read */
#define __SWR   0x0008    /* OK to write */
/* RD and WR are never simultaneously asserted */
#define __SRW   0x0010    /* open for reading & writing */
#define __SEOF  0x0020    /* found EOF */
#define __SERR  0x0040    /* found error */
#define __SMBF  0x0080    /* _buf is from malloc */
#define __SAPP  0x0100    /* fdopen()ed in append mode */
#define __SSTR  0x0200    /* this is an sprintf/snprintf string */
#define __SOPT  0x0400    /* do fseek() optimization */
#define __SNPT  0x0800    /* do not do fseek() optimization */
#define __SOFF  0x1000    /* set iff _offset is in fact correct */
#define __SMOD  0x2000    /* true => fgetln modified _p text */
#define __SALC  0x4000    /* allocate string space dynamically */
#define __SIGN  0x8000    /* ignore this file in _fwalk */


typedef char *stdio_va_list; 

 
#ifndef NULL
#define NULL ((void *)0)
#endif

 

/*  
 "flags" bits definitions
 */


// Flags for the iobuf structure.
#define _IOREAD  1
#define _IOWRT   2
#define _IORW    0x0080

// Standard stream file descriptors.
#define STDIN_FILENO   0
#define STDOUT_FILENO  1
#define STDERR_FILENO  2



/*
//bsd-like
//#todo: use this one.
#define	_IOFBF	0		// setvbuf should set fully buffered 
#define	_IOLBF	1		// setvbuf should set line buffered 
#define	_IONBF	2		// setvbuf should set unbuffered 
*/

//#define _IOFBF    0x0000  /* full buffered */
//#define _IOLBF    0x0040  /* line buffered */
//#define _IONBF    0x0004  /* not buffered */



/*
 * The buffer size as used by setbuf such that it is equivalent to
 * (void) setvbuf(fileSetBuffer, caBuffer, _IOFBF, BUFSIZ).
 */

#define SMALL_BUFSIZ  512
#define BUFSIZ        1024

// Returned by various functions on 
// end of file condition or error.
#ifndef EOF
#define EOF (-1)
#endif

// System V/ANSI C; 
// this is the wrong way to do this, do *not* use these.

#if defined(_XOPEN_SOURCE) || defined(_NETBSD_SOURCE)
#define P_tmpdir    "/var/tmp/"
#endif
#define L_tmpnam    1024    /* XXX must be == PATH_MAX */


//bsd-like
/* Always ensure that this is consistent with <limits.h> */
//#ifndef TMP_MAX
//#define TMP_MAX  308915776	/* Legacy */
//#endif

#define TMP_MAX  32767

#define _IOMYBUF  0x0008  /* stdio malloc()'d buffer */
#define _IOEOF    0x0010  /* EOF reached on read */
#define _IOERR    0x0020  /* I/O error from system */
#define _IOSTRG   0x0040  /* Strange or no file descriptor */


#ifdef __POSIX__
#define _IOAPPEND  0x0200
#endif


// Consistence:
// Always ensure that these are consistent 
// with <fcntl.h> and <unistd.h>! 


#ifndef SEEK_SET
#define SEEK_SET  0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR  1
#endif

#ifndef SEEK_END
#define SEEK_END  2
#endif

// Clear the file buffer
// and set the postition at the start of the file.
#ifndef GRAMADO_SEEK_CLEAR
#define GRAMADO_SEEK_CLEAR  1000
#endif


/* 
 * This is fairly grotesque, 
 * but pure ANSI code must not inspect the innards 
 * of an fpos_t anyway.  The library internally uses off_t,
 * which we assume is exactly as big as eight chars.
 */


/*
#if (!defined(_ANSI_SOURCE) && !defined(__STRICT_ANSI__)) || defined(_LIBC)
typedef __off_t fpos_t;
#else
typedef struct __sfpos {
	__off_t _pos;
} fpos_t;
#endif
*/


// glibc-style
/* The type of the second argument to `fgetpos' and `fsetpos'.  */
// See: /sys/types.h
typedef __off_t          fpos_t;
typedef __darwin_off_t   fpos_t;
typedef __gramado_off_t  fpos_t;


// ??
// apple?
// Define for new stdio with functions.
// #define	_FSTDIO


// =======================================================

//
// FILE
//

// #importante
// Todo suporte à arquivos foi para libio/file.h
// Os tipos que eles precisam ainda está aqui.

#include <libio/file.h>

// =======================================================
// The symbolic constant L_ctermid is the maximum number of
// characters in the returned pathname.
// Required by POSIX.
#define  L_ctermid  255   

// The macro L_cuserid is an integer constant that indicates 
// how long an array you might need to store a username. 
// Required by POSIX.
#define  L_cuserid  255   

//=====================================================

// Virtual address for vga memory ?
// VA=0x800000 = PA=0x000B8000.

#define SCREEN_START 0x800000 
#define ScreenStart SCREEN_START

#define COLUMNS 80
#define SCREEN_WIDTH COLUMNS

#define ROWS 25
#define SCREEN_HEIGHT ROWS

#define SCREEN_WIDTH COLUMNS
#define HEIGHT ROWS


// ??
// Limits
// Lines and coluns
#define SCREEN_MAX_HEIGHT 256
#define SCREEN_MAX_WIDTH  256


#define REVERSE_ATTRIB 0x70

#define PAD_RIGHT 1
#define PAD_ZERO 2

#define PRINT_BUF_LEN 12


// #todo: Delete!
// Pertence ao teclado.
// #define KEY_RETURN   13    



//
// Obs: 
// O tipo da variável aqui é provisório. (UL).
//

//cursor
//unsigned long g_cursor_x;
//unsigned long g_cursor_y;
extern unsigned long g_cursor_x;
extern unsigned long g_cursor_y;

//char.
//unsigned long g_char_attrib;
extern unsigned long g_char_attrib;


//columns and rows
//@todo: Esse precisa ser inicializado
//Obs: Se essas variáveis forem 0, 
// as rotinas de stdio usarão os valores default.
//COLUMNS e ROWS.
//unsigned long g_columns;
//unsigned long g_rows;
extern unsigned long g_columns;
extern unsigned long g_rows;

// ??
// modo gráfico?
//int g_using_gui; 
extern int g_using_gui; 



//===========================================

//
// Prompt support
//

#define PROMPT_MAX_DEFAULT  BUFSIZ

//char prompt[PROMPT_MAX_DEFAULT]; 
//char prompt_out[PROMPT_MAX_DEFAULT]; 
//char prompt_err[PROMPT_MAX_DEFAULT]; 
extern char prompt[PROMPT_MAX_DEFAULT]; 
extern char prompt_out[PROMPT_MAX_DEFAULT]; 
extern char prompt_err[PROMPT_MAX_DEFAULT]; 

//int prompt_pos;
//int prompt_max;
//int prompt_status;
extern int prompt_pos;
extern int prompt_max;
extern int prompt_status;



// =========================================


//
// == Macros ==================================================
//

//=======================
// fileno

#define  __sfileno(_stream)  ((_stream)->_file)
#define   __fileno(_stream)  ((_stream)->_file)

#define  facility_fileno(_stream)  ((_stream)->_file)


//=======================
// feof

#define  __sfeof(p)     (((p)->_flags & __SEOF) != 0)
#define  __bsd_feof(p)  (((p)->_flags & __SEOF) != 0)

#define  bsd_feof(p)       __sfeof(p)
#define  facility_feof(p)  __sfeof(p)


//=======================
// ferror

#define  __sferror(p)   (((p)->_flags & __SERR) != 0)

#define  bsd_ferror(p)       __sferror(p)
#define  facility_ferror(p)  __sferror(p)

//=======================
// clearerr

#define  __sclearerr(p)  ((void)((p)->_flags &= ~(__SERR|__SEOF)))

#define  bsd_clearerr(p)       __sclearerr(p)
#define  facility_clearerr(p)  __sclearerr(p)

// =========================
// __fgetchar

// #test
// #define  __fgetchar  fgetc(stdin)


//======================================================
// facility


/*
#define FACILITY_SETUP_READ   __SRD
#define FACILITY_SETUP_WRITE  __SWR
#define FACILITY_SETUP_RW     (__SRD|__SWR)
#define facility_putc(__c, __stream)   fputc(__c, __stream)
#define facility_putchar(__c)   fputc(__c, stdout)
#define facility_getc(__stream)   fgetc(__stream)
#define facility_getchar()   fgetc(stdin)


#define facility__sfeof(p)      (((p)->_flags & __SEOF) != 0)
#define facility__sferror(p)    (((p)->_flags & __SERR) != 0)
#define facility__sclearerr(p)  ((void)((p)->_flags &= ~(__SERR|__SEOF)))
#define facility__sfileno(p)    ((p)->_file)
*/


//===========================================
// Prototypes.
//===========================================


//
// ==== low level ====
//


//
// Root 1.
//

int __fflush ( FILE *stream);
int fflush_all(void);
int fflush (FILE *stream);
int __getc ( FILE *stream );
int __putc (int ch, FILE *stream);
int ____bfill(FILE *stream);

int getc (FILE *stream);
int putc (int ch, FILE *stream);
int fgetc ( FILE *stream );
int fputc (int ch, FILE *stream);

//
// Root 2
//

int getchar (void);
int putchar (int ch);

//
// Root 3
//

char *gets (char *s);
int puts (const char *s);


//
// Root 4
//

char *fgets (char *s, int size, FILE *stream);
int fputs ( const char *s, FILE *stream );

//
// Root 5
//

int getw (FILE *stream);
int putw (int w, FILE *stream);



// Serial port debug.
void debug_print (char *string);



void clearerr (FILE* stream);

char *ctermid (char *s);

int fpurge (FILE *stream);

int fsetpos (FILE *stream, const fpos_t *pos);

int fgetpos (FILE *stream, fpos_t *pos);

FILE *fmemopen (void *buf, size_t size, const char *mode);

FILE *open_wmemstream (wchar_t **ptr, size_t *sizeloc);

FILE *open_memstream (char **ptr, size_t *sizeloc);

FILE *stdio_make_file ( int fd, const char *mode );
FILE *fdopen (int fd, const char *mode);
FILE *freopen (const char *pathname, const char *mode, FILE *stream);

FILE *tmpfile (void);
char *tempnam (const char *dir, const char *pfx);
char *tmpnam_r (char *s);
char *tmpnam (char *s);



//int vfscanf (FILE *stream, const char *format, va_list ap);
//int vsscanf (const char *str, const char *format, va_list ap);
//int vscanf (const char *format, va_list ap);
//int vsnprintf (char *str, size_t size, const char *format, va_list ap);


//
//  vsprintf and print
//


// Just for fun :^) 
//int Wirzenius_Torvalds_vsprintf (char *buf, const char *fmt, va_list args);
//int Torvalds_printf (const char *fmt, ...);




//int vdprintf (int fd, const char *format, va_list ap); 

//int dprintf (int fd, const char *format, ...);



unsigned int filesize (FILE * fp);
char * fileread (FILE * fp);


// remove - remove a file or directory 
// On success, zero is returned. On error, -1 is returned, and 
// errno is set appropriately. 
// C89, C99, 4.3BSD, POSIX.1-2001. 
int remove (const char *pathname); 

int fileno ( FILE *stream );
 
 
// 
// File Operations
// 
 
FILE *fopen ( const char *filename, const char *mode ); 
FILE *fopen2 ( const char *filename, const char *mode ); 
int fclose (FILE *stream); 


//
// Normal output.
//


void nputs (char *cp, int len);

int 
prints (
    char **out, 
    const char *string, 
    int width, 
    int pad );

int 
printi (
    char **out, 
    int i, 
    int b, 
    int sg, 
    int width, 
    int pad, 
    int letbase );

int print (char **out, int *varg);
void printchar (char **str, int c);
void outbyte (int c);
void _outbyte (int c);

 
// 
// Formatted Output
// 

int sprintf (char *out, const char *format, ...); 
int fprintf (FILE *stream, const char *format, ...); 

//int vfprintf ( FILE *stream, const char *format, stdio_va_list argptr );

void perror (const char *str);

//int stdout_printf (const char *format, ...);
//int stderr_printf (const char *format, ...);




//
// == printf saga ============================================
//


/*
// #todo
// Parece que essa versao pode ser usado com nosso helper de printf.
// See: https://man7.org/linux/man-pages/man3/asprintf.3.html
//int asprintf(char **strp, const char *fmt, ...);
//int vasprintf(char **strp, const char *fmt, va_list ap);
*/

// Non traditional. It is working very well.
//int printf(const char *format, ...); 

// Traditional. Not tested.
//int printf2 (const char *format, ...); 

// Another implementation. (I don't remember).
//int printf3 (const char *fmt, ...);


// ===================================

//
// kinguio printf
//

#define HEX_LEN  8
#define ____INT_MAX  2147483647

void kinguio_i2hex(unsigned int val, char* dest, int len);
char *kinguio_itoa (int val, char *str);
int kinguio_printf(const char *fmt, ...);
int printf(const char *fmt, ...);
void kinguio_puts(const char* str);
int kinguio_vsprintf(char * str,const char * fmt, va_list ap);
//#define printf  kinguio_printf
// ===================================

// Next line sprintf,
int nlsprintf ( char *out, const char *format, ... );

// Find next line
char *stdio_nextline(char *string);

void rewind (FILE *stream);

//
// Normal input.
//
 
// input(): 
// Usado por interpretadores de comando.
// Recebem input e colocam em prompt[].

unsigned long input (unsigned long ch);



//
// Formatted Input
//

int scanf ( const char *fmt, ... );

// #todo Test it.
int fscanf (FILE *stream, const char *format, ... ); 

// New way. (since C99)
// int fscanf ( FILE *restrict stream, const char *restrict format, ... );

int sscanf(const char *str, const char *format, ...);
 
long ftell (FILE *stream);


char *uclib_gets (char *str); 



//--save
int getchar2 (void);
char *gets2 (char *s);
int puts2 (const char *str); 
char *fgets2 (char *s, int count, FILE *fp);
int fputs2 ( const char *str, FILE *stream );

int ungetc ( int c, FILE *stream );



//++
//===================
// Isso vai ler no buffer da stream que está em ring3.
size_t fread (void *ptr, size_t size, size_t n, FILE *fp);
// Isso vai escrever no buffer da stream que está em ring3.
size_t fwrite (const void *ptr, size_t size, size_t n, FILE *fp);
// Isso vai ler no arquivo que está em ring0.
int linux_fgetc (FILE *f);
// Isso vai escrever no arquivo que está em ring0.
int linux_fputc (int c, FILE *f);
//===================
//--



// prompt support.
// coloca no prompt[] em ring3 e exibe com uma rotina em ring0.
int prompt_putchar ( int c, int con_id );
int prompt_put_string ( char *string );
int prompt_strcat (char *string);
int prompt_flush ( int con_id );
void prompt_clean (void);



//
// Direct Input and Output Functions
//
 
 
 
//
// File Positioning Functions
//

int fseek (FILE *stream, long offset, int whence);



//
// Error Functions
//
 
int feof( FILE *stream );
int ferror( FILE *stream ); 

void scroll (void);

// ??
int drawBar (int color);  


//#todo
int snprintf (char *str,size_t count,const char *fmt,...);

//int vprintf (const char *fmt, va_list ap);

//#test
//ainda não foi testada
int fscanf (FILE *stream, const char *format, ... );

// inicializa o fluxo padrão para o processo.
int stdio_initialize_standard_streams (void);


// see: 
// https://linux.die.net/man/3/setvbuf

void setbuf (FILE *stream, char *buf);
void setbuffer (FILE *stream, char *buf, size_t size);
void setlinebuf (FILE *stream);
int setvbuf (FILE *stream, char *buf, int mode, size_t size);

// Adapted from unix v32.
// Not tested yet.
void
_strout ( 
    int count, 
    char *string, 
    int adjust, 
    FILE *file, 
    int fillch );

int unix_get (int ifile); 

//
// initialization
//

int libcStartTerminal (void);

/*Inicialização da biblioteca*/
void stdioInitialize(void);

int
rtl_GetS(
    unsigned char *buf,
    size_t size );

int rtl_y_or_n(void);
int rtl_are_you_sure(void);

int stdio_atoi (char *s);
void stdio_fntos (char *name);

#endif    //__STDIO_H__

//
// End.
//
