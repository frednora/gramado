// fs/search.c
// Created by Fred Nora.

// #todo
// This routines belongs to fat16
// we need to move it to fat/

#include <kernel.h>

// List of clusters. 
// Usado na rotina de carregamento de arquivo.
// #bugbug
// O limite é um arquivo de 512KB.
// see: fs.h
unsigned short file_cluster_list[MAX_CLUSTERS]; 

/*
 * search_in_dir:
 *    Procura por um arquivo, dado o endereço do diretório, 
 * com o diretório já carregado na memória.
 * #todo: 
 *    Atender à especificações do sistemas de arquivos, como:
 *    +Tamanho do cluster
 *    +Tamanho do disco
 *    +Tipo de sistema de arquivos. 
 */
// #todo
// We need to search in the file table first of all.
// The structure found there will give us the inode structure pointer.
// #todo
// Is it a virtual address? change to dir_va in this case.
// ---------------
// IN:
// + File name. (Upper case, no point) "12345678XYZ"
// + Address of the directory.
// ---------------
// OUT:
// 1 = Found.

int 
search_in_dir ( 
    const char *file_name, 
    unsigned long dir_address )
{
    int Status = -1;
// Deslocamento do dir.
    register int i=0;
// Deslocamento no nome.
    int j=0;
// Número máximo de entradas no diretório.
// #todo: O número de entradas poderia ser passado via argumento.
    unsigned long NumberOfEntries = 512;

// Local copy: The desired filename.
    char Name_Desired[13];
// Local copy: The filename found in the current directory entry.
    char Name_InEntry[13];

// Buffer.
// #importante: 
// O endereço do diretório foi passado via argumento.
// #todo:
// Podemos checar a validade desse ponteiro.
    char *dir = (char *) dir_address;
// #bugbug
// Vamos checar o tamanho da string
// Me parece que quando um nome tem extensão
// com menos de três letras, então as últimas 
// letras etão com '0' e não espaços.
    size_t stringSize=0;

    debug_print ("search_in_dir: $\n");

// File name
    if ((void*) file_name == NULL){
        printk ("search_in_dir: file_name\n");
        goto fail;
    }
    if (*file_name == 0){
        printk ("search_in_dir: *file_name\n");
        goto fail;
    }

// The address where the directory is.
// #todo: Explain it better.
    if (dir_address == 0){
        printk ("search_in_dir: dir_address\n");
        goto fail;
    }

//
// Copy file name
//

// Get the size
// #bugbug: Is it a valid pointer?
    stringSize = (size_t) strlen(file_name);

    //#debug
    //printk ("Name size = {%d}\n",stringSize);

// check
// #bugbug
// The teminal.bin application is sending 
// the whole commant line to this routine.
// We need only the filename.
    if (stringSize > 11){
        printk ("search_in_dir: [ERROR] Wrong name size. {%d} \n", 
            stringSize);
        printk ("filename: %s\n",file_name);
        goto fail;
    }

    if (stringSize < 11)
    {
        //#todo: maybe!
        //while(stringSize<11)
        //{
        //    strcat(Name_Desired," ");
        //    stringSize++;
        //}
        //printk ("Name_Desired={%s}\n",Name_Desired);
            
            //#debug
        //refresh_screen();
            //while(1){}
    
        if (stringSize == 10){
            Name_Desired[10] = ' '; 
        }
        
        if (stringSize == 9)
        {
            Name_Desired[10] = ' '; 
            Name_Desired[9]  = ' '; 
        }
        
        if (stringSize == 8)
        { 
            Name_Desired[10] = ' '; 
            Name_Desired[9]  = ' '; 
            Name_Desired[8]  = ' '; 
        }
    }

// Copy
// Copy all the 11 chars.
    strncpy(Name_Desired, file_name, 11);
// Finalize
    Name_Desired[11] = 0;
//---------------------------

/*
// hack hack
    if (stringSize != 11){
        printk ("search_in_dir: [ERROR] Wrong name size. {%d} \n", 
        stringSize);
        goto fail;
    }
*/

// File name

// #bugbug
// #todo
// Can't search for a filename that starts with a '/'.
    if (*file_name == '/'){
        printk ("search_in_dir: Invalid char in file name\n");
        goto fail;
    }

// Search
// Probe for a given filename.

    for (i=0; i < NumberOfEntries; i++)
    {
        // FAT_DIRECTORY_ENTRY_FREE
        if (dir[j] == (char) 0xE5){
            j += 0x20; 
            continue; 
        }

        // Diretório atual ou diretório pai.
        // '.' ou '..'
        if (dir[j] == '.'){
            j += 0x20;
            continue;
        }

        //#TODO
        //pegar o tamanho da string para determinar o quanto comparar.

        // Entrada normal. Diferente de zero.
        // Copia o nome e termina incluindo o char 0.
        if (dir[j] != 0)
        {
            memcpy( Name_InEntry, &dir[j], 11 );
            Name_InEntry[11] = 0;

            //Status = (int) kstrncmp ( file_name, Name_InEntry, 11 );
            Status = (int) kstrncmp( Name_Desired, Name_InEntry, 11 );
            
            // Found!
            // #todo:
            // Maybe we can return the 
            // index in '*index_return' parameter.
            if (Status == 0)
            {
                // #debug
                debug_print("search_in_dir: Found\n");
                // printk ("search_in_dir: Found\n");
                // *index_return = j;
                return (int) TRUE; 
            }
            // Nothing
        }

        // Próxima entrada. Repete 512 vezes.
        j += 0x20;
    };

fail:
    //debug_print("search_in_dir: Not found\n");
    printk     ("search_in_dir: Not found %s\n",Name_Desired);
    // return FALSE;
    return (int) -1;
}

int search_in_root(const char *file_name)
{
    unsigned long dir_va = VOLUME1_ROOTDIR_ADDRESS;
    // #test #todo
    // unsigned long dir_va = (unsigned long) get_rootdir_va();

// Pointer validation.
    if ((void *) file_name == NULL){
        debug_print("search_in_root: file_name\n");
        goto fail;
    }
    if (*file_name == 0){
        debug_print("search_in_root: *file_name\n");
        goto fail;
    }
// IN: filename, dir address
    return (int) search_in_dir(file_name,dir_va);
fail:
    return (int) -1;
}

/*
 * fs_find_n_empty_entries:
 *     Encontrar uma quantidade específica de entradas na fat.
 *     Pois um arquivo demanda uma ou várias entradas, 
 *  dependendo do tamanho.
 * @todo: Corrigir essa função, ela é importante.
 *  Obs: tem algo parecido na função que salva um arquivo.
 *  O retorno deve ser short, mesmo tipo do cluster.
 */
// #bugbug
// empty uninitialized.
// Search in file_cluster_list[]
// IN: 
// Desired number of empty entries.
// OUT:
// + The number of the first cluster in the list.
// + 0 if failure.
unsigned short fs_find_n_empty_entries(int n)
{

//
// #fixme
//

    panic("fs_find_n_empty_entries: #fixme\n");

    int i=0;
    int l=0;
    unsigned short empty=0;

// Limits
// #todo:
// Maybe we need a bigger limit, for bigger files.
    if ( n < 0 || n > 1024 ){
        goto fail;
    }

// Loop
// Procurar uma quantidade de entradas vazias.

    for ( i=0; i < n; i++ )
    {
        //empty = (unsigned short) fs_find_empty_entry(?);

        // Preenche a lista de entradas vazias.
        if ( empty != 0 && empty < MAX_CLUSTERS ){

            file_cluster_list[l] = (unsigned short) empty;
            l++;
        
        }else{ 
            goto fail;
        };

    };

    // Finaliza a lista com uma assinatura.

    file_cluster_list[l] = 0xFFF8; 

// Retorna o primeiro da lista.
done:
    return (unsigned short) file_cluster_list[0];
fail:
    return (unsigned short) 0;
}

/*
 * findEmptyDirectoryEntry:
 * Procura uma entrada vazia no diretório 
 * IN:
 *     Endereço do diretório e número máximo de entradas.
 */
int 
findEmptyDirectoryEntry ( 
    unsigned long dir_address, 
    int number_of_entries )
{
    register int i=0;
    int j=0;
    int EntrySizeInBytes = 32;
    unsigned char *dir = (unsigned char *) dir_address;

// Parameters
    if (dir_address == 0){
        goto fail;
    }
    if (number_of_entries < 0){
        goto fail;
    }

// Probe
    for (i=0; i<number_of_entries; i++)
    {
        if (dir[j] == 0){
            return (int) i; 
        }
        // Next entry
        j = (int) (j + EntrySizeInBytes);
    };

fail:
    return (int) (-1);
}

// Wrapper
// Only in root dir.
int fsSearchFileInRoot(const char *file_name)
{
    //debug_print ("fsSearchFileInRoot:\n");
    if ((void *) file_name == NULL){
        goto fail;
    }
    //if (*file_name == 0)
        //goto fail;
    return (int) search_in_root(file_name);
fail:
    return (int) -1;
}

/*
 * fs_find_empty_entry_in_fat:
 *     Encontrar uma entrada vazia na fat.
 *     @todo: Isso pe importante:
 */
//@todo:
//encontrar uma entrada vazia na fat.
//fornecer o endereço da fat na memória.

unsigned short fs_find_empty_entry_in_fat(char *fat_address)
{
    debug_print("fs_find_empty_entry_in_fat: [TODO]\n");
    panic("fs_find_empty_entry_in_fat:");
    return (unsigned short) 0;
}

// #bugbug
// Not tested yet.
// OUT: index.
int search_path_in_the_inode_table(const char *path)
{
    struct inode_d *tmp_inode;
    register int i=0;
    size_t PathSize = 0;
    int Status = -1;

    debug_print("search_path_in_the_inode_table: [FIXME] Not tested yet\n");

// Parameters
    if ((void*) path == NULL){
        debug_print("search_path_in_the_inode_table: [ERROR] path\n");
        goto fail;
    }
    if (*path == 0){
        debug_print("search_path_in_the_inode_table: [ERROR] *path\n");
        goto fail;
    }
 
    PathSize = (size_t) strlen(path);
    
    for (i=0; i<32; i++)
    {
        if ( inode_table[i] != 0 )
        {
            tmp_inode = (struct inode_d *) inode_table[i];
            if ((void*) tmp_inode != NULL)
            {
                //#todo validation
                
                //#bugbug: types = (const char *)
                Status = kstrncmp( path, tmp_inode->path, PathSize );
                // OK
                if (Status == 0){
                    return (int) i;
                } 
            }
        } 
    };

fail:
    return (int) -1;
}

//
// End
//

