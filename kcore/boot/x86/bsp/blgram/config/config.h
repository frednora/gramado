/*
 * File: config.h 
 * Configuration file for the 32bit boot loader.
 * Created by Fred Nora.
 */


//
// Debug flag.
//

//#define BL_VERBOSE 1  

//
// ## IMPORNTANTE ##
//

// ------------------------------------------------------
// Disk configuration:
// ## IMPORTANTE ##
// TEM QUE CONFIGURAR O BL TAMBÉM
// Usaremos essa configuraçao provisoriamente
// ate que tenhamos a condiçao de selecionarmos corretamente
// o canal e o dispositivo.
// Inicializaremos essas variaveis ao inicializarmos
// o controlador de ata. ide. em ata.c ata_initialize.
// USE PRIMARY MASTER!!
// Portas bases encontradas nas BARs.
// BAR0 = base port 1F0 
// BAR1 = base port 3F6 
// BAR2 = base port 170 
// BAR3 = base port 376 
// #importante
// master e slave é coisa do PATA
// então 3f6 pode ser canal 2 e 376 pode ser canal 3.

// #bugbug
// Nesse momento estamos determinando que o driver do
// controlador ide deve usar o canal 0 e que ele é master.
// primary/master.
// Então essas definições aqui são encontradas no driver do controlador.
// Mas não deve ser assim. Devemos usar uma variável para isso.
// Talvez algum arquivo de configuração devesse nos dizer 
// qual canal devemos usar.
// Ou ainda o número do driver de boot nos de alguma dica.

// #todo
// Lembre-se que estamos fazendo a mesma coisa 
// no arquivo de configuraçao do kernel.
// See:
// https://wiki.osdev.org/PCI_IDE_Controller

// IDE Interface:
// Primary Master Drive.
// Primary Slave Drive.
// Secondary Master Drive.
// Secondary Slave Drive.

// Serial IDE
// Primary Master,   also called SATA1.
// Primary Slave,    also called SATA2.
// Secondary Master, also called SATA3.
// Secondary Slave,  also called SATA4.

#define __BAR0  0  // 0x1F0
#define __BAR1  1  // 0x3F6
#define __BAR2  2  // 0x170
#define __BAR3  3  // 0x376

// See: diskATAInitialize in ide.c
#define __CONFIG_IDE_PORT    __BAR0    // Primary, master
//#define __CONFIG_IDE_PORT    __BAR1
//#define __CONFIG_IDE_PORT    __BAR2
//#define __CONFIG_IDE_PORT    __BAR3

// #importante
// Esses valores são usados pelo driver.
// #obs
// Talvez podemos tomar essa decisão de acordo com o número 
// do dispositivo que foi passado pelo BIOS.


// Discos:
// /dev/sda - 0x80
// /dev/sdb - 0x81
// /dev/sdc - 0x82
// /dev/sdd - 0x83

// Partições:
// sda1, sda2, sda3, sda4 ...



// Configuramos as estruturas de ide, e 
// de acordo com o número passado pelo bios
// selecionamos qual estrutura usar.
// Temos que passar para o kernel o número do
// dispositivo.

// #tests:

// + Funcionou BAR=0 slave=0.
// + Funcionou BAR=0 slave=1.

// + Funcionou BAR=2 slave=0. secondary master 
// + Funcionou BAR=2 slave=1. secondary slave

// #bugbug
// Ouve uma falha..
// Só temos o registro das portas 0 e 2.
// A porta 0 equivale ao canal primary.
// A porta 2 equivale ao canal secondary.
// Eram para as portas 0 e 1 representarem o canal primary.
// Eram para as portas 2 e 3 represerntarem o canal secondary.

// #importante:
// Na verdade só funcionam as portas 0 e 2 porque são 
// selecionadores das BARs 0 e 2, onde estão as portas de HD.
// #todo: rever o código nessa parte de configuração das BARs.


//a questao 'e que existem canais extras
//vamos presizar ler mais bars.

/*
Current disk controller chips almost always support two ATA buses per chip. 
There is a standardized set of IO ports to control the disks on the buses. 
The first two buses are called the Primary and Secondary ATA bus, and are almost 
always controlled by IO ports 0x1F0 through 0x1F7, and 0x170 through 0x177, 
respectively (unless you change it). The associated 
Device Control Registers/Alternate Status ports are IO ports 0x3F6, and 0x376,
respectively. The standard IRQ for the Primary bus is IRQ14, and IRQ15 for the Secondary bus.

If the next two buses exist, they are normally controlled by IO ports 0x1E8 through 0x1EF, 
and 0x168 through 0x16F, respectively. The associated
Device Control Registers/Alternate Status ports are IO ports 0x3E6, and 0x366. 
*/

/*
This is the multiple IDE interface driver, as evolved from hd.c.

It supports up to 9 IDE interfaces per default, on one or more IRQs (usually
14 & 15).  There can be up to two drives per interface, as per the ATA-6 spec.

Primary:    ide0, port 0x1f0; major=3;  hda is minor=0; hdb is minor=64
Secondary:  ide1, port 0x170; major=22; hdc is minor=0; hdd is minor=64
Tertiary:   ide2, port 0x1e8; major=33; hde is minor=0; hdf is minor=64
Quaternary: ide3, port 0x168; major=34; hdg is minor=0; hdh is minor=64
fifth..     ide4, usually PCI, probed
sixth..     ide5, usually PCI, probed
*/


