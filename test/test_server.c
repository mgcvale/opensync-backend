#include <curl/curl.h>
#include "unity.h"
#include "tests.c"
#include <string.h>
#include <stdlib.h>

static char url[64] = "http://localhost:8082";

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((char *)userp)[size * nmemb] = 0;
    strncat(userp, contents, size * nmemb);
    return size * nmemb;
}

void setUp(void) {
}

void tearDown(void) {
}

int main(int argc, char *argv[]) {

    // first, test if server is up
    CURL *curl = NULL;
    CURLcode res;
    char response[128] = {0};

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, strncat(url, "/", 1));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (CURLE_OK != res) {
            fprintf(stderr, "Server is not up; response code on index wasn't 200! Aborting...\n");
            return -1;
        }
    } else {
        fprintf(stderr, "Failed initializing curl; can't test if server is up or not. Aborting...\n");
        return -2;
    }



    return 0;
}
