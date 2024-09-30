#include "handler.h"
#include "user_handler.h"
#include "../util/util.h"

#include <microhttpd.h>
#include <string.h>
#include <stdio.h>

void log_api(const char* url, const char* method) {
    printf("Request: %s %s\n", method, url);
}

enum MHD_Result handler(void *cls, struct MHD_Connection *connection,
                          const char *url, const char *method,
                          const char *version, const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {

    log_api(url, method);

    if (prefix("/user", url)) {
        return root_user_handler(cls, connection, url, method, upload_data, upload_data_size, con_cls);
    }

    const char *response_str = "<html><body><h1>Hello, World!</h1></body></html>";
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(response_str), (void*)response_str, MHD_RESPMEM_PERSISTENT);
    if (!response)
        return MHD_NO;

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}

