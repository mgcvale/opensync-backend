#include <stdbool.h>
#include <string.h>

#include "util.h"
#include "mongoose.h"

bool prefix(const char* pre, const char* str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

bool extract_token(struct mg_http_message* http_msg, char* token_out, size_t token_out_size) {
    const char bearer_prefix[] = "Bearer ";
    size_t bearer_prefix_len = strlen(bearer_prefix);

    // Get the Authorization header
    struct mg_str *auth_header = mg_http_get_header(http_msg, "Authorization");
    if (auth_header == NULL) {
        fprintf(stderr, "Error parsing auth token: missing authorization header\n");
        return false;  // Authorization header missing
    }

    // Check if Authorization header starts with "Bearer "
    if (!prefix(bearer_prefix, auth_header->buf)) {
        fprintf(stderr, "Error parsing auth token: Authorization header doesn't start with Bearer\n");
        return false;
    }

    const char *token = auth_header->buf + bearer_prefix_len;
    size_t token_len = auth_header->len - bearer_prefix_len;

    if (token_len >= token_out_size) {
        fprintf(stderr, "Error parsing auth token: token too long\n");
        return false;  // Token too long
    }

    strncpy(token_out, token, token_len);
    token_out[token_len] = '\0';

    return true;
}
