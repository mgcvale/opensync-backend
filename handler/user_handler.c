#include "user_handler.h"
#include <string.h>
#include <stdio.h>
#include "mongoose.h"
#include "defaults.h"
#include "cjson/cJSON.h"
#include "../util/util.h"
#include "../service/user_service.h"
#include "../model/user.h"
#include <sqlite3.h>
#include "../service/database.h"

void root_user_handler(struct mg_connection* conn, struct mg_http_message *http_msg) {
    if (mg_strcmp(http_msg->uri, mg_str("/user/create")) == 0) {
        return user_create_handler(conn, http_msg);
    } else if (mg_strcmp(http_msg->uri, mg_str("/user/delete")) == 0) {
        return user_delete_handler(conn, http_msg);
    } else if (mg_strcmp(http_msg->uri, mg_str("/user/getbypwd")) == 0) {
        return user_auth_by_pwd_handler(conn, http_msg);
    } else if (mg_strcmp(http_msg->uri, mg_str("/user/getbytoken")) == 0)  {
        return user_auth_by_token_handler(conn, http_msg);
    } else if (mg_strcmp(http_msg->uri, mg_str("/user/auth")) == 0) {
        return user_gettoken_handler(conn, http_msg);
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

void user_delete_handler(struct mg_connection *conn, struct mg_http_message *http_msg) {
    if (mg_strcmp(http_msg->method, mg_str("DELETE"))) {
        return default_405(conn);
    }

    char token[25];
    if (!extract_token(http_msg, token, 25)) {
        MG_ERROR(("Error gathering token bearer in Authentication header."));
        return default_401(conn);
    }

    int rc = remove_user_by_token(token);
    if (rc == NO_AFFECTED_ROWS) {
        MG_ERROR(("No user found in /user/delete"));
        return default_401(conn);
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
        MG_ERROR(("Error parsing JSON in /user/getbypwd"));
        return default_400(conn);
    }

    cJSON *username = cJSON_GetObjectItemCaseSensitive(user_data, "username");
    if (!cJSON_IsString(username) || username->valuestring == NULL) {
        MG_ERROR(("Error parsing `username` field of JSON in /user/getbypwd"));
        cJSON_Delete(user_data);
        return default_400(conn);
    }

    cJSON *password = cJSON_GetObjectItemCaseSensitive(user_data, "password");
    if (!cJSON_IsString(password) || password->valuestring == NULL) {
        MG_ERROR(("Error parsing `password` field of JSON in /user/getbypwd"));
        cJSON_Delete(user_data);
        return default_400(conn);
    }

    User *user = NULL;
    int rc = auth_user_by_pwd(&user, username->valuestring, password->valuestring);
    cJSON_Delete(user_data);

    if (rc == ERR_INVALID_CREDENTIALS) {
        MG_ERROR(("Error authenticating user in /user/getbypwd - invalid credentials"));
        return default_401(conn);
    } else if (rc == DB_NO_RESULT) {
        MG_ERROR(("Error fetching user: no entry found in /user/getbypwd"));
        return default_404(conn);
    } else if (rc != OK) {
        MG_ERROR(("Error constructing user variable in /user/auth, error code: %d", rc));
        return default_500(conn);
    }

    cJSON *userJson = jsonify_user(user); // TODO: stop using json and create tostring function in user.c instead
    free_user(user);
    if (userJson == NULL) {
        MG_ERROR(("Error jsonifying result user in /user/getbypwd"));
        return default_500(conn);
    }

    char *response = cJSON_PrintUnformatted(userJson);
    mg_http_reply(conn, 200, "Content-Type: application/json\n\r", response);
    cJSON_Delete(userJson);
    free(response);
    conn->is_draining = 1;
}

void user_auth_by_token_handler(struct mg_connection* conn, struct mg_http_message* http_msg) {
    if (mg_strcmp(http_msg->method, mg_str("POST"))) {
        return default_405(conn);;
    }

    char token[25];
    if (!extract_token(http_msg, token, 25)) {
        MG_ERROR(("Error gathering token bearer in Authentication header."));
        return default_401(conn);
    }

    User *user = NULL;
    int rc = auth_user_by_token(&user, token);
    if (rc == DB_NO_RESULT) {
        MG_ERROR(("No user found on /user/getbytoken. Check your token."));
        return default_500(conn);
    }

    cJSON *userJson = jsonify_user(user); // TODO: stop using json and create tostring function in user.c instead
    free_user(user);
    if (userJson == NULL) {
        MG_ERROR(("Error jsonifying result user in /user/getbytoken"));
        return default_500(conn);
    }

    char *response = cJSON_PrintUnformatted(userJson);
    mg_http_reply(conn, 200, "Content-Type: application/json\n\r", response);
    cJSON_Delete(userJson);
    free(response);
    conn->is_draining = 1;
}


void user_gettoken_handler(struct mg_connection* conn, struct mg_http_message* http_msg) {
    if (mg_strcmp(http_msg->method, mg_str("POST"))) {
        return default_405(conn);
    }

    cJSON *user_data = cJSON_ParseWithLength(http_msg->body.buf, http_msg->body.len);
    if (user_data == NULL) {
        MG_ERROR(("Error parsing JSON in /user/auth"));
        return default_400(conn);
    }

    cJSON *uname = cJSON_GetObjectItemCaseSensitive(user_data, "username");
    if (!cJSON_IsString(uname) || uname->valuestring == NULL) {
        MG_ERROR(("Error parsing \"username\" field in JSON in /user/auth"));
        cJSON_Delete(user_data);
        return default_400(conn);
    }

    cJSON *pwd = cJSON_GetObjectItemCaseSensitive(user_data, "password");
    if (!cJSON_IsString(pwd) || pwd->valuestring == NULL) {
        MG_ERROR(("Error parsing \"password\" field in JSON in /user/auth"));
        cJSON_Delete(user_data);
        return default_400(conn);
    }

    char token[B64_ENCODED_LENGTH(TOKEN_SIZE)];
    int rc = get_token_by_pwd(token, uname->valuestring, pwd->valuestring);
    cJSON_Delete(user_data);
    if (rc == ERR_INVALID_CREDENTIALS || rc == DB_NO_RESULT) {
        MG_ERROR(("Invalid credentials or nonexistent user in /user/auth"));
        return default_401(conn);
    } else if (rc != OK) {
        MG_ERROR(("Internal server error in /user/auth; error code: %d", rc));
        return default_500(conn);
    }

    char response[sizeof(token) + 16];
    snprintf(response, sizeof(token) + 16, "{\"token\": \"%s\"}", token);
    mg_http_reply(conn, 200, "Content-Type: application/json\r\n", response);
    conn->is_draining = 1;
}
