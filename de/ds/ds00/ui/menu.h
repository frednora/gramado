
// menu.h

#ifndef GWS_MENU_H
#define GWS_MENU_H    1


#define MENU_EVENT_PRESSED      1000
#define MENU_EVENT_RELEASED     2000
#define MENU_EVENT_COMBINATION  3000

// Menu item types
#define MI_TYPE_SEPARATOR  1000
#define MI_TYPE_TEXT       2000


// --------------------
// Menu item
struct gws_menu_item_d
{
    int id;
    int selected;
    unsigned int color;

    // Relative
    unsigned long x;
    unsigned long y;
    unsigned long width;
    unsigned long height;

    struct gws_window_d *bg_window;

    struct gws_menu_item_d *next;
};

// --------------------
// Menu
struct gws_menu_d
{
// TRUE = the menu procedure is running at the moment.
    int in_use;

    unsigned int color;

// Número do item em destaque.
    int highlight;  
    // Relative
    unsigned long x;
    unsigned long y;
    unsigned long width;
    unsigned long height;
// Background window.
    struct gws_window_d *bg_window;
// The window that this menu belongs to.
    struct gws_window_d *owner_window;
// List of items.
    int itens_count;
    struct gws_menu_item_d *list;
};

//
// == Prototypes ==============================================
//

// checa se o mouse esta passando sobre o menu.
//int is_inside_menu(struct gws_menu_d *menu, int x, int y);

//int gwssrv_get_number_of_itens (struct gws_menu_d *menu);

int redraw_main_menu(void);

int main_menu_all_windows_input_status(int input_status);

//void enable_main_menu(void);
//void disable_main_menu(void);

int create_main_menu(void);

int on_mi_clicked(unsigned long item_number);

#endif    


