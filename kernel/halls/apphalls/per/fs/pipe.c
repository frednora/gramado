// fs/pipe.c
// Pipe support.
// 2019 -  Created by Fred Nora.

// A pipe uses the FILE structure.
// #todo: So this maybe we can link two pipes.
// This way the kernel can copy data when writing.

#include <kernel.h>

// Máximo que um pipe pode ser,
// quando não é super user.
unsigned int pipe_max_size = 4096;


static struct te_d *get_current_process_struct(void)
{
    pid_t current_process = (pid_t) get_current_process();
    struct te_d *Process = (void *) teList[current_process];

    if ((void *) Process == NULL)
        return NULL;
    if (Process->used != TRUE || Process->magic != 1234)
        return NULL;

    return Process;
}



int sys_dup(int oldfd)
{
    struct te_d *Process;
    file *fp;
    int newfd = -1;
    int i = 0;

    // Validate oldfd range
    if (oldfd < 0 || oldfd >= OPEN_MAX)
        return (int) (-EBADF);

    // Get current process
    Process = get_current_process_struct();
    if ((void *) Process == NULL)
        return (int) (-EBADF);

    // Get old file pointer
    fp = (file *) Process->Objects[oldfd];
    if ((void *) fp == NULL)
        return (int) (-EBADF);

    // Find a free slot
    for (i = 0; i < OPEN_MAX; i++) {
        if (Process->Objects[i] == 0) {
            newfd = i;
            break;
        }
    }

    if (newfd < 0)
        return (int) (-EMFILE);

    // Duplicate: point newfd to the same file*
    Process->Objects[newfd] = (unsigned long) fp;

    // Increment reference counter in the file structure
    fp->fd_counter++;

    return (int) newfd;
}

int sys_dup2(int oldfd, int newfd)
{
    struct te_d *Process;
    file *fp;

    // Validate ranges
    if (oldfd < 0 || oldfd >= OPEN_MAX ||
        newfd < 0 || newfd >= OPEN_MAX)
        return (int) (-EBADF);

    // Get current process
    Process = get_current_process_struct();
    if ((void *) Process == NULL)
        return (int) (-EBADF);

    // Get old file pointer
    fp = (file *) Process->Objects[oldfd];
    if ((void *) fp == NULL)
        return (int) (-EBADF);

    // POSIX: if oldfd == newfd, just return newfd
    if (oldfd == newfd)
        return (int) newfd;

    // If newfd is already open, close it first
    if (Process->Objects[newfd] != 0) 
    {
        file *old_newfp = (file *) Process->Objects[newfd];

        // Decrement reference count and free if needed
        if (old_newfp != NULL) {
            old_newfp->fd_counter--;
            if (old_newfp->fd_counter <= 0) {
                // last reference: close/free the file structure
                k_fclose(old_newfp);
            }
        }

        Process->Objects[newfd] = 0;
    }

    // Duplicate: point newfd to the same file*
    Process->Objects[newfd] = (unsigned long) fp;
    fp->fd_counter++;

    return (int) newfd;
}

int sys_dup3(int oldfd, int newfd, int flags)
{
    struct te_d *Process;
    file *fp;

    // Validate ranges
    if (oldfd < 0 || oldfd >= OPEN_MAX ||
        newfd < 0 || newfd >= OPEN_MAX)
        return (int) (-EBADF);

    // POSIX: dup3 fails if oldfd == newfd
    if (oldfd == newfd)
        return (int) (-EINVAL);

    // Get current process
    Process = get_current_process_struct();
    if ((void *) Process == NULL)
        return (int) (-EBADF);

    // Get old file pointer
    fp = (file *) Process->Objects[oldfd];
    if ((void *) fp == NULL)
        return (int) (-EBADF);

    // If newfd is already open, close it first
    if (Process->Objects[newfd] != 0) {
        file *old_newfp = (file *) Process->Objects[newfd];

        if (old_newfp != NULL) {
            old_newfp->fd_counter--;
            if (old_newfp->fd_counter <= 0) {
                k_fclose(old_newfp);
            }
        }

        Process->Objects[newfd] = 0;
    }

    // Duplicate: point newfd to the same file*
    Process->Objects[newfd] = (unsigned long) fp;
    fp->fd_counter++;

    /*
    // #todo
    // Handle O_CLOEXEC semantics if you want per‑FD CLOEXEC
    // You currently only have FILE-level flags (_flags).
    // If you later add a separate per-FD table, move this there.
    if (flags & O_CLOEXEC) {
        // This is a bit of a hack: CLOEXEC should be per FD, not per FILE.
        // But with current design, this at least gives you the semantic.
        fp->_flags |= FD_CLOEXEC;
    }
    */

    return (int) newfd;
}


/*
 * sys_pipe:
 * Create a pipe: two file descriptors that share the same buffer.
 * One end is for reading, the other for writing.
 * Service 247
 */
int sys_pipe(int *pipefd, int flags)
{
    file *f1;  // read end
    file *f2;  // write end
    struct te_d *Process;
    pid_t current_process = -1;
    register int i=0;
    int slot1 = -1;
    int slot2 = -1;

    debug_print ("sys_pipe:\n");

    // ------------------------------------------------------------
    // Optional: validate flags. For now only O_CLOEXEC is allowed.
    // This is commented out, but shows intent to reject unsupported flags.
    // ------------------------------------------------------------
    // if ((flags & O_CLOEXEC) != flags)
    //     return -EINVAL;

    // unsigned long fd_flags = (flags & O_CLOEXEC) ? FD_CLOEXEC : 0;

// Process
    current_process = (pid_t) get_current_process();
    Process = (void *) teList[current_process];
    if ((void *) Process == NULL){
        //debug_print("sys_pipe: Process\n");
        //todo printk
        goto fail;
    }
    if ( Process->used != TRUE || Process->magic != 1234 ){
        //debug_print("sys_pipe: validation\n");
        //todo printk
        goto fail;
    }

//#todo
//temos que criar uma rotina que procure slots em Process->Streams[]
//e colocarmos em process.c
//essa é afunção que estamos criando.
	// process_find_empty_stream_slot ( struct te_d *process );

// procurar 2 slots livres.

// #improvisando
// 0, 1, 2 são reservados para o fluxo padrão.
// Como ainda não temos rotinas par ao fluxo padrão,
// pode ser que peguemos os índices reservados.
// Para evitar, começaremos depois deles.

// ------------------------------------------------------------
// Find two free slots in the process's file descriptor table.
// Slots 0,1,2 are reserved (stdin, stdout, stderr).
// We start searching from slot 3 upwards.
// ------------------------------------------------------------

    // Reserva um slot.
    for (i=3; i<OPEN_MAX; i++)
    {
        if (Process->Objects[i] == 0)
        {
            //Process->Objects[i] = 216;
            slot1 = i;
            break;
        }
    };

    // Reserva um slot.
    for (i = (slot1+1); i<OPEN_MAX; i++)
    {
        if (Process->Objects[i] == 0)
        {
            //Process->Objects[i] = 216;
            slot2 = i;
            break;
        }
    };

// Check slots validation 
    if (slot1 == -1 || slot2 == -1)
    {
        debug_print("sys_pipe: slots alocation fail\n");
        goto fail;
    }

// ------------------------------------------------------------
// Allocate a shared buffer for the pipe.
// Both ends will point to the same memory region.
// ------------------------------------------------------------

    char *sh_buff = (char *) kmalloc(BUFSIZ);
    if ((void *) sh_buff == NULL)
    {
        Process->Objects[slot1] = (unsigned long) 0;
        Process->Objects[slot2] = (unsigned long) 0;
        debug_print("sys_pipe: sh_buff\n");
        goto fail;
    }

// ------------------------------------------------------------
// Allocate two file structures (f1 and f2).
// Each represents one end of the pipe.
// ------------------------------------------------------------

// File structures 
    f1 = (void *) kmalloc(sizeof(file));
    f2 = (void *) kmalloc(sizeof(file));
    if ( (void *) f1 == NULL || 
         (void *) f2 == NULL )
    {
        Process->Objects[slot1] = (unsigned long) 0;
        Process->Objects[slot2] = (unsigned long) 0;
        debug_print("sys_pipe: structures fail\n");
        goto fail;
    }

// Initialize file structures

// Early validations?

    f1->used = TRUE;
    f1->magic = 1234;

    f2->used = TRUE;
    f2->magic = 1234;

// As duas estruturas compartilham o mesmo buffer.        

// File: object type.
    f1->____object = ObjectTypePipe;
    f2->____object = ObjectTypePipe;

// Associate with current process credentials
// pid, uid, gid.
    f1->pid = (pid_t) current_process;
    f1->uid = (uid_t) current_user;
    f1->gid = (gid_t) current_group;
    f2->pid = (pid_t) current_process;
    f2->uid = (uid_t) current_user;
    f2->gid = (gid_t) current_group;

// full duplex ?

// sync: 
    f1->sync.sender_pid = (pid_t) -1;
    f1->sync.receiver_pid = (pid_t) -1;
    f1->sync.action = ACTION_NULL;

// ------------------------------------------------------------
// Synchronization flags: here both ends are marked as
// readable and writable, but ideally one should be read‑only
// and the other write‑only.
// ------------------------------------------------------------
// #todo
// f1: read end (can_read = TRUE, can_write = FALSE)
// f2: write end (can_read = FALSE, can_write = TRUE)
    f1->sync.can_read = TRUE;
    f1->sync.can_write = TRUE;

    f1->sync.can_execute = FALSE;
    f1->sync.can_accept = FALSE;
    f1->sync.can_connect = FALSE;

// sync:
    f2->sync.sender_pid = (pid_t) -1;
    f2->sync.receiver_pid = (pid_t) -1;
    f2->sync.action = ACTION_NULL;

// ------------------------------------------------------------
// Synchronization flags: here both ends are marked as
// readable and writable, but ideally one should be read‑only
// and the other write‑only.
// ------------------------------------------------------------
// #todo
// f1: read end (can_read = TRUE, can_write = FALSE)
// f2: write end (can_read = FALSE, can_write = TRUE)
    f2->sync.can_read = TRUE;
    f2->sync.can_write = TRUE;

    f2->sync.can_execute = FALSE;
    f2->sync.can_accept = FALSE;
    f2->sync.can_connect = FALSE;

// No filename (anonymous pipe)
// No name for now.
    f1->_tmpfname = NULL;
    f2->_tmpfname = NULL;

// Both ends share the same buffer
    f1->_base = sh_buff;
    f2->_base = sh_buff;
    f1->_p    = sh_buff;
    f2->_p    = sh_buff;

// Buffer size.
    f1->_lbfsize = BUFSIZ; 
    f2->_lbfsize = BUFSIZ;

// Counters and offsets

// Quanto falta.
    f1->_cnt = f1->_lbfsize;   
    f2->_cnt = f2->_lbfsize; 

// Offsets
    f1->_r = 0;
    f2->_r = 0;
    f1->_w = 0;
    f2->_w = 0;

// fd
    f1->_file = slot1;
    f2->_file = slot2;

// Save file structures into process FD table
    Process->Objects[slot1] = (unsigned long) f1;
    Process->Objects[slot2] = (unsigned long) f2;


// ------------------------------------------------------------
// Return values: pipefd[0] is the read end, pipefd[1] is the write end.
// ------------------------------------------------------------

// #importante
// Esse é o retorno esperado.
// Esses índices representam 
// o número do slot na lista de arquivos abertos 
// na estrutura do processo atual.

// Return
    pipefd[0] = slot1;
    pipefd[1] = slot2; 

    //#debug
    //printk ("sys_pipe: %d %d\n",slot1,slot2);

// OK
    debug_print("sys_pipe: done\n");
    return 0;

fail:
    debug_print("sys_pipe: fail\n");
    return (int) (-1);
}

// Read from pipe buffer
int file_read_pipe_buffer( file *f, char *buffer, int len )
{

    char *p;
    int Count=0;

    p = buffer;
// #test
    Count = (int) (len & 0xFFFF);

// Check file
    if ((void *) f == NULL){
        printk ("file_read_pipe_buffer: f\n");
        goto fail;
    }
    if ( f->used != TRUE || f->magic != 1234 ){
        printk ("file_read_pipe_buffer: f validation\n");
        goto fail;
    }
// Check buffer
    if ((void *) p == NULL){
        printk ("file_read_pipe_buffer: p\n");
        goto fail;
    }
// nada para ler.
    if (Count <= 0){
        printk ("file_read_pipe_buffer: Count <= 0\n");
        goto fail;
    }
// Chech len
// #bugbug: Isso é provisório
// A quantidade que desejamos ler é menor que o tamanho do buffer.
// Estamos lendo do início do arquivo?
    if (Count > f->_lbfsize){
        printk ("file_read_pipe_buffer: Count > f->_lbfsize\n");
        goto fail;
    }

    //if ( Count > f->_fsize ){
        //printk ("file_read_pipe_buffer: Count > f->_fsize\n");
    //    Count = f->_fsize;
    //    goto fail;
    //}

/*
// stdin
    if( f->_file == 0 )
    {
        if( f->_lbfsize != PROMPT_SIZE)
        {
           printk ("file_read_pipe_buffer: [FAIL] Wrong size for stdin _lbfsize\n");
           goto fail;
        }
    }
 */

    if (f->____object != ObjectTypePipe)
        return -1;

    // Copy from pipe buffer to user buffer

    if ( f->____object == ObjectTypePipe )
    {
        // Nothing to read
        if (f->_fsize <= 0){
            return 0;
        }

        // Se o buffer tem tamanho 0.
        if (f->_lbfsize <= 0){
            printk ("file_read_pipe_buffer: _lbfsize is 0\n");
            goto fail;
        }

        // Se o tamanho do buffer for maior que o padrão.
        // #todo: O buffer pdoerá ser maior que isso no futuro.
        //if ( f->_lbfsize > BUFSIZ ){
        //    printk ("file_read_pipe_buffer: _lbfsize\n");
        //    goto fail;
        //}

        // #test: Limite provisorio
        if (f->_lbfsize > (8*1024)){
            printk ("file_read_pipe_buffer: _lbfsize bigger than 8KB\n");
            goto fail;
        }

        // Não podemos ler antes do início do arquivo.
        if ( f->_r < 0 ){
            f->_r = 0;
            printk ("file_read_pipe_buffer: f->_r = 0\n");
            goto fail;
        }

        // Nao leremos depois do fim do arquivo.
        if ( f->_r >= BUFSIZ )
        {
            //#debug: provisorio
            //printk ("file_read_pipe_buffer: f->_r > f->_lbfsize\n");
            //goto fail;
            //debug_print("file_read_pipe_buffer: f->_r > f->_lbfsize\n");
            //f->_r = f->_lbfsize;
            //f->_w = f->_lbfsize;
            //f->_p = (f->_base + f->_lbfsize);
            //f->_cnt = 0;
            //return EOF;

            f->_r = 0;
        }

        // Empty pipe
        // Nothing to read
        // No problem for the first read.
        if (f->_r == f->_w)
        {
            return 0;
        }

        // Se o offset de leitura for maior que
        // o offset de escrita, então temos que esperar.
        // #bugbug: mas talvez isso não seja assim para pipe.
        /*
        if (f->_r >= f->_w)
        {
            // EOF
            //printk ("file_read_pipe_buffer: #debug f->_r > f->_w\n");

            f->_r = f->_w;

            //f->_p = (f->_base + f->_r);
            // You also can write now.
            // But i can still read.
            //f->_flags = __SWR;
            return 0;
        }
        */

        // Se a quantidade que desejamos ler
        // é maior que o espaço que temos.
        // # Isso ja foi feito logo acima.
        /*
        if (Count > f->_lbfsize)
        {
            //printk ("file_read_pipe_buffer: Count > f->_lbfsize\n");
            //goto fail;

            //printk ("file_read_pipe_buffer: [FAIL] local_len limits\n");
            //goto fail;
        
            //#test #bugbug
            // leia tudo então. hahaha
            Count = (f->_lbfsize - 1);
        }
        */

        // Se o tanto que queremos ler é maior
        // que o que nos resta da buffer,
        // então vamos ler apenas o resto do buffer.

        // #bugbug: Isso esta errado. #delete

        // So podemos ler ate limite de bytes disponíveis 
        // no buffer.
        //if (Count > f->_cnt)
        //{
            //printk ("file_read_pipe_buffer: local_len > f->_cnt\n");
            //goto fail;
            //Count = f->_cnt;
        //}
 
        /*
        // 
        int delta = (f->_w - f->_r);

        // nada para ler.
        // pois o ponteiro de escrita e o de leitura sao iguais,
        if (delta == 0){
            // 0 bytes lidos
            //printk ("delta=0\n");
            return 0;
        }
        */
 
        // #delta
        // Se o tanto que queremos ler
        // é maior que o tanto que foi efetivamente escrito,
        // então leremos somente o que foi escrito.
        // se a diferença entra o ponteiro de escrita e o ponteiro
        // de leitura for menor que a quantidade que queremos ler.
        // Se queremos ler mais do que foi escrito.
        // entao vamos ler apenas o que foi escrito.
        /*
        if (Count > delta){
            Count = delta;
        }
        */

        // Atualizando o ponteiro de trabalho.
        // Vamos ler daqui.
        // A partir do offset de leitura.
        f->_p = (f->_base + f->_r);

        // read

        //#debug
        if (Count <= 0){
            printk ("file_read_pipe_buffer: Count <= 0 SECOND\n");
            goto fail;
            //printk("local_len\n");
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

        /*
        if ( f->_r > f->_w ){
            f->_r = f->_w;
        }
        */

        f->_fsize -= Count;

        // You also can write now.
        // But i can still read.
        f->_flags |= __SWR;
        f->sync.can_write = TRUE;

        return (int) Count;
    }

fail:
    return EOF;
}

// Write to pipe buffer
int file_write_pipe_buffer( file *f, char *buffer, int len )
{
    char *p;

// #todo:
// p = buffer; → but you never use p (you use buffer directly in memcpy)
    p = buffer;

    // debug_print ("file_write_pipe_buffer:\n");

// File validation
    if ((void *) f == NULL){
        printk ("file_write_pipe_buffer: f\n");
        goto fail;
    }
    // #todo: Check used and magic.
    //if (f->magic != 1234)
        //return -1;

    if ((void *) p == NULL){
        printk ("file_write_pipe_buffer: p\n");
        goto fail;
    }

// Tentando escrever mais do que cabe no arquivo.
    if (len >= BUFSIZ){
        printk ("file_write_pipe_buffer: len >= BUFSIZ\n");
        goto fail;
    }

//
// Copy
//
    if (f->____object != ObjectTypePipe)
        return -1;

    if ( f->____object == ObjectTypePipe )
    {
        // #bugbug
        // Temos que ter um limite aqui ... !!!
        // #todo

        // Full. block it.
        if (f->_fsize >= f->_lbfsize){
            return -EAGAIN; // full
        }

        // se o tamanho do buffer for maior que o padrao.
        if (f->_lbfsize > BUFSIZ){
            printk ("file_write_pipe_buffer: _lbfsize\n");
            goto fail;
        }

        // Come back to the beginning of the buffer.
        if (f->_w >= BUFSIZ){
            f->_w = 0;
        }

        // Buffer full, block it.
        // No problems at first write.
        if (f->_w == f->_r)
        {
            if (f->_fsize != 0)
                return 0;
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
        /*
        if (f->_w >= BUFSIZ)
        {
            //#bugbug
            debug_print("file_write_pipe_buffer: f->_w >= BUFSIZ\n");
            printk     ("file_write_pipe_buffer: f->_w >= BUFSIZ\n");
            f->_w = BUFSIZ;
            f->_cnt = 0;
            return EOF;
        }
        */

        // recalculando quanto espaço temos.
        //f->_cnt = (f->_lbfsize - f->_w);

        // Se a quantidade que temos ultrapassa os limites.

        // fim do arquivo.
        /*
        if (f->_cnt < 0)
        {
            f->_cnt = 0;
            f->_w = f->_lbfsize;
            f->_r = f->_lbfsize;
            f->_p = (f->_base + f->_w);
            return EOF;
        }
        */

        // Inicio do arquivo
        /*
        if (f->_cnt > f->_lbfsize)
        {
            printk ("file_write_pipe_buffer: _cnt\n");
            f->_cnt = f->_lbfsize;
            f->_p = f->_base;
            f->_w = 0;
            f->_r = 0;
        }
        */

        if (len < 0){
            return (int) -1;
        }

        /*
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
                return (int) -1;
            }

            // Só podemos escrever esse tanto.
            if ( f->_cnt > 0 ){
                len = f->_cnt; 
            }
        }
        */

        // write.
        // Se o offset de escrita é menor que o offset de leitura.
        // Então adiantaremos o offset de escrita.
        // pois nao devemos tocar no offset de leitura.
        if (f->_w < f->_r)
        {
            f->_w = f->_r;
            f->_cnt = ( f->_lbfsize - f->_w );
        }

        // Update the pointer
        f->_p = (f->_base + f->_w);

        // Write it, using the pointer
        memcpy( (void *) f->_p, (const void *) buffer, len ); 
    
        // Update the pointer
        f->_p = (f->_p + len);

        // Update the offset for writing
        f->_w = (f->_w + len);
    
        // Atualizamos o quanto nos falta.
        //f->_cnt = (f->_cnt - len);
        f->_fsize += len;

        // You can read now.
        f->_flags = __SRD;
        f->sync.can_read = TRUE;

        //debug_print ("file_write_pipe_buffer: ok, done\n");

        // Retornamos a quantidade escrita no buffer.
        return (int) len;
    }

// Unknown type
fail:
    //printk ("file_write_pipe_buffer: fail\n");
    return EOF;
}


// Quick pipe read: just call the worker
int sys_read_pipe(int fd, char *ubuf, int count)
{
    debug_print("sys_read_pipe: TODO\n");

    // --- Parameter validation ---
    if (fd < 0 || fd >= OPEN_MAX) {
        return (int)(-EBADF);   // invalid file descriptor
    }
    if ((void*) ubuf == NULL) {
        return (int)(-EINVAL);  // invalid buffer pointer
    }
    if (count <= 0) {
        return -1;              // nothing to read
    }

    // --- Lookup file structure ---
    file *fp = (file *) get_file_from_fd(fd);
    if (!fp)
        return -EBADF;


    // #backup: Old implementation
    // For ObjectTypePipe, file_read_buffer() just memcpy’s from f->_base
    // return (ssize_t) file_read_buffer(fp, ubuf, count);

    // #test: New implementation
    return (ssize_t) file_read_pipe_buffer(fp, ubuf, count);
}

// Quick pipe write: just call the worker
int sys_write_pipe(int fd, char *ubuf, int count)
{
    debug_print("sys_write_pipe: TODO\n");

    // --- Parameter validation ---
    if (fd < 0 || fd >= OPEN_MAX) {
        return (int)(-EBADF);   // invalid file descriptor
    }
    if ((void*) ubuf == NULL) {
        return (int)(-EINVAL);  // invalid buffer pointer
    }
    if (count <= 0) {
        return -1;              // nothing to write
    }

    // --- Lookup file structure ---
    file *fp = (file *) get_file_from_fd(fd);
    if (!fp)
        return -EBADF;


    // #backup: Old implementation
    // For ObjectTypePipe, file_write_buffer() just memcpy’s into f->_base
    // return (ssize_t) file_write_buffer(fp, ubuf, count);

    // #test: New implementation
    return (ssize_t) file_write_pipe_buffer(fp, ubuf, count);
}

// The pipe is created with buffer in form of
// packets.
// So read will read one packet at time.
/*
int is_packetized(struct file *file);
int is_packetized(struct file *file)
{
    return (file->_flags & O_DIRECT) != 0;
}
*/

// #todo: Who calls this worker.
int 
pipe_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    debug_print("pipe_ioctl: #todo\n");

// Parameter:
    if (fd<0 || fd>=OPEN_MAX)
    {
        return (int) (-EBADF);
    }

    switch (request){
    // ...
    default:
        debug_print("pipe_ioctl: [FAIL] default\n");
        break;
    };

//fail:
    return (int) -1;
}

//
// End
//

