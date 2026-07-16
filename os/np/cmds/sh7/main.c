// File: sh7.c
// Credits: Levos7 OS

//#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

//#ifndef MAC_HACK
#include <sys/utsname.h>
//#endif

// #syscalls
#include <rtl/gramado.h>


#define STRCMP(c, s, n) strlen(c) == n && strncmp(c, s, n)

char *the_prompt = "sh7 $ ";

/*
void
process_cmd(char *b)
{
    pid_t pid;
    int rc;
    char *arg;
    if (strncmp(b, "fork ", 5) == 0) {
        arg = b + 5;
        if (pid = fork()) {
            int status;

            //parent 
            //printf("parent\n");
            rc = waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                printf("Exit code: %d\n", WEXITSTATUS(status));
            else if (WIFSIGNALED(status))
                printf("Killed by signal: %d\n", WTERMSIG(status));
            else
                printf("Weird!\n");
            return;
        } else {
            //memcpy(the_prompt, "oops $ ", 7);

            // child 
            printf("child is pid %d\n", getpid());
            char *argvp[] = {
                "/lol",
                arg,
                NULL,
            };

#ifndef MAC_HACK
            execve("/lol", argvp, environ);
#endif
            exit(13);
        }
    }
#ifndef MAC_HACK
    if (STRCMP(b, "uname", 5) == 0) {
        struct uname name;

        uname(&name);

        printf("%s version %s%s, on %s machine named %s\n",
                name.sysname, name.release, name.version,
                name.machine, name.nodename);
        return;
    }
#endif

    if (*b != 0)
        printf("unknown command\n");
}
*/



int main (int argc, char **argv)
{
    char cmdbuf[128];
    register int i=0;
    int ret=0;

/* TODO: tty set raw mode */

    memset(cmdbuf, 0, 128);
    printf("There are %d args\n", argc);

    for (i = 0; i < argc; i ++)
        printf("arg %d: %s\n", i, argv[i]);


// #todo
// We need a better event loop here.
// See init.bin as an example.

    while (1) {
        volatile char c = 0;

prompt:
        printf (the_prompt);
        //fflush(stdout);
        //write(1, "lOS $ ", 6);

read_more:
        /*read(0, (char *) &c, 1);
        if (c == '\r') {
            printf("\n");
            cmdbuf[i] = 0;
            process_cmd(cmdbuf);
            i = 0;
            goto prompt;
        }
        cmdbuf[i++] = c;
        //printf("%c", c);
        */

        memset(cmdbuf, 0, 128);
        
        
        // #
        // Reading from stdin.
        
        // #todo
        // Isso é o input de teclado.
        ret = read (0, cmdbuf, 128);
        
        //printf("ret is %d\n", ret);
        
        if (ret == 1)
            goto prompt;
        
        if (ret == 0)
            exit(0);

        cmdbuf[strlen(cmdbuf) - 1] = 0;
        
        //printf("read \"%s\"\n", cmdbuf);

        //
        // Calling process_cmd()
        //

        printf ("OK: calling command! :0 )");
        //process_cmd(cmdbuf);

        goto prompt;
    };

    return EXIT_SUCCESS;

fail:
    return EXIT_FAILURE;
}

