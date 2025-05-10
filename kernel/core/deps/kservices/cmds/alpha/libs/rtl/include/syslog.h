
// syslog.h
// 4.2BSD, SUSv2, POSIX.1-2001.
// See:
// https://man7.org/linux/man-pages/man3/syslog.3.html


#ifndef __SYSLOG_H
#define __SYSLOG_H  1

void openlog(const char *ident, int option, int facility);
void syslog(int priority, const char *format, ...);
void closelog(void);

//glibc only?
//void vsyslog(int priority, const char *format, va_list ap);

#endif   

