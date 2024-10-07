#ifndef HYPRSYNC_BACKEND_USER_HANDLER_H
#define HYPRSYNC_BACKEND_USER_HANDLER_H

#include "mongoose.h"

void root_user_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_create_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_getall_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_delete_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_get_by_id_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_auth_by_pwd_handler(struct mg_connection *conn, struct mg_http_message *http_msg);

#endif //HYPRSYNC_BACKEND_USER_HANDLER_H
