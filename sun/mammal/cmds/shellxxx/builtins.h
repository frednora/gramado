/*
 * File: builtins.h
 *     Buittin commands for Gramado Core Shell.
 * History:
 *     2018 - Created by Fred Nora.
 */

#ifndef __SHELL00_BUILTINS_H
#define __SHELL00_BUILTINS_H    1
 
#define fgdfgd  44 
 
void cd_buitins(void);

void cls_builtins(void);

void copy_builtins(void);
void date_builtins(void);
void del_builtins(void);
void dir_builtins(void);

void echo_builtins(char *list[]);

void exec_builtins(void);
void exit_builtins(void);
int getpid_builtins(void);
int getppid_builtins(void);
int getgid_builtins(void);
int getuid_builtins(void);
void help_builtins(int arg);
void pwd_builtins(void);

#endif   

