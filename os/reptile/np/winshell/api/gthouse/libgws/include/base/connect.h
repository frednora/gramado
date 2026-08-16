// connect.c
// Created by Fred Nora.

// #todo: Some connections routines for libgws.
// This header should be included by including "gws.h".
// Os codigos da libgws vão precisar disso 
// pra se conectarem com o Display Server. (Gramland)

#ifndef __LIBGWS_CONNECT_H_
#define __LIBGWS_CONNECT_H_    1

int gws_connect (void);

int gws_get_connect_status(void);
void gws_set_connect_status(int status);

int gws_get_client_fd(void);
void gws_set_client_fd(int client_id);

int gws_get_desktop_id(void);
void gws_set_desktop_id(int desktop_id);

int gws_initialize_connection(void);
unsigned long gws_get_message_buffer(void);

#endif   

