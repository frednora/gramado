

#include <kernel.h>

const char body00[] =
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


