#ifndef OPENSYNC_USER_HANDLER_H
#define OPENSYNC_USER_HANDLER_H

#include "mongoose.h"

void root_user_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_create_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_delete_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_auth_by_pwd_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_auth_by_token_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void user_gettoken_handler(struct mg_connection *conn, struct mg_http_message *http_msg);

#endif //OPENSYNC_USER_HANDLER_H
