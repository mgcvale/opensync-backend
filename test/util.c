#include <stdlib.h>
#include <string.h>
#include "util.h"
#include <stdio.h>

const char url[64] = "http://localhost:8082/";

size_t wCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    char *response = (char *)userp;

    if (strlen(response) + total_size + 1 >= RESPONSE_BUFFER_SIZE) {
        fprintf(stderr, "Response buffer overflow in wCallback!\n");
        return 0;
    }

    strncat(response, (char *)contents, total_size);
    response[strlen(response)] = '\0';
    return total_size;
}
