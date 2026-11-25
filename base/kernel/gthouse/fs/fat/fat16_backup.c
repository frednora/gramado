// fat16.c
// FAT16 file system support.
// ring 0.
// Created by Fred Nora.

#include <kernel.h>

// Boot partition.
// These are global variables.
// see: Definitions in fatlib.h
// #todo: Explain it better.
int g_fat_cache_saved=FALSE;
int g_fat_cache_loaded=FALSE;

struct fat_d  *bootvolume_fat;

static int __check_address_validation(unsigned long address);

//
// == Cluster list ===============================
//

// Lista de clusters.
// Isso permite salvar um arquivo com 32 mil clusters ??
// #todo: Em que momento essa lista foi inicializada??
// #bugbug: 
// This list has 32 KB size.
// #bugbug
// Checar se essa 'e a quantidade limite de 
// entradas que podemos acessar na fat.
// Ou e' apenas o limite da lista.

// Cluster chain buffer for FAT16 files.
// Helps you read the file’s contents by tracking which clusters to load.
// 65,536 bytes total. 32,768 slots. (The list has 32KB)
// Option 1: Increase the Static Array Size
// Option 2: Use Larger Cluster Sizes
// Option 3: Dynamic Allocation
// Option 4: Hybrid Approach
// #ps: This is a very large array!
unsigned short fat16ClustersToSave[CLUSTERS_TO_SAVE_MAX];


struct fat16_directory_entry_d *vol_label_directory_entry;

// -------------------------------

/*
// #todo
// Returns 1 if entry is a directory, 0 otherwise.
int fat16_is_directory(struct fat16_directory_entry_d *entry);
int fat16_is_directory(struct fat16_directory_entry_d *entry)
{
    if ((void*) entry == NULL) return 0;
    return (entry->Attributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
}
*/


/*
// #todo
// Returns 1 if entry is a regular file, 0 otherwise.
int fat16_is_regular_file(struct fat16_directory_entry_d *entry);
int fat16_is_regular_file(struct fat16_directory_entry_d *entry)
{
    if ((void*) entry == NULL) return 0;
    return (!(entry->Attributes & FILE_ATTRIBUTE_DIRECTORY) &&
            !(entry->Attributes & FILE_ATTRIBUTE_VOLUME_LABEL));
}
*/

// WORKER
// Called by fat16LoadFile
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

    unsigned long BackbufferCurrrentVA = (unsigned long) display_get_backbuffer_va();

// Parameter
// fat, rootdir, base kernel, lfb, backbuffer ...
    if (address == VOLUME1_FAT_ADDRESS_VA)           { Status=FALSE; }
    if (address == VOLUME1_ROOTDIR_ADDRESS_VA)       { Status=FALSE; }
    if (address == KERNELIMAGE_VA)                   { Status=FALSE; }
    if (address == FRONTBUFFER_VA)       { Status=FALSE; }
    if (address == BackbufferCurrrentVA){ Status=FALSE; }
    // ...

// #todo
// We can check against some other core addresses.

//done:
    return (int) Status;
}

/*
 * fat16LoadFile:
 *    It loads a file into the memory.
 *    Copy from the disk to the memory.
 *    Probably called by sys_open().
IN:
    fat_address          = FAT address.
    dir_addresss         = Directory address.
    dir_entries          = Number of entries in the given directory.
    file_name            = File name.
    buffer               = Where to load the file. The pre-allocated buffer.
    buffer_size_in_bytes = Maximum buffer size.
OUT: 
    0 = ok
    -1 = fail 

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

int 
fat16LoadFile ( 
    unsigned long fat_address,
    unsigned long dir_address,
    int dir_entries,
    const char *file_name, 
    unsigned long buffer,
    unsigned long buffer_size_in_bytes )
{
// Limits:
// Remember: This function is already able to load 
// executables with 200KB or more, just like ds00.

    int Status = -1;
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
    //debug_print ("fat16LoadFile:\n");
    printk("fat16LoadFile: %s\n", file_name);

// Parameters:
// Fat address and dir address.
// Vectors of 'short'
    if (fat_address == 0){
        panic("fat16LoadFile: fat_address\n");
    }
    if (dir_address == 0){
        panic("fat16LoadFile: dir_address\n");
    }

// Addresses
    unsigned short *  fat = (unsigned short *) fat_address;
    unsigned short *__dir = (unsigned short *) dir_address;

// #debug
// We only support one address for now.
    if (fat_address != VOLUME1_FAT_ADDRESS){
        panic("fat16LoadFile: Not valid FAT address\n");
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
        panic ("fat16LoadFile: MaxEntries limits\n");
    }

    if (DirEntries > MaxEntries){
        panic ("fat16LoadFile: DirEntries\n");
    }

// #test: 
// Used only for debug. 512 entries in rootdir.
    if (DirEntries != MaxEntries){
        panic ("fat16LoadFile: [DEBUG] DirEntries IS LESS THE 512\n");
    }

// The pre-allocated buffer.
// Buffer size in bytes
// Max = 512 pages.

    if (BufferSizeInBytes == 0){
        panic("fat16LoadFile: BufferSizeInBytes = 0\n");
    }
    // #test
    // 2048 KB.
    if ( BufferSizeInBytes > (512*4096) ){
        panic("fat16LoadFile: BufferSizeInBytes limits\n");
    }

// Root file system structure.
// + Checa se é válida a estrutura do sistema de arquivos.
// + Pega a quantidade de setores por cluster.
// + Pega o tamanho do diretório raiz. Ou seja, pega o número 
//    máximo de entradas.
// ...

// Root file system for the boot disk.

    if ((void *) root == NULL){
        panic("fat16LoadFile: root\n");
    }
    if (root->used != TRUE || root->magic != 1234){
        panic ("fat16LoadFile: root validation\n");
    }

// Sectors Per Cluster.
// #todo: We need a max limit.
    Spc = (int) root->spc;
    if (Spc <= 0){
        panic ("fat16LoadFile: Spc\n");
    }

// Max entries 
// Número de entradas no rootdir.
// #bugbug: 
// Devemos ver o número de entradas no diretório corrente.
// Por enquanto so aceitamos 512.
    if (root->dir_entries != MaxEntries){
        panic ("fat16LoadFile: root->dir_entries\n"); 
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
        printk ("fat16LoadFile: file_name\n");
        goto fail;
    }
    if (*file_name == 0){
        printk ("fat16LoadFile: *file_name\n");
        goto fail;
    }

//#debug
//vamos mostrar a string.
    //printk ("fat16LoadFile: file_name={%s}\n", file_name);

// name size.
// Se o tamanho da string falhar, vamos ajustar.
    FileNameSize = (size_t) strlen(file_name);
    //if (FileNameSize <= 0)
    //    goto fail;
    if (FileNameSize > 11)
    {
        printk ("fat16LoadFile: Name size %d\n", FileNameSize ); 
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
        debug_print("fat16LoadFile: FileSize\n");
        printk     ("fat16LoadFile: FileSize %d\n", FileSize);
        //goto fail;
    }

// The file size can't be bigger than the buffer size.
    if (FileSize >= BufferSizeInBytes)
    {
        debug_print("fat16LoadFile: Buffer Overflow\n");
        printk     ("fat16LoadFile: FileSize %d BufferSizeInBytes %d\n",
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
    while (i < MaxEntries)
    {
        // Se a entrada não começar com a short '0x0000'.
        if ( __dir[z] != 0x0000 )
        {
            // #bugbug:
            // Maybe here we are not copying the whole file name 
            // from the directory entry.

            // #ps:
            // Maybe this is working because the filename
            // provided by the parameter is formatted
            // in the same way it appears into the FAT entry.

            memcpy( tmpName, &__dir[z], FileNameSize );
            tmpName[FileNameSize] = 0;

            // #test
            // Bugfix
            // memcpy(tmpName, &__dir[z], 11);
            // tmpName[11] = 0;  // Null‑terminate

            Status = gramado_strncmp( file_name, tmpName, FileNameSize );
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
    //debug_print("fat16LoadFile: File not found\n");
    printk     ("fat16LoadFile 1: %s not found\n", file_name );  
    goto fail;

// Found
// O arquivo foi encontrado.
__found:

    // #debug
    // printk ("file FOUND!\n");
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
        debug_print("fat16LoadFile: Cluster limits\n");
        printk     ("fat16LoadFile: Cluster limits %x\n", cluster );
        goto fail;
    }

// #todo
// We can get the attribute
// Directory
// Volume label
// LFN entry
// Hidden file
// Deleted file

    // attribute = DirEntry[11]

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

    //printk("fat16LoadFile: Load fat\n");
    fs_load_fat( VOLUME1_FAT_ADDRESS, VOLUME1_FAT_LBA, 246 );
    printk("fat16LoadFile: Load fat ok\n");

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
        panic ("fat16LoadFile: Not a valid address\n");
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
        panic("fat16LoadFile: fat[] vector limits.\n");
    }

// Read disk

    //printk("read lba:\n");
    int FakeDiskId = 0;  //#todo: fake disk id.
    read_lba ( 
        FakeDiskId,  // #todo: fake disk id.
        Buffer, 
        ( VOLUME1_DATAAREA_LBA + cluster -2 )); 
    //printk("read lba: ok\n");

// Update buffer pointer
    Buffer = (unsigned long) (Buffer + SectorSize); 

// Update cluster.
    next = (unsigned short) fat[cluster];
    cluster = (unsigned short) (next & 0xFFFF);

// #todo:
// We found a bad cluster
    if (cluster == 0xFFF7){
        panic("fat16LoadFile: Bad cluster\n");
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
        if ( (nreads/2) > CONFIG_IMAGESIZE_LIMIT_IN_KB )
        {
            panic("fat16LoadFile: nreads\n");
        }

        printk("fat16LoadFile: done\n");

        return (int) 0;          // 0 = OK
    }

    nreads++;
    goto __loop_next_entry;

fail:
    printk("fat16LoadFile: Fail on file={%s}\n", file_name);
    return (int) -1;
}

// fat16SaveFile:
// Salva um arquivo no disco.
// Somente no diretório raiz.
// It was called by sys_write_file() in fs.c.
// It was called by sys_read_file when the file does not exist.
// #obs
// Isso salva um arquivo.
// Também poderia ser usado para criar um arquivo ou diretório ? 
// #todo: #test
// Número máximo de entradas na lista de clusters. 
// Quantas?
// A FAT tem 246 setores, 123 KB
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

// dir_address → Directory buffer address where the file entry will be created.
// dir_entries → Number of entries in that directory buffer.
// file_name → The name of the file to be saved.
// size_in_sectors → Size of the file in sectors (not bytes).
// file_buffer → Pointer to the memory buffer containing the file’s contents.

int
fat16SaveFile ( 
    unsigned long fat_address,
    unsigned long dir_address,
    int dir_entries,
    const char *file_name, 
    unsigned long size_in_sectors,
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

// Directory entry buffer
    char DirEntry[32];

//Entry size in words.
    int EntrySize = 0;
    int Offset = 0;
    int FreeIndex = -1;

    // #debug:
    debug_print ("fat16SaveFile:\n");
    //printk      ("fat16SaveFile:\n");

// Updating fat address and dir address.

    if (fat_address == 0){
        panic("fat16SaveFile: fat_address\n");
    }
    if (dir_address == 0){
        panic("fat16SaveFile: dir_address\n");
    }

// Ex: VOLUME1_FAT_ADDRESS
    unsigned short *fat = (unsigned short *) fat_address;
// Ex: VOLUME1_ROOTDIR_ADDRESS
    unsigned short *__dir = (unsigned short *) dir_address;

// #debug
// #todo
// We only support one address for now.

    if (fat_address != VOLUME1_FAT_ADDRESS)
    {
        panic("fat16SaveFile: fat_address\n");
    }

// Filename

    if ((void *) file_name == NULL){
        debug_print("fat16SaveFile: [ERROR] file_name\n");
        printk     ("fat16SaveFile: [ERROR] file_name\n"); 
        goto fail;
    }
    if (*file_name == 0){
        debug_print("fat16SaveFile: [ERROR] *file_name\n");
        printk     ("fat16SaveFile: [ERROR] *file_name\n"); 
        goto fail;
    }
    // serial_printk ("fat16SaveFile: %s\n", file_name);

// #bugbug
// Esse endereço eh valido?

/*
    printk ("name address = %x \n", &file_name );
    printk ("name    = %s \n", file_name ); 
    printk ("size    = %d \n", size_in_sectors );
    printk ("nbytes  = %d \n", size_in_bytes );
    printk ("address = %x \n", file_address );
    printk ("flag    = %x \n", flag );
    printk ("entries = %d \n", dir_entries );
 */

// How name direntries this directory have?
    if ( dir_entries == 0 || 
         dir_entries > FAT16_ROOT_ENTRIES )
    {
        panic ("fat16SaveFile: [FAIL] max dir entries");
    }

// size_in_sectors
// #todo: 
// precisamos implementar um limite para o tamanho do arquivo,
// principamente nessa fase de teste.

// #bugbug
// Limite provisorio
// Size in sectors.

    if (size_in_sectors > MaxFileSizeInSectors)
    {
        debug_print("fat16SaveFile: [FIXME] Size in sectors\n");
        printk     ("fat16SaveFile: [FIXME] Size in sectors = %d\n", 
            size_in_sectors ); 
        goto fail;
    }


// Clean the buffer
    memset(DirEntry, 0, 32);


// Load root dir and FAT.
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
            // Encontrado todos os espaços livres que o arquivo precisa.
            // Marca o fim.
            // #importante: 
            // Se der certo, saímos do loop.
            // #bugbug: Esse size deve ter um limite.
            // file_size = file size in sectors, (clusters??)
            if (size_in_sectors == 0){
                fat16ClustersToSave[j] = (unsigned short) 0xfff8; 
                goto save_file;
            }

            //salva um endereço livre
            //salvamos um índice na fat dentro da lista
            //incrementa a lista

            fat16ClustersToSave[j] = (unsigned short) c; 
            j++; 

            // Decrement the number of sectors we need to find
            size_in_sectors--; 
        };

        c++;    // Incrementa o deslocamento na fat. 
        i++;    // Incrementa a quantidade de busca.
    }; 

// Fail
// Nossa busca por clusters livres dentro da fat não deu certo.
// Provavelmente não encontramos uma quantidade suficiente.

out_of_range:  
    printk ("fat16SaveFile: No free cluster \n");
    goto fail;

    // #debug
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
    //printk("fat16SaveFile: save_file: \n"); 

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

    if (FreeIndex == -1){
        printk ("fat16SaveFile: [FAIL] No empty entry\n");
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
    Offset = (int) (FreeIndex * EntrySize);

// FAT16_DIRENTRY_SIZE = 32
    memcpy ( &__dir[Offset], DirEntry, 32 );

// reset
// Reiniciamos o controlador antes de usarmos.

//resetIDE:

    //#debug
    //printk("fat16SaveFile: reset and while\n"); 

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

    unsigned int L_current_ide_port = 
        (unsigned int) ata_get_current_ide_port_index();

// Loop
// Pegamos o atual na lista.
// Se ele eh o sinalizador de fim de lista, 
// entao entao colocamos o sinalizador de fim de arquivo
// no offset indicado pelo penultimo elemento da lista.

    while (TRUE){

        next = fat16ClustersToSave[i];

        // #debug
        printk ("fat16SaveFile: [DEBUG] next={%x}\n", next);

        // O next é o marcador de fim de lista.
        if (next == 0xFFF8){

            // Então pegamos o penúltimo e colocamos
            // o finalizador lá nesse offset.
            next = fat16ClustersToSave[i-1];
            fat[next] = 0xFFF8;
            goto do_save_dir_and_fat;

        // Se não é assinatura ainda.
        } else {

            // #bugbug
            // Estamos usando isso sem nem mesmo checar seu valor.
            // precisamos de limtes aqui. Para evitarmos overflow.
            
            // Grava na fat o endereço do próximo cluster
            fat[next] = fat16ClustersToSave[i+1];
 
            //#debug 
            //printk("write_lba\n");
            
            // Wait and write!
            
            // ata_get_current_ide_port_index()
            disk_ata_wait_irq( ATACurrentPort.g_current_ide_port_index );

            // Low level worker
            //grava - Aqui next esta certo!
            //write_lba ( (unsigned long) address, VOLUME1_DATAAREA_LBA + next -2 );
            ataWriteSector ( 
                L_current_ide_port,
                (unsigned long) address, 
                (unsigned long) ( VOLUME1_DATAAREA_LBA + next -2), 
                0, 
                0 );

            address += 512;
        }; 

        i++;  // next value in the list

        // #bugbug
        // Limitando o tamanho do arquivo a 16 entradas.
        // Why??

        // #bugbug
        // Limite provisorio.
        if (i > 16){
            debug_print ("fat16SaveFile: [FIXME] write sectors limit\n");
            printk      ("fat16SaveFile: [FIXME] write sectors limit\n");
            goto fail;
        }

        // ??
    };

// FAIL
    debug_print ("fat16SaveFile: Loop fail\n");
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

    //debug_print ("fat16SaveFile: [DEBUG] do_save_dir_and_fat\n");
    //printk("fat16SaveFile: do_save_dir_and_fat:\n"); 

// Save rootdir into the disk
// #ps: ATA disk only
// #bugbug: We need to save a directory, not the root.
// IN: 
// root dir address, root dir lba, root dir size in sectors.

    atadsk_save_rootdir( 
        VOLUME1_ROOTDIR_ADDRESS, 
        VOLUME1_ROOTDIR_LBA, 
        32
    );

// Sinalizing that the fat cache needs to be saved
    fs_fat16_cache_not_saved();

    return 0;  // OK

fail:
    debug_print("fat16SaveFile: fail\n");
    printk     ("fat16SaveFile: fail\n");
    return (int) -1;
}

// #test
// #bugbug
// We're using the pointer 'vol_label_directory_entry' for
// directory entry structure. Maybe we can use a pointer as parameter 
// for the function.
//void test_fat16_find_volume_info(struct fat16_directory_entry_d *dir_entry)
void test_fat16_find_volume_info(void)
{
// #test
// This is a test.
// No not call this routine yet.

    char name_buffer[16];
    register int i=0;

    vol_label_directory_entry = 
        (void*) kmalloc( sizeof(struct fat16_directory_entry_d) );
    if ((void*) vol_label_directory_entry == NULL){
        printk("on kmalloc\n");
        goto fail;  
    }

// Get info
// IN:
//   + entry = Put the info here in this structure.
//   + directory_va = 
//   + entry_number = desired Index. 
//   + entry_max = max index. (limit=512)
    int status = -1;
    status = (int) fat16_get_entry_info( 
        vol_label_directory_entry,
        VOLUME1_ROOTDIR_ADDRESS,
        0,
        512 );
 
    if (status != 0)
        goto fail;
 

    // Clear local buffer and put the name into it    
    memset(name_buffer, 0, 16);
    for (i=0; i<(8+3); i++){
        name_buffer[i] = vol_label_directory_entry->FileName[i];
    };

// Show info
    printk("Disk name: {%s}\n",name_buffer);
    return;

fail:
    return;
}

// fat16_get_entry_info:
// IN:
//   + entry = Put the info here in this structure.
//   + directory_va = 
//   + entry_number = desired Index. 
//   + entry_max = max index. (limit=512)
// OUT:
// 0 = OK
// < 0 = fail
int 
fat16_get_entry_info(
    struct fat16_directory_entry_d *entry,
    unsigned long directory_va,
    int entry_number,
    int entry_max )
{
// #test
// Get the entry info

    struct fat16_directory_entry_d *p;  // Local pointer
    register int i=0;

    p = entry;

    if ((void*) p == NULL){
        panic("fat16_get_entry_info: p\n");
    }
    if (directory_va == 0)
        panic("fat16_get_entry_info: directory_va\n");
    if (entry_number < 0 || entry_number > 512)
        panic("fat16_get_entry_info: entry_number\n");
    if (entry_max < 0 || entry_max > 512)
        panic("fat16_get_entry_info: entry_max\n");


    size_t entry_size = sizeof(struct fat16_directory_entry_d);
    unsigned long pos_address = 
      (unsigned long) (directory_va + (entry_size*entry_number)); 
    char *src = (char *) pos_address;
    char *dst = (char *) p;

    for (i=0; i<entry_size; i++){
        dst[i] = src[i];
    };

    return 0;

fail:
    return (int) -1;
}

// Converts a FAT16 8.3 name into a human‑readable string (FILE.TXT).
// Credits: Hoppy OS
void 
from_FAT_name (
    char *src, 
    char *dst )
{
    register int i=0;
    int j=0;
    int k=0;

// #todo: 
// debug messages.
 
    if ((void *) src == NULL){ return; }
    if ((void *) dst == NULL){ return; }

    if (*src == 0){ return; }
    if (*dst == 0){ return; }

// dirty
// pra saber o tamanho do nome exluindo os espaços.

    for (j=7; j >= 0 && src[j] == 0x20; j--)
    {
        // Nothing
    };

    k=0;

// j eh o tamanho do nome calculado anteriormente.
// copia esse nome.

    for ( i=0; i<=j; i++ )
    {
        dst[k++] = src[i];
    };

    if (*src != '.')
    {
        dst[k++] = '.';
    }

// dirty.
// pra saber o tamanho da extensao, excluindo os espaços.
    for ( j=10; 
          j >= 8 && src[j] == 0x20; 
          j-- )
    {
        // Nothing
    };
    
    //
    if (j == 7) {
    
        if (k == 1){
           dst[k]=0;
        } else {

            if (dst[0] == '.'){
                dst[k]=0;
            } else {
                dst[k-1]=0;
            };
        };
    
    } else {
        for (i=8; i<=j; i++)
        {
            dst[k++] = src[i];
        };
        dst[k++] = 0;
    };
}

// Converts a human string into FAT16 8.3 format 
// (pads with spaces, splits extension).
// Credits: Hoppy OS
void 
to_FAT_name (
    char *src,
    char *dst )
{
    register int i=0;
    char *ptr;

    // Parent directory
    if ( !strcmp(src,"..") ) {
        strcpy(dst,src);
        // #todo: use kernel_strcpy_safe(...)
        i=2;
    
    // Current directory
    } else if (!strcmp(src,".")) {
        strcpy(dst,src);
        // #todo: use kernel_strcpy_safe(...)
        i=1;
 
    // Regular file
    } else {

        ptr = src;
        i=0;
        
        // Começamos com o nome.
        while (i<8 && *ptr && *ptr != '.') 
        {
            dst[i++] = *ptr++;
        };

        // Completa com ' ' ate 8.
        while (i<8){
            dst[i++] = 0x20;
        };

        if (*ptr == '.') { 
            ptr++;
        }
        
        // Agora a extensao
        while (i<11 && *ptr)
        {
            dst[i++] = *ptr++;
        };
    };

// Completa com espaço ate o fim.
// 'i' indica o offset de onde devemos começar.
// Isso tambem vai completar com espaço quando a extensao
// tiver menos que 3 chars.

    while (i<11){
        dst[i++] = 0x20;
    };
}

// fsGetFileSize: 
// #bugbug: Isso dá problemas na máquina real.
// Essa rotina é chamada pela função fopen, por isso precisamos dela.
// Pega o tamanho de um arquivo que está no diretório raiz.
// #todo: 
// Podemos alterar para pegar de um arquivo 
// que esteja no diretório alvo.
// #todo:
// Antes de carregar um arquivo o sistema de arquivos
// precisa preencher uma estrutura com informações sobre ele. 
// se já existir um registro é melhor.
// #bugbug
// Estamos com problemas na string do nome.
// #bugbug
// Loading the root dir everytime.
// #todo
// is dir_address virtual or physical?
// Change this name to dir_pa or dir_va.

// fsGetFileSize:
// Retrieves the size of a file located in the root directory (FAT16).
// Limitation: Currently only works for root dir entries (512 max).
// Used by fopen() to validate buffer allocation.
// Known issue: May fail on real hardware due to string handling quirks.

unsigned long 
fsGetFileSize ( 
    unsigned char *file_name, 
    unsigned long dir_address )
{
    unsigned long FileSize=0;    // 64bit
    unsigned int intFileSize=0;  // 32bit

    register int i=0;
    unsigned long MaxNumberOfEntries = 64;  // Dummy value
    unsigned long z = 0;       //Deslocamento do rootdir 
    unsigned long n = 0;       //Deslocamento no nome.

    // #bugbug
    // Estamos com problemas na string do nome.
 
    char NameX[13];
    int CmpStatus = (-1);

    int Spc=0;

// #importante:
// Poderíamos usar malloc ou alocador de páginas ??
// #todo: 
// Devemos carregar o diretório alvo.
// VOLUME1_ROOTDIR_ADDRESS;
    unsigned short *Dir = (unsigned short *) dir_address;

// #todo: Devemos carregar o diretório atual.
//unsigned long current_dir_address = (unsigned long) Newpage();
//#todo: devemos checar se o endereço é válido.
	//unsigned short *current_dir = (unsigned short *) current_dir_address;	
// #todo: 
// devemos chamar uma função que carregue um diretório no endereço passado 
//via argumento.
//...

    // debug_print ("fsGetFileSize: $\n");

    if ((void*) file_name == NULL){
        printk("fsGetFileSize: file_name\n");
        goto fail;
    }
    if (*file_name == 0){
        printk("fsGetFileSize: *file_name\n");
        goto fail;
    }
    if (dir_address == 0){
        printk("fsGetFileSize: dir_address\n");
        goto fail;
    }

//
// ## ROOT ##
//

//Carrega o diretório raiz na memória.
//loadRoot:

    // #debug
    // printk ("fsGetFileSize: Loading root..\n"); 

// #bugbug
// pega o tamanho de um arquivo que está no diretório raiz.
// #todo: 
// podemos alterar para pegar de um arquivo que esteja no diretório alvo.	
// #bugbug
// Estamos chamando isso toda vez que
// tentamos abrir um arquivo.

// Carregando o diretório raiz.
// #bugbug
// Case seja o diretório raiz ...
// e essa for a primeira vez que estamos carregando um arquivo.
// #todo: FIXME

// #bugbug
// Why are we loading the root?
// Why are we loading only for the root?

    if (dir_address == VOLUME1_ROOTDIR_ADDRESS)
    {
        // #bugbug: We can not do this everytime the function is called
        fs_load_rootdir(VOLUME1_ROOTDIR_ADDRESS, VOLUME1_ROOTDIR_LBA, 32);
    }

// #todo:
// precisamos na verdade carregarmos o diretório corrente.
// Continua ... 
// Pegar mais informações sobre o sistema de arquivos.
// #obs
// Checa se é válida a estrutura do sistema de arquivos.
// A intenção é obtermos a quantidade de entradas no diretório raiz.
// #bugbug: 
// Mas isso deveria ser feito para o diretório atual.

//
// == root filesystem structure ===============================
//

    if ((void *) root == NULL){
        panic ("fsGetFileSize: [FAIL] No root file system!\n");
    }else{

        // Setores por cluster
        Spc = root->spc;
        if (Spc <= 0){ panic("fsGetFileSize: [FAIL] spc\n"); }

        // Max entries ~ Número de entradas no rootdir.
        // #bugbug: 
        // Devemos ver o número de entradas no diretório raiz
        // #bugbug: Esse valor não é válido para todos os diretórios
        // por isso temos que usar estruturas para gerenciar
        // diretórios e entradas.

        MaxNumberOfEntries = root->dir_entries;
        // #ps: [wrong] We are using unsigned long.
        if (MaxNumberOfEntries <= 0){ 
            panic("fsGetFileSize: [FAIL] max root entries\n");
        }

        // More?! 
        // ...
    };

//
// file name
//

// #debug
// vamos mostrar a string.
    //printk ("fsGetFileSize: file_name={%s}\n", file_name);

// Busca simples pelo arquivo no diretório raiz.
// todo: Essa busca pode ser uma rotina mais sofisticada. 
// Uma função auxiliar.
// Primero caractere da entrada:
// 0 = entrada vazia.
// $ = entrada de arquivo deletado.
// outros ...
// ATENÇÃO:
// Na verdade a variável 'root' é do tipo short.

// Procura o arquivo no diretório raiz.

//search_file:

// file name limit.
// Se o tamanho da string falhou, vamos ajustar.
    size_t szFileName = (size_t) strlen(file_name); 
    if (szFileName > 11){
        printk ("fsGetFileSize: [FIXME] name size fail %d\n", szFileName );   
        szFileName = 11;
    }

// Compare:
// Copy name and add string terminator char.
// Compare 11 chars of the desired name with the name 
// found into the current entry.

    i=0; 
    while (i < MaxNumberOfEntries)
    {
        // If the entry is NOT empty
        if ( Dir[z] != 0 )
        {
            memcpy ( NameX, &Dir[z], szFileName );
            NameX[szFileName] = 0;
            CmpStatus = gramado_strncmp( file_name, NameX, szFileName );
            if (CmpStatus == 0){
                goto found;
            }
            // Nothing
        }; 

        // Next entry
        // (32/2)  (16 words) 512 times!

        z += 16;   // (short)
        i++;        
    }; 

// Not found
fail:

    if ((void*) file_name != NULL){
        printk ("fsGetFileSize: [FAIL] %s not found\n", file_name );
     }

    //if ( (void*) NameX != NULL )
        // printk ("fsRootDirGetFileSize: %s not found\n", NameX );

    return (unsigned long) 0;

// Found
found:

    // #debug
    // printk("arquivo encontrado\n");
    // while(1){}

// #debug
// Pegando o tamanho do arquivo.
// Offsets: 28 29 30 31

    //FileSize = *(unsigned long*) (VOLUME1_ROOTDIR_ADDRESS + (z*2) + 28 );
    intFileSize = *(unsigned int *) (dir_address + (z*2) + 28 );

    FileSize = (unsigned long) intFileSize;

	// #debug
	// printk ("%d \n" , root[ z+14 ]);
	// printk ("%d \n" , root[ z+15 ]);
	// printk ("done: FileSize=%d \n" , FileSize);
    // printk ("fsGetFileSize: FileSize=%d \n" , FileSize );
	// while(1){ asm("hlt"); }

    return (unsigned long) (FileSize & 0x00000000FFFFFFFF);
}

/*
 * fsFAT16ListFiles:
 * Display the filename in a given fat16 directory.
 * IN:
 * dir_address = va for where the directory was loaded.
 */
// #todo
// is dir_address virtual or physical?
// Change this name to dir_pa or dir_va.

void 
fsFAT16ListFiles ( 
    const char     *dir_name, 
    unsigned short *dir_va, 
    int            number_of_entries )
{
    char dirname[64];
    register int i=0;
    int j=0;  // Offset
    int MaxNumberOfEntries;
    char NameString[12];  // 8.3 name
    // Buffer
    unsigned char *buf = (unsigned char *) dir_va;
    //unsigned short *shortBuffer = (unsigned short *) dir_va;

// Parameter:
    if ((void *) dir_name == NULL){
        printk ("fsFAT16ListFiles: dir_name\n");
        goto fail;
    }
    if (*dir_name == 0){
        printk ("fsFAT16ListFiles: *dir_name\n");
        goto fail;
    }
    if ((void *) buf == NULL){
        printk ("fsFAT16ListFiles: buf\n");
        goto fail;
    }

    memset(dirname, 0, sizeof(dirname));  // Clear local buffer
    memcpy(dirname, dir_name, 64-1);
    dirname[63] = 0;  // Safety measure

// banner message
// #bugbug: Missing string finalization.
 
    printk ("fsFAT16ListFiles: Listing names in %s:\n", dir_name );
            
// Number of entries:
    if (number_of_entries <= 0){
        debug_print ("fsFAT16ListFiles: number_of_entries\n");
        goto fail;
    }
    if (number_of_entries > 512){
        debug_print ("fsFAT16ListFiles: number_of_entries is too big\n");
        goto fail;
    }
    MaxNumberOfEntries = number_of_entries;

// Show 'Max' entries in the directory.

    i=0; 
    j=0;
    while (i < MaxNumberOfEntries){

        // Not invalid and not free.
        if ( buf[j] != FAT_DIRECTORY_ENTRY_LAST &&
             buf[j] != FAT_DIRECTORY_ENTRY_FREE )
        {
            // #bugbug
            memcpy( (char*) NameString, (const char *) &buf[j], 11 );
            NameString[11] = 0;  // finalize string

            // Display it
            printk ("%s\n", NameString );
        }

        j += 32;  // Next entry
        i++;  
    }; 

    // ...
 
done:
    return;
fail:
    printk ("fsFAT16ListFiles: Fail\n");
}

// #todo
// Change the return type to 'int' and
// remove all the messages. Maybe.
// + Initializes the bootvolume_fat structure.
void fat16_init_fat_structure(void)
{
    debug_print ("fat16_init_fat_structure: [TODO]\n");

// root
// The root file system structure.
// "/"

    if ((void *) root == NULL){
        panic ("fat16_init_fat_structure: root\n");
    }

// ==================================

// fat
// Let's create the 'fat' structure.
// See:

    bootvolume_fat = (void *) kmalloc( sizeof(struct fat_d) );
    if ((void *) bootvolume_fat == NULL){
        panic ("fat16_init_fat_structure: bootvolume_fat\n");
    }
    memset(bootvolume_fat, 0, sizeof(struct fat_d));

// Populate it with some values found in the root structure.
    bootvolume_fat->initialized = FALSE;
// ??
// The same type of the root filesystem?
    bootvolume_fat->type = (int) root->type;

    bootvolume_fat->fat_address = 
        (unsigned long) root->fat_address; 

    bootvolume_fat->fat_first_lba = (unsigned long) root->fat_first_lba;
    bootvolume_fat->fat_last_lba  = (unsigned long) root->fat_last_lba;

    //bootvolume_fat->fat_size_in_sectors = (unsigned long) root->fat_size_in_sectors;
    //bootvolume_fat->size_in_bytes = 0;  // (bootvolume_fat->fat_size_in_sectors / 2), se sector=512.
    //bootvolume_fat->size_in_kb = 0;     // bootvolume_fat->size_in_bytes/1024

// #todo
// Check this values.

// #todo
// What is this address?
// Is this the virtual address of the
// start of the fat table?

    if (bootvolume_fat->fat_address == 0){
        panic ("fat16_init_fat_structure: bootvolume_fat->fat_address\n");
    }

// #bugbug: 
// Is it int ?
    if (bootvolume_fat->type <= 0){
        panic ("fat16_init_fat_structure: fat->type\n");
    }

    bootvolume_fat->volume = NULL;

    // ...

    bootvolume_fat->used = TRUE;
    bootvolume_fat->magic = 1234;
    bootvolume_fat->initialized = TRUE;
}

// Helper function to handle fat cache status
void fs_fat16_cache_not_saved(void){
    g_fat_cache_saved = FAT_CACHE_NOT_SAVED;
}

int fs_save_fat16_cache(void)
{
    debug_print("fs_save_fat16_cache: Saving FAT cache\n");

    if (g_fat_cache_saved != FAT_CACHE_NOT_SAVED){
        return (int) -1;
    }

// Save FAT table into the disk.
// ATA disk only.
// see: atadsk_w.c

    atadsk_save_fat (
        VOLUME1_FAT_ADDRESS,
        VOLUME1_FAT_LBA,
        VOLUME1_FAT_SIZE 
    );

    g_fat_cache_saved = FAT_CACHE_SAVED;
    return 0;
}

//
// $
// INITIALIZATION
//

// fat16Init:
// Called by fsInit in this document
int fat16Init(void)
{
    debug_print ("fat16Init:\n");

    // Initializing the cache state
    g_fat_cache_loaded = FAT_CACHE_NOT_LOADED;
    g_fat_cache_saved  = FAT_CACHE_NOT_SAVED;

    set_filesystem_type(FS_TYPE_FAT16);  // Set type
    fs_init_structures();                // fs structures
    fat16_init_fat_structure();          // fat structure

    // debug_print ("fat16Init: Done\n");
    return 0;
}

