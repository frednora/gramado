// fs.c
// Filesystem support for the bootloader.
// 2015 - Created by Fred Nora.

/*
 * Arquivo principal do sistema de arquivos usado no Boot Loader.
 * Sobre o sistema de arquivos:
 *    + HD IDE.
 *    + Parti��o 0. 
 *      Meus Discos:
 *    + FAT16, 8MB, cluster size = 512 bytes. (VHD)
 *    + FAT16, 255 heads, 63 spt, cylinder 243. (Pen Drive, Kingstone DT 2GB).
 *    + FAT16, 255 heads, 63 spt, cylinder 1946. (Pen Drive, SanDisk 16GB).
 *    + ...
 *  Sobre o Boot Loader:   
 *      O prop�sito do Boot Loader � fazer inicializa��es, carregar o Kernel, 
 * alguns m�dulos, aplicativos a GUI. Depois passa o comando para o Kernel.
 * Observa��o: 
 *     Os valores das estruturas podem ser salvos em um registro. No registro 
 * pode ter um registro das opera��es efetuadas no sistema de arquivo.
 */


#include "../bl.h"


// Globals
int g_fat16_root_status=0;
int g_fat16_fat_status=0;
int g_file_system_type=0;

// Lista de clusters em um arquivo.
// #todo: 
// Tamanho de arquivos?
unsigned short file_cluster_list[MAX_CLUSTERS];

// see: fs.h
struct partition_table_d  partition;


/*
 * fs_load_file:
 *     Carrega um arquivo na mem�ria.
 */
 
/* 
int fs_load_file ( unsigned char *file_name, unsigned long file_address )
{
    return (int) 0;    //Ainda n�o implementada.
}
*/

/*
 * fs_search_file:
 *    Procura por um nome de arquivo no diret�rio raiz.
 */ 
/*
int fs_search_file(unsigned char *file_name)
{
    return (int) 0;
}
*/

/*
/// Checks if the sector loaded in user_data is a Fat16 boot sector
unsigned short is_FAT16()
{
    char* label="FAT16";

    unsigned short res;
    res = memcmp( label, user_data+0x36, 5);  // Compares with label in FAT16 Boot
    if(res)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
*/

/*
 * fatClustToSect:
 *     Converte. 
 *     Suporte a clusters. 
 */
 
unsigned long 
fatClustToSect ( 
    unsigned short cluster, 
    unsigned long spc, 
    unsigned long first_data_sector )
{
    unsigned long C = (unsigned long) cluster;

    return (unsigned long) ( ( C - 2 ) * spc) + first_data_sector;
}


/*
 * fatLoadCluster:
 *     Load a cluster full of data 
 */
// Come�a do primeiro setor do cluster.
// SECTOR_SIZE = 512;

void 
fatLoadCluster ( 
    unsigned long sector, 
    unsigned long address, 
    unsigned long spc )
{
    register unsigned long i=0;
    for (i=0; i<spc; i++)
    {
        read_lba( address, sector + i );
        address = (unsigned long) (address + SECTOR_SIZE);
    };
}

/*
 * fs_search_empty_entry: 
 *     Encontrar uma entrada vazia em um dado diret�rio. 
 */

unsigned long fs_search_empty_entry (unsigned long id)
{
    return 0; 
}


/*
 * fs_find_not_empty_entry:
 *     Procura entrada N�O vazia na FAT.
 *     A entrada 0, sempre esta ocupada.
 */

unsigned long fs_find_not_empty_entry ()
{
    return (unsigned long) 0;
}

void fs_test_fat_vector ()
{
}

unsigned long fs_get_fat_entry (unsigned long n)
{
    return (unsigned long) 0;
}


/*
 * fs_create_entry:
 *     Cria uma entrada de diret�rio.
 * Arguments:
 *     +Nome do arquivo.  
 *     +Id do diret�rio.
 *     +Id da entrada de diret�rio.
 *     +N�mero do cluster inicial na FAT.
 *     +Tamanho do arquivo.
 */
 
void 
fs_create_entry ( 
    char *name,  
    unsigned long id,
    unsigned long eid,
    unsigned long cluster,
    unsigned long size )
{
    // Nothing for now
}

/*
 * fsSaveFileName: 
 *     Salva um nome no diret�rio dado o cluster inicial.
 */
 
void 
fsSaveFileName ( 
    unsigned char *name, 
    unsigned long id, 
    unsigned long eid, 
    unsigned long initial_cluster )
{        
    // Nothing for now
}

/*
 * fs_save_entry_on_disc:
 *     @todo: O buffer deve ser salvo no disco.
 */
void fs_save_entry_on_disc ( unsigned long id, unsigned long eid )
{
}

void fs_set_structures ()
{
}

void fs_set_entry ( unsigned long id, unsigned long eid )
{
}

void fs_get_entry ( unsigned long id, unsigned long eid )
{
}

void fs_show_dir_entry ( unsigned long id, unsigned long eid )
{
}

void fs_show_dir (unsigned long id)
{
}

/*
 * fs_check_cluster:
 *     Confere os clusters de um diret�rio, procurando por falhas.
 *     Objetivo: 
 *          Encontrar falha de diret�rio na hora 
 *          do carregamento dos arquivos do sistema.
 *     Obs: Pertence ao Boot Loader.
 */
 
unsigned long fs_check_cluster (unsigned long id)
{
	return 0;
}

/*
 * fsSaveRootDir:
 *     Salva o diret�rio raiz no disco.
 *     @todo: Desenvolver essa fun��o.
 *     Observa��es: 
 *          O Boot Loader pode querer salvar o
 *          arquivo de log de inicializa��o ou
 *          modificar alguma configura��o de inicializa��o
 *          no arquivo de configura��o de inicializa��o,
 *          ent�o salvar� os arquivos no diret�rio e
 *          gravar� o diret�rio no disco. 
 *
 *          Fun��o importante para o processo
 *          de restaura��o da inicializa��o, pois
 *          o objetivo do Boot Loader � apenas
 *          carregar arquivos e n�o salvar arquivos.       
 */

void fsSaveRootDir ()
{
}

/*
 * fs_load_dir: 
 *     Carrega um diret�rio na mem�ria.
 */
void fs_load_dir (unsigned long id)
{
}

/*
 * fs_save_dir:
 *     Grava um diret�rio no disco.
 */
void fs_save_dir (unsigned long id)
{
	//return;
}

void fs_save_structure ()
{
}

/*
 *  fs_save_entry_on_root:
 *      Salva uma entrada de diret�rio no diret�rio raiz,
 *      dado o n�mero da entrada.
 *      @todo: bugug ? tem alguma incoer�ncia aqui.
 */
void fs_save_entry_on_root (unsigned long eid)
{
	//return;
}

/*
 * fs_show_entry:
 *     Mostra os dados da entrada de diret�rio.
 */
void fs_show_entry (unsigned long id, unsigned long eid)
{
	//return;
}

void fs_delete_entry (unsigned long id, unsigned long eid)
{
	//return;
}

void fs_init_bootfile_struct ()
{
}

unsigned long fs_get_entry_status (unsigned long id, unsigned long eid)
{
    return 0;
}

void 
fs_set_entry_status ( 
    unsigned long id, 
    unsigned long eid, 
    unsigned long status )
{
    //return;
}

void fs_set_fat_entry (unsigned long n, unsigned long value)
{
// Nothing
}

unsigned short fs_find_empty_entry ()
{
    return 0;
}

/*
 * fs_relood_dir: 
 *     Recarrega na mem�ria um diret�rio dado seu Id.
 */
void fs_relood_dir (unsigned long id)
{
    //return;  
}

/*
 * fsLoadFile:
 * Carrega um arquivo na mem�ria. (#fat16)
 * O Boot Loader utiliza essa rotina para carregar os arquivos do sistema.
 * #todo:
 * +A libC chama essa fun��o atravez de open(...).
 * +Mudar o tipo de retorno para 'int'.
 */

int 
fsLoadFile ( 
    unsigned char *name, 
    unsigned long file_address, 
    unsigned long dir_address )
{
    int Status = 0;
    //unsigned long i=0;
    //unsigned long j=0;
    unsigned short next=0;
// Directory
    unsigned short *dir = (unsigned short *) dir_address;
// Iterator
// N�mero m�ximo de entradas no root dir.
    register unsigned long max = 512;
    unsigned long z = 0;        //Deslocamento no diret�rio raiz.
    //unsigned long n = 0;        //Deslocamento no nome.
    char name_x[13];

// FAT support.

// OK. Fat is constant.
// We need a structure of 'device context' passed via argument.
// 'dc->fat_address'
    unsigned short *fat = (unsigned short *) FAT16_FAT_ADDRESS;
// Cluster inicial.
    unsigned short cluster=0;


// Parameters:
    if ((void*) name == NULL){
        printf("fsLoadFile: name\n");
        goto fail;
    }
    if (*name == 0){
        printf("fsLoadFile: *name\n");
        goto fail;
    }
    if (file_address == 0){
        printf("fsLoadFile: file_address\n");
        goto fail;
    }
    if (dir_address == 0){
        printf("fsLoadFile: dir_address\n");
        goto fail;
    }

//loadRoot:

//#test: 
// Suspendendo o carregamento do root.
// Ele ser� previamente carregado em main().
// Carrega o diret�rio raiz na mem�ria.
    //fs_load_rootdirEx();

    if (g_fat16_root_status != 1){
        printf ("fsLoadFile: Root Status\n");
        goto fail;
    }

    // #debug 
    //printf("carregar_arquivo: Searching File=[%s]\n",file_name);

// Search
// Procura o arquivo no diret�rio raiz.
//search_file:

    while (max > 0)
    {
        // Se a entrada nao eh vazia, entao:
        // Copia o nome e termina com '0'.
        // Compara 11 caracteres.

        if (dir[z] != 0)
        {
            memcpy ( name_x, &dir[z], 11);
            name_x[11] = 0;

            Status = (int) strncmp( name, name_x, 11 );
            if (Status == 0){
                cluster = (unsigned short) dir[z +13];
                goto found;
            }
        }

        // Se a entrada eh vazia, vamos para a proxima.
        // (32 bytes / 2) = pr�xima entrada!
        // 512 entradas.
        
        z += 16;
        max--;
    }; 

// =======================
// not found
// Se o arquivo n�o for encontrado.
// file_not_found:
    printf ("fsLoadFile: [FAIL] File not found\n"); 
    goto fail;

// ===========================
// found
// Se o arquivo for encontrado.    
found: 

// Pega cluster inicial (unsigned short).
// Limites: 
// 16 entradas por setor, 32 setores de tamanho.
// (0x1A/2) = 13.
// Check limits.

    cluster = (unsigned short) dir[z +13];

// #bugbug
// It is unsigned short.

    // if ( cluster == 0 || cluster > 0xFFF0 )
    if ( cluster <= 0 || cluster > 0xFFF0 )
    {
        printf ("fsLoadFile: [FAIL] Cluster limits File=[%s] Cluster=[%d]\n", 
            &dir[z], cluster );
        goto fail;
    }

// Obs: 
// root[z] � o in�cio da entrada. 
// Podemos pegar v�rias informa��es sobre o arquivo. 
// #todo: 
// Precisamos alocar uma estrutura para salvarmos as informa��es
// sobre o arquivo.

//loadFAT:

    // #debug 
    //printf("carregar_arquivo: Loading FAT ...\n");

//#test: 
// Suspendendo o carregamento do root.
// Ele ser� previamente carregado em main().
// Carrega a FAT na mem�ria. 
    //fs_load_fatEx();

    if (g_fat16_fat_status != 1){
        printf("fsLoadFile: [FAIL] FAT Status\n");
        goto fail;
    }

    // #debug 
    //printf("carregar_arquivo: Loading file ...\n"); 

//
// Carregar o arquivo.
//

// Loop
LOOP_next_entry:

/*
 * @todo: 
 * Carrega clusters com mais de um setor por cluster. As rotinas para isso 
 * est�o no in�cio desse arquivo.
 *
 
    while(1)
    {
	    //Calcula.
		//Primeiro setor do cluster.
	    unsigned long S;
		S = fatClustToSect(cluster, Spc, FAT16_DATAAREA_LBA); 
		
		//Carrega 'x' setores come�ando de S.
		fatLoadCluster( S, file_address, Spc);
	
	     // Pegar o pr�ximo cluster na FAT.
	     //O cluster atual cont�m o n�mero do pr�ximo.
	     
	    cluster = fat[cluster];	

	    //ver se o cluster carregado era o �ltimo.
	    if(cluster == 0xffff || cluster == 0xfff8){
            goto done;
	    };
    };
 *
 */

// Read one sector.
// Ler um setor. 
// (data_area_base + next_cluster - 2)
// 512 bytes por cluster.
// See: hdd.c

    read_lba ( 
        file_address, 
        (FAT16_DATAAREA_LBA + cluster -2) ); 

// Incrementa o buffer. +512;
// #todo: 
// SECTOR_SIZE deveria ser variavel e nao constante.

    file_address = (unsigned long) (file_address + SECTOR_SIZE);

// Get the next cluster into the FAT.
    next = (unsigned short) fat[cluster];
// Setup current cluter.
    cluster = (unsigned short) next;
//  Is it the last one in this file?
    if ( cluster == 0xFFFF || cluster == 0xFFF8 )
    {
        goto done; 
    }

    // #debug
    // printf("%d ", cluster);

// Check next entry in FAT.
    goto LOOP_next_entry;

// ===================================
// O arquivo n�o pode ser carregado.
fail:
    printf ("fsLoadFile: Fail\n");
    refresh_screen ();
    return 1;

// ==============================
// Done: 
// Arquivo carregado com sucesso.
done:
// #debug
    //printf ("BL.BIN: File LOADED!\n");
    //refresh_screen (); 
    return 0;
}

// =========================
// path count
// Credits: Sirius OS.
int fs_path_count(const char *path)
{
    int result=0;
    register int i=0;
    int max = (int) (80*25);

// Parameter:
    if ((void*) path == NULL){
        printf ("fs_path_count: path\n");
        goto fail;
    }
    if (*path == 0){
        printf ("fs_path_count: *path\n");
        goto fail;
    }

// Count
    for ( i=0; i<max; i++ )
    {
        if (path[i] == '\0'){ 
            break; 
        }
        if (path[i] == '/') { 
            result++; 
        }
    };

    return (int) result;
fail:
    return 0;
}

// fs_load_path:
// Load in the given physical address, the file in the given pathname.
// IN:
// 2 levels pathname, physical address.
// OUT:
//   0 ---> ok.
// !=0 ---> fail
int fs_load_path(const char *path, unsigned long address)
{

// #test
// Removing all the abort(),
// We are giving a chance to the emergency shell.


    int Ret = -1;    // fail. Usado na fun��o que carrega o arquivo.
    int i=0;         // Deslocamento dentro do buffer.
    char buffer[12];
    // path pointer.
    unsigned char *p;

    //#todo
    //unsigned long ImageBase=0;
    //unsigned long ImageSize=0;

    //onde carregaremos o diret�rio.
    void *__src_buffer;
    void *__dst_buffer;
    void *__file_buffer;

    int level=0;

    int l=0;
    int n_levels=0;

//
// Checks
//
    
// ====================
// path

    if ((void*) path == NULL){
        printf("fs_load_path: path\n");
        goto fail;
    }
    if (*path == 0){
        printf("fs_load_path: *path\n");
        goto fail;
    }
    if (*path != '/'){
        printf("fs_load_path: It's not an absolute path\n");
        goto fail;
    }

    p = path;
    n_levels = (int) fs_path_count(path);
    if (n_levels <= 0){
        printf("fs_load_path: n_levels\n");
        goto fail;
    }
    level = 0;

//====================
// address

    if (address == 0){
        printf("fs_load_path: address\n");
        goto fail;
    }

// =======================
// buffers

    __file_buffer = (void *) address;
    // primeiro src =  root address;
    __src_buffer = (void *) FAT16_ROOTDIR_ADDRESS;

// =======================
// levels

    for (l=0; l<n_levels; l++)
    {
        printf("\n");
        printf("[LEVEL %d]\n",l);

        // Tem que come�ar o level com '/'
        if ( p[0] != '/' ){
            printf("fs_load_path: The level needs to start with '/' \n");
            goto fail;
        }
        p++; //pula o '/' 

        i=0;
        for ( i=0; i<12; i++ )
        {
            // #debug
            // printf ("%c", (char) *p);

            // Copia o char para o buffer at� que o char seja '/'
            // indicando pr�ximo n�vel.
            
            buffer[i] = (char) *p;
            
            // O ponto deve aparecer no �ltimo n�vel.
            // caso contr�rio falhou
            if ( *p == '.' )
            {
                if ( l != (n_levels-1) )
                {
                    printf("fs_load_path: Directory name with '.'\n");
                    goto fail;
                }
                
                // Se o ponto est� al�m do limite permitido.
                if (i>=7)
                {
                    printf("fs_load_path: '.' fail.\n");
                    printf("Name size bigger than 8.\n");
                    goto fail;
                }

                // Se o ponto for antes do nono slot.
                if (i<8)
                {
                    // Nome tem no m�qximo 8 chars.
                    // completamos com espa�o;
                    while (i<=7)
                    {
                        buffer[i] = ' ';
                        i++;
                    };

                    p++;   // pulamos o ponto.

                    // Colocamos a extens�o
                    while (i<=11)
                    {
                        buffer[i] = (char) *p;
                        i++;
                        p++;
                    };
 
                    // Finaliza a string no buffer. 8+3=11
                    buffer[11] = 0;   
                }

                //
                // Load
                //

                // Alocando mem�ria para carregar o diret�rio.
                //__dst_buffer = (void *) malloc (512*32);
    
                // Como esse � o �ltimo, ent�o vamos usar o endere�o desejado pelo usu�rio.
                __dst_buffer = (void *) __file_buffer;
                if ( (void *) __dst_buffer == NULL )
                {
                    printf("fs_load_path: __dir\n");
                    goto fail;
                }

                Ret = 
                    fsLoadFile ( 
                        (unsigned char *) buffer, 
                        (unsigned long) __dst_buffer,    // Onde carregar
                        (unsigned long) __src_buffer );  // Onde procurar.
                // ok
                if (Ret == 0){
                    // printf ("level %d loaded.\n",l);
                    
                    // #importante
                    // Esse n�vel tinha ponto, ent�o deveria ser o �ltimo.
                    if ( l != (n_levels-1) )
                    {
                        printf("fs_load_path: Directory name with '.'\n");
                        printf("It needs to be the last level.\n");
                        goto fail;
                    }

                    // SUCCESS?!
                    return 0;

                    //sai do for?
                    break;

                }else{
                    printf("fs_load_path: Fail loading level 0\n");
                    goto fail;
                };
            }

            // Se encontramos um indicador de pr�ximo n�vel,
            // ent�o esse n�vel n�o ser� considerado bin�rio.
            // obs: Ao iniciar o for ele precisa encontrar esse mesmo char.
            
            if ( *p == '/' )
            {
                // Vamos completar o nome com espa�os e finalizar.
                if (i<11)
                {
                    while (i<=11)
                    {
                        buffer[i] = ' ';   
                        i++;    
                    };
                }
                buffer[11] = 0;

                //
                // Load
                //

                // Alocando mem�ria para carregar o diret�rio.
                // #bugbug: Slab allocation.
                // It is ok here in the boot loader.
 
                __dst_buffer = (void *) malloc (512*32);
                if ( (void *) __dst_buffer == NULL ){
                    printf("fs_load_path: __dst_buffer\n");
                    goto fail;
                }

                Ret = 
                    fsLoadFile ( 
                        (unsigned char *) buffer, 
                        (unsigned long) __dst_buffer,    // Onde carregar
                        (unsigned long) __src_buffer );  // Onde procurar.
                // ok.
                if ( Ret == 0 ){

                    //printf ("level %d carregado com sucesso.\n",l);
                    
                    // O endere�o onde carregamos o arquivo desse n�vel
                    // ser� o endere�o onde vamos procurar o arquivo do pr�ximo n�vel.
                    __src_buffer = __dst_buffer;
                    break;
                }else{
                    printf("fs_load_path: [FAIL] Fail loading level 0\n");
                    goto fail;
                };
            }

            // Go to the next char if it's was not '.' or '/'.
            p++;
        };
    };   

// Abort?
// Returning to call the rescue shell.
fail:
    printf("fs_load_path: Fail\n");
    refresh_screen();
    return (int) (-1);
}

// Search for a file into the root directory.
unsigned long fsSearchFile(unsigned char *name)
{
    int Status=0;
    unsigned long z=0;  //Deslocamento no rootdir. 
    unsigned long n=0;  //Deslocamento no nome.
// Contando entradas.
    register int i=0;
    unsigned long max = 512;  // Entries in rootdir.
    char name_x[13];
    unsigned short *root = (unsigned short *) FAT16_ROOTDIR_ADDRESS;
// Quando encontrarmos a ultima e.
    int EndOfDir = FALSE;
    int FoundInThisEntry = -1;

// Parameter:
    if ((void*) name == NULL){
        printf("fsSearchFile: name\n");
        goto file_not_found;
    }
    if (*name == 0){
        printf("fsSearchFile: *name\n");
        goto file_not_found;
    }

// Load root dir.
    fs_load_rootdirEx();

// Fixa o n�mero m�ximo de entradas de acordo 
// com o tipo de sistema de arquivos.
// Not standard.

    switch (g_file_system_type){
    case 1:  // fat16
    case 2:  // fat32
    case 3:  // ext2
    default:
        max = 512;
        break;
    };

// Procura o arquivo no diret�rio raiz. 
// Dado o nome via argumento.
// Checa a primeira letra do nome.
// Copia o nome e termina incluindo um 0.
// Compara 11 caracteres.

    // Primeira entrada.
    i = 0;

    while (max > 0)
    {
        // Encontramos a primeira entrada vazia.
        if (root[z] == 0){
            EndOfDir = TRUE;
        }
        
        // Not zero.
        if (root[z] != 0)
        {
            memcpy (name_x, &root[z], 11);
            name_x[11] = 0;

            Status = strncmp(name, name_x, 11);
            
            if (Status == 0){  
                FoundInThisEntry = i;
                goto found;  
            }
        }

        z += 16;    // (32/2) pr�xima entrada!
        max--;      // 512 vezes!
        i++;        // Contando entrada.
    }; 

    // Nothing.

// Se o arquivo n�o for encontrado.
file_not_found:
    FoundInThisEntry = -1;
    printf("bl-fsSearchFile: [FAIL] File not found\n"); 
    if (EndOfDir == TRUE)
    {
        printf("We reached the end of dir\n");
        refresh_screen();
        //#??
        //panic??
    }
    return (unsigned long) 1;

// Se o arquivo for encontrado.    
found:
    if ( FoundInThisEntry < 0 || 
         FoundInThisEntry >= 512 )
    {
        panic("bl-fsSearchFile: [ERROR] FoundInThisEntry\n"); 
    }
    return (unsigned long) 0;
}

/*
 * fsSearchFileName:
 *     Procura um nome de arquivo no diret�rio.
 *     @todo: Rever o tipo de retorno.
 */
unsigned long fsSearchFileName(unsigned char *name)
{
    //if ( (void*) name == NULL )
        //return ?;
    return fsSearchFile(name);
}

/*
 * fs_load_rootdirEx:    
 *    Carrega o diret�rio raiz na mem�ria.
 * Obs:
 *    Carregando 32 setores do rootdir.
 *    Size = 32 setores. 
 *    512 entradas de 32 bytes cada.
 */
// size = 32 sectors.
// #todo: Pegar de estrutura.
 
void fs_load_rootdirEx()
{
// Iteration
    register unsigned long i=0;
    unsigned long RootSize = 32;
// Address offset support.
    unsigned long n=0;

    //unsigned long dir_address = FAT16_ROOTDIR_ADDRESS;

    // debug
    // printf("Loading root dir ...\n");

// Carregar 32 setores na mem�ria.
// Carrega um e incrementa o buffer.
    for ( i=0; i < RootSize; i++ )
    {
        read_lba ( 
            ( FAT16_ROOTDIR_ADDRESS + n ), 
            ( FAT16_ROOTDIR_LBA + i ) );

        n = (n +512);
    };
}

/*
 * fs_load_rootdirEx:
 *     Carrega a FAT na mem�ria. (size = 64 setores). 
 */
// #bugbug
// We need to load the whole fat table.
// Or we will have problems to load the kernel.
// 64 setores de tamanho.
// Talvez seja 246.

void fs_load_fatEx()
{
    
    // #bugbug
    // Is this size right?
    
    // Iteration.
    unsigned long i=0;
    unsigned long FatSize = 128; 
    
    // Address offset support.
    unsigned long n=0;

	// debug
	// printf("Loading Cluster Table.\n");//fat

// Carregar FAT na mem�ria.
// Carregar 32 setores na mem�ria.
// Carrega um e incrementa o buffer.

    for ( i=0; i < FatSize; i++ )
    {
        read_lba ( 
            ( FAT16_FAT_ADDRESS + n), 
            ( FAT16_FAT_LBA + i ) );
 
        n = (n +512);
    };
}


/*
 * fs_put_list_on_fat:
 *     Grava na fat uma lista de clusters.
 *     Uma lista de clusters, forma um arquivo.
 */
void fs_put_list_on_fat()
{
    register unsigned short i=0;
    unsigned short ListSize = MAX_CLUSTERS;

    for ( i=0; i < ListSize; i++ )
    {
        // �ltimo elemento da lista.
        if ( file_cluster_list[i] == 0xFFFF || 
             file_cluster_list[i] == 0xFFF8 )
        {
            fs_set_fat_entry ( file_cluster_list[i], 0xFFFF );
            return;
        }

        // Um elemento qualquer da lista.
        if ( file_cluster_list[i] > 0 && 
             file_cluster_list[i] < ListSize )
        {
            fs_set_fat_entry ( 
                file_cluster_list[i], 
                file_cluster_list[i+1] );
        }
    };
}


/*
 * fs_find_n_empty_entries: 
 */
unsigned long fs_find_n_empty_entries (unsigned long n)
{
    register unsigned long i=0;
    unsigned long l=0;
    unsigned short empty=0;
    unsigned short ListSize = MAX_CLUSTERS;

// #todo: 
// Pegar de estrutura.
// N�mero max de entradas na FAT.

// #bugbug
// What is this?
    
    unsigned short fat_max = (64*512/2); 


// Limits.

// #bugbug
// It is 'unsigned long'

    if ( n < 0 || n > ListSize )
    {
        goto fail;
    }

    for (i = 0; i < n; i++)
    {
        empty = fs_find_empty_entry();

        // Cria a lista de entradas vazias.
        if ( empty != 0 && empty < fat_max ){
            file_cluster_list[l] = empty;
            l++;
        }else{
            goto fail;
        };
    };

    file_cluster_list[l] = 0xFFFF; 

// Done
// Retorna o primeiro da lista.
done:
    // #todo: Check the return type.
    return file_cluster_list[0];
fail:
    return 0;
}

// Load the rootdir.
void fs_load_rootdir()
{
    fs_load_rootdirEx();
}


/*
 * fsSaveFile:
 * Salvando um arquivo.
 * @todo: 
 * Manter a estrutura dessa fun��o mas mudar as rotinas chamadas.
 *           A lib C chama essa fun��o.
 * #todo: A rotina do kernel funciona bem.
 */ 
unsigned long 
fsSaveFile ( 
    unsigned char *file_name, 
    unsigned long file_size, 
    unsigned long file_address )  
{

//#suspensa.
    return 0;

/*
    unsigned long i;  //Deslocamento. Procurando espa�o livre.
    unsigned long j;  //Deslocamento. Salvando cluster number.
    unsigned long c;
    //@todo: Pegar esse valor de uma estrutura de FAT.
    unsigned long max = 128;  //N�mero m�ximo de entradas na FAT.
    unsigned long first;
    unsigned long next;
    unsigned long sector;
    unsigned long endereco;  //File address.    	
	unsigned long *fat_base = (unsigned long *) FAT16_FAT_ADDRESS; 
	//Obs: Endere�o imposto.
    
	//
	// #### BUGBUG ####
	// ENDERE�O.
	
	//@todo: Alocar din�micamente.
    unsigned long *list = (unsigned long *) 0x04600000;  //@todo: Rever esse endere�o.    
	
	//@todo: Check file address limits.
    //if(file_address == 0){}
	
    //File address.
    endereco = (unsigned long) file_address;
    
    //Carrega 2 setores da FAT.
	//@todo: Isso precisa ser ampliado.
    read_lba(FAT16_FAT_ADDRESS, FAT16_FAT_LBA);
    read_lba(FAT16_FAT_ADDRESS + 0x200, FAT16_FAT_LBA + 1);
	
    j = 0;     //Deslocamento na lista, tem que ser zero.
    c = 32;    //Deslocamaento na FAT.
    i = 32;    //Contagem de entradas  na FAT, come�a do 32 e vai at� muito.
    
    //Loop. 
    
	while(i != max)
    {
        //procurando espa�o livre   
        if(fat_base[c] == 0)
        {
            //fim do arquivo ?
            if(file_size == 0)
            {
                //marca o fim          
                list[j] = 0x0000FFFF;  //eof           
                //encontrado todos os espa�os livres que o arquivo precisa.
                goto fat_save;
            };    

            //salva um endere�o livre           
            list[j] = c;

            //incrementa a lisa
            j++; 

            //decrementa o tamanho do arquivo!
            file_size--;                  
         };
         
        //incrementa a fat
        c++; 
        
        //incrementa a quantidade de busca.
        i++;
   }; 
   
  // MessageBox("Max entries !");
   fatseg2: goto fatseg2;
    return 1;           

fat_save:
       
    //In�cio da lista.
    j = (512*4);
    i = 0;
    
    first = list[i];
    
    //
    // Salva no diret�rio raiz o nome e o primeiro cluster ?? @todo: Check.
    // Corrige deslocamento ?? @todo: Check.
    
	// ??
	// salva_nome(file_name, first); 
    
    while(j)
    {            
        //Pega um conte�do da lista.
        next = list[i];
        
        //Salva a trilha indicada pela lista.
        // salva_trilha( next, endereco);
        
		//??
		//save_file( 0, endereco, next, 8);
        
		
        //Encontrada a assinatura na lista.
        if(next == 0x0000FFFF)    //@todo: Usar constante. 
        {
            next = list[i-1];
            fat_base[next] = 0x0000FFFF;    //@todo: Usar constante.  
            goto done;         
        }
        else    //N�o � assinatura ainda.
        { 
            //-0x60 = � uma corre��o por causa valor do tipo (unsigned long)!!
            //grava na fat o endere�o do proximo cluster
            fat_base[next] = list[i+1];
            
            //grava - aqui next esta certo!!!
            write_lba( endereco, next);
            
            endereco += 512; 
        }; 
         
        i++;    //Pr�ximo valor da lista.           
        j--;
    };
    
    //MessageBox("nao encontra assinatura na lista!");
    
	//
	// @todo: Precisa de uma mensagem de erro aqui.
	//
	
	fatseg3: goto fatseg3;
done:
    //Salva dois setores da FAT!
    //@todo: Isso precisa ser ampliado.	
    write_lba(FAT16_FAT_ADDRESS        , FAT16_FAT_LBA);
    write_lba(FAT16_FAT_ADDRESS + 0x200, FAT16_FAT_LBA + 1);
    return (unsigned long) 0;
*/
}


/*
 * fsClearFat:
 *     Apaga a fat, colocando zero em tudo.
 */
void fsClearFat()
{
    fsInitFat ();  //Provis�rio.
}

/*
 * fsCheckFat: 
 *     Confere os par�metros da FAT.
 *     @todo: Usar estrutura.
 */
int fsCheckFat()
{
    unsigned long i=0;
    unsigned long fat_entry=0;

    printf("\n");

    // #bugbug:
    // size ?

    for (i = 0; i < 256; i++)
	{
        //Pega a FAT entry.
		fat_entry = fs_get_fat_entry(i);
		
		//Se a entrada da FAT estiver vazia.
		if(fat_entry == 0){
			printf("%c ",'#');
		}else{
			printf("%d ",fat_entry);
		};
		//Nothing.
	};

//Nothing

exit:
#ifdef BL_VERBOSE
    printf("Done!\n");
#endif
    return 0;
fail:
    printf("Fail!\n");
    return 1;
}


/*
 * fsInitFat:
 *    Inicializa as estruturas da FAT.
 */
// Called by fsInit.
void fsInitFat()
{
    //return;  //Ainda n�o implementada.
}

/*
 * fsInitStructures: 
 *     +Limpa as estruturas para primeiro uso.
 *     +@todo: Configura as estruturas com os falores encontrados
 *             no mbr, no rootdir.
 * OBS: Quem vai usar essas estuturas � o bootloader.
 */
// Called by fsInit.
void fsInitStructures()
{
    //return;
}

/*
 * fsInit:
 *     Inicializa o sistema de arquivos da parti��o do sistema.
 *     Obs: Isso deve ficar no fim do arquivo. 
 */
// Called by init() in init.c
int fsInit(void)
{
    register int i=0;

// Clear the list of clusters.
    for (i=0; i<MAX_CLUSTERS; i++){
        file_cluster_list[i] = 0;
    };

    fsInitStructures();
    fsInitFat();
    // ...
    return 0;
}

//
// End
//

