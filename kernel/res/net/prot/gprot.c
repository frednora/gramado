// gprot.c
// gprot protocol support.
// Created by Fred Nora.

#include <kernel.h>

static char response_file[512];
const char *gprot_response_file = 
    "<html><head></head><body><span>This is the Gramado Kernel</span></body></html>\x00";

int
gprot_send_udp ( 
    uint8_t target_ip[4], 
    uint8_t target_mac[6], 
    unsigned short source_port,
    unsigned short target_port,
    char *data_buffer,   // UDP payload
    size_t data_lenght )
{

// Parameters:
    if ((void*) target_ip == NULL)
        goto fail;
    if ((void*) target_mac == NULL)
        goto fail;
    if ((void*) data_buffer == NULL)
        goto fail;
    if (data_lenght <= 0)
        goto fail;

    network_send_udp(  
        dhcp_info.your_ipv4,  // scr ip
        target_ip,            // dst ip
        target_mac,           // dst mac
        source_port,          // source port: "US"
        target_port,          // target port  "Who sent"
        data_buffer,          // udp payload
        data_lenght );        // udp payload size (Message size)

fail:
    return (int) -1;
}

// #test: 
// Respond the UDP message receive on port 11888.
// Somente se o dhcp foi initializado.
// + Nosso IP ficou registrado na estrutura de dhcp.
// + #todo: 
//   O IP do alvo deveria estar salvo em alguma estrutura
//   provavelmente na estrutura de IP.
// + #todo: 
//   O MAC do alvo ficaria registrado na estrutura de ethernet.
int gprot_handle_protocol(char *data, uint16_t s_port, uint16_t d_port)
{
    char *buf = (char *) data;  // Testing on UDP payload for now.
    uint16_t sport = s_port;
    uint16_t dport = d_port;
    const uint16_t OurPort = 11888;
    size_t MessageSize = 256;

    int NoReply = TRUE;

    if (dhcp_info.initialized != TRUE)
        goto fail;

    if ((void*) buf == NULL)
        goto fail;

    if (dport != OurPort)
        goto fail;

// ----------------
// g:/
// packet type: 0 = request
    
    if ( buf[0] == 'g' && 
         buf[1] == ':' && 
         buf[2] == '/' )
    {
        // Prepare the file
        memset(response_file, 0, sizeof(response_file));
        //ksprintf(response_file,"g:1 ");
        //ksprintf(response_file,"<spam>Hello world<spam/>");
        ksprintf(response_file,gprot_response_file);

        memset(buf, 0, sizeof(buf));
        ksprintf(buf,"g:1 ");  // Reply code
        ksprintf(
            (buf + 4),
            response_file );
        //ksprintf(buf,sizeof(response_file));
        //ksprintf(buf,"g:1 ");  // Reply code
        //ksprintf(
        //    (buf + 4),
        //    "This is a response from Gramado OS\n");
        NoReply = FALSE;
        goto done;
    }



// ----------------
// g:x
// packet type: 0 = request
    if ( buf[0] == 'g' && 
         buf[1] == ':' && 
         buf[2] == 'x' )
    {
        memset(buf, 0, sizeof(buf));
        ksprintf(buf,"g:1 ");  // Reply code
        ksprintf(
            (buf + 4),
            "Gramado OS received a g:x request\n");

        ipc_post_message_to_ds( (int) 800800, 0, 0 );
        NoReply = FALSE;
        goto done;
    }

// ----------------
// g:0
// packet type: 0 = request
    if ( buf[0] == 'g' && 
         buf[1] == ':' && 
         buf[2] == '0' )
    {
        memset(buf, 0, sizeof(buf));
        ksprintf(buf,"g:1 ");  // Reply code
        ksprintf(
            (buf + 4),
            "This is a response from Gramado OS\n");
        NoReply = FALSE;
        goto done;
    }

// -----------------------
// g:1
// packet type: 1 = reply
    if ( buf[0] == 'g' && 
         buf[1] == ':' && 
         buf[2] == '1' )
    {
        memset(buf, 0, sizeof(buf));
        ksprintf(buf,"g:a ");  // ACK
        ksprintf(
            (buf + 4),
            "ACK\n");
        NoReply = FALSE;
        goto done;
    }

// -------------------------
// g:2
// packet type: 2 = event
    if ( buf[0] == 'g' && 
         buf[1] == ':' && 
         buf[2] == '2' )
    {
        memset(buf, 0, sizeof(buf));
        ksprintf(buf,"g:a ");  // ACK
        ksprintf(
            (buf + 4),
            "ACK\n");
        NoReply = FALSE;
        goto done;
    }

// ---------------------------
// g:3
// packet type: 3 = error
    if ( buf[0] == 'g' && 
         buf[1] == ':' && 
         buf[2] == '3' )
    {
        memset(buf, 0, sizeof(buf));
        ksprintf(buf,"g:a ");  // ACK
        ksprintf(
            (buf + 4),
            "ACK\n");
        NoReply = FALSE;
        goto done;
    }

// --------------------------
// g:4
// packet type: 4 = disconnect
    if ( buf[0] == 'g' && 
         buf[1] == ':' && 
         buf[2] == '4' )
    {
        //printk("[g:4] \n");
        //refresh_screen;

        memset(buf, 0, sizeof(buf));
        ksprintf(buf,"g:0 ");          // Request
        ksprintf( (buf + 4), "exit");  // exit command
        NoReply = FALSE;
        goto done;
    }

/*
// --------------------------
// #todo
// g:5
// packet type: 5 = ???
    if ( buf[0] == 'g' && 
         buf[1] == ':' && 
         buf[2] == '5' )
    {
        //printk("[g:5] \n");
        //refresh_screen;

        memset(buf, 0, sizeof(buf));
        ksprintf(buf,"g:0 ");          // Request
        ksprintf( (buf + 4), "exit");  // exit command
        NoReply = FALSE;
        goto done;
    }
*/

// ----------------
// Invalid request
invalid_request:
    memset(buf, 0, sizeof(buf));
    ksprintf(buf,"g:3 ");  // Error
    NoReply = FALSE;
    goto done;

// ---------------------
// Response
done:
    if (NoReply == TRUE)
        return 0;

    //printk ("kernel: Sending response\n");
    //refresh_screen();

    if ((void*)buf==NULL)
        return (int) -1;
    if (MessageSize <= 0)
        return (int) -1;

    // #test
    gprot_send_udp (
        NetworkSaved.caller_ipv4,  // dst ip
        NetworkSaved.caller_mac,   // dst mac
        dport,                // source port: "US"
        sport,                // target port  "Who sent something to us"
        buf,                  // udp payload
        MessageSize );        // udp payload size (Message size)
    /*
    // OK
    network_send_udp (  
        dhcp_info.your_ipv4,  // scr ip
        NetworkSaved.caller_ipv4,  // dst ip
        NetworkSaved.caller_mac,   // dst mac
        dport,                // source port: "US"
        sport,                // target port  "Who sent something to us"
        buf,                  // udp payload
        MessageSize );        // udp payload size (Message size)
    */

    return 0; // ok

fail:
    return (int) -1;
}


/*
int gprot_handle_protocol2(char *data, uint16_t s_port, uint16_t d_port);
int gprot_handle_protocol2(char *data, uint16_t s_port, uint16_t d_port) 
{
    char *buf = data;  // UDP payload
    uint16_t sport = s_port;
    uint16_t dport = d_port;
    size_t MessageSize = 256;
    int NoReply = TRUE;

    if (!data || dhcp_info.initialized != TRUE || d_port != 11888) 
        return -1;

    // Validate prefix format
    if (buf[0] != 'g' || buf[1] != ':')
        return 0;

    switch (buf[2]) {
        case '0':  // Request
            snprintf(buf, MessageSize, "g:1 This is a response from Gramado OS\n");
            NoReply = FALSE;
            break;

        case '1':  // Reply
            printk("[g:1] REPLY\n");
            break;

        case '2':  // Event
            printk("[g:2] EVENT\n");
            break;

        case '3':  // Error
            printk("[g:3] ERROR\n");
            break;

        case '4':  // Disconnect
            snprintf(buf, MessageSize, "g:0 exit");
            NoReply = FALSE;
            break;

        default:
            return 0;
    }

    // Send response if required
    if (!NoReply) {
        network_send_udp(
            dhcp_info.your_ipv4, 
            NetworkSaved.caller_ipv4, 
            NetworkSaved.caller_mac, 
            dport, 
            sport, 
            buf, 
            MessageSize );
    }

    return 0;
}
*/


/*
Approach
Identify HTTP requests based on common prefixes (GET /, POST /, etc.).
Extract the request type (GET, POST).
Formulate a simple response (e.g., return a basic HTTP response like "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello from Gramado OS").
Reuse existing resources (use network_send_udp() to return the response).
*/

/*
// I little bit of http
int gprot_handle_protocol(char *data, uint16_t s_port, uint16_t d_port) ;
int gprot_handle_protocol(char *data, uint16_t s_port, uint16_t d_port) 
{
    if (!data || dhcp_info.initialized != TRUE || d_port != 11888) return -1;

    char *buf = data;  // UDP payload
    uint16_t sport = s_port;
    uint16_t dport = d_port;
    size_t MessageSize = 256;
    int NoReply = TRUE;

    // Check for custom protocol
    if (buf[0] == 'g' && buf[1] == ':') {
        switch (buf[2]) {
            case '0':  // Request
                snprintf(buf, MessageSize, "g:1 This is a response from Gramado OS\n");
                NoReply = FALSE;
                break;

            case '1': printk("[g:1] REPLY\n"); break;
            case '2': printk("[g:2] EVENT\n"); break;
            case '3': printk("[g:3] ERROR\n"); break;
            case '4': 
                snprintf(buf, MessageSize, "g:0 exit");
                NoReply = FALSE;
                break;

            default: return 0;
        }
    }
    // Check for HTTP request
    else if (strncmp(buf, "GET /", 5) == 0 || strncmp(buf, "POST /", 6) == 0) {
        snprintf(buf, MessageSize, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Hello from Gramado OS\n");
        NoReply = FALSE;
    }

    // Send response if needed
    if (!NoReply) 
    {
        network_send_udp(
            dhcp_info.your_ipv4, 
            NetworkSaved.calleripv4, 
            NetworkSaved.caller_mac, 
            dport, 
            sport, 
            buf, 
            MessageSize );
    }

    return 0;
}
*/


