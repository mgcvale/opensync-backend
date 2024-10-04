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
    if (mg_strcmp(http_msg->uri, mg_str("/user/create")) == 0) {
        return user_create_handler(conn, http_msg);
    } else if (mg_strcmp(http_msg->uri, mg_str("/user/getall")) == 0) {
        return user_getall_handler(conn, http_msg);
    } else if (mg_strcmp(http_msg->uri, mg_str("/user/delete")) == 0) {
        return user_delete_handler(conn, http_msg);
    }

    return default_404(conn);
}


void user_create_handler(struct mg_connection* conn, struct mg_http_message *http_msg) {
    if (mg_strcmp(http_msg->method, mg_str("POST"))) {
        return default_405(conn);
    }

    cJSON *user_data = cJSON_ParseWithLength(http_msg->body.buf, http_msg->body.len);
    if (user_data == NULL) {
        MG_ERROR(("Error reading json in /user/create"));
        return default_400(conn);
    }

    cJSON *username = cJSON_GetObjectItemCaseSensitive(user_data, "username");
    cJSON_Delete(user_data);
    if (!cJSON_IsString(username) || username->valuestring == NULL) { // double check in case the string is empty
        MG_ERROR(("Error reading `username` field in json in /user/create"));
        return default_400(conn);
    }

    User *user = create_new_user(username->valuestring, strlen(username->valuestring));
    if (user == NULL) {
        MG_ERROR(("Error creating new user in /user/create"));
        return default_500(conn);
    }
    int rc = add_user(user);
    free_user(user);

    if (rc == ERR_DB_CONFLICT) {
        MG_ERROR(("Conflict when creating user in /user/create"));
        return default_409(conn);
    } else if (rc != OK) {
        MG_ERROR(("Error connecting to database in /user/create"));
        return default_500(conn);
    }

    free_user(user);
    return default_200(conn);
}

void user_getall_handler(struct mg_connection* conn, struct mg_http_message* http_msg) {
    if (mg_strcmp(http_msg->method, mg_str("GET"))) {
        return default_405(conn);
    }

    User_list *users = NULL;
    int rc = get_users_as_list(&users);
    if (rc != OK) {
        MG_ERROR(("Error connecting to database in /user/getall"));
        if (users != NULL) {
            free(users);
        }
        return default_500(conn);
    } else if (users == NULL) {
        MG_ERROR(("Error on user list creation in /user/getall"));
        return default_500(conn);
    }

    cJSON *list = jsonify_list(*users);
    const char *response = cJSON_PrintUnformatted(list);

    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", "%s", response);
    cJSON_Delete(list);
    free(users);
}

void user_delete_handler(struct mg_connection *conn, struct mg_http_message *http_msg) {
    if (mg_strcmp(http_msg->method, mg_str("DELETE"))) {
        return default_405(conn);
    }

    cJSON *user_data = cJSON_ParseWithLength(http_msg->body.buf, http_msg->body.len);
    if (user_data == NULL) {
        MG_ERROR(("Error parsing JSON in /user/delete"));
        return default_400(conn);
    }

    cJSON *id = cJSON_GetObjectItemCaseSensitive(user_data, "id");
    if (!cJSON_IsNumber(id)) {
        MG_ERROR(("Error parsing `id` field of JSON in /user/delete"));
        cJSON_Delete(user_data);
        return default_400(conn);
    }

    int rc = remove_user_by_id(cJSON_GetNumberValue(id));
    cJSON_Delete(user_data);
    if (rc == NO_AFFECTED_ROWS) {
        MG_ERROR(("No user found in /user/delete"));
        return default_404(conn);
    } else if (rc != OK) {
        MG_ERROR(("Error connecting to database in /user/delete"));
        return default_500(conn);
    }

    return default_200(conn);
}



