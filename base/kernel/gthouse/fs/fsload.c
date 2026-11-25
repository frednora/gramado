// fsload.c
// Created by Fred Nora.

/*
Current Limitations
Only supports FAT16 root directory (512 entries).
File size checks are basic (buffer overflow prevention).
No support for subdirectories beyond simple path parsing.
No caching — FAT is reloaded each time.
No integration with file structures (clusters aren’t tracked for later writes).
*/

#include <kernel.h>

// ------------------



static int 
__try_to_load_program_from_special_folder( 
    const char *filename, 
    unsigned long image_va );

// =======================================


// -------------------------------------
// fsLoadProgramFromGRAMADO:
// Load an image from GRAMADO/.
// IN:
// + program_name: Program name.
// + buffer: Pre-allocated buffer.
// + buffer_size_in_bytes: Buffer size in bytes.
// OUT:
// 0 = OK
// -1 = FAIL

int 
fsLoadProgramFromGRAMADO (
    char *program_name,
    unsigned long buffer,
    unsigned long buffer_size_in_bytes )
{
// Load an image from PROGRAM/

    int Status = -1; // fail

    // #debug
    //printk ("fsLoadProgramFromGRAMADO:\n");

    if ((void*) buffer == NULL){
        printk("fsLoadProgramFromGRAMADO: buffer\n");
        goto fail;
    }
    if (buffer_size_in_bytes == 0){
        printk("fsLoadProgramFromGRAMADO: buffer_size_in_bytes\n");
        goto fail;
    }

//
// sdGRAMADO struture.
//

    if (sdGRAMADO.initialized != TRUE){
        printk("fsLoadProgramFromGRAMADO: sdGRAMADO.initialized\n");
        goto fail;
    }
    if (sdGRAMADO.address == 0){
        printk("fsLoadProgramFromGRAMADO: sdGRAMADO.address\n");
        goto fail;
    }
// The memory address for our target directory.
    unsigned long programs_directory_address = 
        sdGRAMADO.address;

/*
 *    It loads a file into the memory.
 * IN:
 *     fat_address  = FAT address.
 *     dir_addresss = Directory address.
 *     dir_entries  = Number of entries in the given directory.
 *     file_name    = File name.
 *     buffer = Where to load the file. The pre-allocated buffer.
 *     buffer_size_in_bytes = Maximum buffer size.
 * OUT: 
 *    1=fail 
 *    0=ok.
 */

    Status = 
        (int) fat16LoadFile( 
                VOLUME1_FAT_ADDRESS, 
                programs_directory_address, // onde procurar 
                512,    //#bugbug: number of entries.
                program_name, 
                (unsigned long) buffer,  // buffer
                (unsigned long) buffer_size_in_bytes );  // buffer limits in bytes
    if (Status != 0)
    {
        goto fail;
    }
    return 0;

fail:
    return (int) -1;
}

// fsLoadProgramFromDE:
// Load an image from DE/.
// IN:
// + program_name: Program name.
// + buffer: Pre-allocated buffer.
// + buffer_size_in_bytes: Buffer size in bytes.
// OUT:
// 0 = OK
// -1 = FAIL

int 
fsLoadProgramFromDE (
    char *program_name,
    unsigned long buffer,
    unsigned long buffer_size_in_bytes )
{
// Load an image from PROGRAM/

    int Status = -1; // fail

    // #debug
    //printk ("fsLoadProgramFromDE:\n");

// Parameters:
    if ((void*) buffer == NULL){
        printk("fsLoadProgramFromDE: buffer\n");
        goto fail;
    }
    if (buffer_size_in_bytes == 0){
        printk("fsLoadProgramFromDE: buffer_size_in_bytes\n");
        goto fail;
    }

// sdDE struture.
    if (sdDE.initialized != TRUE){
        printk("fsLoadProgramFromDE: sdDE.initialized\n");
        goto fail;
    }
    if (sdDE.address == 0){
        printk("fsLoadProgramFromDE: sdDE.address\n");
        goto fail;
    }
// The memory address for our target directory.
    unsigned long programs_directory_address = 
        sdDE.address;

/*
 *    It loads a file into the memory.
 * IN:
 *     fat_address  = FAT address.
 *     dir_addresss = Directory address.
 *     dir_entries  = Number of entries in the given directory.
 *     file_name    = File name.
 *     buffer = Where to load the file. The pre-allocated buffer.
 *     buffer_size_in_bytes = Maximum buffer size.
 * OUT: 
 *    1=fail 
 *    0=ok.
 */

    Status = 
        (int) fat16LoadFile( 
                VOLUME1_FAT_ADDRESS, 
                programs_directory_address, // onde procurar 
                512,    //#bugbug: number of entries.
                program_name, 
                (unsigned long) buffer,  // buffer
                (unsigned long) buffer_size_in_bytes );  // buffer limits in bytes
    if (Status != 0){
        goto fail;
    }
    return 0;

fail:
    return (int) -1;
}

/*
 * fs_load_path:
 *     Carrega nesse endereço o arquivo que está nesse path.
 *     ??: O endereço pode ser ring3? 
 *     n levels.
 */
// IN:
// path:
//     Path de dois níveis.
//     Ex: "/BIN/GDESHELL.BIN"
// address:
//     Address to load the file.
// buffer_size:
//     Size of the buffer for the file.
// #bugbug
// Starts only on root dir.
// #ok
// Carregou um arquivo com 3 niveis.
// See:
// sys_load_path() and service 4004.
// #todo
// is address virtual or physical?
// Change this name to pa or va.
// Change to buffer_size_in_bytes ?
// OUT:
// 0 = OK
// -1 = FAIL

int 
fs_load_path ( 
    const char *path, 
    unsigned long address, 
    unsigned long buffer_size )
{

// Level iterator.
    register int l=0;
// Number of level in the give pathname.
    int n_levels = 0;
// Char iterator.
    int i=0;

// #todo
// Work on that limit stuff.
// We have the limit given by the argument,
// that needs to be respected.
// And we have the size of the root dir.

    unsigned long MaxEntries = FAT16_ROOT_ENTRIES;  //512

// Fail. 
// Usado na função que carrega o arquivo.
    int Ret = -1;    

    char name_buffer[12];
    unsigned char *p;

// Onde carregaremos o diretório
    void *__src_buffer;
    void *__dst_buffer;
    void *__file_buffer;

// Path
    if ((void*) path == NULL){
        panic("fs_load_path: path\n"); 
    }
    if (*path == 0){
        panic("fs_load_path: *path\n"); 
    }

// Address
    if (address == 0){
        panic("fs_load_path: address\n");
    }

// Buffer size
    if (buffer_size == 0){
        panic("fs_load_path: buffer_size\n");
    }

//===================================

// File buffer
    __file_buffer = (void *) address;
    unsigned long __file_buffer_size_in_bytes = 
        (unsigned long) (buffer_size & 0xFFFFFFFF);

//
// Levels
//

// Counting the levels.
// Start with 0.
    n_levels = (int) fs_count_path_levels(path);
// Temos uma quantidade limitada de buffers
// prealocados para os diretórios.
    if ( n_levels<=0 || n_levels >= FS_N_BUFFERS )
    {
        panic("fs_load_path: n_levels\n");
    }

// #debug
    //printk ("fs_load_path: path with %d levels\n",n_levels);

// Local pointer.
    p = path;

// #bugbug
// Overflow quando colocarmos um diretorio maior que o buffer.

// Primeiro src =  root address;
    __src_buffer = (void *) VOLUME1_ROOTDIR_ADDRESS;
    
// Buffer size in bytes.
// 512 entradas de 32bytes. 16KB ... 4 pages
// #hardcoded
// #bugbug: fs_buffers[]

    // 16384 = 32*512 = 4 PAGES. 
    unsigned long DirBufferSizeInBytes = (unsigned long) 16384;

    // Not absolute   
    if (p[0] != '/'){
        panic ("fs_load_path: Not absolute pathname\n");
    }

// loop: 
// Carregar n levels.

    for (l=0; l<n_levels; l++)
    {
        printk ("[LEVEL %d]\n",l);

        // The level needs to start with '/', even the first one.
        if (p[0] != '/'){
            panic ("fs_load_path: All levels need to start with '/'\n");
        }

        // Skip the '/'.
        p++;

        // Walk 13 chars in the same level.
        // Walk 12 chars in the same level.
        
        for ( i=0; i<12; i++ )
        {
            // Copia o char para o buffer de nome de arquivo 
            // até que o char seja '/',
            // indicando inicio do próximo nível.
            
            // E se não houver próximo nível?
            // #debug Show the char.

            name_buffer[i] = (char) *p;

            printk ("%c", (char) *p);

            //
            // Load a FILE.
            //

            // + O ponto deve aparecer no último nível.
            // caso contrário falhou
            // + Precisa ter extensao por enquanto.
            // + Se o ponto está além do limite permitido.
            // + Se o ponto for antes do nono byte. OK.

            if ( *p == '.' )
            {
                // Apareceu um ponto e não é o último nível ainda.
                // O ponto deve aparecer no último nível.
                // Senão falhou. Pois não aceitamos pastas
                // cujos nomes contenham ponto. (ainda nao)
                if ( l != (n_levels-1) ){
                    panic ("fs_load_path: '.' found into a subdirectory\n");
                }

                // Estamos no último nível,
                // mas o nome do arquivo começou com um ponto '.'.
                // We can't start a filename with a dot.
                if (i == 0){
                    panic ("fs_load_path: [fail] Filename starting with a dot.\n");
                }

                // 0~7 = OK.
                // 8 and 8+ = FAIL.
                // O ponto pode aparecer até o índice 7,
                // que representa o oitavo char.
                // 
                if (i > 8){
                    panic ("fs_load_path: Filename bigger than 8.\n");
                }

                // O ponto foi encontrado dentro do range permitido.

                // Nome tem no máximo 8 chars.
                // Completamos com espaço, pois queremos o formato:
                // "FILE    123"
                if (i < 8)
                {
                    // Enquanto for menor que 8 completa com espaço.
                    while (i<8){ 
                        name_buffer[i] = ' '; 
                        i++; 
                    };
                }

                // Vamos trabalhar nas extensões agora.
                // Elas começam no nono byte, que é o oitavo índice.
                // i=8;

                // Skip the dot '.'.
                // Yes it is a dot. See the IF statement above.
                p++;

                // Add the extension.
                i=8;  // [8] [9] [10]
                // #todo: Do not use scope definitions.
                int ExtCounter=0;
                while (i<11)
                {
                    // O marcador de fim de string pode aparecer
                    // emqualquer umdos 3 elementos da extensão.
                    if (*p == 0)
                    {
                       break;
                    }
                    
                    ExtCounter++;
                    name_buffer[i] = (char) *p;
                    i++;
                    p++;
                };
                
                // Se a extensão tinha menos que 3 bytes,
                // Então completamos a textenção com expaços.
                // (Se o break acima funcionou).
                
                // Completando a extenção com expaços.
                // 0 or ' '?
                if (ExtCounter < 3)
                {
                    if (i<11)
                    {
                        while (i<11)
                        {
                            ExtCounter++;
                            name_buffer[i] = ' ';
                            i++;
                        };
                    }
                }
                
                if (ExtCounter!=3){
                    panic("fs_load_path: ExtCounter\n");
                }

                // Finalize the string.
                name_buffer[11] = 0;
                     
                printk("\n");
                printk("fs_load_path: This is the name {%s}\n",name_buffer); 

                //
                // Load file
                //
    
                // Como esse é o último, 
                // então vamos usar o endereço desejado pelo usuário.
                __dst_buffer = (void *) __file_buffer;
                if ((void *) __dst_buffer == NULL){
                    panic ("fs_load_path: __dst_buffer\n");
                }

                // #bugbug
                // Se o diretório for o diretório raiz
                // então não podemos sondar menos que 512 entradas.
                // #todo: Temos que considerar o número de entradas
                // exatos de um diretório.
                // Podemos ter um limite estabelecido pelo sistema.

                // Load the file. (The last level)
                // IN: 
                // fat address, dir address, filename, file address.
                Ret = 
                    (int) fat16LoadFile ( 
                          (unsigned long) VOLUME1_FAT_ADDRESS,  // fat address
                          (unsigned long) __src_buffer,         // dir address. onde procurar. 
                          (unsigned long) MaxEntries,          // #bugbug: Number of entries. 
                          (unsigned char *) name_buffer,       // nome 
                          (unsigned long) __dst_buffer,         // addr. Onde carregar.
                          (unsigned long) __file_buffer_size_in_bytes );  // tamanho do buffer onde carregar.             
                // ok
                if (Ret == 0)
                {
                    printk("Level %d loaded!\n\n",l);

                    // #importante
                    // Esse nível tinha ponto, então deveria ser o último.
                    if ( l != (n_levels-1) )
                    {
                        printk ("fs_load_path: Directory name with '.'\n");
                        panic ("It needs to be the last level.\n");
                    }

                    // SUCCESS ?!!
                    debug_print ("fs_load_path: done\n");
                    return 0;

                }else{
                    panic ("fs_load_path: [FAIL] Loading level 0\n");
                };
            }

            //
            // Load a DIRECTORY.
            //

            // Se encontramos um indicador de próximo nível,
            // então esse nível não será considerado binário.
            // obs: 
            // Ao iniciar o for ele precisa encontrar esse mesmo char.
            
            // Se encontramos indicador de proximo nível,
            // então o que temos no buffer é um nome de diretorio.
            // Se o i for igual a 0, então o nome do diretorio
            // estava errado e temos uma barra seguida da outra.
            if (*p == '/')
            {
                if (i == 0){
                    panic("fs_load_path: Invalid folder name\n");
                }

                if (i >= (8+3)){
                    panic("fs_load_path: The folder name is too long\n");
                }

                // Encontramos o indicador de proximo nivel,
                // o buffer ja tem chars que foram colocados um a um.
                // Nao encontramos ponto nesse nivel.
                // Vamos completar o nome do diretorio com espaços 
                // caso o nome seja menor que (8+3) e 
                // finalizar.
                
                if ( i < (8+3) )
                {
                    // Adicionando espaços.
                    // O formato desejado eh: "DIRXDIRX   "
                    // Nome do diretorio sem extensao.
                    while (i < (8+3))
                    { 
                        //o primeiro espaço deve retirar a barra colocada antes
                        name_buffer[i] = ' ';  
                        i++; 
                    }
                }

                // Finalize the string.
                name_buffer[11] = 0;

                // #debug
                printk("\n");
                printk("fs_load_path: This is the name {%s}\n",name_buffer);

                //
                // Load directory.
                //

                // #bugbug
                // E se o arquivo for maior que a area alocada.
                // Alocando memória para carregar o diretório.
                
                //unsigned long BUGBUG_OVERFLOW = (512*32);
                //BufferSizeInBytes = (unsigned long) BUGBUG_OVERFLOW;
                
                // #todo
                // Grab a slab reusable buffer in a of buffers
                // destinated only for this purpose.
                // O indice do buffer é justamente o máximo
                // de levels suportados por essa rotina.
                                
                //__dst_buffer = (void *) kmalloc (    BUGBUG_OVERFLOW    ); 
                
                if ( l<0 || l >= FS_N_BUFFERS ){
                    panic("fs_load_path: l\n");
                }

                __dst_buffer = (void *) fs_buffers[l];
                if ((void *) __dst_buffer == NULL){
                    panic("fs_load_path: __dst_buffer\n");
                }

                // Load the directory. (Not the last level)
                // IN: 
                // fat address, dir address, filename, file address.
                Ret = 
                    (int) fat16LoadFile ( 
                          (unsigned long) VOLUME1_FAT_ADDRESS,  // fat address
                          (unsigned long) __src_buffer,         // dir address. onde procurar.
                          (int) MaxEntries,                    // #bugbug: Number of entries.  
                          (const char *) name_buffer,          // nome que pegamos no path 
                          (unsigned long) __dst_buffer,         // onde carregar. 
                          (unsigned long) DirBufferSizeInBytes );                             // tamanho do buffer onde carregar.
  
                // ok
                if (Ret == 0)
                {
                    printk("Level %d loaded!\n\n",l);
                    // O endereço onde carregamos o arquivo desse nível
                    // será o endereço onde vamos procurar o arquivo do próximo nível.
                    __src_buffer = __dst_buffer;
                    break;
                }else{
                    panic("fs_load_path: [FAIL] Loading level 0\n");
                };
            }

            // Avançamos o char se não foi '.', nem '/'.
            p++;
        };
    };   

fail:
    printk ("fs_load_path: fail\n");
    return (int) (-1);
}

/*
 * fsLoadFileFromCurrentTargetDir:
 *     Carrega o diretório que está configurado como 'target dir' 
 * em algum lugar qualquer da memória. 
 *     Usa-se a estrutura current_target_dir pra gerenciar isso.
 */
// #bugbug
// too much allocation.
// How many times this function is called ??
// 4KB each time ?

int fsLoadFileFromCurrentTargetDir(void)
{
    int Ret = -1;
    register int i=0;
    unsigned long new_address = 0;

// #bugbug
// 4KB
    unsigned long xxxTempFileSize = 4096;

    debug_print ("fsLoadFileFromCurrentTargetDir: [FIXME] Loading dir\n");

// #bugbug
// Isso 'e um limite para o tamanho do arquivo (apenas dir).
// precisamos expandir isso.
// aqui no m'aquimo o arquivo pode ter 4kb.
// acho ques estamos falando somente de diret'orio aqui.

// #bugbug
// too much allocation.
// How many times this function is called ??
// 4KB each time ?

    new_address = (unsigned long) kmalloc((size_t)xxxTempFileSize);
    if (new_address == 0){
        debug_print("fsLoadFileFromCurrentTargetDir: new_address\n");
        goto fail;
    }
    current_target_dir.current_dir_address = (unsigned long) new_address;

// ??
// Se o endereço atual falhar, 
// resetamos ele e retornamos.
    if (current_target_dir.current_dir_address == 0){
        debug_print("fsLoadFileFromCurrentTargetDir: [FAIL] invalid address\n");
        goto fail;
    }

    //#debug
    //printk ("fsLoadFileFromCurrentTargetDir: dir_name=(%s) old_dir_addr=(%x) #debug \n",
    //    current_target_dir.name, current_target_dir.current_dir_address );

//++
// IN: 
// FAT address, dir address, # dir entries, name, 
// buffer address, buffer size in bytes.
    Ret = 
        (int) fat16LoadFile ( 
                  (unsigned long) VOLUME1_FAT_ADDRESS,                     // fat cache address
                  (unsigned long) current_target_dir.current_dir_address,  // src dir address 
                  (int) FAT16_ROOT_ENTRIES, //#bugbug: Number of entries.  // number of entries.
                  (const char *) current_target_dir.name,                  // file name 
                  (unsigned long)   current_target_dir.current_dir_address,  // file address
                  (unsigned long) xxxTempFileSize );                                    // #bugbug buffer limit 4KB.
//--

    //#debug
    //printk ("fsLoadFileFromCurrentTargetDir: dir_name=(%s) new_dir_addr=(%x) #debug \n",
    //   current_target_dir.name, current_target_dir.current_dir_address );

    // debug_print ("fsLoadFileFromCurrentTargetDir: done\n");
    return (int) Ret;

fail:

// #todo
// debug message.

    current_target_dir.current_dir_address = VOLUME1_ROOTDIR_ADDRESS;
    for ( i=0; i<11; i++ ){
        current_target_dir.name[i] = '\0';
    };
    current_target_dir.name[0] = '/';
    current_target_dir.name[1] = '\0';
    return (int) -1;
}

// ---------------

// 2MB file size limit.
// How many times we can call this function?
static int 
__try_to_load_program_from_special_folder( 
    const char *filename, 
    unsigned long image_va )
{
// Load form /DE/ or /GRAMADO/. 
// It depends on the prefix on pathname.
// '@' for GRAMADO and '#' for DE.

    int Status = -1;

// 2048 KB = 2MB
// 2097152 = (512 * 4096) = 512 pages.
    unsigned long BUGBUG_IMAGE_SIZE_LIMIT = (unsigned long) (2097152);

    char *new_filename;
    new_filename = filename;

    // #debug
    //printk ("__try_to_load_program_from_special_folder:\n");

// Invalid file name.
    if ((void*) new_filename == NULL)
        goto fail;
    if (*new_filename == 0)
        goto fail;

//
// Skip first char and  load the image.
//

    // From GRAMADO/
    if (*new_filename == '@'){
        new_filename++;
        fs_fntos((char *) new_filename);
        Status = 
        (int) fsLoadProgramFromGRAMADO( 
                new_filename, 
                image_va, 
                BUGBUG_IMAGE_SIZE_LIMIT );

    // From PROGRAMS/
    } else if (*new_filename == '#'){
        new_filename++;
        fs_fntos((char *) new_filename);
        Status = 
        (int) fsLoadProgramFromDE( 
                new_filename, 
                image_va, 
                BUGBUG_IMAGE_SIZE_LIMIT );
    } else {
        goto fail;
    };

// done:
    if (Status == 0)
    {

        return 0;  // OK.
    }

fail:
    return (int) -1;
}

// fs_load_image:
// helper.
// Loading a image given the filename and its virtual address.
// Called by copy_process in clone.c right after a
// syscall to 'clone and execute'.
// Limits:
// Remember: This function is already able to load 
// executables with 400KB or more, just like ds00.
// OUT:
// 0 = OK
// -1 = FAIL

int 
fs_load_image( 
    const char *filename, 
    unsigned long image_va )
{

// #todo:
// Explain better all these variables.

    int Status=-1;
// The address of the rootdir.
// Is it a physical address.
// #see: base/new/include/mm/x64gpa.h
    unsigned long dir_va = VOLUME1_ROOTDIR_ADDRESS;
    //unsigned long dir_va = VOLUME1_ROOTDIR_PA;
    unsigned long dir_entries = FAT16_ROOT_ENTRIES;
    char *path;
    char *name;
    unsigned long BUGBUG_IMAGE_SIZE_LIMIT = (unsigned long) (512 * 4096);
    // para 32 entradas.
    // ????????
    unsigned long BUGBUG_OVERFLOW = ( 32*128 );


    printk("fs_load_image: Start\n");

// Parameter:
    if ((void*) filename == NULL){
        printk("fs_load_image: filename\n");
        goto fail;
    }
    if (*filename == 0){
        printk("fs_load_image: *filename\n");
        goto fail;
    }
    path = filename;
    name = filename;

//
// Path support
//

// Loading image from the cwd
    if (path[0] == '.' && path[1] == '/')
    {
        printk("fs_load_image: [FIXME] Can't execute from cwd\n");
        goto fail;
    }

// #test:
// Loading image from /GRAMADO/ or /DE/ directories.

    if ( *filename == '@' || 
         *filename == '#' )
    {

        printk("fs_load_image: Special folder\n");
        return (int) __try_to_load_program_from_special_folder( 
                        filename, image_va );
    }

// Loading image from the root directory.
    if (*filename == '/')
    {
        filename++;
        // New name
        path = filename;
        name = filename;
    }

//
// Loop
//

// Search
// Convertendo o formato do nome do arquivo.
// >>> "12345678XYZ"
// Procura o nome no diretório. 
// Que em nosso caso ainda é o diretório raiz.
// Convert and search.
// Fail if it was not found.

__search:

// #bugbug
// fs_fntos() is gonna include the extension .bin
// if the name doesn't have one.
// This is not what we want for all the cases.

    fs_fntos((char *) name);

    Status = (int) search_in_dir(name, dir_va);
    if (Status != TRUE){
        printk ("fs_load_image: File not found\n");
        goto fail;
    }

// The file was found into the directory
__found:

// ============================

// [3]
// #debug

    //debug_print ("fs_load_image: [3] Loading the image\n");
       //printk ("fs_load_image: [3] Loading the image\n");

// Loading from rootdir?
// >> Load file:
// #importante: 
// Carregando a imagem do processo filho.
// Se o carregamento falhar, temos que abortar a clonagem,
// caso contrário, executa a cópia da imagem do pai. ??
// #bugbug: Essa rotina começou a falhar aqui. Convertendo 
// num formato errado.
// Movemos essa conversão para o início dessa função,
// onde checaremos se o arquivo está no diretório.
// #bugbug
// Se isso não está funcionando direito e uma thread 
// defeituosa fica remanescente quando digitamos um 
// comando errado então vamos matar a thread e o processo.
// #todo
// Num ambiente 'mp' precisaremos de um lock aqui.

    //#debug
    //printk ("do_clone_execute_process: %s\n",filename);

    //unsigned long BUGBUG_IMAGE_SIZE_LIMIT = (unsigned long) (512 * 4096);

    if (dir_va == 0){
        panic("fs_load_image: dir_va\n");
    }
    if ((void *) image_va == NULL){
        panic("fs_load_image: image_va\n");
    }

    Status = 
        (int) fat16LoadFile ( 
                  VOLUME1_FAT_ADDRESS, 
                  (unsigned long) dir_va,       //#bugbug: This is only for the root dir.
                  (unsigned long) dir_entries,  //#bugbug: Number of entries. 
                  name, 
                  (unsigned long) image_va,
                  BUGBUG_IMAGE_SIZE_LIMIT );

    printk("fs_load_image: End\n");
    return (int) Status;   // ok?

fail:

    // #debug
    // We still need this kind of break,
    // cause the fs infrastructure is still immature.
    // panic("fs_load_image: fail\n");

    printk("fs_load_image: Fail\n");

    return (int) -1;
}

