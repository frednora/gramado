// http.c
// Gramnet HTTP – brag-worthy version

#include <kernel.h>


#define __SAFE_MSS  1400   // conservative


static char http_method[8]; 
static char http_uri[256];
static char http_version[16];

static int ParameterId = 0;  // Invalid


static int 
__parse_http_request_line(
    const char *payload, size_t len,
    char *method, size_t msz,
    char *uri, size_t usz,
    char *version, size_t vsz );

static int __http_parse_first_line(char *payload, size_t len);

// ---------------------------------------------

// Parse the first line: METHOD URI VERSION
static int 
__parse_http_request_line(
    const char *payload, size_t len,
    char *method, size_t msz,
    char *uri, size_t usz,
    char *version, size_t vsz )
{
    size_t i = 0;
    size_t pos = 0;

    if (!payload || len == 0) return -1;

    // METHOD
    while (i < len && payload[i] != ' ' && pos < msz-1) 
    {
        method[pos++] = payload[i++];
    }
    method[pos] = 0;
    if (i >= len || payload[i] != ' ') 
        return -1;
    i++; // skip space

    // URI
    pos = 0;
    while (i < len && payload[i] != ' ' && payload[i] != '\r' && pos < usz-1) 
    {
        uri[pos++] = payload[i++];
    }
    uri[pos] = 0;
    if (i >= len || payload[i] != ' ') 
        return -1;
    i++; // skip space

    // VERSION
    pos = 0;
    while (i < len && payload[i] != '\r' && payload[i] != '\n' && pos < vsz-1) 
    {
        version[pos++] = payload[i++];
    }
    version[pos] = 0;

    return 0;
}

static int __http_parse_first_line(char *payload, size_t len)
{
    int Status = -1;

// Parameters:
    if ((void*) payload == NULL)
        return -1;
    if (len <= 0)
        return -1;

    Status = 
    (int) __parse_http_request_line(
            payload, len, 
            http_method, sizeof(http_method),
            http_uri, sizeof(http_uri), 
            http_version, sizeof(http_version) 
        );

    if (Status != 0)
        return -1;

// ---

    printk("HTTP: method=%s uri=%s version=%s\n", 
        http_method, http_uri, http_version );

// Method id:
    int MethodId = 0;     // Invalid

    if ( gramado_strncmp(http_method,"GET",3) == 0 )
        MethodId = 1;
    if ( gramado_strncmp(http_method,"POST",4) == 0 )
        MethodId = 2;

    if (MethodId == 0)
        return -1;


//
// ROUTE: 
// Decide which path the request belongs to
//

// Parameter id:
// 1–4 → /id=N
// 5   → /favicon.ico
// 6   → /index.html

    // Using: Request Uniform Resource Identifier

    // id
    if ( gramado_strncmp(http_uri, "/id=1", 5) == 0 )
        ParameterId = 1;
    if ( gramado_strncmp(http_uri, "/id=2", 5) == 0 )
        ParameterId = 2;
    if ( gramado_strncmp(http_uri, "/id=3", 5) == 0 )
        ParameterId = 3;
    if ( gramado_strncmp(http_uri, "/id=4", 5) == 0 )
        ParameterId = 4;

    // files
    if ( gramado_strncmp(http_uri, "/favicon.ico", 12) == 0 )
        ParameterId = 5;
    if ( gramado_strncmp(http_uri, "/index.html",  11) == 0 )
        ParameterId = 6;
    if ( gramado_strncmp(http_uri, "/", 1) == 0 )
        ParameterId = 7;
    if ( gramado_strncmp(http_uri, "/about", 6) == 0 )
        ParameterId = 8;

    return (int) MethodId;  // Done
}

// Tokens for the first line:
// (Request Uniform Resource Identifier) 
// METHOD SP REQUEST-URI SP HTTP-VERSION CRLF
// GET /index.html HTTP/1.1
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

    printk("\n");
    printk("\n");

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

    printk("HTTP GET from %d.%d.%d.%d:%u\n",
           NetworkSaved.caller_ipv4[0], NetworkSaved.caller_ipv4[1],
           NetworkSaved.caller_ipv4[2], NetworkSaved.caller_ipv4[3],
           sport);

    //char *p;
    //p = payload;

    int WhatMethod = 0;
    WhatMethod = (int) __http_parse_first_line(payload, len);

    if (WhatMethod == 0){
        printk("HTTP: Invalid method\n");
        goto fail;
    }
    // Not a GET
    //if (WhatMethod != 1){
    //    printk("HTTP: Not a GET \n");
    //    goto fail;
    //}


    //static const char notfound_body[] =
        //"<html><body><h1>404 Not Found</h1></body></html>\n";


    // ============================================================
    //  Beautiful response for bragging rights
    // ============================================================
    // The body is its own string, so its length is always known
    // exactly via sizeof(). Content-Length is derived from it below,
    // instead of being a hand-typed number that can drift out of sync.

/*
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
*/


/*
    // -------------------------------------------------
    // Small but complete HTML (full DOM)
    // -------------------------------------------------
    static const char body[] =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "<meta charset=\"utf-8\">\n"
        "<title>Gramado OS</title>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Hello from Gramado OS!</h1>\n"
        "<p>This page was served directly from the <b>kernel</b>.</p>\n"
        "<p>Pure TCP/IP stack written from scratch.</p>\n"
        "<p>No userspace web server.</p>\n"
        "<hr>\n"
        "<p>Port 11888 &mdash; Built by <a href=\"https://github.com/frednora\">Fred Nora</a></p>\n"
        "</body>\n"
        "</html>\n";
*/

    static const char body[] =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head><meta charset=\"utf-8\"><title>Gramado OS</title></head>\n"
    "<body>\n"
    "<h1>Hello from Gramado OS!</h1>\n"
    "<p>This page was served directly from the <b>kernel</b>.</p>\n"
    "<p>Try exploring:</p>\n"
    "<ul>\n"
    "  <li><a href=\"/index.html\">Home</a></li>\n"
    "  <li><a href=\"/about\">About Gramado</a></li>\n"
    "  <li><a href=\"/id=1\">Special ID=1 page</a></li>\n"
    "</ul>\n"
    "<hr>\n"
    "<p>Port 11888 &mdash; Built by Fred Nora</p>\n"
    "</body>\n"
    "</html>\n";

    size_t body_len = sizeof(body) - 1;


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
    memset(resp, 0, sizeof(resp));
    //resp[0] = 0;


    // -- status -------------
    switch (ParameterId) {
    case 1:
        strcat(resp, "HTTP/1.1 200 OK\r\n");
        break;
    case 2:
        strcat(resp, "HTTP/1.1 201 Created\r\n");
        break;
    case 3:
        strcat(resp, "HTTP/1.1 404 Not Found\r\n");
        break;
    case 4:
        strcat(resp, "HTTP/1.1 500 Internal Server Error\r\n");
        break;
    default:
        strcat(resp, "HTTP/1.1 200 OK\r\n");
        //strcat(resp, "HTTP/1.0 200 OK\r\n");
        //strcat(resp, "HTTP/1.1 200 OK\r\n");
        break;
    };

    // -- server --------
    switch (ParameterId){
    case 1:  strcat(resp, "Server: Gramnet/0.1 (Gramado)\r\n");   break;
    case 2:  strcat(resp, "Server: Kerenel/0.8 (Gramado)\r\n");   break;
    case 3:  strcat(resp, "Server: Apache/2.4.41 (Ubuntu)\r\n");  break;
    case 4:  strcat(resp, "Server: Microsoft-IIS/10.0\r\n");      break;
    default:
        strcat(resp, "Server: Gramnet/0.1 (Gramado)\r\n");
        break;
    };

    // -- content type --------
    switch (ParameterId) {
    case 5:  strcat(resp, "Content-Type: image/x-icon\r\n"); break;
    case 6:  strcat(resp, "Content-Type: text/html; charset=utf-8\r\n"); break;
    default: 
        strcat(resp, "Content-Type: text/html; charset=utf-8\r\n"); 
        break;
    }

    // -- content lenght --------
    strcat(resp, "Content-Length: ");
    strcat(resp, len_str);
    strcat(resp, "\r\n");

    // -- connection --------
    switch (WhatMethod) {
    case 1:  strcat(resp, "Connection: close\r\n"); break;  // GET
    case 2:  strcat(resp, "Connection: keep-alive\r\n"); break; // POST
    default: strcat(resp, "Connection: close\r\n"); break;
    }
    strcat(resp, "\r\n");

//
// VIEW: 
// Render the HTML body
//

    strcat(resp, body);

    size_t resp_len = strlen(resp);

//
// Window
//
    if (resp_len > peer_window) {
        printk("HTTP: [ALERT] response %u bytes exceeds peer window %u\n",
               (unsigned) resp_len, (unsigned) peer_window);

        goto fail;
    }

//
// No fragment
//

    if (resp_len > __SAFE_MSS) {
        printk("HTTP: response too big (%u > %u), truncating or splitting needed\n",
           (int)resp_len, (int)__SAFE_MSS);
        // for now just refuse or use a smaller page
        goto fail;
    }


    tcp_seq seq = conn->tcp_conn->snd_nxt;   // 1001 after SYN
    tcp_ack ack = conn->tcp_conn->rcv_nxt;   // client’s ISN + 1

// Send ACK + PUSH + FIN
    int rv = 
    network_send_tcp(
        dhcp_info.your_ipv4,
        NetworkSaved.caller_ipv4,
        NetworkSaved.caller_mac,
        11888, sport,
        seq, ack,
        TH_ACK | TH_PUSH | TH_FIN,
        resp, resp_len 
    );

    if (rv < 0) {
        printk("HTTP: send failed, not advancing state\n");
        return -1;
    }

    // Data + FIN
    conn->tcp_conn->snd_nxt += resp_len + 1;
    // #ps: We are waiting for a FIN because we sent a FIN.
    conn->status = CONN_STATUS_FIN_WAIT1;

    return 0;
fail:
    return -1;
}
