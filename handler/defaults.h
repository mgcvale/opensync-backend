#ifndef HYPRSYNC_BACKEND_DEFAULTS_H
#define HYPRSYNC_BACKEND_DEFAULTS_H

#include "mongoose.h"

void default_404(struct mg_connection *conn);

void default_400(struct mg_connection *conn);

void default_405(struct mg_connection *conn);

void default_200(struct mg_connection *conn);

void default_500(struct mg_connection *conn);

#endif //HYPRSYNC_BACKEND_DEFAULTS_H
