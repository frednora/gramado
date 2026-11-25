// nports.h
// Network ports support.
// Created by Fred Nora.

#ifndef __NET_NPORTS_H
#define __NET_NPORTS_H    1

// Application services can subscribe to one or more port(s) 
// to be notified if a UDP message is sent to that port  
// See: 
// https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
 
#define FTPDATA_PORT     20    // FTP Data. 
#define FTPCONTROL_PORT  21    // FTP Control.
#define TELNET_PORT      23    // TELNET.
#define DNS_PORT         53    // (DNS) - Domain Name System. 
#define HTTP_PORT        80    // HTTP - HyperText Tranfer Protocol.
#define HTTPS_PORT       443   // HTTPS - HyperText Tranfer Protocol Secure.
// ...

#endif    

