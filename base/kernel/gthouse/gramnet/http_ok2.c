// http.c
// Gramnet HTTP – brag-worthy version

#include <kernel.h>


#define __METHOD_NULL    0
#define __METHOD_GET     1
#define __METHOD_POST    2
// ...

#define ROUTEID_ROOT  1
#define ROUTEID_ID  2
#define ROUTEID_ABOUT  3
#define ROUTEID_FAVICON_ICO  4
#define ROUTEID_INDEX_HTML  5
// ...

struct http_request_info_d 
{
    int MethodId;

    int RouteId;
    int ParameterId;
};
static struct http_request_info_d  HTTP_REQUEST_INFO;

struct http_response_info_d
{
    char *response_ptr;   // Pointer to assembled response
    int response_size;    // Length of response in bytes
};
static struct http_response_info_d  HTTP_REPONSE_INFO;


static char resp[2048];

// ------------------

#define __SAFE_MSS  1400   // conservative


static char http_method[8]; 
static char http_uri[256];
static char http_version[16];


static int 
__parse_http_request_line(
    const char *payload, size_t len,
    char *method, size_t msz,
    char *uri, size_t usz,
    char *version, size_t vsz );

static int __http_parse_first_line(char *payload, size_t len);

static int 
__build_response_view(
    int method_id, 
    int route_id, 
    int parameter_id );


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

    // Initialization
    HTTP_REQUEST_INFO.MethodId = 0;
    HTTP_REQUEST_INFO.RouteId = 0;
    HTTP_REQUEST_INFO.ParameterId = 0;

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
    int MethodId = __METHOD_NULL;     // Invalid

    if ( gramado_strncmp(http_method, "GET", 3) == 0 )
        MethodId = __METHOD_GET;
    if ( gramado_strncmp(http_method, "POST", 4) == 0 )
        MethodId = __METHOD_POST;
    // ...
    HTTP_REQUEST_INFO.MethodId = MethodId;


//
// ROUTE: 
// Decide which path the request belongs to
//

// Parameter id:
// 1–4 → /id=N
// 5   → /favicon.ico
// 6   → /index.html

    // Using: Request Uniform Resource Identifier


    // files

    // root --------------
    if ( gramado_strncmp(http_uri, "/", 1) == 0 )
    {
        HTTP_REQUEST_INFO.RouteId = ROUTEID_ROOT;
        HTTP_REQUEST_INFO.ParameterId = 0;
    }

    // root + id ---------------
    if ( gramado_strncmp(http_uri, "/id=0", 5) == 0 )
    {
        HTTP_REQUEST_INFO.RouteId = ROUTEID_ID;
        HTTP_REQUEST_INFO.ParameterId = 0;
    }
    if ( gramado_strncmp(http_uri, "/id=1", 5) == 0 )
    {
        HTTP_REQUEST_INFO.RouteId = ROUTEID_ID;
        HTTP_REQUEST_INFO.ParameterId = 1;
    }
    if ( gramado_strncmp(http_uri, "/id=2", 5) == 0 )
    {
        HTTP_REQUEST_INFO.RouteId = ROUTEID_ID;
        HTTP_REQUEST_INFO.ParameterId = 2;
    }
    if ( gramado_strncmp(http_uri, "/id=3", 5) == 0 )
    {
        HTTP_REQUEST_INFO.RouteId = ROUTEID_ID;
        HTTP_REQUEST_INFO.ParameterId = 3;
    }
    if ( gramado_strncmp(http_uri, "/id=4", 5) == 0 )
    {
        HTTP_REQUEST_INFO.RouteId = ROUTEID_ID;
        HTTP_REQUEST_INFO.ParameterId = 4;
    }

    if ( gramado_strncmp(http_uri, "/about", 6) == 0 )
    {
        HTTP_REQUEST_INFO.RouteId = ROUTEID_ABOUT;
        HTTP_REQUEST_INFO.ParameterId = 0;
    }

    if ( gramado_strncmp(http_uri, "/favicon.ico", 12) == 0 )
    {
        HTTP_REQUEST_INFO.RouteId = ROUTEID_FAVICON_ICO;
        HTTP_REQUEST_INFO.ParameterId = 0;
    }

    if ( gramado_strncmp(http_uri, "/index.html",  11) == 0 )
    {
        HTTP_REQUEST_INFO.RouteId = ROUTEID_INDEX_HTML;
        HTTP_REQUEST_INFO.ParameterId = 0;
    }

//------------------------------------------------


    return 0;  //OK
}

static int 
__build_response_view(
    int method_id, 
    int route_id, 
    int parameter_id )
{
    HTTP_REPONSE_INFO.response_ptr = NULL;
    HTTP_REPONSE_INFO.response_size = 0;


//
// Web page
//

    static const char body1[] =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Gramado OS</title>\n"
    "<style>\n"
    "  body { font-family: system-ui, sans-serif; margin: 0; padding: 0; background: #f5f5f5; }\n"
    "  .container {\n"
    "    max-width: 640px;\n"
    "    margin: 40px auto;\n"
    "    padding: 24px;\n"
    "    background: #fff;\n"
    "    border-radius: 8px;\n"
    "    box-shadow: 0 2px 8px rgba(0,0,0,0.08);\n"
    "    text-align: center;\n"
    "  }\n"
    "  hr { border: none; border-top: 1px solid #ddd; margin: 20px 0; }\n"
    "  ul { list-style: none; padding: 0; }\n"
    "  li { margin: 8px 0; }\n"
    "  a { color: #0066cc; text-decoration: none; }\n"
    "  a:hover { text-decoration: underline; }\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<div class=\"container\">\n"
    "<h1>Hello from Gramado OS!</h1>\n"
    "<hr>\n"
    "<p>This page was served directly from the <b>kernel</b>.</p>\n"
    "<p>Try exploring:</p>\n"
    "<ul>\n"
    "  <li><a href=\"/index.html\">Home</a></li>\n"
    "  <li><a href=\"/about\">About Gramado</a></li>\n"
    "  <li><a href=\"/id=1\">Special ID=1 page</a></li>\n"
    "</ul>\n"
    "<hr>\n"
    "<p>Port 11888 &mdash; Built by Fred Nora</p>\n"
    "</div>\n"
    "</body>\n"
    "</html>\n";

    // Invalid parameter id
    static const char body2[] =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Error</title>\n"
    "</head>\n"
    "<body>\n"
    "<h2>Oops! Invalid id</h2>\n"
    "<p>The server could not process your request.</p>\n"
    "</body>\n"
    "</html>\n";


// body len --------------------------------

    size_t body_len;
    char *body;


    // '/'
    if (route_id == ROUTEID_ROOT){
        body = body1;
        body_len = sizeof(body1) - 1;

    // '/id='
    } else if (route_id == ROUTEID_ID){
    
        switch (parameter_id) {

        case 0:
            body = body1;
            body_len = sizeof(body1) - 1;
            break;

        case 1:
        case 2:
        case 3:
        case 4:
            body = body1;
            body_len = sizeof(body1) - 1;
            break;

        // Invalid id
        default:
            body = body2;
            body_len = sizeof(body2) - 1;
            break;
        };

    // '/about'
    } else if (route_id == ROUTEID_ABOUT) {
        body = body1;
        body_len = sizeof(body1) - 1;

    // '/favicon.ico'
    } else if (route_id == ROUTEID_FAVICON_ICO) {
        body = body1;
        body_len = sizeof(body1) - 1;

    // '/index.html'
    } else if (route_id == ROUTEID_INDEX_HTML) {
        body = body1;
        body_len = sizeof(body1) - 1;

    // Invalid
    } else {
        body = body2;
        body_len = sizeof(body2) - 1;
    }


// -----------------------------------------
// Convert body_len to decimal ASCII manually (no snprintf).
    char body_len_str[8];
    size_t v = body_len;
    int i = 0;
    char tmp[8];

    if (v == 0) {
        tmp[i] = '0';
        i++;
    } else {

        int SizeOfTmp = sizeof(tmp);

        while ( v > 0 && 
                i < (int) SizeOfTmp )
        {
            tmp[i] = (char) ('0' + (v % 10));
            i++;
            v /= 10;
        }
    }
    
    // tmp holds digits reversed; flip into body_len_str.
    int j = 0;
    while (i > 0) 
    {
        body_len_str[j++] = tmp[--i];
    }
    body_len_str[j] = 0;
// -----------------------------------------

// -----------------------------------------
// Headers are assembled with strcat, so Content-Length always
// matches the real body size instead of a hand-typed literal.

    //char resp[2048];
    memset(resp, 0, sizeof(resp));

    // -- status -------------
    switch (parameter_id) {
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
    switch (parameter_id){
    case 1:  strcat(resp, "Server: Gramnet/0.1 (Gramado)\r\n");   break;
    case 2:  strcat(resp, "Server: Kerenel/0.8 (Gramado)\r\n");   break;
    case 3:  strcat(resp, "Server: Apache/2.4.41 (Ubuntu)\r\n");  break;
    case 4:  strcat(resp, "Server: Microsoft-IIS/10.0\r\n");      break;
    default:
        strcat(resp, "Server: Gramnet/0.1 (Gramado)\r\n");
        break;
    };

    // -- content type --------
    switch (route_id) 
    {
        case ROUTEID_ROOT:
            strcat(resp, "Content-Type: text/html; charset=utf-8\r\n");
            break;

        case ROUTEID_INDEX_HTML:
            strcat(resp, "Content-Type: text/html; charset=utf-8\r\n");
            break;

        case ROUTEID_FAVICON_ICO:
            strcat(resp, "Content-Type: image/x-icon\r\n");
            break;

        default: 
            strcat(resp, "Content-Type: text/html; charset=utf-8\r\n"); 
            break;
    };

    // -- content lenght --------
    strcat(resp, "Content-Length: ");
    strcat(resp, body_len_str);
    strcat(resp, "\r\n");

    // -- connection --------
    switch (method_id) 
    {
        // GET
        case __METHOD_GET:  
            strcat(resp, "Connection: close\r\n");
            break;

        // POST
        case __METHOD_POST:
            strcat(resp, "Connection: keep-alive\r\n");
            break;

        default:
            strcat(resp, "Connection: close\r\n");
            break;
    }
    strcat(resp, "\r\n");

//
// VIEW: 
// Render the HTML body
//

    strcat(resp, body);

    size_t resp_len = strlen(resp);

    // Save info

    HTTP_REPONSE_INFO.response_ptr = (char *) resp;
    HTTP_REPONSE_INFO.response_size = resp_len;

    return 0;
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
    uint16_t sport, uint16_t dport )
{
    if (!conn)
        return -1;
    if ( (void*)conn->tcp_conn == NULL || len < 4 )
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

    int Status = 0;
    Status = (int) __http_parse_first_line(payload, len);

    // 0 = OK
    if (Status != 0){
        printk("HTTP: on __http_parse_first_line()\n");
        goto fail;
    }


// -------------------------

// Build the response string: header + body

    __build_response_view(
        HTTP_REQUEST_INFO.MethodId,
        HTTP_REQUEST_INFO.RouteId,
        HTTP_REQUEST_INFO.ParameterId
    );

// -------------------------------------------------    

    char *resp = (char *) HTTP_REPONSE_INFO.response_ptr;
    size_t resp_len = HTTP_REPONSE_INFO.response_size;

//
// Window
//
    if (resp_len > peer_window) 
    {
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

    unsigned int Flags = TH_ACK | TH_PUSH | TH_FIN;

// Send ACK + PUSH + FIN
    int rv = 
    network_send_tcp(
        dhcp_info.your_ipv4,
        NetworkSaved.caller_ipv4,
        NetworkSaved.caller_mac,
        11888, sport,
        seq, ack,
        Flags,
        resp, 
        resp_len 
    );

    if (rv < 0) {
        printk("HTTP: send failed, not advancing state\n");
        return -1;
    }

// Data + FIN
// packetts sent ++
// #ps: 
// We are waiting for a FIN because we sent a FIN.

    conn->tcp_conn->snd_nxt += resp_len + 1;
    conn->packets_sent++;
    conn->status = CONN_STATUS_FIN_WAIT1;
    return 0;

fail:
    return -1;
}
