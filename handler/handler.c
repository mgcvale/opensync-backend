#include "handler.h"
#include "user_handler.h"
#include "../util/util.h"
#include "defaults.h"

#include "mongoose.h"
#include <string.h>
#include <stdio.h>

static void build_request(struct mg_connection *conn, int response_code, const char *content_type, const char *body) {
    mg_printf(conn,
              "HTTP/1.1 %d OK\r\n"
              "Content-Type: %s\r\n"
              "Content-Length: %d\r\n"
              "\r\n%s",
              response_code, content_type, strlen(body), body);
}

void handler(struct mg_connection *conn, int event_type, void *event_data) {
    if (event_type == MG_EV_HTTP_MSG) {
        struct mg_http_message *http_msg = (struct mg_http_message *) event_data;

        if (prefix("/user/", http_msg->uri.buf)) {
            return root_user_handler(conn, http_msg);
        }

        if (mg_strcmp(http_msg->uri, mg_str("/")) == 0) {
            default_200(conn);
        }

        default_404(conn);
    }
}
