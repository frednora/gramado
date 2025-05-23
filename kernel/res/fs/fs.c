// fs.c
// These are the handlers for the syscalls.
// Created by Fred Nora.

#include <kernel.h>


ssize_t sys_read(int fd, const char *ubuf, size_t count)
{
    if (fd<0){
        return (ssize_t) -EBADF;
    }
/*
// #todo
    if (fd >= OPEN_MAX){
        return (ssize_t) -EBADF;
    }
*/

// #test
// Only the foreground thread can read the stdin
// OK. This is working now, and only the foreground thread
// was able to read stdin.
// I tested it in a lot of applications.
// #todo
// We can implement a better routine for this.

    if (fd == 0){
        if (current_thread != foreground_thread)
            return (ssize_t) -EPERM;
    }

// #test
// Usermode buffer validation
// #todo: Check against more limits.
    if (ubuf < CONTROLTHREAD_BASE)
    {
        panic ("sys_read: Invalid ubuf\n");
        //return (ssize_t) -EFAULT;  // bad address
    }

// Nothing to read.
    if (count == 0){
        return (ssize_t) -EINVAL;
    }
/*
// #todo: Create a max limit for this.
    if (count >= MaxXXX){
        return (ssize_t) -EINVAL;
    }
*/

    return (ssize_t) __read_imp(fd, ubuf, count);
}

ssize_t sys_write(int fd, const char *ubuf, size_t count)
{
    if (fd<0){
        return (ssize_t) -EBADF;
    }
/*
// #todo
    if (fd >= OPEN_MAX){
        return (ssize_t) -EBADF;
    }
*/


// #test
// Usermode buffer validation
// #todo: Check agains more limits.
    if (ubuf < CONTROLTHREAD_BASE)
    {
        panic ("sys_write: Invalid ubuf\n");
        //return (ssize_t) -EFAULT;  // bad address
    }

// Nothing to read.
    if (count == 0){
        return (ssize_t) -EINVAL;
    }
/*
// #todo: Create a max limit for this.
    if (count >= MaxXXX){
        return (ssize_t) -EINVAL;
    }
*/

    return (ssize_t) __write_imp(fd, ubuf, count);
}

int 
sys_open(
    const char *pathname, 
    int flags, 
    mode_t mode )
{
// Limits:
// Probably BUFSIZ (1024 for now) is the limit for this routine.
// To improve this we need to allocate a bigger buffer 
// in the file structure.

    if ((void*) pathname == NULL)
        return (int) -EFAULT;  // Bad address. Is it the right error code?
    if (*pathname == 0)
        return (int) -EFAULT;  // Bad address. Is it the right error code?
    return (int) __open_imp( pathname, flags, mode );
}

int sys_close(int fd)
{
    if (fd<0)
        return (int) -EBADF;
/*
// #todo
    if (fd >= OPEN_MAX){
        return (ssize_t) -EBADF;
    }
*/
    return (int) __close_imp(fd);
}

// sys_fcntl:
// Manipulate file descriptor.
// #todo: 
// Rever esses argumentos.
// SVr4, 4.3BSD, POSIX.1-2001. and more.
// See: 
// https://man7.org/linux/man-pages/man2/fcntl.2.html

int sys_fcntl ( int fd, int cmd, unsigned long arg )
{
    debug_print ("sys_fcntl: #todo\n");

// Parameters
    if ( fd < 0 || fd >= OPEN_MAX ){
        return (int) (-EBADF);
    }
    if (cmd < 0){
        return (int) (-EINVAL);
    }

// POSIX Table 6-1.
// See: fcntl.h

    switch (cmd){

    // Duplicating a file descriptor
    // Return: The new file descriptor.
    case F_DUPFD:
        debug_print ("sys_fcntl: [TODO] F_DUPFD\n");
        goto fail;
        break;
    
    // F_DUPFD_CLOEXEC

    // File descriptor flags

    // Return (as the function result) the file descriptor flags.
    case F_GETFD:
        debug_print ("sys_fcntl: [TODO] F_GETFD\n");
        goto fail;
        break;

    // Set the file descriptor flags to the value specified by arg.
    case F_SETFD:
        debug_print ("sys_fcntl: [TODO] F_SETFD\n");
        goto fail;
        break;

    // File status flags

    // Return (as the function result) the file access mode and
    // the file status flags; arg is ignored.
    // Return: Value of file status flags.
    case F_GETFL:
        debug_print ("sys_fcntl: [TODO] F_GETFL\n");
        goto fail;
        break;

    // Set the file status flags to the value specified by arg.
    case F_SETFL:
        debug_print ("sys_fcntl: [TODO] F_SETFL\n");
        goto fail;
        break;

    // Advisory record locking

    // Get record locking information
    case F_GETLK:
        debug_print ("sys_fcntl: [TODO] F_GETLK\n");
        goto fail;
        break;

    // Set record locking information
    case F_SETLK:
        debug_print ("sys_fcntl: [TODO] F_SETLK\n");
        goto fail;
        break;

    // Set record locking info; wait if blocked
    case F_SETLKW:
        debug_print ("sys_fcntl: [TODO] F_SETLKW\n");
        goto fail;
        break;

    // Free a section of a regular file
    case F_FREESP:
        debug_print ("sys_fcntl: [TODO] F_FREESP\n");
        goto fail;
        break;

    // ...

    // Invalid or not supported command value.
    default:
        return (int) (-EINVAL);
        break;
    };

fail:
    debug_print ("sys_fcntl: FAIL\n");
    return (int) -1;
}

// sys_ioctl:
// Called by sc82 in sci.c
// Enquanto sys_ioctl eh chamada pelos applicativos,
// io_ioctl eh chamada pelas rotinas dentro do kernel.
// See: io.c
int sys_ioctl( int fd, unsigned long request, unsigned long arg )
{
// ioctl() implementation.

    int status = -1;

    debug_print ("sys_ioctl: [FIXME] \n");
    //printk("sys_ioctl: [FIXME] \n");

// Parameters
    if ( fd < 0 || fd >= OPEN_MAX ){
        return (int) (-EBADF);
    }

// Call io_.
    status = (int) io_ioctl(fd,request,arg);
    if (status < 0)
    {
        //?
    }

    // #debug
    // We need to see the error messages.
    //refresh_screen();

    return (int) status;
}

// 10003
// Get action.
// See: kstdio.h
int sys_get_global_sync (int sync_id, int request)
{ 
    struct kstdio_sync_d *s;

// Parameters
    if ( sync_id < 0 || sync_id >= SYNC_COUNT_MAX )
    {
        //message? panic?
        goto fail;
    }
    //if (request < 0)
        //goto fail;

    s = (struct kstdio_sync_d *) syncList[sync_id];

    if (request == SYNC_REQUEST_GET_ACTION)
    {
        if ((void*) s == NULL){
            goto fail;
        }
        return (int) s->action;
    }

fail:
    return (int) -1; 
}

// 10002
// Set action.
// See: kstdio.h
void sys_set_global_sync(int sync_id, int request, int data)
{
    struct kstdio_sync_d *s;

// Parameters
    if ( sync_id < 0 || sync_id >= SYNC_COUNT_MAX )
    {
       //message? panic?
       return;
    }
    //if (request < 0)
        //return;

    s = (struct kstdio_sync_d *) syncList[sync_id];

    if (request == SYNC_REQUEST_SET_ACTION)
    {
        if ((void*) s != NULL){
            s->action = data;
        }
    }
}

// 10004
// Create sync structure and
// put the pointer into the list.
// See: kstdio.h
int sync_count=0;
static int __saved_sync_id=0;

// Get the id.
// provisorio, para testes
// See: kstdio.h
int get_saved_sync(void)
{
    return (int) __saved_sync_id;
}

int sys_create_new_sync(void)
{
    struct kstdio_sync_d *s;

// Create new structure.
    s = (struct kstdio_sync_d *) kmalloc( sizeof(struct kstdio_sync_d) );
    if ((void*) s == NULL){
        goto fail;
    }
    memset(s, 0, sizeof(struct kstdio_sync_d));

    s->used=TRUE;
    s->magic=1234;
    s->action = ACTION_NULL;
    sync_count++;
    if ( sync_count > 0 && sync_count < SYNC_COUNT_MAX )
    {
        syncList[sync_count] = (unsigned long) s;
        __saved_sync_id = (int) sync_count;
        return (int) sync_count;
    }

fail:
    return (int) -1;
}


// --------------------------------------------------
// service 10006
// #todo
// Comment the purpose of this routine.
// It is used on socket communication.
void sys_set_file_sync(int fd, int request, int data)
{
    file *object;
    struct process_d  *p;
    pid_t current_process = -1;
   
    //#bugbug
    // Pensaremos nessa possibilidade.

// Parameters
    if ( fd < 0 || fd >= OPEN_MAX ){
        debug_print("sys_set_file_sync: fd\n");
        return;
    }
    //if (request < 0)
        //return;

/*
    if (fd == 0 || fd == 1 || fd == 2 )
    {
        debug_print("sys_close: [FIXME] We can't close the standard stream\n");
        // WHY NOT ???!!
        return (int) (-1);
    }
 */


// == Process ================
    current_process = (pid_t) get_current_process();
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX ){
        debug_print("sys_set_file_sync: current_process\n");
        return;
    }
    p = (void *) processList[current_process];
    if ((void *) p == NULL){
        debug_print("sys_set_file_sync: p\n");
        return;
    }
    if (p->magic != 1234){
        debug_print("sys_set_file_sync: p validation\n");
        return;
    }

// object
// Everything is a file.

    object = (file *) p->Objects[fd];
    if ((void*) object == NULL){
        debug_print("sys_set_file_sync: object\n");
        return;
    }
    if ( object->used != TRUE || object->magic != 1234 ){
        debug_print("sys_set_file_sync: validation\n");
        return;
    }

// request.

    switch (request){

    // set last action
    case SYNC_REQUEST_SET_ACTION:
        object->sync.action = data;
        break;

    // #test
    // reset
    // Now we can write
    // SYNC_REQUEST_RESET_WR
    case 216:

        //#debug
        //printk("216:\n"); 
        //refresh_screen();

        object->sync.action = 0;
        //object->_flags = (__SWR | __SRD); 
        object->_flags = __SWR;
        k_rewind(object);
        object->_r = 0;
        object->_w = 0;
        object->socket_buffer_full = FALSE; //empty buffer
        return;
        break;

    // #test
    // Now we can read
    // SYNC_REQUEST_RESET_RD
    case 217:

        //#debug
        //printk("217:\n"); 
        //refresh_screen();

        object->sync.action = 0;
        //object->_flags = (__SWR | __SRD); 
        object->_flags = __SRD;
        return;
        break;

    // ...

    default:
        debug_print("sys_set_file_sync: [FAIL] Default request\n");
        return;
        break;
    };

    // ...
}

// ---------------------------------
// service 10007
// #todo
// Comment the purpose of this routine.
// It is used on socket communication.
// #todo: Explain the output values.
int sys_get_file_sync(int fd, int request)
{
    file *object;
    struct process_d  *p;
    pid_t current_process = -1;
    
    //#bugbug
    // Pensaremos nessa possibilidade.

// Parameters
    if ( fd < 0 || fd >= OPEN_MAX ){
        return (int) (-EBADF);
    }
    //if (request < 0)
        //return -1;

/*
    if (fd == 0 || fd == 1 || fd == 2 )
    {
        debug_print("sys_close: [FIXME] We can't close the standard stream\n");
        // WHY NOT ???!!
        return (int) (-1);
    }
 */

// == Process ================
    current_process = (pid_t) get_current_process();
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX ){
        debug_print("sys_get_file_sync: current_process\n");
        goto fail;
    }
    p = (void *) processList[current_process];
    if ((void *) p == NULL){
        debug_print("sys_get_file_sync: p\n");
        goto fail;
    }
    if (p->magic != 1234){
        debug_print("sys_get_file_sync: p validation\n");
        goto fail;
    }

// Object

    object = (file *) p->Objects[fd];
    if ((void*) object == NULL){
        debug_print("sys_get_file_sync: [FAIL] object\n");
        goto fail;
    }
    if ( object->used != TRUE || object->magic != 1234 ){
        debug_print("sys_get_file_sync: validation\n");
        goto fail;
    }

    switch (request){
    // get last action
    case SYNC_REQUEST_GET_ACTION:
        return (int) object->sync.action;
        break;
    //case ?:
        //break;
    // ...
    default:
        debug_print("sys_get_file_sync: Default request\n");
        goto fail;
        break;
    };

// ...

// ?? Why '0'?
    return 0;

fail:
    return (int) -1;
}

// sys_get_device_number_by_path:
// Get the device number in the dev_dir[] list given the pathname.
// ex: "/DEV/DEV1"
int sys_get_device_number_by_path(const char *path)
{
    if ((void*)path == NULL)
        return -1;
// ...
    return (int) -1;
}

// #test
// #todo
// Precisamos uma syscall para esse serviço.
// Open device by number.
// Index in the dev_dir[] list.
// Incluir o ponteiro para arquivo na lista 
// de arquivos abertos do processo atual.
// See: sys.h

int sys_open_device_by_number(int device_index)
{
    file *fp;
    struct process_d *p;
    pid_t current_process = -1;
    register int __slot=0;
    int i = device_index;

// Parameters:
    if (i<0 || i>=32){
        return (int) (-EINVAL);
    }

    if (dev_dir[i].magic != 1234){
        goto fail;
    }
    if (dev_dir[i].initialized != TRUE){
        goto fail;
    }

// Get file pointer.
    fp = (file*) dev_dir[i].fp;
    if ((void*) fp == NULL){
        goto fail;
    }
    if (fp->magic != 1234){
        goto fail;
    }

// Put the pointer into the list.

// Process
    current_process = (pid_t) get_current_process();
    if (current_process < 0 || current_process >= PROCESS_COUNT_MAX){
        goto fail;
    }
    p = (struct process_d *) processList[current_process];
    if ((void *) p == NULL){
        debug_print ("sys_open_device_by_number: p\n");
        goto fail;
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        debug_print ("sys_open_device_by_number: validation\n");
        goto fail;
    }

// Probing for a free slot.
    //static int NumberOfslots = 32;
    for (__slot=0; __slot<32; __slot++)
    {
        if ( p->Objects[__slot] == 0 ){
            goto __OK;
        }
    };

// fail
    panic ("sys_open_device_by_number: No slots!\n");

// Slot found.
// Validate index, save the fp and return the index.
__OK:
    if ( __slot < 0 || __slot >= 32 )
    {
        printk ("sys_open_device_by_number: __slot fail\n");
        goto fail;
    }
    p->Objects[__slot] = (unsigned long) fp;
    return (int) __slot;

fail:
    //refresh_screen();
    return (int) -1;
}

// sys_get_file_size:
// 178
// Only root dir.
unsigned long sys_get_file_size(const char *path)
{
    unsigned long FileSize=0;

// Parameter
    if ((void*) path == NULL){
        return 0;
    }
    if ( *path == 0 ){
        return 0;
    }

    FileSize = 
    (unsigned long) fsGetFileSize ( 
                        (unsigned char *) path, 
                        (unsigned long) VOLUME1_ROOTDIR_ADDRESS );

    return (unsigned long) FileSize; 
}

// IN: fd
// OUT: 
// -1= error; 
// FALSE= nao pode ler; 
// TRUE= pode ler.

int sys_sleep_if_socket_is_empty(int fd)
{
    file *object;
    struct process_d *p;
    pid_t current_process = -1;

// Parameter
    if ( fd < 0 || fd >= OPEN_MAX ){
        return (int) (-EBADF);
    }

// Process
    current_process = (pid_t) get_current_process();
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX ){
        debug_print("sys_sleep_if_socket_is_empty: current_process\n");
        goto fail;
    }
    p = (void *) processList[current_process];
    if ((void *) p == NULL){
        debug_print("sys_sleep_if_socket_is_empty: p\n");
        goto fail;
    }
    if (p->magic != 1234){
        goto fail;
    }

// Object

    object = (file *) p->Objects[fd];
    if ((void *) object == NULL){
        debug_print("sys_sleep_if_socket_is_empty: object\n");
        goto fail;
    }
    if (object->used != TRUE || object->magic != 1234 ){
        debug_print("sys_sleep_if_socket_is_empty: object validation\n");
        goto fail;
    }

// Is it a socket object?
    if (object->____object != ObjectTypeSocket){
        debug_print ("sys_sleep_if_socket_is_empty: Not a socket\n");
        goto fail;
    }

    // TRUE or FALSE
    //return (int) object->socket_buffer_full;   

// Sim esta vazio, 
// nao pode ler;  
// retorna FALSE
    if ( object->socket_buffer_full == FALSE )
    {
        debug_print("sys_sleep_if_socket_is_empty: Buffer is empty. we can not read. sleeping\n");
        object->_flags |= __SWR;                  // pode escrever
        //todo: falg que nege a leitura.
        object->tid_waiting = current_thread;     // thread atual dorme   
        //do_thread_waiting (current_thread);
        return FALSE;  // nao pode ler
    }

// O buffer esta cheio, pode ler. retorna TRUE
    if ( object->socket_buffer_full == TRUE ){
        debug_print("sys_sleep_if_socket_is_empty: Buffer is empty. we can read\n");
        object->_flags |= __SRD;
        return TRUE;
    }

//#bugbug
fail:
    debug_print ("sys_sleep_if_socket_is_empty: Unexpected error\n");
    return (int) -1;
}

// sys_pwd -  Service 170.
// #todo
// When this syscall is useful.
// Actually we need to create a ring 3 terminal command 
// for this.
// This syscall is valid only when 
// the plain is print in the raw screen using the 
// kernel console.
void sys_pwd(void)
{
    pid_t current_process = -1;

    current_process = (pid_t) get_current_process();
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX ){
        printk ("sys_pwd: [FAIL] current_process\n");
        return;
    }
    fs_print_process_cwd (current_process);
}


void 
fs_load_boot_metafile (
    unsigned long buffer, 
    unsigned long first_lba, 
    unsigned long size_in_sectors )
{
// See: disk_r.c
    ata_load_boot_metafile(buffer,first_lba,size_in_sectors);
}

void 
fs_store_boot_metafile (
    unsigned long buffer, 
    unsigned long first_lba, 
    unsigned long size_in_sectors )
{
// See: disk_w.c
    ata_store_boot_metafile(buffer,first_lba,size_in_sectors);
}

// Wrapper
// #todo: Explain it.
int 
sys_read_file_from_disk ( 
    const char *file_name, 
    int flags, 
    mode_t mode )
{

// Check validation
    if ((void*) file_name == NULL){
        return (int) (-EINVAL);
    }
    if (*file_name == 0){
        return (int) (-EINVAL);
    }

//
// Local copy
//

    char pathname_local_copy[256];
    memset(pathname_local_copy,0,256);
    // Coping more than we need, 
    // this way we're coping the 0x00 byte at the and of string
    // and some extra bytes.
    strncpy(pathname_local_copy,file_name,256);

// see: fslib.c
    return (int) do_read_file_from_disk(
                    (char *) pathname_local_copy,
                    (int) flags, 
                    (mode_t) mode );
}

// Wrapper
int
sys_write_file_to_disk ( 
    const char *file_name, 
    unsigned long file_size,
    unsigned long size_in_bytes,
    char *file_address,
    char flag )
{

// Check validation
    if ((void*) file_name == NULL){
        return (int) (-EINVAL);
    }
    if (*file_name == 0){
        return (int) (-EINVAL);
    }

//
// Local copy
//

    char pathname_local_copy[256];
    memset(pathname_local_copy,0,256);
    // Coping more than we need, 
    // this way we're coping the 0x00 byte at the and of string
    // and some extra bytes.
    strncpy(pathname_local_copy,file_name,256);

    return (int) do_write_file_to_disk(
                     (char *) pathname_local_copy,
                     (unsigned long) file_size,
                     (unsigned long) size_in_bytes,
                     (char *) file_address,
                     (char) flag );
                     
}

// ==============================
// Service 43
// See: see sci.c
int sys_create_empty_file(const char *file_name)
{
    int __ret = -1;
    //char *FileName;
    //FileName = (char *) file_name;
// #bugbug: 
// We need a buffer in another place.
// #todo: Allocate space for a new file.
    char buffer[BUFSIZ];
    //char *buf;
// How many sectors.
// (FileSizeInBytes/512)
    int NumberOfSectors = 2;
// How many bytes.
    int FileSizeInBytes = BUFSIZ;  // 1024 for now.
// String inside the new file.
    int UseStringForDebug = TRUE;

    debug_print ("sys_create_empty_file:\n");
    printk      ("sys_create_empty_file:\n");

// Parameters:
    if ((void*) file_name == NULL){
        return (int) (-EINVAL);
    }
    if (*file_name == 0){
        return (int) (-EINVAL);
    }

// Change the string format.
    fs_fntos((char *) file_name);

// Clear the buffer and Create a string for
// debug purpose.
    memset(buffer,0,BUFSIZ);

    if (UseStringForDebug == TRUE)
        ksprintf(buffer,"This is a new file.");

// 0x20 = file.
// See: write.c
    //printk ("0x20 \n");
    __ret = 
        (int) fsSaveFile ( 
                VOLUME1_FAT_ADDRESS, 
                VOLUME1_ROOTDIR_ADDRESS, 
                FAT16_ROOT_ENTRIES,
                (char *)         file_name,
                (unsigned long)  NumberOfSectors, 
                (unsigned long)  FileSizeInBytes,  
                (char *)         &buffer[0], 
                (char)           0x20 ); 

    if (__ret < 0){
        debug_print("sys_create_empty_file: fail\n");
        goto fail;
    }

// #todo
// the file structure.
    return (int) __ret;
fail:
    return (int) -1;
}

// sys_cd_command:
// Service 175. cd command.
// #todo
// ou usamos o cwd do processo ou
// o diretorio raiz para paths absolutos.
void sys_cd_command (const char *string)
{

// Parameter
    if ((void *) string == NULL){
        debug_print("sys_cd_command: string\n");
        return;
    }
    if ( *string == 0 ){
        debug_print("sys_cd_command: *string\n");
        return;
    }

// Reset global structure and cwd on process structure.

// #bugbug
// Talvez esse tipo de tratamento precise 
// ser feito pelo próprio shell.

    if (string[1] == 0)
    {
        // $ cd /
        // We also need to clean the name in the process structure.
        if (string[0] == '/'){
            debug_print("sys_cd_command: reseting\n");
            fsInitTargetDir(VOLUME1_ROOTDIR_ADDRESS,"/");
            //#bugbug: invalid pid
            //fs_initialize_process_cwd ( current_directory, "/" );
            return;
        }

        // ...

        return;
    }

// Atualiza na estrutura de processo.
// Atualiza na estrutura global para diretorio alvo.
    fsUpdateWorkingDiretoryString((char *) string);
// Isso carrega o diretorio que agora 'e o diretorio alvo.
    fsLoadFileFromCurrentTargetDir();
    // ...
}

// ================================
// Service 44
// See: fs_create_empty_directory
int sys_create_empty_directory(const char *dir_name)
{
    int __ret=0;

// #bugbug: 
// We need a buffer in another place.
    char buffer[BUFSIZ];  // 1024
    int number_of_sectors = 2;
    int size_in_bytes = BUFSIZ; // (1024 for now)

    debug_print ("sys_create_empty_directory:\n");
    printk      ("sys_create_empty_directory:\n");

// Parameter:
    if ((void*) dir_name == NULL){
        return (int) (-EINVAL);
    }
    if (*dir_name == 0){
        return (int) (-EINVAL);
    }

    fs_fntos((char *) dir_name);

// Clear the buffer.
    memset(buffer,0,BUFSIZ);

// See: write.c
// 0x10 = directory. 
    //printk ("0x10 \n");
    __ret = 
        (int) fsSaveFile ( 
                VOLUME1_FAT_ADDRESS, 
                VOLUME1_ROOTDIR_ADDRESS, 
                FAT16_ROOT_ENTRIES,
                (char *)         dir_name,
                (unsigned long)  number_of_sectors, 
                (unsigned long)  size_in_bytes, 
                (char *)         &buffer[0], 
                (char)           0x10 ); 

    if (__ret < 0){
        debug_print("sys_create_empty_directory: fail\n");
             printk("sys_create_empty_directory: fail\n");
        goto fail;
    }

// #todo
// the file structure.
    return (int) __ret;
fail:
    refresh_screen();
    return (int) -1;
}

//
// #
// INITIALIZATION
//

// Called by I_initKernelComponents() in x64init.c
int fsInitialize(void)
{
    // see: fslib.c.
    __fs_initialize_imp();

    return 0;
}

