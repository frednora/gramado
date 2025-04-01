// path.h
// path support.

#ifndef __PATH_H
#define __PATH_H    1


#define  FS_MAX_NAME_LENGHT (8+3)
#define  WORKINGDIRECTORY_STRING_MAX 32   //64
// ...

// pathname separator and terminator.
#define  FS_PATHNAME_SEPARATOR   "/"
#define  FS_PATHNAME_SEPARATOR2   "\\"
#define  FS_PATHNAME_TERMINATOR  "\0"

// string separator and terminator.
#define  FS_STRING_SEPARATOR    "/"
#define  FS_STRING_SEPARATOR2   "\\"
#define  FS_STRING_TERMINATOR   "\0" 

/* Default search path. */
#define _PATH_DEFPATH  "/bin:/usr/bin:/sbin:/usr/sbin"

/* All standard utilities path. */
#define _PATH_STDPATH	"/bin:/usr/bin:/sbin:/usr/sbin"

#define _PATH_DEV      "/dev"
#define _PATH_DEV_PTS  "/dev/pts"
#define _PATH_TMP      "/tmp"
#define _PATH_TTY      "/dev/tty"
#define	_PATH_DEVNULL  "/dev/null"


#define  FS_ROOT_STRING           "/"
#define  FS_HOME_STRING           "~"
#define  FS_VOLUME0_STRING        "/volume0"   //vfs
#define  FS_VOLUME1_STRING        "/volume1"   //boot volume
#define  FS_VOLUME2_STRING        "/volume2"   //system volume.
#define  FS_UNKNOWNVOLUME_STRING  "/unknown-volume"

#define  FS_VFSWORKINGDIRECTORY_STRING      "/volume1"
#define  FS_BOOTWORKINGDIRECTORY_STRING     "/volume1"
#define  FS_SYSTEMWORKINGDIRECTORY_STRING   "/volume2"
#define  FS_UNKNOWNWORKINGDIRECTORY_STRING  "/unknown-directory"

#define  FS_DIR_VFS          "/vfs"  
#define  FS_DIR_BOOT         "/boot"  
#define  FS_DIR_SYSTEM       "/system"  
#define  FS_DIR_BM           "/BM.BIN" 
#define  FS_DIR_BL           "/BL.BIN"  
#define  FS_DIR_KERNEL       "/KERNEL.BIN"
#define  FS_DIR_INIT         "/INIT.BIN" 
#define  FS_DIR_INITINI      "/INIT.INI"  
#define  FS_DIR_BIN          "/bin"
#define  FS_DIR_DEV          "/dev"
#define  FS_DIR_LIB          "/lib" 
#define  FS_DIR_USER         "/user"
#define  FS_DIR_DEFAULTUSER  "/user/default"
// ...

#endif    



