// See: https://netbsd.gw.com/cgi-bin/man-cgi?intro+2+NetBSD-current

#include <stdio.h>
#include <stdarg.h>

       
void err(int eval, const char *fmt, ...)
{}

void errx(int eval, const char *fmt, ...)
{}

void warn(const char *fmt, ...)
{}

void warnx(const char *fmt, ...)
{}


//precisam de srtarg
void verr(int eval, const char *fmt, va_list args)
{}

void verrx(int eval, const char *fmt, va_list args)
{}

void vwarn(const char *fmt, va_list args)
{}

void vwarnx(const char *fmt, va_list args)
{}


