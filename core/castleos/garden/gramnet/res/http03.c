// http.c
// Gramnet HTTP implementation

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

    // Debug (safe)
    char dbg[80];
    size_t n = len < 79 ? len : 79;
    memcpy(dbg, payload, n);
    dbg[n] = 0;
    printk("HTTP: {%s}\n", dbg);

    if (payload[0] != 'G' || payload[1] != 'E' || payload[2] != 'T' || payload[3] != ' ')
    {
        printk("HTTP: Not a GET request\n");
        return 0;
    }

/*
//#bugbug Function Not working for this case!
    if (gramado_strncmp(payload, "GET ", 4) != 0)
    {
        printk("HTTP: Not a GET request\n");
        return 0;
    }
*/

    printk("HTTP GET from %d.%d.%d.%d:%u\n",
           NetworkSaved.caller_ipv4[0], NetworkSaved.caller_ipv4[1],
           NetworkSaved.caller_ipv4[2], NetworkSaved.caller_ipv4[3],
           sport);

    static const char resp[] =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello, world!";

    size_t resp_len = sizeof(resp) - 1;   // exclude the compiler's null

    tcp_seq seq = conn->tcp_conn->snd_nxt;
    tcp_ack ack = conn->tcp_conn->rcv_nxt;   // must already include the GET length

    network_send_tcp(
        dhcp_info.your_ipv4,
        NetworkSaved.caller_ipv4,
        NetworkSaved.caller_mac,
        11888, sport,
        seq, ack,
        TH_ACK | TH_PUSH | TH_FIN,
        (char *)resp,
        resp_len);

    // Data + FIN
    conn->tcp_conn->snd_nxt += resp_len + 1;
    conn->status = CONN_STATUS_FIN_WAIT;

    return 0;
}


