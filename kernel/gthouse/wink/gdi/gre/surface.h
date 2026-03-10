// surface.h
// Created by Fred Nora.


#ifndef __GRE_SURFACE_H
#define __GRE_SURFACE_H    1

//--------------------------------------

// Isso pode ser útil principalmente
// para passar um retângulo de um ambiente para outro.
// É muito mais didático que a figura do retângulo como objeto.
// #ps: 
// Check dc.h where we have the structure dc_d, which is the device context.
struct surface_d
{
    int used;
    int magic;
    int surface_id;

    struct rect_d rect;

    unsigned long width;
    unsigned long height;
    int bpp;  // Bits per pixel

    unsigned long pitch;
    size_t size_in_bytes;

// Window ID.
// This window owns this surface.
// The wid provided by the window server.
    int owner_wid;

    int dirty;

    struct surface_d *next;
};


#endif  

