// dccanvas.h

#ifndef __LIBDISP_DCCANVAS_H
#define __LIBDISP_DCCANVAS_H  1 


//struct dc00_d 
struct dccanvas_d 
{
    int used;
    int magic;
    int initialized;

// pointer to framebuffer or backbuffer
    unsigned char *data;

// Hardware information
    unsigned long device_width;
    unsigned long device_height;
    unsigned long bpp;         // bytes per pixel
    unsigned long pitch;       // bytes per row
};


#endif   


