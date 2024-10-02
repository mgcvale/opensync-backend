#include "user_handler.h"
#include <string.h>
#include <stdio.h>
#include "mongoose.h"
#include "defaults.h"
#include "cjson/cJSON.h"
#include "../service/user_service.h"
#include "../model/user.h"
#include <sqlite3.h>
#include "../service/database.h"

void root_user_handler(struct mg_connection* conn, struct mg_http_message *http_msg) {
    if (!mg_strcmp(http_msg->uri, mg_str("/user/create"))) {
        return user_create_handler(conn, http_msg);
    }

    return default_404(conn);
}


void user_create_handler(struct mg_connection* conn, struct mg_http_message *http_msg) {
    if (mg_strcmp(http_msg->method, mg_str("POST"))) {
        return default_405(conn);
    }

    int rc;
    cJSON *user_data = cJSON_ParseWithLength(http_msg->body.buf, http_msg->body.len);
    const cJSON *username = NULL;

    if (user_data == NULL) {
        fprintf(stderr, "Error reading json in /user/create");
        return default_400(conn);
    }

    username = cJSON_GetObjectItemCaseSensitive(user_data, "username");
    if (!cJSON_IsString(username) || username->valuestring == NULL) {
        fprintf(stderr, "Error reading `username` field in json in /user/create");
        return default_400(conn);
    }

    User *user = create_user(username->valuestring, strlen(username->valuestring));
    rc = add_user(user);

    if (rc != OK) {
        fprintf(stderr, "Error connecting to database in /user/create");
        return default_500(conn);
    }

    cJSON_Delete(user_data);
    return default_200(conn);
}





