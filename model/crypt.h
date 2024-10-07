#ifndef HYPRSYNC_BACKEND_CRYPT_H
#define HYPRSYNC_BACKEND_CRYPT_H

#define DEFAULT_SALT_LENGTH 16
#define ERR_EVP_CTX_CREATION -1
#define ERR_EVP_DIGEST_INIT -2
#define ERR_FINAL_DIGEST -3
#define ERR_B64_ENCODE -4
#define ERR_MALLOC -5
#define ERR_TOKEN_GEN -6
#define ERR_UNKNOWN -7
#define CRYPT_OK 1
#define SHA256_DIGEST_LENGTH 32

#define SHA256_ITER_COUNT 20000

#define B64_ENCODED_LENGTH(input_length) (4 * ((input_length + 2) / 3) + 1)

#include <stdlib.h>

int gensalt_raw(unsigned char *salt, size_t length);
int encode_salt(const unsigned char *salt, size_t length, char* encoded_salt);
int hash_password(const char* password, unsigned char *salt, char *hash, size_t salt_size);
int gentoken( char *token, size_t length);
size_t b64_encoded_length(size_t input_length);

#endif //HYPRSYNC_BACKEND_CRYPT_H
