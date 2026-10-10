// version.h
// Created by Fred Nora.

#ifndef __LIBGWS_VERSION_H
#define __LIBGWS_VERSION_H    1
//
// == Version ====
//

#define GWS_LIB_PROTOCOL           0
#define GWS_LIB_PROTOCOL_REVISION  1

struct libgws_version_d
{
    unsigned long majorVersion;
    unsigned long minorVersion;
    // ...
};
// Version struct.
// See: gws.c
extern struct libgws_version_d  libgwsVersion;

#endif   

