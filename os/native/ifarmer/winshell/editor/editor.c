// editor.c
// Text editor for Gramado OS.
// This is a client-side GUI application connected 
// with the display server.
// Created by Fred Nora.

// Connecting via AF_INET.
// tutorial example taken from. 
// https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
/*
    To make a process a TCP server, you need to follow the steps 
    given below −
    Create a socket with the socket() system call.
    Bind the socket to an address using the bind() system call. 
    For a server socket on the Internet, an address consists of a 
    port number on the host machine.
    Listen for connections with the listen() system call.
    Accept a connection with the accept() system call. 
    This call typically blocks until a client connects with the server.
    Send and receive data using the read() and write() system calls.
*/
// See:
// https://wiki.osdev.org/Message_Passing_Tutorial
// https://wiki.osdev.org/Synchronization_Primitives
// ...

#include "globals.h"
#include <editor.h>



// Used by small_dc
unsigned char *small_buffer;

struct dccanvas_d *dc00;  // shared dc
struct dccanvas_d *small_dc;
// ...

#define CANVAS_CLIENTAREA  0
#define CANVAS_SMALL  1
// ...

struct ui_component_d *uic_button_save;


/*
#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port   = 7548, 
    .sin_addr   = IP(192, 168, 1, 79),
};
*/

static int isTimeToQuit = FALSE;
static int file_status=FALSE;

//
// Text buffer
//

struct text_buffer_d *text_buffer;

struct editor_initialization_d  EditorInitialization;

// #test
// The file buffer.
//char file_buffer[512];
char file_buffer[1024];

// Divide the client area into equal portions horizontally.
// Used for positioning child windows relative to the main window.
#define CLIENT_AREA_DIVISIONS 8


struct button_info_d
{
    int button_id;

// This is the window id that represents the icon.
    int wid;

// Absolute values
    unsigned long absolute_left;
    unsigned long absolute_top;
    unsigned long width; 
    unsigned long height;

// Relative values
    unsigned long left;
    unsigned long top;

// The state of the icon, it also represents
// the state of the client application.
// (running, minimized, etc.).
    int state;
};
static struct button_info_d  MyButton_Save;

static int __hover_button_id = -1;

// Cached frame/chrome area
static unsigned long frame_left   = 0;
static unsigned long frame_top    = 0;
static unsigned long frame_width  = 0;
static unsigned long frame_height = 0;

// Cached client area
static unsigned long cr_left   = 0;
static unsigned long cr_top    = 0;
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;


//
// Windows
//

// private
static int main_window = -1;
// static int addressbar_window = -1;
// static int savebutton_window = -1;
// static int client_window = -1; // #bugbug: Sometimes we can't delete this window.
// ...

struct child_window_d
{
    unsigned long l;
    unsigned long t;
    unsigned long w;
    unsigned long h;
};
struct child_window_d cwAddressBar;
//struct child_window_d cwButton;
struct child_window_d cwText;

// #todo
// int button_list[8];

// cursor
static int cursor_x = 0;
static int cursor_y = 0;
static int cursor_x_max = 0;
static int cursor_y_max = 0;

static int blink_status=FALSE;

// tmp input pointer.
// #todo
// we will copy all the iput support from the other editor.
// for now we will use this tmp right here.

//int tmp_ip_x=8;
//int tmp_ip_y=8;

// Program name
static const char *program_name = "EDITOR";
// Addressbar title
static const char *bar1_string = "bar1";
// Button label.
static const char *b1_string = "Save";
// Client window title.
static const char *cw_string = "cw";

// text
static unsigned long text1_l = 0;
static unsigned long text1_t = 0;
static unsigned int text1_color = 0;
static const char *text1_string = "Name:";
static const char *text2_string = "TEXT.TXT";

// =====================================
// Prototypes

static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my);
static void on_button_clicked(int id);

static void editorShutdown(int fd);

static void update_clients(int fd);

static int editor_init_globals(void);
static int editor_init_windows(void);
struct text_buffer_d *editorInitBuffer(void);

static void __test_text(int fd, int wid);
//static void __test_load_file(int socked_fd);

static void editorDrawChar_dc(struct dccanvas_d *dc, int ch);
static void editorInjectChar(struct text_buffer_d *tb, int line, int col, char ch);
static void editorInjectCharColored(
    struct text_buffer_d *tb,
    int line, int col,
    char ch,
    unsigned int fg,
    unsigned int bg,
    unsigned int attr );
static void editorDrawCell(struct dccanvas_d *dc, int line, int col);

static void editorHandleKey(int key);

static void editorDrawStatusBar(void);
static int editorDrawInSmallBuffer(void);

static int 
editorProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 );

static void pump(int fd, int wid);

static int __open_document(char *file_name);

// Create a canvas_information_d from a dc
static struct canvas_information_d *
editorCreateCanvasInfo(struct dccanvas_d *dc, int owner_wid, int is_frame);

static int __editor_initialize_small_buffer(void);

static int __editor_initialize(void);


// =====================================
// Functions


// Hit-test for our fake button in PowerApp
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my) 
{

// Button Restart
    if ( rel_mx >= MyButton_Save.left && 
         rel_mx <= MyButton_Save.left + MyButton_Save.width &&
         rel_my >= MyButton_Save.top  && 
         rel_my <= MyButton_Save.top + MyButton_Save.height )
    {
        return (int) MyButton_Save.button_id;
    }

/*
// Button Shutdown
    if ( rel_mx >= MyButton_Shutdown.left && 
         rel_mx <= MyButton_Shutdown.left + MyButton_Shutdown.width &&
         rel_my >= MyButton_Shutdown.top  && 
         rel_my <= MyButton_Shutdown.top + MyButton_Shutdown.height )
    {
        return (int) MyButton_Shutdown.button_id;
    }
*/

    // ...

    return -1;
}

// Handle click events for components
static void on_button_clicked(int id)
{
    if (id < 0)
        return;

    switch (id)
    {
        case 1:  // Save button
            printf("Button %d clicked!\n", id);
            //rtl_clone_and_execute("reboot.bin");
            //exit(0);
            break;

        case 2:  // MyButton_Shutdown.icon_id
            printf("Button %d clicked!\n", id);
            //rtl_clone_and_execute("shutdown.bin");
            //exit(0);
            break;

        default:
            printf("Unknown button clicked: %d\n", id);
            break;
    };
}


static void editorShutdown(int fd)
{
    if (fd<0)
        return;

    //gws_destroy_window(fd,client_window);   // #bugbug: sometimes we can't delete this window.
    //gws_destroy_window(fd,savebutton_window);
    //gws_destroy_window(fd,addressbar_window);
    gws_destroy_window(fd,main_window);
}

static void update_clients(int fd)
{
    struct gws_window_info_d  lWi;

// Parameter
    if (fd < 0){
        return;
    }

// Get info about the main window.
// IN: fd, wid, window info structure.
    gws_get_window_info ( 
        fd, main_window, (struct gws_window_info_d *) &lWi );

    // Frame/chrome
    frame_left = lWi.left;
    frame_top = lWi.top;
    frame_width = lWi.width;
    frame_height = lWi.height;

    // Client area
    cr_left = lWi.cr_left;
    cr_top = lWi.cr_top;
    cr_width = lWi.cr_width;
    cr_height = lWi.cr_height;

// ------------------------

    if ((void*)dc00 == NULL)
        return;

//
// bg for the client area
//

    lingui_draw_rectangle0_dc (
        dc00,
        0, 0, lWi.cr_width, lWi.cr_height,
        COLOR_GRAY,
        0  // ROP
    );


// ------------------------
// Text
// Let's print the text, before the address bar.

    text1_l = 2;
    text1_t = 4 + (24/3);
    text1_color = COLOR_BLACK;

// ---------------------------------------------
// Address bar
// #todo: 
// '.l': It actually depends on the text befor this.
// We need to know the text width.

    cwAddressBar.l = (( lWi.cr_width/8 )*2);
    cwAddressBar.t = 4;
    cwAddressBar.w = (( lWi.cr_width/8 )*3);
    cwAddressBar.h = 24; 

    unsigned long ab_l = (( lWi.cr_width/8 )*2);
    unsigned long ab_t = 4;
    unsigned long ab_w = (( lWi.cr_width/8 )*3);
    unsigned long ab_h = 24;


    lingui_draw_rectangle0_dc (
        dc00,
        ab_l, ab_t, ab_w, ab_h,
        COLOR_WHITE,
        0  // ROP
    );

// ============================================================
// Create restart button

    MyButton_Save.button_id = 1;   // arbitrary ID
    // MyButton_Save.wid     = main_window; // parent window ID

    // Relative values
    MyButton_Save.left = (( lWi.cr_width/8 )*6); //4;
    MyButton_Save.top  = 4;

    // Absolute coordinates (relative to screen)
    MyButton_Save.absolute_left = lWi.left + lWi.cr_left + MyButton_Save.left;
    MyButton_Save.absolute_top  = lWi.top + lWi.cr_top + MyButton_Save.top;
    MyButton_Save.width         = (( lWi.cr_width/8 )*1);
    MyButton_Save.height        = 24;

    // Initial state
    // MyButton_Save.state = 0;
  
    libgui_set_ui_component_position(
        uic_button_save, 
        MyButton_Save.left, 
        MyButton_Save.top );
    libgui_set_ui_component_dimension(
        uic_button_save,
        MyButton_Save.width,
        MyButton_Save.height );
    // #todo: We have an issue with the label string
    libgui_redraw_ui_component( uic_button_save, dc00 );

//-----------------------
// The client window where we type the text.

    cwText.l = 0;
    cwText.t = (cwAddressBar.t + cwAddressBar.h + 2);
    cwText.w = lWi.cr_width;
    cwText.h = (lWi.cr_height - cwText.t);

// (Editbox)
// Client window (White)
// Inside the mainwindow.
// Lembre-se que temos uma status bar.

// left:
    unsigned long cw_left = 0;
// top: pad | address bar | pad
    unsigned long cw_top =  (ab_t + ab_h + 4);
// width: Width - borders.
    unsigned long cw_width = (lWi.cr_width);
// height:
// #bugbug:
// We gotta get the client window values.
    unsigned long cw_height = (lWi.cr_height - cw_top);

    lingui_draw_rectangle0_dc (
        dc00,
        cw_left, cw_top, cw_width, cw_height,
        COLOR_WHITE,
        0  // ROP
    );

//
// BLIT small buffer
//

    const char *msg = "Hello";    // Example string
    size_t StringSize = (size_t) (sizeof(msg) *8);
    if ((void*)small_dc != NULL)
    {
        libgui_blit_canvas_to_canvas(
            CANVAS_SMALL,
            CANVAS_CLIENTAREA,
            0, 0,                       // destination position
            StringSize +2+2, 8 +2+2 );  // size
    }
}

static int editor_init_globals(void)
{
    gScreenWidth  = (unsigned long) gws_get_system_metrics(1);
    gScreenHeight = (unsigned long) gws_get_system_metrics(2);
    //...
    return 0;
}

static int editor_init_windows(void)
{
    register int i=0;
    for (i=0; i<WINDOW_COUNT_MAX; i++){
        windowList[i] = 0;
    };
    return 0;
}

// Initialize the text buffer structure
struct text_buffer_d *editorInitBuffer(void)
{
    int i, j;

    // Allocate the text buffer
    struct text_buffer_d *tb = (struct text_buffer_d *) malloc(sizeof(struct text_buffer_d));
    if (!tb) return NULL;

    tb->line_count = DEFAULT_ROWS;
    tb->max_lines  = DEFAULT_ROWS;

    // Allocate array of line pointers
    tb->lines = (struct line_buffer_d **) malloc(sizeof(struct line_buffer_d *) * DEFAULT_ROWS);
    if (!tb->lines) {
        //free(tb);
        return NULL;
    }

    // Initialize each line
    for (i = 0; i < DEFAULT_ROWS; i++) {
        tb->lines[i] = (struct line_buffer_d *) malloc(sizeof(struct line_buffer_d));
        if (!tb->lines[i]) continue;

        tb->lines[i]->char_count = 0;
        tb->lines[i]->max_chars  = DEFAULT_COLS;

        // Allocate cells for each line
        tb->lines[i]->cells = (struct char_cell_d *) malloc(sizeof(struct char_cell_d) * DEFAULT_COLS);
        if (!tb->lines[i]->cells) continue;

        // Initialize cells
        for (j = 0; j < DEFAULT_COLS; j++) {
            tb->lines[i]->cells[j].ch   = ' ';
            tb->lines[i]->cells[j].fg   = COLOR_BLACK;
            tb->lines[i]->cells[j].bg   = COLOR_WHITE;
            tb->lines[i]->cells[j].attr = 0;
        }
    }

    return tb;
}


// Draw a character directly into the DC
static void editorDrawChar_dc(struct dccanvas_d *dc, int ch)
{
    int pos_x = cursor_x;
    int pos_y = cursor_y;
    unsigned int Color = COLOR_BLACK;

    if (!dc) 
        return;

    // Bounds check
    if (pos_x < 0) pos_x = 0;
    if (pos_y < 0) pos_y = 0;

    // End of line
    if (pos_x >= cursor_x_max) {
        pos_x = 0;
        pos_y++;
    }

    // Last line (no scroll yet)
    if (pos_y >= cursor_y_max) {
        pos_y = cursor_y_max - 1;
    }

    // Save cursor
    cursor_x = pos_x;
    cursor_y = pos_y;

    // Draw character into the canvas
    libgui_drawchar_dc(
        dc,
        cr_left + (cursor_x * 8),   // X position
        cr_top  + (cursor_y * 8),   // Y position
        ch,                         // character
        Color,                      // foreground
        COLOR_WHITE,                // background
        0                           // ROP
    );

    cursor_x++;  // Advance cursor
}

// Updates the logical buffer only.
static void editorInjectChar(struct text_buffer_d *tb, int line, int col, char ch)
{
    if (!tb) return;
    if (line < 0 || line >= tb->line_count) return;

    struct line_buffer_d *lb = tb->lines[line];
    if (!lb) return;
    if (col < 0 || col >= lb->max_chars) return;

    lb->cells[col].ch   = ch;
    lb->cells[col].fg   = COLOR_BLACK;
    lb->cells[col].bg   = COLOR_WHITE;
    lb->cells[col].attr = 0;

    if (col >= lb->char_count) {
        lb->char_count = col + 1;
    }
}

static void editorInjectCharColored(
    struct text_buffer_d *tb,
    int line, int col,
    char ch,
    unsigned int fg,
    unsigned int bg,
    unsigned int attr )
{
    if (!tb) return;
    if (line < 0 || line >= tb->line_count) return;

    struct line_buffer_d *lb = tb->lines[line];
    if (!lb) return;
    if (col < 0 || col >= lb->max_chars) return;

    lb->cells[col].ch   = ch;
    lb->cells[col].fg   = fg;
    lb->cells[col].bg   = bg;
    lb->cells[col].attr = attr;

    if (col >= lb->char_count) {
        lb->char_count = col + 1;
    }
}


// Draws a single cell from the buffer into the canvas.
static void editorDrawCell(struct dccanvas_d *dc, int line, int col)
{
    if (!dc || !text_buffer) return;
    if (line < 0 || line >= text_buffer->line_count) return;

    struct line_buffer_d *lb = text_buffer->lines[line];
    if (!lb || col < 0 || col >= lb->max_chars) return;

    struct char_cell_d *cell = &lb->cells[col];

    libgui_drawchar_dc(
        dc,
        cr_left + (col * 8),
        cr_top  + (line * 8),
        cell->ch,
        cell->fg,
        cell->bg,
        cell->attr
    );
}

// Redraw the entire text buffer into the canvas
static void editorRedrawBuffer(struct dccanvas_d *dc)
{
    int line;
    int LineCount;
    int col;
    int CharCount;

    if (!dc || !text_buffer)
        return;

    LineCount = text_buffer->line_count;
    for (line=0; line < LineCount; line++) 
    {
        struct line_buffer_d *lb = text_buffer->lines[line];
        if (!lb)
            continue;

        CharCount = lb->char_count;
        for (col=0; col < CharCount; col++) {
            editorDrawCell(dc, line, col);
        }
    }
}

// Handle keyboard input for the editor
static void editorHandleKey(int key)
{
    if (key <= 0)
        return;

    switch (key)
    {
        /*
        case VK_RETURN:
            // Move to new line
            cursor_x = 0;
            cursor_y++;
            break;
        */
        case VK_RETURN:
            // Move to new line
            cursor_x = 0;
            if (cursor_y < (DEFAULT_ROWS - 1)) {
                cursor_y++;
            } else {
                // Already at last line, stay there
                cursor_y = DEFAULT_ROWS - 1;
            }
            break;

        case VK_TAB:
            // Insert spaces (basic tab = 4 spaces)
            editorDrawChar_dc(dc00, ' ');
            editorDrawChar_dc(dc00, ' ');
            editorDrawChar_dc(dc00, ' ');
            editorDrawChar_dc(dc00, ' ');
            break;

        case VK_ESCAPE:
            // Escape pressed — could trigger quit or clear
            printf("editor.bin: VK_ESCAPE pressed\n");
            break;

        case VK_BACK:
            // Handle backspace: move cursor back and erase
            if (cursor_x > 0) 
            {
                cursor_x--;
                libgui_drawchar_dc(dc00,
                    cr_left + (cursor_x * 8),
                    cr_top  + (cursor_y * 8),
                    ' ', COLOR_BLACK, COLOR_WHITE, 0);
            }
            break;

        /*
        default:
            // Printable characters
            if (key >= 32 && key <= 126) 
            {
                editorDrawChar_dc(dc00, key);
            }
            break;
        */

        /*
        default:
            if (key >= 32 && key <= 126) 
            {
                editorInjectChar(text_buffer, cursor_y, cursor_x, key);
                editorDrawCell(dc00, cursor_y, cursor_x);
                cursor_x++;
            }
            break;
        */

        default:
            if (key >= 32 && key <= 126) 
            {
                if (cursor_x < DEFAULT_COLS) {
                    editorInjectChar(
                        text_buffer, cursor_y, cursor_x, key);
                    editorDrawCell(dc00, cursor_y, cursor_x);
                    cursor_x++;
                } else {
                    // At end of line, optionally wrap or ignore
                    cursor_x = DEFAULT_COLS - 1;
                }
            }
            break;
    };
}

void
editorSetCursor( 
    int x,
    int y )
{
// #:
// The server is printing the char if the
// window with focus is an editbox.
// So, we need to tell the ws to change the cursor position.

    if (cursor_x >= 0 && 
        cursor_x < cursor_x_max)
    {
        cursor_x = x;
    }

    if (cursor_y >= 0 && 
        cursor_y < cursor_y_max)
    {
        cursor_y = y;
    }
}

// Draw status bar showing line/column
static void editorDrawStatusBar(void)
{
    struct dccanvas_d *dc;

    dc = dc00;

    if (!dc || !text_buffer) 
        return;

    unsigned long sb_height = 24;
    unsigned long sb_left   = 0;
    unsigned long sb_top    = cr_height - sb_height;
    unsigned long sb_width  = cr_width;

    // Background rectangle
    lingui_draw_rectangle0_dc(
        dc,
        sb_left, sb_top, sb_width, sb_height,
        COLOR_LIGHTGRAY,  // background
        0                 // ROP
    );

    // Build status string from cursor position
    char status[64];
    sprintf( status, "Ln %d | Col %d", 
        cursor_y + 1, cursor_x + 1 );

    // Draw string into the status bar
    libgui_drawstring_dc(
        dc,
        sb_left + 8,        // x offset
        sb_top + 6,         // y offset
        COLOR_BLACK,        // foreground
        COLOR_LIGHTGRAY,    // background
        0, 
        status
    );
}

static int editorDrawInSmallBuffer(void)
{
    const char *msg = "Hello";  // Example string

    if ((void*)small_dc == NULL)
    {
        printf("Invalid small_dc\n");
        exit(1);
        //goto fail;
    }

// Fill background of small buffer
    lingui_draw_rectangle0_dc(
        small_dc,
        0, 0, (sizeof(msg) *8) +2+2, 8+2+2,
        COLOR_BLUE,
        ROP_COPY
    );

// Draw at position (2, 2) inside the small buffer
    libgui_drawstring_dc(
        small_dc,
        2, 2,                 // x, y
        COLOR_BLACK,          // foreground
        COLOR_WHITE,          // background
        ROP_COPY,             // raster op
        msg                   // string
    );

//
// BLIT
//

    size_t StringSize = (size_t) (sizeof(msg) *8);
    libgui_blit_canvas_to_canvas(
        CANVAS_SMALL,
        CANVAS_CLIENTAREA,
        0, 0,                      // destination position
        StringSize +2+2, 8+2+2 );  // size

    return 0;

fail:
    return (int) -1;
}

static int 
editorProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    int ButtonId = -1;

// Parameters
    if (fd<0){
        return (int) -1;
    }
    if (event_window<0){
        return (int) -1;
    }
    if (event_type < 0){
        return (int) -1;
    }

// Events
    switch (event_type){

    // Null event
    case 0:
        return 0;
        break;

    /*
    // If the event window is the main window, redraw all client windows.
    case MSG_PAINT:
        if (event_window == main_window)
        {
            // Update the text for address bar.
            //__test_text(fd,addressbar_window);
            // Update the text for the client window.
            //__test_text(fd,client_window);
            // Redraw the client windows.
            update_clients(fd);
            return 0;
        }
        break;
        */

    case MSG_PAINT:
        if (event_window == main_window)
        {
            // Redraw the window chrome and client area
            update_clients(fd);

            // Redraw the text buffer contents
            editorRedrawBuffer(dc00);

            // Draw the status bar
            editorDrawStatusBar();

            return 0;
        }
        break;


    case MSG_KEYDOWN:
        editorHandleKey(long1);
        // Draw the status bar
        editorDrawStatusBar();
        break;

    case MSG_KEYUP:
        //printf("editor: MSG_KEYUP\n");
         switch (long1) {
            case VK_RETURN: 
                printf("Editor: MSG_KEYUP VK_RETURN\n");
                break;
        };
        break;

    case MSG_SYSKEYDOWN:
        //printf("editor: MSG_SYSKEYDOWN %x\n",long1 );
        //printf("editor: VK_INSERT value %x\n",VK_INSERT );
        switch (long1) {
            case VK_F1:  printf("Editor: VK_F1\n"); break;
            case VK_F5:  printf("Editor: VK_F5\n"); break;
            // Unexpected
            // The kernel needs to send it to the server.
            case VK_F11:
                printf("Editor: VK_F11 (unexpected)\n");
                break;
            case VK_F12: 
                printf("Editor: VK_F12\n");
                break;

            // 
            case VK_INSERT:  printf("Editor: VK_INSERT\n"); break;
            case VK_DELETE:  printf("Editor: VK_DELETE\n"); break;
            case VK_HOME:  printf("Editor: VK_HOME\n"); break;
            case VK_END:  printf("Editor: VK_END\n"); break;
            case VK_PAGEUP:  printf("Editor: VK_PAGEUP \n"); break;
            case VK_PAGEDOWN:  printf("Editor: VK_PAGEDOWN \n"); break;
            case VK_RCONTROL:  printf("Editor: VK_RCONTROL \n"); break;
            case VK_ALTGR:  printf("Editor: VK_ALTGR \n"); break;
            case VK_APPS:  printf("Editor: VK_APPS \n"); break;

            //
            case VK_ARROW_RIGHT:  printf("Editor: VK_ARROW_RIGHT \n"); break;
            case VK_ARROW_UP:  printf("Editor: VK_ARROW_UP \n"); break;
            case VK_ARROW_DOWN:  printf("Editor: VK_ARROW_DOWN \n"); break;
            case VK_ARROW_LEFT:  printf("Editor: VK_ARROW_LEFT \n"); break;
        }
        return 0;
        break;

    case MSG_SYSKEYUP:
        switch (long1) {
            // 
            case VK_INSERT:  printf("Editor: VK_INSERT up\n"); break;
            case VK_DELETE:  printf("Editor: VK_DELETE up\n"); break;
            case VK_HOME:  printf("Editor: VK_HOME up\n"); break;
            case VK_END:  printf("Editor: VK_END up\n"); break;
            case VK_PAGEUP:  printf("Editor: 0x49 VK_PAGEUP up \n"); break;
            case VK_PAGEDOWN:  printf("Editor: 0x51 VK_PAGEDOWN up \n"); break;
            case VK_RCONTROL:  printf("Editor: VK_RCONTROL up \n"); break;
            case VK_ALTGR:  printf("Editor: VK_ALTGR up \n"); break;
            case VK_APPS:  printf("Editor: VK_APPS up \n"); break;

            //
            case VK_ARROW_RIGHT:  printf("Editor: VK_ARROW_RIGHT up\n"); break;
            case VK_ARROW_UP:  printf("Editor: VK_ARROW_UP up \n"); break;
            case VK_ARROW_DOWN:  printf("Editor: VK_ARROW_DOWN up \n"); break;
            case VK_ARROW_LEFT:  printf("Editor: VK_ARROW_LEFT up \n"); break;
        }
        return 0;
        break;

    // #test
    case MSG_MOUSEMOVE:
        // #bugbug
        // Kernel is sending us absolute values
        // instead of relative values.
        //printf("%d %d\n", long1, long2);
        ButtonId = (int) __hit_test_button(long1, long2);
        if (ButtonId > 0)
            __hover_button_id = ButtonId;
        if (ButtonId <= 0)
            __hover_button_id = -1;
        break; 

    case MSG_MOUSERELEASED:
        printf("editor: Button released: %d\n", __hover_button_id);
        //printf("editor: MSG_MOUSERELEASED:\n");
        on_button_clicked(__hover_button_id);

            // #test: Testing the activation
        // We gotta do this only when the window is not active
        //gws_set_active( fd, main_window );
        //gws_refresh_window (fd, main_window);

        break;


    // Mouse clicked on a button.
    case GWS_MouseClicked:
        printf("editor: GWS_MouseClicked\n");
        return 0;
        break;

    case MSG_CLOSE:
        printf ("editor.bin: MSG_CLOSE\n");
        editorShutdown(fd);
        //isTimeToQuit = TRUE;
        // #test
        //if ((void*) Display != NULL){
            //gws_close_display(Display);
        //}
        exit(0);
        break;
    
    // After a resize event.
    //case MSG_SIZE:
        //break;

    //case MSG_CREATE: 
        // Initialize the window. 
        //return 0; 
 
    // test
    //case 8888:
        //break;

    case 9191:
        printf("9191\n");
        break;

    case GWS_Undo:  // [control + z] (Undo)
        break;
    case GWS_Cut:   // [control + x] (Cut)
        break;
    case GWS_Copy:  // [control + c] (Copy)
        break;
    case GWS_Paste:  // [control + v] (Paste)
        break;
    case GWS_SelectAll:  // [control + a] (Select all)
        break;
    case GWS_Find:  // [control + f] (Find)
        printf("editor.bin: GWS_Find\n");
        break;
    case GWS_Save:  // [control + s] (Save?)
        break;

    //...
    
    default:
        goto fail;
        break;
    };

fail:
    return (int) -1;
}


// #test
// Set text and Get text into an editbox window.
static void __test_text(int fd, int wid)
{
    char string_buffer[256];
    char *p;
    int Status=0;

    if (fd<0)
        return;
    if (wid<0)
        return;

// Setup the local buffer.
    memset(string_buffer,0,256);
    sprintf(string_buffer,"dirty");

//
// Inject
//

    const char *short_text = "Short text";
    //const char *long_text = "This is a long text, a really long text";
    const char *long_text = 
        "This is a long text, a really, really, really, really, really long text";
    char *target_text;

    target_text = short_text;

    //if (wid == addressbar_window)
    //    target_text = short_text;
    //if (wid == client_window)
    //    target_text = long_text;

    // #bugbug: What is the size?
    // The limit is 256 chars.
    gws_set_text (
        (int) fd,      // fd,
        (int) wid,     // window id,
        (unsigned long) 1, 
        (unsigned long) 1, 
        (unsigned long) COLOR_BLACK,
        target_text );

/*
// ------------------
// Get back
    Status = 
    gws_get_text (
        (int) fd,     // fd,
        (int) wid,    // window id,
        (unsigned long)  1, 
        (unsigned long)  1, 
        (unsigned long) COLOR_BLACK,
        (char *) string_buffer );
*/

    //if ( (void*) p == NULL ){
    //    printf("editor.bin: Invalid text buffer\n");
    //    return;
    //}

    //#debug
    //printf("__test_text: {%s}\n",string_buffer);
    //while(1){}

/*
// ------------------
// Print into the window.
    p = string_buffer;
    gws_draw_text (
        (int) fd,      // fd
        (int) wid,     // window id
        (unsigned long)  8, 
        (unsigned long)  8, 
        (unsigned long) COLOR_RED,
        p );
*/
}

// #test
// Working on routine to load a file
// into the client area of the application window.
/*
static void __test_load_file(int socked_fd)
{
// #
// This is a work in progress!

    int fd = -1;
    //char *name = "init.ini";
    char *name = "fox.txt";
    file_status = FALSE;

//------------------------------------
    struct gws_window_info_d  lWi;

    if (socked_fd<0)
        return;

// Get info about the main window.
// IN: fd, wid, window info structure.
    gws_get_window_info ( 
        socked_fd, main_window, (struct gws_window_info_d *) &lWi );

    // Frame/chrome
    // frame_left = lWi.left;
    // frame_top = lWi.top;
    // frame_width = lWi.width;
    // frame_height = lWi.height;

    // Client area
    cr_left   = lWi.cr_left;
    cr_top    = lWi.cr_top;
    cr_width  = lWi.cr_width;
    cr_height = lWi.cr_height;

// ------------------------------------
// file

    fd = open( (char*) name, 0, "a+" );
    if (fd<0)
        return;

    //lseek(fd,0,SEEK_SET);
    int nreads=0;
    nreads = read(fd,file_buffer,511);
    if (nreads>0)
        file_status = TRUE;

// socket
    //if(socket<0)
        //return;

    //if(wid<0)
        //return;

    unsigned long x=0;
    unsigned long y=0;
    register int i=0;
    // Draw and refresh chars.
    for (i=0; i<nreads; i++)
    {
        //if ( isalnum( file_buffer[i] ) )
        if ( file_buffer[i] != 0 && 
             file_buffer[i] != '\n' )
        {
            // Draw one character
            libgui_drawchar_dc(
                dc00,
                cr_left + x,  // cursor_x
                cr_top  + y,  // cursor_y
                file_buffer[i],   // c
                COLOR_BLACK,      // fg_color
                COLOR_WHITE,      // bg_color
                0  // ROP
            );
        }

        x += 8;  // Next column
        if (x > (8*40))
        {
           x=0;
           y += 8;  // Next row.
        }

        if (file_buffer[i] == '\n')
        {
            x=0;
            y += 8;
        }
    };
}
*/

static void pump(int fd, int wid)
{
    struct gws_event_d lEvent;
    lEvent.used = FALSE;
    lEvent.magic = 0;
    lEvent.type = 0;
    //lEvent.long1 = 0;
    //lEvent.long2 = 0;

    struct gws_event_d *e;

    int target_wid = wid;

// Parameter
    if (fd<0){
        printf("pump: fd\n");
        return;
    }

// Target window
// The main window?
    if (target_wid<0){
        printf("pump: target_wid\n");
        return;
    }

// Pump
    e = 
        (struct gws_event_d *) gws_get_next_event(
                                   fd, 
                                   target_wid,
                                   (struct gws_event_d *) &lEvent );

    if ((void *) e == NULL)
        return;
    if (e->magic != 1234){
        return;
    }
    if (e->type < 0)
        return;
// Dispatch
    editorProcedure( fd, e->window, e->type, e->long1, e->long2 );
}

/*
static int __open_document(char *file_name)
{
    int fd = -1;
    char buf[1024];

    if ((void*)file_name == NULL)
        goto fail;

    //doc_fp = (FILE *) fopen(file_name, "r+");        
    //if (doc_fp == NULL){
    //    goto fail;
    //}
    //int fd = (int) fileno(doc_fp);

    fd = (int) open((char *) file_name, 0, "a+");
    if (fd < 0){
        printf ("__open_document: on open()\n");
        goto fail;
    }
    read(fd, buf, 512);

//
// Inject file
//

    //printf("%s",buf);

    return 0;  // OK

fail:
    printf("fail\n");
    return (int) -1;
}
*/

static int __open_document(char *file_name)
{
    int fd = -1;
    char buf[1024];
    int nreads = 0;
    int i;

    if ((void*)file_name == NULL)
        goto fail;

    fd = open((char *) file_name, 0, "r");  // open for reading
    if (fd < 0) {
        printf("__open_document: failed to open %s\n", file_name);
        goto fail;
    }

    // Read file into buffer
    nreads = read(fd, buf, sizeof(buf)-1);
    if (nreads <= 0) 
    {
        //close(fd);
        return -1;
    }
    buf[nreads] = '\0';  // null terminate

    //close(fd);

//
// Inject into text_buffer
//

    int line = 0;
    int col  = 0;

    for (i=0; i < nreads; i++) 
    {
        char ch = buf[i];

        if (ch == '\n') 
        {
            line++;
            col = 0;
            continue;
        }

        if (line >= text_buffer->line_count)
            break;

        editorInjectChar(text_buffer, line, col, ch);
        col++;

        if (col >= text_buffer->lines[line]->max_chars) 
        {
            line++;
            col = 0;
        }
    };

    EditorInitialization.file_loaded_at_initialization = TRUE;
    return 0; // success

fail:
    return (int) -1;
}


// Create a canvas_information_d from a dc
static struct canvas_information_d *
editorCreateCanvasInfo(struct dccanvas_d *dc, int owner_wid, int is_frame)
{
    if (!dc) return NULL;

    struct canvas_information_d *ci =
        (struct canvas_information_d *) malloc(sizeof(struct canvas_information_d));
    if (!ci) return NULL;

    ci->used        = TRUE;
    ci->magic       = 1234;
    ci->initialized = TRUE;
    ci->dirty       = TRUE;   // mark dirty so compositor will flush
    ci->is_frame    = is_frame;

    ci->width  = dc->device_width;
    ci->height = dc->device_height;
    ci->bpp    = dc->bpp;
    ci->pitch  = dc->pitch;
    ci->base   = dc->data;
    ci->dc     = dc;

    ci->owner_wid = owner_wid;

    return ci;
}


static int __editor_initialize_small_buffer(void)
{
    // Match the screen's width/bpp so refresh_rectangle1's
    // stride math (which uses libgui_SavedX/libgui_SavedBPP)
    // lines up with this buffer's real row size.
    unsigned long w   = dc00->device_width;  //libgui_SavedX;     // screen width, not 320
    unsigned long h   = 200;               // height can stay whatever you need
    unsigned long bpp = dc00->bpp;  //libgui_SavedBPP;   // screen bpp, not 24
    unsigned long bytes_per_pixel = bpp / 8;

    small_buffer = (unsigned char *) malloc(w * h * bytes_per_pixel);
    if (!small_buffer) return -1;

    small_dc = libgui_create_dc(small_buffer, w, h, bpp);
    if (!small_dc) return -1;

    struct canvas_information_d *ci_small =
    editorCreateCanvasInfo(small_dc, main_window, FALSE);

    libgui_canvasList[CANVAS_SMALL] = (unsigned long) ci_small;

    return 0;
}

// Local worker
static int __editor_initialize(void)
{
    const char *display_name_string = "display:name.0";
    int client_fd = -1;
    unsigned int client_area_color = COLOR_RED;  // Not implemented 
    unsigned int frame_color = COLOR_GRAY;

// Main window.
// w_width, w_height, w_left, w_top
    unsigned long w_width = 0;
    unsigned long w_height = 0;
    unsigned long w_left = 0;
    unsigned long w_top = 0;

    isTimeToQuit = FALSE;
// Screen
    gScreenWidth=0;
    gScreenHeight=0;
// Initialize WIDs.
    main_window = -1;
    //addressbar_window = -1;
    //savebutton_window = -1;
    //client_window = -1;
// Cursor
    cursor_x = 0;
    cursor_y = 0;
    cursor_x_max = 0;
    cursor_y_max = 0;
// Blink.
    blink_status=FALSE;

    //if (argc < 0)
        //return 1;

/*
// #test
// OK!
    int i=0;
    for (i = 1; i < argc; i++)
        if (strcmp("--test--", argv[i]) == 0)
            printf("TEST\n");
*/

// ============================
// Open display.
// IN: hostname:number.screen_number
    Display = (struct gws_display_d *) gws_open_display(display_name_string);
    if ((void*) Display == NULL){
        printf("editor.bin: Display\n");
        goto fail;
    }
// Get client socket
    client_fd = (int) Display->fd;
    if (client_fd <= 0){
        printf("editor.bin: client_fd\n");
        goto fail;
    }

// =====================================================

    // editor_init_globals();

// Device info
// #todo: Maybe all these information needs to be 
// available in the display structure we got early.
    unsigned long w = gws_get_system_metrics(1);
    unsigned long h = gws_get_system_metrics(2);
    if ( w == 0 || h == 0 )
    {
        printf("editor.bin: w h \n");
        goto fail;
    }

// -----------------------
// Width
    w_width = (w>>1);
    // #hack for 800
    if (w == 800){ w_width = 640; }
    // #hack for 640
    if (w == 640){ w_width = 480; }
    // #hack for 640
    if (w == 320){ w_width = w;   }

// -----------------------
// Height
    w_height = (h - 100);  //(h>>1);
    if (h == 200){ w_height = h; }

// -----------------------
// Left
    //w_left = ( ( w - w_width ) >> 1 );
    w_left = 10;
    if (w == 320)
        w_left = 0;

// -----------------------
// Top
    //w_top = ( ( h - w_height) >> 1 ); 
    w_top = 10;
    if (w == 320)
        w_top = 0;

// -----------------------
// Position and dimensions for 320x200. (Again)

    if (w == 320)
    {
        w_left = 0; 
        w_top = 0; 
        w_width = w; 
        w_height = h -32 -32;  //#todo: Get this offset.
    }

// Cursor limits based on the window size.
    cursor_x = 0;
    cursor_y = 0;
    cursor_x_max = ((w_width/8)  -1);
    cursor_y_max = ((w_height/8) -1);

// >> Status: interaction/activation. (int)
// Indicates focus, active/inactive, and user engagement.
    unsigned long mw_status = WINDOW_STATUS_ACTIVE;

// >> State: runtime condition. (int)
// Tracks current behavior (minimized, maximized, fullscreen, etc).
    unsigned long mw_state = WINDOW_STATE_NULL;

// >> Style: design-time identity. (unsigned long)
// Defines window type and decorations/features.
    unsigned long mw_style = WS_APP;

// Create main window
    main_window = 
        (int) gws_create_window (
                  client_fd,
                  WT_OVERLAPPED,
                  mw_status, 
                  mw_state,
                  program_name, 
                  w_left, w_top, w_width, w_height,
                  0,   // Parent wid
                  mw_style, 
                  client_area_color,
                  frame_color );

    if (main_window < 0){
        printf("editor.bin: main_window failed\n");
        goto fail;
    }

// Label
// Text inside the main window.
// Right below the title bar.
// Right above the client window.

/*
    text1_l = 2;
    text1_t = 4 + (24/3);
    text1_color = COLOR_BLACK;
*/

// -----------------------------

// Get info about the main window.
// IN: fd, wid, window info structure.

    struct gws_window_info_d  lWi;  // Local
    gws_get_window_info(
        client_fd, 
        main_window,
        (struct gws_window_info_d *) &lWi );


    // Frame/chrome
    frame_left = lWi.left;
    frame_top = lWi.top;
    frame_width = lWi.width;
    frame_height = lWi.height;

    // Client area
    cr_left = lWi.cr_left;
    cr_top = lWi.cr_top;
    cr_width = lWi.cr_width;
    cr_height = lWi.cr_height;

//
// Draw text
//

    text1_l = 2;
    text1_t = 4 + (24/3);
    text1_color = COLOR_BLACK;

// ============================================================
// #test
// Update the wproxy structure that belongs to this thread.

    unsigned long m[10];
    int mytid = gettid();
    m[0] = (unsigned long) (mytid & 0xFFFFFFFF);

    // Frame/chrome rectangle
    m[1] = lWi.left;
    m[2] = lWi.top;
    m[3] = lWi.width;
    m[4] = lWi.height;

    // Client area rectangle
    m[5] = lWi.cr_left;
    m[6] = lWi.cr_top;
    m[7] = lWi.cr_width;
    m[8] = lWi.cr_height;

    sc80( 48, &m[0], &m[0], &m[0] );

// ----------------------------------------

//
// dc
//

    dc00 = (struct dccanvas_d *) libgui_create_dc(
        lWi.ca_canvas_base_address,
        lWi.ca_canvas_width,
        lWi.ca_canvas_height,
        lWi.ca_canvas_bpp
    );
    if ((void*)dc00 == NULL){
        printf("power: on dc00\n");
        exit(1);
    }

//
// Background for the client area
//

    lingui_draw_rectangle0_dc (
        dc00,
        0, 0, lWi.cr_width, lWi.cr_height,
        COLOR_GRAY,
        0  // ROP
    );


//
// Address bar
//

// Address bar - (edit box)
// Inside the main window.
// se a janela mae é overlapped, 
// então estamos relativos à sua área de cliente.

    unsigned long ab_l = (( lWi.cr_width/8 )*2);
    unsigned long ab_t = 4;
    unsigned long ab_w = (( lWi.cr_width/8 )*3);
    unsigned long ab_h = 24;


//
// Draw address bar rectangle
//

    lingui_draw_rectangle0_dc (
        dc00,
        ab_l, ab_t, ab_w, ab_h,
        COLOR_WHITE,
        0  // ROP
    );
 
    // Save
    cwAddressBar.l = (( lWi.cr_width/8 )*2);
    cwAddressBar.t = 4;
    cwAddressBar.w = (( lWi.cr_width/8 )*3);
    cwAddressBar.h = 24;

// The [Save] button.
// inside the main window.

    // #test
    // The 'button state' is the same of window status.

// ============================================================
// Create restart button

    MyButton_Save.button_id = 1;   // arbitrary ID
    // MyButton_Save.wid     = main_window; // parent window ID

    // Relative values
    MyButton_Save.left = (( lWi.cr_width/8 )*6); //4;
    MyButton_Save.top  = 4;

    // Absolute coordinates (relative to screen)
    MyButton_Save.absolute_left = lWi.left + lWi.cr_left + MyButton_Save.left;
    MyButton_Save.absolute_top  = lWi.top + lWi.cr_top + MyButton_Save.top;
    MyButton_Save.width         = (( lWi.cr_width/8 )*1);
    MyButton_Save.height        = 24;

    // MyButton_Save.state = 0;  // Initial state

// Create a button
    uic_button_save = 
        libgui_create_ui_component (
            dc00, 
            1,   // type = button 
            MyButton_Save.left, 
            MyButton_Save.top, 
            MyButton_Save.width, 
            MyButton_Save.height,
            "Save"
        );

//
// == Client window =======================
//

// #todo
// Here is the moment where we're gonna 
// draw the content of the input file into the 
// client window.
// >> We're gonna save the file content into a local buffer,
// large enough for the whole file.
// >> We're gonna send parts of this file to the display server,
// and the server will save it into the text buffer 
// in the window structure.

// (Editbox)
// Client window (White)
// Inside the mainwindow.
// Lembre-se que temos uma status bar.

// left:
    unsigned long cw_left = 0;
// top: pad | address bar | pad
    unsigned long cw_top =  (cwAddressBar.t + cwAddressBar.h + 4);
// width: Width - borders.
    unsigned long cw_width = (lWi.cr_width);
// height:
// #bugbug:
// We gotta get the client window values.
    unsigned long cw_height = (lWi.cr_height - cw_top);


// Rectangle for the editor's client area.
// This is the 'text area'.
    lingui_draw_rectangle0_dc (
        dc00,
        cw_left, cw_top, cw_width, cw_height,
        COLOR_WHITE,
        0  // ROP
    );

    // Save
    cwText.l = 0;
    cwText.t = (cwAddressBar.t + cwAddressBar.h + 4);
    cwText.w = lWi.cr_width;
    cwText.h = (lWi.cr_height - cwText.t);

// ============================================

// Draw buffer's content
    if (EditorInitialization.file_loaded_at_initialization == TRUE)
    {
        editorRedrawBuffer(dc00);
    }

// Draw the status bar
    editorDrawStatusBar();

// ============================================
// Register canvases in the list
    struct canvas_information_d *ci_client =
    editorCreateCanvasInfo(dc00, main_window, TRUE);

    libgui_canvasList[CANVAS_CLIENTAREA] = (unsigned long) ci_client;  // client area canvas

// Create a small buffer
    int b_status = -1;
    b_status = (int) __editor_initialize_small_buffer();
    if (b_status < 0){
        printf("on __editor_initialize_small_buffer()\n");
        exit (1);
    }

// Draw something inside the small buffer and blit.
    editorDrawInSmallBuffer();

// ============================================
    gws_set_active( client_fd, main_window );
    gws_set_focus( client_fd, main_window );
// ============================================


//
// Event loop
//

// ===========================================
// #test
// Get input from stdin

    int C=0;

    rtl_focus_on_this_thread();
    // GRAMADO_SEEK_CLEAR
    lseek( fileno(stdin), 0, 1000);
    // Atualiza as coisas em ring3 e ring0.
    rewind(stdin);

// #test
// IN: service number, sub-service.
// Lets say to the kernel that we want to receive the TAB event.
    rtl_msgctl(2000,2000);
// Lets say to the kernel that we want to receive the ESCAPE event.
    rtl_msgctl(2002,2002);


/*
// ================================
// #test
// Lets setup if we want to block on empty queue or not
// #todo: Create msgctl() api

    int rv = -1;
    rv = (int) sc80( 912, 1000, 1000, 1000 );  // Yes
    //rv = (int) sc80( 912, 1001, 1001, 1001 );  // No
    if (rv < 0){
        printf ("on sc80:912\n");
        exit(0);
    }
*/

    int nSysMsg = 0;

    while (1)
    {
        if (isTimeToQuit == TRUE)
            break;

        // 1. Pump events from Display Server
        pump(client_fd, main_window);

        // 2. Pump events from Input Broker (system events)
        for (nSysMsg=0; nSysMsg<32; nSysMsg++){
        if (rtl_get_event() == TRUE)
        {
            editorProcedure(
                client_fd,
                (int) RTLEventBuffer[0],   // window id
                (int) RTLEventBuffer[1],   // event type (MSG_SYSKEYDOWN, MSG_SYSKEYUP, etc.)
                (unsigned long) RTLEventBuffer[2], // VK code
                (unsigned long) RTLEventBuffer[3]  // scancode
            );
            RTLEventBuffer[1] = 0;
        }
        };
    };


/*
    while (1)
    {
        if (isTimeToQuit == TRUE)
            break;

        // It needs to be the main window for now.
        // Calls gws_get_next_event() to fetch the next event from the DS.
        // And dispatch it to the procedure.
        pump( client_fd, main_window );

        C = fgetc(stdin);
        if (C > 0)
        {
            editorProcedure ( 
                client_fd,    // socket
                client_window,    // window ID
                MSG_KEYDOWN,  // message code
                C,            // long1 (ascii)
                C );          // long2 (ascii)
        }

    };
*/

// ===========================================

// loop
// The server will return an event from the client's event queue.
// Call the local window procedure if a valid event was found.
// #todo: 
// Por enquanto, a rotina no servidor somente lida com 
// eventos na janela com foco de entrada.
// Talvez a ideia é lidar com eventos em todas as janelas
// do processo cliente.

    //Display->running = TRUE;

// Getting the asynchronous events 
// from the window server via socket.
// Processing this events.

/*
    while (1)
    {
        //if ( Display->running != TRUE )
            //break;
        if (isTimeToQuit == TRUE)
            break;

        // It needs to be the main window for now.
        pump( client_fd, main_window );
    };
*/

// ok
    if (isTimeToQuit == TRUE){
        printf("editor.bin: isTimeToQuit\n");
        editorShutdown(client_fd);
        return EXIT_SUCCESS;
    }

// Hang
    printf("editor.bin: main loop failedn");
    while (1){
    };


/*
    int C=0;
    //char data[2];
    //int nread=0;

    //fputc('A',stdin);
    //fputs("This is a string in stdin",stdin);

    rewind(stdin);

    while (1){
        C=fgetc(stdin);
        if(C>0){
            editorProcedure( 
                client_fd,     // socket
                NULL,          // opaque window object
                MSG_KEYDOWN,   // message code
                C,             // long1 (ascii)
                C );           // long2 (ascii)
        }
    };
*/

//==============================================


//
// loop
//

/*
//=================================
// Set foreground thread.
// Get events scanning a queue in the foreground queue.
    rtl_focus_on_this_thread();
    
    while (1){
        if ( rtl_get_event() == TRUE )
        {  
            editorProcedure( 
                client_fd,
                (void*) RTLEventBuffer[0], 
                RTLEventBuffer[1], 
                RTLEventBuffer[2], 
                RTLEventBuffer[3] );
        }
    };

//=================================
*/

// Done
    //close(client_fd);
    printf("editor: exit 0\n");
    return EXIT_SUCCESS;

fail:
    return EXIT_FAILURE;
}

// Called by main() in main.c
int editor_initialize(int argc, char *argv[])
{
    int status = -1;

    EditorInitialization.initialized = FALSE;
    EditorInitialization.file_loaded_at_initialization = FALSE;

    //if (argc < 0)
        // return EXIT_FAILURE;

// #test
// Initializing the text buffer

    text_buffer = (struct text_buffer_d *) editorInitBuffer();
    if ((void *) text_buffer == NULL){
        printf("editor: on editorInitBuffer()\n");
        goto fail;
    }


//
// document
//

    // #test
    // #todo: Save it into a proper buffer
    // We need a structure to handle the document
    // loaded by the editor.

    if (argc >= 2){
        __open_document( (char *) argv[1] );
        //while(1){}
    }

//
// Initialize editor
//

    __editor_initialize();

    return 0;
fail:
    return 1;
}

//
// End
//

