// editor.h


#ifndef __EDITOR_H
#define __EDITOR_H    1

// rtl
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

// The client-side library
#include <gws.h>

// #test
// The client-side library
#include <libgui.h>

// Internal
#include <packet.h>


// ...
#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)



// CharCell  
// Represents a single character plus its attributes 
// (colors, style, etc.).

struct char_cell_d {
    char ch;              // The character
    unsigned int fg;      // Foreground color
    unsigned int bg;      // Background color
    unsigned int attr;    // Extra attributes (bold, underline, etc.)
};

// LineBuffer  
// Represents one line of text as a sequence of char_cell.

struct line_buffer_d {
    int char_count;             // Number of characters used
    int max_chars;              // Max characters
    struct char_cell_d *cells;  // Dynamic array of char_cell
};

// TextBuffer  
// Represents the whole document as a set of lines.

struct text_buffer_d {
    int line_count;                // Number of lines
    int max_lines;                 // Capacity
    struct line_buffer_d **lines;  // Array of pointers to line_buffer
};

#define DEFAULT_COLS  80
#define DEFAULT_ROWS  25

extern struct text_buffer_d *text_buffer;

// Represents the current document being edited
struct doc_d 
{
    char *file_name;           // Path to the file
    int is_open;               // Flag: file opened successfully
    int is_dirty;              // Flag: buffer modified since last save
    struct text_buffer_d *tb;  // Pointer to the text buffer
};

// Represents initialization options for the editor
struct editor_initialization_d 
{
    int initialized;
    int file_loaded_at_initialization;

    //int load_file;             // Flag: load file at startup
    //char *initial_file_name;   // File to load if flag is set
};
extern struct editor_initialization_d EditorInitialization;


// ===========================================================

int editor_initialize(int argc, char *argv[]);

#endif    

