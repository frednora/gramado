// config.h
// Configuration
// Created by Fred Nora.

#ifndef __CONFIG_H
#define __CONFIG_H    1

// Plataform
#define PLATFORM_GRAMADO  1
//#define PLATFORM_WINDOWS  1
//#define PLATFORM_LINUX  1
// ...

// Wallpaper.
#define CONFIG_USE_WALLPAPER  0
// Notification area, at the top.
#define CONFIG_USE_NA  1
// Taskbar at the bottom.
#define CONFIG_USE_TASKBAR  1
// see: wm.c
// Tile window manager.
#define CONFIG_USE_TILE  1


// Call a kernel service to draw rectangles.
// see: rect.c
// #test: It also work withour using the kernel.
#define USE_KERNEL_TO_DRAW_RECTANGLES  1

#define CONFIG_TEST_SPARE_BUFFER  0


// Target interval (in milliseconds) for each iteration of the main loop.
// The main loop is responsible for orchestrating all core tasks:
// - Processing requests from the client-side GUI application
// - Handling input events from the operating system
// - Rendering and displaying the desktop scene
//
// A value of X ms corresponds to approximately Y iterations per second,
// calculated as (1000 / X ≈ Y Hz).
// For example:
// - 16 ms → ~62.5 iterations per second (~60 Hz), 
//   balancing smooth responsiveness with efficient CPU usage
// - 22 ms → ~45 iterations per second (~45 Hz), 
//   reducing CPU load at the cost of responsiveness
//
// After each iteration, the thread sleeps until the next scheduled cycle.

//11 good
//16 ms (~60 Hz): Normal
//33 ms (~30 Hz):

#define CONFIG_MAIN_LOOP_INTERVAL_MS 8
//#define CONFIG_MAIN_LOOP_INTERVAL_MS 11
//#define CONFIG_MAIN_LOOP_INTERVAL_MS 16    // <<< Normal
//#define CONFIG_MAIN_LOOP_INTERVAL_MS 22
//#define CONFIG_MAIN_LOOP_INTERVAL_MS 33

#endif    

