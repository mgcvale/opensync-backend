#include "defaults.h"
#include <string.h>
#include "mongoose.h"

void default_400(struct mg_connection* conn) {
    mg_http_reply(conn, 400, "Content-Type: application/json\r\n", "{\"error\":\"BAD_REQUEST\"}");
}

void default_404(struct mg_connection* conn) {
    mg_http_reply(conn, 404, "Content-Type: application/json\r\n", "{\"error\": \"NOT_FOUND\"}");
}

void default_405(struct mg_connection* conn) {
    mg_http_reply(conn, 405, "Content-Type: application/json\r\n", "{\"error\": \"METHOD_NOT_ALLOWED\"}");
}

void default_200(struct mg_connection* conn) {
    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", "{\"message\": \"SUCCESS\"}");
}

void default_500(struct mg_connection* conn) {
    mg_http_reply(conn, 500, "Content-Type: application/json\r\n", "{\"message\": \"INTERNAL_SERVER_ERROR\"}");
}

void default_409(struct mg_connection* conn) {
    mg_http_reply(conn, 409, "Content-Type: application/json\r\n", "{\"message\": \"CONFLICT\"}");
}

void default_401(struct mg_connection* conn) {
    mg_http_reply(conn, 401, "Content-Type: application/json\r\n", "{\"message\": \"UNAUTHORIZED\"}");
}
