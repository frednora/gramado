// fs/fsload.c
// Created by Fred Nora.

#include <kernel.h>

// ------------------

static int __check_address_validation(unsigned long address);

static int 
__try_to_load_program_from_special_folder( 
    const char *filename, 
    unsigned long image_va );

// ------------------


// WORKER
// Called by fsLoadFile
// Some invalid address.
// We can not load a file in the same addresses of the
// base kernel or the rootdir ...
// See: gva.h
// Protectng some core areas.
// OUT: 
// TRUE = OK | FALSE = FAIL
// #todo
// We also can check against some well known ranges.

static int __check_address_validation(unsigned long address)
{
    int Status = TRUE;

// Parameter
// fat, rootdir, base kernel, lfb, backbuffer ...
    if (address == VOLUME1_FAT_ADDRESS_VA)           { Status=FALSE; }
    if (address == VOLUME1_ROOTDIR_ADDRESS_VA)       { Status=FALSE; }
    if (address == KERNEL_IMAGE_BASE)                { Status=FALSE; }
    if (address == DEFAULT_LFB_VIRTUALADDRESS)       { Status=FALSE; }
    if (address == DEFAULT_BACKBUFFER_VIRTUALADDRESS){ Status=FALSE; }
    // ...

// #todo
// We can check against some other core addresses.

//done:
    return (int) Status;
}

// ------------------------

/*
 * fsLoadFile:
 *    It loads a file into the memory.
IN:
    fat_address          = FAT address.
    dir_addresss         = Directory address.
    dir_entries          = Number of entries in the given directory.
    file_name            = File name.
    buffer               = Where to load the file. The pre-allocated buffer.
    buffer_size_in_bytes = Maximum buffer size.
OUT: 
    1=fail 
    0=ok.
 */
// #bugbug
// Essa rotina somente consegue pegar o tamanho do arquivo
// se o arquivo estiver no diretório raiz.
// #todo: Criar uma rotina para pegar o tamanho do arquivo em 
// qualquer diretório.
// #obs
// Rotina específica para FAT16.
// Podemos mudar o nome para fsFat16LoadFile().
// Ou fs_Fat16_SFN_LoadFile()
// #bugbug
// This routine is nor respecting the number of entries
// in the diretory. It is using the limit of the root dir
// for all the directories, 512 entries.
// #todo
// Maybe we need to use a structure for that set of parameters.
// #todo
// is dir_address virtual or physical?
// Change this name to dir_pa or dir_va.

unsigned long 
fsLoadFile ( 
    unsigned long fat_address,
    unsigned long dir_address,
    int dir_entries,
    const char *file_name, 
    unsigned long buffer,
    unsigned long buffer_size_in_bytes )
{
    int Status=-1;
    register int i=0;
    int SavedDirEntry = 0;
    unsigned short next=0;
    int is_valid=FALSE;
// #todo:
// Rever. Número máximo de entradas.
// #bugbug: 
// Esse eh o numero de entradas no diretorio raiz.
    unsigned long DirEntries = (unsigned long) (dir_entries & 0xFFFF);
    unsigned long MaxEntries = (unsigned long) FAT16_ROOT_ENTRIES;
// Address: Where to load the file.
    unsigned long Buffer = (unsigned long) buffer;
// Size: maximum buffer size in size.
    unsigned long BufferSizeInBytes = 
        (unsigned long) (buffer_size_in_bytes & 0xFFFFFFFF);
    unsigned long z=0;  //Deslocamento do rootdir 
    unsigned long n=0;  //Deslocamento no nome.

// #bugbug: Na verdade uma rotina abaixo esta copiando
// mais do que o nome do arquivo, pois a rotina de copia
// esta copiando shorts e não chars.
// Então vamos aumentar esse buffer por enquanto.
    char tmpName[256];
    size_t FileNameSize = 0;
    unsigned long FileSize = 0;
    //int IsDirectory;
// Cluster inicial
    unsigned short cluster=0; 
// ?? 
// Primeiro setor do cluster.
    unsigned long S=0;  
// Usado junto com o endereço do arquivo.
    unsigned long SectorSize = SECTOR_SIZE;
// Sectors per cluster.
    int Spc=0;

    // #debug:
    debug_print ("fsLoadFile:\n");
    //printk    ("fsLoadFile:\n");

// Fat address and dir address.
// Vectors of 'short'
    if (fat_address == 0){
        panic("fsLoadFile: [FAIL] fat_address\n");
    }
    if (dir_address == 0){
        panic("fsLoadFile: [FAIL] dir_address\n");
    }

// Addresses
    unsigned short *  fat = (unsigned short *) fat_address;
    unsigned short *__dir = (unsigned short *) dir_address;

// #debug
// We only support one address for now.
    if (fat_address != VOLUME1_FAT_ADDRESS){
        panic("fsLoadFile: Not valid fat address\n");
    }

// Initialize variables.
    /*
    if (____IsCdRom) {
        SectorSize = 2048;
    } else {
        SectorSize = SECTOR_SIZE;
    }
    */
// ...


// =======================

// How many entries in this directory.
// For now, this is the limit of the rootdir.

    //if ( MaxEntries == 0 || MaxEntries >= FAT16_ROOT_ENTRIES )
    if (MaxEntries != FAT16_ROOT_ENTRIES){
        panic ("fsLoadFile: MaxEntries limits\n");
    }

    if (DirEntries > MaxEntries){
        panic ("fsLoadFile: DirEntries\n");
    }

// #test: 
// Used only for debug. 512 entries in rootdir.
    if (DirEntries != MaxEntries){
        panic ("fsLoadFile: [DEBUG] DirEntries IS LESS THE 512\n");
    }

// The pre-allocated buffer.
// Buffer size in bytes
// Max = 512 pages.

    if (BufferSizeInBytes == 0){
        panic("fsLoadFile: BufferSizeInBytes = 0\n");
    }
    if ( BufferSizeInBytes > (512*4096) ){
        panic("fsLoadFile: BufferSizeInBytes limits\n");
    }

// Root file system structure.
// + Checa se é válida a estrutura do sistema de arquivos.
// + Pega a quantidade de setores por cluster.
// + Pega o tamanho do diretório raiz. Ou seja, pega o número 
//    máximo de entradas.
// ...

// Root file system for the boot disk.

    if ((void *) root == NULL){
        panic("fsLoadFile: root\n");
    }
    if (root->used != TRUE || root->magic != 1234){
        panic ("fsLoadFile: root validation\n");
    }

// Sectors Per Cluster.
// #todo: We need a max limit.
    Spc = (int) root->spc;
    if (Spc <= 0){
        panic ("fsLoadFile: Spc\n");
    }

// Max entries 
// Número de entradas no rootdir.
// #bugbug: 
// Devemos ver o número de entradas no diretório corrente.
// Por enquanto so aceitamos 512.
    if (root->dir_entries != MaxEntries){
        panic ("fsLoadFile: root->dir_entries\n"); 
    }

    // ...

// Continua ... 
// Pegar mais informações sobre o sistema de arquivos.
// Busca simples pelo arquivo no diretório raiz.
// #todo: 
// Essa busca pode ser uma rotina mais sofisticada. Uma função auxiliar.
// Primero caractere da entrada:
// 0 = entrada vazia.
// $ = entrada de arquivo deletado.
// outros ...
// ATENÇÃO:
// Na verdade a variável 'root' é do tipo short.

// file name
    if ((void *) file_name == NULL){
        printk ("fsLoadFile: file_name\n");
        goto fail;
    }
    if (*file_name == 0){
        printk ("fsLoadFile: *file_name\n");
        goto fail;
    }

//#debug
//vamos mostrar a string.
    //printk ("fsLoadFile: file_name={%s}\n", file_name);

// name size.
// Se o tamanho da string falhar, vamos ajustar.
    FileNameSize = (size_t) strlen(file_name);
    //if (FileNameSize <= 0)
    //    goto fail;
    if (FileNameSize > 11)
    {
        printk ("fsLoadFile: Name size %d\n", FileNameSize ); 
        //FileNameSize = 11;
        //return 1; //fail
        goto fail;
    }

// File size
// #bugbug
// Pegar o tamanho do arquivo e comparar com o limite do buffer.
// #bugbug: 
// Essa rotina so pega o tamanho dos arquivos que estao 
// no diretorio raiz.
// Comparando nosso tamanho obtido com o tamanho do buffer.
// Como a rotina de pegar o tamanho so pega no diretorio raiz
// por enquanto, entao vamos apenas emitir um alerta que 
// o tamanho do arquivo eh maior que o buffer, servira para debug.
// Isso porque em todas as tentativas de pegar o tamanho do arquivo
// fora do root, retornara 0.
// #todo
// Precisamos usar as estruturas de diretorio e 
// as estruturas de buffer.
    
    FileSize = 
        (unsigned long) fsGetFileSize( 
                           (unsigned char *) file_name, 
                           (unsigned long) dir_address );

// Queremos abrir um arquivo que contenha 0 bytes.
// New file.
// #bugbug: Is this a problem?
    if (FileSize == 0)
    {
        debug_print("fsLoadFile: [FIXME] FileSize\n");
        printk     ("fsLoadFile: [FIXME] FileSize %d\n", FileSize);
        //goto fail;
    }

// The file size can't be bigger than the buffer size.
    if (FileSize >= BufferSizeInBytes)
    {
        debug_print("fsLoadFile: [FIXME] Buffer Overflow\n");
        printk("fsLoadFile: [FIXME] FileSize %d BufferSizeInBytes %d\n",
            FileSize, BufferSizeInBytes );
        goto fail;
    }

// We are opening the root dir.

    //if ( file_name[0] == '/' && size == 1 )
    //{
    //}

// Search dirent
// Compare
// #bugbug
// #todo:
// Para a variável 'max' estamos considerando o número de
// entradas no diretório raiz. Mas precisamos considerar
// o número de entradas no diretório atual.
// >> Para isso vamos precisar de uma estrutura de diretório
// >> talvez a mesma usada em arquivos. (file)
// Descrição da rotina:
// Procura o arquivo no diretório raiz.
// Se a entrada não for vazia.
// Copia o nome e termina incluindo o char '0'.
// Compara 'n' caracteres do nome desejado, 
// com o nome encontrado na entrada atual.
// Se for encontrado o nome, então salvamos o número da entreda.
// Cada entrada tem 16 words.
// (32/2) próxima entrada! (16 words) 512 vezes!

    i=0; 
    while (i<MaxEntries)
    {
        // Se a entrada não começar com a short '0x0000'.
        if ( __dir[z] != 0x0000 )
        {
            //#bugbug: Estamos movendo shorts e não chars.
            //memcpy ( tmpName, &__dir[z], (FileNameSize/2) );
            memcpy( tmpName, &__dir[z], FileNameSize );
            tmpName[FileNameSize] = 0;

            Status = kstrncmp( file_name, tmpName, FileNameSize );
            if (Status == 0)
            {
                SavedDirEntry = i; 
                goto __found;
            }
        }; 
        z += 16;
        i++;
    }; 

// Not found.
// Saiu do while. 
// O arquivo não foi encontrado.
//__notFound:
    debug_print("fsLoadFile: File not found\n");
    printk     ("fsLoadFile 1: %s not found\n", file_name );  
    goto fail;

// Found
// O arquivo foi encontrado.
__found:

    // #debug
    // printk ("file FOUND!\n");
    // refresh_screen();
    // while(1){}

// Cluster
// Get the initial cluster. 
// Check cluster Limits.
// (word). 
// (0x1A/2) = 13.
// Checar se 'cluster' está fora dos limites.
// +São 256 entradas de FAT por setor. 
// +São 64 setores por FAT. 
// Isso varia de acordo com o tamanho do disco.
// O número máximo do cluster nesse caso é (256*64).
// #todo
// Na verdade os dois primeiros clusters estão indisponíveis.

// First cluster
    cluster = (unsigned short) __dir[ z+13 ];
    if ( cluster <= 0 || cluster > 0xFFF0 )
    {
        debug_print("fsLoadFile: Cluster limits\n");
        printk     ("fsLoadFile: Cluster limits %x\n", cluster );
        goto fail;
    }

// ========================================
// FAT
// Carrega fat na memória.
// #bugbug: 
// Não devemos carregar a FAT na memória toda vez que 
// formos carregar um arquivo. 
// Talvez ela deva ficar sempre na memória.
// Precisamos de estruturas para volumes que nos dê esse 
// tipo de informação
// #todo
// precisamos de uma estrutura que nos diga o tamanho
// da FAT para o volume que estamos atuando.
// Aliás, qual é o volume?
//loadFAT:
// 246?

    fs_load_fat( VOLUME1_FAT_ADDRESS, VOLUME1_FAT_LBA, 246 );

// Load clusters.
// Carregar o arquivo, cluster por cluster.
// #todo: 
// Por enquanto, um cluster é igual à um setor, 512 bytes.
// Loop de entradas na FAT.
// #todo: 
// Esse loop é provisório, while pode ser problema.
// #todo
// Tabela temporária para salvar os números dos clusters
// usados pelo arquivo.
// ?? Qual será o tamanho dessa tabela ??

    //unsigned short tmp_table[1024];
    
    //#todo: Use while()

// #todo
// Create a helper function like this one.

    // int fsReadClusterChain ( char *file_address, short first_cluster, char *fat_address ){}

//
// == Load cluster chain ================
//

    int nreads=0;   // number of clusters read.

__loop_next_entry:

// #todo
// Esse while é para o caso de termos mais de um setor por cluster.
// Mas não é nosso caso até o momento.

/*
	while(1)
	{
	    //Calcula.
		//Primeiro setor do cluster. 
		S = fatClustToSect(cluster, Spc, VOLUME1_DATAAREA_LBA); 
		
		//Carrega 'x' setores começando de S.
		fatLoadCluster( S, file_address, Spc);
		
		//Pegar o próximo cluster na FAT.
		//O cluster atual contém o número do próximo.
		cluster = fat[cluster];
		
		//Ver se o cluster carregado era o último.
	    if(cluster == 0xFFFF || cluster == 0xFFF8){ goto done; };
		//Nothing.
    };
 */


// #todo
// Poderia ter uma versão dessa função para ler
// um dado número de setores consecutivos.

// #todo
// #importante
// Esse é o momento em que vamos registrar na estrutura de arquivos
// quais foram os clusters usados pelo arquivo, para assim podermos
// salvar somente somente os setores modificados e não sempre o 
// arquivo todo.
// >> Mas não temos a estrutura de arquivos no momento.
// Mesmo assim, talvez ja possomos salvar os números dos clusters
// em uma tabela temporária.

    //tmp_table[tmp_table_index] = cluster;
    //tmp_table_index++;

// Some invalid address.
// We can not load a file in the same core addresses.

    is_valid = (int) __check_address_validation((unsigned long) Buffer);
    if (is_valid != TRUE){
        panic ("fsLoadFile: Not a valid address\n");
    }

//
// Read LBA
//

// Caution!
// Read lba.
// Increment buffer base address.
// Pega o próximo cluster na FAT.
// Configura o cluster atual.
// Ver se o cluster carregado era o último cluster do arquivo.
// Vai para próxima entrada na FAT.
// See: read_r.c
// #bugbug
// Vector overflow?
// #todo
// #important
// Check this value against a vector limit.
// We already did that a single time before.
// #bugbug: Não pode ser menor que 0, pois é unsigned short.

    // #bugbug: O marcador é 0xFFF8
    // Mas no caso de ser um arquivo de apenas um setor
    // então podemos ter aqui o marcador de fim de arquivo.
    //if ( cluster <= 0 || cluster > 0xFFF8 )
    if ( cluster <= 0 || cluster > 0xFFF0 )
    {
        printk("nreads={%d} cluster{%x}\n",nreads,cluster);
        panic("fsLoadFile: fat[] vector limits.\n");
    }

// Read disk.
    read_lba ( 
        Buffer, 
        ( VOLUME1_DATAAREA_LBA + cluster -2 ) ); 

// Update buffer pointer.
    Buffer = (unsigned long) (Buffer + SectorSize); 

// Update cluster.
    next = (unsigned short) fat[cluster];
    cluster = (unsigned short) (next & 0xFFFF);

// #todo:
// We found a bad cluster.
    if (cluster == 0xFFF7){
        panic("fsLoadFile: Bad cluster\n");
    }

// ?? done
// ?? message  
// salvar a tabela na estrutura de arquivo.
// Onde está a estrutura de arquivos ??
// Em que momento ela é criada ?
// #bugbug: tem arquivo carregado pelo kernel
// sem ter sido registrado na estrutura do processo kernel.

    if ( cluster == 0xFFFF || cluster == 0xFFF8 )
    {
        nreads++;
        // File size limit:
        // We have a limit when allocating memory for an image. 
        // See: alloc_memory_for_image_and_stack() in process.c.
        // See: gramado/config.h
        // bytes per sector = 512.
        // sectors per cluster  = 1.
        if ( (nreads/2) > IMAGESIZE_LIMIT_IN_KB )
        {
            panic("fsLoadFile: nreads\n");
        }
        // 0 = OK
        return (unsigned long) 0; 
    }

    nreads++;
    goto __loop_next_entry;

fail:
    debug_print("fsLoadFile: [FAIL]\n");
    printk     ("fsLoadFile: [FAIL] file={%s}\n", file_name );
    refresh_screen();
    return (unsigned long) 1;
}

// Not tested yet
unsigned long 
fsLoadFile2 ( 
    struct file_context_d *fc, 
    unsigned char *file_name )
{
    if ((void*) fc == NULL){
        debug_print("fsLoadFile2: fc\n"); 
        return 0;
    }

    if ((void*) file_name == NULL){
        debug_print("fsLoadFile2: file_name\n"); 
        return 0;
    }
    if (*file_name == 0){
        debug_print("fsLoadFile2: *file_name\n"); 
        return 0;
    }

    fc->file_name = file_name;

    return (unsigned long) fsLoadFile ( 
               (unsigned long)   fc->fat_address,
               (unsigned long)   fc->dir_address,
               (int)             fc->dir_entries,
               (unsigned char *) fc->file_name, 
               (unsigned long)   fc->file_address,
               (unsigned long)   fc->buffer_limit );
}

// -------------------------------------
// fsLoadProgramFromGRAMADO:
// Load an image from GRAMADO/.
// IN:
// + program_name: Program name.
// + buffer: Pre-allocated buffer.
// + buffer_size_in_bytes: Buffer size in bytes.
unsigned long 
fsLoadProgramFromGRAMADO (
    char *program_name,
    unsigned long buffer,
    unsigned long buffer_size_in_bytes )
{
// Load an image from PROGRAM/

    unsigned long Status=1; // fail

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
        (unsigned long) fsLoadFile( 
                            VOLUME1_FAT_ADDRESS, 
                            programs_directory_address, // onde procurar 
                            512,    //#bugbug: number of entries.
                            program_name, 
                            (unsigned long) buffer,  // buffer
                            (unsigned long) buffer_size_in_bytes );  // buffer limits in bytes
    if (Status!=0){
        goto fail;
    }
    return 0;

fail:
    refresh_screen();
    return 1;
}

// -------------------------------------
// fsLoadProgramFromDE:
// Load an image from DE/.
// IN:
// + program_name: Program name.
// + buffer: Pre-allocated buffer.
// + buffer_size_in_bytes: Buffer size in bytes.
unsigned long 
fsLoadProgramFromDE (
    char *program_name,
    unsigned long buffer,
    unsigned long buffer_size_in_bytes )
{
// Load an image from PROGRAM/

    unsigned long Status=1; // fail

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
        (unsigned long) fsLoadFile( 
                            VOLUME1_FAT_ADDRESS, 
                            programs_directory_address, // onde procurar 
                            512,    //#bugbug: number of entries.
                            program_name, 
                            (unsigned long) buffer,  // buffer
                            (unsigned long) buffer_size_in_bytes );  // buffer limits in bytes
    if (Status!=0){
        goto fail;
    }
    return 0;

fail:
    refresh_screen();
    return 1;
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
//    0 ---> ok.
// != 0 ---> fail
// #todo
// is address virtual or physical?
// Change this name to pa or va.
// Change to buffer_size_in_bytes ?

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

// Onde carregaremos o diretório.
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

    unsigned long DirBufferSizeInBytes = (unsigned long) (512*32); //4pages
    //unsigned long DirBufferSizeInBytes = (unsigned long) (4096);

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
                Ret = fsLoadFile ( 
                          (unsigned long) VOLUME1_FAT_ADDRESS,  // fat address
                          (unsigned long) __src_buffer,         // dir address. onde procurar. 
                          (unsigned long) MaxEntries,          // #bugbug: Number of entries. 
                          (unsigned char *) name_buffer,       // nome 
                          (unsigned long) __dst_buffer,         // addr. Onde carregar.
                          (unsigned long) __file_buffer_size_in_bytes );  // tamanho do buffer onde carregar.             
                // ok.
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
                Ret = fsLoadFile ( 
                          (unsigned long) VOLUME1_FAT_ADDRESS,  // fat address
                          (unsigned long) __src_buffer,         // dir address. onde procurar.
                          (int) MaxEntries,                    // #bugbug: Number of entries.  
                          (const char *) name_buffer,          // nome que pegamos no path 
                          (unsigned long) __dst_buffer,         // onde carregar. 
                          (unsigned long) DirBufferSizeInBytes );                             // tamanho do buffer onde carregar.
  
                // ok.
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
    debug_print("fs_load_path: Fail\n");
    printk     ("fs_load_path: Fail\n");
    refresh_screen();
    return (-1);
}

// -------------

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
    //taskswitch_lock ();
    //scheduler_lock ();
    Ret = 
        (int) fsLoadFile ( 
                  (unsigned long) VOLUME1_FAT_ADDRESS,                     // fat cache address
                  (unsigned long) current_target_dir.current_dir_address,  // src dir address 
                  (int) FAT16_ROOT_ENTRIES, //#bugbug: Number of entries.  // number of entries.
                  (const char *) current_target_dir.name,                  // file name 
                  (unsigned long)   current_target_dir.current_dir_address,  // file address
                  (unsigned long) xxxTempFileSize );                                    // #bugbug buffer limit 4KB.
    //scheduler_unlock ();
    //taskswitch_unlock ();
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

static int 
__try_to_load_program_from_special_folder( 
    const char *filename, 
    unsigned long image_va )
{
// Only inside /PROGRAMS/

    unsigned long status = -1;
    unsigned long BUGBUG_IMAGE_SIZE_LIMIT = (unsigned long) (512 * 4096);

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

    status = -1;

    // From GRAMADO/
    if ( *new_filename == '@' ){
        new_filename++;
        fs_fntos((char *) new_filename);
        status = 
        (unsigned long) fsLoadProgramFromGRAMADO( 
                            new_filename, 
                            image_va, 
                            BUGBUG_IMAGE_SIZE_LIMIT );
    // From PROGRAMS/
    }else if ( *new_filename == '#' ){
        new_filename++;
        fs_fntos((char *) new_filename);
        status = 
        (unsigned long) fsLoadProgramFromDE( 
                            new_filename, 
                            image_va, 
                            BUGBUG_IMAGE_SIZE_LIMIT );
    }else{
        goto fail;
    };

// done:
    if (status == 0)
    {
        // OK.
        return 0;
    }

fail:
    return (int) -1;
}

// fs_load_image:
// helper.
// Loading a image given the filename and its virtual address.
// called by copy_process in clone.c right after a
// syscall to 'clone and execute'.
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

//
// Check parameters for image support
//

    if ((void*) filename == NULL){
        panic ("fs_load_image: filename\n");
    }
    if (*filename == 0){
        panic ("fs_load_image: *filename\n");
    }
    path = filename;
    name = filename;

//
// Path support.
//

// Loading image from the cwd.
    if ( path[0] == '.' && 
         path[1] == '/' )
    {
        debug_print ("fs_load_image: [FIXME] Can't execute from cwd\n");
        printk      ("fs_load_image: [FIXME] Can't execute from cwd\n");
        goto fail;
    }

// Loading image from 
// /GRAMADO/ or /PROGRAMS/ directories.

    if ( *filename == '@' || 
         *filename == '#' )
    {
        return (int) __try_to_load_program_from_special_folder( 
                        filename,
                        image_va );
    }

// Loading image from the root directory.
    if (*filename == '/')
    {
        filename++;
        // New name.
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

    Status = (int) search_in_dir(name,dir_va);
    if (Status == 1){ 
        goto __found; 
    }
    debug_print ("fs_load_image: [FAIL] File not found!\n");
    printk      ("fs_load_image: [FAIL] File not found!\n");
    goto fail;

// The file was found into the directory.
__found:

// ============================

// [3]
// #debug

    debug_print ("fs_load_image: [3] Loading the image.\n");
       //printk ("fs_load_image: [3] Loading the image.\n");

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
        (int) fsLoadFile ( 
                  VOLUME1_FAT_ADDRESS, 
                  (unsigned long) dir_va,       //#bugbug: This is only for the root dir.
                  (unsigned long) dir_entries,  //#bugbug: Number of entries. 
                  name, 
                  (unsigned long) image_va,
                  BUGBUG_IMAGE_SIZE_LIMIT );

// ok?
    return (int) Status;
fail:
    // #debug
    // We still need this kind of break,
    // cause the fs infrastructure is still immature.
    panic("fs_load_image: fail\n");
    return (int) -1;
}

