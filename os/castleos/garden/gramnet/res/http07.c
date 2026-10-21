// http.c
// Gramnet HTTP implementation – richer browser-friendly response

#include <kernel.h>


int 
gramnet_handle_http(
    struct connection_d *conn,
    const char *payload,
    size_t len,
    uint16_t sport, uint16_t dport)
{
    if (!conn || !conn->tcp_conn || len < 4)
        return -1;

    // Debug: show request
    char dbg[80];
    size_t n = len < 79 ? len : 79;
    memcpy(dbg, payload, n);
    dbg[n] = 0;
    printk("HTTP: {%s}\n", dbg);

    // Accept only GET for now
    if (payload[0] != 'G' || payload[1] != 'E' ||
        payload[2] != 'T' || payload[3] != ' ')
    {
        printk("HTTP: Not a GET request\n");
        return 0;
    }

    printk("HTTP GET from %d.%d.%d.%d:%u\n",
           NetworkSaved.caller_ipv4[0], NetworkSaved.caller_ipv4[1],
           NetworkSaved.caller_ipv4[2], NetworkSaved.caller_ipv4[3],
           sport);

    // -------------------------------
    // Build body
    // -------------------------------
    static const char body[] =
        "<!DOCTYPE html>\r\n"
        "<html>\r\n"
        "<head>\r\n"
        "  <meta charset=\"utf-8\">\r\n"
        "  <title>Gramado OS</title>\r\n"
        "  <style>\r\n"
        "    body { font-family: system-ui, sans-serif; background:#1a1a2e; color:#eee; text-align:center; padding-top:80px; }\r\n"
        "    h1 { color:#00d4ff; }\r\n"
        "  </style>\r\n"
        "</head>\r\n"
        "<body>\r\n"
        "  <h1>Hello from Gramado OS!</h1>\r\n"
        "  <p>TCP + HTTP is working on port 11888.</p>\r\n"
        "  <p>This response was generated inside the kernel.</p>\r\n"
        "</body>\r\n"
        "</html>\r\n";

    unsigned int body_len = (unsigned int)strlen(body);

    // -------------------------------
    // Build header with correct length
    // -------------------------------
    char header[256];
    ksprintf(header,
        "HTTP/1.0 200 OK\r\n"
        "Server: Gramado/0.1\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        "\r\n",
        body_len);

    // -------------------------------
    // Concatenate header + body
    // -------------------------------
    char response[sizeof(header) + sizeof(body)];
    size_t header_len = strlen(header);
    memcpy(response, header, header_len);
    memcpy(response + header_len, body, body_len);
    size_t resp_len = header_len + body_len;

    // -------------------------------
    // Send response
    // -------------------------------
    tcp_seq seq = conn->tcp_conn->snd_nxt;
    tcp_ack ack = conn->tcp_conn->rcv_nxt;

    network_send_tcp(
        dhcp_info.your_ipv4,
        NetworkSaved.caller_ipv4,
        NetworkSaved.caller_mac,
        11888, sport,
        seq, ack,
        TH_ACK | TH_PUSH | TH_FIN,
        response, resp_len);

    // Advance sequence number (data + FIN)
    conn->tcp_conn->snd_nxt += resp_len + 1;
    conn->status = CONN_STATUS_FIN_WAIT;

    return 0;
}
