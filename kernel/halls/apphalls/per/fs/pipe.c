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

    // size
    f1->_fsize = 0;
    f2->_fsize = 0;

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

    // Link both pointers #suspended
    //f1->link = f2;
    //f2->link = f1;

// Pipe info structure

    struct pipe_info_d *pi;
    pi = (struct pipe_info_d *) kmalloc( sizeof(struct pipe_info_d) );
    if ( (void *) pi == NULL ){
        goto fail;
    }
    // Initialize pipe info
    pi->base = sh_buff;
    pi->buffer_size = BUFSIZ;
    pi->counter = 0;
    pi->read_pos = 0;
    pi->write_pos = 0;

    f1->pipe_info = pi;
    f2->pipe_info = pi;

// OK
    //debug_print("sys_pipe: done\n");
    return 0;

fail:
    debug_print("sys_pipe: fail\n");
    return (int) (-1);
}

int file_read_pipe_buffer(file *f, char *buffer, int len)
{
    if (!f || f->____object != ObjectTypePipe) return -EBADF;
    if (f->used != TRUE || f->magic != 1234)   return -EBADF;
    if (!buffer || len <= 0)                   return -EINVAL;

    if ((void*) f->pipe_info == NULL)
        return -EBADF;

    int Count = 0;
    char *p = buffer;

    while (len > 0) {

        // Nothing to read?
        if (f->pipe_info->counter <= 0) 
        {
            f->pipe_info->counter = 0;
            break; // stop reading, pipe empty
        }

        // Wrap around
        if (f->pipe_info->read_pos >= f->pipe_info->buffer_size)
            f->pipe_info->read_pos = 0;

        // Copy one byte
        *p = f->pipe_info->base[ f->pipe_info->read_pos ];
        f->pipe_info->read_pos++;
        f->pipe_info->counter--;
        Count++;

        p++;
        len--;
    }

    f->_flags = __SWR;          // now writable
    f->sync.can_write = TRUE;

    if (Count > 0)
        printk("file_read_pipe_buffer: read %d bytes\n", Count);

    return Count;
}

int file_write_pipe_buffer(file *f, char *buffer, int len)
{
    if (!f || f->____object != ObjectTypePipe) return -EBADF;
    if (f->used != TRUE || f->magic != 1234)   return -EBADF;
    if (!buffer || len <= 0)                   return -EINVAL;

    if ((void*) f->pipe_info == NULL)
        return -EBADF;

    int Count = 0;
    char *p = buffer;

    while (len > 0) 
    {
        // Full?
        if (f->pipe_info->counter >= f->pipe_info->buffer_size)
        {
            break; // stop writing, pipe full
        }

        // Wrap around
        if (f->pipe_info->write_pos >= f->pipe_info->buffer_size)
            f->pipe_info->write_pos = 0;

        // Copy one byte
        f->pipe_info->base[ f->pipe_info->write_pos ] = *p;
        f->pipe_info->write_pos++;
        f->pipe_info->counter++;
        Count++;

        p++;
        len--;
    }

    f->_flags = __SRD;          // now readable
    f->sync.can_read = TRUE;

    if (Count > 0)
        printk("file_write_pipe_buffer: wrote %d bytes\n", Count);

    return Count;
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

