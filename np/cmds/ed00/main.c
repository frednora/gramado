// ED00.BIN - Simple Text Editor for Gramado OS
// Lightweight version - suitable for kernel console

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES     256
#define MAX_LINE_LEN  192

char *text[MAX_LINES];
int num_lines = 0;
int cursor_x = 0;
int cursor_y = 0;
char filename[128] = {0};
int modified = 0;

void redraw(void);
void save_file(void);
void load_file(const char *fname);

// Simple clear (no ANSI)
// #bugbug: This is valid only for the case
// we are running on the kernel console.
// The Problem:
// On the kernel console, you can clear the full screen 
// with ioctl(1, 440, 0).
// On the GUI virtual terminal, you don't want to clear 
// the entire screen — you want to clear only the client area 
// of the terminal window.
// So you need to intercept escape sequences 
// (like \033[2J for clear screen) and handle them 
// appropriately depending on the environment.

static void clear_screen(void)
{
    // Use your existing method
    ioctl(1, 440, 0);   // Clear background

    // #todo:
    // Send escape sequence to the virtual terminal
}

void load_file(const char *fname)
{
    FILE *f;
    char line[MAX_LINE_LEN];

    if (!fname || !*fname) return;

    f = fopen(fname, "r");
    if (!f) {
        printf("New file: %s\n", fname);
        strncpy(filename, fname, sizeof(filename)-1);
        return;
    }

    strncpy(filename, fname, sizeof(filename)-1);
    num_lines = 0;

    while (num_lines < MAX_LINES && fgets(line, sizeof(line), f))
    {
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';

        text[num_lines] = strdup(line);
        if (!text[num_lines]) break;
        num_lines++;
    }

    fclose(f);
    printf("Loaded: %s (%d lines)\n", filename, num_lines);
}

void save_file(void)
{
    int i;
    FILE *f;

    if (!filename[0]) {
        printf("No filename!\n");
        return;
    }

    f = fopen(filename, "w");
    if (!f) {
        printf("Cannot save!\n");
        return;
    }

    for (i = 0; i < num_lines; i++) {
        if (text[i])
            fprintf(f, "%s\n", text[i]);
        else
            fprintf(f, "\n");
    }

    fclose(f);
    modified = 0;
    printf("Saved: %s\n", filename);
}

void redraw(void)
{
    int i=0;
    int start=0;

    clear_screen();

    if (cursor_y < 0) cursor_y = 0;
    if (num_lines < 0) num_lines = 0;

    start = cursor_y - 8;
    if (start < 0) start = 0;

    // Draw visible lines
    for (i = start; i < start + 16 && i < num_lines; i++)
    {
        if (i >= 0 && i < MAX_LINES && text[i] != NULL)
            printf("%s\n", text[i]);
        else
            printf("\n");
    }

    // Status bar
    printf("\n--- ");
    printf("%s", filename[0] ? filename : "[No Name]");
    if (modified) printf(" [Modified]");
    printf(" | Line %d/%d | Col %d ---\n",
           cursor_y + 1, num_lines, cursor_x + 1);

    fflush(stdout);
}



// ======================
// Initialization
// ======================
static void editor_init(void)
{
    int i;
    for (i = 0; i < MAX_LINES; i++)
        text[i] = NULL;

    num_lines = 0;
    cursor_x = 0;
    cursor_y = 0;
    modified = 0;
    filename[0] = '\0';
}


int main(int argc, char *argv[])
{
    editor_init();        // Safe initialization

    printf("=== Gramado Simple Editor ===\n");

    if (argc > 1) {
        load_file(argv[1]);
    }

    char buf[4];

    while (1)
    {
        redraw();

        while (1){
            if (read(0, buf, 1) > 0)
                break;
        }

        int ch = (int) buf[0];
    
        // ... rest of your input handling ...

        if (ch == ':')        // Command mode
        {
            char cmd[64] = {0};
            fgets(cmd, sizeof(cmd), stdin);

            if (cmd[0] == 'w') {
                if (cmd[1] == ' ' && cmd[2])
                    strncpy(filename, cmd+2, sizeof(filename)-1);
                save_file();
            }
            else if (cmd[0] == 'q') {
                if (modified && cmd[1] != '!') {
                    printf("File modified. Use :q! to quit\n");
                } else {
                    break;
                }
            }
        }
        else if (ch == '\n' || ch == '\r')
        {
            cursor_y++;
            cursor_x = 0;
            if (cursor_y >= num_lines) {
                text[num_lines] = strdup("");
                num_lines++;
                modified = 1;
            }
        }
        else if (ch == '\b' || ch == 127)   // Backspace
        {
            // Simple backspace handling for now
            if (cursor_x > 0) cursor_x--;
            modified = 1;
        }
        else if (ch >= 32 && ch < 127)   // Printable character
        {
            if (cursor_y >= MAX_LINES) {
                printf("Maximum lines reached!\n");
                continue;
            }

            // Create line if it doesn't exist
            if (text[cursor_y] == NULL) {
                text[cursor_y] = (char*) malloc(MAX_LINE_LEN);
                if (text[cursor_y] == NULL) {
                    printf("Out of memory!\n");
                    continue;
                }
                text[cursor_y][0] = '\0';
                if (cursor_y >= num_lines)
                    num_lines = cursor_y + 1;
            }

            size_t len = strlen(text[cursor_y]);

            if (len < MAX_LINE_LEN - 2) 
            {
                // Shift to insert character
                size_t j=0;
                for (j = len; j > (size_t)cursor_x; j--) {
                    text[cursor_y][j] = text[cursor_y][j-1];
                }
                text[cursor_y][cursor_x] = (char)ch;
                cursor_x++;
                modified = 1;
            }
        }

        /*
        else if (ch >= 32 && ch < 127)
        {
            // Insert character (very basic)
            if (cursor_y < MAX_LINES) {
                if (!text[cursor_y])
                    text[cursor_y] = strdup("");
                if (text[cursor_y]) {
                    size_t len = strlen(text[cursor_y]);
                    if (len < MAX_LINE_LEN - 2) {
                        text[cursor_y][cursor_x] = ch;
                        cursor_x++;
                        modified = 1;
                    }
                }
            }
        }
        */
    }

    printf("\nEditor closed.\n");
    return 0;
}

