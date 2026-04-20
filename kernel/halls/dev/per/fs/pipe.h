// pipe.h
// Created by Fred Nora.

#ifndef __FS_PIPE_H
#define __FS_PIPE_H  1

// It will be included into the file structure.
struct pipe_info_d 
{
    char *base;       // shared buffer
    int buffer_size;  // buffer size (BUFSIZ)
    int counter;      // occupancy (bytes currently in pipe)

    int write_pos;  // write offset
    int read_pos;   // read offset
};


int 
pipe_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

int sys_dup (int oldfd);
int sys_dup2 (int oldfd, int newfd);
int sys_dup3 (int oldfd, int newfd, int flags);

// POSIX.1-2001, POSIX.1-2008.
// See: fs.c
int sys_pipe (int *pipefd, int flags);

int file_read_pipe_buffer( file *f, char *buffer, int len );
int file_write_pipe_buffer( file *f, char *buffer, int len );


int sys_read_pipe (int fd, char *ubuf, int count);
int sys_write_pipe (int fd, char *ubuf, int count);

#endif   
