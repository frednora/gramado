// http.c
// Gramnet HTTP implementation

#include <kernel.h>

int gramnet_handle_http(const char *tcp_payload, uint16_t s_port, uint16_t d_port)
{
    // Only handle traffic directed to our test port
    if (d_port != 11888) {
        return -1;
    }

    printk("HTTP handler: request on port %d\n", d_port);

    // Minimal HTTP/1.0 error response
    const char *http_response =
        "HTTP/1.0 400 Bad Request\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 21\r\n"
        "\r\n"
        "Invalid HTTP request.\n";

    // Send response back using your TCP routine
    network_send_tcp(
        dhcp_info.your_ipv4,        // our IP
        NetworkSaved.caller_ipv4,   // client IP
        NetworkSaved.caller_mac,    // client MAC
        d_port,                     // our port (server side)
        s_port,                     // client port
        2000,                       // seq (placeholder, adjust later)
        0,                          // ack (placeholder, adjust later)
        TH_ACK | TH_PUSH,           // flags: ACK + PSH to deliver data
        (char *)http_response,
        strlen(http_response)
    );

    return 0;
}
