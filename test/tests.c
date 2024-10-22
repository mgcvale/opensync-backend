#include <curl/curl.h>
#include "unity.h"
#include "util.h"
#include "tests.h"

// response_body and response_code should NOT be null. They should be allocated, either on the stack or on the heap.
static CURLcode send_post(const char *url, const char *json, long *response_code, char *response_body, const struct curl_slist *headers) {
    CURL *curl = NULL;
    curl = curl_easy_init();

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
        return res;
    } else {
        return -1;
    }
}

void test_addUserShouldReturnSuccess() {
    printf("\n");
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

void test_addUserShouldReturnConflictWhenUsernameIsTaken() {
    printf("\n");
    const char json[] = "{\"username\": \"userConflictTest\", \"password\": \"userConflictTest\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code;

    int res = send_post(full_url, json, &response_code, NULL, NULL);

    char message[256];
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d\n", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating first user in test_addUserShouldReturnConflictWhenUsernameIsTaken. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    // next user addition should return 409 CONFLICT
    res = send_post(full_url, json, &response_code, NULL, NULL);
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d\n", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 409 CONFLICT when creating user with already taken usrname in test_addUserShouldReturnConflictWhenUsernameIsTaken. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code == 409, __LINE__, message);
}

void test_userLoginSucceedsWhenPasswordIsCorrect() {
    printf("\n");
    const char json[] = "{\"username\": \"userLoginTest1\", \"password\": \"userLoginTest1\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code = 502;

    int res = send_post(full_url, json, &response_code, NULL, NULL);

    char message[256];
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating user in test_userLoginSucceedsWhenPasswordIsCorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    // try to authenticate user
    full_url[0] = '\0';
    snprintf(full_url, 64, "%s%s", url, "user/getbypwd");
    res = send_post(full_url, json, &response_code, NULL, NULL);
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d\n", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/getbypwd didn't return 200 OK when authenticating user in test_userLoginSucceedsWhenPasswordIsCorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);
}

void test_userLoginFailsWhenPasswordIsIncorrect() {
    printf("\n");
    char json[] = "{\"username\": \"userLoginTest2\", \"password\": \"userLoginTest2\"}";
    char full_url[64] = {0};
    snprintf(full_url, 64, "%s%s", url, "user/create");
    long response_code = 502;

    int res = send_post(full_url, json, &response_code, NULL, NULL);

    char message[256];
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/create didn't return 200 OK when creating user in test_userLoginFailsWhenPasswordIsIncorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code / 100 == 2, __LINE__, message);

    // try to authenticate user (with wrong password);
    strncpy(json, "{\"username\": \"userLoginTest2\", \"password\": \"userLoginTest0\"}", strlen(json));
    full_url[0] = '\0';
    snprintf(full_url, 64, "%s%s", url, "user/getbypwd");
    res = send_post(full_url, json, &response_code, NULL, NULL);
    snprintf(message, 256, "cURL didn't return success. CURLcode: %d", res);
    UNITY_TEST_ASSERT(res == CURLE_OK, __LINE__, message);

    snprintf(message, 256, "/user/getbypwd didn't return 401 UNAUTHORIZED when authenticating user in test_userLoginFailsWhenPasswordIsIncorrect. Code: %lo\n", response_code);
    UNITY_TEST_ASSERT(response_code == 401, __LINE__, message);
}


