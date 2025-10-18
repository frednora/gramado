// poly00.c
// Polygons
// Created by Fred Nora.

#include "../../ds.h"

// demo: polygon type polyline
// asteroids space ship like.
void demoPolygon(void)
{
    struct gr_polygon_d *p;
    unsigned long vecList[8];
    
    struct gr_vec3D_d *v0;
    struct gr_vec3D_d *v1;
    struct gr_vec3D_d *v2;
    struct gr_vec3D_d *v3;
    struct gr_vec3D_d *v4;

    // ...

    register int i=0;
    int TranslationOffset=0;
    int j=0;

// Structure
    p = (struct gr_polygon_d *) malloc( sizeof( struct gr_polygon_d ) );
    if ((void*)p == NULL){
        return;
    }

// polygon type
    p->type = POLYGON_POLYLINE;

// number of elements
    p->n = 6;

    p->list_address = (void*) vecList;


// clear vecList.
// This is a local list.
    for (i=0; i<8; i++){
        vecList[i] = 0;
    };

// Creating some points.

    v0 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v0==NULL){return;}

    v1 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v1==NULL){return;}

    v2 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v2==NULL){return;}

    v3 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v3==NULL){return;}

    v4 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v4==NULL){return;}

    vecList[0] = (unsigned long) v0;
    vecList[1] = (unsigned long) v1;
    vecList[2] = (unsigned long) v2;
    vecList[3] = (unsigned long) v3;
    vecList[4] = (unsigned long) v4;
    vecList[5] = (unsigned long) v0;  //circular


//
// loop
//

// animation loop.

    int times=0;

    while (1){

    times++;
    if(times>8){break;}
    
    // translation in Y
    TranslationOffset=0;
    
    // animation loop: 
    // clear the screen and draw the the model 10 times.
    
    for (i=0; i<10; i++){

    //rectBackbufferDrawRectangle ( 
        //0, 0, 320, 200, COLOR_BLACK, 1,0 );
    
    demoClearSurface(COLOR_BLACK);
           
    TranslationOffset = (TranslationOffset+i);
    
    v0->x = -(20);
    v0->y =  (20+TranslationOffset);
    v0->z =   0;
    v0->color = COLOR_WHITE;

    // fixed
    v1->x = (0);
    v1->y = (0+TranslationOffset);
    v1->z = 0;
    v1->color = COLOR_WHITE;

    v2->x = (20);
    v2->y = (20+TranslationOffset);
    v2->z =  0;
    v2->color = COLOR_WHITE;

    v3->x =   (0);
    v3->y = -(20-TranslationOffset);  
    v3->z =   0;
    v3->color = COLOR_WHITE;

    v4->x = -(20);
    v4->y =  (20+TranslationOffset);  
    v4->z =  0;
    v4->color = COLOR_WHITE;

    //server_debug_print("calling xxxPolygonZ\n");

    // Draw
    xxxPolygonZ(p);
    // Show
    demoFlushSurface();  
    
    for (j=0; j<8; j++){ rtl_yield(); }

    };
    };   //while

    //server_debug_print("DONE\n");
    //printf ("DONE\n");
}

void demoPolygon2(void)
{
    struct gr_polygon_d *p;
    unsigned long vecList[8];
    
    struct gr_vec3D_d *v0;
    struct gr_vec3D_d *v1;
    struct gr_vec3D_d *v2;
    struct gr_vec3D_d *v3;
    struct gr_vec3D_d *v4;

    // ...

    register int i=0;
    int TranslationOffset=0;
    int j=0;

// Structure
    p = (struct gr_polygon_d *) malloc( sizeof( struct gr_polygon_d ) );
    if ((void*)p == NULL){
        return;
    }

// Polygon type
    p->type = POLYGON_POLYPOINT;
    //p->type = POLYGON_POLYLINE;

    // number of elements
    p->n = 6;

    p->list_address = (void*) vecList;

// clear vecList.
// This is a local list.
    for (i=0; i<8; i++){
        vecList[i] = 0;
    };

// Creating some points.

    v0 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v0==NULL){return;}

    v1 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v1==NULL){return;}

    v2 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v2==NULL){return;}

    v3 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v3==NULL){return;}

    v4 = (struct gr_vec3D_d *) malloc( sizeof( struct gr_vec3D_d ) );
    if((void*)v4==NULL){return;}

    vecList[0] = (unsigned long) v0;
    vecList[1] = (unsigned long) v1;
    vecList[2] = (unsigned long) v2;
    vecList[3] = (unsigned long) v3;
    vecList[4] = (unsigned long) v4;
    vecList[5] = (unsigned long) v0;  //circular


//
// loop
//

// animation loop

    int times=0;

    while (1){

    times++;
    if(times>8){break;}
    
    // translation in Y
    TranslationOffset=0;
    
    // animation loop: 
    // clear the screen and draw the the model 10 times.
    
    for (i=0; i<10; i++){

    // clear surface
    demoClearSurface(COLOR_BLACK);
    
    TranslationOffset = (TranslationOffset+i);
    
    v0->x = -(20);
    v0->y =  (10+TranslationOffset);
    v0->z =   0;
    v0->color = COLOR_WHITE;

    // fixed
    v1->x = (20);
    v1->y = (10+TranslationOffset);
    v1->z =  0;
    v1->color = COLOR_WHITE;

    v2->x =  30;
    v2->y = (0+TranslationOffset);
    v2->z =  0;
    v2->color = COLOR_WHITE;

    v3->x = 0;
    v3->y = -(10-TranslationOffset);  
    v3->z =  0;
    v3->color = COLOR_WHITE;

    v4->x = -30;
    v4->y = (0+TranslationOffset);  
    v4->z = 0;
    v4->color = COLOR_WHITE;

    //server_debug_print("calling xxxPolygonZ\n");

    // Draw

    //p->type = POLYGON_POLYPOINT;
    //xxxPolygonZ(p);
    //gws_refresh_rectangle(0,0,320,200);

    p->type = POLYGON_POLYLINE;
    xxxPolygonZ(p);

    // flush surface.
    demoFlushSurface();  
    
    //delay
    for (j=0; j<20; j++){ rtl_yield();}

    };
    };   //while

    //server_debug_print("DONE\n");
    //printf ("DONE\n");
}

