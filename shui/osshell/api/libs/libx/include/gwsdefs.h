// gwsdefs.h
// Created by Fred Nora.

#ifndef __LIBGWS_GWSDEFS_H
#define __LIBGWS_GWSDEFS_H    1

/*
// wid
typedef int  _wid;
typedef int  __wid;
typedef int  wid;
typedef int  gws_wid;
*/

typedef int  __wid_t;
typedef int  wid_t;

#define __IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)


#endif   


