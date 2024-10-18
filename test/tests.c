#include <curl/curl.h>
#include "unity.h"

void test_addUserShouldReturnSuccess() {
    const char username[] = "userAddTest";
    const char password[] = "userAddTest";

    CURL *curl = NULL;
    curl = curl_easy_init();
    if (curl) {
        struct curl_slitst *headers = NULL;
        char
    }
}
