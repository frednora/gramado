// http.c
// Gramnet HTTP – brag-worthy version

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

    // The client only accepts this many bytes right now.
    uint16_t peer_window = (uint16_t) conn->tcp_conn->snd_wnd;

    // Debug
    char dbg[80];
    size_t n = len < 79 ? len : 79;
    memcpy(dbg, payload, n);
    dbg[n] = 0;
    printk("HTTP: {%s}\n", dbg);

// #test
// Push and return
    // ps: len
    // network_push_packet( payload, 512 );  // #test
    // return 0;

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

    // ============================================================
    //  Beautiful response for bragging rights
    // ============================================================
    // The body is its own string, so its length is always known
    // exactly via sizeof(). Content-Length is derived from it below,
    // instead of being a hand-typed number that can drift out of sync.

    static const char body[] =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "<meta charset=\"utf-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\n"
        "<title>Gramado OS</title>\n"
        "<style>\n"
        "*{margin:0;padding:0;box-sizing:border-box}\n"
        "body{font-family:system-ui,sans-serif;background:linear-gradient(135deg,#0f0c29,#302b63,#24243e);color:#e0e0ff;min-height:100vh;display:flex;align-items:center;justify-content:center;text-align:center}\n"
        ".card{background:rgba(255,255,255,.06);border:1px solid rgba(255,255,255,.12);border-radius:20px;padding:48px 36px;max-width:480px;box-shadow:0 25px 50px rgba(0,0,0,.4)}\n"
        "h1{font-size:2.3rem;background:linear-gradient(90deg,#00d4ff,#a855f7);-webkit-background-clip:text;-webkit-text-fill-color:transparent;margin-bottom:10px}\n"
        ".badge{display:inline-block;background:#00d4ff18;color:#00d4ff;padding:5px 16px;border-radius:20px;font-size:.8rem;margin-bottom:22px;border:1px solid #00d4ff33;letter-spacing:.5px}\n"
        "p{line-height:1.65;opacity:.9;margin-bottom:14px;font-size:1.05rem}\n"
        ".footer{margin-top:28px;font-size:.78rem;opacity:.45}\n"
        ".k{color:#a855f7;font-weight:600}\n"
        "</style>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"card\">\n"
        "<div class=\"badge\">KERNEL HTTP SERVER</div>\n"
        "<h1>Gramado OS</h1>\n"
        "<p>This page was served directly from the <span class=\"k\">kernel</span> using a pure TCP/IP stack written from scratch.</p>\n"
        "<p>No userspace server. No nginx. Just pure hobby OS magic.</p>\n"
        "<div class=\"footer\">Port 11888 &bull; Built with love by Fred Nora</div>\n"
        "</div>\n"
        "</body>\n"
        "</html>";

    size_t body_len = sizeof(body) - 1;  // exclude null terminator

    // Convert body_len to decimal ASCII manually (no snprintf).
    char len_str[8];
    {
        size_t v = body_len;
        int i = 0;
        char tmp[8];

        if (v == 0) {
            tmp[i++] = '0';
        } else {
            while (v > 0 && i < (int) sizeof(tmp)) {
                tmp[i++] = (char) ('0' + (v % 10));
                v /= 10;
            }
        }
        // tmp holds digits reversed; flip into len_str.
        int j = 0;
        while (i > 0) {
            len_str[j++] = tmp[--i];
        }
        len_str[j] = 0;
    }

    // Headers are assembled with strcat, so Content-Length always
    // matches the real body size instead of a hand-typed literal.
    char resp[2048];
    resp[0] = 0;

    strcat(resp, "HTTP/1.0 200 OK\r\n");
    strcat(resp, "Server: Gramado/0.1 (kernel)\r\n");
    strcat(resp, "Content-Type: text/html; charset=utf-8\r\n");
    strcat(resp, "Content-Length: ");
    strcat(resp, len_str);
    strcat(resp, "\r\n");
    strcat(resp, "Connection: close\r\n");
    strcat(resp, "\r\n");
    strcat(resp, body);

    size_t resp_len = strlen(resp);

    //
    // Window
    //
    if (resp_len > peer_window) {
        printk("HTTP: [ALERT] response %u bytes exceeds peer window %u\n",
               (unsigned) resp_len, (unsigned) peer_window);
    }

    tcp_seq seq = conn->tcp_conn->snd_nxt;   // 1001 after SYN
    tcp_ack ack = conn->tcp_conn->rcv_nxt;   // client’s ISN + 1

    int rv = network_send_tcp(
        dhcp_info.your_ipv4,
        NetworkSaved.caller_ipv4,
        NetworkSaved.caller_mac,
        11888, sport,
        seq, ack,
        TH_ACK | TH_PUSH | TH_FIN,
        resp,
        resp_len);

    if (rv < 0) {
        printk("HTTP: send failed, not advancing state\n");
        return -1;
    }

    // Data + FIN
    conn->tcp_conn->snd_nxt += resp_len + 1;
    conn->status = CONN_STATUS_FIN_WAIT;

    return 0;
}
