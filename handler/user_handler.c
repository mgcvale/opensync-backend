#include "user_handler.h"
#include <string.h>
#include <microhttpd.h>
#include "defaults.h"

enum MHD_Result root_user_handler(void* cls, struct MHD_Connection* connection,
                                 const char* url, const char* method, const char* upload_data,
                                 size_t* upload_data_size, void ** con_cls) {

    if (strcmp("/user/create", url) == 0) {
        return create_user_handler(cls, connection, method, upload_data, upload_data_size, con_cls);
    }
    return default_404(connection);
}

enum MHD_Result create_user_handler(void* cls, struct MHD_Connection* connection, const char* method, const char* upload_data,
                                    size_t* upload_data_size, void ** con_cls) {

    if (strcmp("POST", method) != 0) {
        return default_405(connection);
    }

    char* response_str = "users/create";
    struct MHD_Response *response;
    int ret;

    // process data here

    response = MHD_create_response_from_buffer(strlen(response_str), (void*)response_str, MHD_RESPMEM_PERSISTENT);
    if (!response)
        return MHD_NO;

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
}


