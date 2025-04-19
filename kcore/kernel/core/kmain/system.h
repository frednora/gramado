// system.h
// Created by Fred Nora.

#ifndef __SYSTEM_H
#define __SYSTEM_H    1


// getpid_t:
//     Índices usados pela função gde_get_pid(x)
//     Usada para obter o pid de alguns drivers e servidores
// do sistema. Aqueles servidores que só podem ter um do tipo 
// rodando ao mesmo tempo.

typedef enum {
    GETPID_NULL,
    GETPID_WS,
    GETPID_WM
    //...
}getpid_t;

//
// ## sys storage support ##
//

//======================================== 
//Esses são os principais arquivos usandos na 
//inicialização do sistema. 
//Obs: No fat16 eles se apresentam em maiúsculas.
//#define INITTXT_PATH  "/root/init.txt"
//#define BM_PATH       "/root/bm.bin"
//#define BL_PATH       "/root/bl.bin"
//#define KERNEL_PATH   "/root/kernel.bin"
//#define INIT_PATH     "/root/init.bin"
//#define SHELL_PATH    "/root/shell.bin"
//#define TASKMAN_PATH  "/root/taskman.bin" 
//...
//========================================
 

/*
 * Abaixo temos uma lista de variaveis de ambiente usadas
 * pelo sistema, começadas com SYSTEM_.
 */ 

/*
 * Disks
 *    Discos físicos   
 */
#define SYSTEM_DISK0  "/disk0" 
#define SYSTEM_DISK1  "/disk1"
#define SYSTEM_DISK2  "/disk2"
#define SYSTEM_DISK3  "/disk3"
#define SYSTEM_DISK SYSTEM_DISK0  
//...

/*
 * Volumes
 *    Cada partição é um volume.
 *    Um volume pode ocupar mais de um disco. 
 */
#define SYSTEM_VOLUME0  "/volume0"   //vfs
#define SYSTEM_VOLUME1  "/volume1"   //partição de boot
#define SYSTEM_VOLUME2  "/volume2"   //partição do sistema.
#define SYSTEM_VOLUME3  "/volume3"   //??indefinido.

#define VHS_VOLUME    SYSTEM_VOLUME0
#define BOOT_VOLUME   SYSTEM_VOLUME1
#define SYSTEM_VOLUME SYSTEM_VOLUME2  
//...


#define SYSTEM_ROOT      "root:/"
#define SYSTEM_ROOTDIR   SYSTEM_ROOT      

/*
 * /disk0
 *     Esse será sempre o disco principal.
 *     Lista de diretórios dentro do disco principal.
 */
 
//
// Obs: Isso são opções ainda não definidas.
// 
 
//principais.  
#define SYSTEM_GUSERS    "/user"    // (os perfis de usuário ficarão aqui.)
#define SYSTEM_GCMD      "/cmd"      //COMANDO USADOS PELO SHELL CHAMARÃO OS PROGRAMAS QUE ESTÃO AQUI.
#define SYSTEM_GTMP      "/tmp"      //ARQUIVOS TEMPORÁRIOS
#define SYSTEM_GDOWNLOAD "/download"  //DOWNLOADS

//secundários.
#define SYSTEM_GBUILD    "/gbuild"   //Build
#define SYSTEM_GDOCS     "/gdocs"    //Docs
#define SYSTEM_GRAMADO   "/gramado"  //System (aqui ficarão os binários dos programas, em pastas)
#define SYSTEM_GSDK      "/gsdk"     //SDK 
#define SYSTEM_GSRC      "/gsrc"     //Source 


//...

//files (path para alguns arquivos principais)
#define SYSTEM_BM       "/BM.BIN"
#define SYSTEM_BL       "/BL.BIN"
#define SYSTEM_KERNEL   "/KERNEL.BIN"
#define SYSTEM_IDLE     "/IDLE.BIN"
#define SYSTEM_TASKMAN  "/TASKMAN.BIN"
#define SYSTEM_SHELL    "/SHELL.BIN"
#define SYSTEM_INIT     "/INIT.TXT"   // CONFIGURAÇÕES DE INICIALIZAÇÃO.
#define SYSTEM_SWAP     "/SWAP.BIN"   // (ARQUIVO DE PAGINAÇÃO)
//...

 
//Computer.
#define SYSTEM_COMPUTER    "COMPUTER=$"

//Hadware.
//...

//User.
#define SYSTEM_USER        "USER=/$"
#define SYSTEM_USERNAME    "USERNAME="
//...

//Media.
#define SYSTEM_MEDIADISK    "MEDIA=/disk(0000)"
#define SYSTEM_MEDIAVOLUME  "MEDIA=/volume(0000)"    //Partição.
//...
 
//

//devices.
#define SYSTEM_DEVICE          "DEVICE=/$"
#define SYSTEM_DEVICECPU       "DEVICE=/cpu"
#define SYSTEM_DEVICEMEMORY    "DEVICE=/memory"
#define SYSTEM_DEVICENIC       "DEVICE=/nic"     //NIC (placa de rede).
#define SYSTEM_DEVICEKEYBOARD  "DEVICE=/keyboard"
#define SYSTEM_DEVICEMOUSE     "DEVICE=/mouse"
//...


//
// #todo: 
// Continuar criando variáveis de ambiente.
//

//...


//
// SYSTEM METRICS INDEX 
//


//*Importante.
#define SM_NULL 0  
#define SM_SCREENWIDTH 1
#define SM_SCREENHEIGHT 2
#define SM_BPP          3 //Bits Per Pixel.
#define SM_BACKBUFFER_VA 4  //Virtual address for backbuffer.
#define SM_BACKBUFFER_PA 5  //phisical address for backbuffer.
#define SM_FRONTBUFFER_VA 6  //Virtual address for frontbuffer.
#define SM_FRONTBUFFER_PA 7  //phisical address for frontbuffer.
#define SM_KERNELSIZE 8
#define SM_KERNELHEAPSIZE 9
#define SM_KERNELSTACKSIZE 10
#define SM_CHARWIDTH 11
#define SM_CHARHEIGHT 12
#define SM_DEVELOPER_EDITION 13

#define SM_GETMOUSEX 20
#define SM_GETMOUSEY 21

//Continua ...


//procedure tupe mode
//@TODO: CRIAR UM ENUM
#define PROCEDURE_TYPE_MODE_TERMINAL  1    //digitando em um terminal/
#define PROCEDURE_TYPE_MODE_EDITBOX   2    //digitando em um editbox.
//...
 
//
// Globals.
// 


// Not so important
// See: kmain.c
extern int gSystemEdition;
extern int gSystemStatus;


/*
 * version_d:
 *     Estrutura para versão do sistema.    
 *     O que determina a versão do sistema é a versão do Kernel.
 * @todo: 
 *     Usar apenas esses 4 elementos.
 *     Dados complementares sobre a versão devem ficar 
 *     na estrutura versioninfo.
 * Obs: Talvez usar um header. version.h.
 * #bugbug: 
 * Já tem version_t tefinida em algum lugar.
 *         Aparentemente não deu mais problema. 
 */
struct version_d
{
    unsigned long Major;
    unsigned long Minor;
    unsigned long Build;
};
extern struct version_d  *Version;
//...

/*
 * version_info_d:
 *     Dados complemetares sobre a versão.
 */
struct version_info_d
{
    struct version_d *version;
    char *string;             //String para o nome da versão "VERSION..."
    char *copyright_string;   //"ex: (c) Copyright Nora Code"
    // ...
};
extern struct version_info_d  *VersionInfo;
//...

/*
 * system_d:
 *     System information structure:
 *     Essa estrutura guarda as informações gerais sobre o sistema.
 *     Guarda ponteiros para as principais estruturas do sistema, 
 * desde informações de boot, até informações seções de usuários.
 */
struct system_d
{
    int used;
    int magic;

    // Version
    struct version_d *version;
    struct version_info_d *version_info;

    // storage 
    int disk;
    int volume;

    // System  info.
    int ProcessorArchitecture;
    int NumberOfProcessors;
    int PageSize;

    // #todo
    //struct devices_d *deviceList;

    //...
};
extern struct system_d  *System;

//
// == Prototypes =====================
//


unsigned long doGetSystemMetrics(int index);
void doSetupVersion(void);

int do_reboot(unsigned long flags);
int zeroInitializeSystemComponents(void);

#endif    

