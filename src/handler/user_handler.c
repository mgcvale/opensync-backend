#include "user_handler.h"

#include <err.h>
#include <string.h>
#include <stdio.h>
#include "defaults.h"
#include "../util/util.h"
#include "../service/user_service.h"
#include "../model/user.h"
#include <kore/kore.h>
#include <kore/http.h>
#include <lib/yyjson.h>

int user_create_handler(struct http_request *req) {
    kore_log(LOG_INFO, "user_create_handler called with body %s", (char*) req->http_body->data);
    if (req->method != HTTP_METHOD_POST) {
        default_405(req);
        return KORE_RESULT_OK;
    }

    yyjson_doc *doc = yyjson_read((char*) req->http_body->data, req->http_body->length, 0);
    if (!doc) {
        kore_log(LOG_INFO, "Error parsing json in user_create_handler: yyjson_doc was null");
        default_400(req);
        return KORE_RESULT_OK;
    }

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_obj(root)) {
        kore_log(LOG_INFO, "Error parsing json in user_create_handler: yyjson_doc_get_root is null - invalid json format");
        default_400(req);
        yyjson_doc_free(doc);
        return KORE_RESULT_OK;
    }

    const char *uname = yyjson_get_str(yyjson_obj_get(root, "username"));
    const char *password = yyjson_get_str(yyjson_obj_get(root, "password"));

    if (!uname || !password) {
        kore_log(LOG_INFO, "Error parsing json in user_create_handler: uname or pwd is null: username is null: %d, pwd is null: %i", uname == NULL, password == NULL);
        default_400(req);
        yyjson_doc_free(doc);
        /*
        if (uname) free(uname);
        if (password) free(password);
        */
        return KORE_RESULT_OK;
    }

    User *user = create_new_user(uname, strlen(uname), password);
    if (user == NULL) {
        kore_log(LOG_INFO, "Error creating new user in /user/create");
        default_500(req);
        return KORE_RESULT_OK;
    }
    int rc = add_user(user);

    if (rc == ERR_DB_CONFLICT) {
        kore_log(LOG_INFO, "Conflict when creating user in user_create_handler");
        default_409(req);
        return KORE_RESULT_OK;
    }
    if (rc != OK) {
        kore_log(LOG_INFO, "Unknown error when creating user in user_create_handler, ec: %d", rc);
        default_500(req);
        return KORE_RESULT_OK;
    }

    char response[256];
    snprintf(response, 256, "{\"message\": \"success\", \"token\": \"%s\"}", user->token);
    http_response(req, 200, response, strlen(response));
    kore_log(LOG_INFO, "sent");
    return KORE_RESULT_OK;
}
/*
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
*/