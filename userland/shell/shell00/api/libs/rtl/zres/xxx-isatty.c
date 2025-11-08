


//#include <termios.h>
//#include <sys/ioctl.h>



// POSIX.1-2001, POSIX.1-2008, SVr4, 4.3BSD.
/*
int isatty (int fd);
int isatty (int fd){

	struct termios dummy;

	return (!ioctl(fd, TCGETS, &dummy));
}
*/



