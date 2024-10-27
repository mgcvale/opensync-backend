#include "file_handler.h"
#include "mongoose.h"
#include "defaults.h"
#include "../util/util.h"
#include "../model/user.h"
#include "../service/user_service.h"
#include "../service/file_service.h"
#include "../service/database.h"
#include <cjson/cJSON.h>

void root_file_handler(struct mg_connection* conn, struct mg_http_message* http_msg) {
    const struct mg_str uri = http_msg->uri;
    if (mg_strcmp(uri, mg_str("/file/create")) == 0) {
        return file_create_handler(conn, http_msg);
    } else if (mg_strcmp(uri, mg_str("/file/getall")) == 0) {
        return file_getall_handler(conn, http_msg);
    } else if (mg_strcmp(uri, mg_str("/file/getbyid")) == 0) {
        return file_get_by_id_handler(conn, http_msg);
    }
}

void file_create_handler(struct mg_connection* conn, struct mg_http_message* http_msg) {
    if (mg_strcmp((http_msg->method), mg_str("POST")) != 0) {
        return default_405(conn);
    }

    struct mg_str *content_type = mg_http_get_header(http_msg, "Content-Type");
    if (content_type == NULL || mg_strcmp(*content_type, mg_str("multipart/form-data")) != 0) {
        mg_log("Content-Type: %s", content_type->buf);
        mg_http_reply(conn, 400, "Content-Type: application/json\r\n",
                      "{\"error\":\"Content-Type must be multipart/form-data\"}\n");
        return;
    }

    // get authorization bearer
    char token[25];
    if (!extract_token(http_msg, token, 25)) {
        MG_ERROR(("Error gathering token bearer in Authentication header."));
        return default_401(conn);
    }

    // auth user from token
    User *user = NULL;
    int auth_result = auth_user_by_token(&user, token);
    if (auth_result != OK) {
        if (auth_result == DB_NO_RESULT) {
            MG_ERROR(("Invalid token in /file/create"));
            return default_401(conn);
        } else {
            MG_ERROR(("Database error in /file/create: %d", auth_result));
            return default_500(conn);
        }
    }

    char filename[256];
    struct mg_http_part part;
    unsigned char *file_data = NULL;
    size_t file_size = 0;
    bool file_found = false;

    // Process each part of the multipart form
    for (size_t ofs = 0; (ofs = mg_http_next_multipart(http_msg->body, ofs, &part)) > 0;) {
        if (part.filename.len > 0) {  // This is a file
            mg_snprintf(filename, sizeof(filename), "%.*s",
                        (int) part.filename.len, part.filename.buf);

            file_data = malloc(part.body.len);
            if (file_data == NULL) {
                free_user(user);
                mg_http_reply(conn, 500, "Content-Type: application/json\r\n",
                              "{\"error\":\"Memory allocation failed\"}\n");
                return;
            }

            // Copy file data
            memcpy(file_data, part.body.buf, part.body.len);
            file_size = part.body.len;
            file_found = true;
            break;  // only process the first file cuz yes
        }
    }

    // Validate file was found
    if (!file_found || file_size == 0) {
        free_user(user);
        if (file_data != NULL) free(file_data);
        mg_http_reply(conn, 400, "Content-Type: application/json\r\n",
                      "{\"error\":\"No file provided\"}\n");
        return;
    }

    int result = save_file(file_data, file_size, filename, user->uname);
    free(file_data);
    free_user(user);

    if (result != OK) {
        MG_ERROR(("Result not ok here aaaa im starting to go insane"));
        return default_500(conn);
    }

    // TODO: add metadata to database

    return default_200(conn);
}

void file_getall_handler(struct mg_connection* conn, struct mg_http_message* http_msg) {

}

void file_get_by_id_handler(struct mg_connection* conn, struct mg_http_message* http_msg) {

}



