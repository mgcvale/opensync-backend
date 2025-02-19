#include <kore/kore.h>
#include <kore/http.h>
#include "defaults.h"

void default_400(struct http_request *req) {
    http_response_header(req, "Content-Type", "application/json");
    http_response(req, 400, "{\"error\":\"BAD_REQUEST\"}", 22);
}

void default_404(struct http_request *req) {
    http_response_header(req, "Content-Type", "application/json");
    http_response(req, 404, "{\"error\": \"NOT_FOUND\"}", 20);
}

void default_405(struct http_request *req) {
    http_response_header(req, "Content-Type", "application/json");
    http_response(req, 405, "{\"error\": \"METHOD_NOT_ALLOWED\"}", 29);
}

void default_200(struct http_request *req) {
    http_response_header(req, "Content-Type", "application/json");
    http_response(req, 200, "{\"message\": \"SUCCESS\"}", 22);
}

void default_500(struct http_request *req) {
    http_response_header(req, "Content-Type", "application/json");
    http_response(req, 500, "{\"message\": \"INTERNAL_SERVER_ERROR\"}", 34);
}

void default_409(struct http_request *req) {
    http_response_header(req, "Content-Type", "application/json");
    http_response(req, 409, "{\"message\": \"CONFLICT\"}", 21);
}

void default_401(struct http_request *req) {
    http_response_header(req, "Content-Type", "application/json");
    http_response(req, 401, "{\"message\": \"UNAUTHORIZED\"}", 24);
}