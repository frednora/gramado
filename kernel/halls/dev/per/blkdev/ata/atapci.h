// atapci.h
// PCI interface for ATA device driver.
// Created by Fred Nora.

#ifndef __ATA_ATAPCI_H
#define __ATA_ATAPCI_H    1

/*
 * PCIDeviceATA:
 * Estrutura de dispositivos pci para um disco ata.
 * #bugbug: E se tivermos mais que um instalado ???
 * #importante
 * Essa é uma estrutura de dispositivos pci 
 * criada para o gramado, 
 * definida em pci.h e criada em atapci.c
 */
// see: atapci.c
extern struct pci_device_d *PCIDeviceATA;
// ...


#endif   


