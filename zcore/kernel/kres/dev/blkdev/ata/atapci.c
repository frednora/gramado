// atapci.c
// History:
// + Original version Created by Nelson Cole, Sirius OS, BSD license.
// + Document created by Fred Nora.
// + Changed many times by Fred Nora.

#include <kernel.h>

//#define PCI_CLASS_MASS  1

/*
 * PCIDeviceATA:
 * Estrutura de dispositivos pci para um disco ata.
 * #bugbug: E se tivermos mais que um instalado ???
 * #importante
 * Essa é uma estrutura de dispositivos pci 
 * criada para o gramado, 
 * definida em pci.h
 */
struct pci_device_d *PCIDeviceATA;
// struct pci_device_d *PCIDeviceATA2;
// ...

// ---------------------------------------------

/* 
 * diskReadPCIConfigAddr:
 *     READ 
 */
uint32_t 
diskReadPCIConfigAddr ( 
    int bus, 
    int dev,
    int fun, 
    int offset )
{
    out32( PCI_PORT_ADDR, __PCI_CONFIG_ADDR( bus, dev, fun, offset ) );
    return (uint32_t) in32(PCI_PORT_DATA);
}

/* 
 * diskWritePCIConfigAddr:
 *     WRITE 
 */
void 
diskWritePCIConfigAddr ( 
    int bus, 
    int dev,
    int fun, 
    int offset, 
    int data )
{
    out32( PCI_PORT_ADDR, __PCI_CONFIG_ADDR( bus, dev, fun, offset ) );
    out32( PCI_PORT_DATA, data );
}

/*
 * diskPCIScanDevice:
 *     Busca um dispositivo de acordo com a classe.  
 *     Esta função deve retornar uma variável contendo: 
 *     + O número de barramento, 
 *     + o dispositivo e  
 *     + a função.
 * IN: Class.
 * OUT: data.
 *      -1 = error (#bugbug, pois o tipo de retorno eh unsigned int)
 */
uint32_t diskPCIScanDevice(int class)
{
    int bus=0;
    int dev=0;
    int fun=0;
// #bugbug 
// Usando -1 para unsigned int. 
    uint32_t data = -1;

    // #debug
    //printk ("diskPCIScanDevice:\n");
    //refresh_screen ();

// Probe

    for ( bus=0; bus < 256; bus++ )
    {
        for ( dev=0; dev < 32; dev++ )
        {
            for ( fun=0; fun < 8; fun++ )
            {
                out32( 
                    PCI_PORT_ADDR, 
                    __PCI_CONFIG_ADDR( bus, dev, fun, 0x8) );

                data = in32(PCI_PORT_DATA);

                if ( ( data >> 24 & 0xff ) == class )
                {
                    // #debug
                    // printk ("Detected PCI device: %s\n", 
                    //     pci_classes[class] );
                    // Done
                    return (uint32_t) ( fun + (dev*8) + (bus*32) );
                }
            };
        };
    };

// Fail
    printk("diskPCIScanDevice: PCI device NOT detected\n");

// #bugbug 
// Usando -1 para unsigned int. 
    return (uint32_t) (-1);
}

// atapciSetupMassStorageController:
// Espaço de configuraçao PCI Mass Storage.
// Nessa rotina:
// + Encontra o tipo de driver, ser é IDE, RAID, AHCI ou Desconhecido.

int atapciSetupMassStorageController(struct pci_device_d *D)
{
// Called by __ata_initialize().

    uint32_t data=0;

// A estrutura ainda nao foi configurada.
    //ata_port.used = FALSE;
    //ata_port.magic = 0;

// We still don't know the type of this controller.
// But the caller already knows that 
// it's a mass storage device, and ide.
    AtaController.controller_type = (uint8_t) ATA_UNKNOWN_CONTROLLER;

// Check parameters.
    if ((void *) D == NULL){
        printk("atapciConfigurationSpace: D\n");
        goto fail;
    }
    if ( D->used != TRUE || D->magic != 1234 ){
        printk ("atapciSetupMassStorageController: D validation\n");
        goto fail;
    }

// Indentification Device
    //data = (uint32_t) diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0 );

// Salvando configurações.
    //D->Vendor = data &0xffff;
    //D->Device = data >> 16 &0xffff;

    // #debug
    // printk ("Disk info: [ Vendor ID: %x,Device ID: %x ]\n", 
    //     D->Vendor, D->Device );

/*
    if ( D->Vendor == 0x1106 && D->Device == 0x0591 ){
        printk ("VIA disk found\n");
    } else if (D->Vendor == 0x1106 && D->Device == 0x0591){
        // ...
    };
 */

// Obtendo informações.
// Classe code, programming interface, revision id.

    //data  = (uint32_t) diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );

// Saving info.
// Classe e sub-classe.
// prog if.
// Revision.

    //ata_pci->classCode  = data >> 24 & 0xff;
    //ata_pci->subclass   = data >> 16 & 0xff;
    //ata_pci->progif     = data >> 8  & 0xff;
    //ata_pci->revisionId = data       & 0xff;

// #importante:
// Aqui detectamos o tipo de dispositivo com base 
// nas informações de classe e subclasse.


//
// == #SCSI ========
//

// ====
    // 1:0 = SCSI controller.
    if ( D->classCode == PCI_CLASSCODE_MASS && 
         D->subclass == PCI_SUBCLASS_SCSI ){

        AtaController.controller_type = (uint8_t) PCI_SUBCLASS_SCSI;
        printk ("atapciSetupMassStorageController: SCSI not supported\n");
        goto fail;

//
// == #IDE ========
//

// ====
    // 1:1 = IDE controller.
    } else if ( D->classCode == PCI_CLASSCODE_MASS && 
                D->subclass == PCI_SUBCLASS_IDE ){

        // #type: (IDE).
        AtaController.controller_type = (uint8_t) ATA_IDE_CONTROLLER; 

        // Compatibilidade e nativo, primary.
        data  = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x200 ){ 
            diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 8, data | 0x100 ); 
        }

        // Compatibilidade e nativo, secundary.
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x800 ){ 
            diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 8, data | 0x400 ); 
        }

        //#todo: Comment.
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x8000 ){
            // Bus Master Enable
            data = diskReadPCIConfigAddr (D->bus, D->dev, D->func, 4);
            diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 4, data | 0x4);
        }

        // Habilitar interrupcao (INTx#)
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 4 );
        diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 4, data & ~0x400);

        // IDE Decode Enable
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x40 );
        diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 0x40, data | 0x80008000 );

        // Synchronous DMA Control Register
        // Enable UDMA
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x48 );
        diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 0x48, data | 0xf);

        // #debug
        // printk ("[ Sub Class Code %s Programming Interface %d Revision ID %d ]\n",\
        //    ata_sub_class_code_register_strings[AtaController.controller_type],
        //    ata_pci.progif,
        //    ata_pci.revisionId );

//
// == #RAID ========
//

// #todo
// Devemos falhar, 
// pois não daremos suporte à IDE RAID.

// ====
    // 1:4 = RAID controller
    }else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_RAID ){

        // #type: (ATA RAID).
        AtaController.controller_type = (uint8_t) ATA_RAID_CONTROLLER;
        printk ("atapciSetupMassStorageController: ATA RAID not supported\n");
        goto fail;

//
//  # ACHI
//

// #todo
// Devemos falhar, pois temos outro driver 
// para esse tipo de controlador.

// ====
    // 1:6 = SATA controller.
    }else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_SATA ){

        // #type (ACHI)
        AtaController.controller_type = (uint8_t) ATA_AHCI_CONTROLLER;

        // Compatibilidade e nativo, primary.
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x200 ){ 
            diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 8, data | 0x100 ); 
        }

        // Compatibilidade e nativo, secundary.
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x800 ){ 
            diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 8, data | 0x400 ); 
        }

        // ??#todo: Comment
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 8 );
        if ( data & 0x8000 ) 
        {    
            // Bus Master Enable.
            data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 4 );
            diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 4, data | 0x4 );
        } 

        // IDE Decode Enable.
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x40 );
        diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 0x40, data | 0x80008000 );

        // Habilitar interrupcao (INTx#)
        data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 4 );
        diskWritePCIConfigAddr ( D->bus, D->dev, D->func, 4, data & ~0x400);

        // #debug
        // printk ("[ Sub Class Code %s Programming Interface %d Revision ID %d ]\n",\
        //     ata_sub_class_code_register_strings[ata_port.controller_type], 
        //     ata_pci.progif,
        //     ata_pci.revisionId );


    // 1:7
    }else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_SERIALSCSI ){

        AtaController.controller_type = (uint8_t) PCI_SUBCLASS_SCSI;
        printk ("atapciSetupMassStorageController: Serial SCSI not supported\n");
        goto fail;

    // 1:8
    }else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_NVMEMORY ){

        AtaController.controller_type = (uint8_t) PCI_SUBCLASS_SCSI;
        printk ("atapciSetupMassStorageController: NVMe not supported\n");
        goto fail;

    // 1:9
    }else if ( D->classCode == PCI_CLASSCODE_MASS && 
               D->subclass == PCI_SUBCLASS_SAS ){

        AtaController.controller_type = (uint8_t) PCI_SUBCLASS_SCSI;
        printk ("atapciSetupMassStorageController: SAS not supported\n");
        goto fail;

// ====
// No type
// Fail!
// O tipo de dispositivo de armazenaento de massa 
// não é suportado.

    // Fail
    // ?:? = Class/subclass not supported.
    // #type: Unknown controller.
    }else{
        AtaController.controller_type = (uint8_t) ATA_UNKNOWN_CONTROLLER;
        printk("atapciSetupMassStorageController: Mass Storage Device NOT supported\n");
        goto fail;
    };

// #obs:
// Nesse momento já sabemos se é IDE, RAID, AHCI.
// Vamos pegar mais informações,
// Salvaremos as informações na estrutura.

// PCI cacheline, Latancy, Headr type, end BIST
    data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0xC );
    D->latency_timer = (data >> 8)  & 0xff;
    D->header_type   = (data >> 16) & 0xff;
    D->bist          = (data >> 24) & 0xff;

// BARs
    D->BAR0 = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x10 );
    D->BAR1 = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x14 );
    D->BAR2 = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x18 );
    D->BAR3 = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x1C );
    D->BAR4 = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x20 );
    D->BAR5 = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x24 );

// irqline and irq pin.
    data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 0x3C );
    D->irq_line = (data & 0xff);
    D->irq_pin  = (data >> 8) & 0xff;

// PCI command and status.
    data = diskReadPCIConfigAddr ( D->bus, D->dev, D->func, 4 );
    D->Command = (data & 0xffff); 
    D->Status  = (data >> 16) & 0xffff;

    // #debug
    // printk ("[ Command %x Status %x ]\n", D->Command, D->Status );
    // printk ("[ Interrupt Line %x Interrupt Pin %x ]\n", 
    //     D->irq_line, D->irq_pin );

//
// == DMA ====================
//
    data = diskReadPCIConfigAddr( D->bus, D->dev, D->func, 0x48 );
//#debug
    //printk("[ Synchronous DMA Control Register %x ]\n", data );

//done:
    //ata_port.used = TRUE;
    //ata_port.magic = 1234;
    return (int) PCI_MSG_SUCCESSFUL;

fail:
    //ata_port.used = FALSE;
    //ata_port.magic = 0;
    refresh_screen();
    return (int) PCI_MSG_ERROR;
}

