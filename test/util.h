#ifndef OPENSYNC_TEST_UTIL_H
#define OPENSYNC_TEST_UTIL_H

#include <stdlib.h>
#include <string.h>

#define RESPONSE_BUFFER_SIZE 4096
extern const char url[64];

size_t wCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif //OPENSYNC_TEST_UTIL_H

