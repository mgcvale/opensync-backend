#ifndef OPENSYNC_USER_HANDLER_H
#define OPENSYNC_USER_HANDLER_H

#include <kore/kore.h>
#include <kore/http.h>

int user_create_handler(struct http_request *);
int user_delete_handler(struct http_request *);
int user_auth_by_pwd_handler(struct http_request *);
int user_auth_by_token_handler(struct http_request *);
int user_gettoken_handler(struct http_request *);

#endif