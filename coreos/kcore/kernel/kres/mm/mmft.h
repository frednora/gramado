
// mmft.h

#ifndef __MM_MMFT_H
#define __MM_MMFT_H    1


// FT:
// Frame table. 
// Uma região grande da memória física que será usada para
// pegar frames novos, jamais alocados.
// Ela deve começar lego em seguida da última região
// mapeada pela rotina mmSetupPaging e 
// terminar no fim da memória física
// indicada pelo bootblock.

#define FT_NUMBER_OF_SYSTEM_FRAMES    512
#define FT_NUMBER_OF_USER_FRAMES      512

// A área total de frames não pode conter menos frames que isso.
#define FT_TOTAL_FRAMES \
    (FT_NUMBER_OF_SYSTEM_FRAMES + FT_NUMBER_OF_USER_FRAMES)

// Gerencia a área alocável total.
struct frame_table_d 
{

// Flags que indica a validade da estrutura
// e o status da inicialização da estrutura.
    int used;
    int magic;
    int initialized;

// no available ram

// This is the address where the table starts.
// It represent the point after the last mapped address.
    unsigned long start_pa;
// This is the address where the table ends.
// It represents the last valid address of the RAM memory.
    unsigned long end_pa;

    unsigned long size_in_bytes;
    unsigned long size_in_kb;
    unsigned long size_in_mb;

// Quantidade total de frames possíveis 
// nessa área alocável.
    unsigned long size_in_frames;

// used frames

    unsigned long number_of_system_frames;
    struct frame_d system_frames[FT_NUMBER_OF_SYSTEM_FRAMES];

    unsigned long number_of_user_frames;
    struct frame_d user_frames[FT_NUMBER_OF_USER_FRAMES];

// Número de frames gerenciados por essa estrutura.
    unsigned long number_of_used_frames;
// Número de frames que sobraram na área alocável
// e que poder ser usados por outro componente do sistema.
    unsigned long number_of_reserved_frames;
};

// Frametable struct.
// see: mm.c
extern struct frame_table_d  FT;


// =======================

int I_initialize_frame_table(void);
unsigned long mmGetFTStartPA(void);
unsigned long mmGetFTEndPA(void);

#endif   

