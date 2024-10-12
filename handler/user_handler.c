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
    } else if (mg_strcmp(http_msg->uri, mg_str("/user/auth")) == 0) {
        return user_auth_by_pwd_handler(conn, http_msg);
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
    if (!cJSON_IsString(username) || username->valuestring == NULL) { // double check in case the string is empty
        MG_ERROR(("Error reading `username` field in json in /user/create"));
        return default_400(conn);
    }
    cJSON *pwd = cJSON_GetObjectItemCaseSensitive(user_data, "password");
    if (!cJSON_IsString(pwd) || pwd->valuestring == NULL) {
        MG_ERROR(("Error reading `password` field in json in /user/create"));
        return default_400(conn);
    }

    User *user = create_new_user(username->valuestring, strlen(username->valuestring), pwd->valuestring);
    cJSON_Delete(user_data);
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

    int user_json_len = B64_ENCODED_LENGTH(TOKEN_SIZE + SHA256_DIGEST_LENGTH) + TOKEN_SIZE + MAX_USERNAME_LENGTH + 10 + 28;
    int response_len = 3 + users->count * (user_json_len + 1);

    // Allocate memory for response dynamically
    char *response = malloc(response_len);
    if (!response) {
        // Handle memory allocation error
        return default_500(conn);
    }
    response[0] = '[';
    response[1] = '\0';

    _user_node *current = users->head;
    int offset = 1; // Start after the '['

    while (current) {
        char user_json[user_json_len];
        int written = snprintf(user_json, user_json_len,
                               "{"
                               "\"id\": %d,"
                               "\"username\": \"%s\","
                               "\"hash\": \"%s\","
                               "\"token\": \"%s\","
                               "\"salt\": \"%s\""
                               "}",
                               current->user->id,
                               current->user->uname,
                               current->user->pwd_hash,
                               current->user->token,
                               current->user->salt
        );

        if (written < 0 || written >= user_json_len) {
            // Handle snprintf error or overflow
            free(response);
            return default_500(conn);
        }

        // Append user_json to response
        if (offset + written < response_len - 1) {
            memcpy(response + offset, user_json, written);
            offset += written;
            response[offset++] = ',';  // Add comma between user JSON objects
        } else {
            // Handle buffer overflow
            free(response);
            return default_500(conn);
        }

        current = current->next;
    }

    // Replace the last comma with the closing bracket
    if (users->count > 0) {
        response[offset - 1] = ']';  // Overwrite last comma
    } else {
        response[offset++] = ']';
    }
    response[offset] = '\0';

    free_User_list(users);

    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", "%s", response);
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

    cJSON *token = cJSON_GetObjectItemCaseSensitive(user_data, "token");
    if (!cJSON_IsString(token) || token->valuestring == NULL) {
        MG_ERROR(("Error parsing `token` field of JSON in /user/delete"));
        cJSON_Delete(user_data);
        return default_400(conn);
    }

    int rc = remove_user_by_token(token->valuestring);
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

void user_auth_by_pwd_handler(struct mg_connection *conn, struct mg_http_message *http_msg) {
    if (mg_strcmp(http_msg->method, mg_str("POST"))) {
        return default_405(conn);
    }

    cJSON *user_data = cJSON_ParseWithLength(http_msg->body.buf, http_msg->body.len);
    if (user_data == NULL) {
        MG_ERROR(("Error parsing JSON in /user/getbyid"));
        return default_400(conn);
    }

    cJSON *username = cJSON_GetObjectItemCaseSensitive(user_data, "username");
    if (!cJSON_IsString(username) || username->valuestring == NULL) {
        MG_ERROR(("Error parsing `username` field of JSON in /user/auth"));
        cJSON_Delete(user_data);
        return default_400(conn);
    }

    cJSON *password = cJSON_GetObjectItemCaseSensitive(user_data, "password");
    if (!cJSON_IsString(password) || password->valuestring == NULL) {
        MG_ERROR(("Error parsing `password` field of JSON in /user/auth"));
        cJSON_Delete(user_data);
        return default_400(conn);
    }

    char token[B64_ENCODED_LENGTH(TOKEN_SIZE)];
    int rc = get_token_by_pwd(token, username->valuestring, password->valuestring);
    cJSON_Delete(user_data);

    if (rc == NO_AFFECTED_ROWS) {
        MG_ERROR(("Error authenticating user in /user/auth - invalid credentials"));
        return default_401(conn);
    } else if (rc == DB_NO_RESULT) {
        MG_ERROR(("Error fetching user: no entry found in /user/auth"));
        return default_404(conn);
    } else if (rc != OK) {
        MG_ERROR(("Error constructing user variable in /user/auth, error code: %d", rc));
        return default_500(conn);
    }

    char response[256];
    snprintf(response, sizeof(response), "{\"token\": \"%s\"}", token);

    mg_http_reply(conn, 200, "Content-Type: application/json\n\r", response);
    conn->is_draining = 1;
}
