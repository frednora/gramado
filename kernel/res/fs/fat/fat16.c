// fat/fat16.c
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
unsigned short fat16ClustersToSave[CLUSTERS_TO_SAVE_MAX];


struct fat16_directory_entry_d *vol_label_directory_entry;

// -------------------------------

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
 
// Put the name into a local buffer.
    char name_buffer[16];
    memset(name_buffer, 0, 16);

    register int i=0;
    for (i=0; i<(8+3); i++){
        name_buffer[i] = vol_label_directory_entry->FileName[i];
    };

// Show info:    
    printk("Disk name: {%s}\n",name_buffer);
    refresh_screen();

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
// Get the entry info.

    register int i=0;
// Local pointer.
    struct fat16_directory_entry_d *p;
    p = entry;

    if ((void*) p == NULL)
        panic("fat16_get_entry_info: p\n");
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

// #todo: Describe this routine.
// credits: hoppy os.
// from 8.3
// not tested yet.
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
    for (j=10; j>=8 && src[j]==0x20; j--)
    {
        // Nothing.
    };
    
    //
    if (j==7) {
    
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

// to_FAT_name:
// #todo: Describe this routine.
// credits: hoppy os.
// to 8.3
// not tested yet.
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
        i=2;
    
    // Current directory
    } else if (!strcmp(src,".")) {
        strcpy(dst,src);
        i=1;
 
    // Regular file.
    } else {

        ptr = src;
        i=0;
        
        // Começamos com o nome.
        while (i<8 && *ptr && *ptr != '.') 
        {
            dst[i++] = *ptr++;
        };

        // Completa com ' ' ate 8.
        while (i<8){ dst[i++] = 0x20; };
        
        if (*ptr == '.') { ptr++; }
        
        // Agora a extensao
        while (i<11 && *ptr){
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

unsigned long 
fsGetFileSize ( 
    unsigned char *file_name, 
    unsigned long dir_address )
{
    unsigned long FileSize=0;    // 64bit
    unsigned int intFileSize=0;  // 32bit

    register int i=0;
    unsigned long max = 64;    //?? @todo: rever. Número máximo de entradas.
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

    debug_print ("fsGetFileSize: $\n");

    if ((void*) file_name == NULL){
        printk("fsGetFileSize: [ERROR] file_name\n");
        goto fail;
    }
    if (*file_name == 0){
        printk("fsGetFileSize: [ERROR] *file_name\n");
        goto fail;
    }
    if (dir_address == 0){
        printk("fsGetFileSize: [ERROR] dir_address\n");
        goto fail;
    }

// Lock ??.
    //taskswitch_lock();
    //scheduler_lock();

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
        // #bugbug
        // We can not do this everytime this function
        // is called.

        fs_load_rootdir ( 
            VOLUME1_ROOTDIR_ADDRESS, 
            VOLUME1_ROOTDIR_LBA, 
            32 );
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

        // Setores por cluster.
        Spc = root->spc;
        if (Spc <= 0){ panic ("fsGetFileSize: [FAIL] spc\n"); }

        // Max entries ~ Número de entradas no rootdir.
        // #bugbug: 
        // Devemos ver o número de entradas no diretório raiz
        // #bugbug: Esse valor não é válido para todos os diretórios
        // por isso temos que usar estruturas para gerenciar
        // diretórios e entradas.

        max = root->dir_entries;
        if (max <= 0){ panic ("fsGetFileSize: [FAIL] max root entries\n"); }

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

// Compare.
// Copia o nome e termina incluindo o char 0.
// Compara 11 caracteres do nome desejado, 
// com o nome encontrado na entrada atual.
    i=0; 
    while (i < max)
    {
        // Se a entrada não for vazia.
        if ( Dir[z] != 0 )
        {
            memcpy ( NameX, &Dir[z], szFileName );
            NameX[szFileName] = 0;
            CmpStatus = kstrncmp( file_name, NameX, szFileName );
            if (CmpStatus == 0){
                goto found;
            }
            // Nothing
        }; 

        // Next entry.
        // (32/2)  (16 words) 512 times!

        z += 16;    
        i++;        
    }; 

// Not found!

fail:

    if ((void*) file_name != NULL){
        printk ("fsGetFileSize: [FAIL] %s not found\n", file_name );
     }

    //if ( (void*) NameX != NULL )
        // printk ("fsRootDirGetFileSize: %s not found\n", NameX );

    //refresh_screen();
    return (unsigned long) 0;

// Found!

found:

    // #debug
    // printk("arquivo encontrado\n");
    // refresh_screen();
    // while(1){}

// #debug
// Pegando o tamanho do arquivo.
// Offsets: 28 29 30 31

    //FileSize = *(unsigned long*) (VOLUME1_ROOTDIR_ADDRESS + (z*2) + 28 );
    intFileSize = *(unsigned int *) (dir_address + (z*2) + 28 );

    FileSize = (unsigned long) intFileSize;

	//printk ("%d \n" , root[ z+14 ]);
	//printk ("%d \n" , root[ z+15 ]);
	//printk ("done: FileSize=%d \n" , FileSize);

	//#debug
	//refresh_screen();
	//while(1){ asm("hlt"); }

    // #debug
    // printk ("fsGetFileSize: FileSize=%d \n" , FileSize );
    // refresh_screen ();

    return (unsigned long) ( FileSize & 0x00000000FFFFFFFF );
}

/*
 * fsFAT16ListFiles:
 *     Mostra os nomes dos arquivos de um diretório.
 *     Sistema de arquivos fat16.
 * IN:
 * dir_address = Ponteiro para 
 * um endereço de memória onde foi carregado o diretório. 
 */
// #todo
// is dir_address virtual or physical?
// Change this name to dir_pa or dir_va.

void 
fsFAT16ListFiles ( 
    const char     *dir_name, 
    unsigned short *dir_address, 
    int            number_of_entries )
{
// Iterator
    register int i=0;
// Offset
    int j=0;  
// Max number of entries.
    int Max = number_of_entries;
// 8.3
    char NameString[12];
// Buffer
    unsigned short *shortBuffer = (unsigned short *) dir_address;
    unsigned char  *charBuffer  = (unsigned char *)  dir_address;

    if ( (void *) dir_name == NULL ){
        printk ("fsFAT16ListFiles: [FAIL] dir_name\n");
        goto fail;
    }

    if ( *dir_name == 0 ){
        printk ("fsFAT16ListFiles: [FAIL] *dir_name\n");
        goto fail;
    }

// banner message.
// #bugbug
// Missing string finalization.
        
    // printk ("fsFAT16ListFiles: Listing names in [%s]\n\n", 
    //        dir_name );
            
// Number of entries.
    if (number_of_entries <= 0){
        debug_print ("fsFAT16ListFiles: number_of_entries\n");
        goto fail;
    }

// #bugbug
// Number of entries.
    if (number_of_entries > 512){
        debug_print ("fsFAT16ListFiles: number_of_entries is too big\n");
        goto fail;
    }

// Show 'Max' entries in the directory.

    i=0; 
    j=0;
    while (i < Max)
    {
        // Not invalid and not free.
        if ( charBuffer[j] != FAT_DIRECTORY_ENTRY_LAST &&
             charBuffer[j] != FAT_DIRECTORY_ENTRY_FREE )
        {
            // #bugbug
            memcpy( 
                (char*) NameString, 
                (const char *) &charBuffer[j],
                11 );
            NameString[11] = 0;  //finalize string
            printk ("%s\n", NameString );
        }

        // (32/2) proxima entrada! 
        // (16 words) 512 vezes!
 
        //j += 16;  //short buffer  
        j += 32;  //char buffer

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
void fat16_init_fat_structure(void)
{
    debug_print ("fs_init_fat: [TODO]\n");

// root
// The root file system structure.
// "/"

    if ((void *) root == NULL){
        panic ("fs_init_fat: root\n");
    }

// ==================================

// fat
// Let's create the 'fat' structure.
// See:

    bootvolume_fat = (void *) kmalloc( sizeof(struct fat_d) );
    if ((void *) bootvolume_fat == NULL){
        panic ("fs_init_fat: bootvolume_fat\n");
    }
    memset(bootvolume_fat, 0, sizeof(struct fat_d));

// Populate it with some values found in the root structure.
    bootvolume_fat->initialized = FALSE;
// ??
// The same type of the root filesystem?
    bootvolume_fat->type = (int) root->type;

    bootvolume_fat->fat_address = 
        (unsigned long) root->fat_address; 

    bootvolume_fat->fat_first_lba = 
        (unsigned long) root->fat_first_lba;
    bootvolume_fat->fat_last_lba = 
        (unsigned long) root->fat_last_lba;

    //bootvolume_fat->fat_size_in_sectors = (unsigned long) root->fat_size_in_sectors;
    //bootvolume_fat->size_in_bytes = 0;  // (bootvolume_fat->fat_size_in_sectors / 2), se sector=512.
    //bootvolume_fat->size_in_kb = 0;     // bootvolume_fat->size_in_bytes/1024

// #todo
// Check this values.

// #todo
// What is this address?
// Is this the virtual address of the
// start of the fat table?

    if ( bootvolume_fat->fat_address == 0 ){
        panic ("fs_init_fat: bootvolume_fat->fat_address\n");
    }

// #bugbug: 
// Is it int ?
    if ( bootvolume_fat->type <= 0 ){
        panic ("fs_init_fat: fat->type\n");
    }

    bootvolume_fat->volume = NULL;

    // ...

    bootvolume_fat->initialized = TRUE;
    bootvolume_fat->used = TRUE;
    bootvolume_fat->magic = 1234;

// #bugbug
// Tem que passar esse ponteiro para algum lugar.
    //return;
}

// helper function to handle fat cache status.
void fs_fat16_cache_not_saved(void)
{
    g_fat_cache_saved = FAT_CACHE_NOT_SAVED;
}

int fs_save_fat16_cache(void)
{
    debug_print("fs_save_fat16_cache: Saving FAT cache\n");

    if (g_fat_cache_saved != FAT_CACHE_NOT_SAVED){
        return -1;
    }

// #todo: Change this name.
// see: dev/disk_w.c
    fs_save_fat(
        VOLUME1_FAT_ADDRESS,
        VOLUME1_FAT_LBA,
        VOLUME1_FAT_SIZE );
    g_fat_cache_saved = FAT_CACHE_SAVED;

    return 0;
}

//
// $
// INITIALIZATION
//

// fat16Init:
// Called by fsInit inthis document.
int fat16Init(void)
{
    debug_print ("fat16Init: [TODO]\n");

// Initializing the cache state.
    g_fat_cache_loaded = FAT_CACHE_NOT_LOADED;
    g_fat_cache_saved  = FAT_CACHE_NOT_SAVED;
// Set type
    set_filesystem_type(FS_TYPE_FAT16);
// fs structures
    fs_init_structures();
// fat structure
    fat16_init_fat_structure();

    debug_print ("fat16Init: Done\n");
    return 0;
}

