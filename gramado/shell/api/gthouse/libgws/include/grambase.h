// grambase.h
// System components, not user interactive components.
// Created by Fred Nora.

#ifndef __LIBGWS_GRAMBASE_H_
#define __LIBGWS_GRAMBASE_H_    1


// This is a wrapper for the main systemcall
// in rtl.
void *gws_system_call ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d );

// Debug via serial port. (COM1)
void gws_debug_print(const char *string);

// Initialize the library.
int gws_initialize_library(void);

void *gws_malloc(size_t size);
void gws_free(void *ptr);

// System services.
// Dialog with the library
// IN: service number, ...
void *gws_services ( 
    int service,
    unsigned long arg2,
    unsigned long arg3,
    unsigned long arg4 );

// Send system message to a process.
int
gws_send_message_to_process ( 
    int pid, 
    int window, 
    int message,
    unsigned long long1,
    unsigned long long2 );

// Send system message to a thread.
int 
gws_send_message_to_thread ( 
    int tid, 
    int window, 
    int message,
    unsigned long long1,
    unsigned long long2 ); 

//
// System stuff
//

void gws_reboot(int fd);
void gws_shutdown(int fd);

//
// Thread support
//

// Yield the current thread.
void gws_yield(void);
// Refresh the background and yield the current thread.
void gws_refresh_yield (int fd);
// Refresh a given window and yield the current thread.
void gws_refresh_yield2 (int fd, int window);
// Yield the current thread n times.
void gws_yield_n_times (unsigned long n);

void *gws_create_thread ( 
    unsigned long init_rip, 
    unsigned long init_stack, 
    char *name );

void gws_start_thread (void *thread);

//
// Child process support.
//

int gws_clone_and_execute(const char *name);

//#todo: change arguments.
int 
gws_clone_and_execute2 (
    int fd, 
    unsigned long arg1,
    unsigned long arg2,
    unsigned long arg3,
    unsigned long arg4,
    const char *string );

void gws_clone_and_execute_from_prompt(int fd);

//
// System support.
//

unsigned long gws_get_system_metrics (int index);

//
// Critical section support
//

void gws_enter_critical_section(void);
void gws_exit_critical_section(void);

//
// File support
//

// #test 
// Load a file given a path.
// We are testing the path support.
int 
gws_load_path ( 
    const char *path, 
    unsigned long buffer, 
    unsigned long buffer_len );

int gws_create_empty_directory(const char *dir_name);
int gws_create_empty_file(const char *file_name);

// 
// Application
//

int application_start(void);
void application_end(void);
// ==========================

// Input support.
int gws_enable_input_method(int method);

#endif  
