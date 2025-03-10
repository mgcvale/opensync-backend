#include "crypt.h"

#include "../err.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kore/kore.h>
#include <crypt.h>
#include <sys/random.h>

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int encode_b64(const unsigned char *blob, char *output, size_t input_length) {
    if (blob == NULL || output == NULL) {
        return ERR_MALLOC;
    }

    if (input_length == 0) {
        output[0] = '\0';
        return CRYPT_OK;
    }

    size_t o = 0;
    for (size_t i = 0; i < input_length; ) {
        const unsigned int octet_a = i < input_length ? blob[i++] : 0;
        const unsigned int octet_b = i < input_length ? blob[i++] : 0;
        const unsigned int octet_c = i < input_length ? blob[i++] : 0;

        const unsigned int triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        output[o++] = b64_table[(triple >> 18) & 0x3F];
        output[o++] = b64_table[(triple >> 12) & 0x3F];
        output[o++] = (i - 1) > input_length ? '=' : b64_table[(triple >> 6) & 0x3F];
        output[o++] = i > input_length ? '=' : b64_table[triple & 0x3F];
    }

    output[o] = '\0';
    return CRYPT_OK;
}

static int get_random_bytes(unsigned char *buf, const size_t len) {
    size_t total = 0;
    while (total < len) {
        const ssize_t ret = getrandom(buf + total, len - total, 0);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            return ERR_IO;
        }
        total += ret;
    }
    return CRYPT_OK;
}

size_t b64_encoded_length(size_t input_length) {
    return 4 * ((input_length + 2) / 3) + 1; // +1 for null terminator
}

int hash_password(const char *password, char *hash_output, size_t len) {
    if (password == NULL || hash_output == NULL) {
        return ERR_MALLOC;
    }

    unsigned char entropy[ENTROPY_SIZE];
    if (get_random_bytes(entropy, ENTROPY_SIZE) != CRYPT_OK) {
        kore_log(LOG_DEBUG, "Error opening urandom for entropy filling");
        return ERR_IO;
    }
    kore_log(LOG_INFO, "Created entropy");
    char salt[64];
    if (crypt_gensalt_rn("$2b$", BCRYPT_WORKFACTOR, (char*) entropy, ENTROPY_SIZE, salt, sizeof(salt)) == NULL) {
        kore_log(LOG_DEBUG, "errno: %d", errno);
        return ERR_SALT_GEN;
    }
    char *hash = crypt_r(password, salt, NULL);
    if (hash == NULL) {
        return ERR_HASH_GEN;
    }

    strncpy(hash_output, hash, len - 1);
    hash_output[len - 1] = '\0';
    return CRYPT_OK;
}

int gentoken(char *token, size_t length) {
    kore_log(LOG_INFO, "generating token");
    unsigned char *decoded = NULL;
    decoded = malloc(b64_encoded_length(length));
    if (decoded == NULL) {
        return ERR_MALLOC;
    }

    int code = get_random_bytes(decoded, length);
    if (code != 0) {
        free(decoded);
        return ERR_TOKEN_GEN;
    }

    if (encode_b64(decoded, token, length) != CRYPT_OK) {
        free(decoded);
        return ERR_B64_ENCODE;
    }

    free(decoded);
    return CRYPT_OK;
}