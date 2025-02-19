#ifndef OPENSYNCOPENSYNC_DEFAULTS_H
#define OPENSYNCOPENSYNC_DEFAULTS_H

#include <kore/kore.h>
#include <kore/http.h>

void default_404(struct http_request *req);

void default_400(struct http_request *req);

void default_405(struct http_request *req);

void default_200(struct http_request *req);

void default_500(struct http_request *req);

void default_409(struct http_request *req);

void default_401(struct http_request *req);

#endif //OPENSYNCOPENSYNC_DEFAULTS_H
