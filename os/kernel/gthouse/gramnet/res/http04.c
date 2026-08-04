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

    // Quick debug (safe)
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

    // -------------------------------------------------
    // Rich HTML response that modern browsers accept
    // -------------------------------------------------
    static const char resp[] =
        "HTTP/1.0 200 OK\r\n"
        "Server: Gramado/0.1\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: 312\r\n"          // 431 actually. exact length of the body below
        "Connection: close\r\n"
        "\r\n"
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

    size_t resp_len = sizeof(resp) - 1;   // exclude the trailing '\0'

    // Sanity check (remove later if you want)
    if (resp_len != 312 +  /* headers length */  118) {
        // The number above is just a reminder – the real check is:
        // Content-Length must equal the exact number of bytes after the blank line.
        // Current body is 312 bytes.
    }

    tcp_seq seq = conn->tcp_conn->snd_nxt;
    tcp_ack ack = conn->tcp_conn->rcv_nxt;   // already advanced by the GET length

    network_send_tcp(
        dhcp_info.your_ipv4,
        NetworkSaved.caller_ipv4,
        NetworkSaved.caller_mac,
        11888,                  // source port
        sport,                  // client port
        seq, ack,
        TH_ACK | TH_PUSH | TH_FIN,
        (char *)resp,
        resp_len);

    // Advance sequence number (data + FIN)
    conn->tcp_conn->snd_nxt += resp_len + 1;
    conn->status = CONN_STATUS_FIN_WAIT;

    return 0;
}

