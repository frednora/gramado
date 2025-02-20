
// menu.c
// Menu support.
// Created by Fred Nora.

#include "../gwsint.h"

//struct gws_menu_d *main_menu;

/*
static const char *mn_app0 = "terminal.bin";
static const char *mn_app1 = "editor.bin";
static const char *mn_app2 = "reboot.bin";
static const char *mn_app3 = "shutdown.bin";
*/

//int mainmenu_buttons_count=0;
//int mainmenu_buttons[MAINMENU_BUTTONS_MAX];

// ----------------

// #deprecated
int redraw_main_menu(void)
{
    return FALSE;
}

// #deprecated
int main_menu_all_windows_input_status(int input_status)
{
    return FALSE;
}

// #test
// Window server's widget.
// ::: The context menu for the root window.
int create_main_menu(void)
{
    return -1;
}

/*
// #deprecated.
// Checa se o mouse esta passando sobre o main menu.
int is_inside_menu(struct gws_menu_d *menu, int x, int y)
{
    return FALSE;
}
*/

/*
int gwssrv_get_number_of_itens(struct gws_menu_d *menu)
{
    if ((void*) menu == NULL){
        return -1;
    }
    return (int) menu->itens_count;
}
*/

/*
struct gws_menu_item_d *gwssrv_get_menu_item(struct gws_menu_d *menu, int i);
struct gws_menu_item_d *gwssrv_get_menu_item(struct gws_menu_d *menu, int i)
{
     //return (struct gws_menu_item_d *) ?;
}
*/

/*
int gwssrv_redraw_menuitem(struct gws_menu_item_d *);
int gwssrv_redraw_menuitem(struct gws_menu_item_d *)
{
}
*/

/*
int gwssrv_redraw_menu ( struct gws_menu_d *menu );
int gwssrv_redraw_menu ( struct gws_menu_d *menu )
{
    int i=0;
    int n=0;

    if ( (void*) menu == NULL )
        return -1;
    
    n = (int) gwssrv_get_number_of_itens(menu);

    if ( n<=0 )
        return -1;
    
    for (i=0; i<n; i++)
    {
        mi = gwssrv_get_menu_item(i,menu);
        gwssrv_redraw_menuitem(mi);
    };
}
*/

/*
void enable_main_menu(void)
{
}
*/

/*
void disable_main_menu(void)
{
}
*/

// #deprecated
int on_mi_clicked(unsigned long item_number)
{
    int nothing = item_number;
    return 0;
}


