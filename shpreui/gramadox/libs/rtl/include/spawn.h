

#ifndef _SPAWN_H_
#define _SPAWN_H_


#include <sys/spawn.h>


int 
posix_spawn (
    pid_t *pid, 
    const char *path,
    const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp,
    char *const argv[], 
    char *const envp[] );
    
    
int 
posix_spawnp (
    pid_t *pid, 
    const char *file,
    const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp,
    char *const argv[], 
    char *const envp[] );

 
 
 
 
 
 
int spawnv(int mode, char *cmd, char **argv);
int spawnve(int mode, char *path, char *argv[], char *envp[]); 
int spawnvp(int mode, char *path, char *argv[]); 
int spawnvpe(int mode, char *path, char *argv[], char *envp[]); 
int spawnveg(const char* command, char** argv, char** envv, pid_t pgid);        

#endif	/* _SPAWN_H_ */


