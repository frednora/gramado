// async.h - (server-side)
// Indexes used for asynchronous requests to the display server.
// It supports the service: serviceAsyncCommand() in main.c
// + Request numbers.
// + Sub-request numbers.
// Created by Fred Nora.

#ifndef __API_ASYNC_H
#define __API_ASYNC_H    1

// Shutdown the server.
#define ASYNC_REQUEST_EXIT  1
#define ASYNC_REQUEST_HELLO2  2
#define ASYNC_REQUEST_HELLO3  3
#define ASYNC_REQUEST_START_ANIMATION  4  
// 5
#define ASYNC_REQUEST_FPS_FLAG  6  
// 7
// 8
#define ASYNC_REQUEST_SET_FOCUS_BY_WID  9
// 10
// 11
#define ASYNC_REQUEST_SWITCH_ACTIVE_WINDOW  12 
#define ASYNC_REQUEST_INVALIDATE_WINDOW_BY_WID  13
#define ASYNC_REQUEST_CLEAR_WINDOW_BY_WID  14
#define ASYNC_REQUEST_SET_ACTIVE_WINDOW_BY_WID  15
// ...
#define ASYNC_REQUEST_LAUNCH_SHUTDOWN  22
// ...
// 30 - update desktop
// ...
#define ASYNC_REQUEST_ENABLE_PS2_MOUSE  44
// ...
#define ASYNC_REQUEST_QUIT  88
#define ASYNC_REQUEST_REBOOT  89
#define ASYNC_REQUEST_DESTROY_WINDOW  90
// 91
// 92 - dock
#define ASYNC_REQUEST_DOCK_WINDOW             92
// 93 - Dock the active window
#define ASYNC_REQUEST_DOCK_ACTIVE_WINDOW      93
// ...
#define ASYNC_REQUEST_PUT_PIXEL  1000
// ...
// 1010 - DestroyAllWindows
#define ASYNC_REQUEST_DESTROY_ALL_WINDOWS    1010
// 1011 - MinimizeAllWindows
#define ASYNC_REQUEST_MINIMIZE_ALL_WINDOWS   1011
// 1012 - MaximizeAllWindows
#define ASYNC_REQUEST_MAXIMIZE_ALL_WINDOWS   1012
// 1013 - RestoreAllWindows
#define ASYNC_REQUEST_RESTORE_ALL_WINDOWS    1013
// 1014 - maximize active window
#define ASYNC_REQUEST_MAXIMIZE_ACTIVE_WINDOWS  1014
// ...
// 2000
// #define ASYNC_REQUEST_MULTIPLE_DATA          2000
//

#endif   

