// world.c


#include "../gram3d.h"


struct gr_world_d  *CurrentWorld;
struct gr_word3d_d  *current_world_3d;

static int __world3d_initialize(void);

// =====================================================

static int __world3d_initialize(void) 
{
    struct gr_word3d_d *w;

    w = (void *) malloc( sizeof(struct gr_word3d_d) );
    if (w == NULL) {
        printf("world3d_initialize: fail\n");
        goto fail;
    }

    w->x_size = (float) 200.0f;
    w->y_size = (float) 200.0f;
    w->z_size = (float) 200.0f;

    w->used = TRUE;
    w->magic = 1234;
    w->initialized = TRUE;

    current_world_3d = w;
    return 0;
fail:
    return (int) -1;
}


int world_initialize(void)
{
// Not using float.

    CurrentWorld = (void *) malloc( sizeof( struct gr_world_d ) );
    if ( (void*) CurrentWorld == NULL ){
        printf("world_initialize: fail\n");
        exit(1);
    }

    CurrentWorld->center.x = (unsigned int) 0;
    CurrentWorld->center.y = (unsigned int) 0;
    CurrentWorld->center.z = (unsigned int) 0;
    CurrentWorld->center.color = COLOR_BLACK;

    CurrentWorld->x_size = (unsigned int) 200;
    CurrentWorld->y_size = (unsigned int) 200;
    CurrentWorld->z_size = (unsigned int) 200;

    CurrentWorld->used = TRUE;
    CurrentWorld->magic = 1234;
    CurrentWorld->initialized = TRUE;


//
// 3D world using floating point.
//
    int status = -1;
    status = __world3d_initialize();
    if (status<0)
    {
        printf("on __world3d_initialize\n");
        exit(0);
    }
    return 0;
}

int unveil_world(void)
{
// Not using float.

// object window
    struct gws_window_d *ow;
    ow = NULL;
// #todo
// Use the demo window if it is possible.
    if ( (void*) __demo_window != NULL ){
        if (__demo_window->magic==1234)
            ow = __demo_window;
    }

    if ( (void*) CurrentWorld == NULL )
        return -1;
    if (CurrentWorld->magic != 1234)
        return -1;
    if (CurrentWorld->initialized!=TRUE)
        return -1;

// --------------------------------
// The horizon.
// The camera is a vanishing point in this line.
    CurrentWorld->h1.x = CurrentWorld->center.x;
    CurrentWorld->h1.y = CurrentWorld->center.y + CurrentWorld->z_size;
    CurrentWorld->h1.z = CurrentWorld->center.z;
    CurrentWorld->h2.x = CurrentWorld->center.x + (CurrentWorld->x_size << 1);
    CurrentWorld->h2.y = CurrentWorld->center.y + CurrentWorld->z_size;
    CurrentWorld->h2.z = CurrentWorld->center.z;
    plotLine3d ( 
        ow,
        CurrentWorld->h1.x,  CurrentWorld->h1.y, CurrentWorld->h1.z, 
        CurrentWorld->h2.x,  CurrentWorld->h2.y, CurrentWorld->h2.z, 
        COLOR_PURPLE ); 
// --------------------------------
// vanishing point
    CurrentWorld->vp1.x = (CurrentWorld->h1.x + CurrentWorld->h2.x)/2;
    CurrentWorld->vp1.y = (CurrentWorld->h1.y + CurrentWorld->h2.y)/2;
    CurrentWorld->vp1.z = CurrentWorld->h1.z;
    grCircle3 ( 
        ow,
        CurrentWorld->vp1.x,    //x
        CurrentWorld->vp1.y,    //y
        4,        //r
        COLOR_PURPLE,  //color 
        CurrentWorld->vp1.z );   // z 

// ==========================
// Draw the 3 axis
// Draw line
    //int off=100;
// y
    plotLine3d ( 
        ow,
        CurrentWorld->center.x,  
        CurrentWorld->center.y - CurrentWorld->y_size,
        CurrentWorld->center.z, 
        CurrentWorld->center.x,
        CurrentWorld->center.y + CurrentWorld->y_size, 
        CurrentWorld->center.z,
        COLOR_RED ); 
// x
    plotLine3d ( 
        ow,
        CurrentWorld->center.x - CurrentWorld->x_size,
        CurrentWorld->center.y,
        CurrentWorld->center.z, 
        CurrentWorld->center.x + CurrentWorld->x_size,  
        CurrentWorld->center.y,  
        CurrentWorld->center.z,
        COLOR_GREEN ); 
// z
    plotLine3d ( 
        ow,
        CurrentWorld->center.x + CurrentWorld->z_size,
        CurrentWorld->center.y + CurrentWorld->z_size,
        CurrentWorld->center.z, 
        CurrentWorld->center.x - CurrentWorld->z_size,  
        CurrentWorld->center.y - CurrentWorld->z_size,
        CurrentWorld->center.z, 
        COLOR_BLUE ); 

    return 0;
}

