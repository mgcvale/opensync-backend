#ifndef HYPRSYNC_BACKEND_USER_HANDLER_H
#define HYPRSYNC_BACKEND_USER_HANDLER_H

#include <microhttpd.h>

enum MHD_Result root_user_handler(void *cls, struct MHD_Connection *connection,
                            const char *url, const char *method, const char *upload_data,
                            size_t *upload_data_size, void **con_cls);


enum MHD_Result create_user_handler(void *cls, struct MHD_Connection *connection, const char *method, const char *upload_data,
                                    size_t *upload_data_size, void **con_cls);


#endif //HYPRSYNC_BACKEND_USER_HANDLER_H
