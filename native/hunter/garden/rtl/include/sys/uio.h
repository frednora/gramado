

// See:
// https://man7.org/linux/man-pages/man2/writev.2.html

#ifndef _SYS_UIO_H_
#define	_SYS_UIO_H_


struct iovec {
	void	*iov_base;	/* Base address. */
	size_t	 iov_len;	/* Length. */
};



/*
 #todo
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);

ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

ssize_t preadv(int fd, const struct iovec *iov, int iovcnt,
             off_t offset);

ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt,
           off_t offset);

ssize_t preadv2(int fd, const struct iovec *iov, int iovcnt,
          off_t offset, int flags);

ssize_t pwritev2(int fd, const struct iovec *iov, int iovcnt,
           off_t offset, int flags);
 
*/


#endif /* !_SYS_UIO_H_ */   


