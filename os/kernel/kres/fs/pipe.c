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

// Duplicate a file stream.
// #todo
// Here is not the place for this function.
// Move it to rtl.

int sys_dup(int oldfd)
{
    file *f_old;
    file *f_new;
    struct process_d *Process;
    pid_t current_process=-1;
    register int i=0;
    int slot = -1;

// Parameters
    if ( oldfd < 0 || oldfd >= OPEN_MAX ){
        return (int) (-EINVAL);
    }

// Process
    current_process = (pid_t) get_current_process();
    Process = (void *) processList[current_process];
    if ((void *) Process == NULL){
        //debug_print("sys_dup: [FAIL]\n");
        goto fail;
    }
    if ( Process->used != TRUE || Process->magic != 1234 ){
        //debug_print("sys_dup: [FAIL]\n");
        goto fail;
    }

// Get an empty slot.
    for ( i=3; i< NUMBER_OF_FILES; i++ )
    {
        if ( Process->Objects[i] == 0 )
        {
            // reserva.
            //Process->Objects[i] = 216;
            slot = i;
            break;
        }
    };

    // fail
    if (slot == -1)
    {
        Process->Objects[i] = (unsigned long) 0;
        // #todo
        // We need a message here.
        goto fail;
    }

// #todo: 
// Filtrar oldfd

// Old
    f_old = (file *) Process->Objects[oldfd];
    if ((void *) f_old == NULL){
        Process->Objects[i] = (unsigned long) 0;
        goto fail;
    }

// New
    f_new = (void *) kmalloc( sizeof(file) );
    if ((void *) f_new == NULL){
        Process->Objects[i] = (unsigned long) 0;
        // #todo
        // We need a message here?
        goto fail;
    }
// Early validation?
    //f_new->used = TRUE;
    //f_new->magic = 1234;

// Herdando
// So this way they are sharing the same ring0 buffer.
    f_new->____object = f_old->____object;
    f_new->_base      = f_old->_base;
    f_new->_p         = f_old->_p;
    f_new->_tmpfname  = f_old->_tmpfname;
    f_new->_lbfsize   = f_old->_lbfsize; 
    f_new->_cnt       = f_old->_cnt; 
    f_new->used = TRUE;
    f_new->magic = 1234;
    Process->Objects[slot] = (unsigned long) f_new;
    return (int) slot;

// On success, these system calls return the new file descriptor.  
// On error, -1 is returned, and errno is set appropriately.
fail:
    //errno = ?;
    return (int) -1;
 }

// #todo
// Here is not the place for this function.
// Move it to rtl.
int sys_dup2(int oldfd, int newfd)
{
    file *f_old;
    file *f_new;
    struct process_d *Process;
    pid_t current_process = -1;

    if ( oldfd < 0 || oldfd >= OPEN_MAX )
        return (int) (-EINVAL);

    if ( newfd < 0 || newfd >= OPEN_MAX )
        return (int) (-EINVAL);

// Process
    current_process = (pid_t) get_current_process();
    Process = (void *) processList[current_process];
    if ((void *) Process == NULL){
        //#todo: We need a message here.
        goto fail;
    }
    if ( Process->used != TRUE || Process->magic != 1234 )
    {
        goto fail;
    }

    int slot = newfd;
    if ( slot == -1 ){
        Process->Objects[slot] = (unsigned long) 0;
        goto fail;
    }

// #todo: 
// filtrar oldfd

// Old
    f_old = (file *) Process->Objects[oldfd];
    if ((void *) f_old == NULL){
        Process->Objects[slot] = (unsigned long) 0;
        goto fail;
    }

// New
    f_new = (file *) Process->Objects[slot];
    if ((void *) f_new == NULL){
        Process->Objects[slot] = (unsigned long) 0;
        goto fail;
    }
    //f_new->used = TRUE;
    //f_new->magic = 1234;

// Herdado
// Sharing the same ring 0 buffer.
    f_new->____object = f_old->____object;
    f_new->_base      = f_old->_base;
    f_new->_p         = f_old->_p;
    f_new->_tmpfname  = f_old->_tmpfname;
    f_new->_lbfsize   = f_old->_lbfsize; 
    f_new->_cnt       = f_old->_cnt; 
    f_new->used = TRUE;
    f_new->magic = 1234;
    return (int) slot;

// On success, these system calls return the new file descriptor.  
// On error, -1 is returned, and errno is set appropriately.

fail:
	//errno = ?;
    return (int) -1;
}

// #todo
// Here is not the place for this function.
// Move it to rtl.

int sys_dup3(int oldfd, int newfd, int flags)
{
    file  *f_old;
    file  *f_new;
    struct process_d *Process;
    pid_t current_process = -1;

    //#todo: flags.

    if ( oldfd < 0 || oldfd >= OPEN_MAX )
        return (int) (-EINVAL);

    if ( newfd < 0 || newfd >= OPEN_MAX )
        return (int) (-EINVAL);

// Process
    current_process = (pid_t) get_current_process();
    Process = (void *) processList[current_process];
    if ((void *) Process == NULL){
        goto fail;
    }
    if ( Process->used != TRUE || Process->magic != 1234 )
    {
        goto fail;
    }

    int slot = newfd;
    if ( slot == -1 ) {
        Process->Objects[slot] = (unsigned long) 0;
        goto fail;
    }

// #todo: filtrar oldfd

// Old
    f_old = (file *) Process->Objects[oldfd];
    if ((void *) f_old == NULL){
        Process->Objects[slot] = (unsigned long) 0;
        goto fail;
    }

// New
    f_new = (file *) Process->Objects[slot];
    if ((void *) f_new == NULL){
        Process->Objects[slot] = (unsigned long) 0;
        goto fail;
    }
    //f_new->used = TRUE;
    //f_new->magic = 1234;

// Herdado
// Shearing the same ring 0 buffer.
    f_new->____object = f_old->____object;
    f_new->_base      = f_old->_base;
    f_new->_p         = f_old->_p;
    f_new->_tmpfname  = f_old->_tmpfname;
    f_new->_lbfsize   = f_old->_lbfsize; 
    f_new->_cnt       = f_old->_cnt; 
    f_new->used = TRUE;
    f_new->magic = 1234;
    return (int) slot;

// On success, these system calls return the new file descriptor.  
// On error, -1 is returned, and errno is set appropriately.	

fail:
	//errno = ?;
    return (int) (-1);
}

/*
 * sys_pipe:
 * It creates two structures of stream that point to the same buffer.
 * It has no name.
 * It return two file descriptors.
 */
// #bugbug
// Maybe we need just one stream and two pointer.
// So we need to control the reader and the writer.
// #todo
// Isso precisa ter duas estruturas de arquivos,
// dois descritores, mas apenas um buffer.

int sys_pipe( int *pipefd, int flags )
{
    file *f1;
    file *f2;
    struct process_d *Process;
    pid_t current_process = -1;
    register int i=0;
    int slot1 = -1;
    int slot2 = -1;

    debug_print ("sys_pipe:\n");
       
    // Why ?    
    // Reject flags other than O_CLOEXEC.
    //if ((flags & O_CLOEXEC) != flags)
    //    return -EINVAL;

    // unsigned long fd_flags = (flags & O_CLOEXEC) ? FD_CLOEXEC : 0;

// Process
    current_process = (pid_t) get_current_process();
    Process = (void *) processList[current_process];
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
	// process_find_empty_stream_slot ( struct process_d *process );

// procurar 2 slots livres.

// #improvisando
// 0, 1, 2 são reservados para o fluxo padrão.
// Como ainda não temos rotinas par ao fluxo padrão,
// pode ser que peguemos os índices reservados.
// Para evitar, começaremos depois deles.

    // Reserva um slot.
    for ( i=3; i< OPEN_MAX; i++ )
    {
        if (Process->Objects[i] == 0)
        {
            //Process->Objects[i] = 216;
            slot1 = i;
            break;
        }
    };

    // Reserva um slot.
    for ( i=3; i< OPEN_MAX; i++ )
    {
        if (Process->Objects[i] == 0)
        {
            //Process->Objects[i] = 216;
            slot2 = i;
            break;
        }
    };

    // Check slots validation. 
    if ( slot1 == -1 || slot2 == -1 )
    {
        debug_print("sys_pipe: slots alocation fail\n");
        // todo: printk
        goto fail;
    }

// buffer

    //char *buff = (char *) newPage();
    char *buff = (char *) kmalloc(BUFSIZ);
    if ((void *) buff == NULL)
    {
        Process->Objects[slot1] = (unsigned long) 0;
        Process->Objects[slot2] = (unsigned long) 0;
        debug_print("sys_pipe: buffer fail\n");
        goto fail;
    }

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

// Early validations?

    f1->used = TRUE;
    f1->magic = 1234;

    f2->used = TRUE;
    f2->magic = 1234;

// As duas estruturas compartilham o mesmo buffer.        

// File: object type.
    f1->____object = ObjectTypePipe;
    f2->____object = ObjectTypePipe;

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
    f1->sync.can_read = TRUE;
    f1->sync.can_write = TRUE;
    f1->sync.can_execute = FALSE;
    f1->sync.can_accept = FALSE;
    f1->sync.can_connect = FALSE;

// sync:
    f2->sync.sender_pid = (pid_t) -1;
    f2->sync.receiver_pid = (pid_t) -1;
    f2->sync.action = ACTION_NULL;
    f2->sync.can_read = TRUE;
    f2->sync.can_write = TRUE;
    f2->sync.can_execute = FALSE;
    f2->sync.can_accept = FALSE;
    f2->sync.can_connect = FALSE;

// No name for now.
    f1->_tmpfname = NULL;
    f2->_tmpfname = NULL;

// The same buffer
    f1->_base = buff;
    f2->_base = buff;
    f1->_p    = buff;
    f2->_p    = buff;

// Buffer size.
    f1->_lbfsize = BUFSIZ; 
    f2->_lbfsize = BUFSIZ; 

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

// Saving structure.
    Process->Objects[slot1] = (unsigned long) f1;
    Process->Objects[slot2] = (unsigned long) f2;

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
    //refresh_screen();

// OK
    debug_print("sys_pipe: done\n");
    return 0;

fail:
    debug_print("sys_pipe: fail\n");
    return (int) (-1);
}

// #todo:
int sys_read_pipe( int fd, char *ubuf, int count )
{
    debug_print ("sys_read_pipe: TODO\n");

// Parameters
    if (fd<0 || fd>=OPEN_MAX){
        return (int) (-EBADF);
    }
    if ((void*) ubuf == NULL){
        return (int) (-EINVAL);
    }
    if (count<0)
        return -1;

// #todo ...

//fail:
    return (int) -1;
}

// #todo:
int sys_write_pipe( int fd, char *ubuf, int count )
{
    debug_print ("sys_write_pipe: TODO\n");

// Parameters
    if (fd<0 || fd >= OPEN_MAX)
        return (int) (-EBADF);

    if ((void*) ubuf == NULL)
        return (int) (-EINVAL);
    if (count<0)
        return -1;

// #todo: ...

    return -1;
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

int 
pipe_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    debug_print("pipe_ioctl: #todo\n");

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

