#ifndef OPENSYNCOPENSYNC_HANDLER_H
#define OPENSYNCOPENSYNC_HANDLER_H

#include <mongoose.h>

void log_api(const char *url, const char *method);

static void build_request(struct mg_connection *conn, int response_code, const char *content_type, const char *body);

void handler(struct mg_connection *conn, int ev, void *ev_data);

#endif
