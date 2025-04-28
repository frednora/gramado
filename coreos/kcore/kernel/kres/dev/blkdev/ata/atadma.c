
// atadma.c

#include <kernel.h>  


// ide_dma_start:
//     2018 - Created by Nelson Cole. 

void ide_dma_start (int p)
{
    unsigned char data = 
        in8( ata_port[p].bus_master_base_address + ide_dma_reg_cmd );

    out8 ( ata_port[p].bus_master_base_address + ide_dma_reg_cmd, data | 1 );
}

// ide_dma_stop:
//     2018 - Created by Nelson Cole. 

void ide_dma_stop (int p)
{
    unsigned char data = 
        in8 ( ata_port[p].bus_master_base_address + ide_dma_reg_cmd ); 

    out8 ( ata_port[p].bus_master_base_address + ide_dma_reg_cmd, data & ~1 );

    data = in8 ( ata_port[p].bus_master_base_address + ide_dma_reg_status );

    out8 ( ata_port[p].bus_master_base_address + ide_dma_reg_status, data & ~6 );
}

/*
 * ide_dma_read_status:
 *     DMA read status.
 *     2018 - Created by Nelson Cole.  
 */
int ide_dma_read_status (int p)
{
    return in8 ( ata_port[p].bus_master_base_address + ide_dma_reg_status );
}

/*
 ****************************************
 * ide_dma_data:
 *     O que segue é um suporte ao controlador de DMA para uso nas rotinas de IDE. 
 *     #todo Nelson, Não se esqueça de habiliatar o // Bus Master Enable
 *  no espaço de configuraçao PCI (offset 0x4 Command Register)
 *
 * 2018 - Created by Nelson Cole. 
 */

void 
ide_dma_data ( 
    void *addr, 
    uint16_t byte_count,
    uint8_t flg,
    uint8_t nport )
{

    unsigned char data=0;
    uint32_t phy=0;


	// #todo: 
	// Check limits.

	// #todo: (&~1)sera que e necessario?


    ide_dma_prdt[nport].addr = (unsigned long) addr;  
    ide_dma_prdt[nport].len  = byte_count | 0x80000000;

    phy = (uint32_t) &ide_dma_prdt[nport];

    // prds physical.
    out32 ( ata_port[nport].bus_master_base_address + ide_dma_reg_addr, phy );

	// (bit 3 read/write)
	// 0 = Memory reads.
	// 1 = Memory writes.

    data = in8 ( ata_port[nport].bus_master_base_address + ide_dma_reg_cmd ) &~8;

    // #todo: bit 8, Confilito no Oracle VirtualBox
	// #obs: Isso foi enviado via argumento e agora foi alerado.

    flg = 1;

    out8 ( ata_port[nport].bus_master_base_address + ide_dma_reg_cmd, data | flg << 3 );

	// Limpar o bit de interrupção e 
	// o bit de erro no registo de status.

    data = in8 ( ata_port[nport].bus_master_base_address + ide_dma_reg_status );
    out8 ( ata_port[nport].bus_master_base_address + ide_dma_reg_status, data &~6 );
}



