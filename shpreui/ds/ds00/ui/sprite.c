// sprite.c
// Sprite support.
// Created by Fred Nora.

#include "../gwsint.h"

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

/*
 * PlotSprite3D:
 *     display     - display structure.
 *     x,y,z       - position.
 *     sprite_list - linked list of sprite structures.
 *     list_size   - how many elements in the list.
 */
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

    //server_debug_print("PlotSprite3D: [TODO] \n");
 
    // Invalid address
    if (sprite_list_address == 0){
        //server_debug_print("PlotSprite3D: [FAIL] sprite_list_address\n");
        goto fail;
    }

    // No elements
    if (number_of_elements <= 0){
        //server_debug_print("PlotSprite3D: [FAIL] no elements\n");
        goto fail;
    }

    // Too much elements
    if (number_of_elements >= SPRITE_MAX_NUMBER){
        //server_debug_print("PlotSprite3D: [FAIL] number_of_elements\n");
        goto fail;
    }

    // Loop: 
    // todos os elementos da lista.
    
    for(i=0; i<number_of_elements; i++){


    // Get sprite structure.

    sprite = (struct gws_sprite_d *) list[i];

    // last one.
    if ( (void*) sprite == NULL ){
        //server_debug_print("PlotSprite3D: [ERROR] sprite \n");
        break;
    }
    if ( sprite->used != 1 || sprite->magic != 1234 ){
        //server_debug_print("PlotSprite3D: [ERROR] sprite validation\n");
        break;
    }

    // type
    Type = sprite->type;
    switch (Type){

        // #todo
        // Insert here the primitives.
        
        case SPRITE_NULL:
            //server_debug_print("PlotSprite3D: [SPRITE_NULL] \n");
            break;
        
        case SPRITE_POINT:
            //server_debug_print("PlotSprite3D: [SPRITE_POINT] \n");
            grPlot0 (
                NULL, 
                (int) (z + sprite->z1), 
                (int) (x + sprite->x1), 
                (int) (y + sprite->y1), 
                (unsigned int) sprite->color1,
                0 );
            break;

        case SPRITE_LINE:
            //server_debug_print("PlotSprite3D: [SPRITE_LINE] \n");
            plotLine3d ( 
               (int) (x + sprite->x1), 
               (int) (y + sprite->y1), 
               (int) (z + sprite->z1), 
               (int) (x + sprite->x2), 
               (int) (y + sprite->y2), 
               (int) (z + sprite->z2), 
               (unsigned int) sprite->color1 );
            break;

        case SPRITE_CIRCLE:
            //server_debug_print("PlotSprite3D: [SPRITE_CIRCLE] \n");
            plotCircleZ ( 
                (int) (x + sprite->x1),           // xm 
                (int) (y + sprite->y1),           // ym 
                (int) sprite->r,                  // r 
                (unsigned int) sprite->color1,   // color 
                (int) (z + sprite->z1) );         // z 
            break;
            
        // ...
        
        default:
            //server_debug_print("PlotSprite3D: [default] \n");
            break;

    };
        if (just_one == TRUE){ 
            break;
        }
    };

// #todo
// Restaura para o padrao os elemento da estrutura display.
// device context
    
    //server_debug_print("PlotSprite3D: done\n");
    return 0;
fail:
    return -1;
}

