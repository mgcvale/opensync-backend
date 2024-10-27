#include "user_handler.h"
#include <string.h>
#include <stdio.h>
#include "mongoose.h"
#include "defaults.h"
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

    char *uname = mg_json_get_str(http_msg->body, "$.username");
    char *password = mg_json_get_str(http_msg->body, "$.password");
    if (uname == NULL || password == NULL) {
        MG_ERROR(("Error reading fields in /user/create.\n"));
    }

    User *user = create_new_user(uname, strlen(uname), password);
    free(uname);
    free(password);
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

    char response[256];
    snprintf(response, 256, "{\"message\": \"success\", \"token\": \"%s\"}", user->token);
    mg_http_reply(conn, 200, "Content-Type: application/json\n\r", response);
    conn->is_draining = 1;
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

    char *uname = mg_json_get_str(http_msg->body, "$.username");
    char *password = mg_json_get_str(http_msg->body, "$.password");
    if (uname == NULL || password == NULL) {
        MG_ERROR(("Error reading fields in /user/create.\n"));
    }

    User *user = NULL;
    int rc = auth_user_by_pwd(&user, uname, password);

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

    char *user_json = to_json_string(user);
    free_user(user);
    if (user_json == NULL) {
        MG_ERROR(("Error jsonifying result user in /user/getbytoken"));
        return default_500(conn);
    }

    mg_http_reply(conn, 200, "Content-Type: application/json\n\r", user_json);
    free(user_json);
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
        return default_401(conn);
    }

    char *user_json = to_json_string(user);
    free_user(user);
    if (user_json == NULL) {
        MG_ERROR(("Error jsonifying result user in /user/getbytoken"));
        return default_500(conn);
    }

    mg_http_reply(conn, 200, "Content-Type: application/json\n\r", user_json);
    free(user_json);
    conn->is_draining = 1;
}


void user_gettoken_handler(struct mg_connection* conn, struct mg_http_message* http_msg) {
    if (mg_strcmp(http_msg->method, mg_str("POST"))) {
        return default_405(conn);
    }

    char *uname = mg_json_get_str(http_msg->body, "$.username");
    char *password = mg_json_get_str(http_msg->body, "$.password");
    if (uname == NULL || password == NULL) {
        MG_ERROR(("Error reading fields in /user/create.\n"));
    }

    char token[B64_ENCODED_LENGTH(TOKEN_SIZE)];
    int rc = get_token_by_pwd(token, uname, password);
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
