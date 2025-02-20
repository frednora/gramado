// globals.c
// Created by Fred Nora.

// #test:
// Testing ioctl()
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctls.h>
#include <sys/ioctl.h>
#include <stdlib.h>
//#include <stdio.h>
//#include <unistd.h>

// #ps
// This thing is including a lot of libc headers too.
#include "include/terminal.h"

// Client-side library.
#include <gws.h>

int saved_argc=0;
char **saved_argv;


char pathname_buffer[PATHNAME_LENGHT];
int pathname_lenght=0;  //tamanho do atual pathname.
int pathname_initilized=0;


char filename_buffer[FILENAME_LENGHT];
int filename_lenght=0; //tamanho do nome de arquivo atual.
int filename_initilized=0;


int EOF_Reached=0;

char *current_volume_string;
int current_volume_id=0;

//...

int g_current_disk_id=0;
int g_current_volume_id=0;
int g_current_workingdirectory_id=0;  //??

// a string do diretório de trabalho.
char current_workingdiretory_string[WORKINGDIRECTORY_STRING_MAX];
int pwd_initialized=0;


struct _iobuf  *pwd;
struct _iobuf  *root;


