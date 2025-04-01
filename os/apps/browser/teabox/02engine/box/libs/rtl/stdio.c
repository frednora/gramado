/*
 * File: stdio.c
 *     I/O routines support.
 *     c99 - ISO/IEC 9899:1999
 * Environment: ring3.
 * Credits:
 *     printf: https://www.menie.org/georges/embedded/small_printf_source_code.html
 *     stdio_fntos: Luiz Felipe
 *     Serenity OS. (bsd)
 *     2015 - Created by Fred Nora.
 *     2020 - New functions.
 */
// See:
// http://kirste.userpage.fu-berlin.de/chemnet/use/info/libc/libc_7.html 


#include <limits.h>
#include <stdio.h>
#include <types.h> 
#include <stdarg.h> 
#include <stdio_ext.h>
#include <stddef.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <rtl/gramado.h> 


#define  SYSTEMCALL_READ_LBA    1
#define  SYSTEMCALL_WRITE_LBA   2
#define  SYSTEMCALL_READ_FILE   3
#define  SYSTEMCALL_WRITE_FILE  4
// ...

// Why?
#define VK_RETURN     0x1C 
#define VK_BACKSPACE  0x0E 
#define VK_BACK       0x0E 
#define VK_TAB        0x0F 


// Globals
int __libc_tty_id=0;
unsigned long g_cursor_x=0;
unsigned long g_cursor_y=0;
unsigned long g_char_attrib=0;
unsigned long g_columns=0;
unsigned long g_rows=0;
int g_using_gui=0;

// Global
// prompt support.
char prompt[PROMPT_MAX_DEFAULT]; 
char prompt_out[PROMPT_MAX_DEFAULT]; 
char prompt_err[PROMPT_MAX_DEFAULT]; 

//char prompt_text[] = "$ ";
//...

int prompt_pos=0;
int prompt_max=0;
int prompt_status=0;

// Globals
// standard streams support.
FILE *stdin;
FILE *stdout;
FILE *stderr;

unsigned long Streams[NUMBER_OF_FILES];

// The main environment.
// crt0.c will setup this.
char **environ;

// #importante: 
// Tem que inicializar isso.

static int __libc_output_mode=0;
static int terminal___PID=0;

/*
// #todo
const char *errno_list[32] = {
    "error 0",
    "error 1",
    "error 2",
    // ...
};
*/

//
// =========================================================
//

//++
// Internas.
// Estamos falando do posicionamento do cursor dentro da janela 
// e não dentro do terminal. 
void stdioSetCursor( unsigned long x, unsigned long y );
unsigned long stdioGetCursorX(void); 
unsigned long stdioGetCursorY(void); 
static size_t stdio_strlen (const char *s);
static char *_vsputs_r(char *dest, char *src);
//--

//
// =========================================================
//


/*
char *out_char(char *dst,char ch);
char *out_char(char *dst,char ch)
{
  if (dst==NULL)
    putchar(ch);
  else
    *(dst++)=ch;
  return dst;
}
*/


/*
char terry_toupper(char ch);
char terry_toupper(char ch)
{
    if (ch>='a' && ch<='z')
        return ch + 'A'-'a'; 
    else 
        return ch;
}
*/

int rtl_y_or_n(void)
{
    static int ch=0;
    printf ("Type: 'y' or 'n'\n");

// Get and convert to capital.
    while (1)
    {
        ch = (int) fgetc(stdin);
        ch = (int) toupper(ch);
        if (ch == 'Y'){
            printf ("~YES!\n");
            return TRUE;
        } else if (ch == 'N'){
            printf ("~NO!\n");
            return FALSE;
        };
    };
    // return (int) -1;
}

int rtl_are_you_sure(void)
{
    printf ("Are You sure?\n");
    return (int) rtl_y_or_n();
}

// stdio_atoi:
// Talvez isso possa ir para o topo do 
// arquivo para servir mais funções.
int stdio_atoi (char *s)
{
    int rv=0; 
    char sign=0;

// Skip till we find either a digit or '+' or '-'.
    while (*s){
        if (*s <= '9' && *s >= '0'){ break; }
        if (*s == '-' || *s == '+'){ break; }
        s++;
    };

    if (*s == '-'){
        sign=1;
    }

    //     sign = (*s == '-');
    if (*s == '-' || *s == '+')
    {
        s++;
    }

    while (*s && *s >= '0' && *s <= '9') 
    {
        rv = (rv * 10) + (*s - '0');
        s++;
    };

    if (sign){ 
        return (-rv);
    }else{ 
        return (rv);
    };

    // return (sign ? -rv : rv);
}


// stdio_fntos:
// rotina interna de support.
// isso deve ir para bibliotecas depois.
// não tem protótipo ainda.
// Credits: Luiz Felipe

void stdio_fntos (char *name)
{
    int i=0;
    int ns=0;
    char ext[4];
    //const char ext[4];

    ext[0] = 0; 
    ext[1] = 0; 
    ext[2] = 0; 
    ext[3] = 0;

// Transforma em maiúscula enquanto não achar um ponto.

    while ( *name && *name != '.' )
    {
        if ( *name >= 'a' && *name <= 'z' )
        {
            *name -= 0x20;
        }
        name++;
        ns++;
    };

// Aqui name[0] é o ponto.
// Então constrói a extensão.

    for ( i=0; i < 3 && name[i+1]; i++ )
    {
        //Transforma uma letra da extensão em maiúscula.

        //if (name[i+1] >= 'a' && name[i+1] <= 'z')
        //    name[i+1] -= 0x20;

        //ext[i] = name[i+1];

        // #testando
        //Se não for letra então não colocamos no buffer de extensão;

        if (name[i+1] >= 'a' && name[i+1] <= 'z')
        {
            name[i+1] -= 0x20;
            ext[i] = name[i+1];
        }
    };

// Acrescentamos ' ' até completarmos as oito letras do nome.

    while (ns < 8)
    {
        *name++ = ' ';
        ns++;
    };

// Acrescentamos a extensão.

    for (i=0; i < 3; i++)
    {
        *name++ = ext[i];
    };

    *name = '\0';
    //*name = 0;
}

/*
 # todo
void __init_FILE (FILE *fp, int fd, unsigned char *buffer, int flags );
void __init_FILE (FILE *fp, int fd, unsigned char *buffer, int flags )
{
    if(!fp)
        return;
        
    fp->used = 1;
    fp->magic = 1234;        
        
    //fp->iopl = 3;
            
    fp->fd = fd;
    fp->_base = (unsigned char *) buffer;
    fp->_lbfsize = BUFSIZ;
    fp->_flags = flags;
}
*/


/*
 # todo
FILE *__make_FILE (int fd)
{
    FILE *fp = (FILE *) malloc ( sizeof(FILE) );
    
    if (!fp)
        return NULL;
    
    //fp->used = 1;
    //fp->magic = 1234;        
     
    // CLean 
    memset (fp, 0, sizeof(FILE));

    unsigned char *buffer;
    buffer = (unsigned char *) malloc (BUFSIZ);
    __init_FILE ( (FILE *) fp, (int) fd, (unsigned char *) buffer, 0 );

    return (FILE *) fp;
}
*/

/*
 * remove:
 *     It removes a file from the file system.
 */
// It calls unlink(2) for files, and rmdir(2) for directories. 
// But here in ring3 we do not know if the file is a diretory or a file.
// Maybe we need to call a function to tell us if the file is a file
// or a diretory ... or simply call the the remove() system call 
// and the kernel will do all the job.
// #todo
// https://linux.die.net/man/3/remove

int remove (const char *pathname)
{
    debug_print ("remove: [TODO] It removes a file from the file system\n");
    if( (void*) pathname == NULL){
        goto fail;
    }
    if (*pathname == 0){
        goto fail;
    }

// #todo

fail:
    return (int) (-1);
}

// Adapted from unix v32.
// Not tested yet.
void
_strout ( 
    int count, 
    char *string, 
    int adjust, 
    FILE *file, 
    int fillch )
{

//#todo: filtros.

    while (adjust < 0)
    {
        if (*string=='-' && fillch=='0')
        {
            putc (*string++, file);
            count--;
        }
        putc (fillch, file);
        adjust++;
    };

    while (--count>=0){
        putc (*string++, file);
    };

    while (adjust) {
        putc (fillch, file);
        adjust--;
    };
}

//
// == low level =====================
//

// fflush() implementation.
// Called by fflush();
int __fflush (FILE *stream)
{
    ssize_t nwrite = -1;
    register size_t Count=0;

    //debug_print( "__fflush:\n");

    // FIXME: fflush(NULL) should flush all open output streams.
    //ASSERT(stream);

    if ((void *) stream == NULL){
        return (int) fflush_all();
    }

    // #todo
    // Not buffered. 
	// if (stream->_flags & _IONBF)
		//return (0);

    // #todo
    // Not writable. 
	//if (!(stream->flags & _IOWRITE))
		//return (0);

    //if ( !stream->_w )
        //return 0;

// Have an invalid ring3 buffer.
    if ((void *) stream->_base == NULL){
        debug_print( "__fflush: [ERROR] Invalid ring3 buffer\n");
        return (int) (-1);
    } 

// The 'write offset' is the number of bytes to write.
// So, it can't be '0' or negative.
// #todo
// Return '0' if there is nothing to flush.

    //Count = stream->_ptr - stream->_base;
    Count = (size_t) stream->_w;
    if (Count <= 0)
    {
        stream->_p = stream->_base;
        stream->_w = 0;    // ajust.
        //stream->_r = 0;  // ajust.
        // #debug
        debug_print ( "__fflush: [FAIL] _w\n");
        return 0;  // OK.
        //return (int) (-1);
    }

// #todo: 
// This is the desired way.
// Nesse teste escreveremos em stdout. Seu descritor indicará
// que é um dispositivo do tipo console. O kernel escreverá no console 0.  

    if (Count > BUFSIZ){
        Count=BUFSIZ;
    }

//
// Write
//

//#ifdef _POSIX_
    nwrite = write( fileno(stream), stream->_base, Count );
//#else
//    nwrite = __write( fileno(stream), stream->_base, Count );
//#endif

// Fail
    if (nwrite <= 0)
    {
        //#debug: Recursive.
        //printf ("__fflush: [FAIL] nwrite Count={%d} \n", Count);
        Count = 0;
        // Ajust to the next flush.
        stream->_p = stream->_base;
        stream->_w = 0;
        // #todo
        // stream->_flags |= _IOERROR; 
        // stream->error = errno;
        return EOF;
    }

// Error
// #todo
// Something is wrong
    if (nwrite != Count)
    {
        debug_print("__fflush: [DEBUG] nwrite != Count\n");
        // #todo
        // stream->_flags |= _IOERROR; 
        // stream->error = errno;
        // #todo
        //return EOF;
    }

// Re-initialize
    stream->_p = stream->_base;
    stream->_w = 0;
    //stream->error = 0;
    //stream->eof = 0;
    //stream->have_ungotten = false;
    //stream->ungotten = 0;

// done
    return 0;
}

// #todo:
// We need a table.
int fflush_all(void)
{
    printf("fflush_all: #todo\n");
    return 0;
}

// #importante
// A informaçao eh entregue para ser salva no disco,
// Ela nao eh salva no disco.
// O sistema operacional decide qual eh a melhor hora,
// Ao menos que seja explicitamente chamado alguma rotina
// dizendo que eh pra salvar no disco ... sync.
// Eh um tipo de commit. Um comprometimento.
// Note that fflush() only flushes the user space buffers 
// provided by the C library. To ensure that the data is 
// physically stored on disk the kernel buffers must be flushed too. 
// The contents of the stream buffer are written 
// to the underlying file or device and the buffer is discarded.
// A call to fflush negates the effect of any prior 
// call to ungetc for the stream. The stream remains open after the call.
// ms-commit
// The commit-to-disk feature of the run-time library 
// lets you ensure that critical data is written directly to disk 
// rather than to the operating-system buffers.
// The fflush() function ensures that data has been written 
// to the kernel buffer pools from your application's buffer 
// (either for a single file, or for all output files if you use fflush(0) or fflush(NULL)).
// If a file is open, can write and is buffered,
// so we flush the ring3 buffer. It will send the data
// to the file in ring0.
// + if the stream is a console device, so the data will be show in the screen
// + if it is a regular file, so it will be saved into the disk, i guess (commit)
// See:
// sync, syncfs - commit buffer cache to disk.
// sync() causes all buffered modifications to file metadata and 
// data to be written to the underlying file systems.
// fflush - flush a stream
// For  output  streams,  fflush() forces a write of all 
// user-space buffered data for the given output or 
// update stream via the stream's underlying write function.  
// For input streams, fflush() discards any buffered data 
// that has been fetched from the underlying file, 
// but has not been consumed by the application.  
// The open status  of the stream is unaffected.
// See:
// for sync(), see unistd.c
// IN:
// NULL is ok.
// NULL will flush all the streams in the table[].
// #todo: We need a talbe here in the lib.

int fflush(FILE *stream)
{
    if ((void*) stream == NULL){
        return (int) fflush_all();
    }
    return (int) __fflush(stream);
}

//
// == Root 1 =================
//

// Buffer fill.
// Precisamos pegar o conteúdo do arquivo que está em ring0 e
// colocar no buffer em ring3 para o __getc pegar um byte.
// OUT: nreads. (quantos bytes foram lidos.)

int ____bfill (FILE *stream)
{
    int n_read = 0;

    if ((void *) stream == NULL)
    {
        debug_print("____bfill: [FAIL] struct\n");
        printf     ("____bfill: [FAIL] struct\n");
        goto fail;
    }

// Check buffer size
    if (stream->_lbfsize != BUFSIZ)
    {
        debug_print ("____bfill: [FAIL] _lbfsize \n");
        printf      ("____bfill: [FAIL] _lbfsize %d\n",
            stream->_lbfsize);
        stream->_cnt = 0;
        // #debug
        // provisorio
        exit(1);
        //return EOF;
    }

    // ...

// Read file
// #importante:
// Colocaremos no offset e não na base. 
// Pois esse é o local onde __getc vai pegar.
// Só podemos colocar o quanto ainda cabe no buffer.
 
// O tamanho do buffer menos o quanto ja foi consumido.
    //size_t how_much = ( stream->_lbfsize - (stream->_p - stream->_base) );

// cnt deve ser 0, pois ____bfill foi chamado
// quando o buffer acabou. então vamos encher o buffer novamente
// e atualizarmos o cnt.
    //size_t how_much = stream->_lbfsize;
    //if ( how_much <= 0 || how_much > BUFSIZ )
    //{
    //    debug_print ("____bfill: [BUGBUG] buffer fail\n");
    //    printf      ("____bfill: [BUGBUG] buffer fail how_much=%d\n", how_much);
    //    stream->_cnt = 0; 
    //    exit(1); //debug
        //return EOF;
    //}
    
// #importante
// Se o buffer acabou é porque o ponteiro se deslocou até o fim.
// Temos que reiniciar.

    stream->_cnt = (BUFSIZ-1);
    stream->_p = stream->_base;
    stream->_w = 0;
    stream->_r = 0;

//
// Read
//

// cnt tem a quantidade disponível para leitura.
// Read vai ler, la no kernel, a partir da última posição.
// A intenção dessa rotina é reencher o buffer em ring3,
// pegando uma nova parte do arquivo que está em ring0.

// #todo
    //if ( stream->_flags & ____USE_BUFFER ){
    //n_read = (int) read ( 
    //                   fileno(stream), 
    //                   stream->_p,
    //                   stream->_cnt );
    //}

    n_read = (int) read( fileno(stream), stream->_p, BUFSIZ );
    if (n_read < 0)
    {
        stream->_fsize = 0;
        stream->_cnt = BUFSIZ;
        goto fail;
    }
    if (n_read == 0)
    {
        stream->_fsize = 0;
        stream->_cnt = BUFSIZ;
        goto fail;
    }
    // Enchemos o buffer
    if ( n_read >= (BUFSIZ-1) )
    {
        stream->_fsize = BUFSIZ;
        stream->_cnt = 0;  // Nao ha mais espaco.
        stream->_p = stream->_base;
        stream->_w = 0;
        stream->_r = 0;
        // Retorna a quantidade de bytes carregados no buffer.
        return (int) BUFSIZ;
    }

// Update
// Number of available characters in buffer.
// >>> O buffer deve ser do tamanho da quantidade
// de bytes lidos la no kernel
// se a quantidade estiver dentro dos limites padrões.

    stream->_fsize = n_read;
    stream->_cnt = (stream->_lbfsize - n_read);
    stream->_p = stream->_base;
    stream->_w = 0;
    stream->_r = 0;

//done:
// Retornamos a quantidade disponível para leitura.
// Isso será usado por __getc.
// Retorna a quantidade de bytes carregados no buffer.
    return (int) n_read;
fail:
    return (int) EOF;
}

// __getc:
// #todo:
// #importante
// Se tem alguma coisa no buffer em ring3 a gente pega,
// se não tem, então precisamos usar o read() e copiar do arquivo
// em ring0 para o buffer em ring3.
// >>>> _fillbuf()
// Cada processo pode ter uma 'vista' do arquivo em ring3.
// Então:
// + Se acabou nosso buffer em ring3, enchemos o buffer novamente.
// + Se não acabou então pegamos um byte no buffer.
// Isso vale para arquivos criados com fopen cujo buffer ja começa vazio.
// Vamos ler do buffer da stream, em ring3.

int __getc(FILE *stream)
{
// Get a byte from a ring3 local buffer.
// If the buffer is empty, so 
// refill the buffer using read().

    register int ch=0;
    int nreads=0;

    if ((void *) stream == NULL)
    {
        debug_print("__getc: stream\n");
        printf     ("__getc: stream\n");
        goto fail;
    }

    // File is not readable.
    // if (!(stream->flags & _IOREAD))
        //return (EOF);

// have ungotten?
// Pegamos o ungotten.
// não mexemos nos contadores.
// Do NOT have forgotten!
// Pega o char no posicionamento absoluto do arquivo.
// Ajust file.
// Return the char.

    if (stream->have_ungotten == TRUE)
    {
        ch = (int) stream->ungotten;
        stream->have_ungotten = FALSE;
        return (int) ch;
    }

// Se o ponteiro de leitura for inválido.
// Não podemos acessar um ponteiro nulo ... 
// no caso endereço.

    if ((void*) stream->_p == NULL)
    {
        debug_print("__getc: Invalid stream->_p\n");
        printf     ("__getc: Invalid stream->_p\n");
        goto fail;
    }

// Se a quantidade de bytes restantes no buffer for maior que zero 
// Ou se for menor igual ao buffer padrão. (#bugbug: Talvez não pode ser igual)
// Se o offset de leitura for menor que o tamanho do arquivo.

    //if ( stream->_cnt > 0 && 
    //     stream->_cnt < BUFSIZ && 
    //     stream->_r < stream->_fsize )
    //{}
    
    if ( stream->_cnt > 0 && 
         stream->_cnt <= BUFSIZ && 
         stream->_r < stream->_fsize )
    {
        // Get the char.
        ch = (int) *stream->_p;

        stream->_p++;  // pointer
        stream->_r++;  // offset
        // Diminui a quantidade de bytes restantes no buffer em ring3.
        stream->_cnt--;
        return (int) ch;
    }

// Depois de consumirmos todo nosso buffer local,
// até chegarmos ao fim do arquivo, não do buffer.
// Ou chegamos ao fim do buffer.
// Atingimos o fim do buffer na libc.
// Se acabou o buffer.
// O _cnt decrementou e chegou a zero.
// Significa que: Não há mais caracteres disponíveis entre 
// stream->_ptr e o tamanho do buffer.
// Então vamos encher o buffer NOVAMENTE em ring3 dessa stream
// e atualizarmos o cnt.

    if ( stream->_cnt <= 0 || stream->_r >= stream->_fsize )
    {
        //debug_print("__getc: [DEBUG] ring3 buffer is empty\n");

        // Coloque bytes no buffer dessa stream.
        nreads = (int) ____bfill(stream);

        // Falhou. Não conseguimos ler.
        // nada no buffer local.
        if (nreads <= 0)
        {
            // O buffer não tem dados,
            // então o tamanho do arquivo é 0. 
            stream->_fsize = 0;
            // Temos todo o espaço disponível.
            // #bugbug: Actually we have nothing.
            stream->_cnt = BUFSIZ;
            //stream->_cnt = 0;
            
            stream->_p = stream->_base;
            stream->_w = 0;
            stream->_r = 0;
            goto fail;
        }

        // Ok
        // Temos bytes no buffer dessa stream. Então vamos pegar um.
        // #importante: Eles foram colocados no offset e não na base.
        // Atualiza.
        // Retorna o buffer que pegamos do buffer em ring3.
        if (nreads > BUFSIZ){
            nreads = BUFSIZ;
        }

        // Se nosso buffer tem alguma coisa.
        // Os elementos da estrutura foram configurados por ____bfill.
        // O ponteiro está na base. Vamos ler o primeiro byte.
        if (nreads > 0)
        {
            ch = (int) *stream->_p;
            stream->_p++;    // Incrementamos o ponteiro de trabalho.
            stream->_r++;    // Incrementamos o offset de leitura.
            // ____bfill  atualizou os valores dos offsets,
            // ja podemos decrementar.
            stream->_cnt--;
            return (int) (ch & 0xFF);
        }

        // Algo deu errado.
        // Vamos para o fim do buffer
        // para que ele seja cheio novamente.        
        stream->_cnt = 0;
        stream->_w = BUFSIZ;  // o escritor eta no fim do arquivo.
        stream->_r = BUFSIZ;  // o leitor eta no fim do arquivo.
        goto fail;
    }

// == Read ring3 buffer ====================
// Ok. 
// Vamos ler o buffer, porque ele não está vazio.
// #bugbug
// FAIL!

    debug_print("__getc: [BUGBUG] Unexpected return\n");
    printf     ("__getc: [BUGBUG] Unexpected return\n");

fail:
    return (int) EOF;
}

int __putc(int ch, FILE *stream)
{

// Invalid char
    //if(ch<0){
    //    goto fail;
    //}

// Invalid stream
    if ((void *) stream == NULL)
    {
       debug_print("__putc: stream\n");
       printf     ("__putc: stream\n");
       goto fail;
    }

// Se nosso ponteiro de escrita é 
// maior que o tamanho do buffer.
// Não podemos escrever além do buffer.

    //if (stream->_w > stream->_lbfsize)
    if (stream->_w >= BUFSIZ)
    {
       debug_print("__putc: [FAIL] Overflow\n");
       printf     ("__putc: [FAIL] Overflow\n");
       stream->_cnt = 0;  // Fim do arquivo.
       goto fail;
    }

// Insert into the buffer and increment the offset.
// All the chars. '\n' is also include.
    stream->_base[stream->_w] = ch;
    stream->_w++;
// #test
    //stream->_cnt = ( BUFSIZ - stream->_w ); 

// Overflow
// Se chegamos ao fim do arquivo.
    if (stream->_w >= BUFSIZ)
    {
        debug_print("__putc: Overflow 2\n");
        printf     ("__putc: Overflow 2\n");
        stream->_cnt = 0;
        fflush(stream);
        return (int) ch;
    }

    // if (stream->_flags == _IONBF || 
    //    (stream->_flags == _IOLBF && ch == '\n'))
    // {}

// Se o char que colocamos no buffer é um '\n'.
// Então vamos enviar o buffer para o kernel.
// Assim o kernel vai exibir o buffer no console atual.

    if (ch == '\n')
    {
        fflush(stream);
        return (int) ch;
    }

    //if (stream->eof || stream->error)
        //return EOF;

    return (int) ch;

fail:
    return (int) EOF;
}

// Don't change it.
int getc (FILE *stream){
    return (int) __getc (stream);
}

// Don't change it.
int putc (int ch, FILE *stream){
    return (int) __putc (ch, stream);
}

// Don't change it.
int fgetc(FILE *stream){
    return (int) getc(stream);
}

// Don't change it.
int fputc ( int ch, FILE *stream ){
    return (int) putc(ch,stream);
}

//
// Root 2
//

// Don't change it.
int getchar (void){
    return (int) getc(stdin);
}

// Don't change it.
int putchar (int ch){
    return (int) putc ( (int) ch, stdout );
}

//
// Root 3
//

// #todo
// Do not use this. Use fgets instead.
// Using gets we can read more than we need.
char *gets(char *s)
{
    register int c=0;
    register char *cs;
    cs = s;

    // #ugly
    while ((c = getchar()) != '\n' && c >= 0)
        *cs++ = c;
    if (c<0 && cs==s)
        return (NULL);

    *cs++ = '\0';

    return (s);
}

int puts(const char *s)
{
    register int c=0;
    while (c = *s++){
        putchar(c);
    };
    return (int) putchar('\n');
}

//
// Root 4
//

//s n iop
char *fgets(char *s, int size, FILE *stream)
{
    register int c=0;
    register char *cs;
    cs = s;

    // Maior ou igual a 0.
    while ( --size > 0 && 
            (c = getc(stream)) >= 0 ) 
    {
        *cs++ = c;
        if (c == '\n'){
            break;
        }
    };

// Nesse momento, acabou o size, ou
// encontramos um '\n'.
// Se o último char for EOF
// e não pegamos char algum. 
    if ( c<0 && cs==s ){
        return NULL;
    }
// Finalizamos a string construída.
    *cs++ = '\0';

    return (s);
}

//s iop
int fputs( const char *s, FILE *stream )
{
    register int c=0;
    register int r=0;

    //#todo
    //if ( (void*) s == NULL ){}

    while (c = *s++){
        r = putc(c,stream);
    };

    return (r);
}


//
// Root 5
//

int getw (FILE *stream)
{
    register int i=0;

    //#todo
    //if ( (void*) stream == NULL ){}

    i = getc(stream);

	//#todo
	//if (stream->_flags&_IOEOF)
		//return(-1);

    if (stream->eof == 1 ){
        return EOF;
    }

    return (i | (getc(stream)<<8));
}

//#test
int putw (int w, FILE *stream)
{
    //#todo
    //if ( (void*) stream == NULL ){}

    putc ( w,      stream);
    putc ( w >> 8, stream);

    // ?? rever retorno.
    return 0;
}

// fclose:
// see: 
// https://man7.org/linux/man-pages/man3/fclose.3.html
/*
The fclose() function flushes the stream pointed to by stream
(writing any buffered output data using fflush(3)) and closes the
underlying file descriptor.
Note that fclose() flushes only the user-space buffers provided
by the C library.  To ensure that the data is physically stored
on disk the kernel buffers must be flushed too, for example, with
sync(2) or fsync(2).
*/
// Close a file.
// If work, return 0.
// #todo
// fclose() takes a stream, flushes it, and closes it.
// linux klibc style
// See: unistd.c for close().
// #todo
// Any buffered output is written and any buffered input is discarded.

int fclose(FILE *stream)
{
    // int __ret = -1;

// #bugbug
// Vamos simplificar pois esta falhando.
// Mas close ja funciona.
    
    debug_print ("fclose: [FIXME]\n");

// Check parameters.
    if ((void *) stream == NULL){
        errno = EBADF;
        return EOF;  
    }

// Flush the data that are in the ring3 buffer.
    fflush(stream);

/*
    if ( stream == stdin )
        return EOF;
    if ( stream == stdout )
        return EOF;
    if ( stream == stderr )
        return EOF;
*/

// Delete the ring 3 structure.

    if ( (void *) stream != NULL )
    {
        stream->_base = NULL;
        stream->_p = NULL;

        stream->_flags = 0;
        stream->_cnt = 0;

        // Delete the structure.        
        stream = NULL;

        // ok
        return 0;
    }

// Close the file and save it into the disk.
    //return (int) close( fileno(stream) ); 
    
    return EOF;
}

// Carrega um arquivo e retorna o ponteiro para a estrutura de stream.
// #bugbug
// We just have the pointer for a structure.
// The file was loaded in ring0 by fopen(), using open()
// So, for now, we simply can't access the data of the file.
// The only way is using read() and the fd given by open().
// See: https://man7.org/linux/man-pages/man2/open.2.html

FILE *fopen ( const char *filename, const char *mode )
{
    FILE *__stream;  // Return this pointer.
    int fd=0;        // File descriptor.  
    int flags=0;     // flags

// mode #todo
// his argument must be supplied when
//          O_CREAT or O_TMPFILE is specified in flags;

    int oflags=0;

    // #todo
    // Check filename validation
    
    //if ( (void*) filename == NULL ){}
    //if ( *filename == 0 ){}

// #todo:
// The 'mode' passed via argment will give us the 'flags'
// used in open().

// NOTE: 
// rt is a non-standard mode which opens a file for read, explicitly
// specifying that it's a text file
    
    if (!strcmp(mode, "r") || !strcmp(mode, "rb") || !strcmp(mode, "rt")){
        flags = O_RDONLY;
    } else if (!strcmp(mode, "r+") || !strcmp(mode, "rb+")){
        flags = O_RDWR;
    } else if (!strcmp(mode, "w") || !strcmp(mode, "wb")){
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else if (!strcmp(mode, "w+") || !strcmp(mode, "wb+")){
        flags = O_RDWR | O_CREAT | O_TRUNC;
    } else if (!strcmp(mode, "a") || !strcmp(mode, "ab")){
        flags = O_WRONLY | O_APPEND | O_CREAT;
    } else if (!strcmp(mode, "a+") || !strcmp(mode, "ab+")){
        flags = O_RDWR | O_APPEND | O_CREAT;
    } else {
        printf ("fopen: [FIXME] flags fail\n");
        //fprintf(stderr, "FIXME(LibC): fopen('%s', '%s')\n", pathname, mode);
        //ASSERT_NOT_REACHED();
    };

// Open
// See: fcntl.c
    fd = open (filename, flags, oflags);  
    if (fd < 0){
        printf ("fopen: open() fail\n");
        return NULL;
    }

// Stream
    __stream = (FILE *) malloc ( sizeof(FILE) );
    if ( (void *) __stream == NULL ){
        printf ("fopen: __stream fail\n");
        return NULL;
    }
    memset( __stream, 0, sizeof(struct _iobuf) );

// Descriptor
    __stream->_file = fd;

// ==============================
// Buffer
// #importante:
// Dessa forma fopen não permite que os aplicativos
// leiam o conteúdo do arquivo no buffer.
// Então o aplicativo terá que usar read pra ler 
// o conteúdo no buffer em ring0.

    __stream->_base = (char *) malloc(BUFSIZ);
    if ( (void *) __stream->_base == NULL ){
        debug_print("fopen: stream buffer fail\n");
        printf     ("fopen: stream buffer fail\n");
        // #todo
        // Temos que liberar a memoria da estrutura.
        //free(__stream);
        return NULL;
     }

// Buffer
    __stream->_p = __stream->_base;
    __stream->_r = 0;
    __stream->_w = 0;   //#todo: Write from the end of the file.
// The size of the ring3 buffer.
    __stream->_lbfsize = BUFSIZ;
// #bugbug
// The size of the 'data' in ring 3?
// it depends on how many bites was read.
// So it needs to be initialized with 0.
    //__stream->_fsize = BUFSIZ;
    __stream->_fsize = 0;
    __stream->_cnt = BUFSIZ;

// Flags
    __stream->_flags = flags;
// File name.
// Saving the name in ring3.
    __stream->_tmpfname = (char *) strdup(filename);
//done:
    __stream->used = TRUE;
    __stream->magic = 1234;
    return (FILE *) __stream;
}

/*
 * fopen2:
 *     Open a file.
 *     @todo: Abrir onde? saída padrão?
 *     @retorna o que? o endereço do buffer?
 *     Obs: Tem função na api que carrega um arquivo em um buffer.
 * #obs: Deveríamos checar o tamanho do arquivo antes de criar o buffer.
 * #todo: E se o ponteiro para o nome do arquivo for inválido? tem que  
 * falhar e retornar null.
 */

FILE *fopen2 ( const char *filename, const char *mode )
{
    FILE *__stream;
    int fd = -1;

    __stream = (FILE *) malloc( sizeof(FILE) );
    if ( (void *) __stream == NULL ){
        printf("fopen2: __stream\n");
        return NULL;
    }

/*
    if(*mode == 'w')
        f = creat(filename, 0600);
    else if (*mode == 'a'){

		//if ((f = open(filename, 1)) < 0)
			//f = creat(filename, 0600);
		//seek(f, 0, 2);
    
    }else{
	    //f = open(filename, 0);
	    //if (f < 0)
		    //return(NULL);
    };
 */

// Size

    stdio_fntos( (char *) filename);
// get file size
    size_t s = 
        (size_t) gramado_system_call ( 
                     178, 
                     (unsigned long) filename,
                      0,
                      0 );

// #bugbug
// 1MB
    if ( s <= 0 || s > (1024*1024) ){
        printf ("fopen2: size\n");
        return NULL;
    }

// endereço desejando.
// ring 3.
    unsigned long address = (unsigned long) malloc(s);    
    if (address == 0){
        printf ("fopen2: address\n");
        return NULL;
    }

// load the file into the address.
// Vai retornar o fd.
// IN: service, name, address, 0, 0 
    fd = 
        (int) gramado_system_call ( 
                  3, 
                  (unsigned long) filename, 
                  (unsigned long) address,  
                  0 );

    if (fd < 0){
        printf ("fopen2: Couldn't load the file\n");
        return NULL;
    }

// Flags

    // #todo
    //__stream->_flags &= ~(_IOREAD|_IOWRT);

    if (*mode != 'r'){
        __stream->_flags |= _IOWRT;
    }else{
        __stream->_flags |= _IOREAD;
    };

// #todo    
// Isso deve ser o retorno de open() ou creat()
// Me parece que a chamada acima também retorna o fd.
    __stream->_file = fd;
// Buffer
    __stream->_base = (unsigned char *) address; 
    __stream->_p = __stream->_base;
    __stream->_lbfsize = (int) s;
    //__stream->_fsize = 0;
    __stream->_cnt = __stream->_lbfsize;
// Retornar a stream que criamos aqui.
    return (FILE *) __stream;
}

/*
 * scroll:
 *     Scroll the screen in (text mode).
 *     @todo: Mudar para tmScroll(void);
 *     @todo: Criar gmScroll(void) talvez;
 *     @todo: Essa rotina precisa ser revisada e aprimorada.
 *     #todo: Talvez deva levar em conta o modo de operação.
 *            Se estamos ou full screen ou não.
 */
void scroll (void)
{

// #bugbug
// These definitions?

    // loop
    register unsigned short i=0;
    register unsigned short j=0;
// Início da tela.
    unsigned short *p1 = (unsigned short *) SCREEN_START;
// Início da segunda linha.
    unsigned short *p2 = 
        (unsigned short *) (SCREEN_START + 2 * SCREEN_WIDTH);

// Linhas.
// Usa o valor default se passar dos limites.

    if ( g_rows == 0 || g_rows >= SCREEN_MAX_HEIGHT )
    {
        g_rows = SCREEN_HEIGHT;
    }

// 24 vezes.

    for ( i=0; i < g_rows -1; i++ )
    {
        //80 vezes
        //Se a variável nao foi inicializada, usar valor default.
        if (g_columns == 0 || g_columns >= SCREEN_MAX_WIDTH)
        { 
            g_columns = SCREEN_WIDTH;
        }

		//modo texto
		//if(g_using_gui == 0)
		//{
		//    for( j=0; j < g_columns; j++ ){
		//        *p1++ = *p2++;
		//    };
		//};

        // modo gráfico.
        if (g_using_gui == 1)
        {
            for ( j=0; j < g_columns; j++ )
            {
                //*p1++ = *p2++;
                //@todo:
            };
        }
    };

// Limpando a última linha.
// modo gráfico.

    if (g_using_gui == 1)
    {
        for ( i=0; i < g_columns; i++ )
        { 
            //*p1++ = 0x07*256 + ' ';
            // @todo:
        };    
    }
}

void clearerr(FILE* stream)
{
    if ( (void *) stream == NULL ){
        return;
    }
    //stream->_flags &= ~(_IOERR|_IOEOF);
    stream->eof = FALSE;
    stream->error = 0;
}

// fread:
// #importante
// Ler uma certa quantidade de chars de uma stream e coloca-los no buffer.
// Isso vai ser usado pelo terminal por exemplo.
// Assim o terminal pode pegar uma string que esteja no arquivo.
// See:
// http://man7.org/linux/man-pages/man3/fread.3.html
// https://www.tutorialspoint.com/c_standard_library/c_function_fread.htm 
// #todo:
// Check the sizes.

size_t fread (void *ptr, size_t size, size_t n, FILE *fp)
{
    void *data;
    size_t nreads=0;
    size_t number_of_bytes = -1;
    data = ptr;

    if ( (void *) data == NULL ){
        printf ("fread: ptr \n");
        return (size_t) -1;
    }
    if (size <= 0){
        printf ("fread: size \n");
        return (size_t) -1;
    }
// Quantidade inválida.
    if (n <= 0){
        printf ("fread: n \n");
        return (size_t) -1;
    }
// Se não temos o ponteiro, então não teremos o fd
// para usarmos em read().
    if ( (void *) fp == NULL ){
        printf ("fread: fp \n");
        return (size_t) -1;
    }

// ====================================
// Calculando a quantidade de bytes.
    number_of_bytes = (size_t) (size * n);

// buffer size
    size_t _max = sizeof(data);    
    if (_max <= 0){
        printf("fread: [DEBUG] buffer size <= 0 \n");
        return (size_t) -1;
    }
    if (_max > BUFSIZ){
        printf("fread: [DEBUG] buffer size\n");
        _max = BUFSIZ;
    }

//
// Read
//

    if ( number_of_bytes >= _max ){
        nreads = read( fileno(fp), data, _max );
    }else{
        nreads = read( fileno(fp), data, number_of_bytes );
    };

    if (nreads <= 0){
        printf ("fread: read() fail\n");
        fp->_flags |= _IOERR;
        return (size_t) -1;   
    }

    // menos do que desejavamos.
    if (nreads < number_of_bytes){
        printf ("fread: read() fail. Less than desired\n");
        fp->_flags |= _IOERR;
        return (size_t) -1;   
    }

    return (size_t) nreads;
}


/*
 * fwrite:
 *     write in a file. (fp)
 */
// IN:
// ptr = pointer.
// size = Tamanho do elemento dado em bytes.
// n = quantidade de elementos.
// fp = stream pointer.
// See: 
// https://linux.die.net/man/3/fwrite

size_t 
fwrite ( 
    const void *ptr, 
    size_t size, 
    size_t n, 
    FILE *fp )
{
    size_t nwrite=0;
    size_t number_of_bytes = -1;

// Pointer validation.
    if ( (void *) ptr == NULL ){
        printf ("fwrite: ptr \n");
        goto error0;
    }
// Tamanho do elemento dado em bytes.
    if (size <= 0){
        printf("fwrite: size\n");
        goto error0;
    }
// Quantidade de elementos.
    if (n <= 0){
        printf("fwrite: n\n");
        goto error0;
    }
// File pointer
// Se não temos o ponteiro, então não teremos o fd
// para usarmos em read().
    if ( (void *) fp == NULL ){
        printf ("fwrite: fp\n");
        goto error0;
    }
// ====================================
// Calculando a quantidade de bytes.
    number_of_bytes = (size_t) (size * n);

    size_t _max = sizeof(ptr);
    if (number_of_bytes > _max){
        number_of_bytes = _max;
    }

//
// Write
//

    //nwrite = write( fileno(fp), ptr, sizeof(ptr) );
    //nwrite = write ( fileno(fp), ptr, n );
    nwrite = write ( fileno(fp), ptr, number_of_bytes );
// Se não conseguimos escrever.
    if (nwrite <= 0){
        printf ("fwrite: write() fail\n");
        goto error1;
    }
// Se escrevemos menos do que desejavamos.
    if (nwrite < number_of_bytes){
        printf ("fwrite: write() less than desired\n");
        goto error1;
    }
//done:
    return (size_t) nwrite; 
// Se já temos um ponteiro de arquivo válido.
error1:
    fp->_flags |= _IOERR;
    // ...
// Ainda não sabemos se o ponteiro de arquivo é válido.
error0:
    return (size_t) EOF;   
}


// O prompt precisa ser inicializado pelo crt0 e
// quando ele sobre flush.
int prompt_putchar ( int c, int con_id )
{
    if ( con_id < 0 ){
        return -1;
    }
    if ( con_id >= 4 ){
        return -1;
    }

// #todo: 
// Overflow do prompt[]

// ??
//eol
    if ( c != '\n' ){
        input ( (unsigned long) c );
        return c;
    }
// se \n
    if ( c == '\n' ){
       prompt_flush (con_id);
    }
// ??
// #bugbug: We need a return here.
    return 0;  //??
} 


int prompt_put_string (char *string)
{
    if ( (void *) string == NULL ){
        return -1;
    }
    sprintf(prompt,(const char *) string);
    return 0;
}

int prompt_strcat (char *string)
{
    if ( (void *) string == NULL ){
        return -1;
    }
    strcat(prompt,(const char *) string);
    return 0;
}

int prompt_flush (int con_id)
{
    size_t len=0;

    if (con_id < 0){
        return -1;
    }
    if (con_id >= 4){
        return -1;
    }

//finaliza.
    input ('\0'); 
    len = strlen( (const char *) prompt );  
    gramado_system_call ( 66, (unsigned long) prompt, con_id, len );  
    prompt_clean();

    return 0;
}

// Linpando o buffer de entrada.
void prompt_clean (void)
{
    register int i=0;
    for ( i=0; i<PROMPT_MAX_DEFAULT; i++ )
    {
        prompt[i] = (char) '\0';
    };
    prompt[0] = (char) '\0';
    prompt_pos = 0;
    prompt_status = 0;
    prompt_max = PROMPT_MAX_DEFAULT;  
}

/*
 ****************************************************************
 *    Building the function printf;
 ****************************************************************
 */


/*
 **********************************************************************
 * prints:
 */
// #bubgug:
// Esses argumentos podem estar fora dos padrões.
// Mas a implementação feita pelo altor é assim mesmo. 

int 
prints ( 
    char **out, 
    const char *string, 
    int width, 
    int pad )
{
    register int pc = 0, padchar = ' ';

// filtro: 
// E se width for menor que 0?

	//if(width < 0){
	//	return (int) 1;
	//}

    if (width > 0) 
    {
        register int len = 0;
        register const char *ptr;

        for(ptr = string; *ptr; ++ptr) ++len;
        if(len >= width) width = 0;
        else width -= len;
        if(pad & PAD_ZERO) padchar = '0';
    }

    if ( !(pad & PAD_RIGHT) )
    {
        for ( ; width > 0; --width )
        {
            printchar ( out,padchar );
            ++pc;
        };
    }

    for ( ; *string ; ++string )
    {
        printchar (out, *string);
        ++pc;
    };

    for ( ; width > 0; --width )
    {
        printchar (out,padchar);
        ++pc;
    };

    return pc;
}


/*
 * printi:
 *     used by printf.
 */

int 
printi ( 
    char **out, 
    int i, 
    int b, 
    int sg, 
    int width, 
    int pad, 
    int letbase )
{
    register char *s;
    register int t, neg = 0, pc = 0;
    register unsigned int u = i;

    char print_buf[PRINT_BUF_LEN];


    if (i == 0){
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints (out, print_buf, width, pad);
    }


    if ( sg && b == 10 && i < 0) 
    {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    while (u) 
    {
        t = (u % b);

        if (t >= 10)
            t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    };


    if (neg) 
    {
        if ( width && (pad & PAD_ZERO) ) 
        {
            printchar (out, '-');
            ++pc;
            --width;

        }else{
            *--s = '-';
        };
    }

    return pc + prints(out, s, width, pad);
}

/*
 * print:
 *     Used by printf.
 */

int print ( char **out, int *varg )
{
    register int width, pad;
    register int pc = 0;
    register char *format = (char *) (*varg++);
    char scr[2];

    for ( ; *format != 0; format++ ) 
    {

		//switch.
        if ( *format == '%' ) 
        {

			++format;
			width = pad = 0;

            if ( *format == '\0' ){  break;  }

            if ( *format == '%' ){  goto out;  }

            if ( *format == '-' )
            {
                ++format;
                pad = PAD_RIGHT;
            }

			while ( *format == '0' ){
				++format;
				pad |= PAD_ZERO;
			};
			
			for ( ; *format >= '0' && *format <= '9'; ++format )
			{
				width *= 10;
				width += *format - '0';
			};
			
			//string
			if ( *format == 's' ){
				
				register char *s = *( (char **) varg++ );
				pc += prints ( out, s?s:"(null)", width, pad);
				continue;
			}
			
			//int
			if ( *format == 'd' ){
				pc += printi ( out, *varg++, 10, 1, width, pad, 'a');
				continue;
			}
			
			//hexa
			if ( *format == 'x' ){
				pc += printi ( out, *varg++, 16, 0, width, pad, 'a');
				continue;
			}
			
			if ( *format == 'X' ){
				pc += printi ( out, *varg++, 16, 0, width, pad, 'A');
				continue;
			}
			
			if ( *format == 'u' ){
				pc += printi ( out, *varg++, 10, 0, width, pad, 'a');
				continue;
			}
			
			//char
			//char are converted to int then pushed on the stack. 
			if ( *format == 'c' ){
				
				scr[0] = *varg++;
				scr[1] = '\0';
				pc += prints ( out, scr, width, pad);
				continue;
			}
			
			//Nothing.
		
		}else{
		    out:
			    printchar ( out, *format );
			    ++pc;
		};
		//Nothing.
    };

    if (out){
        **out = '\0';
    }

    return (int) pc;
}


/* 
 ****************************************************
 * printf:
 *     Imprime uma string formatada.
 *     Assuming sizeof(void *) == sizeof(int).
 * Obs: Temos uma chamada ao sistema para cada caractere.
 * Obs: Se passássemos apenas o ponteiro da string para o kernel pintá-la,
 * não estariamos usando o printf da biblioteca em user mode e sim 
 * a printf do kernel.
 */
// Padrão não tradicional, mas funciona. 

/*
int printf3 ( const char *format, ... )
{
    register int *varg = (int *)(&format);

    return (int) print ( 0, varg );
}
*/


//======================
// #todo: poderia se chamar l2hex?
void 
kinguio_i2hex_versao2(
    unsigned long val, 
    char* dest, 
    int len)
{
    register int i=0; 
    char *cp;
    int x=0;
    unsigned long n=0;

    if (val == 0)
    {
        cp = &dest[0];
        *cp++ = '0';
        *cp = '\0';
        return;
    }

    n = val;
    cp = &dest[len];
    while (cp > dest)
    {
        x = n & 0xF;
        n >>= 4;
        *--cp = x + ((x > 9) ? 'A' - 10 : '0');
    };

    dest[len]='\0';

    cp = &dest[0];
    for (i=0; i<len; i++)
    {
        if (*cp == '0'){
            cp++;
        }else{
            strcpy(dest,cp);
            break;
        };
    };

    cp = &dest[0];
    n = strlen(cp);

    memset(
        dest+n,
        0,
        len-n );
}

//=============================================================
// Usada por kinguio_printf

//local
void 
kinguio_i2hex( 
    unsigned int val, 
    char *dest, 
    int len )
{
    register int i=0; 
    char* cp;
    int x=0;
    unsigned n=0;

// Simplesmente coloca '0' no buffer.
    if (val == 0)
    {
        cp = &dest[0];
        *cp++ = '0';
        *cp = '\0';
        return;
    }

// error
    if (len<0)
        return;

// #todo
// Qual é o tamanho desse buffer?

    n = val;
    cp = &dest[len];
    while (cp > dest)
    {
        // Pega um nibble.
        x = (n & 0xF);
        // Próximo nibble.
        n >>= 4;
        
        *--cp = x + ((x > (HEX_LEN+1)) ? 'A' - 10 : '0' );
    };
    
    dest[len]='\0';

    cp = &dest[0];
    for (i=0; i<len; i++)
    {
        if (*cp == '0'){
            cp++;
        }else{
            strcpy(dest,cp);
            break;
        };
    };

    cp = &dest[0];
    n = strlen(cp);

    memset( 
        dest+n, 
        0,
        8-n );   //32bit
}

void 
kinguio_l2hex( 
    unsigned long val, 
    char *dest, 
    int len )
{
    register int i=0; 
    char* cp;
    int x=0;
    unsigned n=0;

// Simplesmente coloca '0' no buffer.
    if (val == 0)
    {
        cp = &dest[0];
        *cp++ = '0';
        *cp = '\0';
        return;
    }

// error
    if (len<0)
        return;

// #todo
// Qual é o tamanho desse buffer?

    n = val;
    cp = &dest[len];
    while (cp > dest)
    {
        // Pega um nibble.
        x = (n & 0xF);
        // Próximo nibble.
        n >>= 4;
        
        *--cp = x + ((x > (HEX_LEN+1)) ? 'A' - 10 : '0' );
    };
    
    dest[len]='\0';

    cp = &dest[0];
    for (i=0; i<len; i++)
    {
        if (*cp == '0'){
            cp++;
        }else{
            strcpy(dest,cp);
            break;
        };
    };

    cp = &dest[0];
    n = strlen(cp);
   
    //memset( 
    //    dest + n, 
    //    0, 
    //    8-n );

    memset( 
        dest+n, 
        0, 
        16-n );   //64bit
}

//local
static char *__utoa_r (unsigned long val, char *str)
{
    char* valuestring = (char*) str;
    unsigned long value = val;
    char swap, *p;

    p = valuestring;

    do
    {
        *p++ = (char)(value % 10) + '0';
        value /= 10;

    } while (value);

    *p-- = '\0';

    while (p > valuestring)
    {
        swap = *valuestring;
        *valuestring++ = *p;
        *p-- = swap;
    }

    return str;
}


//local
char *kinguio_utoa(
    unsigned long val, 
    char *dst, 
    int radix )
{
    char *cp = (char *) dst;
    unsigned long lu = (unsigned long)val;

    if(radix == 16)
        kinguio_i2hex_versao2(lu, cp, 16);
    else 
        __utoa_r(lu, cp);
    return dst;

    return dst;  //#bugbug
}


char *kinguio_itoa (int val, char *str) 
{
    char *valuestring = (char *) str;
    int value = val;
    int min_flag;
    char swap, *p;
    min_flag = 0;

    if (0 > value)
    {
        *valuestring++ = '-';
        value = -____INT_MAX> value ? min_flag = ____INT_MAX : -value;
    }

    p = valuestring;

    do
    {
        *p++ = (char)(value % 10) + '0';
        value /= 10;
    } while (value);

    if (min_flag != 0)
    {
        ++*valuestring;
    }
    
    *p-- = '\0';

    while (p > valuestring)
    {
        swap = *valuestring;
        *valuestring++ = *p;
        *p-- = swap;
   };

    return str;
}


// printf
// Credits: Nelson Cole. Project Sirius/Kinguio.
int kinguio_printf(const char *fmt, ...)
{
    int ret=0;
    char buf[256];

    memset(buf,0,256); 

    va_list ap;
    va_start (ap, fmt);
    ret = kinguio_vsprintf(buf, fmt, ap);
    va_end (ap);

// Print
    kinguio_puts(buf);

    return (int) ret;
}


int printf(const char *fmt, ...)
{
    static char print_buffer[1024];
    int ret=0;

    va_list ap;
    va_start (ap, fmt);
    memset(print_buffer, 0, 1024); 
    ret = (int) kinguio_vsprintf(print_buffer, fmt, ap);
    va_end (ap);

// Print and return.
    kinguio_puts(print_buffer);
    return (int) ret;
}

void kinguio_puts(const char* str)
{
    register int i=0;
    if (!str){
        return;
    }
    size_t StringLen = (size_t) strlen(str);
    for (i=0; i<StringLen; i++){
        putchar( str[i] );
    };
}

// #local
static char *_vsputs_r(char *dest, char *src)
{
    unsigned char *usrc  = (unsigned char *) src;
    unsigned char *udest = (unsigned char *) dest;
    while (*usrc){
        *udest++ = *usrc++; 
    };
    return (char * ) udest;
}

int 
kinguio_vsprintf( 
    char * str, 
    const char *fmt, 
    va_list ap )
{
    register int index=0;
// Char/String support.
    char c=0;  //c
    char *s;   //s
    char *str_tmp = str;
    char _c_r[] = "\0\0";
    char buffer[256];
// d|i|x
    int   d=0;
    long ld=0;  //signed long
// u
    //unsigned char u=0;
    unsigned int   u=0;
    unsigned long lu=0;
// 64bit Format Specifier
    int type64bit = FALSE;

// loop
    while ( fmt[index] )
    {
        switch (fmt[index]){
        
        case '%':
            ++index;
            
            // Estamos lidando com 64bit?
            // Vamos para o próximo char e
            // indicaremos essa condição.
            if ( fmt[index] == 'l' ){
                ++index;
                type64bit = TRUE;
            }
            
            switch (fmt[index]){
            
            //int
            case 'c':
                //*_c_r = c = (char) va_arg (ap, int);
                c = (char) va_arg (ap, int);
                *_c_r = c;
                str_tmp = _vsputs_r( str_tmp, _c_r );  //print
                break;

            //char*
            case 's':
            case 'S':
                s = va_arg(ap, char*);
                if( (void*) s != NULL ){
                    str_tmp = _vsputs_r(str_tmp,s);
                }
                if( (void*) s == NULL ){
                    str_tmp = _vsputs_r(str_tmp,"(null)");
                }
                break;

            // int
            case 'd':
            case 'i':
                d = va_arg (ap, int);
                kinguio_itoa (d,buffer);
                str_tmp = _vsputs_r(str_tmp,buffer);
                break;

            //unsigned int
            case 'u':
                //'lu'
                if(type64bit==TRUE){
                    lu = va_arg (ap, unsigned long);
                    //kinguio_itoa (lu,buffer);
                    kinguio_utoa(lu, buffer, 10);  //ok
                    str_tmp  = _vsputs_r(str_tmp,buffer);
                    type64bit=FALSE;
                    break;
                }
                //if(type64bit==FALSE){
                u = va_arg (ap, unsigned int);
                kinguio_itoa (u,buffer);
                //utoa(u, buffer, 10);
                str_tmp  = _vsputs_r(str_tmp,buffer);
                //}
                break;

            //int  (hexa)
            case 'X':
            case 'x':
                //'lx'  #fail
                if(type64bit==TRUE){
                    lu = va_arg (ap, unsigned long);
                    //kinguio_itoa (lu,buffer);
                    kinguio_utoa(lu, buffer, 16);
                    str_tmp  = _vsputs_r(str_tmp,buffer);
                    type64bit=FALSE;
                    break;
                }
                d = va_arg (ap, int);
                //d = va_arg (ap, unsigned int);
                kinguio_i2hex(d,buffer,8);
                str_tmp  = _vsputs_r(str_tmp,buffer);
                break;

            default:
                str_tmp = _vsputs_r(str_tmp,"%%");
                break;
            };
            break;

        default:
            *_c_r = fmt[index];
            str_tmp  = _vsputs_r(str_tmp,_c_r);
            break;
        };

        ++index;
    };

// done:
// Return size ?
    return (int) ((long)str_tmp - (long)str);
}
//=============================================================

//
// =============================================================
//

// usada na printf2
void printf_atoi (int value, char* valuestring)
{
    int min_flag=0;
    char swap, *p;
    //min_flag = 0;

    if (0 > value)
    {
        *valuestring++ = '-';

        value = -INT_MAX> value ? min_flag = INT_MAX : -value;
    }

    p = valuestring;

    do {

        *p++ = (char)(value % 10) + '0';

        value /= 10;
      
    } while (value);


    if (min_flag != 0)
    {
        ++*valuestring;
    }

    *p-- = '\0';

    while (p > valuestring)
    {
        swap = *valuestring;
        
        *valuestring++ = *p;
        
        *p-- = swap;
    };
}

// Usada na printf2
void printf_i2hex (uint32_t val, char *dest, int len)
{
    char *cp;
    char x=0;
    uint32_t n=0;

    n = val;
    cp = &dest[len];

    while (cp > dest)
    {
        x = n & 0xF;
        n >>= 4;

        *--cp = x + ((x > 9) ? 'A' - 10 : '0');
    };

    dest[len+1] = '\0';
}



// #test 
// tentando implementar a printf do nelson cole.
// padrão tradicional, incompleta, não funciona ainda,
// estamo implementando

/*
int printf2 ( const char *format, ... )
{

    char *ap;
    va_start (ap,format);

    int index = 0;
    uint8_t u;
    int d;
    char c, *s;

    char buffer[256];


    while ( format[index] )
    {
        switch (format[index])
        {
            case '%':
			    ++index;
			    switch (format[index])
			    {
			
                    case 'C':
			        case 'c':
				        c = (int8_t) va_arg (ap, int32_t);
				        putchar(c);
				        break;
     
                    case 'S':
			        case 's':
				        s = va_arg (ap, int8_t*);
				        //puts(s);
				        printf2(s);
						break;

			        case 'd':
			        case 'i':
				        d = va_arg (ap, int32_t);
				        //atoi(d, buffer);
				        printf_atoi(d, buffer);
						//puts(buffer);
						printf2(buffer);
				        break;

			        case 'U':
			        case 'u':
				        u = va_arg (ap, uint32_t);
				        //atoi(u, buffer);
				        printf_atoi (u, buffer);
						//puts(buffer);
				        printf2 (buffer);
						break;

			        case 'X':
			        case 'x':
				        d = va_arg (ap, int32_t);
				        //i2hex(d, buffer,8);
						//printf_i2hex(d, buffer,8);
				        //puts(buffer);
                        printf ("%x",d);
                        break;

                    default:
                        putchar ('%');
                        putchar ('%');
                        break;
                }
                break;

            default:
                putchar ( format[index] );
                break;
        };

        ++index;
    };

    return 0;
}
*/


//
//=============================================================
//

//find next line
char *stdio_nextline (char *string)
{
    char *p;

    p = (char *) strchr (string, '\n');

    if (p==NULL){ return (p); }

    ++p;

// Não há próxima linha    
    if (*p=='\0'){ 
        return (NULL);
// Retorna o ponteiro para a próxima linha. 
    }else{
        return (p);
    };
}

// next line sprintf
int nlsprintf ( char *out, const char *format, ... )
{
    char *tmp;

    tmp = (char *) stdio_nextline(out);
    if (tmp == NULL){
        // ?? message
        return -1;
    }
    register int *varg = (int *)(&format);

    return (int) print( &tmp, varg );
}

/*
 *****************************************************************
 * sprintf:
 *     Emprime uma string formatada em 'out'.
 *     Escreve em um buffer de caracteres.
 * @todo: ??
 * Em caso de sucesso, a função retorna o número de caracteres 
 * da string formatada. Em caso de erro, um valor negativo é retornado.
 */ 
//opção 
//int sprintf(char *str, const char *format, ...) ?? 

int sprintf ( char *out, const char *format, ... )
{
    register int *varg = (int *)(&format);

    return (int) print( &out, varg );
}


void printchar ( char **str, int c )
{
    if (str) 
    {
        **str = c;
        ++(*str);
    } else (void) putchar(c);
}


// Setup libc mode.
void libc_set_output_mode(int mode)
{
    switch (mode){
    case LIBC_DRAW_MODE:
        __libc_output_mode = mode;
        break;
    case LIBC_NORMAL_MODE:
        __libc_output_mode = mode;
        break;
    default:
        __libc_output_mode = LIBC_DRAW_MODE;
        printf ("libc_set_output_mode: fail\n");
        break;
    };
}

/*
 **********
 * outbyte:
 * @todo: Colocar no buffer de arquivo.
 * #obs: essa função chamará _outbyte.
 * #importante: 
 * Não me lebro se o kernel efetua o refresh do char nesse caso.
 */
 
void outbyte (int c)
{
// Copy.
    register int Ch=c;
    static char prev = 0;

// spaces
    if ( Ch <  ' '  && 
         Ch != '\r' && 
         Ch != '\n' && 
         Ch != '\t' && 
         Ch != '\b' )
    {
        return;
    }

//#testando.
//nada para imprimir.
//?? onde fica o tratamento de backspace.??
	//if( c == '\b' )
    //{
	//	return;
	//}

// carriege return 
// Volta ao início da linha.
    if ( Ch == '\r' )
    {
        g_cursor_x = 0;
        prev = Ch;
        return;  
    }

//Próxima linha e não início da linha.   
    if ( Ch == '\n' && prev != '\r' ) 
    {
        g_cursor_y++; 
        g_cursor_x = 0;  
        prev = Ch;
        return;
    }

//Próxima linha e início da linha. 	   
    if ( Ch == '\n' && prev == '\r' ) 
    {
        g_cursor_y++;  
        prev = Ch;
        return;
    }

//Tab
//@todo: Criar a var -> 'g_tab_size'.
    if ( Ch == '\t' )  
    {
        g_cursor_x += (4);    
        prev = Ch;
        return;   
    }

//Space 
//#todo:
// ?? talvez devêssemos imprimir o caractere espaço. ??    
    if ( Ch == ' ' )  
    {
        g_cursor_x++; 
        prev = Ch;
        return; 
    }

//Delete
    if ( Ch == 8 )  
    {
        g_cursor_x--; 
        prev = Ch;
        return; 
    }

// Filtra as dimensões da janela onde esta pintando.
 
//checkLimits:

//Colunas
//Definindo a largura que usaremos.
//A largura precisa ser maior que '0' e menor que o limite máximo.
//Obs: @todo: Essa rotina de definição pode ir para o momento da inicialização
//da biblioteca. Ela contunua aqui porque está funcionando como um filtro.

    if ( g_columns == 0 || g_columns >= SCREEN_MAX_WIDTH )
    {
        g_columns = COLUMNS;
    }

	//O cursor não pode ultrapassar a largura definida.
    if ( g_cursor_x > g_columns )
    {
        g_cursor_x = 0;
        g_cursor_y++;  
    }else{
		//Se não alcançamos o limite, apenas incrementa o x.
        g_cursor_x++;    
    };

// Linhas

//Definindo a altura que usaremos.
//A altura precisa ser maior que '0' e menor que o limite máximo.
    if ( g_rows == 0 || g_rows >= SCREEN_MAX_HEIGHT )
    {
        g_rows = ROWS;
    }

//O cursor não pode ultrapassar a altura definida.
//se ultrapassar, chamaremos o scroll.
//Obs: O scroll ainda não está implementado.
//O scroll será feito depois que implementarmos o array de ponteiros
//para estruturas de linha.
	
    if ( g_cursor_y > g_rows )
    { 
        scroll();
        g_cursor_x = 0;             //O cursor deve fica na primeira coluna.
        g_cursor_y = (g_rows-1);    //O cursor deve ficar na última linha.
    }

// Imprime os caracteres normais.
    _outbyte (Ch);
//Atualisa o prev.
    prev = Ch; 
}


/*
 * _outbyte:
 * Just output a byte on the screen.
 * Obs: 
 * A função não se preocupa com o cursor.
 * Essa rotina está preparada somente par ao modo gráfico.
 * Talvez usaremos um selecionador de modo.   
 * #obs: 
 * #importante: 
 * Não me lebro se o kernel efetua o refresh do char 
 * nesse caso.
 */
// #obs: 
// Tamanho do char constante = 8. 
// O que queremos é usar uma variável.

void _outbyte (int c)
{

// #bugbug
// Essa funçao nao 'e usada ... NAO funciona.
// printf usa outra coisa (65).
// #bugbug: size = 8

    gramado_system_call ( 
        7, 
        (8 * g_cursor_x), 
        (8 * g_cursor_y), 
        (unsigned long) c ); 

    //if ( (void *) stdout == NULL )
       //return;

    //#todo
    //putc ( ch, stdout );
}


/*
int gramado_input ( const char *string, va_list arglist );
int gramado_input ( const char *string, va_list arglist )
{
   //todo
   return -1;
}
*/


/*
 ************************************************************
 * input:
 *     Include the given chars into a string named 'prompt[]'.
 */
// #bugbug: 
// Deveríamos considerar o posicionamento dentro do arquivo.
// Dentro da stream.
// Isso eh chamado pelo shell pra construir uma linha de comandos
// com as teclas digitadas.
// Serve para construir uma string.
// ??
// E esse retorno ??

unsigned long input (unsigned long ch)
{
// Convert
    char c = (char) ch;

// Ajust prompt max.
    if ( prompt_max == 0 || prompt_max >= PROMPT_MAX_DEFAULT )
    {
        prompt_max = PROMPT_MAX_DEFAULT;
    }

//Filtra limite.
//retornar 1??

    if ( prompt_pos > prompt_max ){
        printf ("input: [FAIL] Full buffer!\n");
        return (unsigned long) 0;   
    }

// Trata caractere digitado.
// #obs: 
// Não deve ser trabalho de input() atualizar
// o cursor do console. Ele somente atua sobre o buffer em ring3.

    switch (c){

       // [ Enter ]
       case VK_RETURN:
            prompt[prompt_pos] = (char ) '\0';
            goto input_done;
            break;

        // obs: 
        // O tab avança o cursor, mas precisamos 
        // pintar o espaço onde estava o cursor.
        // Por isso talvez seja bom um while de espaços.
        // Mas no buffer fica apenas o '\t'.
        case VK_TAB:
            prompt[prompt_pos] = (char ) '\t';
            //for( i=0; i<4; i++)
            //{ printf("%c", (char) ' '); }
            goto input_done;
            break;

        // ok, isso funcionou no gdeshell.
        // Volta, mas nao apaga, quem apaga eh o 0x20.
        case 0x8:
        case 0x7f:      // del
        case VK_BACKSPACE:  //0x0E
            // Se ja estamos no início da linha.
            if (prompt_pos <= 0)
            {
                prompt_pos = 0; 
                prompt[prompt_pos] = (char ) '\0';
                break; 
            }
            //Se nao estamos no inicio da linha.
            prompt_pos--;              //volta um no buffer.
            break;


		//Continua ...
		
		// Vamos deixar o sistema mostrar o cursor.
		// Coloca carctere normal no buffer.
		// Incrementa posicionamento no buffer.
        default:
            prompt[prompt_pos] = c;
            prompt_pos++;
            break;
    };

    return (unsigned long) 0;

input_done:
    return VK_RETURN;
}


/*
 * #opção.
int fprintf(FILE *fp, const char *fmt, ...);
int fprintf(FILE *fp, const char *fmt, ...)
{
    va_list ap;
    int err;

    va_start(ap, fmt);

    if ( (void *) fp == NULL )
       return EOF;

    err = vfprintf(fp, fmt, ap);
    va_end(ap);
    return err;
}
*/


/*
 ********************************
 * fprintf:     
 */
// #bugbug
// We just have the pointer for a structure.
// The file was loaded in ring0 by fopen(), using open()
// So, for now, we simply can't access the data of the file.
// The only way is using read() and the fd given by open().

int fprintf ( FILE *stream, const char *format, ... )
{
    debug_print ("fprintf: [FIXME]\n");
         printf ("fprintf: [FIXME]\n");

    if ( (void *) stream == NULL ){
       debug_print ("fprintf: stream\n");
       return EOF;
    }

    //++
    va_list ap;
    va_start(ap, format);
    
    //#bugbug Talvez sem isso
    //unsigned arg = va_arg(ap,unsigned long);

    // #todo

    //#test.
    //vfprintf ( stream, format, arg );
    //vfprintf ( stream, format, ap );

    va_end(ap);
    //--
     
    // Suspendendo a notificação do terminal.
    return 0;
}



//#todo: testar.
//Credits: Sombra OS.
void nputs (char *cp, int len)
{
    register int i=len;
    char *str;

// nao podemos escrever no endereço 0.
    if ( (void*) cp == NULL ){
        return;
    }

    if ( len < 0 ){
        return;
    }

    for ( str=cp; i; str++, i-- )
    {
        putchar ( (int) *str);
    };
}


/*uClib style*/
/* 
 * Strictly speaking, this implementation is incorrect as the number
 * of chars gets can read should be unlimited.  However, I can't
 * imagine anyone wanting to gets() into a buffer bigger than INT_MAX.
 * Besides, this function is inherently unsafe and shouldn't be used.
 */
/* This is an UNSAFE function! */

char *uclib_gets (char *str) 
{

// nao podemos usr o endereço '0'.
    if ( (void *) str == NULL ){
       return (char *) 0;
    }

    if ( (void *) stdin == NULL ){
       return (char *) 0;
    }

    return (char *) fgets (str, INT_MAX, stdin);
}


/*
//unix v32 like
char *unixv32_gets (char *s);
char *unixv32_gets (char *s)
{
    register c;
    register char *cs;

    cs = s;
    while ((c = getchar()) != '\n' && c >= 0)
        *cs++ = c;
    if (c<0 && cs==s)
        return(NULL);

    *cs++ = '\0';

    return (s);
}
*/


// glibc
/* Read a newline-terminated string from stdin into S,
   removing the trailing newline.  Return S or NULL.  */
/*
char *glibc_gets (char *s);
char *glibc_gets (char *s)
{
  register char *p = s;
  register int c;
  FILE *stream = stdin;

    if ( (void *) stream == NULL )
       return (char *) 0;


    if (!__validfp(stream) || p == NULL)
    {
        errno = EINVAL;
         return NULL;
    }

    if (feof(stream) || ferror(stream))
        return NULL;

    while ((c = getchar()) != EOF)
        if (c == '\n')
            break;
        else
            *p++ = c;

     *p = '\0';

     //Return null if we had an error, or if we got EOF
     //  before writing any characters.  

     if (ferror (stream) || (feof (stream) && p == s))
        return NULL;

     return s;
}
*/


/*
ssize_t getline(char** lineptr, size_t* n, FILE* stream)
{
    return getdelim(lineptr, n, '\n', stream);
}
*/


/*
 *********************************
 * ungetc:
 */
// #todo:
// #importante:
// Se recolocarmos alguma coisa com o ungetc(),
// então e getc() deve checar se antes temos um unget
// se tiver deve pega-lo, so depois fazer seu trabalho.

int ungetc ( int c, FILE *stream )
{
    if (c == EOF){
        return EOF;
    }

    if ( (void *) stream == NULL ){
        debug_print ("unget: stream\n");
        return EOF;
    }else{

        // tem um ungotten
        // Não precisamos mudar.
        if (stream->have_ungotten)
        {
            // substituir o que ja tem. ??
            //stream->ungotten = c; 
            //return (int) c;
            
            return EOF;
        }

        // Mudando.
        stream->have_ungotten = TRUE;
        // Salva.
        stream->ungotten = c;
        stream->eof = FALSE;
        return (int) c;
    };

    return EOF;
}


/*linux - klibc style*/
/*
static __inline__ off_t ftell(FILE *__f)
{
  extern off_t lseek(int, off_t, int);
  return lseek(fileno(__f), 0, SEEK_CUR);
}
*/

// Deixaremos o kernel manipular a estrutura.
// This function returns the current file position of the stream stream. 

long ftell (FILE *stream)
{
    int fd=-1;    
    int offset=0;
    long file_pos=0;
    int rCount=0;

    if ( (void *) stream == NULL ){
        debug_print ("ftell: stream\n");
        return 0;
    }else{

        fd = fileno(stream);
        
        if (fd<0){
            printf("ftell: [FAIL] fd\n");
            return 0;
        }
        
        // Corrigir overflow.
        // local. (ring3)
        if (stream->_cnt < 0){
            stream->_cnt = 0;
        }
        
        //fflush (stream);
        
        // Pegamos o valor, assim podemos atualizar a estrutura local.
        
        file_pos = (long) lseek ( fd, 0, SEEK_CUR); 
        
        // offset
        // offset = stream->_ptr - stream->_base;
        
        // ??
        //if (filepos == 0)
        //    return((long)offset);
        
        // local
        //if (stream->_cnt == 0)
        //    offset = 0;

        // rCount = stream->_cnt + (stream->_ptr - stream->_base);
        
        return (long) file_pos;
    };

    debug_print ("ftell: [ERROR] Something is wrong\n");
    
    return 0;
}


// fileno: 
// Gets the file id.
// The kernel gets this value from the local stream struct.

int fileno(FILE *stream)
{
    if ( (void *) stream == NULL ){
       return EOF; 
    } 
    return (int) stream->_file;
}


// linux klibc style.
// Isso vai ler no arquivo que está em ring0.
// #ugly
int linux_fgetc (FILE *f)
{
    unsigned char ch=0;
  
    if ( (void *) f == NULL ){
       return EOF;
    }

    return ( fread (&ch, 1, 1, f) == 1) ? (int) ch : EOF;
}


/*uClib style*/
char *fgets2 (char *s, int count, FILE *fp)
{
    char *p;
    int ch=0;

    p = s;

    if ( (void *) fp == NULL ){
       return (char *) 0;
    }

// Guard against count arg == INT_MIN. 
    while (count-- > 1) 
    {
        ch = getc(fp);

        if (ch == EOF){ break; }

        *p++ = ch;

        if (ch == '\n') { break; }
    };


    if ( ferror(fp) || (s == p) ) 
    {
        return (char *) 0;
    }

    *p = 0;

    return (char *) s;
}


/*
 * fputs2: 
 */

int fputs2 ( const char *str, FILE *stream )
{
    if ( (void *) stream == NULL ){ return EOF; }
// ugly
    for (; *str; ++str) 
    {
        int rc = putc (*str, stream);

        if (rc == EOF){ return EOF; }
    }
    return 1;
}

/*
 * gets:
 *     gets() devolve um ponteiro para string
 */
// #todo
// Do not use this. Use fgets instead.
// Using gets we can read more than we need.

char *gets2 (char *s)
{
    char *p;
    int ch=0;
    int t = 0;

	//printf("gets:\n");

    //salva
    p = s; 

    while (1)
    {
        ch = (int) getchar();

        if ( ch != -1 )
        {

            switch (ch) 
            {
                /* termina a string */
                case '\n':
                case VK_RETURN: 
                    s[t] =  (char) '\0'; 
                    goto done;
                    break;

                case '\b':
                case VK_BACKSPACE:
                    if(t > 0){  t--;  };
                    break;
            
			    default:
                    //s[t] = (char) ch;
					//t++;
					break;
            };

            //ok
            printf ("%c",ch);

            s[t] = (char) ch;
            t++;
        };
 
        // #bugbug
        // IF X86
        asm ("pause");
    };

done:
    //s[t] = (char) '\0';
    return (char *) p;
}


/*
 ***********************
 * puts:
 */
// #bugbug
// isso deve escrever no arquivo, assim como tudo em libc.
// ??

int puts2 ( const char *str )
{
    return (int) printf ("%s",str);
}


/*
 *************************************
 * getchar2:
 * O kernel pega no stdin que é a fila do teclado.
 * Isso funciona.
 */

int getchar2 (void)
{
    int Ret=0;

// #todo: 
// ? Já temos uma função para essa chamada ? 137.

Loop:
    Ret = (int) gramado_system_call ( 137, 0, 0, 0 ); 
    if (Ret > 0){
        return (int) Ret;    
    }
    goto Loop;

    //if ( (void *) stdin == NULL )
       //return EOF;

    // se glibc
    //return __getc(stdin);
}


/*
int getchar3(void);
int getchar3(void)
{
    int ch=0;
    ch=getc(stdin);
    if(ch!=EOF){
        putc(ch,stdout);
        return ch;
    }
    return EOF;
}
*/

/*
int getchar4(FILE *input_stream);
int getchar4(FILE *input_stream)
{
    int ch=0;
    ch=getc(input_stream);
    if(ch!=EOF){
        putc(ch,stdout);
        return ch;
    }
    return EOF;
}
*/

/*
int getchar5(FILE *output_stream);
int getchar5(FILE *output_stream)
{
    int ch=0;
    ch=getc(stdin);
    if(ch!=EOF){
        putc(ch,output_stream);
        return ch;
    }
    return EOF;
}
*/

/*
int getchar6(FILE *input_stream,FILE *output_stream);
int getchar6(FILE *input_stream,FILE *output_stream)
{
    int ch=0;
    ch=getc(input_stream);
    if(ch!=EOF){
        putc(ch,output_stream);
        return ch;
    }
    return EOF;
}
*/



int
rtl_GetS(
    unsigned char *buf,
    size_t size )
{
// Prompt into fixed length string. Size must include terminator.
// Credits: TempleOS
// #todo: Not tested yet.

    unsigned char *st;
    int ch=0;
    int i=0;

    if ((void*)buf == NULL)
        return 0;
    if (size <= 0) 
        return 0;

    while (TRUE) 
    {
        ch = (int) fgetc(stdin);

        if (ch == '\n') {
            //printf("%c",ch);
            //printf("\n");
            fflush(stdout);
            break;
      
        } else if (ch == '\b') {
            if (i>0) {
                i--;
                printf("%c",ch);
            }
      
        } else {
            if (i<size-1) {
                buf[i++]=ch;
                printf("%c",ch);
            }
        };
    }

    buf[i]=0;

    return (int) i;
}


/*
int feof(FILE *fp)
{
    if ( (void *) fp == NULL )
       return EOF;

    return fp->mode & __MODE_EOF;
}
*/


// feof:
// #todo: Explain ti better.
int feof (FILE *stream)
{
    if ( (void *) stream == NULL ){
        return EOF;
    }
    return (int) stream->eof;
}


/*
 //This function clears the end-of-file and error indicators for the stream stream. 
void clearerr(FILE *fp)
{
    if ( (void *) fp == NULL )
       return;

    //#bugbug: Isso precisa ser feito em ring0;
	//fp->mode &= ~(__MODE_EOF | __MODE_ERR);
}
*/


/*
 *********************************
 * ferror:
 */

int ferror (FILE *stream)
{
    if ( (void *) stream == NULL ){
       return EOF;
    }
    return (int) stream->error;
}

/*linux - klibc style*/
/*
static __inline__ int fseek(FILE *__f, off_t __o, int __w)
{
  extern off_t lseek(int, off_t, int

    if ( (void *) __f == NULL )
       return EOF;

  return (lseek(fileno(__f), __o, __w) == (off_t)-1) ? -1 : 0;
}
*/
	
	
/*
 **************************************
 * fseek:
 *     offset argument is the position that you want to seek to,
 *     and whence is what that offset is relative to.
 */
// The fseek function is used to change the file position of the stream stream. 
 
int fseek ( FILE *stream, long offset, int whence )
{
// #bugbug:
// This function is not working yet.

    off_t off=0;

    //debug_print ("fseek: TODO. Not implemented yet\n");
    //return -1;
    
	/*
    assert(stream);
    fflush(stream);
    off_t off = lseek(stream->fd, offset, whence);
    if (off < 0)
        return off;
    stream->eof = false;
    stream->error = 0;
    stream->have_ungotten = false;
    stream->ungotten = 0;
    return 0;
    */
    
    if ( (void*) stream == NULL ){
        debug_print ("fseek: stream \n");
        return -1;
    }
 
// lseek
    off = lseek ( fileno(stream), offset, whence );
    if (off < 0){
        debug_print ("fseek: lseek fail \n");
        return off;
    }

    // local
    
    // Not the end.
    // stream->_flags &= ~_IOEOF;
    // stream->eof = FALSE;
        
    // We do not know what is the next operation.
    // if (stream->_flags & _IORW)
    //     stream->_flags &= ~(_IOWRT|_IOREAD);

    // stream->error = 0;

    stream->have_ungotten = FALSE;
    stream->ungotten = 0;

    return off;
}


/*linux klibc style*/
// Isso vai escrever no arquivo que está em ring0.
int linux_fputc (int c, FILE *f)
{
    unsigned char ch = c;

    if ( (void *) f == NULL ){
       return EOF;
    }

    return (int) fwrite(&ch, 1, 1, f) == 1 ? ch : EOF;
}


/*
 * #todo: Implementar isso.
int putw (int w, FILE *stream);
int putw (int w, FILE *stream)
{

    if ( (void *) stream == NULL )
       return EOF;

    if ( fwrite ( (const char*) &w, sizeof (w), 1, stream ) != 1 )
        return EOF;

    return 0;
}
*/


/*
ssize_t getline (char **lineptr, size_t *n, FILE *stream); 
ssize_t getline (char **lineptr, size_t *n, FILE *stream)
{
    if ( (void *) stream == NULL )
       return -1;

  return getdelim (lineptr, n, '\n', stream);
}  
*/


// interna
// #todo: criar essa rotina na libc.
void debug_print (char *string)
{
    if( (void*) string == NULL )
        return;

    gramado_system_call ( 
        289, 
        (unsigned long) string,
        (unsigned long) string,
        (unsigned long) string );
}


/*
 * stdioSetCursor:
 * Estamos falando do posicionamento do cursor dentro da janela
 * e não dentro do terminal.
 */
//34 - set cursor.

void stdioSetCursor ( unsigned long x, unsigned long y )
{
    gramado_system_call ( 34, x, y, 0 );
}


/*
 * stdioGetCursorX:
 *     Get cursor x. 
 *     estamos falando do posicionamento do cursor dentro da janela
 *     e não dentro do terminal.
 */ 

unsigned long stdioGetCursorX (void)
{
    return (unsigned long) gramado_system_call ( 240, 0, 0, 0 );
}


/*
 * stdioGetCursorY:
 *     Get cursor y.
 *     estamos falando do posicionamento do cursor dentro da janela
 *     e não dentro do terminal. 
 */

unsigned long stdioGetCursorY (void)
{
    return (unsigned long) gramado_system_call ( 241, 0, 0, 0 );
}


//======================================================================
// scanf support (start)
//======================================================================

int scanf ( const char *fmt, ... )
{
    va_list ap;
    va_start (ap, fmt);

    int d;
    char *s;

	//%c
    int ch=0;

	//usado em %d
    int *i;
    char tmp[80];


	//o char.
    int c=0;

    char *cp;

    char *t;

    int nread=0;
    int size = (int) stdio_strlen (fmt);

    nread = 0;


    while (*fmt){

        // get
        c = *fmt;

        if (c == 0){  return 0;  }

		// pulando os espaços
		if ( isspace (c) ) 
		{
			while ( size > 0 && isspace(*fmt) )
			{
				nread++; 
				size--; 
				fmt++;
			};
			
			continue;
		}
		
        switch (*fmt++) 
		{
			/* string */
            case 's':              
				//pego o ponteiro para  a string, depois é só usar o gets. ??
				s = va_arg(ap, char *);
				
				//usaremos esse ponteiro para colocar uma string digitada.
				if ( (void *) s != NULL )
				{
				    gets (s);
				}else{
				    printf ("scanf: s null pointer\n");	
				}
				
				//testar ...
				//printf("string %s\n", s);
                break;
            
			/* int */
			case 'd': 
				//pego o ponteiro para  a string, depois é só usar o gets. ??
				i = va_arg(ap, int *);
				
				//usaremos esse ponteiro para colocar uma string digitada.
				if( (void *) i != NULL )
				{
				    //pego uma string de caracteres, que são números digitados.
					gets (tmp);
					
					//converte essa string em dígito
					i[0] = (int) stdio_atoi (tmp);                     
					
				}else{
				    printf ("scanf: s null pointer\n");	
				}
				
				//testar ...
				//printf("string %s\n", s);
			
                //d = va_arg(ap, int);
                //printf("int %d\n", d);
                break;
            
			/* char */
			case 'c':    
                t = va_arg (ap, char *);
				if ( (void *) t != NULL )
				{
				
                    while (1)
                    {
			            ch = (int) getchar ();
						
						if ( ch != -1 )
						{
						    t[0] = ch;
                            //printf("scanf ch={%c}",ch);
						    break;	
						}			
					};	
					
				}else{
					printf ("scanf: c null pointer\n");
				}; 				 
                break;
				
			//default:
            //    break; 
				
        };//switch
    };

   //va_end (ap);
   
   return 0;
}
//======================================================================
// scanf support (end)
//======================================================================


/*
 * sscanf: 
 * Scan items from a string in accordance with a format.  This is much
 * simpler than the C standard function: it only recognises %d without a
 * field width, and does not treat space in the format string or the
 * input any differently from other characters.  The return value is the
 * number of characters from the input string that were successfully
 * scanned, not the number of format items matched as in standard sscanf.
 * e.mcmanus@opengroup.org, 12 Feb 97
 * Credits: apple open source.
 */

int sscanf ( const char *str, const char *format, ... )
{
    va_list args;
    va_start(args, format);
    const char *start = str;

    for ( ; *format != '\0'; format++ )
    {
        if ( *format == '%' && format[1] == 'd' ){

            // #bugbug
            // Podemos definir isso no início da função.
            int positive;
            int value;
            int *valp;

            if (*str == '-'){
				positive = 0;
				str++;
            } else
				positive = 1;
			if (!isdigit(*str))
				break;
			value = 0;
            do {
				value = (value * 10) - (*str - '0');
				str++;
            } while (isdigit(*str));

			if (positive)
				value = -value;
			valp = va_arg(args, int *);
			*valp = value;
			format++;

        } else if (*format == *str) {
            str++;
        } else
            break;
    };

    va_end (args);

    return str - start;
}



//=============================================================
// printf (start)
//=============================================================
/*-
 * Copyright (c) 1986, 1988, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *	@(#)subr_prf.c	8.3 (Berkeley) 1/21/94
 */

//typedef unsigned long size_t;
//typedef long ssize_t;
//#ifdef __64BIT__
//typedef unsigned long long uintmax_t;
//typedef long long intmax_t;
//#else
typedef unsigned int uintmax_t;
typedef int          intmax_t;
//#endif
typedef unsigned char  u_char;
typedef unsigned int   u_int;
typedef unsigned long  u_long;
typedef unsigned short u_short;

typedef unsigned long long u_quad_t;  //#bugbug
typedef long long quad_t;             //#bugbug
typedef unsigned long uintptr_t;

//typedef long ptrdiff_t;
//#define NULL ((void*)0)
#define NBBY    8               /* number of bits in a byte */
char const hex2ascii_data[] = "0123456789abcdefghijklmnopqrstuvwxyz";
#define hex2ascii(hex)  (hex2ascii_data[hex])
//#define va_list __builtin_va_list
//#define va_start __builtin_va_start
//#define va_arg __builtin_va_arg
//#define va_end __builtin_va_end

//#obs:
//vamos usar o que está em ctype.h
//#define toupper(c)  ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))

static size_t stdio_strlen (const char *s)
{
    register size_t l=0;
    while (*s++){
        l++;
    };
    return l;
}

/* Max number conversion buffer length: 
 a u_quad_t in base 2, plus NUL byte. */
#define MAXNBUF  (sizeof(intmax_t) * NBBY + 1)


/*
 * ksprintn:
 *     Put a NUL-terminated ASCII number (base <= 36) in a buffer 
 * in reverse order; return an optional length and a pointer to 
 * the last character written in the buffer 
 * (i.e., the first character of the string).
 * The buffer pointed to by `nbuf' must have length >= MAXNBUF.
 */
 
static char *ksprintn ( 
    char *nbuf, 
    uintmax_t num, 
    int base, 
    int *lenp, 
    int upper )
{
    char *p, c;

    p = nbuf;

	//*p = ' ';
	*p = 0;

    do {
        c = hex2ascii (num % base);

        *++p = upper ? toupper(c) : c;

    } while (num /= base);


    if (lenp){
        *lenp = (p - nbuf);
    }

    return (p);
}


/*
 *******************************************************
 * kvprintf:
 *     Scaled down version of printf(3).
 * Two additional formats:
 *     The format %b is supported to decode error registers.
 *     Its usage is:
 *	printf("reg=%bn", regval, "*");
 * where  is the output base expressed as a control character, e.g.
 * 10 gives octal; 20 gives hex.  Each arg is a sequence of characters,
 * the first of which gives the bit number to be inspected (origin 1), and
 * the next characters (up to a control character, i.e. a character <= 32),
 * give the name of the register.  Thus:
 *	kvprintf("reg=%bn", 3, "102BITTWO1BITONEn");
 * would produce output:
 *	reg=3
 * XXX:  %D  -- Hexdump, takes pointer and separator string:
 *		("%6D", ptr, ":")   -> XX:XX:XX:XX:XX:XX
 *		("%*D", len, ptr, " " -> XX XX XX XX ...
 */
// #bugbug
// Passing a function via argument is not a good thing for our compiler.

int 
kvprintf ( 
    char const *fmt, 
    void (*func)( int, void * ), 
    void *arg, 
    int radix, 
    va_list ap )
{

    // #danger
    //#define PCHAR(c) { int cc=(c); if(func) (*func)(cc,arg); else *d++ = cc; retval++; }
    #define PCHAR(c) { int cc=(c); if(func) (*func)(cc,arg); else *d++ = cc; retval++; }

	char nbuf[MAXNBUF];
	char *d;
	const char *p, *percent, *q;
	u_char *up;

	int ch, n;
	uintmax_t num;
	int base, lflag, qflag, tmp, width, ladjust, sharpflag, neg, sign, dot;
	int cflag, hflag, jflag, tflag, zflag;
	int dwidth, upper;
	char padc;
	int stop = 0, retval = 0;

    num = 0;

    if (!func){
        d = (char *) arg;
    }else{
        d = NULL;
    };

    if (fmt == NULL){
        fmt = "(fmt null)n";
    }

    if (radix < 2 || radix > 36)
    {
        radix = 10;
    }

// loop
    
    for (;;){

        padc = ' ';
        width = 0;

        while ( ( ch = (u_char) *fmt++ ) != '%' || stop ) 
        {
            //if (ch == '')
            //    return (retval);
            if (ch == 0){
                return (retval);
            }
            PCHAR (ch);
        };

		percent = fmt - 1;
		
		qflag = 0; 
		lflag = 0; 
		ladjust = 0; 
		sharpflag = 0; 
		neg = 0;
		
		sign = 0; 
		dot = 0; 
		dwidth = 0; 
		upper = 0;
		
		cflag = 0; 
		hflag = 0; 
		jflag = 0; 
		tflag = 0; 
		zflag = 0;

        //
        // Loop.
        //
        
        reswitch:

        switch ( ch = (u_char) *fmt++ ){

        case '.':  dot = 1;        goto reswitch;
        case '#':  sharpflag = 1;  goto reswitch;
        case '+':  sign = 1;       goto reswitch;
        case '-':  ladjust = 1;    goto reswitch;

        case '%':  
            PCHAR (ch);
            break;

        case '*':
			if (!dot) 
			{
				width = va_arg(ap, int);
				
				if (width < 0)
				{
					ladjust = !ladjust;
					width = -width;
				}
				
			}else{
				
				dwidth = va_arg(ap, int);
			};
			goto reswitch;

        case '0':
			if (!dot) 
			{
				padc = '0';
				goto reswitch;
			}

		case '1': 
		case '2': 
		case '3': 
		case '4':
		case '5': 
		case '6': 
		case '7': 
		case '8': 
		case '9':
			for ( n = 0;; ++fmt ) 
			{
				n = n * 10 + ch - '0';
				ch = *fmt;
				
				if (ch < '0' || ch > '9') { break; }
			};

			if (dot)
				dwidth = n;
			else
				width = n;

			goto reswitch;

		case 'b':
			num = (u_int)va_arg(ap, int);
			p = va_arg(ap, char *);

            // #bugbug: 
            // This is very very dangeours.
            for (q = ksprintn(nbuf, num, *p++, NULL, 0); *q;)
            {
                PCHAR (*q--);
            };

            if (num == 0){ break; }

			for (tmp = 0; *p;) 
			{
				n = *p++;
				
				if (num & (1 << (n - 1))) 
				{
					PCHAR(tmp ? ',' : '<');
					
					for (; (n = *p) > ' '; ++p)
						PCHAR(n);
					tmp = 1;
				} else
					for (; *p > ' '; ++p)
						continue;
			};

            if (tmp){  PCHAR('>');  }

            break;

        case 'c':
            PCHAR (va_arg(ap, int));
            break;


        case 'D':
			up = va_arg(ap, u_char *);
			p = va_arg(ap, char *);
			
			if (!width)
				width = 16;
			while(width--) 
			{
				PCHAR(hex2ascii(*up >> 4));
				PCHAR(hex2ascii(*up & 0x0f));
				up++;
				
				if (width)
					for (q=p;*q;q++)
						PCHAR(*q);
			};
			break;

        // Decimal.
        case 'd':
        case 'i':
            base = 10;
            sign = 1;
            goto handle_sign;


        // Hexadecimal.
        case 'h':
			if (hflag) 
			{
				hflag = 0;
				cflag = 1;
			} else
				hflag = 1;
			goto reswitch;


		case 'j':
			jflag = 1;
			goto reswitch;


		case 'l':
			if (lflag) 
			{
				lflag = 0;
				qflag = 1;
			} else
				lflag = 1;
			goto reswitch;


		case 'n':
			if (jflag)
				*(va_arg(ap, intmax_t *)) = retval;
			else if (qflag)
				*(va_arg(ap, quad_t *)) = retval;
			else if (lflag)
				*(va_arg(ap, long *)) = retval;
			else if (zflag)
				*(va_arg(ap, size_t *)) = retval;
			else if (hflag)
				*(va_arg(ap, short *)) = retval;
			else if (cflag)
				*(va_arg(ap, char *)) = retval;
			else
				*(va_arg(ap, int *)) = retval;
			break;

        //octal
		case 'o':
			base = 8;
			goto handle_nosign;


		case 'p':
			base = 16;
			sharpflag = (width == 0);
			sign = 0;
			num = (uintptr_t)va_arg(ap, void *);
			goto number;
			
		case 'q':
			qflag = 1;
			goto reswitch;
			
		case 'r':
			base = radix;
			if (sign)
				goto handle_sign;
			goto handle_nosign;
			
		case 's':
			p = va_arg(ap, char *);
			if (p == NULL)
				p = "(null)";
			if (!dot)
				n = stdio_strlen (p);
			else
				for (n = 0; n < dwidth && p[n]; n++)
					continue;

			width -= n;

			if (!ladjust && width > 0)
				while (width--)
					PCHAR(padc);
			while (n--)
				PCHAR(*p++);
			if (ladjust && width > 0)
				while (width--)
					PCHAR(padc);
			break;


		case 't':
			tflag = 1;
			goto reswitch;


		// ?? case 'U': ??
		case 'u':
			base = 10;
			goto handle_nosign;


		case 'X':
			upper = 1;
		case 'x':
			base = 16;
			goto handle_nosign;
			
		case 'y':
			base = 16;
			sign = 1;
			goto handle_sign;
			
		case 'z':
			zflag = 1;
			goto reswitch;

        handle_nosign:
			sign = 0;
			if (jflag)
				num = va_arg(ap, uintmax_t);
			else if (qflag)
				num = va_arg(ap, u_quad_t);
			else if (tflag)
				num = va_arg(ap, ptrdiff_t);
			else if (lflag)
				num = va_arg(ap, u_long);
			else if (zflag)
				num = va_arg(ap, size_t);
			else if (hflag)
				num = (u_short)va_arg(ap, int);
			else if (cflag)
				num = (u_char)va_arg(ap, int);
			else
				num = va_arg(ap, u_int);
			goto number;
			
        handle_sign:
			if (jflag)
				num = va_arg(ap, intmax_t);
			else if (qflag)
				num = va_arg(ap, quad_t);
			else if (tflag)
				num = va_arg(ap, ptrdiff_t);
			else if (lflag)
				num = va_arg(ap, long);
			else if (zflag)
				num = va_arg(ap, ssize_t);
			else if (hflag)
				num = (short)va_arg(ap, int);
			else if (cflag)
				num = (char)va_arg(ap, int);
			else
				num = va_arg(ap, int);
			
        number:
			if (sign && (intmax_t)num < 0) {
				neg = 1;
				num = -(intmax_t)num;
			}
			p = ksprintn(nbuf, num, base, &tmp, upper);
			if (sharpflag && num != 0) {
				if (base == 8)
					tmp++;
				else if (base == 16)
					tmp += 2;
			}
			if (neg)
				tmp++;

			if (!ladjust && padc != '0' && width
			    && (width -= tmp) > 0)
				while (width--)
					PCHAR(padc);
			if (neg)
				PCHAR('-');
			if (sharpflag && num != 0) {
				if (base == 8) {
					PCHAR('0');
				} else if (base == 16) {
					PCHAR('0');
					PCHAR('x');
				}
			}
			if (!ladjust && width && (width -= tmp) > 0)
				while (width--)
					PCHAR(padc);

            while (*p){
                PCHAR(*p--);
            };

			if (ladjust && width && (width -= tmp) > 0)
				while (width--)
					PCHAR(padc);

			break;

        default:
            while (percent < fmt)
            {
                PCHAR (*percent++);
            };


			/* Since we ignore an formatting argument it is no
			 * longer safe to obey the remaining formatting
			 * arguments as the arguments will no longer match
			 * the format specs. */
			 
			stop = 1;
			break;
		};
	};
#undef PCHAR
}


static void xxxputchar ( int c, void *arg )
{
	/* add your putchar here */

	//printf("%c",c);
    putchar( (int) c );
}


/*
 *===========================================
 * printf:
 *     http://www.pagetable.com/?p=298
 */

    // #todo
    // Talvez usar semáforo aqui.

/*
int printf ( const char *fmt, ... ){

    va_list ap;
    va_start(ap, fmt);

    // The prototype.
    // int 
    // kvprintf ( 
    //     char const *fmt, 
    //     void (*func)( int, void* ), 
    //     void *arg, 
    //     int radix,    // ??10 gives octal; 20 gives hex.
    //     va_list ap );


    kvprintf ( fmt, xxxputchar, NULL, 10, ap );

    va_end (ap);

    return 0;
}
//=============================================================
// printf end
//=============================================================
*/


/*
 *===========================================
 * printf_draw:
 *     http://www.pagetable.com/?p=298
 */
// Nessa versão a rotina pintará na tela ao invés de
// colocar o char no arquivos.
// para isso ela configura o modo de output, voltando ao normal
// ao fim da rotina.
// >> Essa rotina é usada para debug no caso de não termos acesso
// a rotinas da api ou do x-server para pintura na tela.

int printf_draw ( const char *fmt, ... )
{
//Habilita o modo draw.
	libc_set_output_mode ( LIBC_DRAW_MODE );
	
// #todo
// Talvez usar semáforo aqui.
	
	va_list ap;
	va_start(ap, fmt);
	
	//int 
	//kvprintf ( char const *fmt, 
    //       void (*func)( int, void* ), 
	//	   void *arg, 
	//	   int radix, //??10 gives octal; 20 gives hex.
	//	   va_list ap );	
	
	kvprintf ( fmt, xxxputchar, NULL, 10, ap );
	
	//#todo.
	va_end (ap);
	
//reabilita o modo normal. Onde os caracteres serão colocados 
//no stdout.
	libc_set_output_mode ( LIBC_NORMAL_MODE );

    return 0;
}


/*glibc style*/
/* Optimizing.  */
/*
#ifdef	__OPTIMIZE__
extern __inline int vprintf (const char *__fmt, __gnuc_va_list __arg)
{
    if ( (void *) stdout == NULL )
       return EOF;

  return vfprintf (stdout, __fmt, __arg);
}
#endif 
*/



/*
 **********************
 * vfprintf:
 */

// #bugbug
// Estamos em ring3, não devemos acessar os elementos da estrutura de stream.


/*
int 
vfprintf ( 
    FILE *stream, 
    const char *format, 
    stdio_va_list argptr )
{

    debug_print("vfprintf: [TODO]\n");

	//#suspenso.
	//return (int) kvprintf ( format, NULL, stream->_ptr, 10, argptr );


    int size=0;

    //if ( (void *) stream == NULL )
       //return EOF;

    if ( (void *) stream == NULL )
    {
        return (int) (-1);
    } else {

        size = (int) stdio_strlen (format);

		//Se a string for maior que o tanto de bytes
		//disponíveis no arquivo
		if ( size > stream->_cnt )
		{
			return (int) (-1);
		}
		
		//recalcula o tanto de bytes disponíveis
		stream->_cnt = (int) (stream->_cnt - size);
		
		//coloca no buffer, usando o ponteiro atual.
		//#obs: isso provavelmente funcione, 
		//mas vamos tentar o segundo método.
		//sprintf ( stream->_ptr, format );
		//kvprintf ( format, NULL, stream->_ptr, 10, argptr );
		kvprintf ( format, NULL, stream->_p, 10, argptr );
		
		//atualiza o ponteiro atual.
		//stream->_ptr = stream->_ptr + size;
		stream->_p = stream->_p + size;
        
        return 0;
    };

    return (int) (-1);
} 
*/



/* #bsd style */
/*
int vprintf (const char *fmt, va_list ap)
{
    if ( (void *) stdout == NULL )
    {
       debug_print ("vprint: [FAIL] stdout\n");
       return EOF;
    }

    return (int) vfprintf (stdout, fmt, ap);
}
*/


/*
//printf que escreve no stdout. 
//#bugbug: não devemos usar stream em ring3.
int stdout_printf (const char *format, ...)
{
    va_list arg;
    va_start (arg, format);

    int done=0;

    if ( (void *) stdout == NULL ){
       return EOF;
    }

    done = vfprintf (stdout, format, arg);

    va_end (arg);

    return done;
}
*/


/*
// printf que escreve no stderr. 
int stderr_printf (const char *format, ... )
{
    va_list arg;
    va_start (arg, format);

    int done=0;
    
    if ( (void *) stderr == NULL ){
       debug_print ("stderr_printf: stderr\n");
       return EOF;
    }

    // Print!
    
    done = vfprintf (stderr, format, arg);

    va_end (arg);

    return done;
}
*/


/*
 * perror:
 */
// #todo:
// Oh Jees..., this function deserves a little bit of respect.
// The perror() function produces a message on standard error describing
// the last error encountered during a call to a system or library
// function.
// See: http://man7.org/linux/man-pages/man3/perror.3.html

void perror (const char *str)
{
// #todo
// Maybe we need to use stderr output;
// Vamos imprimir a string se ela for válida.
    if ( (void *) str != NULL ){
        printf ("Error: %s, ",str);
    }
// #bugbug
// There is a list of strings for errors.
// We gotta fill this list with string before using it.
    printf ("The last error number is {%d} [TODO].\n",errno);
    //printf ("The last is: {%s}.\n",errno_list[errno]);
}


// Reiniciamos a estrutura na biblioteca e
// chamamos lseek para o kernel modar o ponteiro
// la na estrutura de arquivos no kernel.

void rewind(FILE *stream)
{
    int fd=-1;

    if ( (void *) stream == NULL ){
        printf ("rewind: [FAIL] stream\n");
        return;
    }

// local
    //stream->_p = stream->_base;

// #todo:
    // fflush(stream);

    // Clear errors
    // local.
    //stream->_flags &= ~(_IOERR|_IOEOF);

// flags
// local
    //if (stream->_flags & _IORW)
    //    stream->_flags &= ~(_IOREAD|_IOWRT);

// #todo
// We need to do the same in the file.

	//unix 32V
	//fflush(stream);
	//lseek( fileno(stream), 0, 0);
	//stream->_p = stream->_base;
	//stream->_flag &= ~(_IOERR|_IOEOF);

    //se glibc
    //clearerr(stream);
    //(void) fseek(stream, 0L, SEEK_SET);
    //clearerr(stream);

    fd = fileno(stream);
    if(fd<0){
        printf("rewind: [ERROR] fd\n");
        return;
    }

// In ring3 buffer.
    stream->_p = stream->_base;
    stream->_w = 0;
    stream->_r = 0;

// #bugbug
// No gramado essa variavel se refere a quantidade de bytes disponiveis
// entao deve ser o tamanho do buffer.
// O kernel ja esta fazendo isso.
// Vamos ajustar isso aqui tambem.

// Não há bytes no buffer da libc
// #bugbug
// Isso deve ser o tamanho do buffer.
// pois é o contador de bytes restantes.
  
    stream->_cnt = stream->_lbfsize;

// rewind in ring0
    off_t v = lseek(fd,0,SEEK_SET);
    //if(v!=0){
    //    printf("Testing lseek: fail\n");
    //    asm ("int $3");
    //}
}

/*
void rewind(FILE* stream)
{
    ASSERT(stream);
    int rc = fseek(stream, 0, SEEK_SET);
    ASSERT(rc == 0);
}
*/



/*
void rewind2(FILE *f);
void rewind2(FILE *f)
{
    if ( (void*) f == NULL )
        return;

    fflush(f);
    lseek(fileno(f), 0, 0);
    f->_cnt = 0;
    f->_p = f->_base;
    f->_flags &= ~(_IOERR|_IOEOF);
    if (f->_flags & _IORW)
        f->_flags &= ~(_IOREAD|_IOWRT);
}
*/



/*
static void buffer_putch(char*& bufptr, char ch)
{
    *bufptr++ = ch;
}
*/


/*
static size_t __vsnprintf_space_remaining;
static void sized_buffer_putch(char*& bufptr, char ch)
{
    if (__vsnprintf_space_remaining) {
        *bufptr++ = ch;
        --__vsnprintf_space_remaining;
    }
}
*/



/*
 * snprintf:
 *     #todo
 */

int snprintf ( char *str, size_t count, const char *fmt, ... )
{
    va_list ap;
    va_start (ap, fmt);

    size_t ret=0;

    debug_print ("snprintf: [TODO]\n");

    if ( count <= 0 )
        return EOF;


	//#todo 
	//Isso parece fácil
	//ret = vsnprintf(str, count, fmt, ap);


    va_end (ap);
    
    return ret;
}


//?? #todo
// inicializa o fluxo padrão para o processo.
int stdio_initialize_standard_streams (void)
{
    debug_print("stdio_initialize_standard_streams: [FIXME] What is this?\n");
    return (int) gramado_system_call ( 
                     700, 
                     (unsigned long) stdin, 
                     (unsigned long) stdout, 
                     (unsigned long) stderr ); 
}


// wtf ... it looks so cool!
/*
file *file_purpose (file *f); 
file *file_purpose (file *f) 
{
	if (f->purpose == purpose_recv) {
		return stdin;
	}
	else if (f->purpose == purpose_send) {
		return stdout;
	}
	else if (f->purpose == (purpose_send | purpose_dbg)) {
		return dbgout;
	}
	else if (f->purpose == (purpose_send | purpose_err)) {
		return stderr;
	}
	else {
		// TODO: Warn about unknown file for pipe. Default to stderr
		return stderr;
	}
}
*/


/*credits: bsd*/
/*
 * Return the (stdio) flags for a given mode.  Store the flags
 * to be passed to an open() syscall through *optr.
 * Return 0 on error.
 */

/*
int
__sflags(const char *mode, int *optr);
int
__sflags(const char *mode, int *optr)
{
	int ret, m, o;

	//#todo: Falta essa macro. ela fica em assert.h
	//_DIAGASSERT(mode != NULL);

	switch (*mode++) {

	case 'r':	// open for reading 
		ret = __SRD;
		m = O_RDONLY;
		o = 0;
		break;

	case 'w':	// open for writing 
		ret = __SWR;
		m = O_WRONLY;
		o = O_CREAT | O_TRUNC;
		break;

	case 'a':	// open for appending 
		ret = __SWR;
		m = O_WRONLY;
		o = O_CREAT | O_APPEND;
		break;

	default:	// illegal mode 
		errno = EINVAL;
		return 0;
	}

	
	 //[rwa]\+ or [rwa]b\+ means read and write 
	 //e means set close on exec.
	 //f means open only plain files.
	 //l means don't follow symlinks.
	 //x means exclusive open.
	 
	for (; *mode; mode++)
		switch (*mode) {
		case '+':
			ret = __SRW;
			m = O_RDWR;
			break;
		case 'b':
			break;
		case 'e':
			o |= O_CLOEXEC;
			break;
		case 'f':
			o |= O_REGULAR;
			break;
		case 'l':
			o |= O_NOFOLLOW;
			break;
		case 'x':
			o |= O_EXCL;
			break;
		default:	// We could produce a warning here 
			break;
		}

	*optr = m | o;
	return ret;
}
*/


/*credits: bsd*/
/*
int
_fwalk(int (*function)(FILE *));
int
_fwalk(int (*function)(FILE *))
{
	FILE *fp;
	int n, ret;
	struct glue *g; //falta isso.

    //#todo: Falta essa macro. ela fica em assert.h
	_DIAGASSERT(function != NULL);

	ret = 0;
	for (g = &__sglue; g != NULL; g = g->next)
		for (fp = g->iobs, n = g->niobs; --n >= 0; fp++)
			if (fp->_flags != 0)
				ret |= (*function)(fp);
	return ret;
}
*/


/* 
 * apiStartTerminal:
 *     
 */

// 'Clona' e executa o noraterm como processo filho. 
// registra o terminal noraterm como terminal atual.
// pega o pid do terminal atual
// manda uma mensagem pedindo para o terminal dizer hello.

// #obs: Isso funcionou.

// #bugbug
// Maybe it's libc isn't the better place for this fucntion.
// Try libgui.

int libcStartTerminal (void)
{

    debug_print ("libcStartTerminal: [DEPRECATED]\n");

    /*
    int PID=0;
    // 'Clona' e executa o noraterm como processo filho. 
    //PID = (int) system_call ( 900, (unsigned long) "noraterm.bin", 0, 0 );

    PID = (int) gramado_system_call ( 900, 
                    (unsigned long) "noraterm.bin", 0, 0 );

    // Exibe o PID para debug.
    //printf ("PID = %d \n", PID);

    //registra o terminal como terminal atual.
    gramado_system_call ( 1003, PID, 0, 0 ); 

    //invalida a variável.
    PID = -1;

    //pega o pid do terminal atual
    PID = (int) gramado_system_call ( 1004, 0, 0, 0 ); 

    if ( PID <= 0 )
    {
        printf ("PID fail. We can't send the message\n");
        return -1;
    }

    	//manda uma mensagem pedindo para o terminal dizer hello.
	//__SendMessageToProcess ( PID, NULL, MSG_TERMINALCOMMAND, 2001, 2001 );

	return (int) PID;
    */

    return -1;
}


//
// stream buffer support
//

// see: 
// https://linux.die.net/man/3/setvbuf
// #ugly: See: setbuf_test1

void setbuf (FILE *stream, char *buf)
{
    debug_print ("setbuf: [FIXME]\n");
    if ( (void*) stream == NULL ){
        return;
    }
    if ( (void*) buf == NULL ){
        return;
    }
    setvbuf (stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}


/*
void setbuf_test1 (FILE *stream, char *buf);
void setbuf_test1 (FILE *stream, char *buf)
{
    if ( (void*) stream == NULL )
        return;

	if (buf == NULL)
		setvbuf(stream, NULL, _IONBF, 0);
	else
		setvbuf(stream, buf, _IOFBF, BUFSIZ);
}
*/


//If buf is a null pointer, this function makes stream unbuffered. 
//Otherwise, it makes stream fully buffered using buf as the buffer. 
//The size argument specifies the length of buf.
//This function is provided for compatibility with old BSD code. 
//Use setvbuf instead. 

void setbuffer (FILE *stream, char *buf, size_t size)
{
    if ( (void *) stream == NULL ){
       debug_print ("setbuffer: stream\n");
       return;
    }
    if ( (void *) buf == NULL ){
       debug_print ("setbuffer: buf\n");
       return;
    }
// todo: size.
    gramado_system_call ( 
        611, 
        (unsigned long) stream, 
        (unsigned long) buf, 
        (unsigned long) size ); 
}

//This function makes stream be line buffered, and allocates the buffer for you.
//This function is provided for compatibility with old BSD code. 
//Use setvbuf instead. 

void setlinebuf (FILE *stream)
{
    debug_print ("setlinebuf: [FIXME]\n");
    setvbuf (stream, (char *) 0, _IOLBF, 0);
}


//The setvbuf() function may be used on any open stream to change its buffer.
//If the argument buf is NULL, only the mode is affected; 
//a new buffer will be allocated on the next read or write operation. 
// See: https://linux.die.net/man/3/setvbuf

int 
setvbuf (
    FILE *stream, 
    char *buf, 
    int mode, 
    size_t size )
{

    // stream
    if ( (void *) stream == NULL ){
       debug_print ("setvbuf: stream\n");
       return -1;
    }

    // mode
    // #bugbug
    // Esse tratamanto de bits pode estar errado.
    // unbuffered, line buffered, fully buffered 
    if ( mode != _IONBF && 
         mode != _IOLBF && 
         mode != _IOFBF )
    {
        debug_print ("setvbuf: mode \n");
        //errno = EINVAL;
        return -1;
    }


    //size
    if (size <= 0){
        debug_print ("setvbuf: size fail\n");
        return (-1);
    }



    /*
    if (mode == _IONBF)
    {
        // Nesse caso. 
        // Limpa a estrutura e buffer nulo.
    }
    */

    // #todo size limits.


    //if (mode == _IOFBF || mode == _IOLBF){


    // Valid buffer.
    // Se foi passado um buffer válido.
    if ( (void *) buf != NULL ){
        debug_print ("setvbuf: using new buffer \n");
        stream->_base = (char *) buf;
        stream->_lbfsize = size;
    } 

    // Invalid buffer.    
    // Nenhum buffer foi passado.
    // Devemos criar um. Vamos usar o default.
    if ( (void *) buf == NULL ){
        debug_print ("setvbuf: using default buffer \n");
        stream->_base = (char *) stream->default_buffer;
        stream->_lbfsize = BUFSIZ;
    }

    //}



    // #bugbug
    // E se nesse momento ainda temos um ponteiro nulo para buffer?

    // r/w pointers and offsets.
    stream->_p = stream->_base;
    stream->_w = 0;
    stream->_r = 0;
    
    // Unset any buffering flag
    //stream->_flags &= ~(_IOFBF | _IOLBF | _IONBF);

    // And set what the user requested
    stream->_flags |= mode;

    return 0;
}

// #todo
// Create a wrapper called rtl_filesize()
unsigned int filesize(FILE *fp)
{
// #bugbug
// fseek() and rewind() are not working.

    unsigned int ret=0;

    if (!fp){ 
        debug_print ("filesize: [FAIL] fp\n");
        return 0;
    }

// #
// Well.
// We worked a little bit in these routines.
// It is getting better.

    fseek(fp, 0, SEEK_END);         //#bugbug
    ret = (unsigned int) ftell(fp);  //#bugbug
    rewind(fp);                     //#bugbug

    return (unsigned int) ret;
}


char *fileread (FILE *fp)
{
    unsigned int buffer_size = 0;
    char *buff;
    
    if (!fp){ 
        debug_print ("fileread: fp\n");
        return (char *) 0;
    }

    // #bugbug
    buffer_size = filesize(fp);

    buff = (char *) malloc (buffer_size);
    
    if (!buff){ 
        debug_print ("fileread: [FAIL] buff\n");
        return (char *) 0;
    }

    fread (buff, sizeof(char), buffer_size, fp);

    return (char *) buff;
}


/*
int dprintf (int fd, const char *format, ...)
{ 
    debug_print ("dprintf: [TODO]\n");
	return -1; 
}
*/

/*
int vdprintf (int fd, const char *format, va_list ap)
{ 
    debug_print ("vdprintf: [TODO]\n");
	return -1; 
}
*/


//
//==================================================================
//

/* we use this so that we can do without the ctype library */
#define __is_digit(c)	((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
    int i=0;

    while (__is_digit(**s))
    {
        i = i*10 + *((*s)++) - '0';
    };

    return i;
}


#define ZEROPAD   1  /* pad with zero */
#define SIGN      2  /* unsigned/signed long */
#define PLUS      4  /* show plus */
#define SPACE     8  /* space if plus */
#define LEFT     16  /* left justified */
#define SPECIAL  32  /* 0x */
#define SMALL    64  /* use 'abcdef' instead of 'ABCDEF' */


// oh Jeess ? x86 stuff.
#define do_div(n,base) ({ \
int __res; \
__asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
__res; })


static char *number ( 
    char *str, 
    int num, 
    int base, 
    int size, 
    int precision, 
    int type )
{
    
    char c, sign, tmp[36];
    const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    register int i=0;

    if (type & SMALL)
        digits="0123456789abcdefghijklmnopqrstuvwxyz";
    if (type & LEFT) 
        type &= ~ZEROPAD;
    if (base<2 || base>36)
        return 0;
    c = (type & ZEROPAD) ? '0' : ' ' ;

    if (type&SIGN && num<0) {
		sign='-';
		num = -num;
    } else
        sign=(type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);
    if (sign) size--;
	if (type&SPECIAL)
		if (base==16) size -= 2;
		else if (base==8) size--;
	i=0;
	if (num==0)
		tmp[i++]='0';
	else while (num!=0)
		tmp[i++]=digits[do_div(num,base)];
	if (i>precision) precision=i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type&SPECIAL)
		if (base==8)
			*str++ = '0';
		else if (base==16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	if (!(type&LEFT))
		while(size-->0)
			*str++ = c;
	while(i<precision--)
		*str++ = '0';
	while(i-->0)
		*str++ = tmp[i];
	while(size-->0)
		*str++ = ' ';
	return str;
}


// It was taken from linux 0.01. (GPL)
// It works yet.
// Just for fun. :^) 

/*
int 
Wirzenius_Torvalds_vsprintf (
    char *buf, 
    const char *fmt, 
    va_list args )
{
    int len=0;
    int i=0;

    char *str;
    char *s;
    int *ip;

    //flags to number()
    int flags;

    // width of output field 
    int field_width;	
	
    // min. # of digits for integers; 
    // max number of chars for from string.
    int precision;
   
    //'h', 'l', or 'L' for integer fields 
    int qualifier;


	for (str=buf ; *fmt ; ++fmt)
    {
		if (*fmt != '%')
        {
			*str++ = *fmt;
			continue;
		}

		//process flags
		flags = 0;
		repeat:
			++fmt;  //this also skips first '%' 
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}
		
		//get field width 
		field_width = -1;
		if (__is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			//it's the next argument 
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		//get the precision 
		precision = -1;
		if (*fmt == '.') {
			++fmt;	
			if (__is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				//it's the next argument 
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		//get the conversion qualifier 
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			break;

		case 's':
			s = va_arg(args, char *);
			len = strlen(s);
			if (precision < 0)
				precision = len;
			else if (len > precision)
				len = precision;

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			break;

		case 'o':
			str = number(str, va_arg(args, unsigned long), 8,
				field_width, precision, flags);
			break;

		case 'p':
			if (field_width == -1) {
				field_width = 8;
				flags |= ZEROPAD;
			}
			str = number(str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			break;

		case 'x':
			flags |= SMALL;
		case 'X':
			str = number(str, va_arg(args, unsigned long), 16,
				field_width, precision, flags);
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			str = number(str, va_arg(args, unsigned long), 10,
				field_width, precision, flags);
			break;

		case 'n':
			ip = va_arg(args, int *);
			*ip = (str - buf);
			break;

		default:
			if (*fmt != '%')
				*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			break;
		}
    }

    *str = '\0';


    return (int) (str-buf);
}
*/



// It's used by printf ?
//static char __printbuf[1024];


// It was taken from linux 0.01. gpl
// It works yet.
// Just for fun. :^) 

/*
int Torvalds_printf (const char *fmt, ...)
{

    va_list args;
    va_start(args, fmt);

    //int this_tty_id = 0;
    int i=0;

    // Pegando o id da tty desse processo.
    // #obs: Isso ja foi feito na inicialização da biblioteca.
    // Deltar isso.
    //this_tty_id = (int) gramado_system_call ( 266, getpid(), 0, 0 );  


    //escrevendo no virtual console. 0.
    //write_VC ( 0, 
    //    __printbuf,
    //    i=Wirzenius_Torvalds_vsprintf(__printbuf, fmt, args) );

    // #todo
    // We can use this function.
    
    write ( 0, 
        __printbuf,
        i=Wirzenius_Torvalds_vsprintf(__printbuf, fmt, args) );


    // #todo: talvez seja possível usar a base da stream.
    //write ( this_tty_id, 
        //__printbuf,
        //i=Wirzenius_Torvalds_vsprintf(stdout->_base, fmt, args) );

    va_end(args);
 
    return (int) i;
}
*/


//
//==================================================================
//

/*
int 
vsnprintf ( 
    char *str, 
    size_t size, 
    const char *format, 
    va_list ap )
{
    debug_print ("vsnprintf: [TODO]\n");
    return -1; 
}
*/


/*
int vscanf (const char *format, va_list ap)
{ 
    debug_print ("vscanf: [TODO]\n");
    return -1; 
}
*/


/*
int 
vsscanf ( 
    const char *str, 
    const char *format, 
    va_list ap )
{
    debug_print ("vsscanf: [TODO]\n");
    return -1; 
}
*/


/*
int 
vfscanf (
    FILE *stream, 
    const char *format, 
    va_list ap )
{ 
    debug_print ("vfscanf: [TODO]\n");

    if ( (void *) stream == NULL ){
       return EOF;
    }

    // ...

    return -1; 
}
*/


FILE *tmpfile (void)
{
    debug_print ("tmpfile: [TODO]\n");

    // steps:
    // + Salva um arquivo vazio com nome randômico.
    // + Abre esse arquivo com open.
    // + Cria uma stream para esse fd.

    return (FILE *) 0;
}


//tmpnam(): 
//SVr4, 4.3BSD, C89, C99, POSIX.1-2001.  
//POSIX.1-2008 marks tmpnam() as obsolete.
char *tmpnam (char *s)
{
    debug_print ("tmpnam: [TODO]\n");

    //if ( (void*) s == NULL ){}

    return NULL; 
}


//tmpnam_r() is a nonstandard extension that is 
//also available on a few other systems.
char *tmpnam_r (char *s)
{
    debug_print ("tmpnam_r: [TODO]\n");

    //if ( (void*) s == NULL ){}

    return NULL; 
}


char *tempnam (const char *dir, const char *pfx)
{
    debug_print ("tempnam: [TODO]\n"); 

    //if ( (void*) dir == NULL ){}
 
    return NULL; 
}



/*
 ERRORS  for tmpfile();

       EACCES Search permission denied for directory in file's path prefix.

       EEXIST Unable to generate a unique filename.

       EINTR  The call was interrupted by a signal; see signal(7).

       EMFILE The per-process limit on the number of open file descriptors
              has been reached.

       ENFILE The system-wide limit on the total number of open files has
              been reached.

       ENOSPC There was no room in the directory to add the new filename.

       EROFS  Read-only filesystem.
 */

// helper function

// IN: valid fd, mode
FILE *stdio_make_file( int fd, const char *mode )
{
    FILE *__file;

    // #todo
    
//
// given fd.
//

    //if (fd<0){
    //    printf("stdio_make_file: [FAIL] not valid fd\n");
    //    return NULL;
    //}


    __file = (FILE *) malloc ( sizeof(FILE) );

    if ( (void *) __file == NULL )
    {
        // debug_print(...)
        return NULL;
    }    
 
    __file->used = TRUE;
    __file->magic = 1234;
    __file->_cnt = 0;

// given fd
    __file->_file = fd;
// flags
    __file->_flags = 0;

    /*
    switch (*mode) {

    case 'r':
        __file->_flags |= _IOREAD;
        break;

    case 'a':
        lseek(fd, 0L, 2);
        //No break
    case 'w':
        __file->_flags |= _IOWRT;
        break;

    default:
        return(NULL);
    }
    */


    /*
	if (mode[1] == '+') {
		__file->_flags &= ~(_IOREAD|_IOWRT);
		__file->_flags |= _IORW;
	}
    */

    //__file->_tmpfname = (char *) strdup(filename);

    __file->_base = NULL;    // ??   
    __file->_r = 0;
    __file->_w = 0;
   
    //#todo: Initializa structure.
    //...

    return (FILE *) __file;
}


// #test
// Cria uma nova stream para o fd.
// O fd foi obtido anteriormente
// IN: valid fd, mode
FILE *fdopen (int fd, const char *mode)
{
    if (fd<0){
        printf("fdopen: Invalid fd\n");
        return NULL;
    }
    return (FILE *) stdio_make_file( fd, (const char *) mode);
}


/*
 * freopen:
 */
/*
    cool!!
    The freopen() function opens the file whose name 
    is the string pointed to by path and 
    associates the stream pointed to by stream with it. 
    The original stream (if it exists) is closed. 
    The mode argument is used just as in the fopen() function. 
    The primary use of the freopen() function is 
    to change the file associated with a 
    standard text stream (stderr, stdin, or stdout). 
*/

// See:
// https://linux.die.net/man/3/freopen

FILE *freopen (
    const char *pathname, 
    const char *mode, 
    FILE *stream )
{

    debug_print ("freopen: TODO: \n");

    if ( (void *) stream == NULL )
    {
       printf ("freopen: [FAIL] stream\n");
       return (FILE *) 0;
    }

    // ??
    // fopen() ??
    
    return (FILE *) 0;
}



/* #todo it needs 'cookie_io_functions_t'
FILE *fopencookie(void *cookie, const char *mode,
                         cookie_io_functions_t io_funcs);
FILE *fopencookie(void *cookie, const char *mode,
                         cookie_io_functions_t io_funcs)
{
    debug_print ("fopencookie: TODO: \n");
	return (FILE *) 0;
}
*/


FILE *open_memstream (char **ptr, size_t *sizeloc)
{
    debug_print ("open_memstream: TODO: \n");
	return (FILE *) 0;
}


FILE *open_wmemstream (wchar_t **ptr, size_t *sizeloc)
{
    debug_print ("open_wmemstream: TODO: \n");
	return (FILE *) 0;
}


FILE *fmemopen (void *buf, size_t size, const char *mode)
{
    printf ("fmemopen: TODO\n");
    return (FILE *) 0;
}


/*
int fgetpos(FILE* stream, fpos_t* pos)
{
    assert(stream);
    assert(pos);

    long val = ftell(stream);
    if (val == -1L)
        return 1;

    *pos = val;
    return 0;
} 
*/

int fgetpos (FILE *stream, fpos_t *pos )
{

    // #todo
    /*
    if ( !__validfp(stream) )
    {
        errno = EINVAL;
        return EOF;
    }
    */


    if ( (void *) stream == NULL )
    {
       printf("fgetpos: [FAIL] stream\n");
       return EOF;
    }
    
    *pos = ftell(stream);

    if (*pos < 0L)
    {
        return (-1);
    }
 
    return 0;
}


int fsetpos (FILE *stream, const fpos_t *pos)
{
// #bugbug
// fseek() is not working.
    if ( (void *) stream == NULL ){
        printf("fsetpos: [FAIL] stream\n");
        return EOF;
    }
    return (int) fseek (stream, (long) *pos, SEEK_SET);
}


int fpurge (FILE *stream)
{
    debug_print ("fpurge: [TODO] \n");
    
    if ( (void *) stream == NULL ){
        return EOF;
    }
 
    return -1; 
}



//#todo: esse protótipo pertence à stdio_ext.h
void __fpurge (FILE *stream)
{
    debug_print ("__fpurge: TODO: \n");

    if ( (void *) stream == NULL ){
       return;
    }
}

//ctermid - get controlling terminal name
//POSIX.1-2001, POSIX.1-2008, Svr4.
char *ctermid (char *s)
{
    printf ("ctermid: TODO: \n");
    
    if ( (void*) s == NULL )
        return NULL;

    return NULL; 
}


/*
 * stdioInitialize:
 *     Inicializa stdio para usar o fluxo padrão.
 *     O retorno deve ser (int) e falhar caso dê algo errado.
 */
// This routine ws called by crt0() in crt0.c
// #bugbug
// Essa estrutura lida com elementos de estrutura em ring3.
// #atenção: Algumas rotinas importantes estão usando esses elementos.
// #bugbug
// Precisamos usar os arquivos herdados do processo pai.
// Eles estão na estrutura de processo desse processo.
// #bugbug
// Talvez não seria o caso de apenas abrirmos os arquivos herdados.
// E o heap ??
// Em que momento foi inicializado o heap do processo? 
// crt0 chamou libcInitRT antes de chamar essa função.
// See: stdlib/stdlib.c
// See: crts/crt0.c

void stdioInitialize(void)
{
    int status = 0;
    int i=0;

// #bugbug
// Isso deve estar errado.
// provavelmente deveríamos apenas abrir os
// três arquivos herdados do processo pai.
// ?? Não sei se o processo init também terá um fluxo herdado.

    // Buffers para as estruturas.
    //unsigned char buffer0[BUFSIZ];
    //unsigned char buffer1[BUFSIZ];
    //unsigned char buffer2[BUFSIZ];

    // Buffers usados pelos arquivos.
    //unsigned char buffer0_data[BUFSIZ];
    //unsigned char buffer1_data[BUFSIZ];
    //unsigned char buffer2_data[BUFSIZ];

    debug_print ("stdioInitialize:\n");

//
// Pointers
//

    //stdin  = (FILE *) &buffer0[0];
    //stdout = (FILE *) &buffer1[0];
    //stderr = (FILE *) &buffer2[0];

// #bugbug
// Se essa inicializaçao falhar, nao temos como mostrar
// mensagens de erro.

// Os aplicativos precisam de uma estrutura clinet-side
// para gerenciarem o fluxo de dados dos arquivos,
// mesmo tendo os arquivos uma estrutura kernel-side.

// ===============
// stdin
    debug_print ("stdioInitialize: [1] stdin\n");  
    stdin = (FILE *) malloc( sizeof(FILE) );
    if ((void*) stdin == NULL)
    {
        debug_print ("stdioInitialize: stdin fail\n");
        //printf      ("stdioInitialize: stdin fail\n");
        exit(1);
    }
    memset( stdin, 0, sizeof(struct _iobuf) );

// ===============
// stdout
    debug_print ("stdioInitialize: [2] stdout\n");  
    stdout = (FILE *) malloc( sizeof(FILE) );
    if ((void*) stdout == NULL)
    {
        debug_print ("stdioInitialize: stdout fail\n");
        //printf ("stdioInitialize: stdout fail\n");
        exit(1);
    }
    memset( stdout, 0, sizeof(struct _iobuf) );

// ===============
// stderr
    debug_print ("stdioInitialize: [3] stderr\n");  
    stderr = (FILE *) malloc( sizeof(FILE) );
    if ((void*) stderr == NULL)
    {
        debug_print ("stdioInitialize: stderr fail\n");
        //printf ("stdioInitialize: stderr fail\n");
        exit(1);
    }
    memset( stderr, 0, sizeof(struct _iobuf) );

//
// Buffers
//

    // Buffers.
    // Buffers dos arquivos.
    //stdin->_base  = &buffer0_data[0];
    //stdout->_base = &buffer1_data[0];
    //stderr->_base = &buffer2_data[0];

// ========
// stdin
    stdin->_base = (char *) malloc(BUFSIZ);
    if ( (void*) stdin->_base == NULL ){
        debug_print ("stdioInitialize: stdin->_base fail\n");
        //printf ("stdioInitialize: stdin->_base fail\n");
        exit(1);
    }
    stdin->_lbfsize = BUFSIZ;
    stdin->_p  = stdin->_base;
    stdin->_cnt  = 0;  //BUFSIZ-1;
    stdin->_w  = 0;
    stdin->_r  = 0;  
    stdin->_file  = 0;

// ========
// stdout
    stdout->_base = (char *) malloc(BUFSIZ);
    if ( (void*) stdout->_base == NULL ){
        debug_print ("stdioInitialize: stdout->_base fail\n");
        //printf ("stdioInitialize: stdout->_base fail\n");
        exit(1);
    }
    stdout->_lbfsize = BUFSIZ;
    stdout->_p = stdout->_base;
    stdout->_cnt = 0; //BUFSIZ-1; 
    stdout->_w = 0;
    stdout->_r = 0;
    stdout->_file = 1;
        
// ========
// stderr   
    stderr->_base = (char *) malloc(BUFSIZ);
    if ( (void*) stderr->_base == NULL ){
        debug_print ("stdioInitialize: stderr->_base fail\n");
        //printf ("stdioInitialize: stderr->_base fail\n");
        exit(1);
    }
    stderr->_lbfsize = BUFSIZ;    
    stderr->_p = stderr->_base;
    stderr->_cnt = 0; //BUFSIZ-1;    
    stderr->_w = 0; 
    stderr->_r = 0;
    stderr->_file = 2;

//
// # libc mode #
//

// #bugbug:
// Vamos usar o modo draw até implementarmos o modo normal.

// Normal mode:
// Os caracteres são colocados em stdout.
    //__libc_output_mode = LIBC_NORMAL_MODE;

// Draw mode:
// Os caracteres são pintados na tela em full screen.
// usando um dos consoles virtuais do kernel.
    __libc_output_mode = LIBC_DRAW_MODE;

// #importante:
// Vamos conectar o processo filho ao processo pai
// atraves das ttys privadas dos processos.
// o processo pai é o terminal. (às vezes)
// #bugbug:
// Esse metodo não funcionara no caso
// do processo filho do shell

// #bugbug
// Deveria ser o contrário.
// O pai ser master e o filho slave.

// #fixme
// Something is not working.
// Suspended for now.
// I don't wanna see error messages everytime we launch a command.

    /*
      gramado_system_call ( 
          267,
          getpid(),    //master
          getppid(),   //slave pai(terminal)
          0 );
    */

//
// tty
//

// ok
// This is the tty of this process.
    __libc_tty_id = (int) gramado_system_call ( 266, getpid(), 0, 0 ); 
// Clear prompt[] buffer.
    prompt_clean();
    //debug_print ("stdioInitialize: done\n");
}

// #test
/*
 *******************************
 * unix_get:
 */

char __unix_get_buf[512];
int __unix_get_nread = 1;

int unix_get (int ifile) 
{
    char *ibuf;
    //static ibuf;
    
    //printf ("$");
    //fflush(stdout);
    
    //#todo
    //if (ifile<0){return -1;}    
    
    // #ugly
    if (--__unix_get_nread)
    {
        return (*ibuf++);
    }

    // #ugly
    if (__unix_get_nread = read (ifile, __unix_get_buf, 512) )
    {
        if (__unix_get_nread < 0){
            goto err;
        }
        
        ibuf = __unix_get_buf;
        
        return (int) (*ibuf++);
    }

    __unix_get_nread = 1;
    
    return (-1);

err:
    __unix_get_nread = 1;
    
    printf ("unix_get: [FAIL] read error\n");
    return (-1);
}


/*
void 
write_char (
    int ch,
    FILE *f,
    int *nwritten );

void 
write_char (
    int ch,
    FILE *f,
    int *nwritten )
{

    if (fputc(ch, f) == EOF)
    {
        *nwritten = -1;
    
    }else{
        
        // ??
        ++(*nwritten);
    };
}
*/


/*
char *minix_gets(char *s);
char *minix_gets(char *s)
{
	register FILE *stream = stdin;
	register int ch;
	register char *ptr;

	ptr = s;
	while ((ch = getc(stream)) != EOF && ch != '\n')
		*ptr++ = ch;

	if (ch == EOF) {
		if (feof(stream)) {
			if (ptr == s) return NULL;
		} else return NULL;
	}

	*ptr = '\0';
	return s;
}
*/


/*
char *minix_fgets (char *s, register int n, register FILE *stream);
char *minix_fgets (char *s, register int n, register FILE *stream)
{
	register int ch;
	register char *ptr;

	ptr = s;
	while (--n > 0 && (ch = getc(stream)) != EOF) {
		*ptr++ = ch;
		if ( ch == '\n')
			break;
	}
	if (ch == EOF) {
		if (feof(stream)) {
			if (ptr == s) return NULL;
		} else return NULL;
	}
	*ptr = '\0';
	return s;
}
*/


/*
int minix_fgetpos(FILE *stream, fpos_t *pos);
int minix_fgetpos(FILE *stream, fpos_t *pos)
{
	*pos = ftell(stream);
	
	if (*pos == -1) 
	    return -1;
	
	return 0;
}
*/

/*
int minix_fsetpos(FILE *stream, fpos_t *pos);
int minix_fsetpos(FILE *stream, fpos_t *pos)
{
	return fseek(stream, *pos, SEEK_SET);
}
*/

/*
int minix_puts(register const char *s);
int minix_puts(register const char *s)
{
	register FILE *file = stdout;
	register int i = 0;

	while (*s) {
		if (putc(*s++, file) == EOF) return EOF;
		else i++;
	}
	if (putc('\n', file) == EOF) return EOF;
	return i + 1;
}
*/


/*
int minx_fputs(register const char *s, register FILE *stream);
int minx_fputs(register const char *s, register FILE *stream)
{
	register int i = 0;

	while (*s) 
		if (putc(*s++, stream) == EOF) return EOF;
		else i++;

	return i;
}
*/

//===============================


/*
int data_getc(FILE *_stream);
int data_getc(FILE *_stream)
{
    int data = 0;
    
    _stream->_cnt--;
    
    if ( _stream->_cnt >= 0 )
    {
        data =  (int) (_stream->_ptr & 0xFF);
        _stream->_ptr++;
        return (int) data;
    }else{
        _filbuf(_stream);
    };
}
*/


/*
void data_putc (int ch,FILE *_stream);
void data_putc (int ch,FILE *_stream)  
{
    _stream->_cnt--;
    
    if ( _stream->_cnt >= 0 )
    {
        _stream->_ptr[0] = (char) ( ch & 0xFF );
        _stream->_ptr++;
        return;
    }else{
        _flsbuf (ch,_stream);
        return;
    };
}
*/

//
// End
//


