
// menu.h


#ifndef GWS_MENU_H
#define GWS_MENU_H    1

// Menu item types
#define MI_TYPE_SEPARATOR  1000
#define MI_TYPE_TEXT       2000


struct gwssrv_menu_item_d
{
    struct gws_window_d *window;

    int id;
    int selected;
    
    unsigned long x;
    unsigned long y;
    unsigned long width;
    unsigned long height;
    unsigned int color;

    struct gwssrv_menu_item_d *next;
};


// menu
struct gwssrv_menu_d
{
    struct gws_window_d  *window;
    struct gws_window_d  *parent;

// TRUE = the menu procedure is running at the moment.
    int in_use;

    unsigned long x;
    unsigned long y;
    unsigned long width;
    unsigned long height;

    unsigned int color;
    
    int highlight; //item em destaque
    
    //list
    int itens_count;
    struct gwssrv_menu_item_d *list;

    //struct gwssrv_menu_d *next;
};


//
// == Prototypes ==============================================
//

// checa se o mouse esta passando sobre o menu.
int __is_inside_menu(struct gwssrv_menu_d *menu, int x, int y);

int gwssrv_get_number_of_itens (struct gwssrv_menu_d *menu);

struct gwssrv_menu_d *gwssrv_create_menu (
    struct gws_window_d *parent,
    int highlight,
    int count,
    unsigned long x,
    unsigned long y,
    unsigned long width,
    unsigned long height,
    unsigned int color );

struct gwssrv_menu_item_d *gwssrv_create_menu_item (
    char *label,
    int id,
    struct gwssrv_menu_d *menu);

int create_main_menu(int position_x, int position_y);

#endif    


