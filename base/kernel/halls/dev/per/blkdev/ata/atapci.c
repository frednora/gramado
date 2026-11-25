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
 * criada para o gramado, definida em pci.h
 */
struct pci_device_d *PCIDeviceATA;
// ...

// ---------------------------------------------


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

                if (( data >> 24 & 0xff ) == class)
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

