
// mmft.c
// The frame table FT.
// A pool of frames.

#include <kernel.h>

// Frametable struct.
// see: x64mm.h
struct frame_table_d  FT;



// I_initialize_frame_table:
// Frame table to handle a pool of page frames.
// Vamos configurar a frame table de acordo com o total de memória RAM.
// See: x64mm.h
// #important
// Vamos configurar a frame table de acordo com o total de memória RAM.
// 'memorysizeTotal' is the RAM size in KB.
// Configura apenas o início e o fim.
// >>> Start:
// FRAME_TABLE_START_PA
// This is the start of the table.
// This is the 256MB mark.
// >>> End:
// FRAME_TABLE_END_PA
// This is the end of the table.
// See:
// x64gpa.h
// Setup the start of the table.
// It's always the same.
// We need at least 256 MB.
// The system with 256MB has no FT.
// We need more them this to have a FT.
// It will depend on the size of the RAM.
// This routine will find this value.
// ------
// Initialize frame table.
// It depends on the size of the physical memory.
// It starts at mark of 128MB and goes untill
// the mark of almost 256MB.
// So this routine needs to check if we have memory
// enough for this operation.
int I_initialize_frame_table(void)
{
// Called by mmInitializePaging().

// #warning:
// Essa rotina depende da sondagem do 
// tamanho da memoria fisica.

    //PROGRESS("I_initialize_frame_table:\n");

// ----------------
// #dependencies

    if (MemorySizeInfo.initialized != TRUE){
        debug_print("I_initialize_frame_table: MemorySizeInfo.initialized\n");
        x_panic("I_initialize_frame_table: MemorySizeInfo");
    }

// Clear.
    FT.initialized = FALSE;
    FT.used  = FALSE;
    FT.magic = 0;

// Setup 'Start' and 'End' default physical addresses.
// __128MB_MARK_PA
// Size = 0.
    FT.start_pa = (unsigned long) FRAMETABLE_START_PA;
    FT.end_pa   = (unsigned long) FRAMETABLE_START_PA;

// what is the 'End'?
// Let's setup the size of the frame table.
// Changing the 'End' physical address given
// the size of the physical RAM.

// =================================================
// Size in KB.
// Se a RAM for maior ou igual à 1GB.
// Então temos mais memória do que precisamos
// e a frame table será limitada à marca de 1GB.

    // 1024 KB?
    if ( memorysizeTotal >= (1024*1024) ){
        FT.end_pa = (unsigned long) FRAMETABLE_END_PA;
        debug_print("I_initialize_frame_table: We have 1GB or more\n");
        goto initialize_frame_table;
    }

// =================================================
// Size in KB.
// Se a RAM for maior ou igual à 512MB.

    // 512 KB?
    if ( memorysizeTotal >= (512*1024) ){
        FT.end_pa = (unsigned long) FRAMETABLE_END_PA;
        debug_print ("I_initialize_frame_table: We have 512MB or more\n");
        goto initialize_frame_table;
    } 

// =================================================
// Size in KB.
// Se a RAM for maior ou igual à 256MB.

    // 256 KB?
    if ( memorysizeTotal >= (256*1024) ){
        FT.end_pa = (unsigned long) FRAMETABLE_END_PA;
        debug_print ("I_initialize_frame_table: We have 256MB or more\n");
        goto initialize_frame_table;
    } 

// =================================================
// Error:
// Size in KB.
// Se a RAM for menor que 256MB.
// #bugbug
// Nossa rotina que calcula o tamanho da memória RAM
// nos entrga um valor que é um pouco menos que o
// total disponível.
// Porque ele não testa o último mega.
// The available ram is less than 256.
    if ( memorysizeTotal < (256*1024) )
    {
        debug_print("I_initialize_frame_table: [ALERT] Less than 256MB\n");
    }


// #bugbug
// The x_panic is not working at this time. :)

// ---------------------------
// #fatal
// The available RAM is almost 256MB
// Its because we a 256MB card,
// But the boot loader did not check the last mb.
// #bugbug: x_panic is not available yet.

    if ( memorysizeTotal < (250*1024) )
    {
        debug_print("I_initialize_frame_table: Less than 250MB\n");
        x_panic    ("I_initialize_frame_table: Less than 250MB\n");
    }

// memorysizeTotal is a value in kb, iguess.
// Change this name to memorysizeTotal_in_kb?

    if ( (FRAMETABLE_END_PA/1024) >= memorysizeTotal )
    {
        debug_print("I_initialize_frame_table: Invalid FRAMETABLE_END_PA\n");
        x_panic    ("I_initialize_frame_table:  Invalid FRAMETABLE_END_PA\n");
    }

// Minimum
// Não é menor que 250MB.
// 250*1024*1024 = 268435456 = 0x10000000.
    // Take the difference
    FT.end_pa = (unsigned long) FRAMETABLE_END_PA;

//============================================================
// Initializing all the other elements of the frame table.
initialize_frame_table:

// #bugbug
// Slow. Use a define for this value.
// 250*1024*1024 = 268435456 = 0x10000000.

    //debug_print ("I_initialize_frame_table: Checking range limits\n");

    // Underflow.
    if (FT.end_pa < FT.start_pa){
        x_panic("I_initialize_frame_table: FT.end_pa < FT.start_pa\n");
    }
    // No table
    if (FT.end_pa == FT.start_pa){
        x_panic("I_initialize_frame_table: No table\n");
    }

    // Too small.
    if (FT.end_pa > FRAMETABLE_END_PA){
        x_panic("I_initialize_frame_table: FT.end_pa > FRAMETABLE_END_PA\n");
    }


// Total size in KB.
    mm_used_frame_table = 
        (unsigned long)((FT.end_pa - FT.start_pa)/1024);

//
// ===================================================================
//

// Invalid start address.
    if (FT.start_pa == 0){
         debug_print("I_initialize_frame_table: FT.start_pa\n");
         x_panic    ("I_initialize_frame_table: FT.start_pa\n");
    }
// Invalid end address.
    if (FT.end_pa == 0){
         debug_print("I_initialize_frame_table: FT.end_pa\n");
         x_panic    ("I_initialize_frame_table: FT.end_pa\n");
    }

// Size in Bytes 
// Size in KB 
// Size in MB
    FT.size_in_bytes = 
        (unsigned long) (FT.end_pa - FT.start_pa);
    FT.size_in_kb = 
        (unsigned long) (FT.size_in_bytes/1024);
    FT.size_in_mb = 
        (unsigned long) (FT.size_in_kb/1024);

// Size in frames.
// Each frame has 4096 bytes. 
// This is because each page has 4096 bytes.
    FT.size_in_frames = 
        (unsigned long) (FT.size_in_bytes/4096);
// System frames.
    FT.number_of_system_frames = 
        (unsigned long) FT_NUMBER_OF_SYSTEM_FRAMES;
// User frames.
    FT.number_of_user_frames = 
        (unsigned long) FT_NUMBER_OF_USER_FRAMES;
// Used frames.
// Número de frames gerenciados por essa estrutura.
    FT.number_of_used_frames = 
        (unsigned long) FT_TOTAL_FRAMES;

// O número de frames contidos na área alocável
// não pode ser menor que a quantidade de frames 
// gerenciados por essa estrutura.

    if (FT.size_in_frames <= FT.number_of_used_frames)
    {
        debug_print("I_initialize_frame_table: FT.size_in_frames\n");
        x_panic    ("I_initialize_frame_table: FT.size_in_frames\n");
    }

// Número de frames que sobraram na área alocável
// e que poder ser usados por outro componente do sistema.
    FT.number_of_reserved_frames = 
        (unsigned long) (FT.size_in_frames - FT.number_of_used_frames);

    FT.used = TRUE;
    FT.magic = 1234;
    FT.initialized = TRUE;

    return 0;
}


unsigned long mmGetFTStartPA(void)
{
    return (unsigned long) FT.start_pa;
}

unsigned long mmGetFTEndPA(void)
{
    return (unsigned long) FT.end_pa;
}






