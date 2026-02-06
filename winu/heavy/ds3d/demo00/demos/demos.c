// demos.c
// Commom for all demos.


#include "../gram3d.h"

#include <fcntl.h>
#include <unistd.h>


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


// Process key combinations only for demos 
int demos_on_combination(int msg_code)
{
    int char_id = 0;  // Target char
    int direction = 0;
    float value = 0.2f;  //0.08f;

    // #todo
    // Return is the demo is not running

    if (gamestate != GS_LEVEL)
        goto fail;

// Parameters:
    if (msg_code < 0)
        goto fail;

//
// Control + arrows
//

    switch (msg_code){
    case GWS_ControlArrowLeft:   direction = DIRECTION_LEFT;  break;
    case GWS_ControlArrowRight:  direction = DIRECTION_RIGHT; break;
    case GWS_ControlArrowUp:     direction = DIRECTION_FRONT; break;
    case GWS_ControlArrowDown:   direction = DIRECTION_BACK;  break;
    default:
        goto fail;
        break;
    };

    demoHumanoidMoveCharacter( char_id, direction, (float) value );
    return 0;

fail:
    return (int) -1;
}

// Function to read entire file into buffer using open()
// # Limited size. 512 bytes
char *demosReadFileIntoBuffer(const char *filename) 
{
    //size_t FakeFileSize=512;
    //size_t FakeFileSize=512*4;
    size_t FakeFileSize = 1024 -1;
    int fd;

    if ((void*) filename == NULL)
        return NULL;
    if (*filename == 0)
        return NULL;

// --------------------------
// Open
    fd = open(filename,0,"a+");
    if (fd < 0) {
        printf("File opening failed\n");
        return NULL;
    }

    // Get file size
    //off_t size = lseek(fd, 0, SEEK_END);
    //lseek(fd, 0, SEEK_SET); // Reset file position

    // Allocate buffer
    char *buffer = malloc(FakeFileSize);
    if (!buffer) 
    {
        printf("Memory allocation failed\n");
        //close(fd);
        return NULL;
    }

    // Read file content
    memset(buffer,0,FakeFileSize);

// --------------------------
// Read
    ssize_t bytesRead = read(fd, buffer, FakeFileSize);
    buffer[bytesRead] = '\0'; // Null-terminate

    //close(fd); // Close file descriptor

// Return 
    return (char *) buffer;
}

// Worker called by the demos.
struct gws_window_d *__create_demo_window (
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height )
{
    struct gws_window_d *w;

    if ((void*) __root_window == NULL){
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
                                    __root_window, 
                                    0, 
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


