// pci.h
// PCI interface support.
// Created by Fred Nora.

#ifndef __DD_PCI_H
#define __DD_PCI_H    1

#define PCI_ADDRESS_PORT      0x0CF8
#define PCI_DATA_PORT         0x0CFC
 
#define PCI_MAX_BUSES         0xFF  //256 ??
#define PCI_MAX_DEVICES       32    //Ok
#define PCI_MAX_FUNCTIONS     8     //Ok.

#define PCI_INVALID_VENDORID  0xFFFF

//
// PCI Offset.
//

// Deslocamento dentro da tabela de configuração, dado em hexa.
#define PCI_OFFSET_VENDORID       0       //short
#define PCI_OFFSET_DEVICEID       0x02    //short
#define PCI_OFFSET_COMMAND        0x04    //short CMD
#define PCI_OFFSET_STATUS         0x06    //short
#define PCI_OFFSET_REVISIONID     0x08    //char RID
#define PCI_OFFSET_PROGIF         0x09    //char  PI Programming Inteface
#define PCI_OFFSET_SUBCLAASS      0x0A    //char Class Code CC
#define PCI_OFFSET_CLASSCODE      0x0B    //char.
#define PCI_OFFSET_CACHELINESIZE  0x0C    //char CLS
#define PCI_OFFSET_LATENCYTIMER   0x0D    //char Master Latency Timer - MLT
#define PCI_OFFSET_HEADERTYPE     0x0E    //char HTYPE
#define PCI_OFFSET_BIST           0x0F    //char
#define PCI_OFFSET_BASEADDRESS0   0x10  //unsigned long
#define PCI_OFFSET_BASEADDRESS1   0x14  //unsigned long
#define PCI_OFFSET_BASEADDRESS2   0x18  //unsigned long
#define PCI_OFFSET_BASEADDRESS3   0x1C  //unsigned long
#define PCI_OFFSET_BASEADDRESS4   0x20  //unsigned long
#define PCI_OFFSET_BASEADDRESS5   0x24  //unsigned long
#define PCI_OFFSET_CISP           0x28  //unsigned long
#define PCI_OFFSET_SUBSYSTEMVENDORID        0x2C //short sub system identifiers SS
#define PCI_OFFSET_SUBSYSTEMID              0x2E //short
#define PCI_OFFSET_EXPANSIONROMBASEADDRESS  0x30  //unsigned long
#define PCI_OFFSET_CAPABILITIES_POINTER 0x34    //char Capabilities Pointer. CAP
//#define PCI_OFFSET_RESERVEDZZ         0x35    //3 bytes.
//#define PCI_OFFSET_RESERVEDZZZ 0x38  //unsigned long  
#define PCI_OFFSET_INTERRUPTLINE            0x3C //char (Read and wright). Interrupt Information INTR
#define PCI_OFFSET_INTERRUPTPIN             0x3D //char (Read only).
#define PCI_OFFSET_MIN_GRANT                0x3E //char  Minimum Grant MGNT
#define PCI_OFFSET_MAX_LATENCY              0x3F //char  Maximum Latency MLAT

// 
// Class Codes. 
//

#define PCI_CLASSCODE_LEGACY                    0
#define PCI_CLASSCODE_MASS                      0x01    //Mass storage controller.
#define PCI_CLASSCODE_NETWORK                   0x02
#define PCI_CLASSCODE_DISPLAY                   0x03
#define PCI_CLASSCODE_MULTIMEDIA                0x04
#define PCI_CLASSCODE_MEMORY                    0x05
#define PCI_CLASSCODE_BRIDGE                    0x06
#define PCI_CLASSCODE_COMMUNICATION             0x07
#define PCI_CLASSCODE_GENERICSYS                0x08    //Generic system peripheral
#define PCI_CLASSCODE_INPUT                     0x09
#define PCI_CLASSCODE_GENERICDOCKINGSTATION     0x0A
#define PCI_CLASSCODE_PROCESSOR                 0x0B
#define PCI_CLASSCODE_SERIALBUS                 0x0C
#define PCI_CLASSCODE_WIRELESS                  0x0D
#define PCI_CLASSCODE_INTELLIGENTIO             0x0E
#define PCI_CLASSCODE_ENCRYPTION_DECRYPTION     0x0F //Satellite communications controllers.
#define PCI_CLASSCODE_SATELLITECOMMUNICATION    0x10 //Encryption/Decryption controllers.
//11h - Data acquisition and signal processing controllers.
//12h~feh - Reserved.
//#define PCI_CLASSCODE_MISC   0xFF //Device does not fit any of the defined class codes.

//Class Code Register:
//Class Code	            Sub-Class Code	        Prog. I/F
//23..................16	15..................8	7..................0


//Device subclasses.

//0
#define PCI_SUBCLASS_NONVGA 0    //Non-VGA unclassified device.
#define PCI_SUBCLASS_VGA    1    //VGA compatible unclassified device.

//1
#define PCI_SUBCLASS_SCSI        0     //SCSI storage controller.
#define PCI_SUBCLASS_IDE         0x01  //IDE interface.
//#define PCI_SUBCLASS_FLOPPY      0x02  //Floppy disk controller.
#define PCI_SUBCLASS_IPI         0x03  //IPI bus controller.
#define PCI_SUBCLASS_RAID        0x04  //RAID bus controller.
#define PCI_SUBCLASS_ATA         0x05  //ATA controller.
#define PCI_SUBCLASS_SATA        0x06  //SATA controller.
#define PCI_SUBCLASS_SERIALSCSI  0x07  //Serial Attached SCSI controller.
//#define PCI_SUBCLASS_NVMEMORY    0x08  //Non-Volatile memory controller.
#define PCI_SUBCLASS_MASS        0x80  //Mass storage controller.


//2
#define PCI_SUBCLASS_ETHERNET   0     //Ethernet controller.
#define PCI_SUBCLASS_TRN        0x01  //Token ring network controller.
#define PCI_SUBCLASS_FDDI       0x02  //FDDI network controller.
#define PCI_SUBCLASS_ATM        0x03  //ATM network controller.
#define PCI_SUBCLASS_ISDN       0x04  //ISDN controller.
#define PCI_SUBCLASS_WF         0x05  //WorldFip controller.
//#define PCI_SUBCLASS_PICMG      0x06  //PICMG controller.
//#define PCI_SUBCLASS_INFINIBAND 0x07  //Infiniband controller.
//#define PCI_SUBCLASS_FABRIC     0x08  //Fabric controller.
#define PCI_SUBCLASS_NETWORK    0x80  //Network controller.


//3
#define PCI_SUBCLASS_VGACOMPATIBLE 0     //VGA compatible controller.
#define PCI_SUBCLASS_XGACOMPATIBLE 0x01  //XGA compatible controller.
//#define PCI_SUBCLASS_3D            0x02  //3D controller.
#define PCI_SUBCLASS_DISPLAY       0x80  //Display controller.


//4
#define PCI_SUBCLASS_MVIDEO      0     //Multimedia video controller.
#define PCI_SUBCLASS_MAUDIO      0x01   //Multimedia audio controller.
#define PCI_SUBCLASS_TELEPHONY   0x02   //Computer telephony device.
#define PCI_SUBCLASS_AUDIO       0x03   //Audio device. 
#define PCI_SUBCLASS_MULTIMEDIA  0x80  //Multimedia controller.


//5
#define PCI_SUBCLASS_RAM       0     //RAM memory.
#define PCI_SUBCLASS_FLASH     0x01  //FLASH memory.
#define PCI_SUBCLASS_MEMORY    0x80  //Memory controller.


//6
#define PCI_SUBCLASS_HOST    0	    //Host bridge.	
#define PCI_SUBCLASS_ISA     0x01	//ISA bridge.	
#define PCI_SUBCLASS_EISA    0x02	//EISA bridge.	
#define PCI_SUBCLASS_MICROCHANNEL    0x03	//MicroChannel bridge.	
#define PCI_SUBCLASS_PCI        0x04	//PCI bridge.	
#define PCI_SUBCLASS_PCMCIA     0x05	//PCMCIA bridge.	
#define PCI_SUBCLASS_NUBUS      0x06	//NuBus bridge.	
#define PCI_SUBCLASS_CARDBUS    0x07	//CardBus bridge.	
#define PCI_SUBCLASS_RACEWAY    0x08	//RACEway bridge.	
#define PCI_SUBCLASS_PCITOPCI   0x09	//Semi-transparent PCI-to-PCI bridge.	
//#define PCI_SUBCLASS_INFINIBANDTOPCI    0x0A	//InfiniBand to PCI host bridge.	
#define PCI_SUBCLASS_BRIDGE     0x80	//Bridge.

//7
#define PCI_SUBCLASS_SERIAL       0       //Serial controller.	
#define PCI_SUBCLASS_PARALLEL     0x01    //Parallel controller.	
#define PCI_SUBCLASS_MULTIPORT    0x02	  //Multiport serial controller.	
#define PCI_SUBCLASS_MODEM        0x03	  //Modem.	
#define PCI_SUBCLASS_GPIB         0x04	  //GPIB controller.	
#define PCI_SUBCLASS_SMARDCARD        0x05    //Smard Card controller.	
#define PCI_SUBCLASS_COMMUNICATION    0x80	  //Communication controller.

//8
//#define PCI_SUBCLASS_PIC          0x00
//#define PCI_SUBCLASS_DMA          0x01
//#define PCI_SUBCLASS_TIMER        0x02
//#define PCI_SUBCLASS_RTC          0x03
#define PCI_SUBCLASS_PCIHOTPLUG   0x04
#define PCI_SUBCLASS_SDHOST       0x05
#define PCI_SUBCLASS_IOMMU        0x06

//9
#define PCI_SUBCLASS_KEYBOARD     0	      //Keyboard controller.	
//#define PCI_SUBCLASS_DIGPEN       0x01	  //Digitizer Pen.	
//#define PCI_SUBCLASS_MOUSE        0x02	  //Mouse controller.	
//#define PCI_SUBCLASS_SCANNER      0x03	  //Scanner controller.	
#define PCI_SUBCLASS_GAMEPORT     0x04	  //Gameport controller.	
#define PCI_SUBCLASS_INPUT        0x80    //Input device controller.

//A
//#define PCI_SUBCLASS_GDS  0	      //Generic Docking Station.	
//#define PCI_SUBCLASS_DS   0x80    //Docking Station.

//b
#define PCI_SUBCLASS_386        0       //386.	
#define PCI_SUBCLASS_486        0x01    //486.	
#define PCI_SUBCLASS_PENTIUM    0x02	//Pentium.	
#define PCI_SUBCLASS_ALPHA      0x10	//Alpha.	
#define PCI_SUBCLASS_POWERPC    0x20	//Power PC.	
#define PCI_SUBCLASS_MIPS       0x30    //MIPS.	
//#define PCI_SUBCLASS_COPROCESSOR    0x40	//Co-processor.


//c
//#define PCI_SUBCLASS_FW        0	   //FireWire (IEEE 1394).	
#define PCI_SUBCLASS_ACCESS    0x01    //ACCESS Bus.
#define PCI_SUBCLASS_SSA       0x02	   //SSA.	
#define PCI_SUBCLASS_USB       0x03	   //USB controller.	
//#define PCI_SUBCLASS_FIBRE     0x04	   //Fibre Channel.	
#define PCI_SUBCLASS_SMBUS     0x05	   //SMBus.	
#define PCI_SUBCLASS_INFINIBAND2   0x06    //InfiniBand.	
#define PCI_SUBCLASS_IPMI     0x07	  //IPMI SMIC interface.	
#define PCI_SUBCLASS_SERCOS   0x08	  //SERCOS interface.	
#define PCI_SUBCLASS_CANBUS   0x09    //CANBUS.


//d
#define PCI_SUBCLASS_IRDA    0	     //IRDA controller.	
#define PCI_SUBCLASS_CIR     0x01	 //Consumer IR controller.	
#define PCI_SUBCLASS_RF      0x10    //RF controller.	
#define PCI_SUBCLASS_BLUETOOTH    0x11    //Bluetooth.	
#define PCI_SUBCLASS_BROADBAND    0x12	  //Broadband.	
#define PCI_SUBCLASS_8021A        0x20	  //802.1a controller.	
#define PCI_SUBCLASS_8021B        0x21	  //802.1b controller.	
#define PCI_SUBCLASS_WIRELESS     0x80	  //Wireless controller.


//e
//#define PCI_SUBCLASS_I2O    0    //I2O.


//f
//#define PCI_SUBCLASS_STV       0x01    //Satellite TV controller.	
//#define PCI_SUBCLASS_SAUDIO    0x02	   //Satellite audio communication controller.	
//#define PCI_SUBCLASS_SVOICE    0x03	   //Satellite voice communication controller.	
#define PCI_SUBCLASS_SDATACOMMUNICATION    0x04  //Satellite data communication controller.


//0x10
//#define PCI_SUBCLASS_NCENCRYPTION    0	     //Network and computing encryption device.	
//#define PCI_SUBCLASS_EENCRYPTION     0x10	 //Entertainment encryption device.	
//#define PCI_SUBCLASS_ENCRYPTION      0x80    //Encryption controller.


//0x11
#define PCI_SUBCLASS_DPIO         0	      //DPIO module.	
#define PCI_SUBCLASS_PCOUNTERS    0x01    //Performance counters.	
#define PCI_SUBCLASS_CSYNC  0x10	//Communication synchronizer.	
#define PCI_SUBCLASS_SPM    0x20	//Signal processing management.	
#define PCI_SUBCLASS_SPC    0x80	//Signal processing controller.


//0x12
//#define PCI_SUBCLASS_PACCEL    0    //Processing accelerators.

//...

#endif   

