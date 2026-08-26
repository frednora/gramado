
// tksock.c
// The main goal of this project is create 
// wrapper routines to call the standard socket routines 
// found in the libc.
// One function will call several functions from the libs
// to make the job easier.

//ex
int tksock_socket(...)
{
    return (int) socket(...);
}



