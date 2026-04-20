// calculator.c
// Gramado OS client-side GUI calculator application
// Demonstrates complete Gramado OS application architecture

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>
#include <gws.h>

// Global display pointer
struct gws_display_d *Display;

// Window and control IDs
static int main_window = -1;
static int display_window = -1;
static int button_0 = -1;
static int button_1 = -1;
static int button_2 = -1;
static int button_3 = -1;
static int button_4 = -1;
static int button_5 = -1;
static int button_6 = -1;
static int button_7 = -1;
static int button_8 = -1;
static int button_9 = -1;
static int button_add = -1;
static int button_sub = -1;
static int button_mul = -1;
static int button_div = -1;
static int button_equals = -1;
static int button_clear = -1;
static int button_close = -1;

// Default responder for keyboard navigation
static int default_responder = -1;

// Cached client area
static unsigned long cr_left = 0;
static unsigned long cr_top = 0;
static unsigned long cr_width = 0;
static unsigned long cr_height = 0;

// Calculator state
static char display_text[64] = "0";
//static double accumulator = 0.0;
static long accumulator = 0;
static char current_op = '\0';
static int new_number = 1;

// Function prototypes
static void query_client_area(int fd);
static void update_display(int fd);
static void update_children(int fd);
static void handle_digit(int digit);
static void handle_operator(char op);
static void handle_equals(void);
static void handle_clear(void);
static void set_default_responder(int wid);
static void switch_responder(int fd);
static void trigger_default_responder(int fd);
static void exitProgram(int fd);
static int calculatorProcedure(int fd, int event_window, int event_type, 
                               unsigned long long1, unsigned long long2);
static void pump(int fd);

// ----------------------------------------------------
// Helper functions
// ----------------------------------------------------

static void query_client_area(int fd)
{
    struct gws_window_info_d wi;
    gws_get_window_info(fd, main_window, &wi);
    cr_left = wi.cr_left;
    cr_top = wi.cr_top;
    cr_width = wi.cr_width;
    cr_height = wi.cr_height;
}

static void update_display(int fd)
{
    // Clear display area and redraw text
    gws_redraw_window(fd, display_window, TRUE);
    gws_draw_text(fd, display_window, 10, 15, COLOR_BLACK, display_text);
    gws_refresh_window(fd, display_window);
}

// ----------------------------------------------------
// Calculator logic
// ----------------------------------------------------

static void handle_digit(int digit)
{
    if (new_number) {
        sprintf(display_text, "%d", digit);
        new_number = 0;
    } else {
        if (strlen(display_text) < 15) {
            char temp[64];
            sprintf(temp, "%s%d", display_text, digit);
            strcpy(display_text, temp);
        }
    }
}

static void handle_operator(char op)
{
    if (current_op != '\0' && !new_number) {
        handle_equals();
    }
    //accumulator = atof(display_text);
    accumulator = atoi(display_text);
    current_op = op;
    new_number = 1;
}

static void handle_equals(void)
{
    if (current_op == '\0') return;
    
    //double current_value = atof(display_text);
    long current_value = atoi(display_text);
    //double result = 0.0;
    long result = 0;

    switch (current_op) {
        case '+': result = accumulator + current_value; break;
        case '-': result = accumulator - current_value; break;
        case '*': result = accumulator * current_value; break;
        case '/': 
            //if (current_value != 0.0)
            if (current_value != 0)
                result = accumulator / current_value;
            else
                result = 0;//.0;
            break;
    }

    //sprintf(display_text, "%.2f", result);
    sprintf(display_text, "%d", result);
    accumulator = result;
    current_op = '\0';
    new_number = 1;
}

static void handle_clear(void)
{
    strcpy(display_text, "0");
    accumulator = 0; //0.0;
    current_op = '\0';
    new_number = 1;
}

// ----------------------------------------------------
// Layout and UI update
// ----------------------------------------------------

static void update_children(int fd)
{
    query_client_area(fd);
    
    // Calculate responsive sizes
    unsigned long btn_w = cr_width / 5;
    unsigned long btn_h = cr_height / 7;
    if (btn_h < 40) btn_h = 40;
    if (btn_w < 60) btn_w = 60;
    
    unsigned long margin = 10;
    unsigned long display_h = btn_h;
    unsigned long display_y = margin;
    
    // Redraw main window background
    gws_redraw_window(fd, main_window, TRUE);
    
    // Update display window
    gws_change_window_position(fd, display_window, margin, display_y);
    gws_resize_window(fd, display_window, cr_width - (2 * margin), display_h);
    gws_redraw_window(fd, display_window, TRUE);
    gws_draw_text(fd, display_window, 10, 15, COLOR_BLACK, display_text);
    
    // Button grid starts below display
    unsigned long grid_y = display_y + display_h + margin;
    
    // Row 0: 7, 8, 9, /
    gws_change_window_position(fd, button_7, margin, grid_y);
    gws_resize_window(fd, button_7, btn_w, btn_h);
    gws_redraw_window(fd, button_7, FALSE);
    
    gws_change_window_position(fd, button_8, margin + btn_w, grid_y);
    gws_resize_window(fd, button_8, btn_w, btn_h);
    gws_redraw_window(fd, button_8, FALSE);
    
    gws_change_window_position(fd, button_9, margin + 2*btn_w, grid_y);
    gws_resize_window(fd, button_9, btn_w, btn_h);
    gws_redraw_window(fd, button_9, FALSE);
    
    gws_change_window_position(fd, button_div, margin + 3*btn_w, grid_y);
    gws_resize_window(fd, button_div, btn_w, btn_h);
    gws_redraw_window(fd, button_div, FALSE);
    
    // Row 1: 4, 5, 6, *
    grid_y += btn_h;
    gws_change_window_position(fd, button_4, margin, grid_y);
    gws_resize_window(fd, button_4, btn_w, btn_h);
    gws_redraw_window(fd, button_4, FALSE);
    
    gws_change_window_position(fd, button_5, margin + btn_w, grid_y);
    gws_resize_window(fd, button_5, btn_w, btn_h);
    gws_redraw_window(fd, button_5, FALSE);
    
    gws_change_window_position(fd, button_6, margin + 2*btn_w, grid_y);
    gws_resize_window(fd, button_6, btn_w, btn_h);
    gws_redraw_window(fd, button_6, FALSE);
    
    gws_change_window_position(fd, button_mul, margin + 3*btn_w, grid_y);
    gws_resize_window(fd, button_mul, btn_w, btn_h);
    gws_redraw_window(fd, button_mul, FALSE);
    
    // Row 2: 1, 2, 3, -
    grid_y += btn_h;
    gws_change_window_position(fd, button_1, margin, grid_y);
    gws_resize_window(fd, button_1, btn_w, btn_h);
    gws_redraw_window(fd, button_1, FALSE);
    
    gws_change_window_position(fd, button_2, margin + btn_w, grid_y);
    gws_resize_window(fd, button_2, btn_w, btn_h);
    gws_redraw_window(fd, button_2, FALSE);
    
    gws_change_window_position(fd, button_3, margin + 2*btn_w, grid_y);
    gws_resize_window(fd, button_3, btn_w, btn_h);
    gws_redraw_window(fd, button_3, FALSE);
    
    gws_change_window_position(fd, button_sub, margin + 3*btn_w, grid_y);
    gws_resize_window(fd, button_sub, btn_w, btn_h);
    gws_redraw_window(fd, button_sub, FALSE);
    
    // Row 3: 0, C, =, +
    grid_y += btn_h;
    gws_change_window_position(fd, button_0, margin, grid_y);
    gws_resize_window(fd, button_0, btn_w, btn_h);
    gws_redraw_window(fd, button_0, FALSE);
    
    gws_change_window_position(fd, button_clear, margin + btn_w, grid_y);
    gws_resize_window(fd, button_clear, btn_w, btn_h);
    gws_redraw_window(fd, button_clear, FALSE);
    
    gws_change_window_position(fd, button_equals, margin + 2*btn_w, grid_y);
    gws_resize_window(fd, button_equals, btn_w, btn_h);
    gws_redraw_window(fd, button_equals, FALSE);
    
    gws_change_window_position(fd, button_add, margin + 3*btn_w, grid_y);
    gws_resize_window(fd, button_add, btn_w, btn_h);
    gws_redraw_window(fd, button_add, FALSE);
    
    // Close button at bottom
    grid_y += btn_h + margin;
    gws_change_window_position(fd, button_close, cr_width/2 - btn_w/2, grid_y);
    gws_resize_window(fd, button_close, btn_w, btn_h);
    gws_redraw_window(fd, button_close, FALSE);
    
    // Refresh main window
    gws_refresh_window(fd, main_window);
}

// ----------------------------------------------------
// Responder management
// ----------------------------------------------------

static void set_default_responder(int wid)
{
    if (wid >= 0)
        default_responder = wid;
}

static void switch_responder(int fd)
{
    // Simple toggle between equals and close for demo
    if (default_responder == button_equals) {
        set_default_responder(button_close);
        gws_set_focus(fd, button_close);
    } else {
        set_default_responder(button_equals);
        gws_set_focus(fd, button_equals);
    }
}

static void trigger_default_responder(int fd)
{
    if (default_responder == button_equals) {
        handle_equals();
        update_display(fd);
    } else if (default_responder == button_close) {
        exitProgram(fd);
    }
}

static void exitProgram(int fd)
{
    if (fd < 0) return;
    
    // Destroy all windows
    gws_destroy_window(fd, display_window);
    gws_destroy_window(fd, button_0);
    gws_destroy_window(fd, button_1);
    gws_destroy_window(fd, button_2);
    gws_destroy_window(fd, button_3);
    gws_destroy_window(fd, button_4);
    gws_destroy_window(fd, button_5);
    gws_destroy_window(fd, button_6);
    gws_destroy_window(fd, button_7);
    gws_destroy_window(fd, button_8);
    gws_destroy_window(fd, button_9);
    gws_destroy_window(fd, button_add);
    gws_destroy_window(fd, button_sub);
    gws_destroy_window(fd, button_mul);
    gws_destroy_window(fd, button_div);
    gws_destroy_window(fd, button_equals);
    gws_destroy_window(fd, button_clear);
    gws_destroy_window(fd, button_close);
    gws_destroy_window(fd, main_window);
    
    exit(0);
}

// ----------------------------------------------------
// Event procedure
// ----------------------------------------------------

static int calculatorProcedure(
    int fd,
    int event_window,
    int event_type,
    unsigned long long1,
    unsigned long long2)
{
    if (fd < 0 || event_window < 0 || event_type < 0)
        return -1;
    
    switch (event_type) {
        
    case 0:
        // Null/heartbeat event
        return 0;
        
    case MSG_KEYDOWN:
        switch (long1) {
        case VK_RETURN:
            trigger_default_responder(fd);
            break;
        case '0': handle_digit(0); update_display(fd); break;
        case '1': handle_digit(1); update_display(fd); break;
        case '2': handle_digit(2); update_display(fd); break;
        case '3': handle_digit(3); update_display(fd); break;
        case '4': handle_digit(4); update_display(fd); break;
        case '5': handle_digit(5); update_display(fd); break;
        case '6': handle_digit(6); update_display(fd); break;
        case '7': handle_digit(7); update_display(fd); break;
        case '8': handle_digit(8); update_display(fd); break;
        case '9': handle_digit(9); update_display(fd); break;
        case '+': handle_operator('+'); update_display(fd); break;
        case '-': handle_operator('-'); update_display(fd); break;
        case '*': handle_operator('*'); update_display(fd); break;
        case '/': handle_operator('/'); update_display(fd); break;
        case '=': handle_equals(); update_display(fd); break;
        case 'C':
        case 'c': handle_clear(); update_display(fd); break;
        }
        break;
        
    case MSG_SYSKEYDOWN:
        switch (long1) {
        case VK_F5:
            printf("calculator: VK_F5 - Refresh\n");
            update_children(fd);
            break;
        case VK_F12:
            printf("calculator: VK_F12 - Debug info\n");
            printf("  Display: %s\n", display_text);
            printf("  Accumulator: %.2f\n", accumulator);
            printf("  Operator: %c\n", current_op ? current_op : '0');
            break;
        case VK_ARROW_LEFT:
            printf("calculator: VK_ARROW_LEFT\n");
            switch_responder(fd);
            break;
        case VK_ARROW_RIGHT:
            printf("calculator: VK_ARROW_RIGHT\n");
            break;
        case VK_ARROW_UP:
            printf("calculator: VK_ARROW_UP\n");
            break;
        case VK_ARROW_DOWN:
            printf("calculator: VK_ARROW_DOWN\n");
            break;
        }
        break;
        
    case GWS_MouseClicked:
        // Handle button clicks
        if ((int)long1 == button_0) { handle_digit(0); update_display(fd); }
        else if ((int)long1 == button_1) { handle_digit(1); update_display(fd); }
        else if ((int)long1 == button_2) { handle_digit(2); update_display(fd); }
        else if ((int)long1 == button_3) { handle_digit(3); update_display(fd); }
        else if ((int)long1 == button_4) { handle_digit(4); update_display(fd); }
        else if ((int)long1 == button_5) { handle_digit(5); update_display(fd); }
        else if ((int)long1 == button_6) { handle_digit(6); update_display(fd); }
        else if ((int)long1 == button_7) { handle_digit(7); update_display(fd); }
        else if ((int)long1 == button_8) { handle_digit(8); update_display(fd); }
        else if ((int)long1 == button_9) { handle_digit(9); update_display(fd); }
        else if ((int)long1 == button_add) { handle_operator('+'); update_display(fd); }
        else if ((int)long1 == button_sub) { handle_operator('-'); update_display(fd); }
        else if ((int)long1 == button_mul) { handle_operator('*'); update_display(fd); }
        else if ((int)long1 == button_div) { handle_operator('/'); update_display(fd); }
        else if ((int)long1 == button_equals) { handle_equals(); update_display(fd); }
        else if ((int)long1 == button_clear) { handle_clear(); update_display(fd); }
        else if ((int)long1 == button_close) { exitProgram(fd); }
        break;
        
    case MSG_CLOSE:
        exitProgram(fd);
        break;
        
    case MSG_PAINT:
        update_children(fd);
        break;
        
    default:
        return -1;
    }
    
    return 0;
}

// ----------------------------------------------------
// Pump: fetch events from Display Server
// ----------------------------------------------------

static void pump(int fd)
{
    struct gws_event_d event;
    event.used = FALSE;
    event.magic = 0;
    event.type = 0;
    
    struct gws_event_d *e =
        (struct gws_event_d *) gws_get_next_event(fd, main_window, &event);
    
    if ((void*)e == NULL) return;
    if (e->magic != 1234 || e->used != TRUE) return;
    if (e->type <= 0) return;
    
    calculatorProcedure(fd, e->window, e->type, e->long1, e->long2);
}

// ----------------------------------------------------
// Main
// ----------------------------------------------------

int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    
    // Connect to Display Server
    Display = gws_open_display(display_name);
    if ((void*)Display == NULL) {
        printf("calculator: Could not open display\n");
        return EXIT_FAILURE;
    }
    
    int fd = Display->fd;
    if (fd <= 0) {
        printf("calculator: Invalid fd\n");
        return EXIT_FAILURE;
    }
    
    // Get screen dimensions
    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);
    
    // Main window geometry
    unsigned long win_w = 320;
    unsigned long win_h = 400;
    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;
    
    // Create main window
    main_window = gws_create_window(
        fd,
        WT_OVERLAPPED,
        WINDOW_STATUS_ACTIVE,
        WINDOW_STATE_NULL,
        "Calculator",
        win_x, win_y, win_w, win_h,
        0,
        WS_APP,
        COLOR_WHITE,
        COLOR_GRAY);
    
    if (main_window < 0) {
        printf("calculator: Failed to create main window\n");
        return EXIT_FAILURE;
    }
    
    gws_refresh_window(fd, main_window);
    
    // Query client area
    query_client_area(fd);
    
    // Calculate initial button sizes
    unsigned long btn_w = cr_width / 5;
    unsigned long btn_h = cr_height / 7;
    if (btn_h < 40) btn_h = 40;
    if (btn_w < 60) btn_w = 60;
    
    unsigned long margin = 10;
    unsigned long display_h = btn_h;
    unsigned long display_y = margin;
  
/*
    // Create display window
    display_window = 
        gws_create_window(
        fd, WT_EDITBOX, 1, 1,
        "Calc", 
        margin, 
        display_y, 
        cr_width - (2 * margin), 
        display_h,
        main_window, WS_CHILD, COLOR_WHITE, COLOR_WHITE);
*/


    // Create display window
    display_window = 
        gws_create_window(
        fd, 
        WT_SIMPLE,            // type
        1,                    // Window status - (active, inactive) (Overlapped only)
        WINDOW_STATE_NORMAL,  // Window state - (min, max ...)
        "Calc", 
        margin, 
        display_y, 
        cr_width - (2 * margin), 
        display_h,
        main_window, 
        WS_CHILD,     // style - (bit flags)
        COLOR_BLACK,          // Client area color
        COLOR_SOFT_WHITE      // bg color
    );

    // Button grid
    unsigned long grid_y = display_y + display_h + margin;
    
    // Create all buttons
    button_7 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "7",
        margin, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_8 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "8",
        margin + btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_9 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "9",
        margin + 2*btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_div = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "/",
        margin + 3*btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    
    grid_y += btn_h;
    button_4 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "4",
        margin, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_5 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "5",
        margin + btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_6 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "6",
        margin + 2*btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_mul = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "*",
        margin + 3*btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    
    grid_y += btn_h;
    button_1 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "1",
        margin, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_2 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "2",
        margin + btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_3 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "3",
        margin + 2*btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_sub = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "-",
        margin + 3*btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    
    grid_y += btn_h;
    button_0 = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "0",
        margin, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_clear = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "C",
        margin + btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_equals = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "=",
        margin + 2*btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    button_add = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "+",
        margin + 3*btn_w, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    
    grid_y += btn_h + margin;
    button_close = gws_create_window(fd, WT_BUTTON, BS_DEFAULT, 1, "Close",
        cr_width/2 - btn_w/2, grid_y, btn_w, btn_h, main_window, 0, COLOR_GRAY, COLOR_GRAY);
    
    // Set default responder
    set_default_responder(button_equals);
    
    // Activate and show
    gws_set_active(fd, main_window);
    gws_set_focus(fd, button_equals);
    gws_refresh_window(fd, main_window);
    
    // ================================
    // Event loop with dual sources
    // ================================
    
    while (1) {
        // 1. Pump events from Display Server
        pump(fd);
        
        // 2. Pump events from Input Broker (system events)
        if (rtl_get_event() == TRUE) {
            calculatorProcedure(
                fd,
                (int) RTLEventBuffer[0],           // window id
                (int) RTLEventBuffer[1],           // event type
                (unsigned long) RTLEventBuffer[2], // VK code
                (unsigned long) RTLEventBuffer[3]  // scancode
            );
            RTLEventBuffer[1] = 0; // clear after dispatch
        }
    }
    
    return EXIT_SUCCESS;
}