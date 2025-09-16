// service.h
// The services called by the dispacher.
// Created by Fred Nora.

// This is a list of actions.
// A route maps a request to a controller action.
// A route is an entity that links a request with an action.
// In our case, dsProcedure() is that route, that makes the connections 
// of the request with the controller action.
// Actions are provided by controllers and these actions 
// handles the objects, that are windows or menus.
// The handlers for the requests.
// In main.c we have the function dispatcher() that calls dsProcedure(),
// the function dsProcedure() will call all the handlers exposed in this header.
// All the functions for the servers start with the prefix serviceXXXX().

// See:
//   protocol.h = The indexes for the services.
//   dispatch.h = The prototypes for the handlers.
//   main.c     = Call all the services.
//   grprim.c   = some low level services.
// ...

#ifndef __SERVICE_H
#define __SERVICE_H    1

// 1000
// hello

int serviceHello(void);

// 1001
// Create window.
// See: main.c
int serviceCreateWindow(int client_fd);

// 1002
// Put pixel in the backbuffer.
int servicepixelBackBufferPutpixel(void);

// 1003
// Draw horizontal line
int servicelineBackbufferDrawHorizontalLine(void);

// 1004
// Draw char
// See: main.c
int serviceDrawChar(void);

// 1005
// Draw text
int serviceDrawText(void);

// 1006
// Refresh window
int serviceRefreshWindow(void);

// 1007
// Redraw a window
int serviceRedrawWindow(void);

// 1008
// Resize a window
int serviceResizeWindow(void);

// 1009
// Change window position
int serviceChangeWindowPosition(void);

// --------------

// 2000
// Paint a pixel in the backbuffer. (Second implementation!)
// See: libdisp_backbuffer_putpixel()

// 2010
// Disconnect

// 2020
// Refresh screen
// See: gws_show_backbuffer()

// 2021
// Refresh a rectangle
int serviceRefreshRectangle(void);

// 2030 ?
// When a client send us an event
int serviceClientEvent(void);

// 2031
// Get next event
// When a client get the next event from it's own queue.
// See: main.c
int serviceNextEvent(void);


// 2040
int serviceGrPlot0(void);

// 2041 (deprecated)
int serviceGrCubeZ(void);

// 2042 (deprecated)
int serviceGrRectangle(void);

// --------------

// 2222
// See: main.c
int serviceAsyncCommand(void);

// 2222: service 1
void serviceExitGWS(void);

// 2223
// Put a message into the client's queue.
int servicePutClientMessage(void);

// 2224
// Get a message from the client's queue.
int serviceGetClientMessage(void);

// 2240
// Set a text into a buffer in the window structure.
int serviceSetText(void);

// 2241
// Get a text from a buffer in the window structure.
int serviceGetText(void); 

// --------------

// 4080
// Quit the process if it's possible.

// --------------

// 8080
// Drawin input

// --------------

// 9093
// Get information about a given window.
// See: main.c
int serviceGetWindowInfo(void);

// 9099
// Clone this process and execute the child, 
// given the image name.
void serviceCloneAndExecute(void);


// --------------

// ??
// Button
// Deprecated?
// See: main.c
int serviceDrawButton(void); 

// ??
// Get a message given the Index and the Restart flag.
// Deprecated?
// See: main.c
int serviceNextEvent2(void);


#endif    

