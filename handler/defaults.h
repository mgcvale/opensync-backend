#ifndef OPENSYNCOPENSYNC_DEFAULTS_H
#define OPENSYNCOPENSYNC_DEFAULTS_H

#include "mongoose.h"

void default_404(struct mg_connection *conn);

void default_400(struct mg_connection *conn);

void default_405(struct mg_connection *conn);

void default_200(struct mg_connection *conn);

void default_500(struct mg_connection *conn);

void default_409(struct mg_connection *conn);

void default_401(struct mg_connection *conn);

#endif //OPENSYNCOPENSYNC_DEFAULTS_H
