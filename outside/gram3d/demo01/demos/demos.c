// demos.c
// Commom for all demos.


#include "../gram3d.h"


// Use demos or not
int gUseDemos = TRUE;

// The demos window. This is the canvas for demos.
struct gws_window_d *__demo_window;

// Global counter used by the demos.
unsigned long gBeginTick=0;

// Example main routine to test the multi-line loader.
#define SEQUENCE_MAX_ELEMENTS  48
//static int sequence[3*16];  //cube
static int sequence[512];  //cube
static char model_file_buffer[512];


// ===================================

// Revised function: Loads a multi‑line sequence file using scan00_custom_read_float()
// Each line may have one or more numbers.
// The parsed numbers (as floats) are cast to int and stored into the sequence array.
int load_sequence_multiline(int *sequence, int max_elements) 
{

    /*
    // Open the file using open()
    //int fd = open(filename, O_RDONLY);
    int fd = open(filename, 0, "a+");

    if (fd < 0) {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }
    
    off_t fsize = 512;
    // Determine the file size using lseek()
    //off_t fsize = lseek(fd, 0, SEEK_END);
    //if (fsize == -1) {
    //    printf("Error: Could not determine file size.\n");
        //close(fd);
    //    return -1;
    //}
    // Reset file pointer to beginning.
    //lseek(fd, 0, SEEK_SET);
    
    // Allocate a buffer and read the entire file.
    //char *buffer = (char *)malloc(fsize + 1);
    char *buffer = (char *)malloc(fsize);
    if (buffer == NULL) 
    {
        printf("Error: Could not allocate buffer memory of size %ld\n", (long)fsize);
        //close(fd);
        return -1;
    }

    //ssize_t bytesRead = read(fd, buffer, fsize);
    ssize_t bytesRead = read(fd, buffer, fsize);
    if (bytesRead < 0) {
        printf("Error: Could not read from file.\n");
        //free(buffer);
        //close(fd);
        return -1;
    }
    buffer[bytesRead] = '\0';
    //close(fd);

*/

    // Now, use strtok_r() to split the buffer into lines.
    char *saveptr = NULL;
    char *line = strtok_r(model_file_buffer, "\n", &saveptr);
    int count = 0;
    
    while (line != NULL && count < max_elements) 
    {
        // For each line, prepare a pointer for parsing numbers.
        const char *p = line;
        
        // Use scan00_custom_read_float() as long as there is something in the line.
        while (*p != '\0' && count < max_elements) 
        {
            // Skip any whitespace if needed (scan00_custom_read_float takes care of this).
            float value = (float) scan00_custom_read_float(&p);
            // If you wish, you can add a check here to ensure a valid number was parsed.
            sequence[count] = (int) value;

            //#debug ok
            //printf("%d\n",sequence[count]);

            count++;
        }
        line = strtok_r(NULL, "\n", &saveptr);
    }

    //while(1){}

    //free(buffer);
    return count;
}


/*
// How to use the function load_sequence_multiline()

    int numElements = load_sequence_multiline(sequence, SEQUENCE_MAX_ELEMENTS);
    if (numElements < 0) {
        printf("numElements\n");
        exit(0);
    }

*/


struct gws_window_d *__create_demo_window (
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height )
{
    struct gws_window_d *w;

    if ( (void*) __root_window == NULL ){
        return NULL;
    }

// Create window

    w = 
        (struct gws_window_d *) CreateWindow ( 
                                    WT_SIMPLE, 
                                    0, //style
                                    1, //status 
                                    1, //view
                                    "DemoWin",  
                                    left, top, width, height,   
                                    __root_window, 0, 
                                    COLOR_BLACK, 
                                    COLOR_BLACK );

    if ( (void *) w == NULL ){
        return NULL;
    }
    if ( w->used != TRUE ||  w->magic != 1234 ){
        return NULL;
    }

// Register the window.
    int WindowId= -1;
    WindowId = (int) RegisterWindow(w);
    if (WindowId < 0)
    {
        return NULL;
    }

// ok
    return (struct gws_window_d *)  w;
}


