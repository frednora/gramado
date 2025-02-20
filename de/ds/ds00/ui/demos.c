// demos.c
// A place for demos.
// #remember:
// This is the display server.
// This is not the right place for this thing.
// But maybe it will be useful for effects.
// Created by Fred Nora.

#include "../gwsint.h"

int gUseDemos = TRUE;

// See: demos.h
struct cat_model_d CatModel;

static void __draw_cat(int eye_scale, int cat_z);
static void __draw_demo_curve1(int position, int model_z);

//======================

void demoLines(void)
{
// Draw lines.

    register int i=0;

    for (i=0; i<320; i+=5)
    {
        plotLine3d ( 
                0, i, 0,
            320-i, 0, 0, 
            COLOR_WHITE); 

        plotLine3d ( 
                   0, i, 0,
            -(320-i), 0, 0, 
            COLOR_WHITE); 

        plotLine3d ( 
            -i,        0, 0,
             0, -(320-i), 0, 
            COLOR_WHITE); 

        plotLine3d ( 
           i,        0, 0,
           0, -(320-i), 0, 
            COLOR_WHITE);
    };

    refresh_screen();
}

void setupCatModel(int eyes, int whiskers, int mouth )
{
    CatModel.eyesVisible     = eyes;
    CatModel.whiskersVisible = whiskers;
    CatModel.mouthVisible    = mouth;
}

//worker
static void __draw_cat(int eye_scale, int cat_z)
{
    int model_z = (int) cat_z;

//---
    // head
    // IN: x, y, r, color, z
    plotCircleZ ( 
        0,    //x
        12,   //y
        25,   //r
        GRCOLOR_LIGHTBLACK,  //color 
        model_z );   // z 

    // eyes
    if ( CatModel.eyesVisible == TRUE )
    {
        plotCircleZ ( 
            -10, 
            20, 
            1+eye_scale, 
            GRCOLOR_LIGHTBLACK, 
            model_z );  //z 
        plotCircleZ ( 
            10, 
            20, 
            1+eye_scale, 
            GRCOLOR_LIGHTBLACK, 
            model_z );  //z 
    }

    // whiskers
    if ( CatModel.whiskersVisible == TRUE ){
        // =
        plotLine3d ( 
            -40, 8, model_z, 
            -4,  5, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            -40, 5, model_z, 
            -4,  4, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            -40, 2, model_z, 
            -4,  3, model_z, 
            GRCOLOR_LIGHTBLACK); 

        // =
        plotLine3d ( 
            4,  5, model_z, 
            40, 8, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            4,  4, model_z, 
            40, 5, model_z, 
            GRCOLOR_LIGHTBLACK); 
        plotLine3d ( 
            4,  3, model_z, 
            40, 2, model_z, 
            GRCOLOR_LIGHTBLACK); 
    }

    // mouth
    if ( CatModel.mouthVisible == TRUE ){
        plotLine3d ( 
            -10, -2, model_z, 
             10, -2, model_z, 
             GRCOLOR_LIGHTBLACK); 
    }
//---
}


// #todo
// We can use this for 'screen saver'
// and break the loop whe the user hit a key
// or move the mouse.
void demoCat(void)
{
    register int i=0;
    int j=0;
    //int count = 8;
    static int count=4;
    int scale_max = 100;

// Changing the vire for the current projection.
    gr_depth_range(
        CurrentProjection,   // projection
        0,                   // zNear
        80 );                // zFar

// Setup model
// eyes, whiskers, mouth
    setupCatModel(TRUE,TRUE,TRUE);

// Loop
    while (count>0)
    {
        for (i=0; i<scale_max; i++)
        {
            validate_background();                 //begin paint
            demoClearSurface(GRCOLOR_LIGHTCYAN);   // Clear surface
            // IN: scale, z
            //__draw_cat(i,i);                         // IN: eye scale
            __draw_cat(1,i);
            //invalidate_background();               // end paint
            demoFlushSurface();                  // Flush surface

            // good for qemu,
            //for (j=0; j<8; j++){ rtl_yield();}  // Delay
            // good for kvm,
            for (j=0; j<80; j++)
            {
                rtl_yield();  //#todo: Do not use this delay for now.
            }  
        };
        
        count--;
    };
}


void demoLine1(void)
{
// Draw lines.

    //int width = getWidth();
    //int height = getHeight();
    int width  = 200/2;  //320/2;
    int height = 200/2;
    int x1 = 0, y1 = 0,
        x2 = 0, y2 = height;

// Valid os mode
    if (os_mode != GRAMADO_JAIL){
        return;
    }

    while (y1 < height) {
        //g.drawLine(x1, y1, x2, y2);
        plotLine3d ( x1,y1,0, x2,y2,0, COLOR_WHITE); 
            y1+=8;                 //You should modify this if
            x2+=8;                 //it's not an equal square (like 250x250)
    };
    demoFlushSurface();  
}

void demoTriangle(void)
{
    struct gr_triangle_d *triangle;
    register int i=0;
    int T=0;

    triangle = (void *) malloc( sizeof( struct gr_triangle_d ) );
    if ((void*) triangle == NULL ){
        return;
    }

// down
    triangle->p[0].x = 0; 
    triangle->p[0].y = 0;
    triangle->p[0].z = 0;
    triangle->p[0].color = COLOR_RED;
// right
    triangle->p[1].x = 80; 
    triangle->p[1].y = 80;
    triangle->p[1].z =  0;
    triangle->p[1].color = COLOR_GREEN;
// left
    triangle->p[2].x = -80;
    triangle->p[2].y =  80;
    triangle->p[2].z =   0;
    triangle->p[2].color = COLOR_BLUE;

    for (i=0; i<10; i++)
    {
        // Clear
        demoClearSurface(COLOR_BLACK);
        // Draw
        xxxTriangleZ(triangle);
        // Translation
        triangle->p[0].x++;
        triangle->p[1].x++;
        triangle->p[2].x++;
        // Flush surface
        demoFlushSurface();  
        T++;
    };
}

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

// IN: ?
static void __draw_demo_curve1(int position, int model_z)
{
    // ??
    int yOffset = (position + position);
    int modelz = (int) model_z;
    //int modelz = 0;

    // line
    //a variaçao de y2 me pareceu certa.
    //IN: ??
    plotQuadBezierSeg ( 
        0,   0,  modelz,          //x0, y0, z0, //ponto inicial
        40,  40, modelz,          //x1, y1, z1, //?
       100,  20+yOffset, modelz,  //x2, y2, z2, //ponto final
       GRCOLOR_LIGHTBLACK );

    //string! char by char
    //IN: x,y,color,c,z
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*0), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'G', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*1), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'R', modelz );   
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*2), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'A', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*3), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'M', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*4), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'A', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*5), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'D', modelz );
    plotCharBackbufferDrawcharTransparentZ ( 
        40+ (8*6), 
        20+yOffset, 
        GRCOLOR_LIGHTRED, 'O', modelz );
}

// curva e string.
void demoCurve(void)
{
    register int i=0;
    register int j=0;
    int count=8;

    gr_depth_range(
        CurrentProjection,
        0,      //near
        100 );  //far

    while (count>0){

        count--;

    for (i=0; i<10; i++)
    {
        validate_background();
        demoClearSurface(GRCOLOR_LIGHTYELLOW);

        // IN: position, modelz
        __draw_demo_curve1(i,0);
        //__draw_demo_curve1(i,i*4);

        //invalidate_background();
        demoFlushSurface();          // flush surface
        
        // delay  
        for (j=0; j<8; j++){ rtl_yield();}
    };
    }
}


// Rotina usada para rodar rotinas demo na inicializaçao.
// Seleciona a rotina demo a ser executada.
void demos_startup_animation(int i)
{
    switch (i){
    case 1: 
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7: demoCurve();  break;
    case 8:
        break;
    case 9: demoCat();  break;
    case 10: demoTriangle();  break;
    case 11:
        break;
    case 12: demoLine1();  break;
    case 13:  
        break;
    case 14: demoPolygon();  break; 
    case 15: demoPolygon2();  break;
    default:
        break;
    };
}

