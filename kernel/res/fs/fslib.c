// fslib.c
// File system support.
// The workers and the real implementation for the services
// called by fs.c.
// Created by Fred Nora.

#include <kernel.h>

// Buffers for 8 directories. Each one with 512 entries.
// 32*512 = 16384
// 16KB
// 16384/4096 = 4 pages.
// ------------------
// with 8 pages we have 8 directories for 1024 entries each dir.

//unsigned long fs_buffers[8];
unsigned long fs_buffers[FS_N_BUFFERS];

//see: fs.h
struct filesystem_d  *root;

// gcc -Wall Defined but not used!
static char *____root_name = "/";

struct cwd_d  CWD;

//char search_path[?];
unsigned long search_path_dir_address=0;
unsigned long search_path_dir_entries=0;


// ========================================

//
// == Private functions: Prototypes ===========
//

static int __initialize_fs_buffers(void);
// ...

//=============================


// Local
// The buffers used to load the directories
// when loading a path.
// Buffers
// Buffers for loading the directories while walking on a pathname
// when loading a file.
// 0=ok | <0 = fail.
static int __initialize_fs_buffers(void)
{
    register int i=0;
// The max number of levels in a path.
    size_t Max = FS_N_BUFFERS;
// 512 entries = 16KB.
// 32*512 = 16KB.
    const int PagesPerBuffer = 4;
    unsigned long TmpAddr=0;

// #bugbug
// 4 pages per level.
    for (i=0; i<Max; i++){
        TmpAddr = (unsigned long) allocPages(PagesPerBuffer);
        if ((void*) TmpAddr == NULL){
            goto fail;
        }
        fs_buffers[i] = (unsigned long) TmpAddr;
    };

    return 0;
fail:
    return (int) -1;
}

// File read.
// It's called by sys_read.
// Copia à partir do início do arquivo.
// OUT: number of read bytes.
int file_read_buffer( file *f, char *buffer, int len )
{
// Limits:
// We're reading from the file structure,
// The buffer in the file structure is BUFSIZ (1024 for now)
// at minimum.

    char *p;
    int Count=0;

    p = buffer;
// #test
    Count = (int) (len & 0xFFFF);

// Check file
    if ((void *) f == NULL){
        printk ("file_read_buffer: f\n");
        goto fail;
    }
    if ( f->used != TRUE || f->magic != 1234 ){
        printk ("file_read_buffer: f validation\n");
        goto fail;
    }
// Check buffer
    if ((void *) p == NULL){
        printk ("file_read_buffer: p\n");
        goto fail;
    }
// nada para ler.
    if (Count <= 0){
        printk ("file_read_buffer: Count <= 0\n");
        goto fail;
    }
// Chech len
// #bugbug: Isso é provisório
// A quantidade que desejamos ler é menor que o tamanho do buffer.
// Estamos lendo do início do arquivo?
    if (Count > f->_lbfsize){
        printk ("file_read_buffer: Count > f->_lbfsize\n");
        goto fail;
    }

    //if ( Count > f->_fsize ){
        //printk ("file_read_buffer: Count > f->_fsize\n");
    //    Count = f->_fsize;
    //    goto fail;
    //}

/*
// stdin
    if( f->_file == 0 )
    {
        if( f->_lbfsize != PROMPT_SIZE)
        {
           printk ("file_read_buffer: [FAIL] Wrong size for stdin _lbfsize\n");
           goto fail;
        }
    }
 */

//=======================================
// Copy!
// #todo
// nao podemos ler mais que o limite do arquivo.
// A próxima leitura precisa ser depois dessa.

// =================================
// Socket:
// Se o arquivo é um socket, 
// então não concatenaremos escrita ou leitura.
// You also can write now.
// But i can still read.
    if (f->____object == ObjectTypeSocket){
        memcpy ( (void *) buffer, (const void *) f->_base, Count );
        f->_flags |= __SWR;
        return (int) Count;
    }

// =================================
// Pipe:
// Não concatenaremos
// You also can write now.
// But i can still read.
    if (f->____object == ObjectTypePipe){
        memcpy ( (void *) buffer, (const void *) f->_base, Count );
        f->_flags |= __SWR;
        return (int) Count;
    }

// =================================
// Regular file, tty, iobuffer.
// Nesse caso a leitura tem que respeitar os offsets e limites.
// #todo:
// We can create an IF for each one of these types.
    if ( f->____object == ObjectTypeFile ||
         f->____object == ObjectTypeTTY  ||
         f->____object == ObjectTypeIoBuffer)
    {
        // Se o buffer tem tamanho 0.
        if (f->_lbfsize <= 0){
            printk ("file_read_buffer: _lbfsize is 0\n");
            goto fail;
        }

        // Se o tamanho do buffer for maior que o padrão.
        // #todo: O buffer pdoerá ser maior que isso no futuro.
        //if ( f->_lbfsize > BUFSIZ ){
        //    printk ("file_read_buffer: _lbfsize\n");
        //    goto fail;
        //}

        // #test: Limite provisorio
        if (f->_lbfsize > (8*1024)){
            printk ("file_read_buffer: _lbfsize bigger than 8KB\n");
            goto fail;
        }

        // Não podemos ler antes do início do arquivo.
        if ( f->_r < 0 ){
            f->_r = 0;
            printk ("file_read_buffer: f->_r = 0\n");
            goto fail;
        }

        // Nao leremos depois do fim do arquivo.
        if ( f->_r > f->_lbfsize )
        {
            //#debug: provisorio
            printk ("file_read_buffer: f->_r > f->_lbfsize\n");
            goto fail;
            //debug_print("file_read_buffer: f->_r > f->_lbfsize\n");
            //f->_r = f->_lbfsize;
            //f->_w = f->_lbfsize;
            //f->_p = (f->_base + f->_lbfsize);
            //f->_cnt = 0;
            //return EOF;
        }

        // Se o offset de leitura for maior que
        // o offset de escrita, então temos que esperar.
        // #bugbug: mas talvez isso não seja assim para pipe.
        if (f->_r > f->_w)
        {
            // EOF
            printk ("file_read_buffer: f->_r > f->_w\n");
            //goto fail;
            //debug_print("file_read_buffer: f->_r > f->_w\n");
            f->_r = f->_w;
            //faremos o ajuste logo abaixo.
            //f->_p = (f->_base + f->_r);
            // You also can write now.
            // But i can still read.
            //f->_flags = __SWR;
            return 0;
        }

        // Se a quantidade que desejamos ler
        // é maior que o espaço que temos.
        // # Isso ja foi feito logo acima.
        if (Count > f->_lbfsize)
        {
            //printk ("file_read_buffer: Count > f->_lbfsize\n");
            //goto fail;

            //printk ("file_read_buffer: [FAIL] local_len limits\n");
            //goto fail;
        
            //#test #bugbug
            // leia tudo então. hahaha
            Count = (f->_lbfsize - 1);
        }

        // Se o tanto que queremos ler é maior
        // que o que nos resta da buffer,
        // então vamos ler apenas o resto do buffer.

        // #bugbug: Isso esta errado. #delete

        // So podemos ler ate limite de bytes disponíveis 
        // no buffer.
        //if (Count > f->_cnt)
        //{
            //printk ("file_read_buffer: local_len > f->_cnt\n");
            //goto fail;
            //Count = f->_cnt;
        //}
 
        // 
        int delta = (f->_w - f->_r);

        // nada para ler.
        // pois o ponteiro de escrita e o de leitura sao iguais,
        if (delta == 0){
            // 0 bytes lidos
            //printk ("delta=0\n");
            return 0;
        }
 
        // #delta
        // Se o tanto que queremos ler
        // é maior que o tanto que foi efetivamente escrito,
        // então leremos somente o que foi escrito.
        // se a diferença entra o ponteiro de escrita e o ponteiro
        // de leitura for menor que a quantidade que queremos ler.
        // Se queremos ler mais do que foi escrito.
        // entao vamos ler apenas o que foi escrito.
        if (Count > delta){
            Count = delta;
        }

        // Atualizando o ponteiro de trabalho.
        // Vamos ler daqui.
        // A partir do offset de leitura.
        f->_p = (f->_base + f->_r);

        // read

        //#debug
        if (Count <= 0){
            printk ("file_read_buffer: Count <= 0 SECOND\n");
            goto fail;
            //printk("local_len\n");
            //refresh_screen();
            //return -1;
        }

        // Copy from the address pointed by the file structure
        // to a given ring3 or ring0 buffer.
        // The file structure has the limit of BUFSIZ.
        memcpy (
            (void *) buffer, 
            (const void *) f->_p, 
            Count ); 

        // Atualizamos o ponteiro de trabalho
        f->_p = (f->_p + Count);

        // Atualizamos o offset de leitura.
        // Ele é usado em relação à base.
        // #bugbug: Talvez essa leitura e escrita devesse ser em relação
        // ao ponteiro de trabalho.
        f->_r = (f->_r + Count);

        if ( f->_r > f->_w ){
            f->_r = f->_w;
        }

        // You also can write now.
        // But i can still read.
        f->_flags |= __SWR;
        f->sync.can_write = TRUE;

        return (int) Count;
    }

fail:
    //refresh_screen();
    return EOF;
}

/*
 * file_write_buffer: 
 * Escreve no arquivo uma certa quantidade 
 * de caracteres de uma dada string.
 */
// Escreve no arquivo uma certa quantidade de caracteres 
// de uma dada string.
// It's called by sys_write.
// OUT: number of written bytes.
int 
file_write_buffer ( 
    file *f, 
    char *string, 
    int len )
{
    char *p;
    p = string;

    // debug_print ("file_write_buffer:\n");

// File validation.
    if ((void *) f == NULL){
        printk ("file_write_buffer: f\n");
        goto fail;
    }
    // #todo: Check used and magic.
    //if (f->magic != 1234)
        //return -1;

    if ((void *) p == NULL){
        printk ("file_write_buffer: p\n");
        goto fail;
    }

// Tentando escrever mais do que cabe no arquivo.
    if (len >= BUFSIZ){
        printk ("file_write_buffer: len >= BUFSIZ\n");
        goto fail;
    }

//
// Copy
//

// Socket file.
// Se o arquivo é um socket, então não concatenaremos 
// escrita ou leitura.
    if (f->____object == ObjectTypeSocket){
        memcpy( (void *) f->_base, (const void *) string, len );
        return (int) len;
    }

// Pipe file.
// não concatenaremos
    if (f->____object == ObjectTypePipe){
        memcpy( (void *) f->_base, (const void *) string, len );
        return (int) len;
    }

// Regular file, TTY file, IO Buffer file.
// Tem que atualizar o ponteiro para que o próximo
// write seja depois desse write.
// Para isso o ponteiro precisa estar no base quando
// o write for usado pela primeira vez.
// Mas se o write for usado num arquivo aberto com 
// open(), então o ponteiro deve estar no fim do arquivo.
// Isso não significa que está no fim do buffer.
// Nesse caso o aplicative deveria usar rewind() antes
// para ter um ponteiro de escrita adequado.

    if ( f->____object == ObjectTypeFile ||     // regular file
         f->____object == ObjectTypeTTY  ||     // tty
         f->____object == ObjectTypeIoBuffer )  // ??
    {
        // #bugbug
        // Temos que ter um limite aqui ... !!!
        // #todo

        // se o tamanho do buffer for maior que o padrao.
        if (f->_lbfsize > BUFSIZ){
            printk ("file_write_buffer: _lbfsize\n");
            goto fail;
        }
    
        if (f->_w < 0)
        {
            f->_p = f->_base;
            f->_w = 0;
            f->_r = 0;
            f->_cnt = f->_lbfsize;
            return EOF;
        }

        // Se o offset de escrita ultrapassa os limites.
        if (f->_w >= BUFSIZ)
        {
            //#bugbug
            debug_print("file_write_buffer: f->_w >= BUFSIZ\n");
            printk     ("file_write_buffer: f->_w >= BUFSIZ\n");
            f->_w = BUFSIZ;
            f->_cnt = 0;
            return EOF;
        }

        //if ( f->_w 

        // recalculando quanto espaço temos.
        //f->_cnt = (f->_lbfsize - f->_w);

        // Se a quantidade que temos ultrapassa os limites.

        // fim do arquivo.
        if (f->_cnt < 0)
        {
            f->_cnt = 0;
            f->_w = f->_lbfsize;
            f->_r = f->_lbfsize;
            f->_p = (f->_base + f->_w);
            return EOF;
        }

        // Inicio do arquivo
        if (f->_cnt > f->_lbfsize)
        {
            printk ("file_write_buffer: _cnt\n");
            f->_cnt = f->_lbfsize;
            f->_p = f->_base;
            f->_w = 0;
            f->_r = 0;
        }

        if (len < 0){
            return -1;
        }

        // Se o que desejamos escrever é maior que o espaço que temos.
        if (len > f->_cnt)
        {
            // Estamos no fim do arquivo
            if ( f->_cnt <= 0 )
            {
                f->_w = f->_lbfsize;
                f->_r = f->_lbfsize;
                f->_p = f->_base + f->_lbfsize;
                f->_cnt = 0;
                return -1;
            }

            // Só podemos escrever esse tanto.
            if ( f->_cnt > 0 ){
                len = f->_cnt; 
            }
        }

        // write.
        // Se o offset de escrita é menor que o offset de leitura.
        // Então adiantaremos o offset de escrita.
        // pois nao devemos tocar no offset de leitura.
        if (f->_w < f->_r)
        {
            f->_w = f->_r;
            f->_cnt = ( f->_lbfsize - f->_w );
        }

        // Vamos escrever aqui.
        f->_p = (f->_base + f->_w);

        // Escrevemos usando o ponteiro de escrita.
        memcpy( (void *) f->_p, (const void *) string, len ); 
    
        // Atualizamos o ponteiro de escrita
        f->_p = (f->_p + len);

        // Atualizamos o offset de escrita.
        f->_w = (f->_w + len);
    
        // Atualizamos o quanto nos falta.
        f->_cnt = (f->_cnt - len);
        
        // You can read now.
        f->_flags = __SRD;
        f->sync.can_read = TRUE;

        //debug_print ("file_write_buffer: ok, done\n");

        // Retornamos a quantidade escrita no buffer.
        return (int) len;
    }

// Unknown type
fail:
    //printk ("file_write_buffer: fail\n");
    //refresh_screen();
    return EOF;
}

// __read_imp
// It implements the service for read() syscall.
// ring0 read() implementation.
// Service 18.
// #todo: There's a lot to do with synchronization.
// #todo
// We need to call one subfunctions for
// each different kind of file intead of implementing
// an internal sub-routine.
// #todo
// Isso deveria ser apenas um wrapper,
// chamando as rotinas apropriadas para cada tipo 
// de arquivo.
// OUT:
// 0 = Couldn't read.
// -1 = Error.
// return 'ssize_t'?

ssize_t 
__read_imp (
    int fd,
    char *ubuf,       //#todo: use 'void *'
    size_t count )
{
// Limits:
// #test
// Probably we can extand the limit to BUFSIZ,
// Because this is the limit used by the sys_open().
// #test: 
// So, let's try to accept a user buffer 
// with the limit up to BUFSIZ.

// #: It needs to be used only as a worker for syscalls.
// #todo
// Get the processor pointer
// and increment the read operation counter for this process.

// File pointer.
    file *fp;

    ssize_t nbytes=0;
    struct socket_d *s;
    //#bugbug: We don't know the size of the user buffer.
    //size_t ubuf_len=0;  // # Out limit is BUFSIZ

//-----------------------------
    struct process_d *p;
    pid_t current_process = (pid_t) get_current_process();
// Process
// #todo: There is a helper for that small routine.
    if (current_process < 0 || current_process >= PROCESS_COUNT_MAX){
        debug_print("__read_imp: current_process\n");
        goto fail;
    }
    p = (void *) processList[current_process];
    if ((void *) p == NULL){
        debug_print("__read_imp: p\n");
        goto fail;
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        debug_print("__read_imp: p validation\n");
        goto fail;
    }
// #warning
// The goal here is counting how many timer the operation was called.
    p->read_counter++;
//-----------------------------

// #bugbug
// O argumento é 'unsigned int'.
// Deveria ser int?

// fd
    if ( fd < 0 || fd >= OPEN_MAX ){
        return (ssize_t) (-EBADF);
    }
// ubuf
// todo: Checar a validade da região de memória.
    if ((void *) ubuf == NULL){
        return (ssize_t) (-EINVAL);
    }
// count
    if (count < 0){ 
        return (ssize_t) (-EINVAL);
    }
// Nothing to do.
    if (count == 0){ 
        return 0; 
    }

// Size of the buffer.
// #test: Our limit is BUFSIZ (1024) for now.
    //ubuf_len = strlen((const char *) ubuf);
    //ubuf_len = sizeof((const char *) ubuf);

// The user wants to read more than the size of the 
// buffer provide by him.
// Se a quantidade desejada é maior que o buffer disponível.
    //if (count > ubuf_len)
    //{
        //serial_printk ("__read_imp: count=%d ubuf_len=%d\n",
        //count, ubuf_len );
        // #test
        //panic("__read_imp: ubuf is not enough");
    //}

// Se o buffer disponível é maior que a quantidade desejada
// então não temos problemas, nem precisamos disso. 
    //if (ubuf_len > count){
        //ubuf_len = count;
    //}

// #fixme: 
// The buffer is too small.
// #todo
// We want a bigger limit for reading,
// we will read a socke with this routine,
// use the stadard size for a socket at least.
// See: SO_SNDBUF and SO_RCVBUF
// #ps:
// Actually this routine can be able to read files
// bigger than a socket.

    //if (ubuf_len > 512){
        //debug_print ("__read_imp: ubuf_len\n");
        //ubuf_len = 512;
    //}

// File
// Get the object pointer.
// #warning: For the current process.
    fp = (file *) get_file_from_fd(fd);
    if ((void *) fp == NULL)
    {
        debug_print("__read_imp: fp\n");
        printk     ("__read_imp: fp\n");
        goto fail; 
    }

    if (fp->sync.can_read != TRUE){
        debug_print("__read_imp: Permission\n");
        printk     ("__read_imp: Permission\n");
        goto fail; 
    }

/*
    // #todo
    // ainda nao inicializamos esse elemento.
    if( fp->is_readable == FALSE ){
        debug_print ("__read_imp: Not readable\n");
        return -1;
    }
*/

    // #todo: Create thie element in the structure.
    // if( fp->is_directory == TRUE ){}

//==========================================================
// ::0
// stdin
    if (fp->_file == STDIN_FILENO){
        if (fp->____object == ObjectTypeFile){
            goto RegularFile;
        }
    }

//==========================================================
// ::1
// stdout
    if (fp->_file == STDOUT_FILENO){
        if (fp->____object == ObjectTypeFile){
            goto RegularFile;
        }
        /*
        // Not tested yet.
        // If the file is a console.
        if (fp->____object == ObjectTypeVirtualConsole)
        {
            return (int) console_read ( 
                            (int) fg_console, 
                            (const void *) ubuf, 
                            (size_t) count );
        }
        */
    }

//==========================================================
// ::2
// stderr
    if (fp->_file == STDERR_FILENO){
        if (fp->____object == ObjectTypeFile){
            goto RegularFile;
        }
    }

//
// == Socket file ===========================================
//

// ==== Socket =======================
// Se o arquivo for um socket.
// #todo: 
// Now we can call socket_read(...)

    if (fp->____object == ObjectTypeSocket)
    {
        // debug_print("__read_imp: [DEBUG] Trying to read a socket object\n");
        do_credits_by_tid(current_thread);

        // not reading yet
        if ((fp->_flags & __SRD) == 0) 
        {
            //debug_print("__read_imp: [FAIL] flag __SRD \n");
            //yield (current_thread);
            goto fail;
        }

        nbytes = 0;

        // vazio? 
        // nao podemos ler.
        // acorda escritores e dorme.
        if (fp->socket_buffer_full == FALSE)
        { 
            debug_print("__read_imp: [FAIL] can't read an empty buffer\n");
            //goto fail;
            
            // #debug
            // debug_print("__read_imp: WAKEUP WRITER\n");
            
            fp->_flags = 0;
            fp->_flags |= __SWR;                  // pode escrever
            //do_thread_ready( fp->tid_waiting );   // acorda escritores. 
            fp->tid_waiting = -1;

            // #bugbug
            // Isso pode ser ruim pela natureza da chamada sys_read()
            // que vem de uma syscall que nao salvou o contexto.

            if (fp->sync.block_on_read_empty == TRUE )
            {
                debug_print("__read_imp: SLEEP READER\n");
                panic("__read_imp: [DEBUG] Couldn't read socket. Buffer not full\n");

                fp->tid_waiting = current_thread;
                //do_thread_waiting (current_thread);
                yield (current_thread);
                goto fail;
            }
            yield (current_thread);
            goto fail;
        }

        // cheio? 
        // le e acorda escritores.
        if (fp->socket_buffer_full == TRUE)
        {
            if (fp->_flags & __SRD)
            {
                //debug_print ("__read_imp: >>>> READ\n");
            
                // Read from the file structure to the 
                // user buffer. The limit in the file structure
                // is BUFSIZ (1024 for now)
                nbytes = 
                    (ssize_t) file_read_buffer ( 
                                  (file *) fp, 
                                  (char *) ubuf, 
                                  (int) count );

                if (nbytes <= 0){
                    debug_print("__read_imp: [FAIL] file_read_buffer fail when reading a socket \n");
                    //yield (current_thread);
                    goto fail;
                }

                // ok
                if (nbytes > 0)
                {
                    //debug_print("__read_imp: [DEBUG] lemos mais que 0 bytes em um socket.\n");
                    fp->socket_buffer_full = FALSE;     // buffer vazio
                    fp->_flags &= ~__SRD;  // nao posso mais LER.            
                    fp->_flags |= __SWR;   // pode escrever também
                    //debug_print("__read_imp: WAKEUP WRITER\n");
                    //do_thread_ready( fp->tid_waiting );  // acorda escritores.
                    fp->tid_waiting = -1;
                    //debug_print("__read_imp:done\n");

                    return (ssize_t) nbytes;    // bytes escritos.
                }
            }
        } 

        panic ("__read_imp: Unexpected error when reading socket\n");
    }

//
// == Regular file ============================================
//

// ==== Regular file =======================
// Read a regular file.
// See: unistd.c
// #todo Tem que retornar a quantidade de bytes lido.
// #bugbug: Nem todos arquivos criados possuem essa flag configurada
// apropriadamente ...estamos fazendo isso.
// #hackhack
// We will open all the other kind of object
// with this IF right here.
// We need to fix it in the open() routines
// to give us the correct type.

RegularFile:

    if (fp->____object == ObjectTypeFile)
    {

         //#debug
         //if( fp->_file == 5)
         //{
         //    if ( (fp->_flags & __SRD) == 0 )
         //        printk("cant read\n");
         //    if ( fp->_flags & __SRD )
         //        printk("can read\n");
         //    printk("__read_imp-OUTPUT 5: %s \n",fp->_base);
         //    refresh_screen();
         //}

        //debug_print("__read_imp: [DEBUG] Trying to read a regular file object\n");
        
        //Se não pode ler.
        if ( (fp->_flags & __SRD) == 0 )
        {
            //debug_print("__read_imp: [FLAGS] Can't read!\n");
        
            //Não conseguimos ler.
            //nada de errado, apenas espera.
            //do_thread_waiting (current_thread);
            //fp->tid_waiting = current_thread;
            //fp->_flags |= __SWR;  //pode escrever.
            //scheduler();
            
            //printk("__read_imp: fp->_flags & __SRD \n");
            //refresh_screen();
            
            goto fail;
        }
        
        nbytes = 0;

        // Se puder ler:
        // + Call a function to read a regular file.
        // + Sinalize that another process can write.
        // #todo: wake the one that was waiting to write.

        if (fp->_flags & __SRD)
        {

            // Read from the file structure to the 
            // user buffer. The limit in the file structure
            // is BUFSIZ (1024 for now)

            nbytes = 
                (ssize_t) file_read_buffer ( 
                              (file *) fp, 
                              (char *) ubuf, 
                              (int) count );
 
            if (nbytes <= 0){
               //yield (current_thread);
               goto fail;
            }

            // Se conseguimos ler.
            if (nbytes>0)
            {
                //fp->_flags &= ~__SRD;  // nao posso mais LER.            
                //fp->_flags |= __SWR;   // pode escrever também
                // ok to write.
                fp->_flags |= __SWR;
                fp->sync.can_write = TRUE;
                // #test
                // Acordar quem esperava por esse evento
                //do_thread_ready( fp->tid_waiting );
                return (ssize_t) nbytes; 
            }

            //Não conseguimos ler.
            //nada de errado, apenas espera.
            //do_thread_waiting (current_thread);
            //fp->tid_waiting = current_thread;
            //fp->_flags |= __SWR;  //pode escrever.
            //scheduler();
            return 0;
        }

        // #todo
        // Do we need something here ?
        //return -1;
    }

// ========================================
// Pseudo terminal multiplexer.

    if (fp->____object == ObjectTypePTMX){
        printk ("__read_imp: [TODO] trying to read a PTMX device file\n");
        //refresh_screen();
        return 0;
    }

// ========================================
// pty () pseudo terminal.

    if (fp->____object == ObjectTypePTY){
        printk ("__read_imp: [TODO] trying to read a PTY device file\n");
        //refresh_screen();
        return 0;
    }

// ========================================
// file system

    if (fp->____object == ObjectTypeFileSystem){
        printk ("__read_imp: [TODO] trying to read a file system\n");
        //refresh_screen();
        return 0;
    }

// ========================================
// See: pipe.c
    //#bugbug: Do not use sys_xxxx use the worker, or another wrapper.
    if (fp->____object == ObjectTypePipe){
        return (ssize_t) sys_read_pipe ( (int) fd, (char *) ubuf, (int) count ); 
    }

// ======================================================
    // ...
    debug_print ("__read_imp: [FAIL] Unknown object type\n");
// ==========================================

//
// Fail
//
    
// #bugbug
// See manual for the error:
// https://www.gnu.org/software/libc/manual/html_node/I_002fO-Primitives.html
// http://man7.org/linux/man-pages/man2/read.2.html

// A intenção é que um erro seja representado por -1 e não por 0.
// Zero representará fim de arquivo. 

fail:

    // #debug
    // debug_print ("__read_imp: [FAIL] something is wrong!\n");

// #bugbug: 
// Isso pode degradar o desempenho.

    // printk ("__read_imp: [FAIL] something is wrong!\n");
    //refresh_screen();  
    //bloqueando, autorizando a escrita e reescalonando.
    //do_thread_waiting (current_thread);
    //fp->tid_waiting = current_thread;
    //fp->_flags |= __SWR;  //pode escrever      
    //scheduler();  //#bugbug: Isso é um teste  

// Something is wrong!
    return (ssize_t) (-1);
}


// __write_imp
// It implements the service for write() syscall.
// Implemantation of write() for libc.
// service 19.
// Copiar um buffer para um arquivo dado o descritor.
// Aqui devemos selecionar o dispositivo à escrever.
// See:
// https://github.com/zavg/linux-0.01/blob/master/fs/read_write.c
// https://linux.die.net/man/2/write
// ...
// #todo
// We need to call one subfunction for each different kind of file 
// intead of implementing an internal sub-routine.

// #todo
// Isso deveria ser apenas um wrapper,
// chamando as rotinas apropriadas para cada tipo 
// de arquivo.

// #todo
// Copiar deve ser uma opcao e nao uma regra!
// Precisamos de uma flag para copyonwrite.
// atençao
// Podemos escrever num socket que nao eh o privado!!!
// existe a possibilidade de o processo atual estar 
// escrevendo em um socket que pertence a outro processo.
// mas que por causa da conexao possui o fd dele registrado
// em sua estrutura.
// Entao o fd passado por argumento nao precisa ser o mesmo
// do socket privado do processo. Pois poderemos estar
// escrevendo em outro socket que nao o privado.

// OUT:
// 0 = Couldn't read.
// -1 = Error.

ssize_t __write_imp (int fd, char *ubuf, size_t count)
{
// #: It needs to be used only as a worker for syscalls.
// #todo
// Get the processor pointer
// and increment the write operation counter for this process.


// File pointer.
    file *fp;

    ssize_t nbytes=0;
    struct socket_d *s1;
    struct socket_d *s2;
    //int ubuf_len=0;  //#bugbug: We don't know the size of the user buffer.
    size_t ncopy=0;

    //debug_print("__write_imp: :)\n");

//-----------------------------
    struct process_d *p;
    pid_t current_process = (pid_t) get_current_process();
// Process
// #todo: There is a helper for that small routine.
    if (current_process < 0 || current_process >= PROCESS_COUNT_MAX){
        debug_print("__write_imp: current_process\n");
        goto fail;
    }
    p = (void *) processList[current_process];
    if ((void *) p == NULL){
        debug_print("__write_imp: p\n");
        goto fail;
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        debug_print("__write_imp: p validation\n");
        goto fail;
    }
// #warning
// The goal here is counting how many timer the operation was called.
    p->write_counter++;
//-----------------------------


// fd
    if ( fd < 0 || fd >= OPEN_MAX ){
        return (ssize_t) (-EBADF);
    }
// ubuf
    if ((void *) ubuf == NULL){
        return (ssize_t) (-EINVAL);
    }
// count
    if (count < 0){ 
        return (ssize_t) (-EINVAL);
    }
// Nothing to do.
    if (count == 0){ 
        return 0; 
    }

//
// Size of the buffer.
//

// len
    //ubuf_len = strlen((const char *) ubuf);

// Se a quantidade desejada é maior que o buffer disponível.
    //if (count > ubuf_len){
        //debug_print("__write_imp: [FIXME] count too long!\n");
    //}

// se o buffer é maior que a quantidade desejada, 
// então não temos problemas
    //if (ubuf_len > count ){
        //ubuf_len = count;
        //debug_print ("__write_imp: [FIXME] Ajusting ubuf_len\n");
    //}

// #debug: 
// limits
// Um socket tem o tamanho de BUFSIZ.
    //if (ubuf_len > 512){
        //ubuf_len = 512;
        //debug_print ("__write_imp: [FIXME] Ajusting ubuf_len to 512\n");
    //}

// File
// Get the object pointer from the list
// in the process structure.
// #warning: For the current process.
    fp = (file *) get_file_from_fd(fd);
    if ((void *) fp == NULL)
    {
        debug_print("__write_imp: fp not open\n");
        printk     ("__write_imp: fp not open #hang\n");
        //printk      ("fd{%d} pid{%d}\n",fd,current_process);
        //printk("entry0: %x\n", __P->Objects[0]);
        //printk("entry1: %x\n", __P->Objects[1]);
        //printk("entry2: %x\n", __P->Objects[2]);
        //printk("entry3: %x\n", __P->Objects[3]);
        //printk("entry4: %x\n", __P->Objects[4]);
        //refresh_screen();
        //while(1){}
        goto fail;
    }

    if (fp->sync.can_write != TRUE){
        debug_print("__write_imp: [PERMISSION] Can NOT write the file\n");
        printk     ("__write_imp: [PERMISSION] Can NOT write the file\n");
        goto fail; 
    }

/*
// #todo
// ainda nao inicializamos esse elemento.
    if( fp->is_writable == FALSE ){
        debug_print ("__write_imp: Not writable\n");
        return -1;
    }
*/

// #todo: 
// Create thie element in the structure.
    //if( fp->is_directory == TRUE ){}

// =======================================================
// ::0
// stdin
// + Write on regular file.
    if (fp->_file == STDIN_FILENO)
    {
        if (fp->____object == ObjectTypeFile){
            goto RegularFile;
        }
        goto fail;
    }

// =======================================================
// ::1
// stdout
// + Write on console.
// + Write on regular file.
// see: console.c
    if (fp->_file == STDOUT_FILENO)
    {
        // If the file is a regular file.
        if (fp->____object == ObjectTypeFile){
            goto RegularFile;
        }
        // If the file is a console.
        if (fp->____object == ObjectTypeVirtualConsole)
        {
            if (Initialization.printk_to_serial == TRUE){
                return (int) debug_print_nbytes( 
                    (const void *) ubuf, 
                    (size_t) count );
            } else {
                return (int) console_write ( 
                    (int) fg_console, 
                    (const void *) ubuf, 
                    (size_t) count );
            }
        }

        // ...
        goto fail;
    }

// =======================================================
// ::2 
// stderr
// + Write on regular file.
    if (fp->_file == STDERR_FILENO)
    {
        if (fp->____object == ObjectTypeFile){
            goto RegularFile;
        }
        goto fail;
    }

//
// == NIC device ===============================================
//

    if ( fp->____object == ObjectTypeNIC ){
       printk("__write_imp: Writing on NIC device ... #todo\n");
       //network_on_sending(ubuf,count);
       return 0;
   }

//
// == Sockets ===============================================
//
    ncopy = count;

// == Socket ===============================
// Descobrindo o soquete que devemos copiar.
// Se o arquivo é do tipo socket, então devemos
// saber onde está o buffer.
// #todo: Talvez podemos chamar a função socket_write().
// #todo: Lidar com a questao do copiar, pois
// accept deve retornar o fd do cliente, para que o servidor
// construa uma lista de clientes.
// Entao copiar sera uma opçao, gerenciada por uma flag.

// #bugbug
// Nao podemos fazer a copia se os dois sockets 
// estiverem com a conexao pendente.
    if (fp->____object == ObjectTypeSocket)
    {
        do_credits_by_tid(current_thread);
        
        // Can't write.
        if ((fp->_flags & __SWR) == 0){
            debug_print("__write_imp: [FAIL] flag __SWR \n");
            //yield (current_thread);
            goto fail;
        }

        // cheio? 
        // Não podemos escrever porque o buffer está cheio.
        // Acorde os leitores para esvaziar.
        // Mas e se não tiver mais escritores.
        // #todo: Indicação da quantidade de escritores.
        // Sendo assim, vamos dizer que também podem ler
        // caso existam leitores. Mas não pode mais escrever.
        if (fp->socket_buffer_full == TRUE)
        {
            
            debug_print("__write_imp: [FAIL] can't write on a full buffer\n");
            
            //#debug
            //debug_print("__write_imp: WAKEUP READER\n");
            
            fp->_flags = 0; // não pode mais escrever.
            fp->_flags |= __SRD;                 // pode ler.
            do_thread_ready( fp->tid_waiting );  // acorda leitores
            fp->tid_waiting = -1;
            if (fp->sync.block_on_write_full == TRUE)
            {
                //#debug
                //debug_print("__write_imp: SLEEP WRITER\n");
                fp->tid_waiting = current_thread;
                //do_thread_waiting(current_thread);
                yield (current_thread);
                goto fail;
            }
            yield (current_thread); 
            goto fail;
        }

        // Se o buffer não estiver cheio,
        // Então escreva e acorde os leitores.
        if (fp->socket_buffer_full == FALSE)
        {
            // Se podemos escrever.
            // #todo: Ja fizemos isso logo acima.
            if ( fp->_flags & __SWR )
            {
                //debug_print ("__write_imp: >>>> WRITE\n");
                fp->_flags = 0;

                // Write in the socket buffer.
                nbytes = 
                    (ssize_t) file_write_buffer ( 
                                  (file *) fp, 
                                  (char *) ubuf, 
                                  (int) count );

                // fail
                if (nbytes <= 0){
                    debug_print("__write_imp: [FAIL] file_write_buffer couldn't write on socket \n");
                    //#todo: Isso pode afetar o desempenho.
                    yield (current_thread);
                    goto fail;
                }

                // ok, write funcionou.
                if (nbytes>0)
                {
                    // #debug
                    // debug_print("__write_imp: WAKEUP READER\n");
                    
                    fp->socket_buffer_full = TRUE;     // buffer cheio
                    fp->_flags &= ~__SWR;              // nao posso mais ESCREVER.            
                    fp->_flags |= __SRD;               // pode ler 
                    //do_thread_ready(fp->tid_waiting);  // acorda leitores
                    fp->tid_waiting = -1;
                
                    // #bugbug
                    // Isso pode ser ruim pela natureza da chamada sys_write()
                    // que vem de uma syscall que nao salvou o contexto.
                    if (fp->sync.block_on_write == TRUE)
                    {
                        // #debug
                        //debug_print("__write_imp: SLEEP WRITER\n");
                        
                        fp->tid_waiting = current_thread;
                        //do_thread_waiting(current_thread);
                    }
                    // #bugbug: test ...
                    //  impedir que eu mesmo me leia.
                    //yield (current_thread);
                    return (ssize_t) nbytes;
                }
            }
        }

        panic("__write_imp: Unexpected error when writing on socket\n");
    }

//
// Arquivos normais (regular)
//

RegularFile:

// == Regular file =========================================
// Tem que retonar o tanto de bytes escritos.
// Escreve em uma stream uma certa quantidade de chars.

    if (fp->____object == ObjectTypeFile)
    {
        // Can't write.
        if ( (fp->_flags & __SWR) == 0){
             debug_print("__write_imp: [FLAGS] Can't write!\n");
             return 0;
        }

        // Can write.
        if (fp->_flags & __SWR)
        {
            // Regular file.
            nbytes = 
                (ssize_t) file_write_buffer ( 
                              (file *) fp, 
                              (char *) ubuf, 
                              (int) count );

            // Avisa que o arquivo não está mais no modo escrita,
            // que agora pode ler.

            // Adiciona o bit que permite a leitura.
            // Assim o servidor pode ler o request.
            // #todo: wait on write.
            // #bugbug:
            // A questão é que se o cliente está esperando por resposta,
            // então ele lerá também.
            // Atualizamos a flag e acordamos quem esperava pelo evento.

            if (nbytes>0){
                fp->_flags = __SRD;
                fp->sync.can_read = TRUE;
                do_thread_ready(fp->tid_waiting);
                return (ssize_t) nbytes;
            }

            // suspenso.
            // Não conseguimos escrever ... 
            // nada de errado, apenas esperaremos.
            //do_thread_waiting (current_thread);
            //fp->tid_waiting = current_thread;
            //fp->_flags |= __SWR;  //pode escrever.
            //scheduler();
            debug_print ("__write_imp: [FAIL] file_write_buffer fail!\n");
            return 0;
        }
 
        debug_print ("__write_imp: [FAIL] Something is wrong!\n");
        return (ssize_t) (-1);
    } //regular file.

// ======================================================
// Pseudo terminal multiplexer.
    if (fp->____object == ObjectTypePTMX){
        printk ("__write_imp: [TODO] trying to write a PTMX device file\n");
        //refresh_screen();
        return 0;
    }

// ======================================================
// pty () pseudo terminal.
    if (fp->____object == ObjectTypePTY){
        printk ("__write_imp: [TODO] trying to write a PTY device file\n");
        //refresh_screen();
        return 0;
    }

// ======================================================
// File system
    if (fp->____object == ObjectTypeFileSystem){
        printk ("__write_imp: [TODO] trying to write a file system\n");
        //refresh_screen();
        return 0;
    }

// ======================================================
// pipe:
// See: pipe.c
    //#bugbug: No not use sys_xxx use another wrapper.
    if (fp->____object == ObjectTypePipe){
        return (ssize_t) sys_write_pipe ( (int) fd, (char *) ubuf, (int) count ); 
    }

    // ...

//==============================================
    debug_print ("__write_imp: [FAIL] Unknown object type!\n");
//==============================================

fail:
    // #debug
    // debug_print ("__write_imp: [FAIL] Something is wrong!\n");
    // printk      ("__write_imp: [FAIL] something is wrong!\n");
fail2:

    //invalidate_screen();

// #bugbug: 
// Isso pode degradar o desempenho.
    //refresh_screen();   

    // Não conseguimos escrever ... 
    // Estamos com problemas 
    //do_thread_waiting (current_thread);
    //fp->tid_waiting = current_thread;
    //fp->_flags |= __SWR;  //pode escrever.
    //scheduler();

    return (ssize_t) (-1);
}

// __open_imp:
// It implements the service for open() syscall.
// open() function implementation in ring 0.
// Syscall 16.
// #bugbug
// Precisamos de um endereço em ring 3
// para que a libc possa manipular o buffer ?
// ou open deve ser usado somente com dispositivos ??
// #obs:
// vamos retornar o indice da tabela de arquivos abertos 
// do processo atual.
// #? isso não pertence à fcntl.c?
// SVr4,  4.3BSD,  POSIX.1-2001. 
// OUT: fd.

int 
__open_imp (
    const char *pathname, 
    int flags, 
    mode_t mode )
{
// Limits:
// Probably BUFSIZ (1024 for now) is the limit for this routine.
// To improve this we need to allocate a bigger buffer 
// in the file structure.
// The worker is: do_read_file_from_disk() with the same limit.

// Open a file, or create if is doesn't exist.
// creat chama open.
// open tenta ler num arquivo que nao existe?

    int value = -1;
    file *fp;

    //debug_print ("__open_imp: $\n");

// #todo:
// check arguments.

    if ((void*) pathname == NULL){
        return (int) (-EFAULT);
    }
    if (*pathname == 0){
        return (int) (-EFAULT);
    }

// Local copy
// Coping more than we need, 
// this way we're coping the 0x00 byte at the end of string
// and some extra bytes.

    char pathname_local_copy[256];
    memset(pathname_local_copy,0,256);
    strncpy(pathname_local_copy,pathname,256);

// ----------------------------
// #hackhack
// Trying to open the current kernel virtual console.
// But this is the standard stdout for everyone.
// Wrinting on stdout always send the data to the current console.

// /dev/tty	
//    Represents the controlling terminal for the current process. 
//    If a process is attached to a terminal, reading from /dev/tty gets its input, 
//    and writing sends output there.

    // # All the processes are associated with the current kernel console
    //   in the open file table.
    if (kstrncmp(pathname_local_copy,"/dev/tty",8) == 0)
        return (int) STDOUT_FILENO;
    if (kstrncmp(pathname_local_copy,"/DEV/TTY",8) == 0)
        return (int) STDOUT_FILENO;

// /dev/console	
//     Represents the system console, where kernel messages 
//     (printk()) and emergency output are directed. 
//     This is often mapped to the first virtual terminal (/dev/tty0) or 
//     a serial device in headless setups.

    if (kstrncmp(pathname_local_copy,"/dev/console",12) == 0)
        return (int) STDOUT_FILENO;
    if (kstrncmp(pathname_local_copy,"/DEV/CONSOLE",12) == 0)
        return (int) STDOUT_FILENO;


//----------------------------

// Searth for a device associated with this path
// in the deviceList[]
// See: devmgr.c

// #test #todo
// OK, it's working when we call open() from ring3,
// using a pathname.
// Now we need to get the filepointer 
// from the device data structures,
// put it into the process structure in p->Objects[]
// and return the index to the ring3.

    fp = (file *) devmgr_search_in_dev_list(pathname_local_copy);

// Yes, 
// we have a valid pointer found in the table.

    if ((void*) fp != NULL)
    {
        if (fp->isDevice == TRUE)
        {
            // #todo 
            // Put it into the list inside the
            // current process structure
            // and return the fd.
            printk("__open_imp: #todo isDevice!\n");
            //refresh_screen();
            goto fail;
        }
    }

// Vamos carregar o arquivo que esta no disco.
// See: fs.c
// OUT: fd

// #bugbug
// This function is gonna call fs_fntos()
// that is gonna include the extension .bin
// if the name doesn't have one.
// This is not what we want for all the cases.

// 
    value = 
        (int) do_read_file_from_disk ( 
                  (char *) pathname_local_copy, 
                  flags, 
                  mode );

    if (value < 0){
        goto fail;
    }

// The limit is 32.
// Too many open files.

    if (value > 31){
        return (int) (-EMFILE);
    }

// fd
    // #debug
    // refresh_screen();
    return (int) value;
fail:
    return (int) -1;
}

// __close_imp:
// It implements the service for close() syscall.
// Fechar um dos objetos abertos do processo atual.
// O descritor é um índice na sua tabela de objetos abertos.
// #todo: Se fecharmos um socket, tem que antes destruir a estrutura
// de socket associada ao arquivo.
// See:
// https://man7.org/linux/man-pages/man2/close.2.html
// https://pubs.opengroup.org/onlinepubs/009695399/functions/close.html
/*
A successful close does not guarantee that the data has been
successfully saved to disk, as the kernel uses the buffer cache
to defer writes.  Typically, filesystems do not flush buffers
when a file is closed.  If you need to be sure that the data is
physically stored on the underlying disk, use fsync(2).  (It will
depend on the disk hardware at this point.)
*/

int __close_imp(int fd)
{
// #: It needs to be used only as a worker for syscalls.

    file *object;
    struct process_d *p;
    pid_t current_process = -1;
    int Done=FALSE;

// ??
// Can we close this devices?

    /*
    if (fd == 0 || fd == 1 || fd == 2 )
    {
        debug_print("__close_imp: [FIXME] We can't close the standard stream\n");
        // WHY NOT ???!!
        return (int) (-1);
    }
    */

// Invalid fd
    if ( fd < 0 || fd >= OPEN_MAX )
    {
        debug_print("__close_imp: bad fd\n");
        return (int) (-EBADF);
    }

// Process
    current_process = (pid_t) get_current_process();
    if (current_process < 0 || current_process >= PROCESS_COUNT_MAX){
        debug_print("__close_imp: current_process\n");
        goto fail;
    }
    p = (void *) processList[current_process];
    if ((void *) p == NULL){
        debug_print("__close_imp: p\n");
        goto fail;
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        debug_print("__close_imp: p validation\n");
        goto fail;
    }

// object
// The object is a file structure.

    object = (file *) p->Objects[fd];
    if ((void *) object == NULL){
        debug_print("__close_imp: object\n");
        goto fail;
    }
    if ( object->used != TRUE || object->magic != 1234 ){
        debug_print("__close_imp: object validation\n");
        goto fail;
    }

// What type of object?
// socket, pipe, virtual console, tty, regular file ??

    Done = FALSE;

// ===============================================
// socket
// Do NOT save it into the disk.
    if (object->____object == ObjectTypeSocket)
    {
        debug_print("__close_imp: Trying to close a socket object\n");
        object->socket->used = FALSE; //invalidando a estrutura de socket
        object->socket->magic = 0;//invalidando a estrutura de socket
        Done = TRUE;
    }

// ==============================================
// pipe
    if (object->____object == ObjectTypePipe){
        debug_print("__close_imp: Trying to close a pipe object\n");
        Done = TRUE;
    }

// ====================================================
// virtual console.
// #todo
    if (object->____object == ObjectTypeVirtualConsole){
        debug_print("__close_imp: Trying to close a virtual console object\n");
        return 0;
    }

// =====================================================
// tty
    if (object->____object == ObjectTypeTTY){
        debug_print("__close_imp: Trying to close a tty object\n");
        Done = TRUE;
    }

// #bugbug
// Poderemos ter problemas aqui com os diversos tipos
// de arquivos.

// #bugbug
// ugly test

// ===========================================
// regular file
// Save the file if it is a regular file.
    if (object->____object == ObjectTypeFile)
    {
        debug_print("__close_imp: [FIXME] trying to close a regular file\n");
        //debug_print("__close_imp: [FIXME] fsSaveFile\n");

        // #fixme: buffer limit
        if (object->_lbfsize < 512){  
            debug_print("__close_imp: [FIXME] Ajusting file size\n");
            object->_lbfsize = 512; 
        }

        // Save file in the root dir.
        // #bugbug: Where to save?
        debug_print("__close_imp: [FIXME] fsSaveFile\n");

        fsSaveFile ( 
            VOLUME1_FAT_ADDRESS, 
            VOLUME1_ROOTDIR_ADDRESS, 
            FAT16_ROOT_ENTRIES,
            (char *)         object->_tmpfname, 
            (unsigned long) (object->_lbfsize/512),  // file_size, in sectors       
            (unsigned long)  object->_lbfsize,       // size_in_bytes,  
            (char *)         object->_base,          // buffer address          
            (char)           0x20 );                 // flag ??

        Done = TRUE;
    }

    // ...

// Object type not supported.
    if (Done != TRUE){
        debug_print("__close_imp:[FAIL] Object type not supported\n");
        goto fail;
    }

// Close it!
// Everything was done.
// Let's destroy the object pointer, empty the spot in the list
// and return 0.
    if (Done == TRUE)
    {
        object = NULL;
        p->Objects[fd] = (unsigned long) 0;
        return 0;
    }

fail:
    debug_print("__close_imp: [FAIL]\n");
    return (int) (-1);
}

// get_file_from_fd:
// helper
// dado o fd, pegamos o ponteiro 
// para estrutura de arquivo na lista de
// objetos abertos no processo atual.
// ## Talvez essa rotina ja foi implementada
// em algum outro lugar.
// Prototype in rtl/sci/sys.h
// OUT:
// Return the file pointer.
file *get_file_from_fd(int fd)
{
// File pointer
    file *fp;
// Current process
    struct process_d *p;
    pid_t current_pid = -1;

// Parameter
    if (fd < 0 || fd >= 32){
        goto fail;
    }

// Current PID and process pointer.
    current_pid = (pid_t) get_current_process();
    if ( current_pid < 0 || current_pid >= PROCESS_COUNT_MAX ){
        return NULL;
    }
    p = (struct process_d *) processList[current_pid];
    if ((void*) p == NULL){
        debug_print ("get_file_from_fd: p\n");
        panic       ("get_file_from_fd: p\n");
        //return NULL;
    }
    if (p->used != TRUE){
        return NULL;
    }
    if (p->magic != 1234){
        return NULL;
    }

// File pointer.
    fp = (file *) p->Objects[fd];
    if ((void*) fp == NULL){
        //#debug
        //printk("fd{%d} pid{%d}\n",fd,current_pid);
        //printk("entry0: %x\n", p->Objects[0]);
        //printk("entry1: %x\n", p->Objects[1]);
        //printk("entry2: %x\n", p->Objects[2]);
        //printk("entry3: %x\n", p->Objects[3]);
        //printk("entry4: %x\n", p->Objects[4]);
        goto fail;
    }
    return (file *) fp;

fail:
    return NULL;
}

// Get free slots in the file_table[].
int get_free_slots_in_the_file_table(void)
{
// Called by kstdio.c when creating streams.

    file *tmp;
    register int i=0;

    if (FileTableInfo.initialized != TRUE){
        panic("get_free_slots_in_the_file_table: FileTableInfo not initialized\n");
    }

    for (i=0; i<NUMBER_OF_FILES; i++)
    {
        tmp = (void*) file_table[i];
        // Nenhum file descritor está usando essa estrutura.
        if (tmp->used == TRUE && 
            tmp->magic == 1234 && 
            tmp->fd_counter == 0)
        { 
            return (int) i;
        }
    };

    return (int) -1;
}

// Get free slots in the inode_table[].
int get_free_slots_in_the_inode_table(void)
{
    struct inode_d *tmp;
    register int i=0;

    for (i=0; i<32; i++)
    {
        tmp = (void*) inode_table[i];
        // Se nenhum descritor de estrutura de arquivo 
        // está usando essa estrutura inode.
        if (tmp->used == TRUE && 
            tmp->magic == 1234 && 
            tmp->filestruct_counter == 0)
        { 
            return (int) i; 
        }
    }

    return (int) -1;
}

// Get file system type.
// #bugbug: 
// What is the volume?
// Is it only for the system volume?
int get_filesystem_type (void)
{
    return (int) g_currentvolume_filesystem_type;
}

// Set file system type.
// #bugbug: 
// What is the volume?
// Is it only for the system volume?
void set_filesystem_type (int type)
{
    if (type < 0)
        return;
    g_currentvolume_filesystem_type = (int) type;
}

// fs_count_path_levels:
// Counting path levels.
// Credits: Sirius OS.
// return 'int'?
unsigned long fs_count_path_levels (unsigned char *path)
{
    unsigned long Counter=0;
    register int i=0;
// (80*25), 25 lines.
    size_t MaxChars = 2000;

// Parameter
    if ((void*) path == NULL){
        return 0;
    }
    if (*path == 0){
        return 0;
    }

    for ( i=0; i < MaxChars; ++i )
    {
        if (path[i] == '/') { Counter++; }
        if (path[i] == '\0'){ break; }
    };

    return (unsigned long) Counter;
}

// Get a file pointer from the global file table.
void *get_file (int Index)
{
//Limits.
//#todo: max.
    if (Index < 0){
        // ?? todo: message
        return NULL;
    }
    return (void *) file_table[Index];
}

// Set a file pointer into the global file table.
void set_file ( void *file, int Index )
{
// #todo:
// Limite máximo da lista.

// Parameters
    if ((void *) file == NULL){
        // ?? todo: message
        return;
    }
    if (Index < 0){
        // ?? todo: message
        return;
    }

// Include pointer into the list.
    file_table[Index] = (unsigned long) file;
}

// #todo
// Change the return type to 'int' and
// remove all the messages. Maybe.
// Create the 'root' filesystem structure.
void fs_init_structures (void)
{
    int Type=0;

    //debug_print ("fs_init_structures: [TODO]\n");

//
// root
//

// struct
// Create the 'root' filesystem structure.
// Initialize the 'root' filesystem structure.
// "/"

    //debug_print ("fs_init_structures: root\n");

    root = (void *) kmalloc( sizeof(struct filesystem_d) );
    if ((void *) root == NULL){
        panic("fs_init_structures: Couldn't create the root structure\n");
    }
    memset( root, 0, sizeof(struct filesystem_d) );

    root->objectType = ObjectTypeFileSystem;
    root->objectClass = ObjectClassKernelObject;
    root->used = TRUE;
    root->magic = 1234;

    root->name = (char *) ____root_name;

// #todo 
// #bugbug   volume_vfs  ??

// Se o volume do vfs ainda não foi criado 
// então não podemos prosseguir.

        //if ( (void *) volume_vfs == NULL )
        //{
        //    debug_print("fs_init_structures: [FAIL] volume_vfs not initialized");
        //    panic      ("fs_init_structures: [FAIL] volume_vfs not initialized");
        //}
        //volume_vfs->fs = root;
 

// Save is in the 'storage' low level structure.

    if ((void*) storage == NULL){
        panic("fs_init_structures: storage");
    }

    storage->fs = (void*) root;
    // ...

// Type
// #bugbug: 
// Em qual disco e volume pegamos o tipo de sistema de arquivos?

    //debug_print ("fs_init_structures: Type\n");

// #bugbug
// Why do we have this information?
    Type = (int) get_filesystem_type();
    Type = (Type & 0xFFFF);
    if (Type <= 0){
        panic ("fs_init_structures: [PANIC] Type\n");
    }else{
        root->type = (int) Type;
    };

    switch (Type){

    case FS_TYPE_FAT16:

        // Disk stuff.
        // spc - Sectors per cluster.
        root->spc = (int) VOLUME1_SPC;
        //root->spc = (int) get_spc(); 

        // Rootdir, Fat and data area.
        // #bugbug: Specific for fat16.
        
        // root dir
        root->rootdir_address   = VOLUME1_ROOTDIR_ADDRESS;
        root->rootdir_first_lba = VOLUME1_ROOTDIR_LBA;
        //root->rootdir_last_lba = 0;  //#todo 
        root->rootdir_size_in_sectors = 0;  //#todo
        
        // fat1
        root->fat_address     = VOLUME1_FAT_ADDRESS;
        root->fat_first_lba   = VOLUME1_FAT_LBA;
        //root->fat_last_lba = 0;  //#todo
        root->fat_size_in_sectors = 0;  //#todo
        
        // fat2
        // ...
        
        // dataarea
        //filesystem->dataarea_address = ??;
        root->dataarea_first_lba    = VOLUME1_DATAAREA_LBA; 
        //root->dataarea_last_lba = 0;  //#todo
        //root->dataarea_size_in_sectors = 0;  //#todo
 
        //root->fs_first_lba = 0;  //#todo
        
        //#todo (dataarea + dataarea size)?
        //root->fs_last_lba = 0;   

        //(mbrsize + reserved + rootdirsize + fat1size + fat2size +dataareasize)
        //root->fs_size_in_sectors = 0;

        // Root dir.
            
        // Number of entries in the root dir.
        // #bugbug: Specific for fat16.
        root->dir_entries = FAT16_ROOT_ENTRIES;

        // Size of the entry in bytes.
        // #bugbug: Specific for fat16.
        root->entry_size = FAT16_ENTRY_SIZE;

        // ...
        break;

    // #todo: Not implemented yet.
    case FS_TYPE_EXT2:
    default:
        panic ("fs_init_structures: Default Type\n");
        break;
    };
}

// Close a file given the structure pointer.
int file_close (file *_file)
{
// #todo: Not implemented yet!

    debug_print("file_close: todo\n");

    if ((void*) _file == NULL){
        return (int) -1;
    }
// Structure not in use.
    if (_file->used != TRUE)
        return (int) -1;
// Reuse structure.
    //if (_file->magic == 4321)
        //something();

    // ...

    return (int) -1;
}

size_t file_get_len(file *_file)
{
// Not implemented yet!

    debug_print("file_get_len: todo\n");

    if ((void*) _file == NULL){
        return -1;
    }

    //... 

    return (size_t) -1;
}

//OUT: inode structure.
struct inode_d *file_inode (file *f)
{
    if ((void *) f == NULL){
        return NULL;
    }
    // #todo: Test MAGIC.
    return (struct inode_d *) f->inode;
}

// ??
// #todo: Explain this.
int file_truncate (file *_file, size_t len)
{
// Not implemented yet!

    debug_print("file_truncate: todo\n");

    if ( (void*) _file == NULL ){
        return -1;
    }
    if (len<0){
        return -1;
    }

    return -1;
}

// fsCheckELFFile:
// Check the signature in a elf header.
// OUT:
// 0 = OK. #todo: Isso poderia ser '1'.
// < 0 Means error.
// #todo
// We can do the same for some other types.
// Use TRUE or FALSE.

// #todo
// is address virtual or physical?
// Change this name to pa or va.

int fsCheckELFFile(unsigned long address)
{
    if (address == 0){
        return 0;
    }
    return (int) elfCheckSignature(address);
}

/*
 * fsInitTargetDir:
 *     Para inicializarmos o sistema já com um alvo.
 */
// #todo
// is dir_address virtual or physical?
// Change this name to dir_pa or dir_va.

// using the structure 'current_target_dir'.
// IN:
// directory address, directory name.

int fsInitTargetDir (unsigned long dir_address, char *dir_name)
{
    register int i=0;

    current_target_dir.used = TRUE;
    current_target_dir.magic = 1234;
    for ( i=0; i<11; i++ ){
        current_target_dir.name[i] = '\0';
    };

// Dir address
    if (dir_address == 0){
        panic("fsInitTargetDir: dir_address\n");
    }
    //current_target_dir.current_dir_address = VOLUME1_ROOTDIR_ADDRESS;
    current_target_dir.current_dir_address = 
        (unsigned long) dir_address;

// Dir name
    if ((void*) dir_name == NULL){
        panic("fsInitTargetDir: dir_name\n");
    }
    if (*dir_name == 0){
        panic("fsInitTargetDir: *dir_name\n");
    }
    //current_target_dir.name[0] = '/';
    //current_target_dir.name[1] = 0;

// Limits: 
// Copy 8 bytes only
    for ( i=0; i<8; i++ ){
        current_target_dir.name[i] = dir_name[i];
    };
// done:
    current_target_dir.initialized = TRUE;

    return 0;
}

// fsList
// Ring 0 routine to list files.
// #todo
// Use 'pathname'.
// #shortcut: '[' 

int fsList(const char *dir_name)
{
    int Absolute = FALSE;
    register int i=0;

// Directory name
    if ((void *) dir_name == NULL){
        debug_print ("fsList: dir_name\n");
        goto fail;
    }
    if ( *dir_name == 0 ){
        debug_print ("fsList: *dir_name\n");
        goto fail;
    }

// Copy name
// Limits: 
// Copy 8 bytes only

    for ( i=0; i<8; i++ ){
        current_target_dir.name[i] = dir_name[i];
    };
    current_target_dir.name[i] = '\0';

// The root directory.
// #shortcut: '[' 
// #todo: Explain it.
// Open the dirname '/'.

// #test: Default in root.
    current_target_dir.current_dir_address = 
        VOLUME1_ROOTDIR_ADDRESS;
    current_target_dir.name[0] = '/';
    current_target_dir.name[1] = '\0'; 

    if ( dir_name[0] == '[' && 
         dir_name[1] == 0 )
    {
        //debug_print ("fsList: root\n");
        Absolute = TRUE;

        current_target_dir.current_dir_address = 
            VOLUME1_ROOTDIR_ADDRESS;
        
        // Clear the whole buffer.
        for ( i=0; i<11; i++ ){
            current_target_dir.name[i] = '\0';
        };
        // Create the name.
        current_target_dir.name[0] = '/';
        current_target_dir.name[1] = '\0'; 
    }


// #bugbug
// We are using the current directory address,
// not the directory provide by the user.
// #todo
// Set up the current dir address, based on the
// name provided by the user.

//
// == current_target_dir ====================
//

// directory address.
    if (current_target_dir.current_dir_address == 0){
        debug_print ("fsList: current_target_dir.current_dir_address\n");
        goto fail;
    }
    
    // #bugbug
    // Missing string finalization.
    //printk ("fsList: current_target_dir.name = {%s}\n", current_target_dir.name);

// Listing ...
// IN:
// name, dir address, number of entries;
// No return value.

    // Well, the root dir has 512 entries.
    int n=256;
    if ( current_target_dir.current_dir_address == VOLUME1_ROOTDIR_ADDRESS )
    {
        n=512;
    }

    fsFAT16ListFiles ( 
        (const char *)     current_target_dir.name,
        (unsigned short *) current_target_dir.current_dir_address, 
        (int) n );

    //debug_print ("fsList: done\n");
    return 0;

fail:
    debug_print ("fsList: fail\n");
    //refresh_screen();
    return (int) -1;
}

/*
 * fsListFiles:
 *     Lista os arquivos em um diretório, 
 * dados os índices de disco, 
 * volume e diretório.
 */
// #bugbug
// Do not list this in ring0.

void 
fsListFiles ( 
    int disk_id, 
    int volume_id, 
    int directory_id )
{
// #todo: Incomplete routine.

    // #todo: 
    // Checar mais limites.
    // Use this: if ( disk_id < 0 || volume_id < 0 || directory_id < 0 )

    if ( disk_id == -1 || volume_id == -1 || directory_id == -1 )
    {
        debug_print ("fsListFiles: [FAIL] parameters\n");
        goto fail;
    }

    printk ("fsListFiles: disk={%d} vol={%d} dir={%d}\n", 
        disk_id, volume_id, directory_id );

// Show!
// Se o diret�rio selecionado � o diret�rio raiz do VFS.

    if ( current_disk == 0 && 
         current_volume == 0 && 
         current_directory == 0 )
    {
        debug_print ("fsListFiles: [FIXME] current\n");
        //vfsListFiles ();
        goto done;
    }

// #todo: 
// Suportar outros diretórios.
// ...

    goto done;

fail:
    printk ("fail\n");
done:
    //refresh_screen ();
    return;
}

/*
 * fsUpdateWorkingDiretoryString:
 *     +Atualiza o pathname na estrutura do processo atual.
 *     +Atualiza o pathname na string global. 
 */ 
// Used by the service 175, cd command.

void fsUpdateWorkingDiretoryString (char *string)
{
    struct process_d  *p;
    pid_t current_process = -1;
    char *tmp;
    register int i=0; 
    int string_size = 0;

    //debug_print ("fsUpdateWorkingDiretoryString:\n"); 

// Initialized?
    if (CWD.initialized != TRUE)
    {
        debug_print ("fsUpdateWorkingDiretoryString: [FAIL] CWD not initialized\n"); 
        // #todo
        // Call the initialization routine.
        return;
    }

// Parameter
    if ((void *) string == NULL){
        debug_print ("fsUpdateWorkingDiretoryString: string\n"); 
        return;  
    }
    if (*string == 0){
        debug_print ("fsUpdateWorkingDiretoryString: *string\n"); 
        return;  
    }
    tmp = string;

// String size.
    string_size = sizeof(string);
    if (string_size <= 0){
        debug_print ("fsUpdateWorkingDiretoryString: string_size\n"); 
        return;  
    }

// Current process
    current_process = (pid_t) get_current_process();
    if (current_process < 0 || current_process >= PROCESS_COUNT_MAX){
        panic ("fsUpdateWorkingDiretoryString: current_process\n");
    }
    p = (struct process_d *) processList[current_process];
    if ((void *) p == NULL){
        panic ("fsUpdateWorkingDiretoryString: p\n");
    }else{
        if ( p->used != 1 || p->magic != 1234 ){
            panic ("fsUpdateWorkingDiretoryString: p validation\n");
        }

        // Atualiza a string do processo atual. 
        // Concatenando.

        if ((void *) string != NULL)
        {
            // #bugbug
            // We need to handle the string size limit.
            
            // Concatena string.
            strcat ( p->cwd_string, string );

            // Concatena separador.
            strcat ( p->cwd_string, FS_PATHNAME_SEPARATOR );

            // Atualiza a string global usando a string do 
            // processo atual.
            // #bugbug: nao precisamos disso ...
            // so precismos de cwd na estrutura de processo.
            
            // #importante
            // Respeitar o limite.
            
            for ( i=0; i<32; i++ ){
                CWD.path[i] = p->cwd_string[i];
            };
            CWD.path[31] = 0; //finaliza

            // #bugbug: rever isso.
            // Nome do diretório alvo atual.
            
            // nao precismos disso ...
            // ou usamos o cwd do processo ou
            // o diretorio raiz para paths absolutos.
            
            for ( i=0; i< 11; i++ )
            {
                current_target_dir.name[i] = *tmp;
                tmp++;
            };
            current_target_dir.name[11] = 0; //finaliza
        }
    };

    //debug_print ("fsUpdateWorkingDiretoryString: done\n"); 
}

/*
 * fs_fntos:  
 *     (Filename to string).   
 *     'file name to string'.
 *     rotina interna de support.
 *     isso deve ir para bibliotecas depois.
 *     não tem protótipo ainda.
 * Created by: Luiz Felipe.
 * 2020 - Adapted by Fred Nora.
 */
 // #bugbug
 // Isso modifica a string lá em ring3.
 // prejudicando uma segunda chamada com a mesma string
 // pois já virá formatada.
// #bugbug
// const char * tornaria esse endereço em apenas leitura.
// Allowed in fat16: $ % ' - _ @ ~ ` ! ( ) { } ^ # &
void fs_fntos(char *name)
{
    int i=0;
    int ns = 0;
    char ext[4];
    //int WeReallyNeedAnBinExt = add_bin_ext;

    // #todo
    int fAddNewExt = FALSE;
    
// No extension
    ext[0] = 0;
    ext[1] = 0;
    ext[2] = 0;
    ext[3] = 0;

// Parameter
    if ((void*) name == NULL){
        return;
    }
    if (*name == 0){
        return;
    }

// Transforma em maiúscula enquanto não achar um ponto.
// #bugbug: 
// E se a string já vier maiúscula teremos problemas.

// Transforma somente as letras e 
// somente ate encontrarmos o ponto.
    //int max = (8+1+3); 
    //int max = (8); 
    while ( *name && *name != '.' )
    {
        // Se a string eh muito grande.
        //if (ns >= max)
            //break;
        
        if ( *name >= 'a' && *name <= 'z' )
        {
            *name -= 0x20;
        }
        name++;
        ns++;
    };

// #bugbug
// Esse negócio de acrescentar a extensão
// não é bom para todos os casos.
// >> name[0] significa que na ultima posiçao do ponteiro *p,
//    encontramos o valor 0x00.
// >> Mas ainda somos menor ou igual a 8, que eh o campo
//    para nome de 8 bytes.

    if ( name[0] == '\0' && ns <= 8 )
    {
        // The flag says that 
        // we're gonna have a new extension.
        fAddNewExt = TRUE;
        // Complete the DOS name with spaces. Until 8 bytes.
        goto CompleteName8WithSpaces;
    }

    //if ( name[0] == '.' && ns < 8 )

// Aqui name[0] é o ponto. 
// Mas o nome tem 8 bytes.
// Então constrói a extensão colocando as letras na extensão.

    for ( i=0; 
          i<3 && name[i+1]; 
          i++ )
    {
        // #ps: name[0] = '.'

        //Transforma uma letra da extensão em maiúscula.

        // Letras na extensao: 
        // Mudando as letras para maiusculo.
        if ( name[i+1] >= 'a' && 
             name[i+1] <= 'z' )
        {
            name[i+1] -= 0x20;   // Change
            ext[i] = name[i+1];  // Save
        }

        // Bytes especiais na extensao: 
        // Colocando os bytes, sem mudar.
        // Allowed in fat16: $ % ' - _ @ ~ ` ! ( ) { } ^ # &
                
        if ( name[i+1] == '$' ||
             name[i+1] == '%' ||
             name[i+1] == '-' ||
             name[i+1] == '_' ||
             name[i+1] == '(' ||
             name[i+1] == ')' ||
             name[i+1] == '{' ||
             name[i+1] == '}'  )
        {
            ext[i] = name[i+1];  // Save
        }

        // Se a extensao tiver numeros.
        if  ( name[i+1] >= '0' &&
              name[i+1] <= '9' ) 
        {
            ext[i] = name[i+1];  // Save
        }
        
        // E se tiver um 0x00 entre os bytes da extensao?
        // Mas a extensao tinha um ponto.
        if ( name[i+1] == 0x00 )
        {
            fAddNewExt = FALSE;
            goto CompleteName8WithSpaces;
        }
    };

// Acrescentamos ' ' até completarmos as oito letras do nome.
// Acrescentamos a extensão
// Finalizamos.

CompleteName8WithSpaces:

// Save some spaces in the name until 8 bytes.
    while (ns < 8)
    {
        *name++ = ' ';
        ns++;
    };

// Add extension.
AddExt:

// Prepare a new extension 
// if no extension was provided.
    if (fAddNewExt == TRUE)
    {
        // Pre-load the new extension bytes.
        ext[0] = 'B';  
        ext[1] = 'I';  
        ext[2] = 'N';  
        ext[3] = '\0';
    }

// Save extension bytes.
    for (i=0; i<3; i++)
    {
        // No caso de extensoes com 3 bytes
        // copiamos todos os bytes.
        if ( ext[i] != 0x00 )
        {
            *name++ = ext[i];  
        }
        
        // No caso das extensoes com menos bytes,
        // o buffer pode ter 0x00.
        // No lugar no 0x00 colocaremos ' '.
        if ( ext[i] == 0x00 )
        {
            *name++ = ' ';  
        }
    };

// #bugbug
// Is it an overflow?

    *name = '\0';
}

// No 'BIN' extension is added.
// Add '3 spaces' in the extension field.
void fs_fntos2(char *name)
{
    int i=0;
    int ns = 0;
    char ext[4];
    //int WeReallyNeedAnBinExt = add_bin_ext;

    // #todo
    int fAddNewExt = FALSE;
    
// No extension
    ext[0] = 0;
    ext[1] = 0;
    ext[2] = 0;
    ext[3] = 0;

// Invalid parameter
    if ((void*) name == NULL){
        return;
    }
    if (*name == 0){
        return;
    }

// Transforma em maiúscula enquanto não achar um ponto.
// #bugbug: 
// E se a string já vier maiúscula teremos problemas.

// Transforma somente as letras e 
// somente ate encontrarmos o ponto.
    //int max = (8+1+3); 
    //int max = (8); 
    while ( *name && *name != '.' )
    {
        // Se a string eh muito grande.
        //if (ns >= max)
            //break;
        
        if ( *name >= 'a' && *name <= 'z' )
        {
            *name -= 0x20;
        }
        name++;
        ns++;
    };

// #bugbug
// Esse negócio de acrescentar a extensão
// não é bom para todos os casos.
// >> name[0] significa que na ultima posiçao do ponteiro *p,
//    encontramos o valor 0x00.
// >> Mas ainda somos menor ou igual a 8, que eh o campo
//    para nome de 8 bytes.

    if ( name[0] == '\0' && ns <= 8 )
    {
        // The flag says that 
        // we're gonna have a new extension.
        fAddNewExt = TRUE;
        // Complete the DOS name with spaces. Until 8 bytes.
        goto CompleteName8WithSpaces;
    }

    //if ( name[0] == '.' && ns < 8 )

// Aqui name[0] é o ponto. 
// Mas o nome tem 8 bytes.
// Então constrói a extensão colocando as letras na extensão.

    for ( i=0; 
          i<3 && name[i+1]; 
          i++ )
    {
        // #ps: name[0] = '.'

        //Transforma uma letra da extensão em maiúscula.

        // Letras na extensao: 
        // Mudando as letras para maiusculo.
        if ( name[i+1] >= 'a' && 
             name[i+1] <= 'z' )
        {
            name[i+1] -= 0x20;   // Change
            ext[i] = name[i+1];  // Save
        }

        // Bytes especiais na extensao: 
        // Colocando os bytes, sem mudar.
        // Allowed in fat16: $ % ' - _ @ ~ ` ! ( ) { } ^ # &
                
        if ( name[i+1] == '$' ||
             name[i+1] == '%' ||
             name[i+1] == '-' ||
             name[i+1] == '_' ||
             name[i+1] == '(' ||
             name[i+1] == ')' ||
             name[i+1] == '{' ||
             name[i+1] == '}'  )
        {
            ext[i] = name[i+1];  // Save
        }

        // Se a extensao tiver numeros.
        if  ( name[i+1] >= '0' &&
              name[i+1] <= '9' ) 
        {
            ext[i] = name[i+1];  // Save
        }
        
        // E se tiver um 0x00 entre os bytes da extensao?
        // Mas a extensao tinha um ponto.
        if ( name[i+1] == 0x00 )
        {
            fAddNewExt = FALSE;
            goto CompleteName8WithSpaces;
        }
    };

// Acrescentamos ' ' até completarmos as oito letras do nome.
// Acrescentamos a extensão
// Finalizamos.

CompleteName8WithSpaces:

// Save some spaces in the name until 8 bytes.
    while (ns < 8)
    {
        *name++ = ' ';
        ns++;
    };

// Add extension.
AddExt:

// Prepare a new extension 
// if no extension was provided.
    if (fAddNewExt == TRUE)
    {
        // Pre-load the new extension bytes.
        // #warning:
        // fs_fntos2() do NOT add the .bin extension.
        ext[0] = ' ';  
        ext[1] = ' ';  
        ext[2] = ' ';  
        ext[3] = '\0';
    }

// Save extension bytes.
    for (i=0; i<3; i++)
    {
        // No caso de extensoes com 3 bytes
        // copiamos todos os bytes.
        if ( ext[i] != 0x00 )
        {
            *name++ = ext[i];  
        }
        
        // No caso das extensoes com menos bytes,
        // o buffer pode ter 0x00.
        // No lugar no 0x00 colocaremos ' '.
        if ( ext[i] == 0x00 )
        {
            *name++ = ' ';  
        }
    };

// #bugbug
// Is it an overflow?

    *name = '\0';
}


// Pega um fd na lista de arquivos do processo, dado o PID.
// Objects[i]
int fs_get_free_fd_from_pid (pid_t pid)
{
    register int __slot=0;
    struct process_d *p;

// Parameter
    if ( pid<0 || pid >= PROCESS_COUNT_MAX ){
        debug_print ("fs_get_free_fd_from_pid: pid\n");
        goto fail;
    }

// Process
    p = (struct process_d *) processList[pid];
    if ((void *) p == NULL){
        debug_print ("fs_get_free_fd_from_pid: p\n");
        goto fail;
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        debug_print ("fs_get_free_fd_from_pid: p validation\n");
        goto fail;
    }

// Pick a free one and return the index.
// Each element has a pointer for the file structure?
    for (__slot=0; __slot<32; __slot++)
    {
        if (p->Objects[__slot] == 0){
            return (int) __slot;
        }
    };

fail:
    return (int) -1;
}

/*
 * fs_initialize_process_cwd:
 *     Cada processo deve inicialiar seus dados aqui. 
 */
// #todo:
// handle return value ...
// What functions is calling us?
int fs_initialize_process_cwd ( pid_t pid, char *string )
{
    struct process_d *p;
    register int i=0;

// Parameters
    if (pid<0 || pid >= PROCESS_COUNT_MAX){
        debug_print ("fs_initialize_process_cwd: pid\n");
        return 1;
    }
    if ((void *) string == NULL){
        panic ("fs_initialize_process_cwd: string\n");
        //return 1;
    }
    if (*string == 0){
        panic ("fs_initialize_process_cwd: *string\n");
        //return 1;
    }

// Current process.
// #importante
// Vamos copiar a string para a estrutura do processo atual.

    p = (struct process_d *) processList[pid];
    if ((void *) p == NULL){
        panic ("fs_initialize_process_cwd: p\n");
    }
    if ( p->used != 1 || p->magic != 1234 ){
        panic ("fs_initialize_process_cwd: validation\n");
    }

// The buffer has 32 bytes.
// #bugbug: What is the string size?
// It needs to be less than 32.
    //size_t StringSize = (size_t) strlen(string);

    for (i=0; i<32; i++){
        p->cwd_string[i] = 0;
    };
    for (i=0; i<32; i++){
        p->cwd_string[i] = string[i];
    };
    p->cwd_string[31] = 0;  // Finalize 

    return 0;
}

/* 
 * fs_pathname_backup:
 *     Remove n nomes de diret�rio do pathname do processo indicado no 
 * argumento.
 *     Copia o nome para a string global.
 *     Remove the last N directories from PATH.  
 *     Do not leave a blank path.
 *     PATH must contain enough space for MAXPATHLEN characters. 
 *     #obs: O PID costuma ser do processo atual mesmo. 
 *     Credits: bash 1.05 
 */
// #todo: Describe 'n'.

void fs_pathname_backup ( pid_t pid, int n )
{
    struct process_d *p;
    register int i=0;

// CWD
    if (CWD.initialized != TRUE){
        printk ("fs_pathname_backup: CWD not initialized\n"); 
        return;
    } 

// Parameters
    if ( pid<0 || pid >= PROCESS_COUNT_MAX ){
        printk ("fs_pathname_backup: pid\n"); 
        return;
    }
    if (n <= 0) {
        return;
    }

// Process
    p = (struct process_d *) processList[pid];
    if ((void *) p == NULL){
        panic ("fsUpdateWorkingDiretoryString: p\n");
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        panic ("fsUpdateWorkingDiretoryString: validation\n");
    }

    char *path = (char *) p->cwd_string;

    register char *s = path + strlen(path);
 
    if (*path){ 
        s--;
    };

    while (n--)
    {
        while (*s == '/'){ s--; };
        while (*s != '/'){ s--; };

        *++s = '\0';
    };

// Atualizando a string global.
    for ( i=0; i<32; i++ ){
        CWD.path[i] = p->cwd_string[i];
    };

// Name
    for ( i=0; i< 11; i++ ){
        current_target_dir.name[i] = '\0';
    };
}

// Service 170: Print the cwd string of a given PID.
int fs_print_process_cwd(pid_t pid)
{
    struct process_d *p;

    //debug_print ("fs_print_process_cwd:\n");
    printk      ("fs_print_process_cwd:\n");

// Parameter
    if (pid<0 || pid>=PROCESS_COUNT_MAX){
        debug_print ("fs_print_process_cwd: pid\n");
        goto fail;
    }

// Process
    p = (struct process_d *) processList[pid];
    if ((void *) p == NULL){
        panic ("fs_print_process_cwd: p\n");
    }else{
        if ( p->used != 1 || p->magic != 1234 ){
            panic ("fs_print_process_cwd: validation\n");
        }

        // #bugbug
        // Is this element a pointer or a buffer ?
        // >>> This element is an array.
        
        // #bugbug: Size of string
        if ((void *) p->cwd_string != NULL)
        {
            //p->cwd_string[31] = 0;
            printk ("> PID=%d p->cwd_string {%s}\n", 
                p->pid, p->cwd_string);
        }

        // #bugbug
        // Is this element a pointer or a buffer ?

        if ( (void *) current_target_dir.name != NULL )
        {
            //current_target_dir.name[31] = 0;
            printk ("> PID=%d current_target_dir.name {%s}\n", 
                p->pid, current_target_dir.name);
        }

        //refresh_screen();
        return 0;
    };

fail:
    debug_print ("fs_print_process_cwd: fail\n");   
    return (int) -1;
}

void fs_show_file_info (file *f)
{
    size_t StringSize=0;

// Parameter
    if ((void*) f == NULL){
        debug_print("fs_show_file_info: fail\n");
        return;
    }
    if (f->used != TRUE)
        return;

    // #bugbug: What is the size?
    // StringSize = (size_t) strlen(f->_tmpfname);
    if ((void*) f->_tmpfname != NULL){
        printk ("Name={%s}\n",f->_tmpfname);
        //refresh_screen();
    }
}

void fs_show_file_table(void)
{
    file *f;
    register int i=0;

    printk ("\nfile_table:\n");
    
    for (i=0; i<32; i++)
    {
        f = (file*) file_table[i];
        if ((void*)f != NULL){
            fs_show_file_info(f);
        }
    };

    //refresh_screen();
}

void fs_show_inode_info (struct inode_d *i)
{

// Parameter
    if ((void *) i == NULL){
        debug_print("fs_show_inode_info: fail\n");
        return;
    }
    if (i->used != TRUE)
        return;

    if ((void*)i->path != NULL){
        printk ("Name={%s}\n",i->path);
    }
}

void fs_show_inode_table(void)
{
    struct inode_d *inode;
    register int i=0;

    printk ("inode_table:\n");
    for (i=0; i<32; ++i)
    {
        inode = (struct inode_d *) inode_table[i];
        if ((void *)inode != NULL)
        {
            if ( inode->used == 1 && inode->magic == 1234 )
            {
                fs_show_inode_info(inode);
            }
        }
    };
    //refresh_screen();
}

void fs_show_root_fs_info(void)
{
    //printk ("\n");
    printk ("fs_show_root_fs_info:\n");

// root fs structure.

    if ((void *) root == NULL){
        printk ("No root structure\n");
        goto fail;
    
    }else{

        if ( root->used != 1 || root->magic != 1234 ){
            printk ("Validation fail\n");
            goto fail;
        }

        printk ("name = %s \n",       root->name );
        printk ("Object type %d \n",  root->objectType );
        printk ("Object class %d \n", root->objectClass );
        printk ("type = %d \n",       root->type );
        printk ("Dir entries %d \n",  root->dir_entries );
        printk ("Entry size %d \n",   root->entry_size );
        // ...
        goto done;
    }; 

fail:
    printk("fail\n");
done:
    //refresh_screen();
    return;
}


/*
 * fsSaveFile:
 *     Salva um arquivo no disco.
 *     Somente no diretório raiz.
 */
// It was called by sys_write_file() in fs.c.
// It was called by sys_read_file when the file does not exist.
// #obs
// Isso salva um arquivo.
// Também poderia ser usado para criar um arquivo ou diretório ? 
// #todo: #test
// Número máximo de entradas na lista de clusters. 
// Quantas?
// A FAT tem 246 setores, 123 KB
// fsSaveFile:
//     Salva um arquivo. 
//     Onde? #todo vamos fornecer o endereço do diretorio.
// IN: 
// name, size in sectors, size in bytes, adress, flag. 
// OUT:
// #bugbug
// O nome nao esta ficando certo na entrada.
// #todo
// is dir_address virtual or physical?
// Change this name to dir_pa or dir_va.

int
fsSaveFile ( 
    unsigned long fat_address,
    unsigned long dir_address,
    int dir_entries,
    const char *file_name, 
    unsigned long file_size,   // number of sectors
    unsigned long size_in_bytes,
    char *file_address,
    char flag )  
{
// #todo
// For now we are using a local buffer to handle
// the target directory entry.
// Use the entry structure (fat16_directory_entry_d) found in fat.h.
// >>> Used to save/create a file or a directory,
// depending on the flag.

    int Status = 0;

    unsigned long i = 0; 
    unsigned long j = 0;    // Deslocamento na lista, tem que ser zero.
    unsigned long c = 0;    // Deslocamento na FAT.

    unsigned short first=0;
    unsigned short next=0;
    unsigned short sector=0;

    unsigned long MaxFileSizeInSectors = 16;

// #todo: Use the structure.
// Entry structure.
// see: fat.h
    //struct fat16_directory_entry_d entry_fat16;
// Directory entry buffer.
    char DirEntry[32];

//Entry size in words.
    int EntrySize = 0;
    int Offset = 0;
    int FreeIndex = -1;

    // #debug:
    debug_print ("fsSaveFile:\n");
    //printk      ("fsSaveFile:\n");

// Updating fat address and dir address.

    if (fat_address == 0){
        panic("fsSaveFile: fat_address\n");
    }
    if (dir_address == 0){
        panic("fsSaveFile: dir_address\n");
    }

// Ex: VOLUME1_FAT_ADDRESS
    unsigned short *fat = 
        (unsigned short *) fat_address;
// Ex: VOLUME1_ROOTDIR_ADDRESS
    unsigned short *__dir = 
        (unsigned short *) dir_address;

// #debug
// #todo
// We only support one address for now.

    if (fat_address != VOLUME1_FAT_ADDRESS)
    {
        //panic("fsSaveFile: [FIXME] We only support ONE fat address for now!\n");
        panic ("fsSaveFile: fat_address\n");
    }

// Filename

    if ((void *) file_name == NULL){
        debug_print("fsSaveFile: [ERROR] file_name\n");
        printk     ("fsSaveFile: [ERROR] file_name\n"); 
        goto fail;
    }
    if (*file_name == 0){
        debug_print("fsSaveFile: [ERROR] *file_name\n");
        printk     ("fsSaveFile: [ERROR] *file_name\n"); 
        goto fail;
    }
    // serial_printk ("fsSaveFile: %s\n", file_name);

// #bugbug
// Esse endereço eh valido?

/*
    printk ("name address = %x \n", &file_name );
    printk ("name    = %s \n", file_name ); 
    printk ("size    = %d \n", file_size );       // Size in sectors.
    printk ("nbytes  = %d \n", size_in_bytes );
    printk ("address = %x \n", file_address );
    printk ("flag    = %x \n", flag );
    printk ("entries = %d \n", dir_entries );
 */

// How name direntries this directory have?
    if ( dir_entries == 0 || 
         dir_entries > FAT16_ROOT_ENTRIES )
    {
        panic ("fsSaveFile: [FAIL] max dir entries");
    }

// file_size
// #todo: 
// precisamos implementar um limite para o tamanho do arquivo,
// principamente nessa fase de teste.

// #bugbug
// Limite provisorio
// Size in sectors.
    if (file_size > MaxFileSizeInSectors)
    {
        debug_print("fsSaveFile: [FIXME] Size in sectors\n");
        printk     ("fsSaveFile: [FIXME] Size in sectors = %d \n", 
            file_size ); 
        goto fail;
    }

// Load root dir and FAT.
// see: dev/blkdev/disk_r.c
    fs_load_rootdir( VOLUME1_ROOTDIR_ADDRESS, VOLUME1_ROOTDIR_LBA, 32 );
    fs_load_fat( VOLUME1_FAT_ADDRESS, VOLUME1_FAT_LBA, 246 );

// Procurando cluster livre na fat.
// Nesse momento construimos uma lista de clusters livres.
// #todo: 
// Essa lista já devia existir e agora somente 
// usaríamos.
// #todo: Essa rotina poderia seruma helper function?

//SearchEmptyEntries:
 
// #bugbug
// Obs: Esse limite é improvisado.
// see fatlib.h

    // nao podemos começar do 0.
    c = 3; 

    while (i < CLUSTERS_TO_SAVE_MAX)
    {
        //procurando cluster livre na fat.
        //@todo:isso poderia ser uma função.

        if (fat[c] == 0)
        {
            // number of sectors
            // Encontrado todos os espaços livres 
            // que o arquivo precisa.
            // Marca o fim.
            // #importante: 
            // Se der certo, saímos do loop.
            // #bugbug: Esse size deve ter um limite.
            // file_size = file size in sectors, (clusters??)
            if (file_size == 0){
                fat16ClustersToSave[j] = (unsigned short) 0xfff8; 
                goto save_file;
            }

            //salva um endereço livre
            //salvamos um índice na fat dentro da lista
            //incrementa a lista

            fat16ClustersToSave[j] = (unsigned short) c; 
            j++; 

            // Decrementa o tamanho do arquivo!
            file_size--; 
        };

        c++;    // Incrementa o deslocamento na fat. 
        i++;    // Incrementa a quantidade de busca.
    }; 

// Fail
// Nossa busca por clusters livres dentro da fat não deu certo.
// Provavelmente não encontramos uma quantidade suficiente.

out_of_range:  
    printk ("fsSaveFile: No free cluster \n");
    goto fail;

    // #debug
    //refresh_screen();
    //while(1){ asm("hlt"); }

// #importante:
// Deu certo. Encontramos na fat todos os clusters que o arquivo precisa.   
// Salva o arquivo.
// O arquivo tem na lista todos os clusters que ele precisa.

// Save!

save_file:
// saving file into the disk.
// But remember, we goota save the fat and the root into the disk.

    //#debug
    //printk("fsSaveFile: save_file: \n"); 
    //refresh_screen();

// Início da lista.
    i = 0; 
    first = i;

// Size limits.

// #bugbug
// Limite máximo improvisado.
// 2KB.

    j = (512*4);


// Create directory entry
// Name/ext 8.3
    DirEntry[0]  = (char) file_name[0];
    DirEntry[1]  = (char) file_name[1];
    DirEntry[2]  = (char) file_name[2];
    DirEntry[3]  = (char) file_name[3];
    DirEntry[4]  = (char) file_name[4];
    DirEntry[5]  = (char) file_name[5];
    DirEntry[6]  = (char) file_name[6];
    DirEntry[7]  = (char) file_name[7];
// extension.
    // Use extension or files.
    DirEntry[8]  = (char) file_name[8];
    DirEntry[9]  = (char) file_name[9];
    DirEntry[10] = (char) file_name[10];
    // No extension for directories. Using spaces. I guess. 
    if (flag == 0x10)
    {
        DirEntry[8]  = (char) 0x20;
        DirEntry[9]  = (char) 0x20;
        DirEntry[10] = (char) 0x20;
    }

    /*
    printk ("flag=%x \n",flag);
    printk ("%c\n",DirEntry[8]);
    printk ("%c\n",DirEntry[9]);
    printk ("%c\n",DirEntry[10]);
    */

// Attribute byte.
// 0x01: read only
// 0x02: hidden
// 0x04: system
// 0x08: volume label
// 0x10: >>>> Directory <<<<
// 0x20: >>>> Archive <<<<
// 0x40: Unused
// 0x80: Unused

    DirEntry[11] = (char) flag;

    // #todo
    //if (IsReadOnly)
    //    DirEntry[11] = (char) DirEntry[11] | 0x01;
    //if (IsHidden)
    //    DirEntry[11] = (char) DirEntry[11] | 0x02;
    //if (IsSystem)
    //    DirEntry[11] = (char) DirEntry[11] | 0x04;
    //if (IsVolumeLabel)
    //    DirEntry[11] = (char) DirEntry[11] | 0x08;


// Reserved
    DirEntry[12] = 0; 

// Creation time. 14 15 16
    DirEntry[13] = 0x97;  // Create Time (ms)
    //
    DirEntry[14] = 0xD3;  // Create Time (Hrs/Mins/Secs)
    DirEntry[15] = 0xBA;
// Creation date
    DirEntry[16] = 0xE2;
    DirEntry[17] = 0x58;
// Access date
    DirEntry[18] = 0xE2;
    DirEntry[19] = 0x58;

// First cluster. (16 bits)
// Only used in FAT32 Systems
// 0 para fat12 ou 16
    DirEntry[20] = 0;  // File/Folder Start Cluster (High)
    DirEntry[21] = 0;

// Modifield time
    DirEntry[22] = 0xD3;
    DirEntry[23] = 0xBA;
// Modifield date
    DirEntry[24] = 0xE2;
    DirEntry[25] = 0x58;

// #test
// First cluster. (Low word)
// The low 16 bits of this entry's first cluster number. 
// Use this number to find the first cluster for this entry.
// first = Index in the fat16ClustersToSave[] list.
   //int fIndex = first;  //=0.
    unsigned short OurFirstCluster = (unsigned short) fat16ClustersToSave[0];
    char *p = (char *) &OurFirstCluster;
    DirEntry[26] = (char) (*p);  // File/Folder Start Cluster (Low)
    p++;
    DirEntry[27] = (char) (*p);

// File size in bytes. (32 bits)
// 4 bytes: (28,29,30,31)
// (for files)
    DirEntry[28] = (char) size_in_bytes;
    size_in_bytes = (size_in_bytes >> 8);
    DirEntry[29] = (char) size_in_bytes;
    size_in_bytes = (size_in_bytes >> 8);
    DirEntry[30] = (char) size_in_bytes;
    size_in_bytes = (size_in_bytes >> 8);
    DirEntry[31] = (char) size_in_bytes;

// Folders will have a File Size of 0x0000
// (for folders)
    if (flag == 0x10)
    {
        DirEntry[28] = (char) 0;
        DirEntry[29] = (char) 0;
        DirEntry[30] = (char) 0;
        DirEntry[31] = (char) 0;
    }


// #importante:
// Vamos encontrar uma entrada livre no diretório para
// salvarmos o nome do arquivo.
// Copia o nome para dentro da entrada do diretório.
// Obs: As entradas são de 32 bytes. Como root[] é um 
// array de short então faremos um deslocamento de 16 shorts.
// root[]
// #importante: root[] é um array de short.	
// IN: 
// Endereço do diretótio e número máximo de entradas.
// #todo: 
// Talvez possamos ampliar esse número para o máximo 
// de entradas no diretório.
// #bugbug: A quantidade de entrada depende to diretório.
// See: search.c

// IN: 
// directory address, max number of entries.
    
    FreeIndex = 
        (int) findEmptyDirectoryEntry ( 
                  dir_address, 
                  dir_entries );

    if ( FreeIndex == -1 ){
        printk ("fsSaveFile: [FAIL] No empty entry\n");
        goto fail;
    }

//
// Write entry into the directory buffer.
//

// 32/2 = 16 words.
// Offset:
// Deslocamento dentro do diretório.
// representa o início da entrada que encontramos.
// Encontramos multiplicando o índice da entrada pelo 
// tamanho da entrada.
// Copy entry into the root in the memory.
// Copia 32 bytes.
 
    EntrySize = (FAT16_ENTRY_SIZE/2);
    Offset = (int) ( FreeIndex * EntrySize );

// FAT16_DIRENTRY_SIZE = 32
    memcpy ( &__dir[Offset], DirEntry, 32 );

// reset
// Reiniciamos o controlador antes de usarmos.

//resetIDE:

    //#debug
    //printk("fsSaveFile: reset and while\n"); 
    //refresh_screen();

    //reset_ide0 ();

// ## Save ##
// Vamos ao salvamento propriamente dito.

    i=0; 

//#debug 
//improvisando um endereço válido

    unsigned long address = (unsigned long) file_address;

// Save!  
// Saving the file into the disk.
// Cluster by cluster.

//SavingFile:

// Routine:
// +Pega um conteúdo da lista.
// +Encontrada a assinatura na lista!
// ...

// #bugbug
// E se o primeiro for um marcador de fim de arquivo?
    //next = list[0];
    //if (next == 0xFFF8)
        //what??

// Loop
// Pegamos o atual na lista.
// Se ele eh o sinalizador de fim de lista, 
// entao entao colocamos o sinalizador de fim de arquivo
// no offset indicado pelo penultimo elemento da lista.

    while (TRUE){

        next = fat16ClustersToSave[i];

        // #debug
        printk ("fsSaveFile: [DEBUG] next={%x}\n", next);

        // O next é o marcador de fim de lista.
        if (next == 0xFFF8){

            // Então pegamos o penúltimo e colocamos
            // o finalizador lá nesse offset.
            next = fat16ClustersToSave[i-1];
            fat[next] = 0xFFF8;
            goto do_save_dir_and_fat;

        // Se não é assinatura ainda.
        }else{

            // #bugbug
            // Estamos usando isso sem nem mesmo checar seu valor.
            // precisamos de limtes aqui. Para evitarmos overflow.
            
            // Grava na fat o endereço do próximo cluster
            fat[next] = fat16ClustersToSave[i+1];
 
            //#debug 
            //printk("write_lba\n");
            //refresh_screen();
            
            // Wait and write!
            
            // ata_get_current_ide_port_index()
            disk_ata_wait_irq(ATACurrentPort.g_current_ide_port_index);

            //grava - Aqui next esta certo!
            //write_lba ( (unsigned long) address, VOLUME1_DATAAREA_LBA + next -2 );
            ataWriteSector ( 
                (unsigned long) address, 
                (unsigned long) ( VOLUME1_DATAAREA_LBA + next -2), 
                0, 
                0 );

            address += 512;
        }; 

        //Próximo valor da lista.
        i++;

        // #bugbug
        // Limitando o tamanho do arquivo a 16 entradas.
        // Why??

        // #bugbug
        // Limite provisorio.
        if (i > 16){
            debug_print ("fsSaveFile: [FIXME] write sectors limit\n");
            printk      ("fsSaveFile: [FIXME] write sectors limit\n");
            goto fail;
        }

        // ??
    };

// FAIL
    debug_print ("fsSaveFile: Loop fail\n");
    goto fail;
   
//
// == done ========================================
//

// Saving rood dir and FAT.
// Nesse momento já salvamos os clusters do arquivo.
// OK. Funcionou no qemu.
// #bugbug
// Não vamos mais salvar a fat toda vez que salvarmos
// um arquivo.
// Vamos salvar a FAT apenas no fim da sessão.
// Como ainda não temos shutdown, então vamos salvar 
// quando chamarmos reboot.
// #important
// Updating the cache state.

do_save_dir_and_fat:

    //debug_print ("fsSaveFile: [DEBUG] do_save_dir_and_fat\n");
    //printk("fsSaveFile: do_save_dir_and_fat:\n"); 
    //refresh_screen();

// Save root
// #bugbug: We need to save a directory, not the root.
// IN: root dir address, root dir lba, root dir size in sectors.

    //if ( dir_address == ROO...
    fs_save_rootdir( VOLUME1_ROOTDIR_ADDRESS, VOLUME1_ROOTDIR_LBA, 32 );

// Sinalizando que o cache de fat precisa ser salvo.
    fs_fat16_cache_not_saved();

    // #debug
    //debug_print ("fsSaveFile: done\n");
    //printk      ("fsSaveFile: done\n");
    //refresh_screen();

    return 0;

fail:
    debug_print("fsSaveFile: Fail\n");
    printk     ("fsSaveFile: Fail\n");
    //refresh_screen ();
    return (int) 1;  // Why 1?
    //return -1;
}



/*
 * do_read_file_from_disk: 
 *     This is called by sys_open() and sys_read_file_from_disk().
 */
// Usada por open().
// Tem que retornar o fd e colocar o ponteiro na lista de arquivos abertos.
// Carrega um arquivo do disco para a memória.
// #bugbug
// Na minha maquina real, as vezes da problema no tamanho do arquivo.
// #bugbug
// Precisamos colocar os arquivos também na 
// lista global de arquivos abertos. file_table[]
// E na lista de inodes. inode_table[]
// #bugbug
// Nao seria o read() usado para ler um arquivo ja aberto?
// Sim, a rotina de suporte para read esta em sys_read e 
// nao chama esse worker aqui.
// #todo:
// Essa aqui poderia ter outro nome, pois ela carrega um arquivo
// poderia chamar-se loadxxxx().
// See: fs.c
// IN:
//   + #todo
// Worker
int 
do_read_file_from_disk ( 
    char *file_name, 
    int flags, 
    mode_t mode )
{
// Limits:
// Probably BUFSIZ (1024 for now) is the limit for this routine.
// To improve this we need to allocate a bigger buffer 
// in the file structure.

    int __ret = -1;
    int Status = -1;

// File pointer.
    file *fp;

    size_t FileSize = -1;
    struct process_d *p;
    int __slot = -1;  // o fd.
    void *buff;

// For now we're just able to get files and info
// in the root dir.
// Default: Root dir.
    unsigned long TargetDirAddress = VOLUME1_ROOTDIR_ADDRESS;
    unsigned long NumberOfEntries = FAT16_ROOT_ENTRIES;

// Parameter:
    if ((void*) file_name == NULL){
        return (int) (-EINVAL);
    }
    if (*file_name == 0){
        return (int) (-EINVAL);
    }

    // #debug
    // serial_printk ("do_read_file_from_disk: {%s}\n", file_name);


// -------------------------------------------
// Root dir?
    if (*file_name == '/')
    {
        TargetDirAddress = VOLUME1_ROOTDIR_ADDRESS;
        NumberOfEntries = FAT16_ROOT_ENTRIES;
        file_name++;
    }

// -------------------------------------------
// Reserved shortcut
    //if (*file_name == '!')
    //{}
// -------------------------------------------

// -------------------------------------------
// Is it inside the GRAMADO/ folder?
// This is not a new root directory for the whole system,
// it is only the directory where we're gonna serch
// for the the desired file.
// Remember: 
// We can setup the memory address for this directory 
// in the structure sdGRAMADO.
    if (*file_name == '@')
    {
        if (sdGRAMADO.initialized != TRUE){
            printk("do_read_file_from_disk: sdGRAMADO.initialized\n");
            goto fail;
        }
        if (sdGRAMADO.address == 0){
            printk("do_read_file_from_disk: sdGRAMADO.address\n");
            goto fail;
        }
        TargetDirAddress = sdGRAMADO.address;
        NumberOfEntries = FAT16_ROOT_ENTRIES;
        file_name++;
        goto EndOfShortcuts;
    }
// -------------------------------------------

// -------------------------------------------
// Is it inside the DE/ folder?
// This is not a new root directory for the whole system,
// it is only the directory where we're gonna serch
// for the the desired file.
// Remember: 
// We can setup the memory address for this directory 
// in the structure sdDE.

    if (*file_name == '#')
    {
        if (sdDE.initialized != TRUE){
            printk("do_read_file_from_disk: sdDE.initialized\n");
            goto fail;
        }
        if (sdDE.address == 0){
            printk("do_read_file_from_disk: sdDE.address\n");
            goto fail;
        }
        TargetDirAddress = sdDE.address;
        NumberOfEntries = FAT16_ROOT_ENTRIES;
        file_name++;
        goto EndOfShortcuts;
    }
// -------------------------------------------

// -------------------------------------------
// Reserved shortcut
    //if (*file_name == '$')
    //{}
// -------------------------------------------

// -------------------------------------------
// Reserved shortcut
    //if (*file_name == '%')
    //{}
// -------------------------------------------

EndOfShortcuts:

// Convertendo o formato do nome do arquivo.    
// >>> "12345678XYZ"
// #todo: 
// Não fazer isso em ring3.

// Add 'BIN' extension if we don't have one.
    //fs_fntos((char *) file_name);
// Add '   ' (3 spaces) extension if we don't have one.
    fs_fntos2((char *) file_name);

// #debug

    //printk ("do_read_file_from_disk: FILE={%s}\n",
    //    file_name);
    //refresh_screen();

// #bugbug
// We need to search in the inode list. inode_table[]
// If the file is found in the inode list, so we don't
// need to load it again, just increment the counter.

    // #todo
    // fs_search_inode_table(file_name);

// Searching for the file only on the root dir.
// Quando não existe, tentamos criar.
// #bugbug: Então 'cat' não deve chamar essa função.

    Status = (int) search_in_dir(file_name,TargetDirAddress);
// Found.
    if (Status == TRUE){
        goto __go;
    }

// Not found.
// Let's create it if the flags tell us to do that.
    if (Status != TRUE)
    {
         //#debug
        printk ("do_read_file_from_disk: [FIXME] File not found!\n");
        //refresh_screen();

        // Create a new one.
        // #todo: Use sys_create_empty_file.
        if (flags & O_CREAT)
        {
            debug_print ("do_read_file_from_disk: [O_CREAT] Creating a new file\n"); 

            // #todo:
            // Define the default value for this case.
            buff = (void*) kmalloc(BUFSIZ);
            if ((void*) buff == NULL)
            {
                printk("do_read_file_from_disk: buff\n");
                goto fail;
            }
            memset(buff,0,BUFSIZ);
            ksprintf(buff,"This is a new file.");

            //++
            // See: 
            __ret = 
                (int) fsSaveFile ( 
                          VOLUME1_FAT_ADDRESS, 
                           TargetDirAddress, 
                           NumberOfEntries, 
                           (char *) file_name, 
                           (unsigned long)  1,    // 1, size in sectors 
                           (unsigned long) 512,  // 512, size in bytes  
                           (char *) buff,         // buffer ?
                           (char) 0x20 );         // flag 
              //--

              // Ok
              if (__ret == 0){
                  debug_print("do_read_file_from_disk: Created new file\n");
                  //refresh_screen();
                  goto __go;
              }
         }

        printk("do_read_file_from_disk: [FIXME] Can't create new file\n");
        goto fail;
    }

__go:

// Process
    p = (struct process_d *) get_current_process_pointer();
    if ((void *) p == NULL){
        printk("do_read_file_from_disk: p\n");
        goto fail;
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        printk("do_read_file_from_disk: p validation\n");
        goto fail;
    }

// Procurando um slot livre.
    for (__slot=0; __slot<32; __slot++)
    {
        if (p->Objects[__slot] == 0){ goto __OK; }
    };

// fail
    //panic ("do_read_file_from_disk: No slots!\n");
    printk("do_read_file_from_disk: No slots!\n");
    goto fail;

// Slot found.
__OK:

    if ( __slot < 0 || __slot >= 32 ){
        printk ("do_read_file_from_disk: Slot fail\n");
        goto fail;
    }

// File struct
    fp = (file *) kmalloc(sizeof(file));
    if ((void *) fp == NULL){
        printk ("do_read_file_from_disk: fp\n");
        goto fail;
    }
    memset( fp, 0, sizeof(file) );

// Initialize.
    fp->used = TRUE;
    fp->magic = 1234;
    fp->pid = (pid_t) p->pid;  //current_process;
    fp->uid = (uid_t) current_user;
    fp->gid = (gid_t) current_group;

// #bugbug [FIXME]
// We need a type in read().

// #bugbug
// This function was called by sys_open, and open
// is able to open any kind of file.
// Why are we using this type here?

    fp->____object = ObjectTypeFile;

// ==================
// #todo #bubug
// Permissoes:
// As permissoes dependem do tipo de arquivo.

// #bugbug: Let's do this for normal files for now.
    fp->sync.can_read  = TRUE;
    fp->sync.can_write = TRUE;

    fp->sync.action = ACTION_NULL;
    // ==================

// #todo:
// We need to get the name in the inode.

    //fp->_tmpfname = NULL;

    fp->_fsize = 0;  // Isso é configurado logo abaixo.
    fp->_lbfsize = BUFSIZ;

// Inicializando apenas.
// #bugbug: Isso é provisório. 
// Caso contrário teremos problemas pra ler.
    fp->_r = 0;
    fp->_w = 0;
    fp->_cnt = BUFSIZ;  // Anda temos bastante espaço. todo o buffer

// #
// This is gonna be the return value.
    fp->_file = __slot;

    fp->fd_counter = 1;  //inicializando. 

// #todo
// Se ele não foi encontrado na lista de inodes
// e tivemos que carrega-lo do disco, então
// precisamos colocar ele na lista de inodes.. inode_table[]
// Atenção: O arquivo será carregado logo abaixo.

// #todo
// atualizar a tabela  global de arquivos. file_table[]

//
// buffer
//

// buffer padrão
// #bugbug: open chama isso. E se o arquivo for maior que o buffer ?
// open() precisa alocar outro buffer.

    fp->_base = (char *) kmalloc(BUFSIZ);
    if ((void *) fp->_base == NULL){
        printk ("do_read_file_from_disk: fp->_base\n");
        goto fail;
    }
    memset(fp->_base, 0, BUFSIZ);

    //#test provisório
    fp->_lbfsize = BUFSIZ;

// #debug
    //printk ("FILE_AGAIN={%s}\n",file_name);
    //refresh_screen();

// File size.
// #bugbug: 
// OUT: 'unsigned long' ?

    FileSize = 
        (size_t) fsGetFileSize( 
                      (unsigned char *) file_name,
                      (unsigned long) TargetDirAddress );

    if (FileSize <= 0){
        printk ("do_read_file_from_disk: FileSize\n");
        goto fail;
    }
// Structure field for file size.
    fp->_fsize = (int) FileSize;

// #bugbug
// #importante

// Limits.
    //if ( FileSize < fp->_lbfsize ){ 
    //    FileSize = fp->_lbfsize; 
    //}

// Limits.
// Se o arquivo for maior que buffer disponivel.
// Podemos aumentar o buffer.

    if (FileSize >= fp->_lbfsize)
    {
        // #debug
        printk("do_read_file_from_disk: [todo] File size out of limits\n");
        //printk("Size {%d}\n",FileSize);
        //goto fail;

        // #bugbug: Provisório.
        // Limite - 1MB.
        //if (FileSize > 1024*1024)
        if (FileSize > 8*1024)  //8KB
        {
            printk ("do_read_file_from_disk: File size out of limits\n");
            printk ("%d bytes \n",FileSize);
            //refresh_screen();
            return (-1);
        }
        
        // Allocate new buffer.
        // The buffer must to be bigger than the file size.
        size_t buflen = FileSize+8;
        // #bugbug:
        // We need a limit here.
        // The limit is 8KB. See above.
        //if (buflen > BUFSIZ)
            //return -1;
        fp->_base = (char *) kmalloc(buflen);
        if ((void *) fp->_base == NULL){
            printk ("do_read_file_from_disk: Couldn't create a new buffer\n");
            //refresh_screen();
            return -1;             
        }
        memset(fp->_base, 0, buflen);
 
        // Temos um novo buffer size.
        fp->_lbfsize = (int) buflen;
    }

// #paranoia.
// Checando os limites novamente.
// #bugbug: Provisório.
// Limits - 1MB
    //if (FileSize > 1024*1024)
    //{
    //    printk ("do_read_file_from_disk: File size out of limits\n");
    //    refresh_screen();
    //    return -1;
    //}

// #paranoia.
// Checando base novamente.

    if ((void *) fp->_base == NULL){
        printk("do_read_file_from_disk: fp->_base (again)\n");
        goto fail;
    }

//
// #todo (Unix-like)
// 

// What is the unix-like standard for this initialization?

// Pointer.
    fp->_p = fp->_base;

// Offsets
// Atualizando os offsets que foram apenas inicializados.

// #importante
// Não poderemos ler se r e w forem iguais.
// vamos ler do começo do arquivo.
    fp->_r = 0;

// #importante
// O ponteiro de escrita mudou 
// pois escrevemos um arquivo inteiro no buffer.
    //fp->_w = FileSize;
    fp->_w = fp->_fsize;
    
// #bugbug

    //if ( FileSize >= BUFSIZ )
    if (fp->_fsize >= fp->_lbfsize)
    {
        printk ("do_read_file_from_disk: File larger than buffer\n");
        //refresh_screen();
        fp->_r = fp->_lbfsize;
        fp->_w = fp->_lbfsize;
        fp->_cnt = 0;
    }

// Agora temos menos espaço no buffer.
    //fp->_cnt = ( BUFSIZ - FileSize );
    fp->_cnt = ( fp->_lbfsize - fp->_fsize );

// Load.
// Load the file into the memory.
    //printk("Load ....\n");
    Status = 
        (int) fsLoadFile ( 
                  VOLUME1_FAT_ADDRESS, 
                  TargetDirAddress, 
                  NumberOfEntries,  //#bugbug: Number of entries.
                  file_name, 
                  (unsigned long) fp->_base,
                  fp->_lbfsize );

    if (Status != 0){
        printk ("do_read_file_from_disk: fsLoadFile failed\n");
        goto fail;
    }
    //printk("Loaded ....\n");

// #bugbug
// Agora é a hora de atualizarmos as tabelas ....
// Depois de carregarmos o arquivo.

//
// Pointer. (mode)
//

// Ajusta o ponteiro para depois do tamanho do arquivo.
// Dependendo do modo.
// See:
// https://linux.die.net/man/2/open

           /*
           O_RDONLY        open for reading only
           O_WRONLY        open for writing only
           O_RDWR          open for reading and writing
           O_NONBLOCK      do not block on open or for data to become available
           O_APPEND        append on each write
           O_CREAT         create file if it does not exist
           O_TRUNC         truncate size to 0
           O_EXCL          error if O_CREAT and the file exists
           O_SHLOCK        atomically obtain a shared lock
           O_EXLOCK        atomically obtain an exclusive lock
           O_NOFOLLOW      do not follow symlinks
           O_SYMLINK       allow open of symlinks
           O_FSYNC         write will save into the disk. :)
           */

// Saving into de disk ?
// You can use the O_FSYNC open mode to make write always 
// store the data to disk before returning;    

     // Default ???

    //if (mode == 0)
    //{
        //debug_print ("do_read_file_from_disk: default mode\n");
        //fp->_p = fp->_base;
    //}

// Pointer
    fp->_p = fp->_base;

// Offsets
// Atualizando os offsets que foram apenas inicializados.

// #importante
// Não poderemos ler se r e w forem iguais.
// vamos ler do começo do arquivo.
    fp->_r = 0;

// #importante
// O ponteiro de escrita mudou 
// pois escrevemos um arquivo inteiro no buffer.
    //fp->_w = FileSize;
    fp->_w = fp->_fsize;

    fp->_cnt = ( fp->_lbfsize - fp->_fsize );

// The file is opened in append mode. 
// O offset fica no fim do arquivo.
    if (mode & O_APPEND){
        debug_print ("do_read_file_from_disk: O_APPEND\n");
        //fp->_p = fp->_base + s;
    }

    if (mode & O_ASYNC){
        debug_print ("do_read_file_from_disk: O_ASYNC\n");
    }

/* 
    // Enable the close-on-exec flag for the new file descriptor.
    if ( mode & O_CLOEXEC ){ 
         debug_print ("sys_read_file: O_CLOEXEC\n");
    }
 */

    if (mode & O_CREAT){
        debug_print ("do_read_file_from_disk: O_CREAT\n");
    }

// #importante
// Se não liberarmos para leitura então read()
// não poderá ler.

    // ok to read
    fp->_flags = (fp->_flags | __SRD);

    // ok to write
    //fp->_flags = (fp->_flags | __SWR);

// Salva o ponteiro de estrutura de arquivo.
// Ja checamos fd.
    p->Objects[__slot] = (unsigned long) fp;

    //#debug
    //printk ("process name: %s\n",p->__processname);
    //printk ("fd %d\n",fp->_file);
    //printk("do_read_file_from_disk-OUTPUT: %s \n",fp->_base);
    //refresh_screen();

done:
// Return fd. Called by open().
// #todo:
// Here is a good moment to validate the fp structure,
// not in the beginning of the routine.
    return (int) fp->_file;
fail:
    //refresh_screen();
    return (int) -1;
}

/*
 * do_write_file_to_disk:
 *     This is called by sys_write_file_to_disk().
 *     Interface para salvar arquivo ou diretório.
 *     Isso pode ser usado para criar um diretório ou 
 * copiar um diretório. 
 */
// #todo:
// vamos fazer igual ao sys_read_file 
// e criarmos opções ... se possível.
// IN: 
// name, size in sectors, size in bytes, adress, flag.

// Worker
int
do_write_file_to_disk ( 
    char *file_name, 
    unsigned long file_size,
    unsigned long size_in_bytes,
    char *file_address,
    char flag )  
{
    int __ret = -1;

// #todo:
// We can do the same we did in the Read routine,
// providing a method to access the DE/ GRAMADO/ folders.
    unsigned long TargetDirAddress = VOLUME1_ROOTDIR_ADDRESS;
    unsigned long NumberOfEntries = FAT16_ROOT_ENTRIES;

// Parameter.
    if ((void*) file_name == NULL){
        return (int) (-EINVAL);
    }
    if (*file_name == 0){
        return (int) (-EINVAL);
    }

    // #debug
    // serial_printk ("do_write_file_to_disk: {%s}\n", file_name);

// -------------------------------------------
// Root dir?
    if (*file_name == '/')
    {
        TargetDirAddress = VOLUME1_ROOTDIR_ADDRESS;
        NumberOfEntries = FAT16_ROOT_ENTRIES;
        file_name++;
    }

    // #todo
    // toupper
    // Shortcuts: ! @ # $ %.

//++
// See: sci/fs/write.c
    //taskswitch_lock ();
    //scheduler_lock ();

    __ret = 
        (int) fsSaveFile ( 
                  VOLUME1_FAT_ADDRESS, 
                  TargetDirAddress, 
                  NumberOfEntries,
                  (char *) file_name,
                  (unsigned long) file_size,
                  (unsigned long) size_in_bytes,
                  (char *) file_address,
                  (char) flag );

    //scheduler_unlock ();
    //taskswitch_unlock ();
//--

    return (int) __ret;
}

// #todo:
// Change parameter name "file". 
// Maybe we can use "fp" instead.
void set_global_open_file ( void *file, int Index )
{

// #todo:
// Limite maximo da lista.

// Structure
    if ((void *) file == NULL)
    {
        // ?? todo: message
        return;
    }

    if (Index < 0){
        // ?? todo: message
        return;
    }

// Include pointer in the list.
    file_table[Index] = (unsigned long) file;
}

// Get a pointer in file_table[i] given the index.
// Get file pointer?
// Can we use "FILE *" instead?
void *get_global_open_file (int Index)
{

//Limits.
//@todo: max. NUMBER_OF_FILES

    if (Index < 0){
        // ?? todo: message
        return NULL;
    }

    return (void *) file_table[Index];
}

// Clear the list 'dev_dir[i]'.
int fs_initialize_dev_dir(void)
{
    register int i=0;
    static int NumberOfSlots = 32;

    for (i=0; i<NumberOfSlots; i++)
    {
        dev_dir[i].used = FALSE;
        dev_dir[i].magic = FALSE;
        dev_dir[i].initialized = FALSE;
        dev_dir[i].path[0] = 0;
        dev_dir[i].fp = NULL;
    };
    return 0;
}

// Update the 'Current Working Directory' string.
// string:>
void fsInitializeWorkingDiretoryString (void)
{

    // #bugbug
    // We have some issues with string sizes.
    // Buffer overflow!

    // vamos contar o tamanho da string que estamos construindo.
    int string_count = 0;
    int string_size = 0;
    struct volume_d  *v;
    char volume_string[8];   

    //debug_print ("fsInitializeWorkingDiretoryString:\n");

// See: 
// fs.h

    CWD.initialized = FALSE;

// volume string 

    volume_string[0] = 'v';
    volume_string[1] = 'o';
    volume_string[2] = 'l';
    volume_string[3] = 'u';
    volume_string[4] = 'm';
    volume_string[5] = 'e';
    volume_string[6] = (char)( '1' + (char) current_volume - (char) 1 );
    volume_string[7] = '\0';

// 'root:'
//  ## volume list ##
// Primeiro colocamos a string que indica a lista de volumes. 
    ksprintf(CWD.path,FS_ROOT_STRING); 

// '/'
// ## separador ##
    strcat(CWD.path,FS_PATHNAME_SEPARATOR);

//
// volume root dir 
//

// #todo: Check overflow.
    if (current_volume < 0){
        panic ("fsInitializeWorkingDiretoryString: current_volume\n");
    }
    //if (current_volume >= ?)
        //something()

// Structure pointer
    v = (struct volume_d *) volumeList[current_volume];
    if ((void *) v == NULL){
        panic ("fsInitializeWorkingDiretoryString: v\n");
    }else{
        if ( v->used != TRUE || v->magic != 1234 ){
            panic ("fsInitializeWorkingDiretoryString: validation\n");
        }

        switch (v->id)
        {
			//case 0:
		        //global usada para string do nome do volume.
			//    current_volume_string = (char *) FS_VOLUME0_STRING;
			//    break;

			//case 1:
		        //global usada para string do nome do volume.
			//   current_volume_string = (char *) FS_VOLUME1_STRING;
			//   break;
			
            default:
		        //fail.
		        //printk("fsInitializeWorkingDiretoryString: default volume #todo\n");
		        //die();
                current_volume_string = (char *) volume_string; 
                break;
        };

        // #bugbug
        // We need to finalize the string.
        // limit 32.
        // See: volume.h
        
        // path string na estrutura do volume.
        
        string_size = sizeof(current_volume_string);
        if (string_size >= 32){
            debug_print ("fsInitializeWorkingDiretoryString: [FIXME] string size\n"); 
            return;
        }
        
        // copy the string. limit 32 bytes.
        ksprintf ( v->path_string, current_volume_string );
        
        // finalize.
        v->path_string[31] = 0;

        // #bugbug
        // What is the limit for this string ? 32 bytes.
        // See: rtl/fs/path.h and globals.h

        strcat ( CWD.path, v->path_string );
	    //strcat ( CWD.path, current_volume_string );
	    
        CWD.path[31] = 0;
    };

// #bugbug
// What is the limit for this string ? 32 bytes.
// See: path.h and globals.h

// Separador
    strcat ( CWD.path, FS_PATHNAME_SEPARATOR );
    CWD.path[31] = 0;

// Size
    size_t PathSize=0;
    PathSize = (size_t) strlen(CWD.path);
    if (PathSize > 31){
        PathSize = 31;
    }
    CWD.size = PathSize;

// More ?...

    //debug_print ("fsInitializeWorkingDiretoryString: done\n");

// See: fs.h
    CWD.initialized = TRUE;
}


// =========================================
// __fs_initialize_imp:
// Called by fsInitialize() in fs.c.
int __fs_initialize_imp (void)
{
    register int i=0;
    int slot = -1;

    PROGRESS("__fs_initialize_imp:\n");

// Initialize slab buffers to load directories.
    int buffers_ok = (int) __initialize_fs_buffers();
    if (buffers_ok < 0){
        panic("__fs_initialize_imp: buffers");
    }

// Undefined fs type.
    set_filesystem_type(FS_TYPE_NULL);

// Initialize fat16 support for the system's volume.
// #todo: 
// Devemos checar o tipo da partiçao de boot. 
// Se nao aqui, depois!
    fat16Init();

// Init dev/ dir.
// Is it a 'virtual folder'?
    fs_initialize_dev_dir();

//
// == fileList =========================
//

// Initializing the file pointers
// volume1_rootdir_fp and volume2_rootdir_fp.
// See: kstdio.h

//
// == volume1_rootdir ================================
//

    // pega slot em file_table[] para
    slot = get_free_slots_in_the_file_table();
    if (slot<0 || slot >= NUMBER_OF_FILES){
        panic("__fs_initialize_imp: slot\n");
    }
    volume1_rootdir_fp = (file *) file_table[slot];
    volume1_rootdir_fp->filetable_index = slot;

    if ((void *) volume1_rootdir_fp == NULL){
        panic ("__fs_initialize_imp: volume1_rootdir_fp\n");
    }

    volume1_rootdir_fp->used = TRUE;
    volume1_rootdir_fp->magic = 1234;
    volume1_rootdir_fp->____object = ObjectTypeVolume;

    volume1_rootdir_fp->_base = (unsigned char *) VOLUME1_ROOTDIR_ADDRESS;
    volume1_rootdir_fp->_p    = (unsigned char *) VOLUME1_ROOTDIR_ADDRESS;
    volume1_rootdir_fp->_cnt = (32 * 512);
    volume1_rootdir_fp->_file = 0;
    volume1_rootdir_fp->_tmpfname = "VOLUME1 VOL";
    volume1_rootdir_fp->fd_counter = 1;

// #bugbug: 
// Validade da estrutura.
        
// ...
// inode support.
// pega slot em inode_table[] 
    slot = get_free_slots_in_the_inode_table();
    if (slot < 0 || slot >= 32){
        panic("__fs_initialize_imp: volume1_rootdir_fp inode slot\n");
    }
    volume1_rootdir_fp->inode = (struct inode_d *) inode_table[slot];
    volume1_rootdir_fp->inodetable_index = slot;
    if ((void*) volume1_rootdir_fp->inode == NULL){
        panic("__fs_initialize_imp: volume1_rootdir_fp inode struct\n");
    }
    volume1_rootdir_fp->inode->filestruct_counter = 1;  //inicialize
    memcpy ( 
        (void*)       volume1_rootdir_fp->inode->path, 
        (const void*) volume1_rootdir_fp->_tmpfname, 
              sizeof( volume1_rootdir_fp->inode->path ) );
    // ... 

// #test
// The file pointer represents the boot volume.
// see: storage.h
    if ((void*) storage == NULL)
        panic("__fs_initialize_imp: storage\n");
    if (storage->magic != 1234)
        panic("__fs_initialize_imp: storage validation\n");

    storage->bootvolume_fp = (file *) volume1_rootdir_fp;

//
// == volume2_rootdir =========================================== 
//
    // pega slot em file_table[] para
    slot = get_free_slots_in_the_file_table();
    if (slot<0 || slot >= NUMBER_OF_FILES){
        panic("__fs_initialize_imp: slot\n");
    }
    volume2_rootdir_fp = (file *) file_table[slot];
    volume2_rootdir_fp->filetable_index = slot;

    if ((void *) volume2_rootdir_fp == NULL){
        panic ("__fs_initialize_imp: volume2_rootdir_fp\n");
    }
    volume2_rootdir_fp->used = TRUE;
    volume2_rootdir_fp->magic = 1234;
    volume2_rootdir_fp->____object = ObjectTypeVolume;
 
    volume2_rootdir_fp->_base = (unsigned char *) VOLUME2_ROOTDIR_ADDRESS;
    volume2_rootdir_fp->_p    = (unsigned char *) VOLUME2_ROOTDIR_ADDRESS;
    volume2_rootdir_fp->_cnt = (32 * 512);  // #bugbug: Check this size.
    volume2_rootdir_fp->_file = 0;          // ?
    volume2_rootdir_fp->_tmpfname = "VOLUME2 VOL";
    volume2_rootdir_fp->fd_counter = 1;

// ...
// inode support.
// pega slot em inode_table[] 
    slot = get_free_slots_in_the_inode_table();
    if (slot<0 || slot >= 32){
        panic("__fs_initialize_imp: volume2_rootdir inode slot\n");
    }
    volume2_rootdir_fp->inode = (struct inode_d *) inode_table[slot];
    volume2_rootdir_fp->inodetable_index = slot;
    if ( (void*) volume2_rootdir_fp->inode == NULL ){
        panic("__fs_initialize_imp: volume2_rootdir_fp inode struct");
    }
    volume2_rootdir_fp->inode->filestruct_counter = 1;  //inicialize
    memcpy ( 
        (void*)       volume2_rootdir_fp->inode->path, 
        (const void*) volume2_rootdir_fp->_tmpfname, 
              sizeof( volume2_rootdir_fp->inode->path ) );
        // ... 

//
// == pipe_gramadocore_init_execve ================================ 
//

// ## Inicializando os pipes usados em execve ## 
// gramado core init execve 
// #todo: mudar para pipe_gramadocore_init_execve_fp
// aloca memoria para a estrutura.
    
    pipe_gramadocore_init_execve = (file *) kmalloc(sizeof(file));
    if ((void *) pipe_gramadocore_init_execve == NULL){
        panic ("__fs_initialize_imp: pipe_gramadocore_init_execve\n");
    }

// Aloca memória para o buffer.
// #todo: Define this variable in the top of the body.
// #bugbug: Chech this size.
// #todo: Use BUFSIZ as a normal file.
    unsigned long pipe0base = (unsigned long) kmalloc(512);
    if ((void *) pipe0base == NULL){
        panic ("__fs_initialize_imp: pipe0base\n");
    }

    pipe_gramadocore_init_execve->used = TRUE;
    pipe_gramadocore_init_execve->magic = 1234;

    pipe_gramadocore_init_execve->_base = (unsigned char *) pipe0base;
    pipe_gramadocore_init_execve->_p    = (unsigned char *) pipe0base;
    pipe_gramadocore_init_execve->_cnt  = 512;  // #bugbug: Check this size.
    pipe_gramadocore_init_execve->_file = 0;    // ??
    pipe_gramadocore_init_execve->_tmpfname = "pipe0";

    // #todo
    //fileList[ ? ] = (unsigned long) pipe_gramadocore_init_execve;

    // 0
    pipeList[0] = (unsigned long) pipe_gramadocore_init_execve;

// -------------------------

    // #deprecated
    //init_directory_facilities();

// CWD Structure.
// Inicializa o pwd support.
    fsInitializeWorkingDiretoryString();

// Target dir struct
// Inicializa a estrutura de suporte ao target dir.
    fsInitTargetDir(VOLUME1_ROOTDIR_ADDRESS,"/");

// FAT support for the boot partition
    fsbp_initialize_fat();

// Initialize boot partitions canonical directories.
// see: fsbp.c
    fsbp_initialize_bp_directories();
    // ...

    PROGRESS("__fs_initialize_imp: Done\n");

    return 0;
}


//
// End
//

