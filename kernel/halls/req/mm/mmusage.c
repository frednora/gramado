
#include <kernel.h>


//========================================================
// Used memory:
// Estamos medindo o uso de memória física.
// Lembrando que a mesma região de memória física
// pode ser mapeada mais de uma vez.
// #todo #bugbug
// Precisamos checar corretamente qual é o endereço físico
// de cada uma dessas regiões e suas sobreposições.
// size=2MB
// see: mminit.c
unsigned long mm_used_ring0_area=0;
unsigned long mm_used_ring3_area=0;
unsigned long mm_used_kernelimage=0;
unsigned long mm_used_backbuffer=0;
unsigned long mm_used_pagedpool=0;
unsigned long mm_used_heappool=0;
unsigned long mm_used_extraheap1=0;
unsigned long mm_used_extraheap2=0;
unsigned long mm_used_extraheap3=0;
unsigned long mm_used_frame_table=0;
// start = ?? size = 2MB
unsigned long mm_used_lfb=0; 


