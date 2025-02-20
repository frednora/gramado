// File: file.h
// Baixo nível usado por stdio.h
// Somente definições, estruturas e funções de baixo nível
// envolvendo arquivos. (FILE) streams. 
// As funções devem começar com '__'  ou com '__file' 

#ifndef __FILE_H__
#define __FILE_H__    1

/*
 * #bsd-like
 * FOPEN_MAX is a minimum maximum, and is the number of streams that
 * stdio can provide without attempting to allocate further resources
 * (which could fail).  Do not use this for anything.
 */
                              /* must be == _POSIX_STREAM_MAX <limits.h> */
//#define FOPEN_MAX     20    /* must be <= OPEN_MAX <sys/syslimits.h> */
//#define FILENAME_MAX  1024  /* must be <= PATH_MAX <sys/syslimits.h> */


#define FOPEN_MAX  (32)

// #todo
#ifndef FILENAME_MAX
    //#define  FILENAME_MAX  (260)    // Nope! 
    #define  FILENAME_MAX  (64) 
    //#define  FILENAME_MAX  (11)     // 8.3
#endif

// #todo
// #define TMP_MAX 	32767


#define GRAMADO_NUMBER_OF_FILES (32)
#define NUMBER_OF_FILES  GRAMADO_NUMBER_OF_FILES


// =========================================================

// mode:
// The mode of I/O, as given in the MODE argument to fopen, etc.
// glibc-style
// #todo: We need it for bsd-like.

typedef struct
{
    unsigned int  __read:1;       /* Open for reading.  */
    unsigned int  __write:1;      /* Open for writing.  */
    unsigned int  __append:1;     /* Open for appending.  */
    unsigned int  __binary:1;     /* Opened binary.  */
    unsigned int  __create:1;     /* Create the file.  */
    unsigned int  __exclusive:1;  /* Error if it already exists.  */
    unsigned int  __truncate:1;   /* Truncate the file on opening.  */
} __io_mode;




/*
 * NB: to fit things in six character monocase externals, the stdio
 * code uses the prefix `__s' for stdio objects, typically followed
 * by a three-character attempt at a mnemonic.
 */

// stdio buffers 

struct __sbuf 
{
    unsigned char *_base;
    int _size;
};



// FILE structure.
// bsd-like
// 2019 - Created by Fred Nora.

struct _iobuf 
{
    int used;
    int magic;
    char *_tmpfname;
// Pointer to the base of the file. 
// Sometimes used as a buffer pointer.
    unsigned char *_base;    
// current position in (some) buffer
// Current position of file pointer (absolute address).
    unsigned char *_p;
// read space left for getc()
    int _r;
// write space left for putc()
    int _w;
// Buffer size.
    int _lbfsize;  
// The size of the file. 
// Tem que ser menor que o buffer.
// BUFSIZ - _cnt.
    int _fsize;
// Quantidade de bytes disponiveis no buffer.
// Number of available characters in buffer.
// Isso também representa o tamanho do arquivo,
// pois quando incluimos bytes no arquivo, então
// Essa quantidade diminue.
    int _cnt;
// flags, below; this FILE is free if 0 
// Flags (see FileFlags). the state of the stream
    unsigned short _flags;

// File descriptor.
    int _file;

// the buffer (at least 1 byte, if !NULL)
    struct __sbuf _bf;

// operations - (bsd-like) 
// #todo: olhar __P em sys/cdefs.h
    void *_cookie;  // cookie passed to io functions 

/*
    int    (*_close) __P((void *));
    int    (*_read)  __P((void *, char *, int));
    fpos_t (*_seek)  __P((void *, fpos_t, int));
    int    (*_write) __P((void *, const char *, int));
*/

//file extension 
    struct __sbuf _ext;

// separate buffer for long sequences of ungetc() 
// saved _p when _p is doing ungetc data 
    unsigned char *_up;

// saved _r when _r is counting ungetc data
    int _ur;

// tricks to meet minimum requirements even when malloc() fails 
    unsigned char _ubuf[3];    // guarantee an ungetc() buffer 
    unsigned char _nbuf[1];    // guarantee a getc() buffer 

// Separate buffer for fgetln() when 
// line crosses buffer boundary.
    struct __sbuf _lb;    // buffer for fgetln() 

// Unix stdio files get aligned to block boundaries on fseek() 
    int _blksize;      // stat.st_blksize (may be != _bf._size) 
    fpos_t _offset;    // current lseek offset 

    int   _charbuf;   

    int eof;
    int error;
    int have_ungotten;
    char ungotten;

    char default_buffer[BUFSIZ];

// 1= is a device; 0= is a file.
// Se é um dispositivo ou não.
// Se for um dispositivo então o dispositivo terá
// na lista deviceList o mesmo offset da stream na list Streams.

    int isDevice;
    int deviceId;  // índice na lista deviceList[] no kernel.

    int iopl;

    struct _iobuf *next;
};


// ===========================
// _iobuf

#define  __stdio_file  _iobuf  // glibc-like
#define  __sFILE       _iobuf  // bsd-like
// ...


//++
#ifndef  __FILE_defined
// The opaque type of streams.
typedef struct _iobuf FILE; 
// typedef struct _iobuf GRAMADO_FILE; 
#define  __FILE_defined   1
// #define  __GRAMADO_FILE_defined   1
#endif
//--

//
// Standard stream.
//

//FILE *stdin;
//FILE *stdout;
//FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;



// Ajust.
#define  __validfp (stream)    \
             ( (void *) stream !=  NULL )


//
// A list of stream in the library.
//

//unsigned long Streams[NUMBER_OF_FILES];
extern unsigned long Streams[NUMBER_OF_FILES];

#endif    //__FILE_H__


