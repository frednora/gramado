// comp.c
// The purpose of these routines is compose the final frame
// into the backbuffer and display it into the frontbuffer.
// Created by Fred Nora.

// In this routines all the windows are drawn into the backbuffer and 
// the backbuffer is copyed to the front buffer. But i am introducing 
// the idea of having a small offscreen area where i am testing the 
// features necessary to have a full compositor with offscreen areas for windows.
// The called: Per-window offscreen buffers (canvases)

/*
Pipeline:
 1. DCs for drawing → isolated painting into canvases.
 2. Canvas‑to‑canvas blits → flexible composition, like brush strokes.
 3. Backbuffer → frontbuffer flush → final presentation.
*/

// comp.c:
// stays focused on buffer management, composition, and flush logic.

#include "../ds.h"



// It manages the compositor behavior.
struct compositor_d  Compositor;

// Spare buffer
struct spare_buffer_clip_info_d  SpareBufferClipInfo;
char *spare_128kb_buffer_p;

struct dccanvas_d *spare_dccanvas;
struct dccanvas_d *test00_dccanvas;
struct dccanvas_d *bg_dccanvas;
// 

// The whole desktop was updated
// int gCompositorUpdateDesktop = FALSE;

// #todo
// Create some configuration globals here
// int gUseSomething = TRUE;
// ...

// Mouse hover
extern struct gws_window_d *mouse_hover;
// The limits for the mouse pointer.
// Normally it's the screen size (root window),
// but it can be the client area of an application window 
// when the mouse is captured by an application window.
extern struct gws_window_d *cursor_clip;


//old
static long __old_mouse_x=0;
static long __old_mouse_y=0;
//current
static long __new_mouse_x=0;
static long __new_mouse_y=0;
static int __mouse_initialized = FALSE;

// Apaga mouse pointer.
// Let's clear the area where the mouse was painted.
// Flushing the area from backbuffer to LFB.
// It erases the pointer in the screen.
static int __clear_mousebox = FALSE;


int need_rootwindow_redraw = FALSE;

// --------------------------

static void direct_draw_mouse_pointer(void);

// --------------------------

// Refresh screen via kernel.
// Copy the backbuffer in the frontbuffer(lfb).
// #??
// It uses the embedded display server in the kernel.
//#define	SYSTEMCALL_REFRESHSCREEN        11
// #todo
// trocar o nome dessa systemcall.
// refresh screen será associado à refresh all windows.
// Initialize the spare buffer clipping info
void 
setup_spare_buffer_clip(
    unsigned long width,
    unsigned long height,
    unsigned long bpp,
    void *base )
{

    // #bugbug: Not used yet.
/*
    // Mark as not initialized until all fields are set
    SpareBufferClipInfo.initialized = 0;

    SpareBufferClipInfo.width  = (unsigned long) width;
    SpareBufferClipInfo.height = (unsigned long) height;
    SpareBufferClipInfo.bpp    = (unsigned long) bpp;

    // Calculate pitch: width * bytes per pixel
    SpareBufferClipInfo.pitch = (unsigned long) (width * bpp);

    // Base pointer to buffer memory
    SpareBufferClipInfo.base = (unsigned long) base;

    // Now mark as initialized
    SpareBufferClipInfo.initialized = 1;
*/
}


//==============================================================
// comp_create_spare_buffer
//
// Allocate a pointer into the spare region of the global backbuffer.
//
// Parameters:
//   size_in_kb - requested buffer size in kilobytes.
//
// Behavior:
//   - Reads device metrics (width, height, bytes per pixel).
//   - Computes the visible backbuffer size in bytes.
//   - Compares against the fixed 2 MB allocation.
//   - If there is enough spare space (>= size_in_kb), returns
//     a pointer to the start of the spare region immediately
//     after the visible backbuffer.
//   - Otherwise returns NULL.
//
// Notes:
//   - Currently assumes a fixed 2 MB backbuffer allocation.
//   - Caller is responsible for ensuring the buffer does not
//     exceed the spare region or overlap with other allocations.
//   - Alignment is not enforced; add rounding if hardware requires.
//
// Return:
//   void* pointer to the allocated spare buffer region,
//   or NULL if insufficient space.
//==============================================================

// The compositor:
// Doesn’t care who drew the pixels or how.
// Its job is purely to check flags 
// (dirty, show_when_creating, redraw, etc.) and 
// decide whether to copy/blit the buffer into the global backbuffer.
void *comp_create_slab_spare_128kb_buffer(size_t size_in_kb)
{
    SpareBufferClipInfo.initialized = FALSE;

// Device info
// #bugbug
// If you ever change resolution or bpp, recompute the spare size before writing.
    unsigned long DeviceWidth  = (unsigned long) server_get_system_metrics(1);
    unsigned long DeviceHeight = (unsigned long) server_get_system_metrics(2);

    // Bits per pixel
    unsigned long DeviceBPP    = (unsigned long) server_get_system_metrics(9);

// Backbuffer Address
    unsigned long BackbufferAddress = (unsigned long) rtl_get_system_metrics(12);
// Backbuffer total size
// #bugbug: Constant for now. This is gonna change.
    unsigned long TotalBackbufferInBytes = (2*1024*1024);  // 2 MB

// Pitch
// Backbuffer visible area. (Screen size)
    unsigned long Pitch = 
        (DeviceWidth * (DeviceBPP/8));

    unsigned long BackbufferSizeInBytes = (DeviceHeight * Pitch);
    unsigned long BackbufferSizeInKB = (BackbufferSizeInBytes/1024);

// Spare area
    unsigned long TotalSpareInBytes = (TotalBackbufferInBytes - BackbufferSizeInBytes);
    unsigned long TotalSpareInKB = (TotalSpareInBytes / 1024);
    unsigned long addr;

    struct dccanvas_d *tmp_dccanvas;


    if (TotalSpareInKB > size_in_kb){
        addr = (unsigned long) BackbufferAddress + BackbufferSizeInBytes;

        // --- Setup clipping info for spare buffer ---
        SpareBufferClipInfo.initialized = FALSE;

        SpareBufferClipInfo.bpp    = DeviceBPP;  // bytes per pixel
        SpareBufferClipInfo.pitch  = Pitch;      // Same of the device

        SpareBufferClipInfo.width  = DeviceWidth;   // align with OS-supported width
        SpareBufferClipInfo.height = (TotalSpareInBytes/Pitch); 

        SpareBufferClipInfo.base   = (void*) addr;

        SpareBufferClipInfo.initialized = TRUE;
        // -------------------------------------------

        tmp_dccanvas = 
            (void *) libgd_create_dc (
                SpareBufferClipInfo.base,
                SpareBufferClipInfo.width,
                SpareBufferClipInfo.height,
                SpareBufferClipInfo.bpp      // bits per pixel
             );

        // Saving
        spare_dccanvas = tmp_dccanvas;
        if ((void*)spare_dccanvas == NULL)
        {
            spare_dccanvas = NULL;
            return NULL;
        }
        if (spare_dccanvas->magic != 1234)
        {
            spare_dccanvas = NULL;
            return NULL;
        }

        return (void*) addr;

    } else {
        SpareBufferClipInfo.initialized = FALSE;
        spare_dccanvas = NULL;
        return NULL;
    }

// fail
    SpareBufferClipInfo.initialized = FALSE;
    spare_dccanvas = NULL;
    return NULL;
}

// Create a dc for a buffer given its size in KB.
struct dccanvas_d *comp_create_dc_for_a_buffer(
    char *buffer_address, size_t size_in_kb )
{

// Device info
// #bugbug
// If you ever change resolution or bpp, recompute the spare size before writing.
    unsigned long DeviceWidth  = (unsigned long) server_get_system_metrics(1);
    unsigned long DeviceHeight = (unsigned long) server_get_system_metrics(2);
    // Bits per pixel
    unsigned long DeviceBPP    = (unsigned long) server_get_system_metrics(9);

// Pitch
// Backbuffer visible area. (Screen size)
    unsigned long Pitch = (DeviceWidth * (DeviceBPP/8));

// new area
    unsigned long TotalSpareInBytes = (size_in_kb * 1024);

    // Allocate a new buffer in heap memory
    //char *buf = (char *) malloc(TotalSpareInBytes);
    char *buf = (char *) buffer_address;
    if (!buf) 
        return NULL;

    //----------------------------

    struct dccanvas_d *tmp_dccanvas;

    // --- Setup clipping info for spare buffer ---

    unsigned long BufferClipInfo_bpp    = DeviceBPP;  // bytes per pixel
    unsigned long BufferClipInfo_pitch  = Pitch;      // Same of the device

    unsigned long BufferClipInfo_width  = DeviceWidth;   // align with OS-supported width
    unsigned long BufferClipInfo_height = (TotalSpareInBytes/Pitch); 

    // -------------------------------------------

    tmp_dccanvas = 
        (void *) libgd_create_dc (
            (char *) buf,
            BufferClipInfo_width,
            BufferClipInfo_height,
            BufferClipInfo_bpp      // bits per pixel
        );

    if ((void*)tmp_dccanvas == NULL){
        return NULL;
    }
    if (tmp_dccanvas->magic != 1234){
        return NULL;
    }

    return (void*) tmp_dccanvas;  // return dc

// fail
fail:
    return NULL;
}

struct dccanvas_d *comp_create_dc_and_allocate_buffer(size_t size_in_kb)
{

// Device info
// #bugbug
// If you ever change resolution or bpp, recompute the spare size before writing.
    unsigned long DeviceWidth  = (unsigned long) server_get_system_metrics(1);
    unsigned long DeviceHeight = (unsigned long) server_get_system_metrics(2);
    // Bits per pixel
    unsigned long DeviceBPP    = (unsigned long) server_get_system_metrics(9);

// Pitch
// Backbuffer visible area. (Screen size)
    unsigned long Pitch = (DeviceWidth * (DeviceBPP/8));

// new area
    unsigned long TotalSpareInBytes = (size_in_kb * 1024);

    // Allocate a new buffer in heap memory
    char *buf = (char *) malloc(TotalSpareInBytes);
    if (!buf) 
        return NULL;

    //----------------------------

    struct dccanvas_d *tmp_dccanvas;

    // --- Setup clipping info for spare buffer ---

    unsigned long BufferClipInfo_bpp    = DeviceBPP;  // bytes per pixel
    unsigned long BufferClipInfo_pitch  = Pitch;      // Same of the device

    unsigned long BufferClipInfo_width  = DeviceWidth;   // align with OS-supported width
    unsigned long BufferClipInfo_height = (TotalSpareInBytes/Pitch); 

    // -------------------------------------------

    tmp_dccanvas = 
        (void *) libgd_create_dc (
            (char *) buf,
            BufferClipInfo_width,
            BufferClipInfo_height,
            BufferClipInfo_bpp      // bits per pixel
        );

    if ((void*)tmp_dccanvas == NULL){
        return NULL;
    }
    if (tmp_dccanvas->magic != 1234){
        return NULL;
    }

    return (void*) tmp_dccanvas;  // return dc

// fail
fail:
    return NULL;
}

void comp_add_to_list(struct canvas_information_d *ci)
{
    struct canvas_information_d *tmp;

    if ((void*) ci == NULL)
        return;
// ...


// Empty list
    if ((void*) canvas_head == NULL)
    {
        canvas_head = ci;
        ci->next = NULL;
        return;
    }


// Walk the list
    tmp = (struct canvas_information_d *) canvas_head;
    while (1)
    {
        if ((void*) tmp->next == NULL)
        {
            tmp->next = ci;
            ci->next = NULL;
            break;
        }
        tmp = tmp->next;
    };
}


// w, h, bits per pixel, dc
struct canvas_information_d *compCreateNewCanvas(struct dccanvas_d *dc)
{
    //char *buf;
    struct canvas_information_d *ci_new;

    if ((void*) dc == NULL){
        printf("compCreateNewCanvas: dc\n"); 
        goto fail;
    }

// Create structure for canvas information
    ci_new = (void*) malloc(sizeof(struct canvas_information_d));
    if ((void*) ci_new == NULL){
        printf("compCreateNewCanvas: ci_newd\n"); 
        goto fail; 
    }

    ci_new->width  = dc->device_width; 
    ci_new->height = dc->device_height; 
    ci_new->bpp    = dc->bpp;   // bits per pixel

    unsigned long DeviceWidth  = (unsigned long) server_get_system_metrics(1);

// Pitch
// Backbuffer visible area. (Screen size)
    //unsigned long Pitch = 
        //(DeviceWidth * (dc->bpp/8));

// bytes per row (width * bytes_per_pixel)
    ci_new->pitch = dc->pitch; //Pitch;

    ci_new->base = (void*) dc->data; 

    ci_new->dc = (struct dccanvas_d *) dc;

    // It belongs to the root window for now.
    ci_new->owner_window = __root_window; 
    ci_new->used = TRUE;
    ci_new->magic = 1234;
    ci_new->initialized = TRUE;

    return (struct canvas_information_d*) ci_new;
fail:
    return NULL;
}


// Create a drawable buffer in a spare area 
// at the end of the backbuffer.
// The compositor:
// Doesn’t care who drew the pixels or how.
// Its job is purely to check flags 
// (dirty, show_when_creating, redraw, etc.) and 
// decide whether to copy/blit the buffer into the global backbuffer.

struct canvas_information_d *compCreateCanvasUsingSpareBuffer(void)
{
    void *b;

    // 10*4*10 is ok.
    // 64 is ok.
    size_t WindowSizeInKB = 64; 
    b = (void *) comp_create_slab_spare_128kb_buffer(WindowSizeInKB);
    if ((void*) b == NULL){
        return NULL;
    }

    spare_128kb_buffer_p = b;

// ------------------------------------------------------
// Create canvas object for the spare buffer 
    struct canvas_information_d *ci_sparebuffer;
    ci_sparebuffer = (void*) malloc(sizeof(struct canvas_information_d));
    if ((void*) ci_sparebuffer == NULL){
        printf("compCreateCanvasUsingSpareBuffer: malloc failed\n"); 
        return NULL; 
    }
    ci_sparebuffer->width = SpareBufferClipInfo.width; 
    ci_sparebuffer->height = SpareBufferClipInfo.height; 
    ci_sparebuffer->bpp = SpareBufferClipInfo.bpp; 
    ci_sparebuffer->pitch = SpareBufferClipInfo.pitch; 
    ci_sparebuffer->base = (void*) spare_128kb_buffer_p; 

    ci_sparebuffer->dc = NULL;  // No dc for now

    // It belongs to the root window for now.
    ci_sparebuffer->owner_window = __root_window; 
    ci_sparebuffer->used = TRUE;
    ci_sparebuffer->magic = 1234;
    ci_sparebuffer->initialized = TRUE;

// Save into global list 
    canvasList[CANVAS_SPAREBUFFER] = (unsigned long) ci_sparebuffer;

// -----------------------------------------------------

// Draw something early.
// First: draw a test pattern into the spare buffer
    //if (CONFIG_TEST_SPARE_BUFFER == 1)
    //{
        comp_draw_into_spare_buffer();

        // #debug:
        // Draw something early.
        // Draw a red pixel at (0,0) inside the spare buffer
        // putpixel0(0xFFFF0000, 0, 0, ROP_COPY, (unsigned long) b);
    //}

    return (struct canvas_information_d *) ci_sparebuffer;
};

// Draw a pixel into the spare buffer. Using clipping.
void 
spare_putpixel0(
    unsigned int color, 
    unsigned long x, 
    unsigned long y, 
    unsigned long rop )
{
// Not initialized.
    if (SpareBufferClipInfo.initialized != TRUE)
        return;

    if (SpareBufferClipInfo.width == 0)
        return;
    if (SpareBufferClipInfo.height == 0)
        return;

// Clipping
// Using 'unsigned long'.
    if (x >= SpareBufferClipInfo.width)
        return;
    if (y >= SpareBufferClipInfo.height)
        return;

// # PF
// #todo: Check against the limits this application can access
    if ((void*) SpareBufferClipInfo.base == NULL)
        return;

// Draw it

    if (!spare_dccanvas) 
        return;

// #test: New method with dc.
    putpixel0(
        spare_dccanvas,
        color, 
        x, 
        y, 
        rop );
}

// Test drawing directly into the spare buffer using putpixel0.
// Only draws pixels, does not blit or refresh.
void comp_draw_into_spare_buffer(void)
{

    //if (!spare_dccanvas) 
       // return;

/*
// Old test without clipping
    if ((void*) spare_128kb_buffer_p == NULL)
        return;
    unsigned long canvas = (unsigned long) spare_128kb_buffer_p;
    // Draw a few colored pixels in different positions
    putpixel0(0xFFFF0000, 0, 0, ROP_COPY, canvas);
    putpixel0(0xFFFF0000, 9, 0, ROP_COPY, canvas);
    putpixel0(0xFFFF0000, 9, 9, ROP_COPY, canvas);
    putpixel0(0xFFFF0000, 0, 9, ROP_COPY, canvas);
*/

// New test using clipping
    spare_putpixel0(0xFFFF0000,  1,  1, ROP_COPY);
    spare_putpixel0(0xFFFF0000, 10,  1, ROP_COPY);
    spare_putpixel0(0xFFFF0000, 10, 10, ROP_COPY);
    spare_putpixel0(0xFFFF0000,  1, 10, ROP_COPY);

    // #test: Draw char
    // dc_drawchar(spare_dccanvas, 10, 2, 'H', COLOR_YELLOW, COLOR_BLUE, ROP_COPY);
    // dc_drawchar(spare_dccanvas, 18, 2, 'i', COLOR_YELLOW, COLOR_BLUE, ROP_COPY);

    // Draw string
    dc_drawstring ( 
        spare_dccanvas,  //dc 
        10,  // x
        2,  // y
        COLOR_YELLOW,  //fg_color
        COLOR_BLUE,    //bg_color
        ROP_COPY,  //rop
        "Spare buffer" 
    );

/*
    spare_putpixel0(
        0xFFFF0000, 
        (SpareBufferClipInfo.width >> 1), 
        (SpareBufferClipInfo.height >> 1), 
        ROP_COPY );
*/
}

void comp_test_spare_buffer(void)
{
    unsigned long BackbufferAddress = (unsigned long) rtl_get_system_metrics(12);
    if ((void*)spare_128kb_buffer_p == NULL)
        return;
    unsigned char *dst = (unsigned char *) BackbufferAddress;
    unsigned char *src = (unsigned char *) spare_128kb_buffer_p;
    int i=0;
    int max=10*4*10;
    // Copy byte by byte
    for (i=0; i<max; i++){
        dst[i] = src[i];
    };
}

// Copy from spare buffer (0,0) into backbuffer at (dst_x, dst_y).
void comp_blit_spare_to_backbuffer(
    int dst_x, int dst_y,
    int width, int height )
{
    unsigned long backbuffer_addr = (unsigned long) rtl_get_system_metrics(12);
    unsigned long spare_addr      = (unsigned long) spare_128kb_buffer_p;

    if (!spare_addr || !backbuffer_addr)
        return;

    // Use rectangle worker with source fixed at (0,0).
    __refresh_rectangle1(
        width, height,
        dst_x, dst_y, backbuffer_addr,   // destination
        0, 0, spare_addr                 // source (always top-left)
    );
}

// #test
// Blit from one canvas into another.
// src_canvas: source canvas (offscreen, spare, window, etc.)
// dst_canvas: destination canvas (backbuffer, frontbuffer, etc.)
void 
comp_blit_canvas_to_canvas_imp(
    struct canvas_information_d *src_canvas,
    struct canvas_information_d *dst_canvas,
    int dst_x, int dst_y,
    int width, int height )
{

// Validation
    if ((void*) src_canvas == NULL || (void*) dst_canvas == NULL)
        return;
    if (src_canvas->used != TRUE || dst_canvas->used != TRUE)
        return;
    if (src_canvas->magic != 1234 || dst_canvas->magic != 1234)
        return;
    if (src_canvas->initialized != TRUE || dst_canvas->initialized != TRUE)
        return;
    if (src_canvas->base == NULL || dst_canvas->base == NULL)
        return;

    // Copy rectangle from source canvas (always top-left for now)
    __refresh_rectangle1(
        width, height,
        dst_x, dst_y, (unsigned long) dst_canvas->base,   // destination
        0, 0, (unsigned long) src_canvas->base            // source
    );
}

// #test
// Given the indexes
void 
comp_blit_canvas_to_canvas(
    int id_src_canvas,
    int id_dst_canvas,
    int dst_x, int dst_y,
    int width, int height )
{
    struct canvas_information_d *src;
    struct canvas_information_d *dst;

// We have few canvases for now.
    if (id_src_canvas < 0 || id_src_canvas >= CANVAS_COUNT_MAX)
        return;
    if (id_dst_canvas < 0 || id_dst_canvas >= CANVAS_COUNT_MAX)
        return;
    // #todo: More filters.

    src = (struct canvas_information_d *) canvasList[id_src_canvas];
    if ((void *) src == NULL)
        return;
    if (src->magic != 1234)
        return;

    dst = (struct canvas_information_d *) canvasList[id_dst_canvas];
    if ((void *) dst == NULL)
        return;
    if (dst->magic != 1234)
        return;

    comp_blit_canvas_to_canvas_imp(
        (struct canvas_information_d *) src,
        (struct canvas_information_d *) dst,
        dst_x, dst_y, width, height 
    );
}

// ??
// Using the kernel to show the backbuffer.
void gwssrv_show_backbuffer(void)
{
    gramado_system_call(11,0,0,0);
}

int flush_window(struct gws_window_d *window)
{
    //if ((void*) window == NULL)
        //return -1;
    return (int) gws_show_window_rect(window);
}

int flush_window_by_id(int wid)
{
    struct gws_window_d *w;
// wid
    if (wid<0 || wid >= WINDOW_COUNT_MAX){
        goto fail;
    }
// Structure validation
    w = (void*) windowList[wid];
    if ((void*) w == NULL){
        goto fail;
    }
// Flush
    flush_window(w);
    return 0;
    //return (int) flush_window(w);
fail:
    return (int) (-1);
}

// Flush the whole backbuffer.
// see: painter.c
void flush_frame(void)
{
    wm_flush_screen();
}

// gws_show_window_rect:
// Show the rectangle of a window that was painted in the main backbuffer.
// Copy from backbuffer to frontbuffer.
// Does it need vsync?
// #todo: criar um define chamado refresh_window.
// ??
// Devemos validar essa janela, para que ela 
// não seja redesenhada sem antes ter sido suja?
// E se validarmos alguma janela que não está pronta?
// #test: validando

int gws_show_window_rect(struct gws_window_d *window)
{
    struct gws_window_d *parent;

    //#debug
    //debug_print("gws_show_window_rect:\n");

// Parameter:
    if ((void *) window == NULL){
        goto fail;
    }
    if (window->used != TRUE){
        goto fail;
    }
    if (window->magic != 1234){
        goto fail;
    }

//#shadow 
// ?? E se a janela tiver uma sombra, 
// então precisamos mostrar a sombra também. 
//#bugbug
//Extranhamente essa checagem atraza a pintura da janela.
//Ou talvez o novo tamanho favoreça o refresh rectangle,
//ja que tem rotinas diferentes para larguras diferentes

    //if ( window->shadowUsed == 1 )
    //{
        //window->width = window->width +4;
        //window->height = window->height +4;
        //refresh_rectangle ( window->left, window->top, 
        //    window->width +2, window->height +2 ); 
        //return (int) 0;
    //}

    //p = window->parent;

// We can't show a minimied window.
// We need to restore it first.
    if (window->state == WINDOW_STATE_MINIMIZED)
        goto fail;

// We can't draw if the parent's type is WT_OVERLAPPED and it's minimized.
    parent = (struct gws_window_d *) window->parent;
    if ((void*) parent != NULL)
    {
        if (parent->magic == 1234)
        {
            if (parent->type == WT_OVERLAPPED)
            {
                if (parent->state == WINDOW_STATE_MINIMIZED)
                    goto fail;
            }
        }
    }

// Refresh rectangle
// See: rect.c   
    gws_refresh_rectangle ( 
        window->absolute_x, 
        window->absolute_y, 
        window->width, 
        window->height ); 

    validate_window(window);
    return 0;

fail:
    // #slow
    //debug_print("gws_show_window_rect: fail\n");
    return (int) -1;
}

// Sinaliza que precisamos apagar o ponteiro do mouse,
// copiando o conteudo do backbuffer no LFB.
void DoWeNeedToEraseMousePointer(int value)
{
    if ( value != FALSE && 
         value != TRUE )
    {
        return;
    }
    __clear_mousebox = (int) value;
}

// Find the new mouse_hover window.
// What is the window where the mouse pointer is inside?
// Compare the global variables for mouse pointer 
// against the windows dimensions to find the perfect match.
// mouse_hover is the pointer for the window with the mouse pointer.
// Compare the mouse position (__new_mouse_x and__new_mouse_y)
// against the windows. 

// #todo: 
// Order:  taskbar --> app window --> children


struct gws_window_d *mouse_at(void)
{
// #deprecated
// Our new hit-testing is in wm.c.
// See wm_hit_test_2().

/*

    struct gws_window_d *new_hover_window;
    register int i=0;

// Check if the mouse pointer is already in mouse_hover.
// Compare the mouse position (__new_mouse_x and__new_mouse_y)
// against the mouse_hover window. 

    // For a valid mousehover window
    if ((void*) mouse_hover != NULL)
    {
        if (mouse_hover->magic == 1234)
        {
            if ( __new_mouse_x > mouse_hover->absolute_x &&
                 __new_mouse_x < mouse_hover->absolute_right &&
                 __new_mouse_y > mouse_hover->absolute_y &&
                 __new_mouse_y < mouse_hover->absolute_bottom )
            {
                // Not the root
                if (mouse_hover != __root_window)
                {
                    //mouse_hover = (void *) w;
                    //redraw_window(w,TRUE);

                    return (struct gws_window_d *) mouse_hover;
                }
            }
        }
    }

// Find the new mouse_hover window.
// Compare the mouse position (__new_mouse_x and__new_mouse_y)
// against all the mouse_hover window. 

// #bugbug
// We gotta check window inside window.

    // Find a new mousehover window
    for (i=0; i<WINDOW_COUNT_MAX; i++)
    {
        // Target
        new_hover_window = (void*) windowList[i];

        if ((void*) new_hover_window != NULL)
        {
            if (new_hover_window->magic == 1234)
            {
                if ( __new_mouse_x > new_hover_window->absolute_x &&
                     __new_mouse_x < new_hover_window->absolute_right &&
                     __new_mouse_y > new_hover_window->absolute_y &&
                     __new_mouse_y < new_hover_window->absolute_bottom )
                {
                    // Not the root. So accept this hover window.
                    if (new_hover_window != __root_window)
                    {
                        mouse_hover = (void *) new_hover_window;

                        return (struct gws_window_d *) mouse_hover;
                    }
                }
            }
        }
    };

*/

    return NULL;  // No hit
}

long comp_get_mouse_x_position(void)
{
    return (long) __new_mouse_x;
}

long comp_get_mouse_y_position(void)
{
    return (long) __new_mouse_y;
}

// TODO:
// This routine should eventually be moved into a more appropriate file,
// such as painter.c or mouse.c, since it is directly related to mouse
// rendering logic.
//
// Note:
// This is a low-level routine that interacts with the display device driver.
// It draws the mouse pointer directly into the frontbuffer (LFB).
//
static void direct_draw_mouse_pointer(void)
{
    // TODO:
    // Each window may define its own type of mouse pointer.
    // We should check the mouse_hover window's structure to determine
    // how to paint the pointer for that specific window.
    // See: window->mpp structure.

// Default rectangle parameters for the pointer
    unsigned long rectLeft   = __new_mouse_x;
    unsigned long rectTop    = __new_mouse_y;
    unsigned long rectWidth  = 8;
    unsigned long rectHeight = 8;

    unsigned int rectColor = COLOR_RED;  // Default color
    unsigned long rectROP = 0;           // Raster operation

    struct gws_window_d *w;

// Pointer color:
// If the hovered window is valid, use its mpp.bg_color
    w = (struct gws_window_d *) get_mousehover();
    if ((void*)w != NULL){
        if (w->magic == 1234){
            rectColor = (unsigned int) w->mpp.bg_color;
        }
    }

// Drag-and-drop visual feedback:
// Adjust pointer size depending on drag state
    if (DragAndDropInfo.is_pressed == TRUE)
    {
        if (DragAndDropInfo.is_dragging == TRUE){
            rectWidth  += 8;  // Larger when dragging
            rectHeight += 8;
        } else {
            rectWidth  -= 4;  // Smaller when pressed but not dragging
            rectHeight -= 4;
        }
    }

// Draw the rectangle directly into the frontbuffer (LFB).
// This bypasses the backbuffer and paints immediately on screen.

    frontbuffer_draw_rectangle( 
        (unsigned long) rectLeft, (unsigned long) rectTop, 
        (unsigned long) rectWidth, (unsigned long) rectHeight, 
        (unsigned int) rectColor, 
        (unsigned long) rectROP );
}

// + Apaga o cursor antigo, copiando o conteudo do backbuffer
// + Pinta o novo cursor diretamente no lfb.
void __display_mouse_cursor(void)
{
    unsigned long rWidth = 16;
    unsigned long rHeight = 16;

// Display server not initialized yet.
    if ((void*) display_server == NULL)
        return;
    if (display_server->initialized != TRUE)
        return;

// Mouse not initialized yet.
    if (gUseMouse != TRUE)
        return;

    if (gDisplayMousePointer != TRUE)
        return;

// #todo Limits
// Precisa inicializar os valores sobre o mouse.
// Precisa criar uma estrura pra eles.

    if ( __old_mouse_x<0 ){ __old_mouse_x=0; }
    if ( __old_mouse_y<0 ){ __old_mouse_y=0; }

    if ( __new_mouse_x<0 ){ __new_mouse_x=0; }
    if ( __new_mouse_y<0 ){ __new_mouse_y=0; }

//------
// We need to clear the mousebox,
// refreshing the content from backbuffer to LFB.
// We call it when we receive an 'mouse move' event.
    if (__clear_mousebox == TRUE)
    {
        gws_refresh_rectangle(
            __old_mouse_x, __old_mouse_y, rWidth, rHeight );
        DoWeNeedToEraseMousePointer(FALSE);
    }

// save
    __old_mouse_x = __new_mouse_x;
    __old_mouse_y = __new_mouse_y;

// ---------------------------
// Draw the pointer direcly into the LFB.
// Not printing it into the backbuffer.
// It uses the new values.
    direct_draw_mouse_pointer();
//------ 
}


// reactRefreshDirtyWindows: 
// Called by wmReactToPaintEvents().
// O compositor deve ser chamado para compor um frame 
// logo após uma intervenção do painter, que reaje às
// ações do usuário.
// Ele não deve ser chamado X vezes por segundo.
// Quem deve ser chamado X vezes por segundo é a rotina 
// de refresh, que vai efetuar refresh dos retângulos sujos e
// dependendo da ação do compositor, o refresh pode ser da tela toda.
// Refresh
// Lookup the main window list.
// #todo: This is very slow. We need a linked list.
// Get next
// It is a valid window and
// it is a dirty window.
// Flush the window's rectangle and invalidate the window.
// see: rect.c
// + We need to encapsulate the variables used by this routine
//   to prevent about concorrent access problems.
// #bugbug
// This is not a effitient way of doing this.
// We got to refresh folowind the bottom top order.
// + If we refreshed the background window, so,we dont
//   need to refresh any other window when we're not using 
//   individual buffer for the windows in the compositor.

void reactRefreshDirtyWindows(void)
{
// Called by wmReactToPaintEvents();
    register int i=0;

// The component.
// It's a window, but we don't care about its type.
// All we need to do is refreshing the window's rectangle.

    struct gws_window_d *w;

    int fOnlyValidate = FALSE;

// Is the root window a valid window

// Get the window pointer, refresh the windows retangle via KGWS and 
// validate the window.
    for (i=0; i<WINDOW_COUNT_MAX; ++i)
    {
        w = (struct gws_window_d *) windowList[i];
        if ((void*) w != NULL)
        {
            if (w->used == TRUE && w->magic == 1234)
            {
                if (w->dirty == TRUE)
                {
                    // If the root window was refreshed,
                    // there is no need to refresh any other window,
                    // so, lets simply validate them.

                    if (fOnlyValidate != TRUE)
                    {
                        gws_refresh_rectangle ( 
                            w->absolute_x,  w->absolute_y, 
                            w->width, w->height );
                    }

                    // Validate the window we refreshed.
                    validate_window(w);

                    // The window was the root.
                    // There is no need to refresh anyother
                    // #bugbug: But they are still marked as dirty.
                    // For now, we're gonna refresh them in the next round,
                    // but we can simple validate all the rest.
                    // Continue the loop,
                    // but now we will only validate the windows, not refresh.
                    if (w == __root_window){
                        fOnlyValidate = TRUE;
                    }
                }
            }
        }
    };
}

// wmReactToPaintEvents:
// Called by comp_display_desktop_components();
// Refresh only the components that was changed by the painter.
// #todo
// Maybe in the future we can react to 
// changes in other components than windows.
void wmReactToPaintEvents(void)
{
    // Refresh only the components that was changed by the painter.
    // It means that we're reacting to all the paint events.
    reactRefreshDirtyWindows();

    // ...
}

// Flush
// Display the desktop components without using the compositor.
// Called by ServerLoop() in main.c.
// Called by wmCompose() and callback_compose().
void comp_display_desktop_components(void)
{
    static int Dirty = FALSE;

// fps++
    if (WindowManager.initialized == TRUE){
        WindowManager.frame_counter++;
    }

// Backgroud
// If the background is marked as dirty, 
// we flush it, validate it, show the cursor and return.
    Dirty = (int) isdirty();
    if (Dirty == TRUE)
    {
        gws_show_backbuffer();
        // #bugbug
        // #todo: 
        // We miss the cursor here before refreshing the whole screen.
        if (gUseMouse == TRUE)
        {
            if (gDisplayMousePointer == TRUE)
                __display_mouse_cursor();
        }
        validate();
        return;
    }

// Refresh only the components that was changed by the painter
    wmReactToPaintEvents();

    if (CONFIG_TEST_SPARE_BUFFER == 1)
    {
        //---------
        // #test
        // Copy bytes from the spare buffer to the 
        // top left corner of the screen.
        //comp_test_spare_buffer();
        // ok
        //comp_blit_spare_to_backbuffer(100, 100,10,10);
        //gws_refresh_rectangle ( 100, 100, 10, 10 );
        //---------

        /*
        //---------
        // #test >>> backbuffer
        comp_blit_canvas_to_canvas(
            CANVAS_SPAREBUFFER,   // source
            CANVAS_BACKBUFFER,    // destination
            200, 200,             // destination position
            10, 10 );                // width, height
        gws_refresh_rectangle(200, 200, 10, 10);
        //---------
        */

        /*
        //---------
        // #test >>> frontbuffer
        comp_blit_canvas_to_canvas(
            CANVAS_SPAREBUFFER,   // source
            CANVAS_FRONTBUFFER,    // destination
            300, 100,             // destination position
            10, 10 );                // width, height
        //---------
        */

        //---------
        // #test >>> frontbuffer
        if ((void*) spare_dccanvas != NULL)
        {
            if (spare_dccanvas->magic == 1234)
            {
                comp_blit_canvas_to_canvas(
                    CANVAS_SPAREBUFFER,    // source
                    CANVAS_FRONTBUFFER,    // destination
                    20, 20,                // destination position
                    spare_dccanvas->device_width >> 1,  // width 
                    spare_dccanvas->device_height >> 1  // height
                );  
            }
        }
        //---------
    }

// Show the mouse cursor in the screen.
    if (gUseMouse == TRUE)
    {
        if (gDisplayMousePointer == TRUE)
            __display_mouse_cursor();
    }

// Validate the whole screen.
    validate();

// fps
    //__update_fps();
}

// The worker that compose the desktop scene
void compComposeDesktop(void)
{
    register int i=0;

// The component.
// It's a window, but we don't care about its type.
// All we need to do is refreshing the window's rectangle.

    //struct gws_window_d *w;
    //int fOnlyValidate = FALSE;

// fps++
    if (WindowManager.initialized == TRUE){
        WindowManager.frame_counter++;
    }

    if (CONFIG_USE_REAL_COMPOSITOR != 1)
        return;

// #todo: 
// Root window also needs its own canvas.
    //if (gCompositorUpdateDesktop == TRUE)
    //redraw_window(__root_window, FALSE);
    //refresh_window(__root_window);


// Walk the list of canvas
    struct canvas_information_d *ci;
    struct canvas_information_d *ci_src;
    struct canvas_information_d *ci_dst;
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    ci = (struct canvas_information_d *) canvas_head;


/*
    ci_src = ci;
    ci_dst = canvas_backbuffer;
    // Values for destination (backbuffer)
    left = 0;
    top = 0;
    width  = 40;
    height = 40;

    // Copy the canvas into the baclbuffer
    comp_blit_canvas_to_canvas_imp (
        ci_src,
        ci_dst,
        left,
        top,
        width,
        height
    );
*/

    int Counter=0;
    while (1)
    {
        // End of list
        if ((void*) ci == NULL)
            break;
        
        //if ((void*) ci != NULL)
        //{
            if (ci->initialized == TRUE)
            {
                // If dirty, flush it into the backbuffer.
                if (ci->dirty == TRUE)
                {
                    //printf ("dirty\n");
                    ci_src = ci;
                    ci_dst = canvas_backbuffer;
                    // Values for destination (backbuffer)
                    left = 0;
                    top = Counter * 20;
                    // #todo: Check dc validation
                    width = 0;
                    height = 0;
                    // It has a dc
                    if ((void*) ci->dc != NULL)
                    {
                        width  = ci->dc->device_width;
                        height = ci->dc->device_height;
                    }
                    // Now it belongs to a window, let's respect 
                    // the window dimensions.
                    if ((void*) ci->owner_window != NULL)
                    {
                        left = ci->owner_window->absolute_x;
                        top = ci->owner_window->absolute_y;
    
                        // We gotta clip is into the screen now.

                        // the window is larger than the space we have
                        //if (ci->owner_window->width > 
                        //    (ci->dc->device_width - ci->owner_window->absolute_x) )
                        //{
                            //width = (ci->dc->device_width - ci->owner_window->absolute_x);  
                        //}
                        width = ci->owner_window->width;

                        //if (ci->owner_window->height > 
                        //    (ci->dc->device_height - ci->owner_window->absolute_y) )
                        //{
                        //    height = (ci->dc->device_height - ci->owner_window->absolute_y);  
                        //}
                        // #ps: Here we need a valid dc
                        height = ci->owner_window->height;
                        if (10 < ci->dc->device_height)
                            height = 10;
                        //if (ci->owner_window->height > ci->dc->device_height)
                            //height = ci->dc->device_height;

                        if (ci->owner_window->type == WT_POPUP)
                            height = 2;
                    }

                    // Copy the canvas for the frame into the backbuffer
                    comp_blit_canvas_to_canvas_imp (
                        ci_src,
                        ci_dst,
                        left, top, width, height
                    );

                    // Based on the index into the table.
                    
                    //comp_blit_canvas_to_canvas(
                    //    CANVAS_SPAREBUFFER,    // source
                    //    CANVAS_BACKBUFFER,     // destination
                    //    w->absolute_x, 
                    //    w->absolute_y,
                    //    my_width,  // width 
                    //    my_height  // height
                    //);

                }
            }
        //}

        Counter++;
        
        ci = ci->next;  // Get next in the list
    };


// ===========================================
// Redraw root window, but do not show it yet.
// #todo: We're redrawing for now ... but the plain is copying it from its

/*
    if (CONFIG_TEST_SPARE_BUFFER == 1 || CONFIG_USE_REAL_COMPOSITOR)
    {
        if (need_rootwindow_redraw == TRUE)
        {
            need_rootwindow_redraw = FALSE;
            redraw_window(__root_window,FALSE);
        }
    }
*/


// Is the root window a valid window

    //unsigned long my_width;
    //unsigned long my_height;

/*
// Get the window pointer, refresh the windows retangle via KGWS and 
// validate the window.
    for (i=0; i<WINDOW_COUNT_MAX; ++i)
    {
        w = (struct gws_window_d *) windowList[i];
        if ((void*) w != NULL)
        {
            if (w->used == TRUE && w->magic == 1234)
            {
                if (w->dirty == TRUE)
                {

                    //---------
                    // #test
                    // Considering the sparebuffer only for overlapped
                    if (CONFIG_TEST_SPARE_BUFFER == 1)
                    {
                        // #test >>> frontbuffer
                        if ((void*) spare_dccanvas != NULL)
                        {
                            if (spare_dccanvas->magic == 1234)
                            {
                                // Width (clipping)
                                //my_width = spare_dccanvas->device_width;
                                //if (spare_dccanvas->device_width > w->width)
                                //    my_width = w->width;

                                // Height (clipping)
                                //my_height = spare_dccanvas->device_height;
                                //if (spare_dccanvas->device_height > w->height)
                                    //my_height = w->height;

                                //my_width = w->width >> 1;
                                //my_height = 26;

                                my_width = w->width - (w->width/3);
                                my_height = 26;

                                if (w->type == WT_OVERLAPPED)
                                {
                                    // Fake titlebar
                                    comp_blit_canvas_to_canvas(
                                        CANVAS_SPAREBUFFER,    // source
                                        CANVAS_BACKBUFFER,     // destination
                                        w->absolute_x, 
                                        w->absolute_y,
                                        my_width,  // width 
                                        my_height  // height
                                    );

                                    
                                
                                    // #todo:
                                    // Here we can flush the client area,
                                    // After the moment the client finished 
                                    // the painting
                                    //comp_blit_canvas_to_canvas(
                                    //    CANVAS_BG00,    // source
                                    //    CANVAS_BACKBUFFER,     // destination
                                    //    w->absolute_x, 
                                    //    w->absolute_y,
                                    //    my_width,  // width 
                                    //    my_height  // height
                                    //);                                    

                                }
                            // ...
                            }
                        }
                    }
                    //---------

                    if (w->type == WT_BUTTON)
                    {
                        // Width (clipping)
                        my_width = test00_dccanvas->device_width;
                        if (test00_dccanvas->device_width > w->width)
                            my_width = w->width;

                        // Height (clipping)
                        my_height = test00_dccanvas->device_height;
                        if (test00_dccanvas->device_height > w->height)
                            my_height = w->height;

                        // 6 is the index of our new test canvas.
                        comp_blit_canvas_to_canvas(
                            CANVAS_TEST00,       // source
                            CANVAS_BACKBUFFER,   // destination
                            w->absolute_x +4, 
                            w->absolute_y +4,
                            (my_width -8),  // width 
                            (my_height -8)  // height
                        );
                    }

                    // If the root window was refreshed,
                    // there is no need to refresh any other window,
                    // so, lets simply validate them.

                    
                    //if (fOnlyValidate != TRUE)
                    //{
                    //    gws_refresh_rectangle ( 
                    //        w->absolute_x,  w->absolute_y, 
                    //        w->width, w->height );
                    //}
                    

                    // Validate the window we refreshed.
                    //validate_window(w);

                    // The window was the root.
                    // There is no need to refresh anyother
                    // #bugbug: But they are still marked as dirty.
                    // For now, we're gonna refresh them in the next round,
                    // but we can simple validate all the rest.
                    // Continue the loop,
                    // but now we will only validate the windows, not refresh.
                    
                    //if (w == __root_window){
                    //    fOnlyValidate = TRUE;
                    //}
                    
                }
            }
        }
    };
*/

   // if (CONFIG_TEST_SPARE_BUFFER == 1)
   // {
        //---------
        // #test
        // Copy bytes from the spare buffer to the 
        // top left corner of the screen.
        //comp_test_spare_buffer();
        // ok
        //comp_blit_spare_to_backbuffer(100, 100,10,10);
        //gws_refresh_rectangle ( 100, 100, 10, 10 );
        //---------

        /*
        //---------
        // #test >>> backbuffer
        comp_blit_canvas_to_canvas(
            CANVAS_SPAREBUFFER,   // source
            CANVAS_BACKBUFFER,    // destination
            200, 200,             // destination position
            10, 10 );                // width, height
        gws_refresh_rectangle(200, 200, 10, 10);
        //---------
        */

        /*
        //---------
        // #test >>> frontbuffer
        comp_blit_canvas_to_canvas(
            CANVAS_SPAREBUFFER,   // source
            CANVAS_FRONTBUFFER,    // destination
            300, 100,             // destination position
            10, 10 );                // width, height
        //---------
        */

        /*
        //---------
        // #test >>> frontbuffer
        if ((void*) spare_dccanvas != NULL)
        {
            if (spare_dccanvas->magic == 1234)
            {
                comp_blit_canvas_to_canvas(
                    CANVAS_SPAREBUFFER,    // source
                    CANVAS_FRONTBUFFER,    // destination
                    20, 20,                // destination position
                    spare_dccanvas->device_width >> 1,  // width 
                    spare_dccanvas->device_height >> 1  // height
                );  
            }
        }
        //---------
        */
   // }

// #provisory?
// Flush backbuffer into the front buffer
    //refresh_screen();

/*
// #test (flush something) last thing, like pointer
    comp_blit_canvas_to_canvas(
        CANVAS_TEST00,    // source
        CANVAS_BACKBUFFER,    // destination
        20, 20,                // destination position
        40,  // width 
        40  // height
    );  
*/

// Flush
    comp_blit_canvas_to_canvas(
        CANVAS_BACKBUFFER,    // source
        CANVAS_FRONTBUFFER,    // destination
        0, 0,                // destination position
        800,  // width 
        600  // height
    );                 
}

// wmCompose:
// Called by the main routine for now.
// Its gonna be called by the timer.
void 
wmCompose(
    unsigned long jiffies, 
    unsigned long clocks_per_second )
{

// It's locked. Return.
    if (Compositor._locked == TRUE)
        return;

// Lock.
// This way this routine can't be called recursively,
// or for a callback routine or signal.
    Compositor._locked = TRUE;

    // Compositor
    // Every window was painted into private offscreen buffers.
    if (Compositor.__enable_composition == TRUE){
        // compose();

    // Flush
    // Every window was painted into the backbuffer.
    } else {
        comp_display_desktop_components();
    };

    Compositor.counter++;

// Unlock
    Compositor._locked = FALSE;
}

/*
// Marca como 'dirty' todas as janelas filhas,
// dai o compositor faz o trabalho de exibir na tela.
void refresh_subwidnows( struct gws_window_d *w );
void refresh_subwidnows( struct gws_window_d *w )
{
}
*/

// Initialize the mouse support.
// global
// #temporary:
// Mouse is using the limit os 0~800.
void comp_initialize_mouse(void)
{
    int hotspotx=0;
    int hotspoty=0;
    unsigned long w = gws_get_device_width();
    unsigned long h = gws_get_device_height();

// #test
// Initializing the cursor clipping region with the root window.
    if ((void*) __root_window != NULL){
        cursor_clip = __root_window;
    }

    if (w>=0 && w<=800)
        hotspotx = (w >> 1);
    if (h>=0 && h<=800)
        hotspoty = (h >> 1);

// Save it globally.
    __old_mouse_x = hotspotx;
    __old_mouse_y = hotspoty;
    __new_mouse_x = hotspotx;
    __new_mouse_y = hotspoty;


    __mouse_initialized = TRUE; 

// CONFIG:
    //gUseMouse = TRUE;
    gUseMouse = FALSE;
}

// global
// Set new mouse position.
void comp_set_mouse_position(long x, long y)
{
    unsigned long minw = 0;
    unsigned long maxw = gws_get_device_width();
    unsigned long minh = 0;
    unsigned long maxh = gws_get_device_height();

// Lower limit
    if (x < minw){ x=minw; }
    if (y < minh){ y=minh; }

// Upper limit
// #bugbug: Check if it is '>=' instead.
    if (x > maxw){
        x=maxw;
    }
    if (y > maxh){
        y=maxh;
    }

// Save it globally.
    __new_mouse_x = (long) x;
    __new_mouse_y = (long) y;

    // changed = TRUE;
}

//
// $
// INITIALIZATION
//

// + InitializeCompositor structure.
// + Initialize mouse support.
int compInitializeCompositor(void)
{
    Compositor.initialized = FALSE;

    Compositor.counter = 0;
    Compositor._locked = FALSE;

// >> This flag enables composition for the display server.
// In this case the server will compose a final backbbuffer
// using buffers and the zorder for these buffers. In this case 
// each application window will have it's own buffer.
// >> If this flag is not set, all the windows will be painted in the
// directly in the same backbuffer, and the compositor will just
// copy the backbuffer to the LFB.
    Compositor.__enable_composition = FALSE; 

// The structure is initialized.
    Compositor.used = TRUE;
    Compositor.magic = 1234;
    Compositor.initialized = TRUE;

// Initialize the mouser support.
// Not enabled yet.
    comp_initialize_mouse();

/*
This was the first experiment in order to have a future
 compositor with per-window buffers. I will expand this 
 idea and use the allocator for the next experiments ... 
 starting small and in the future, all the windows will have
  a big side-buffer that has the same size of the screen ...
   for full screen mode. It allows the painter to build 
   small and big windows in the side-buffer
*/

// -------------------------
    struct canvas_information_d *ci_tmp;
    if (CONFIG_TEST_SPARE_BUFFER == 1)
    {
        ci_tmp = (void *) compCreateCanvasUsingSpareBuffer();
        if ((void*) ci_tmp == NULL){
            printf("comp.c: ci_tmp\n");
            exit(1);
        }
    }
// -------------------------

// ---------------------------------
// the dc
    struct dccanvas_d *dc;
    dc = (struct dccanvas_d *) comp_create_dc_and_allocate_buffer(64);  //64KB
    if ((void*)dc == NULL){
        return NULL;
    }
    if (dc->magic != 1234){
        return NULL;
    }

// Associating the new dc with our canvas info structure.
    ci_tmp->dc = (struct dccanvas_d *) dc;

// Save the dc for future usage
    test00_dccanvas = (struct dccanvas_d *) dc;

    //comp_add_to_list(ci_tmp);


// --------------

// Create a new off-screen buffer
// baesd on the dc
    struct canvas_information_d *ci00;
    // Bits per pixel
    //unsigned long DeviceBPP00 = (unsigned long) server_get_system_metrics(9);
    ci00 = (void *) compCreateNewCanvas(dc);
    if ((void*) ci00 == NULL){
        printf("comp.c: ci00\n");
        exit(1);
    }

    // Saving into the list for testing purpose
    canvasList[CANVAS_TEST00] = (unsigned long) ci00;

    //comp_add_to_list(ci00);

// -----------------
// Draw into the new dc
    // B of button
    dc_drawchar( dc, 2, 2, 'B', COLOR_YELLOW, COLOR_RED, ROP_COPY );

    putpixel0(
        dc,
        COLOR_YELLOW, 
        1, 
        1, 
        ROP_COPY );

    putpixel0(
        dc,
        COLOR_YELLOW, 
        10, 
        1, 
        ROP_COPY );

    putpixel0(
        dc,
        COLOR_YELLOW, 
        1, 
        10, 
        ROP_COPY );

    putpixel0(
        dc,
        COLOR_YELLOW, 
        10, 
        10, 
        ROP_COPY );

// ...


// ============================================
// #test
// Dirty creation of canvas for shared bg

    struct dccanvas_d *dc88;
    dc88 = (struct dccanvas_d *) comp_create_dc_and_allocate_buffer(64);  //64KB
    if ((void*) dc88 == NULL)
        return -1;
    bg_dccanvas = dc88;   // Global for painting routines

    struct canvas_information_d *ci88;
    ci88 = (void *) compCreateNewCanvas(dc88);
    if ((void*) ci88 == NULL)
        return -1;

    dc_drawchar( bg_dccanvas, 2, 2, 'z', COLOR_YELLOW, COLOR_RED, ROP_COPY );

    ci88->dirty = TRUE;

    // Saving into the list for testing purpose
    canvasList[CANVAS_BG00] = (unsigned long) ci88;

//
// Head
//

// Initialize our list with one single element
// #todo: Create the function to add elements to the list.
    //ci88->next = NULL;  // finish the list
    //canvas_head = ci88;


    //comp_add_to_list(ci88);

    return 0;
}

