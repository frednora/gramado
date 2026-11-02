
// termios.c 

#include <stddef.h>
#include <types.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/ioctls.h>
#include <termios.h>
#include <stdio.h>


/*
int tcgetwinsize(int fd, struct winsize *ws)
{
    return ioctl(fd, TIOCGWINSZ, ws);
}
*/


// Get attributes.
// #maybe: TIOCGETA? TTY_GETS?
int tcgetattr (int fd, struct termios *termios_p)
{
    int value=-1;

    if (fd<0)
    {
        errno = EBADF;
        return (int) -1;
    }

    value = (int) ioctl(fd, TCGETS, termios_p);

    if(value<0)
    {
        errno = (-value);
        return (int) (-1);
    }
    return (int) value;
}


// Set attributes.
int 
tcsetattr ( 
    int fd, 
    int optional_actions,
    const struct termios *termios_p )
{
    int value = -1;

    if (fd<0)
    {
        errno = EBADF;
        return (int) -1;
    }

    switch (optional_actions){

    case TCSANOW:
        value = (int) ioctl(fd, TCSETS , termios_p);
        break;

    case TCSADRAIN:  
        value = (int) ioctl(fd, TCSETSW, termios_p);
        break;

    case TCSAFLUSH:  
        value = (int) ioctl(fd, TCSETSF, termios_p);
        break;

    // ...

    default:
        //debug_print ("tcsetattr: [FAIL] default\n");
        errno = EINVAL;
        return (int) -1;
        break;
    };

    if(value<0)
    {
        errno=(-value);
        return (int) (-1);
    }

    return (int) value;
}


// Send break.
int tcsendbreak (int fd, int duration)
{
    debug_print ("tcsendbreak: [TODO]\n");

    if (fd<0)
    {
        errno = EBADF;
        return (int) -1;
    }


    return -1; 
}


// Drain.
int tcdrain (int fd)
{
    int value = -1;
    
    if (fd<0)
    {
        errno = EBADF;
        return (int) -1;
    }

    value = (int) ioctl(fd, TIOCDRAIN, 0);

    if(value<0)
    {
        errno = (-value);
        return (int) -1;
    }
    
    return (int) value;
}


// Flush
int tcflush (int fd, int queue_selector)
{
    debug_print ("tcflush: [TODO]\n"); 

    if (fd<0)
    {
        errno = EBADF;
        return (int) -1;
    }

    return -1;

	/*
    int com;

	switch (queue_selector){

	case TCIFLUSH:
		com = FREAD;
		break;
	case TCOFLUSH:
		com = FWRITE;
		break;
	case TCIOFLUSH:
		com = FREAD | FWRITE;
		break;

	default:
		errno = EINVAL;
		return (-1);
	}
	return ( ioctl(fd, TIOCFLUSH, &com) );
	*/
}


// Flow
int tcflow (int fd, int action)
{
    debug_print ("tcflow: [TODO]\n");  

    if (fd<0)
    {
        errno = EBADF;
        return (int) -1;
    }

    return -1; 
}


// Make raw
void cfmakeraw (struct termios *termios_p)
{
    if ( (void *) termios_p == NULL ){
        debug_print ("cfmakeraw: termios_p\n");
        errno = EINVAL;
        return;
    }

    termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
                             INLCR | IGNCR | ICRNL | IXON);

    termios_p->c_oflag &= ~OPOST;

    termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    termios_p->c_cflag &= ~(CSIZE | PARENB);
    termios_p->c_cflag |= CS8;

    termios_p->c_cc[VMIN] = 1;
    termios_p->c_cc[VTIME] = 0;
}


// Get i speed.
speed_t cfgetispeed (const struct termios* tp)
{
    if ( (void *) tp == NULL )
    {
        debug_print ("cfgetispeed: tp\n");
        errno = EINVAL;
        return 0;  //??
    }

    return (speed_t) tp->c_ispeed;
}


// Get o speed.
speed_t cfgetospeed (const struct termios* tp)
{
    if ( (void *) tp == NULL ){
        debug_print ("cfgetospeed: tp\n");
        return 0;
    }

    return (speed_t) tp->c_ospeed;
}


// Set i speed.
int cfsetispeed (struct termios *termios_p, speed_t speed)
{
    if ( (void *) termios_p == NULL )
    {
        debug_print ("cfsetispeed: termios_p\n");
        errno = EINVAL;
        return (int) -1;
    }

    return -1;
}


// Set o speed.
int cfsetospeed (struct termios *termios_p, speed_t speed)
{
    if ( (void *) termios_p == NULL )
    {
        debug_print ("cfsetospeed: termios_p\n");
        errno = EINVAL;
        return -1;
    }

    return -1;
}


// Set speed.
int cfsetspeed(struct termios *termios_p, speed_t speed)
{
    if ( (void *) termios_p == NULL )
    {
        debug_print ("cfsetspeed: termios_p\n");  
        errno=EINVAL;
        return -1;
    }

    return -1;
}


