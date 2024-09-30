#include <stdbool.h>
#include <string.h>

#include "util.h"

bool prefix(const char* pre, const char* str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}
