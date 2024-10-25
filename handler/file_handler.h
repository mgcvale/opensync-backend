#ifndef OPENSYNC_FILE_HANDLER_H
#define OPENSYNC_FILE_HANDLER_H

#include "mongoose.h"

void root_file_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void file_create_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void file_delete_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void file_get_by_name_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void file_get_by_id_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void file_getall_handler(struct mg_connection *conn, struct mg_http_message *http_msg);
void file_getall_preview_handler(struct mg_connection *conn, struct mg_http_message *http_msg);

#endif //OPENSYNC_FILE_HANDLER_H
