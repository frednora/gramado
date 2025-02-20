
// sprite.c

#include "gram3d.h"

// Display all the sprites found in a given list.
int gr_display_sprite_list( struct gws_sprite_d *root )
{
    struct gws_sprite_d *tmp;
    int Result=0;

// Invalid dc
// #todo: We can get a dc via parameter.
    if ( (void*) gr_dc == NULL )
        return 0;
    if (gr_dc->initialized != TRUE)
        return 0;

// ------------
    tmp = (struct gws_sprite_d *) root;
    if ( (void*) tmp == NULL ){
        return 0;
    }
    if (tmp->magic!=1234){
        return 0;
    }

    while(1)
    {
        if (tmp->initialized == TRUE){
            gr_display_sprite( gr_dc, tmp, 0, 0, 0 );
        }

        Result++;
        
        tmp = (struct gws_sprite_d *) tmp->next;
        if ( (void*) tmp == NULL ){
            break;
        }
        if ( tmp->magic!=1234){
            break;
        }
    };

// The number of elements in the list.
    return (int) Result;
}


// Get The number of elements in a list of sprites.
int gr_sprite_cnt_list( struct gws_sprite_d *root )
{
    struct gws_sprite_d *tmp;
    int Result=0;

    tmp = (struct gws_sprite_d *) root;
    if ( (void*) tmp == NULL ){
        return 0;
    }
    if (tmp->magic!=1234){
        return 0;
    }

    while(1)
    {
        Result++;
        
        tmp = (struct gws_sprite_d *) tmp->next;
        if ( (void*) tmp == NULL ){
            break;
        }
        if (tmp->magic!=1234){
            break;
        }
    };

// The number of elements in the list.
    return (int) Result;
}



/*
// #todo
// create a sprite of given types and
// return the structure.
// The caller will put that pointer in a list
// to call the loop function PlotSprite3D().
struct gws_sprite_d *create_sprite_point()
struct gws_sprite_d *create_sprite_line()
struct gws_sprite_d *create_sprite_circle()
*/


/*
//#todo
struct gws_sprite_d *create_sprite_point (int z, int x, int y, unsigned long color);
struct gws_sprite_d *create_sprite_point (int z, int x, int y, unsigned long color)
{
    struct gws_sprite_d *sprite;

    sprite = (struct gws_sprite_d *) malloc( sizeof(struct gws_sprite_d) );
    
    // todo check.
    
    sprite->x1 = x;
    sprite->y1 = y;
    sprite->z1 = z;
    sprite->color1 = color;

    return (struct gws_sprite_d *) sprite;
}
*/


void sprite_test1(void)
{
// polyline

    struct gws_sprite_d *sprite;
    int buf32[32]; // elements of the lines.

// 3 lines
//  '/' 
//  '-'
//  '|'

    buf32[0] = (int) 0;
    buf32[1] = (int) 0; 
    buf32[2] = (int) 8;
    buf32[3] = (int) 8;   //1
    buf32[4] = (int) -8;
    buf32[5] = (int)  8;  //2
    buf32[6] = (int) -8;
    buf32[7] = (int) -8;  //3

// sprite
    sprite = (struct gws_sprite_d *) malloc (sizeof(struct gws_sprite_d));
    if((void*)sprite==NULL)
        return;
    sprite->used = TRUE;
    sprite->magic = 1234;
    sprite->initialized = FALSE;

    sprite->type = SPRITE_POLYLINE;

// Poiter for the data base
    sprite->base = (void*) &buf32[0];    

// max number of iterations.
// as linhas estão conectadas,
// o segundo ponto de uma linha é
// o primeiro ponto da proxima.
// 'i' starts in 1.
    sprite->num = 4;   
    sprite->color1 = COLOR_WHITE;

    sprite->initialized = TRUE;
    
    //display polyline.
    gr_display_sprite( 
        gr_dc,
        sprite,
        0,0,0);

    refresh_screen();
}

// display a sprite given the dc.
int 
gr_display_sprite( 
    struct dc_d *dc, 
    struct gws_sprite_d *sprite, 
    int x, int y, int z )
{
    int i=0;
// Pointer to an array of values.
// given by the sprite.
    int *ptr32;

    int x1=0;  int y1=0;  int z1=0;
    int x2=0;  int y2=0;  int z2=0;
    int x3=0;  int y3=0;  int z3=0;
    
// dc

    if( (void*) dc == NULL ){
        return -1;
    }
    if(dc->magic != 1234){
        return -1;
    }
    if(dc->initialized != TRUE){
        return -1;
    }

// sprite

    if( (void*) sprite == NULL ){
        return -1;
    }
    if(sprite->magic !=1234){
        return -1;
    }
    if(sprite->initialized != TRUE){
        return -1;
    }

// draw

    switch (sprite->type)
    {
        case SPRITE_NULL:
            break;
        case SPRITE_POINT:
            grPlot0 (
                NULL, 
                (int) (z + sprite->z1), 
                (int) (x + sprite->x1), 
                (int) (y + sprite->y1), 
                (unsigned int) sprite->color1,
                0 );
            break;
        case SPRITE_LINE:
            plotLine3d ( 
               NULL,
               (int) (x + sprite->x1), 
               (int) (y + sprite->y1), 
               (int) (z + sprite->z1), 
               (int) (x + sprite->x2), 
               (int) (y + sprite->y2), 
               (int) (z + sprite->z2), 
               (unsigned int) sprite->color1 );
            break;
        
        // as linhas estão conectadas,
        // o segundo ponto de uma linha é
        // o primeiro ponto da proxima.
        // ok: it is working
        case SPRITE_POLYLINE:
            ptr32 = (int*) sprite->base;
            x1 = ptr32[0];
            y1 = ptr32[1];
            for (i=1; i < sprite->num ; i++)
            {
                x2 = (int) ptr32[ (2*i) ];
                y2 = (int) ptr32[ (2*i) +1 ];
                
                //printf ("%d %d | %d %d\n", x1,y1,x2,y2);
                plotLine3d(
                    NULL,
                    x1, y1, 0,
                    x2, y2, 0,
                    (unsigned int) sprite->color1 );
            
                x1 = (int) x2;
                y1 = (int) y2;
            };
            break;
        
        case SPRITE_TRIANGLE:
            // #todo: use a triangle structure.
            //xxxTriangleZ(NULL,triangle);
            break;
        case SPRITE_RECTANGLE:
            //gws_refresh_rectangle(
                //gr_dc->left, 
                //gr_dc->top, 
                //gr_dc->width, 
                //gr_dc->height );
            break;
        case SPRITE_CIRCLE:
            grCircle3 ( 
                NULL,
                (int) (x + sprite->x1),           // xm 
                (int) (y + sprite->y1),           // ym 
                (int) sprite->r,                  // r 
                (unsigned int) sprite->color1,   // color 
                (int) (z + sprite->z1) );         // z 
            break;
        case SPRITE_ELLIPSE:
            //grEllipse ( i, i, i*5, i*5, COLOR_BLUE);
            //grEllipse3 ( i%20, i%20, i, i, COLOR_BLUE,i%20);
            break;
        //bitmap file?
        case SPRITE_BITMAP:
        //bmpDisplayBMP( 
        //    (char *) sm_buffer, 
        //    (unsigned long) bmp_x, 
        //    (unsigned long) bmp_y ); 
            break;
        case SPRITE_POLYGON:
            // #todo: use the polygon structure.
            // xxxPolygonZ(p);
            break;
        case SPRITE_MESH:
            break;

        default:
            return -1;
            break;
    };

    return 0;
}


/*
 * PlotSprite3D:
 *     display     - display structure.
 *     x,y,z       - position.
 *     sprite_list - linked list of sprite structures.
 *     list_size   - how many elements in the list.
 */
// Display a list of sprites (elments)
// given a display structure.

int 
PlotSprite3D ( 
    struct gws_display_d *display,
    int x, int y, int z,
    unsigned long sprite_list_address,
    int sprite_list_size,
    int just_one )
{
    unsigned long *list = (unsigned long *) sprite_list_address;

    struct gws_sprite_d *sprite;
    int Type = SPRITE_NULL;

    int i=0;
    int number_of_elements = sprite_list_size;
    int Max = SPRITE_MAX_NUMBER;


    //
    // ===== #todo ===============================
    //


    gwssrv_debug_print("PlotSprite3D: [TODO] \n");
 
// invalid address
    if ( sprite_list_address == 0 )
    {
        gwssrv_debug_print("PlotSprite3D: [FAIL] sprite_list_address\n");
        return -1;
    }

    // no elements
    if ( number_of_elements <= 0 )
    {
        gwssrv_debug_print("PlotSprite3D: [FAIL] no elements\n");
        return -1;
    }

    // too much elements
    if ( number_of_elements >= SPRITE_MAX_NUMBER )
    {
        gwssrv_debug_print("PlotSprite3D: [FAIL] number_of_elements\n");
        return -1;
    }

    // Loop: 
    // todos os elementos da lista.
    
    for(i=0; i<number_of_elements; i++){


    // Get sprite structure.

    sprite = (struct gws_sprite_d *) list[i];

    // last one.
    if ( (void*) sprite == NULL ){
        gwssrv_debug_print("PlotSprite3D: [ERROR] sprite \n");
        break;
    }

    if ( sprite->used != TRUE || 
         sprite->magic != 1234 )
    {
        gwssrv_debug_print("PlotSprite3D: sprite validation\n");
        break;
    }

    if ( sprite->initialized != TRUE ){
        gwssrv_debug_print("PlotSprite3D: sprite not initialized\n");
        break;
    }

    // type
    Type = sprite->type;
    switch (Type){

        case SPRITE_NULL:
            break;
        
        case SPRITE_POINT:
        case SPRITE_LINE:
        case SPRITE_POLYLINE:
        case SPRITE_CIRCLE:
            // IN: default dc, sprite, x, y, z
            gr_display_sprite( gr_dc, sprite, x, y, z);
            break;

        // ...
        
        default:
            //gwssrv_debug_print("PlotSprite3D: [default] \n");
            break;

    };
    
        if (just_one == TRUE){ break; }
    };

// #todo
// Restaura para o padrao os elemento da estrutura display.
// device context

    return 0;
}

