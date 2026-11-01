// fsload.h
// Created by Fred Nora

#ifndef __FS_FSLOAD_H
#define __FS_FSLOAD_H    1


// Load an image from GRAMADO/
int 
fsLoadProgramFromGRAMADO (
    char *program_name,
    unsigned long buffer,
    unsigned long buffer_size_in_bytes );

// Load an image from DE/
int 
fsLoadProgramFromDE (
    char *program_name,
    unsigned long buffer,
    unsigned long buffer_size_in_bytes );

int 
fs_load_path ( 
    const char *path, 
    unsigned long address, 
    unsigned long buffer_size );


int fsLoadFileFromCurrentTargetDir(void);

int 
fs_load_image(
    const char *filename, 
    unsigned long image_va );


#endif

