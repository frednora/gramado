// kstdio.c
// unix-like i/o support.
// Created by Fred Nora.

#include <kernel.h>

//
// File pointers.
//

// Global list of files.
unsigned long file_table[NUMBER_OF_FILES];

struct file_table_info_d  FileTableInfo;

// Global list of pipes.
unsigned long pipeList[NUMBER_OF_PIPES];

// Standard stream.
// Initialized by kstdio_initialize() in kstdio.c
file *stdin;
file *stdout;
file *stderr;

int kstdio_standard_streams_initialized=FALSE;

// VFS
// Not initialized yet.
file *vfs_fp;

// Rootdir for bootvolume and systemvolume.
// Initialized by fsInit() in fs.c
file *volume1_rootdir_fp;
file *volume2_rootdir_fp;

//...

// 0
//Pipe para a rotina execve particular 
//usada no no init do ambiente Gramado Core.
file *pipe_gramadocore_init_execve;

// 1
//Pipe usado pela rotina execve.
file *pipe_execve;

int g_inputmode=0;

// The prompt[] support for input() and standard streams.
char prompt[PROMPT_SIZE];      //buffer para stdin
char prompt_out[PROMPT_SIZE];  //buffer para stdout
char prompt_err[PROMPT_SIZE];  //buffer para strerr
unsigned long prompt_pos=0;
unsigned long prompt_max=0; 
unsigned long prompt_status=0;

// ?
int stdio_terminalmode_flag=0;
// ?
int stdio_verbosemode_flag=0;

// Global sync stuff.
// see: kstdio.h
unsigned long syncList[SYNC_COUNT_MAX];



// ---------------------------
static void __clear_prompt_buffers(void);
static void __initialize_file_table(void);
static void __initialize_inode_table(void);
static void __initialize_stdin(void);
static void __initialize_stdout(void);
static void __initialize_stderr(void);

static void __initialize_virtual_consoles(void);

static char *_vsputs_r(char *dest, char *src);
// ---------------------------

// Feed stdin.
int kstdio_feed_stdin(int ch)
{

// ASCII Codes (0x00~0x7F)
// Extended ASCII Codes (0x80~0xFF)

    char Data = (char) (ch & 0xFF);
    char ch_buffer[2];

    if ((void*) stdin == NULL){
        goto fail;
    }
    if (stdin->used != TRUE){
        goto fail;
    }
    if (stdin->magic != 1234){
        goto fail;
    }

// #bugbug
// We're not respecting the flag.

// Can write.
    stdin->sync.can_write = TRUE;
// Can write and read.
    stdin->_flags = __SWR;
// Data
    ch_buffer[0] = (char) (Data & 0xFF);

// Write
// Write 1 byte.
// see: fs.c

    int wbytes = 0;

    wbytes = 
        (int) file_write_buffer ( 
            (file *) stdin, 
            (char *) ch_buffer, 
            (int) 1 );

// Can read.
// O aplicativo precisa disso.
    stdin->sync.can_read = TRUE;
// Can read.
// O worker no kernel precisa disso.
    stdin->_flags = __SRD;

// Return the number of written bytes.
    return (int) wbytes;

fail:
    return (int) -1;
}

// service 8002
// IN: fd for the new stdin
// OUT: TRUE or FALSE.
int sys_setup_stdin(int stdin_fd)
{
    file *f;
    struct process_d *p;
    pid_t current_process = -1;

    if (stdin_fd < 0 || stdin_fd >= 32){
        return FALSE;
    }

// pid
    current_process = (pid_t) get_current_process();
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX )
        return FALSE;
// Process structure
    p = (struct process_d *) processList[current_process];
    if ( (void*)p == NULL ){
        return FALSE;
    }
    if (p->used != TRUE){
        return FALSE;
    }
    if (p->magic != 1234){
        return FALSE;
    }

// File structure
    f = (file *) p->Objects[stdin_fd];
    if ((void*) f == NULL){
        return FALSE;
    }
    if (f->used != TRUE){
        return FALSE;
    }
    if (f->magic != 1234){
        return FALSE;
    }

// The new stdin.
    stdin = (file *) f;
// Permission for writing.
    stdin->sync.can_write = TRUE;
// flags
    stdin->_flags |= __SWR;

// ok, we have a new stdin.
// this way the keyboard can put the bytes into this new file.
    return TRUE;
}

int is_socket(file *f)
{
// Fail
    if ((void *) f == NULL){
        return FALSE;
    }
    if (f->magic != 1234){
        return FALSE;
    }
// Yes
    if (f->____object == ObjectTypeSocket){
        return TRUE;
    }
// No
    return FALSE;
}

int is_virtual_console(file *f)
{
// Fail
    if ((void *) f == NULL){
        return FALSE;
    }
// Yes
    if (f->____object == ObjectTypeVirtualConsole){
        return TRUE;
    } 
// No
    return FALSE;
}

// Kernel size version of the
// standard input() libc function.
unsigned long kinput(unsigned long ch)
{
    char c = (char) ch;
    register int i=0;

// Input mode.
// #bugbug:
// Estamos nos referindo ao tipo de linha, 
// se é simples ou multiplas linhas.
// Mas temos outra flag relativa à imput mode 
// que trata dos eventos.
// See: ???
// Onde estão as flags ???

    if (g_inputmode == INPUT_MODE_LINE)
    {
        if (prompt_pos >= PROMPT_SIZE)
        {
            //printk ("kstdio-input: [FAIL] INPUT_MODE_LINE full buffer!\n");
            goto fail;
            // refresh_screen ();
            //return (unsigned long) 0; 
        }
    }

    //tem que ter o tamanho de um arquivo.
    if (g_inputmode == INPUT_MODE_MULTIPLE_LINES)
    {
        if (prompt_pos >= PROMPT_SIZE)
        {
            //printk("kstdio-input: [FAIL] INPUT_MODE_MULTIPLE_LINES full buffer\n");
            goto fail;
            //refresh_screen();
            //return (unsigned long) 0; 
        }
    }

// Trata o caractere digitado. 

    switch (c){

    // Enter.
    // +se for modo comando devemos finalizar com zero.
    // +se for modo texto, devemos apenas incluir os caracteres \r \n.
    //case 0x1C:
    case VK_RETURN:
        //modo linha 
        if (g_inputmode == INPUT_MODE_LINE)
        {
            prompt[prompt_pos] = (char )'\0'; //end of line.
            //#todo: ?? ldiscCompare();
            // o compare está no aplicativo.
            for (i=0; i<PROMPT_MAX_DEFAULT; i++)
            {
                prompt[i]     = (char) '\0';
                prompt_out[i] = (char) '\0';
                prompt_err[i] = (char) '\0';
            };
            prompt_pos=0;
            goto input_done;
        }
        //modo multiplas linhas 
        if (g_inputmode == INPUT_MODE_MULTIPLE_LINES)
        {
            prompt[prompt_pos] = (char )'\r'; 
            prompt_pos++;
            prompt[prompt_pos] = (char )'\n';
            prompt_pos++;
        }
        break;

    // Backspace
    case 0x0E:
        if (prompt_pos <= 0)
        {
            prompt_pos = 0;
            prompt[prompt_pos] = (char ) '\0';
            break; 
        }
        // Apaga o anterior (no buffer).
        prompt_pos--;
        prompt[prompt_pos] = (char ) '\0';
        break;

    // ...

    // Para qualquer caractere que não sejam 
    // os especiais tratados acima.
    default:
        prompt[prompt_pos] = c;
        prompt_pos++;
        break;
    };

//input_more:
    return 0;
input_done:
    return VK_RETURN;
fail:
    // #bugbug: Slow.
    refresh_screen();
    return (unsigned long) 0; 
}

/*
 * printchar:
 *     Coloca o caractere na string ou imprime.
 * Essa função chama uma rotina que deverá tratar o caractere e 
 * em seguida enviá-lo para a tela.
 * Essa rotina é chamada pelas funções: /print/printi/prints.
 */
// #importante
// Se a string existe colocamos nela,
// caso contrário imprimimos no backbuffer.
// Vamos aproveitar esse momento para ativarmos a
// pintura no caso dos caraters enviados para uma 
// stream de output, como stdout.
// Ativaremos a rotina de mostrar na tela só no momento em que 
// encontramos um fim de linha.
// IN:
//     **str // #dangerdanger
//     c
void printchar(char **str, int c)
{

// Valid pointer.
// We have a string.
    if (str){

        if (c == '\n'){
            // tty->print_pending = 1;
        }
        
        // #dangerdanger
        **str = c;

        ++(*str);

// Invalid pointer
// No string.
    } else {
        putchar (c);
    };
}

// Used for tests.
void putchar_K(void)
{
    putchar('K');
}

/*
 * putchar:
 *     Put a char on the screen. (libC).
 *     Essa rotina chama uma rotina de tratamento de caractes, 
 * somente depois é que o caractere será enviado para a tela.
 *     Essa rotina é chamada pelas funções: /printchar/input/.
 */
// Para virtual consoles.
// Em tty/console.c

int putchar(int ch)
{
// + Draw char.
// + Do not refresh char.

// #todo
// Maybe we can filter the printable chars
// for the console font.

    if (ch<0){
        goto fail;
    }
    if (fg_console<0 || fg_console > 3){
        goto fail;
    }

// + Draw char.
// + Do not refresh char.
    console_outbyte( ch, fg_console );

    return (int) ch;
fail:
    return (int)(-1);
}


/*
 *  == Segue aqui o suporte a função 'printk' ====
 *
 * #obs:
 * Em user mode temos uma modelo mais tradicional de printk,
 * talvez seja bom implementa-lo aqui também.
 */

/*
 * prints:
 *     Rotina de suporta a printk. 
 */
 
// #bugbug
// We need to create a cleaner routine.
// This one is a mess.
// '**out' #dangerdanger

int 
prints ( 
    char **out, 
    const char *string, 
    int width, 
    int pad )
{
    register int pc = 0, padchar = ' ';
    register int len = 0;
    register const char *ptr;

    if (width > 0) 
    {
        // ?? ugly
        for (ptr = string; *ptr; ++ptr) ++len;
        if (len >= width) width = 0;
        else width -= len;
        if (pad & PAD_ZERO) padchar = '0';
    }


    if ( !(pad & PAD_RIGHT) ) 
    {
        //#ugly
        for ( ; width > 0; --width)
        {
            printchar (out,padchar);
            ++pc;
        };
    }

    //#ugly
    for ( ; *string; ++string )
    {
        printchar (out, *string);
        ++pc;
    };

    //#ugly
    for ( ; width > 0; --width )
    {
        printchar (out,padchar);
        ++pc;
    };

    return (int) pc;
}


/*
 * printi:
 *     Rotina de suporta a printk.
 */
// '**out' #dangerdanger
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
    char print_buf[PRINT_BUF_LEN];
    register char *s;
    register int t, neg = 0, pc = 0;
// loop
    register unsigned int u = i;

    if (i == 0){
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return (int) prints (out, print_buf, width, pad);
    }

    if ( sg && 
         b == 10 && 
         i < 0 )
    {
        neg = 1;
        u = -i;
    }

    s = ( print_buf + ( PRINT_BUF_LEN -1 ) );
    *s = '\0';

    while (u)
    {
        t = (u % b);
        
        // ugly
        
        if ( t >= 10 )
            t += letbase - '0' - 10;
        
        *--s = t + '0';
        u /= b;
    };

    if (neg)
    {
        if ( width && (pad & PAD_ZERO) ){
            printchar (out, '-');
            ++pc;
            --width;
        }else { 
            *--s = '-'; 
        };
    }

// #obs: 
// retorna pc + o retorno da função.
// ugly shit
    
    return (int) pc + prints(out, s, width, pad);
}


// print:
// Rotina de suporta a printk.
// #bugbug
// E se essa rotina for chamada com o primeiro argumento nulo?
// vai escrever na IVT ?
// Vai chamar alguma rotina passando esse emsmo endereço de buffer.
//Atençao:
// print() nao analisa flags como runlevel ou kernel phase.

int print ( char **out, int *varg )
{
    register int width = 0;
    register int pad = 0;
    register int pc = 0;

// #bugbug
// Different sizes.
// Compilation warning!
    register char *format = (char *) (*varg++);

    char scr[2];

    // ugly
    for ( ; *format != 0; ++format ) 
    {
        if ( *format == '%' ) 
        {
            ++format;

            width = pad = 0;

            if ( *format == '\0' ) { break; }
            if ( *format == '%' )  { goto ____out; }

            if ( *format == '-' )
            {
                ++format;
                pad = PAD_RIGHT;
            }

            while ( *format == '0' ) 
            {
                ++format;
                pad |= PAD_ZERO;
            };

            // #ugly
            for ( ; 
                *format >= '0' && *format <= '9'; 
                ++format )
            {
                width *= 10;
                width += *format - '0';
            };

            //#ugly
            if ( *format == 's' ) 
            {
                // #bugbug: definition here.
                register char *s = *((char **)varg++);
                 pc += prints (out, s?s:"(null)", width, pad);

                continue;
            }

            if ( *format == 'd' ){
                pc += printi (out, *varg++, 10, 1, width, pad, 'a');
                continue;
            }

            if ( *format == 'x' ){
                pc += printi (out, *varg++, 16, 0, width, pad, 'a');
                continue;
            }

            if ( *format == 'X' ){
                pc += printi (out, *varg++, 16, 0, width, pad, 'A');
                continue;
            }

            if ( *format == 'u' ){
                pc += printi (out, *varg++, 10, 0, width, pad, 'a');
                continue;
            }

            // #ugly
            if ( *format == 'c' ) 
            {
                // 'char' are converted to int 
                // then pushed on the stack.
                scr[0] = *varg++;
                scr[1] = '\0';
                pc += prints (out, scr, width, pad);
                continue;
            }

        // Os caracteres normais diferentes de "%".
        }else{
            ____out:
                printchar (out, *format);
                ++pc;
        };
    };

// Se o buffer existe, finaliza a string?
    if (out){ 
        **out = '\0'; 
    }

    return (int) pc;
}


/*
 * printk_old:
 *     @field 2
 *     The printk function.
 *     Assuming sizeof(void *) == sizeof(int).
 * Em user mode temos uma modelo mais tradicional de printk,
 * talvez seja bom implementa-lo aqui tambem.
 */
// #bugbug
// #todo:
// Devemos tentar usar o mesmo printk implementado na libc
// Essa aqui não está no padrão.
// #todo:
// Vamos substtuir essa função por uma de licensa bsd.
// Olhar na biblioteca.
// #suspensa
// Essa implementação foi feita para 32bit e não funciona
// por inteiro em long mode.
// Usaremos kinguio_printf por enquanto.

int printk_old( const char *format, ... )
{
    register int *varg = (int *) (&format);

// #bugbug:
// Se print() está usando '0' como buffer,
// então ele está sujando a IVT. 
// #:
// print() nao analisa flags.

    return (int) print(0,varg);
}    

// print() is a helper function for this one.
/*
int vsprintf(char *string, const char *format, va_list ap);
int vsprintf(char *string, const char *format, va_list ap)
{
}
*/

// ===================================

void 
kinguio_i2hex( 
    unsigned int val, 
    char *dest, 
    int len )
{
    char *cp;
    register int i=0;
    int x=0;
    unsigned n=0;  //??

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
        x = (n & 0xF);
        n >>= 4;
        
        // #
        *--cp = x + ((x > (HEX_LEN+1)) ? 'A' - 10 : '0');
    };

    dest[len] = '\0';

    cp = &dest[0];

    for (i=0; i<len; i++)
    {
        if (*cp == '0'){
            cp++;
        }else{
            strcpy (dest,cp);
            break;
        };
    }

    cp = &dest[0];
    n = strlen(cp);

    memset( (dest+n), 0, (8-n) );
}

char *kinguio_itoa (int val, char *str) 
{
    int value = val;
    char *valuestring = (char *) str;
    int min_flag=0;
    char swap=0; 
    char *p;

    if (0 > value)
    {
        *valuestring++ = '-';
        value = -____INT_MAX> value ? min_flag = ____INT_MAX : -value;
    }

    p = valuestring;

    do {
         *p++ = (char) (value % 10) + '0';
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

static char *_vsputs_r(char *dest, char *src)
{
    unsigned char *usrc = (unsigned char *) src;
    unsigned char *udest = (unsigned char *) dest;

    while ( *usrc )
    { 
        *udest++ = *usrc++; 
    };

    return (char *) udest;
}

int 
kinguio_vsprintf(
    char *str, 
    const char *fmt, 
    va_list ap )
{
    char *str_tmp = str;
    int index=0;
    unsigned char u=0;
    int d=0;
    char c=0; 
    char *s;
    char buffer[256];  //#bugbug: Too short.
    char _c_r[] = "\0\0";

    while ( fmt[index] )
    {
        switch (fmt[index]){

        case '%':
            ++index;
            switch (fmt[index]){

            case 'c':
                *_c_r = c = (char) va_arg (ap, int);
                str_tmp = _vsputs_r(str_tmp,_c_r);
                break;

            case 's':
                s = va_arg (ap, char*);
                str_tmp = _vsputs_r(str_tmp,s);
                break;

            case 'd':
            case 'i':
                d = va_arg (ap, int);
                kinguio_itoa (d,buffer);
                str_tmp = _vsputs_r(str_tmp,buffer);
                break;

            case 'u':
                u = va_arg (ap, unsigned int);
                kinguio_itoa (u,buffer);
                str_tmp = _vsputs_r(str_tmp,buffer);
                break;

            case 'X':
            case 'x':
                d = va_arg (ap, int);
                kinguio_i2hex(d, buffer,8);
                str_tmp = _vsputs_r(str_tmp,buffer);
                break;

            default:
                str_tmp = _vsputs_r(str_tmp,"%%");
                break;
            }
            break;

        default:
            *_c_r = fmt[index];
            str_tmp = _vsputs_r(str_tmp,_c_r);
            break;
        }
        ++index;
    }

    return (int) ( (long) str_tmp - (long) str );
}


// Print a string.
void kinguio_puts(const char* str)
{
    register int i=0;
    size_t StringLen=0;
    int _char=0;

    if (!str){
        return;
    }

    StringLen = (size_t) strlen(str);
    if (StringLen<=0)
        return;

// Print chars. 
    for (i=0; i<StringLen; i++)
    {
        _char = (int) ( str[i] & 0xFF );
        // Draw, and refresh a single char.
        console_outbyte2 (_char, fg_console);
        // Draw, but not refresh.
         //console_outbyte (_char, fg_console);
    };
}

// printk worker.
// Credits: Nelson Cole. Project Sirius/Kinguio.

int kinguio_printf(const char *fmt, ...)
{
    static char data_buffer[1024];
    int ret=0;

// If the virtual console isn't full initialized yet.
    if (Initialization.is_console_log_initialized != TRUE){
        return -1;
    }

    memset (data_buffer, 0, 1024); 

//----------
    va_list ap;
    va_start(ap, fmt);
    ret = kinguio_vsprintf(data_buffer, fmt, ap);
    va_end(ap);
//-----------

// Print the data buffer.
    kinguio_puts(data_buffer);

    return (int) ret;
}
// ===================================

int kputs(const char *str)
{
    if ((void *) str == NULL){
        return -1;
    }
    return (int) printk("%s",str);
    //return (int) printk ("%s",str);
}

/*
 * sprintf_old:
 *     @field 2
 *     int ksprintf ( char * str, const char * format, ... );
 *     Composes a string with the same text that would be 
 * printed if format was used on printk, 
 * but instead of being printed, the content is stored 
 * as a C string in the buffer pointed by str.
 */
// #suspensa
// Essa implementação foi feita para 32bit e não funciona
// por inteiro em long mode.

int sprintf_old ( char *str, const char *format, ... )
{
    register int *varg = (int *) (&format);

    return (int) print (&str, varg);
}    

// mysprintf: (ksprintf)
// Variable parameter form to achieve ksprintf.
int mysprintf(char *buf, const char *fmt, ...)
{
    register int n=0;

// Write the fmt format string to the buffer buf 
    va_list args;
    va_start(args, fmt);
    n = (int) kinguio_vsprintf(buf, fmt, args);
    //buf[n] = '\0';
    va_end(args);

    return (int) n;
}

int k_ungetc( int c, file *f )
{
    if (c == EOF){ 
        return (int) c;
    }
    if ((void *) f == NULL){
        return EOF;
    }

// #todo: 
// flag eof.

    //stream->flags = (stream->flags & ~_IOEOF);

    f->_p--;
    f->_p[0] = (char) c;

    return (int) c;
}

long k_ftell(file *f)
{
    if ((void *) f == NULL){
        return EOF;
    }
    return (long) (f->_p - f->_base);
}

// Return the fd.
int k_fileno(file *f)
{
    if ((void *) f == NULL){
        return EOF;
    }
    return (int) f->_file;  
}

int k_fgetc(file *f)
{
    int ch=0;

    if ((void *) f == NULL){
        printk ("k_fgetc: f\n");
        goto fail;
    }

    //(--(p)->_r < 0 ? __srget(p) : (int)(*(p)->_p++))

//#fim.
//cnt decrementou e chegou a zero.
//Não há mais caracteres disponíveis entre 
    //stream->_ptr e o tamanho do buffer.
		
		/*
		if ( stream->_cnt <= 0 )
		{
			stream->_flags = (stream->_flags | _IOEOF); 
			stream->_cnt = 0;
			
		    //printk ("#debug: fgetc: $\n");
			
			//isso funciona, significa que a estrutura tem ponteiro e base validos.
			//printk("show fgetc:: %s @\n", stream->_base );
		    //refresh_screen();
			
			return EOF;
		};
		*/
		
		//#debug
		//nao podemos acessar um ponteiro nulo... no caso endereço.

// Buffer pointer
    if ( f->_p == 0 ){
        printk ("k_fgetc: [FAIL] f->_p \n");
        goto fail;
    }

// #obs: 
// Tem que ter a opção de pegarmos usando o posicionamento
// no buffer. O terminal gosta dessas coisas.

// Get the char.
    ch = (int) *f->_p;

// Update the pointer
    f->_p++;
    f->_cnt--;

    return (int) ch;

fail:
    //#debug
    // #bugbug: Slow
    printk ("k_fgetc: fail\n");
    refresh_screen();
    return EOF;
}

int k_feof (file *f)
{
    int ch=0;
 
    if ((void *) f == NULL){
        return (int) (-1);
    } else {
        ch = k_fgetc(f);
        if (ch == EOF){
            return (int) 1;
        }else{
            return 0;
        };
    };

//checar se o eof foi atingido.
    // return( (stream->_flag & _IOEOF) );

    return 0;
}

int k_ferror(file *f)
{
    if ((void *) f == NULL){
        return EOF;
    }
    return (int) ((f->_flags & _IOERR));
}

// k_fseek:
// offset argument is the position that you want to seek to,
// and whence is what that offset is relative to.
// #todo
// Checar limites do offset.
// #dictioonary-ptbr: whence=deonde
// #todo
// IN:
int k_fseek( file *f, long offset, int whence )
{
    register int i=0;
    char *p;

    if ((void *) f == NULL){
        debug_print ("k_fseek: f\n");
        goto fail;
    }
    if (whence<0){
        goto fail;
    }

/*
SEEK_SET
    The file offset is set to offset bytes.
SEEK_CUR
    The file offset is set to its current location plus offset bytes.
SEEK_END
    The file offset is set to the size of the file 'plus' offset bytes.
    (#danger: It goes beyond!)
*/

// Partindo 'de onde'?
// #see: 
// kstdio.h
// https://man7.org/linux/man-pages/man2/lseek.2.html
    switch (whence){

// Não podemos aceitar valores negativos,
// pois ultrapassa o inicio do arquivo.
// Não podemos colocar o ponteiro de leitura
// depois do fim do arquivo.
// Não mexe no offset de escrita.
// continuaremos a escrever de onde paramos.
// Atualiza o offset de leitura.
// A quantidade de bytes disponíveis
// continua a mesma, pois não incluímos ou retiramos 
// dados de dentro do buffer.
// O ponteiro de trabalho para refletir a próxima
// leitura. mas não precisa isso, pois o offset é relativo à base.
    case SEEK_SET:
        if (offset<0){
            offset=0;
        }
        if ( offset > f->_cnt ){
            offset = f->_cnt;
        }
        // Read and write devem começar de (base + offset).
        f->_r = offset;
        f->_w = offset;
        f->_p = (f->_base + offset); 
        //printk("offset: %d\n",offset);
        //refresh_screen();
        //while(1){}
        goto done;
        break;

// Levando em consideração o ponteiro de trabalho atual,
// vamos mudar o offset e reposicionar o ponteiro de trabalho.
// Não mexe no offset de escrita.
// continuaremos a escrever de onde paramos.
// A quantidade de bytes disponíveis
// continua a mesma, pois não incluímos ou retiramos 
// dados de dentro do buffer.
// Atualiza o offset de leitura.
// mas agora em relação ao ponteiro de trabalho e 
// não em relação ao ponteiro da base.
// temos que respeitar a quantidade disponivel.
    case SEEK_CUR:
        if ( offset > f->_cnt ){
            offset = f->_cnt;
        }
        // Read and write devem começar de (current + offset).
        f->_p = (f->_p + offset);
        f->_r = (f->_p - f->_base);
        f->_w = (f->_p - f->_base);
        goto done;
        break;

// #todo: 
// For now we can't accept positive values.
// porque vai ultrapassar o fim do arquivo.
// não vamos mexer no ponteiro de escrita,
// Vamos continuar escrevendo de onde paramos,
// pois nenhuma informação foi incluida ou retirada do arquivo.
// Não vamos mexer na quantidade de bytes disponível no buffer,
// pois não estamos incluindo informação no buffer.
    case SEEK_END:
        if(offset>0){
            offset=0;
        }
        // Read and write devem começar de (end of file + offset).
        // Estamos falando do fim do arquivo e não do fim do buffer.
        // #bugbug: 
        // O arquivo não pode ser maior que o buffer disponível pra ele.
        if ( f->_fsize > f->_lbfsize ){
            panic("k_fseek: #todo f->_fsize > f->_lbfsize\n"); 
        }
        if ( (f->_fsize + offset) > f->_lbfsize ){
            panic("k_fseek: #todo (f->_fsize + offset) > f->_lbfsize\n");
        } 
        f->_r = (f->_fsize + offset);
        f->_w = (f->_fsize + offset);
        f->_p = 
            ( f->_base + (f->_fsize + offset) );
        goto done;
        break;

// #see: kstdio.h
// BUFSIZ is the minimum for a valid buffer.
// Lets clear only the first part of the buffer.
    case GRAMADO_SEEK_CLEAR:
        p = (char *) f->_base;
        //#test
        for (i=0; i<BUFSIZ; i++)
        {
           *p = 0;
           p++; 
        };
        f->_p = f->_base;
        f->_r = 0;
        f->_w = 0;
        f->_cnt = f->_lbfsize;
        goto done;
        break;
    
    //case SEEK_DATA:
    //    break;
    //case SEEK_HOLE:
    //    break;

// Default
    default:
        //printk ("default:\n");
        goto fail;
        break;
    };

fail:
    printk ("k_fseek fail\n");
    refresh_screen();
    return (int) (-1);
done:
    return 0;
}


// bsd stuff
/*
 * Various output routines call wsetup to be sure it is safe to write,
 * because either _flags does not include __SWR, or _buf is NULL.
 * _wsetup returns 0 if OK to write, nonzero otherwise.
 */
/*
int __swsetup(file *fp)
int __swsetup(file *fp)
{
    return -1;
}
*/


/*
 * Write the given character into the (probably full) buffer for
 * the given file.  Flush the buffer out if it is or becomes full,
 * or if c=='\n' and the file is line buffered.
 */
// #importante
// Isso é usado em __sputc no bsd.
/*
int __swbuf (int c, file *fp);
int __swbuf (int c, file *fp)
{
    return -1;
}
*/


int k_fputc( int ch, file *f )
{

// Pointer validation
    if ((void *) f == NULL){
        debug_print ("k_fputc: f\n");
        panic       ("k_fputc: f\n");
        return EOF;
    }

// The buffer is not full,
// and the char is not a '\n'.

    if ( f->_cnt > 0 && ch != '\n' )
    {
        // Write it into the buffer.
        ksprintf ( f->_p, "%c", ch);
        // Update the pointer.
        f->_p++;
        // Update the offset for the writer.
        f->_w++;
        // Update the counter.
        // How many bytes the buffer still have?
        f->_cnt--;
        // The buffer is full.
        // #todo: What we need to do now?
        if ( f->_cnt <= 0 ){
            debug_print ("k_fputc: [DEBUG] _cnt\n");
            panic       ("k_fputc: [DEBUG] _cnt\n");
            f->_cnt = 0;
            return EOF;
        }
        f->_flags |= __SRD;  //pode ler.
        return (int) ch;  
    }

// Atualizar as flags quando o buffer está cheio.
// se o buffer está cheio.

    /*
    //Now writing. 
    if (stream->flags & _IORW)
    {
        stream->flags &= ~_IOREAD;
        stream->flags |= _IOWRITE;
    }
    */

    //File is not writable. 
    //if (!(stream->flags & _IOWRITE))
         //return (EOF);        

//fail:
    return EOF;
}


int k_fscanf (file *f, const char *format, ... )
{
    panic ("k_fscanf: #todo\n");
    return -1;
}

/*
int vfprintf ( file *stream, const char *format, stdio_va_list argptr );
int vfprintf ( file *stream, const char *format, stdio_va_list argptr )
{
}
*/

//Writes format output of a stdarg argument list to a file.
/* 
int vfprintf(file *stream, const char *format, va_list ap);
int vfprintf(file *stream, const char *format, va_list ap)
{
	int n;             // Characters written. 
	char buffer[1024]; // Buffer.             
	
	// Format string. 
	n = vsprintf(buffer, format, ap);
	
	// Write formated string to file. 
	if (n > 0)
		fputs(buffer, stream);
	
	return (n);
}
*/

void k_rewind(file *f)
{
    //fseek (f, 0L, SEEK_SET);
    if ((void *) f == NULL){
        return;
    }
    f->_p = f->_base;
}

// Change f->magic to 4321.
int k_fclose (file *f)
{
    if ((void *) f == NULL){
        return EOF;
    }
    if (f->used != TRUE){
        return EOF;
    }
    if (f->magic != 1234){
        return EOF;
    }
    f->used = TRUE;
// Inverted.
    f->magic = 4321;
    return 0;
}

int k_fputs(const char *str, file *f)
{
    size_t StringSize=0;

    if ((void*) str == NULL){
        goto fail;
    }
    if ((void *) f == NULL){
        goto fail;
    } 

// Get string size.
    StringSize = (size_t) strlen(str);
    if (StringSize > f->_cnt){
        goto fail;
    }
// Update _cnt.
    f->_cnt = (int) (f->_cnt - StringSize);
// Put
    ksprintf( f->_p, str );
// Update _p.
    f->_p = (f->_p + StringSize);
// OK
    return 0;
fail:
    return (int) (-1);
}

// k_setbuf:
// see: 
// https://linux.die.net/man/3/setvbuf
void k_setbuf(file *f, char *buf)
{
    if ((void *) f == NULL){
        debug_print("k_setbuf: f\n");
        return;
    }
    if ((void *) buf == NULL){
        debug_print("k_setbuf: buf\n");
        return;
    }

//#todo
//se o buffer é válido.

    //if (stream->_bf._base != NULL) 
    //{
        //if (stream->cnt > 0)
            //fflush (stream);

        //free (stream->buf);
    //}

// Udate stream.
    f->_bf._base = buf;
    //stream->_lbfsize = size;        
    // ?? stream->bufmode = mode;
    f->_p = buf;
    // f->_p = f->_bf._base
    // f->cnt = 0;

// #todo
// Setup all the buffer elements.
// ...
}

void k_setbuffer (file *f, char *buf, size_t size)
{
  // #todo
  // Check parameters.

    /*
    if ( (void *) f == NULL ){
        debug_print("k_setbuffer: f\n");
        return;
    }

    if ( (void *) buf == NULL ){
        debug_print("k_setbuffer: buf\n");
        return;
    }
    */


    if ( (void *) f == NULL ){
        // #todo
        // Maybe we need a message here.
        return;

    }else{

        //#todo
        //se o buffer é válido.
        
        //if (stream->_bf._base != NULL) 
        //{
            //if (stream->cnt > 0)
                //fflush (stream);
                
            //free (stream->buf);
        //}
        
        // Udate stream.
        f->_bf._base = buf;
        f->_lbfsize = size;        
        // ?? stream->bufmode = mode;
        f->_p = buf;
        // ??stream->cnt = 0;
        //...
    };
}

void k_setlinebuf(file *f)
{
    printk ("k_setlinebuf: #todo\n");
    if ((void *) f == NULL){
        return;
    }
}

// #todo
// Not implemented yet.
int 
k_setvbuf (
    file *f, 
    char *buf, 
    int mode, 
    size_t size )
{
    if ( (void *) f == NULL ){
        // MSG ?
        return -1;
    }else{

        //#todo
        //se o buffer é válido.

        //if (f->_bf._base != NULL) 
        //{
            //if (f->cnt > 0)
                //fflush (f);
                
            //free (f->buf);
        //}
        
        // Udate stream.
        f->_bf._base = buf;
        f->_lbfsize = size; 
        // ?? f->bufmode = mode;

        f->_p = buf;
        // ??f->cnt = 0;
        //...
    };

    return 0;
}

// Maybe we can do some operations 
// in a regular file using ioctl.
// EINVAL request or argp is not valid.
int 
regularfile_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    debug_print ("regularfile_ioctl: #todo\n");

    if ( fd < 0 || fd >= OPEN_MAX )
    {
        return (int) (-EBADF);
    }

//...

    return -1;
}


// #warning
// Because the libc has hardcoded file descriptor values for the
// standard streams, we gotta use the same values.
// So, this way we need to use the same file descriptors.
// We're gonna use STDIN_FILENO.
static void __initialize_stdin(void)
{
// Called by kstdio_initialize().

    int fd = STDIN_FILENO;
    int slot=-1;

// stdin
// pega slot em file_table[] para stdin

    slot = get_free_slots_in_the_file_table();
    if ( slot < 0 || slot >= NUMBER_OF_FILES ){
        x_panic("__initialize_stdin: slot");
    }
    stdin = (file *) file_table[slot];
    if ( (void*) stdin == NULL ){
        x_panic("__initialize_stdin: stdin");
    }
    stdin->filetable_index = slot;

// fd
    stdin->_file = fd;  //0
// This is a regular file.
    stdin->____object = ObjectTypeFile;
// sync
    stdin->sync.sender_pid = (pid_t) -1;
    stdin->sync.receiver_pid = (pid_t) -1;
    stdin->sync.can_read = TRUE;
    stdin->sync.can_write = FALSE;
    stdin->sync.can_execute = FALSE;
    stdin->sync.can_accept  = FALSE;
    stdin->sync.can_connect = FALSE;
// _flags
    stdin->_flags = (__SWR | __SRD); 

// #bugbug
// Esse buffer está sendo usado pelo console.

    stdin->_base = &prompt[0];    //See: kstdio.h
    stdin->_p = &prompt[0];
    stdin->_bf._base = stdin->_base;
    stdin->_lbfsize = PROMPT_SIZE; //128; //#todo
    stdin->_r = 0;
    stdin->_w = 0;
    stdin->_cnt = PROMPT_SIZE;
    stdin->_tmpfname = "KSTDIN  TXT";
    stdin->fd_counter = 1;

    // inode support.
    // pega slot em inode_table[] 
    slot = get_free_slots_in_the_inode_table();
    if (slot<0 || slot >=NUMBER_OF_FILES){
        x_panic("__initialize_stdin: [FAIL] stdin inode slot\n");
    }
    stdin->inode = (struct inode_d *) inode_table[slot];
    stdin->inodetable_index = slot;
    if ((void*) stdin->inode == NULL){
        x_panic("__initialize_stdin: [FAIL] stdin inode struct\n");
    }
    stdin->inode->filestruct_counter = 1; //inicialize

// Copy the name
    memcpy ( 
        (void *)      stdin->inode->path, 
        (const void*) stdin->_tmpfname, 
              sizeof( stdin->inode->path ) );

    stdin->used = TRUE;
    stdin->magic = 1234;
}

// #warning
// Because the libc has hardcoded file descriptor values for the
// standard streams, we gotta use the same values.
// So, this way we need to use the same file descriptors.
// We're gonna use STDOUT_FILENO.
static void __initialize_stdout(void)
{
// Called by kstdio_initialize().

    int slot=-1;
    int fd = STDOUT_FILENO;

// stdout
// pega slot em file_table[] para stdout

    slot = get_free_slots_in_the_file_table();
    if ( slot < 0 || slot >= NUMBER_OF_FILES ){
        x_panic("__initialize_stdout: slot");
    }
    stdout = (file *) file_table[slot];
    if ((void*) stdout == NULL){
        x_panic("__initialize_stdout: stdout");
    }
    stdout->filetable_index = slot;

// fd
    stdout->_file = fd;  //1

// This is a virtual console.
// Configurando a estrutura de stdout.
// This is a virtual console device. Used to output
// directly into the virtual console.
// It is a device.

    stdout->____object = ObjectTypeVirtualConsole; 

// sync
    stdout->sync.sender_pid = (pid_t) -1;
    stdout->sync.receiver_pid = (pid_t) -1;
    stdout->sync.can_read = TRUE;
    stdout->sync.can_write = TRUE;
    stdout->sync.can_execute = FALSE;
    stdout->sync.can_accept  = FALSE;
    stdout->sync.can_connect = FALSE;
// _flags
    stdout->_flags = (__SWR | __SRD); 

// buffer
    stdout->_base = &prompt_out[0];  //See: kstdio.h
    stdout->_p = &prompt_out[0];
    stdout->_bf._base = stdout->_base;
    stdout->_lbfsize = PROMPT_SIZE; //128; //#todo
    stdout->_r = 0;
    stdout->_w = 0;
    stdout->_cnt = PROMPT_SIZE;
    stdout->_tmpfname = "KSTDOUT DEV";
    stdout->fd_counter = 1;
    // ...
    // inode support.
    // pega slot em inode_table[] 
    slot = get_free_slots_in_the_inode_table();
    if (slot<0 || slot >=NUMBER_OF_FILES){
        x_panic("__initialize_stdout: stdout inode slot\n");
    }
    stdout->inode = (struct inode_d *) inode_table[slot];
    stdout->inodetable_index = slot;
    if ( (void*) stdout->inode == NULL ){
        x_panic("__initialize_stdout: stdout inode struct\n");
    }
    stdout->inode->filestruct_counter = 1; //inicialize

// Copy the name
    memcpy( 
        (void*)       stdout->inode->path, 
        (const void*) stdout->_tmpfname, 
              sizeof( stdout->inode->path ) );

    stdout->used = TRUE;
    stdout->magic = 1234;
}

// #warning
// Because the libc has hardcoded file descriptor values for the
// standard streams, we gotta use the same values.
// So, this way we need to use the same file descriptors.
// We're gonna use STDERR_FILENO.
static void __initialize_stderr(void)
{
// Called by kstdio_initialize().

    int fd = STDERR_FILENO;
    int slot = -1;

// stderr
// pega slot em file_table[] para stderr

    slot = get_free_slots_in_the_file_table();
    if ( slot < 0 || slot >= NUMBER_OF_FILES ){
        x_panic("__initialize_stderr: slot");
    }
    stderr = (file *) file_table[slot];
    if ((void*) stderr == NULL){
        x_panic("__initialize_stderr: stderr");
    }
    stderr->filetable_index = slot;

// fd
    stderr->_file = fd;  //2
// This is a regular file.
    stderr->____object = ObjectTypeFile;
// sync
    stderr->sync.sender_pid = (pid_t) -1;
    stderr->sync.receiver_pid = (pid_t) -1;
    stderr->sync.can_read    = TRUE;
    stderr->sync.can_write   = TRUE;
    stderr->sync.can_execute = FALSE;
    stderr->sync.can_accept  = FALSE;
    stderr->sync.can_connect = FALSE;
// _flags
    stderr->_flags = (__SWR | __SRD); 
// buffer
    stderr->_base = &prompt_err[0];  //See: kstdio.h
    stderr->_p = &prompt_err[0];
    stderr->_bf._base = stderr->_base;
    stderr->_lbfsize = PROMPT_SIZE; //128; //#todo
    stderr->_r = 0;
    stderr->_w = 0;
    stderr->_cnt = PROMPT_SIZE;
    stderr->_tmpfname = "KSTDERR TXT";
    stderr->fd_counter = 1;
    // ...
    // inode support.
    // pega slot em inode_table[] 
    slot = get_free_slots_in_the_inode_table();
    if (slot<0 || slot >=NUMBER_OF_FILES){
        x_panic("__initialize_stderr: stderr inode slot\n");
    }
    stderr->inode = (struct inode_d *) inode_table[slot];
    stderr->inodetable_index = slot;
    if ((void*) stderr->inode == NULL){
        x_panic("__initialize_stderr: stderr inode struct\n");
    }
    stderr->inode->filestruct_counter = 1; //inicialize

// Copy the name
    memcpy ( 
        (void*)       stderr->inode->path, 
        (const void*) stderr->_tmpfname, 
              sizeof( stderr->inode->path ) );

    stderr->used = TRUE;
    stderr->magic = 1234;
}


file *new_file(object_type_t object_type)
{
// Called by kstdio_initialize().

    file *new_file;
    int slot = -1;

// stderr
// pega slot em file_table[] para stderr

    slot = get_free_slots_in_the_file_table();
    if ( slot < 0 || slot >= NUMBER_OF_FILES ){
        x_panic("new_file: slot");
    }
    new_file = (file *) file_table[slot];
    if ((void*) new_file == NULL){
        x_panic("new_file: new_file");
    }
    new_file->filetable_index = slot;

// fd
// We don't have this yet.
    //new_file->_file = STDERR_FILENO;  //2
    new_file->_file = -1;

// This is a regular file.
    new_file->____object = object_type;
// sync
    new_file->sync.sender_pid = (pid_t) -1;
    new_file->sync.receiver_pid = (pid_t) -1;
    new_file->sync.can_read    = TRUE;
    new_file->sync.can_write   = TRUE;
    new_file->sync.can_execute = FALSE;
    new_file->sync.can_accept  = FALSE;
    new_file->sync.can_connect = FALSE;
// _flags
    new_file->_flags = (__SWR | __SRD); 

// buffer

    //new_file->_base = &prompt_err[0];  //See: kstdio.h
    //new_file->_p = &prompt_err[0];

    new_file->_base = kmalloc(PROMPT_SIZE);
    if ( (void*) new_file->_base == NULL )
        panic("new_file: new_file->_base");
    new_file->_p = new_file->_base;

    new_file->_bf._base = new_file->_base;
    new_file->_lbfsize = PROMPT_SIZE; //128; //#todo
    new_file->_r = 0;
    new_file->_w = 0;
    new_file->_cnt = PROMPT_SIZE;
    
    // #todo
    new_file->_tmpfname = "NONAME.TXT";

    new_file->fd_counter = 1;
    // ...
    // inode support.
    // pega slot em inode_table[] 
    slot = get_free_slots_in_the_inode_table();
    if (slot<0 || slot >=NUMBER_OF_FILES){
        x_panic("new_file: new_file inode slot\n");
    }
    new_file->inode = (struct inode_d *) inode_table[slot];
    new_file->inodetable_index = slot;
    if ((void*) new_file->inode == NULL){
        x_panic("new_file: new_file inode struct\n");
    }
    new_file->inode->filestruct_counter = 1; //inicialize

// Copy the name
    memcpy ( 
        (void*)       new_file->inode->path, 
        (const void*) new_file->_tmpfname, 
              sizeof( new_file->inode->path ) );

    new_file->used = TRUE;
    new_file->magic = 1234;

// Return the pointer for a new stream.
    return (file*) new_file;
}


// Os buffers dos arquivos acima.
// prompt[]
// Esses prompts são usados como arquivos.
// São buffers para as streams.
// See: kstdio.h
static void __clear_prompt_buffers(void)
{
    register int i=0;
    for (i=0; i<PROMPT_SIZE; i++)
    {
        prompt[i] = (char) '\0';
        //prompt_in[i] = (char) '\0';  //#todo #maybe
        prompt_out[i] = (char) '\0';
        prompt_err[i] = (char) '\0';
    };
    prompt_pos = 0;
}

// Create n files and put the pointer into the file table.
static void __initialize_file_table(void)
{
// #todo
// We need a structure to track te information
// about the file table. file_table[],
// maybe FileTableInfo.xxxx.

    register int i=0;
    file *tmp;

    FileTableInfo.initialized = FALSE;
    FileTableInfo.size = (int) NUMBER_OF_FILES;

    for (i=0; i<NUMBER_OF_FILES; i++)
    {
        tmp = (void*) kmalloc(sizeof(file));
        if ((void*)tmp == NULL){
           x_panic("__initialize_file_table: tmp\n");
        }
        memset( tmp, 0, sizeof(struct file_d) );

        tmp->____object = ObjectTypeFile;  // Regular file.
        tmp->_flags = 0;                   // (__SWR | __SRD); 
        tmp->fd_counter = 0;
        tmp->_tmpfname = NULL;
        //...
        tmp->used = TRUE;
        tmp->magic = 1234;
        file_table[i] = (unsigned long) tmp; 
    };

    FileTableInfo.initialized = TRUE;
}

// Create n inodes and put the pointers
// into the inode table.
static void __initialize_inode_table(void)
{
    register int i=0;
    struct inode_d *tmp_inode;

    for (i=0; i<32; i++)
    {
        tmp_inode = (void*) kmalloc(sizeof(struct inode_d));
        if ((void*)tmp_inode == NULL){
            x_panic("__initialize_inode_table: tmp_inode\n");
        }
        memset( tmp_inode, 0, sizeof(struct inode_d) );

        tmp_inode->filestruct_counter = 0;
        tmp_inode->path[0] = 0;
        //...
        tmp_inode->used = TRUE;
        tmp_inode->magic = 1234;
        inode_table[i] = (unsigned long) tmp_inode; 
    };
}

// Initialize virtual consoles.
// We already made a early initialization.
// We will reinitialize only if the console was
// not initialized before. In the case of failure 
// of the early initialization routine.
static void __initialize_virtual_consoles(void)
{
    register int i=0;

    PROGRESS("__initialize_virtual_consoles: (second time)\n");

// stdout:
// At this moment we need a valid stdout structure.

    if ((void*) stdout == NULL)
        x_panic("__initialize_virtual_consoles: No stdout");
    if (stdout->magic != 1234)
        x_panic("__initialize_virtual_consoles: Invalid stdout");

//
// Setup colors.
//

    unsigned int bg_colors[CONSOLETTYS_COUNT_MAX];
    unsigned int fg_colors[CONSOLETTYS_COUNT_MAX];

// Default kernel console.
    bg_colors[0] = (unsigned int) COLOR_BLUE;
    fg_colors[0] = (unsigned int) COLOR_WHITE;
// Auxiliary kernel console.
    bg_colors[1] = (unsigned int) COLOR_BLUE;
    fg_colors[1] = (unsigned int) COLOR_YELLOW;
// Warning console.
    bg_colors[2] = (unsigned int) COLOR_ORANGE;
    fg_colors[2] = (unsigned int) COLOR_WHITE;
// Danger console.
    bg_colors[3] = (unsigned int) COLOR_RED;
    fg_colors[3] = (unsigned int) COLOR_YELLOW;

//
// Initialize
// 

// #bugbug
// We have the same loop in console.c

    // #debug
    //PROGRESS("kstdio.c:\n");;

    for (i=0; i<CONSOLETTYS_COUNT_MAX; i++)
    {
        if (CONSOLE_TTYS[i].initialized == TRUE){
            debug_print("__initialize_virtual_consoles: Console already initialized\n");
        }

        // Make the standard initialization.
        if (CONSOLE_TTYS[i].initialized == FALSE)
        {
            // IN: console index, bg color, fg color
            DDINIT_console(
                i,
                bg_colors[i],
                fg_colors[i] );

            if (i==0)
            {
                if ((void*) console0_tty != NULL)
                {
                    if (console0_tty->magic != 1234){
                        x_panic("__initialize_virtual_consoles: No console0_tty");
                    }
                    console0_tty->fp = (file *) stdout;
                    console0_tty->next = NULL;
                }
            }

            if (i==1)
            {
                if ((void*) console1_tty != NULL)
                {
                    if (console1_tty->magic != 1234){
                        x_panic("__initialize_virtual_consoles: No console1_tty");
                    }
                    console1_tty->fp = (file *) stdout;
                    console1_tty->next = NULL;
                }
            }

            if (i==2)
            {
                if ((void*) console2_tty != NULL)
                {
                    if (console2_tty->magic != 1234){
                        x_panic("__initialize_virtual_consoles: No console2_tty");
                    }
                    console2_tty->fp = (file *) stdout;
                    console2_tty->next = NULL;
                }
            }

            if (i==3)
            {
                if ((void*) console3_tty != NULL)
                {
                    if (console3_tty->magic != 1234){
                        x_panic("__initialize_virtual_consoles: No console3_tty");
                    }
                    console3_tty->fp = (file *) stdout;
                    console0_tty->next = NULL;
                }
            }
        }
    };

// The foreground console.
    jobcontrol_switch_console(DEFAULT_CONSOLE);

// #test
    //set_up_cursor(0,1);
    //console_outbyte('x',fg_console);
    //refresh_screen();
    //while(1){}
}

/*
 * kstdio_initialize:
 *     Inicializando stdio pertencente ao kernel base.
 *     Inicializa as estruturas do fluxo padrão.
 *     Quem chamou essa inicialização ?? Em que hora ??
 * #bugbug: Pelo jeito somente depois dessa inicialização é que temos mensagens 
 * com printk decentes. Então a inicialização do kernel precisa disso.
 * >> precisamos antecipar essa inicilização. Mas ela precisa ser depois da
 * inicialização da paginação.
 */
// Estamos no kernel base em ring 0.
// Queremos que as streams sejam acessíveis para as rotinas
// da libc em ring3. Para a libc alterar os elementos
// da estrutura.
// #bugbug: Talvez seja possível criar essas estruturas
// em memória compartilhada, usado o alocaro apropriado.
// kmalloc com certeza e ring0.
// In this routine:
// + Initializing the structures for stdin, stdout and stderr

int kstdio_initialize(void)
{
    kstdio_standard_streams_initialized = FALSE;

// Ja temos suporte a print nesse momento por causa
// das configurações de console. Mas nessa rotina 
// refaremos as configurações de console.

    debug_print("kstdio_initialize: [TODO]\n");
    //printk   ("kstdio_initialize: [TODO]\n");

// ??
// Input mode
// #bugbug
// We have another definition of input mode.
// An global io structure.

    g_inputmode = INPUT_MODE_MULTIPLE_LINES;

    stdio_terminalmode_flag = TRUE;
    stdio_verbosemode_flag = TRUE;

// Last registered error.
    errno = 0;

// Buffers used by the standard stream.
// Initialize the global file table.
// Initialize the global inode table.
    __clear_prompt_buffers();
    __initialize_file_table();
    __initialize_inode_table();

// Initializing the file pointers
// stdio, stdout and stderr.
// See: kstdio.h

// Create standard stream.
    __initialize_stdin();
    __initialize_stdout();
    __initialize_stderr();

// Virtual consoles.
    __initialize_virtual_consoles();

    //PROGRESS("BREAKPOINT\n");
    //while(1){}

// Background (Second time).
    displayInitializeBackground(
        COLOR_KERNEL_BACKGROUND,
        TRUE );

// Done
    kstdio_standard_streams_initialized = TRUE;
    return TRUE;
//fail:
    //return FALSE;
}


