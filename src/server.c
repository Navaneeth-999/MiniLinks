#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mongoose.h"

/* Function Prototypes */
void generate_code(char *code);
char *save_url(const char *url);

/* Generate random 6-character code */
void generate_code(char *code)
{
    const char chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";

    for (int i = 0; i < 6; i++)
    {
        code[i] = chars[rand() % 62];
    }

    code[6] = '\0';
}

/* Save URL and return generated code */
char *save_url(const char *url)
{
    static char code[7];

    generate_code(code);

    FILE *fp = fopen("urls.db", "a");

    if (fp != NULL)
    {
        fprintf(fp, "%s|%s\n", code, url);
        fclose(fp);
    }

    return code;
}
char *find_url(const char *code)
{
    static char url[1024];

    FILE *fp = fopen("urls.db", "r");

    if (fp == NULL)
        return NULL;

    char line[2048];
    char stored_code[100];

    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%99[^|]|%1023s", stored_code, url) == 2)
        {
            if (strcmp(stored_code, code) == 0)
            {
                fclose(fp);
                return url;
            }
        }
    }

    fclose(fp);
    return NULL;
}
static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        printf("URI = [%.*s]\n", (int) hm->uri.len, hm->uri.buf);
        char code[100];

snprintf(code, sizeof(code),
         "%.*s",
         (int)hm->uri.len - 1,
         hm->uri.buf + 1);
{
    char *url = find_url(code);

    if (url)
    {
        printf("Found URL = %s\n", url);

mg_printf(c,
          "HTTP/1.1 302 Found\r\n"
          "Location: %s\r\n"
          "Content-Length: 0\r\n\r\n",
          url);

return;
    }
}
       if (mg_match(hm->uri, mg_str("/shorten"), NULL))
{
    char url[1024];

    mg_http_get_var(&hm->body, "url", url, sizeof(url));

    char *code = save_url(url);

    mg_http_reply(c, 200,
"Content-Type: text/html\r\n",

"<!DOCTYPE html>"
"<html>"
"<head>"
"<title>MiniLinks Success</title>"
"<style>"
"body{"
"font-family:Segoe UI,sans-serif;"
"height:100vh;"
"display:flex;"
"justify-content:center;"
"align-items:center;"
"background:linear-gradient(135deg,#00c6ff,#0072ff);"
"}"

".card{"
"background:white;"
"padding:40px;"
"border-radius:20px;"
"width:600px;"
"text-align:center;"
"box-shadow:0 20px 40px rgba(0,0,0,.2);"
"}"

"h1{"
"color:#28a745;"
"margin-bottom:15px;"
"}"

".url{"
"background:#f5f5f5;"
"padding:15px;"
"border-radius:10px;"
"margin:20px 0;"
"word-wrap:break-word;"
"}"

"a{"
"display:inline-block;"
"padding:12px 25px;"
"background:#0072ff;"
"color:white;"
"text-decoration:none;"
"border-radius:10px;"
"}"

"a:hover{"
"background:#0056d6;"
"}"
"</style>"
"</head>"

"<body>"

"<div class='card'>"

"<h1>🎉 URL Shortened Successfully!</h1>"

"<p>Your giant URL has been saved.</p>"

"<div class='url'>Original URL:<br>%s</div>"

"<h2>🔗 Your Short URL</h2>"

"<div class='url'>"
"http://localhost:8000/%s"
"</div>"

"<a href='/'>⬅ Back To Home</a>"

"</div>"

"</body>"
"</html>",
url,code);

    return;
}
        mg_http_reply(c, 200,
"Content-Type: text/html\r\n",
"<!DOCTYPE html>"
"<html>"
"<head>"
"<title>MiniLinks</title>"
"<style>"
"*{margin:0;padding:0;box-sizing:border-box;}"
"body{"
"font-family:Segoe UI,sans-serif;"
"height:100vh;"
"display:flex;"
"justify-content:center;"
"align-items:center;"
"background:linear-gradient(135deg,#667eea,#764ba2);"
"}"
".card{"
"background:white;"
"padding:40px;"
"border-radius:20px;"
"box-shadow:0 15px 35px rgba(0,0,0,0.2);"
"width:500px;"
"text-align:center;"
"}"
"h1{"
"font-size:42px;"
"margin-bottom:10px;"
"color:#333;"
"}"
".subtitle{"
"color:#666;"
"margin-bottom:25px;"
"}"
"input{"
"width:100%;"
"padding:15px;"
"border:2px solid #ddd;"
"border-radius:12px;"
"font-size:16px;"
"}"
"button{"
"margin-top:15px;"
"width:100%;"
"padding:15px;"
"background:#667eea;"
"color:white;"
"border:none;"
"border-radius:12px;"
"font-size:18px;"
"cursor:pointer;"
"}"
"button:hover{"
"background:#5563d6;"
"}"
".footer{"
"margin-top:20px;"
"font-size:14px;"
"color:#888;"
"}"
"</style>"
"</head>"
"<body>"

"<div class='card'>"

"<h1>🚀 MiniLinks</h1>"
"<p class='subtitle'>Turn giant URLs into tiny superheroes.</p>"

"<form method='POST' action='/shorten'>"

"<input "
"type='text' "
"name='url' "
"placeholder='Paste your giant URL here...' "
"required>"

"<button type='submit'>✨ Shorten URL</button>"

"</form>"

"<div class='footer'>"
"Warning: May cause excessive link sharing 😎"
"</div>"

"</div>"

"</body>"
"</html>");
    }
}

int main(void) {
    srand(time(NULL));
    struct mg_mgr mgr;

    mg_mgr_init(&mgr);

    const char *port = getenv("PORT");
char addr[100];

sprintf(addr, "http://0.0.0.0:%s", port ? port : "8000");

mg_http_listen(&mgr, addr, fn, NULL);

    printf("Server running on http://localhost:8000\n");

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
    return 0;
}