#include "defaults.h"
#include <microhttpd.h>
#include <string.h>


// TODO: remove strlen() for better performance

enum MHD_Result default_400(struct MHD_Connection* connection) {
    const char* response_str = "Bad Request (BAD_REQUEST 400)";
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(response_str), (void*)response_str, MHD_RESPMEM_PERSISTENT);
    if(!response) {
        return MHD_NO;
    }

    ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
    MHD_destroy_response(response);
    return ret;
}

enum MHD_Result default_405(struct MHD_Connection* connection) {
    const char* response_str = "Method Not Allowed (METHOD_NOT_ALLOWED 405)";
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(response_str), (void*)response_str, MHD_RESPMEM_PERSISTENT);
    if(!response) {
        return MHD_NO;
    }

    ret = MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response);
    MHD_destroy_response(response);
    return ret;
}

enum MHD_Result default_404(struct MHD_Connection* connection) {
    const char* response_str = "Resource not Found (NOT_FOUND 404)";
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(response_str), (void*)response_str, MHD_RESPMEM_PERSISTENT);
    if(!response) {
        return MHD_NO;
    }

    ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}
