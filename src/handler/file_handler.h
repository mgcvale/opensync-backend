#ifndef OPENSYNC_FILE_HANDLER_H
#define OPENSYNC_FILE_HANDLER_H

#include <kore/kore.h>
#include <kore/http.h>

int file_create_handler(struct http_request *);
int file_delete_handler(struct http_request *);
int file_get_by_name_handler(struct http_request *);
int file_get_by_id_handler(struct http_request *);
int file_getall_handler(struct http_request *);
int file_getall_preview_handler(struct http_request *);

#endif //OPENSYNC_FILE_HANDLER_H
