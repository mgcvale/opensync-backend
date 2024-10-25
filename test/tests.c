#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "unity.h"
#include "util.h"
#include "tests.h"

// response_body and response_code should NOT be null. They should be allocated, either on the stack or on the heap.
static CURLcode send_post(const char *url, const char *json, long *response_code, char *response_body, struct curl_slist *headers) {
    CURL *curl = NULL;
    curl = curl_easy_init();

    headers = curl_slist_append(headers, "Content-Type: application/json");

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (response_body != NULL) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_body);
        }
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_code);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return res;
    } else {
        return -1;
    }
}

void test_addUserShouldReturnSuccess() {
    printf("Beggining tests\n");
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    const char json[] = "{\"username\": \"userAddTest\", \"password\": \"userAddTest\"}";
    long response_code;

    int res = send_post(full_url, json, &response_code, NULL, NULL);

    char message[128];
    snprintf(message, 128, "cURL didn't return success. CURLcode: %d\n", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    message[0] = '\0';
    snprintf(message, 128, "/user/create didn't return 200 OK. return code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);
}

void test_addUserConflictsWhenUsernameIsAlreadyTaken() {
    printf("\n");
    const char json[] = "{\"username\": \"userConflictTest\", \"password\": \"userConflictTest\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code;

    int res = send_post(full_url, json, &response_code, NULL, NULL);

    char message[256];
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d\n", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating first user in test_addUserConflictsWhenUsernameIsAlreadyTaken. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    // next user addition should return 409 CONFLICT
    res = send_post(full_url, json, &response_code, NULL, NULL);
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d\n", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 409 CONFLICT when creating user with already taken usrname in test_addUserConflictsWhenUsernameIsAlreadyTaken. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code == 409, __LINE__, message);
}

void test_userGetByPasswordSucceedsWhenPasswordIsCorrect() {
    printf("\n");
    const char json[] = "{\"username\": \"userLoginTest1\", \"password\": \"userLoginTest1\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code = 502;

    int res = send_post(full_url, json, &response_code, NULL, NULL);

    char message[256];
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating user in test_userGetByPasswordSucceedsWhenPasswordIsCorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    // try to authenticate user
    full_url[0] = '\0';
    snprintf(full_url, 64, "%s%s", url, "user/getbypwd");
    res = send_post(full_url, json, &response_code, NULL, NULL);
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d\n", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/getbypwd didn't return 200 OK when authenticating user in test_userGetByPasswordSucceedsWhenPasswordIsCorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);
}

void test_userGetByPasswordFailsWhenPasswordIsIncorrect() {
    printf("\n");
    char json[] = "{\"username\": \"userLoginTest2\", \"password\": \"userLoginTest2\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code = 502;

    int res = send_post(full_url, json, &response_code, NULL, NULL);

    char message[256];
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating user in test_userGetByPasswordFailsWhenPasswordIsIncorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    // try to authenticate user (with wrong password);
    strncpy(json, "{\"username\": \"userLoginTest2\", \"password\": \"userLoginTest0\"}", strlen(json));
    full_url[0] = '\0';
    snprintf(full_url, 64, "%s%s", url, "user/getbypwd");
    res = send_post(full_url, json, &response_code, NULL, NULL);
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/getbypwd didn't return 401 UNAUTHORIZED when authenticating user in test_userGetByPasswordFailsWhenPasswordIsIncorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code == 401, __LINE__, message);
}

void test_userGetByTokenSucceedsWhenTokenIsCorrect() {
    printf("\n");
    char json[128] = "{\"username\": \"userGetByTokenTest1\", \"password\": \"userGetByTokenTest1\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code = 502;
    char response[256] = {0};
    char message[256];

    int res = send_post(full_url, json, &response_code, response, NULL);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating user in test_userGetByTokenSucceedsWhenTokenIsCorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    cJSON *response_json = cJSON_ParseWithLength(response, strlen(response));
    UNITY_TEST_ASSERT(response_json != NULL, __LINE__, "Failed to parse /user/create JSON in test_userGetByTokenSucceedsWhenTokenIsCorrect; cJSON object is NULL.");
    cJSON *token = cJSON_GetObjectItemCaseSensitive(response_json, "token");
    if (!cJSON_IsString(token) || token->valuestring == NULL) {
        cJSON_Delete(response_json);
        UNITY_TEST_FAIL(__LINE__, "Failed to parse `token` field in response json from /user/create in test_userGetByTokenSucceedsWhenTokenIsCorrect");
    }

    char auth_header[128];
    snprintf(auth_header, 128, "Authorization: Bearer %s", token->valuestring);
    printf("%s\n", auth_header);
    cJSON_Delete(response_json);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, auth_header);
    full_url[0] = '\0';
    snprintf(full_url, 64, "%s%s", url, "user/getbytoken");

    res = send_post(full_url, json, &response_code, NULL, headers);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/getbytoken didn't return 200 OK when authenticating user in test_userGetByTokenSucceedsWhenTokenIsCorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code == 200, __LINE__, message);
}


void test_userGetByTokenFailsWhenTokenIsIncorrect () {
    printf("\n");
    char json[128] = "{\"username\": \"userGetByTokenTest2\", \"password\": \"userGetByTokenTest2\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code = 502;
    char message[256];

    int res = send_post(full_url, json, &response_code, NULL, NULL);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating user in test_userGetByTokenFailsWhenTokenIsIncorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);
    char auth_header[] = "Authorization: Bearer 123123123";
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, auth_header);
    full_url[0] = '\0';
    snprintf(full_url, 64, "%s%s", url, "user/getbytoken");

    res = send_post(full_url, json, &response_code, NULL, headers);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/getbytoken didn't return 401 UNAUTHORIZED when authenticating user in test_userGetByTokenFailsWhenTokenIsIncorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code == 401, __LINE__, message);
}

void test_userGetByTokenFailsWhenTokenIsMissing() {
    printf("\n");
    char json[128] = "{\"username\": \"userGetByTokenTest3\", \"password\": \"userGetByTokenTest3\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code = 502;
    char message[256];

    int res = send_post(full_url, json, &response_code, NULL, NULL);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating user in test_userGetByTokenFailsWhenTokenIsMissing. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);
    full_url[0] = '\0';
    snprintf(full_url, 64, "%s%s", url, "user/getbytoken");

    res = send_post(full_url, json, &response_code, NULL, NULL);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/getbytoken didn't return 401 UNAUTHORIZED when authenticating user in test_userGetByTokenFailsWhenTokenIsMissing. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code == 401, __LINE__, message);
}


void test_userAuthenticationSucceedsWhenPasswordIsCorrect() {
    printf("\n");
    char json[128] = "{\"username\": \"userAuthTest1\", \"password\": \"userAuthTest1\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code = 502;
    char response[256] = {0};
    char message[256];

    int res = send_post(full_url, json, &response_code, response, NULL);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating user in test_userAuthenticationSucceedsWhenPasswordIsCorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    cJSON *response_json = cJSON_ParseWithLength(response, strlen(response));
    UNITY_TEST_ASSERT(response_json != NULL, __LINE__, "Failed to parse /user/create JSON in test_userAuthenticationSucceedsWhenPasswordIsCorrect; cJSON object is NULL.");
    cJSON *token = cJSON_GetObjectItemCaseSensitive(response_json, "token");
    if (!cJSON_IsString(token) || token->valuestring == NULL) {
        cJSON_Delete(response_json);
        UNITY_TEST_FAIL(__LINE__, "Failed to parse `token` field in response json from /user/create in test_userAuthenticationSucceedsWhenPasswordIsCorrect");
    }
    char actualToken[32];
    strncpy(actualToken, token->valuestring, 25);
    actualToken[24] = '\0';
    cJSON_Delete(response_json);

    full_url[0] = '\0';
    snprintf(full_url, 64, "%s%s", url, "user/auth");
    response[0] = '\0';

    // same json cuz the password will be the same
    res = send_post(full_url, json, &response_code, response, NULL);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/auth didn't return 200 OK when authenticating user in test_userAuthenticationSucceedsWhenPasswordIsCorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    response_json = cJSON_ParseWithLength(response, strlen(response));
    UNITY_TEST_ASSERT(response_json != NULL, __LINE__, "Failed to parse /user/create JSON in test_userAuthenticationSucceedsWhenPasswordIsCorrect; cJSON object is NULL.");
    token = cJSON_GetObjectItemCaseSensitive(response_json, "token");
    if (!cJSON_IsString(token) || token->valuestring == NULL) {
        cJSON_Delete(response_json);
        UNITY_TEST_FAIL(__LINE__, "Failed to parse `token` field in response json from /user/create in test_userAuthenticationSucceedsWhenPasswordIsCorrect");
    }

    message[0] = '\0';
    snprintf(message, 256, "Token from /user/auth and token from /user/add aren't equal: %s != %s", token->valuestring, actualToken);
    UNITY_TEST_ASSERT(strncmp(token->valuestring, actualToken, 25) == 0, __LINE__, message);
    cJSON_Delete(response_json);
}

void test_userAuthenticationFailsWhenPasswordIsIncorrect() {
    printf("\n");
    char json[128] = "{\"username\": \"userAuthTest2\", \"password\": \"userAuthTest2\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code = 502;
    char response[256] = {0};
    char message[256];

    int res = send_post(full_url, json, &response_code, response, NULL);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating user in test_userAuthenticationFailsWhenPasswordIsIncorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    cJSON *response_json = cJSON_ParseWithLength(response, strlen(response));
    UNITY_TEST_ASSERT(response_json != NULL, __LINE__, "Failed to parse /user/create JSON in test_userAuthenticationFailsWhenPasswordIsIncorrect; cJSON object is NULL.");
    cJSON *token = cJSON_GetObjectItemCaseSensitive(response_json, "token");
    if (!cJSON_IsString(token) || token->valuestring == NULL) {
        cJSON_Delete(response_json);
        UNITY_TEST_FAIL(__LINE__, "Failed to parse `token` field in response json from /user/create in test_userAuthenticationFailsWhenPasswordIsIncorrect");
    }
    char actualToken[32];
    strncpy(actualToken, token->valuestring, 25);
    actualToken[24] = '\0';
    cJSON_Delete(response_json);

    full_url[0] = '\0';
    snprintf(full_url, 64, "%s%s", url, "user/auth");
    response[0] = '\0';

    // change json password
    json[strlen(json) - 4] = '.';
    res = send_post(full_url, json, &response_code, response, NULL);

    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/auth didn't return 401 UNAUTHORIZED when authenticating user in test_userAuthenticationFailsWhenPasswordIsIncorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code == 401, __LINE__, message);

    response_json = cJSON_ParseWithLength(response, strlen(response));
    UNITY_TEST_ASSERT(response_json != NULL, __LINE__, "Failed to parse /user/create JSON in test_userAuthenticationFailsWhenPasswordIsIncorrect; cJSON object is NULL.");
    token = cJSON_GetObjectItemCaseSensitive(response_json, "token");
    printf("%i\n", token == NULL);
    UNITY_TEST_ASSERT(token == NULL, __LINE__, "`token` field in /user/auth response body exists in test_userAuthenticationFailsWhenPasswordIsIncorrect");
    cJSON_Delete(response_json);
}

