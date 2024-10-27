#ifndef OPENSYNCOPENSYNC_UTIL_H
#define OPENSYNCOPENSYNC_UTIL_H

#include <stdbool.h>
#include "mongoose.h"

bool prefix(const char *pre, const char *str);
bool extract_token(struct mg_http_message *http_msg, char *token_out, size_t token_out_size);
char *util_get_file_contents(const char *path, int size);

#endif //OPENSYNCOPENSYNC_UTIL_H
