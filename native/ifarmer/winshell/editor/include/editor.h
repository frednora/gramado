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

// ===========================================================

int editor_initialize(int argc, char *argv[]);

#endif    

