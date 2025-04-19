
// kobject.h
// List of object types.
// A lot of structures are using these indexes
// to describe the onject handled by the structure.

#ifndef __KE_KOBJECT_H
#define __KE_KOBJECT_H    1

// Object types.
// This list is used in the element file->____object,
// to describe the 'operation mode. If the file is a console,
// or a pipe, or a socket, or a regular file.
typedef enum {

    ObjectTypeNull,               //0 Null. (Free)

// File
    ObjectTypeDirectory,          // diretório de arquivos
    ObjectTypeFile,               // (Regular file)
    ObjectTypePipe,               //
    ObjectTypeFileSystem,         // <<<< This file represents a 'File System'.

// Metafile
    // metafile just like GRAMADO or BRAMADO.
    // or even a hidden lba with no entry in the directory
    ObjectTypeMetafile,

    //ObjectTypeSerialPort,    //COM ports.
    
    ObjectTypeDevice,         // device ??
    ObjectTypeDisk,           //
    ObjectTypeDiskInfo,       // disk info,
    ObjectTypeDiskPartition,  // info struct    
    ObjectTypeVolume,         // disk volume 
    ObjectTypeTerminal,       // 
    ObjectTypeLine,           //
    ObjectTypePTMX,           // <<<< This file represents a pty multiplexer.  
    ObjectTypeTTY,            //
    ObjectTypePTY,            //
    ObjectTypeVirtualConsole, //
    ObjectTypeCharDev,        //
    ObjectTypeBlockDev,       //
    ObjectTypeNIC,     // NIC device
    ObjectTypeVideo,          // video
    ObjectTypeCpu,           
    ObjectTypeDma,  
    ObjectTypeProcessor,      // processor
    ObjectTypeProcessorBlock, // processor block 
    ObjectTypeMemory,         // memory card info.
    ObjectTypePIC,            // pic controller.
    ObjectTypeTimer,          // PIT timer
    ObjectTypeRTC,            // rtc controller, clock
    ObjectTypeKeyboard,       // keyboard device.
    ObjectTypeMouse,          // mouse device.
    ObjectTypeComputer,       //
    ObjectTypeCpuRegister,    //
    ObjectTypeGDT,            //
    ObjectTypeLDT,            //
    ObjectTypeIDT,            //
    ObjectTypeTSS,            //
    ObjectTypePort,           // (i/o port) generic
    ObjectTypeController,     //
    ObjectTypePciDevice,        // PCI device info generic
    ObjectTypePciDeviceDriver,  // PCI device driver info.
    ObjectTypeScreen,           // Screen.

    // klib/
    ObjectTypeSignal,     // unix signal object ??

    // net/
    ObjectTypeHostInfo,     // HostInfo. 
    ObjectTypeProtocol,     // protocol
    ObjectTypeSocket,       // sockets.
    ObjectTypeIpAddress,    // IP address.
    ObjectTypeMacAddress,   // MAC Address.
    ObjectTypeChannel,      // channel = two sockets.

    // ps/ 
    ObjectTypeProcess,            // Process.
    ObjectTypeThread,             // Thread.
    ObjectTypePageDirectory,      // page directory
    ObjectTypePageTable,          // page table
    ObjectTypePageFrame,          // page frame
    ObjectTypeFramePool,          // frame pool
    ObjectTypeASpace,             // Address Space. (memory address)
    ObjectTypeDSpace,             // Disk Space.
    ObjectTypeBank,               // Bank. (banco de dados).
    ObjectTypeHeap,               // heap
    ObjectTypeIoBuffer,           // i/o buffer
    ObjectTypeProcessMemoryInfo,  // process memory info
    ObjectTypePhysicalMemoryInfo, // physical memory info
    ObjectTypeMemoryInfo,         // memory info
    ObjectTypeMemoryBlock,        // memory block. Usado para swap.
    ObjectTypeSemaphore,          // Semaphore
    ObjectTypeMutex, 

    // security/
    ObjectTypeUser,               // user ??
    ObjectTypeUserInfo,           // userinfo ??
    ObjectTypeGroup,              // user group
    ObjectTypeUserSession,        // User session
    ObjectTypeToken,              // Token de acesso à objetos. (access token)

    // ws/
    ObjectTypeWindow,             // window 
    ObjectTypeRectangle,          // rectangle
    ObjectTypeMenu,               // menu
    ObjectTypeMenuItem,           // menuitem
    ObjectTypeButton,    
    ObjectTypeGrid,     
    ObjectTypeColorScheme,  
    ObjectTypeFont,  
    ObjectTypeIcon,  
    ObjectTypeRGBA,               // rgba 
    ObjectTypeWindowProcedure,    // window procedure
    ObjectTypeCursor, 
    ObjectTypeMessage,            // system message

    //kernel/
    ObjectTypeRequest,        // request de kernel.
    ObjectTypeKM,             // ??
    ObjectTypeUM,             // ??

// para validação de objetos não especificados 
// ou até nulos, talvez. :)
    ObjectTypeGeneric       

}object_type_t;


//
// Object class.
//

// Some structures are using this element
// to better describe what is the object it is handling.
typedef enum {

// #todo
// Maybe we can change these names
// and get some better ones.

    ObjectClassKernelObject,  // Kernel Objects
    ObjectClassUserObject     // User Objects
}object_class_t;


//
// prototypes ==========
//

// Nothing

#endif    



