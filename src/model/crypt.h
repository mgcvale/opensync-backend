#ifndef OPENSYNCOPENSYNC_CRYPT_H
#define OPENSYNCOPENSYNC_CRYPT_H

#include <stdlib.h>

#define BCRYPT_WORKFACTOR 12
#define BCRYPT_HASH_LENGTH 64
#define TOKEN_SALT_LENGTH 16
#define SALT_LEN 32
#define ENTROPY_SIZE 16
#define B64_ENCODED_LENGTH(input_length) (4 * ((input_length + 2) / 3) + 1)

int hash_password(const char* password, char* hash_output, size_t len);
int gentoken(char* token, size_t length);
size_t b64_encoded_length(size_t input_length);

#endif //OPENSYNCOPENSYNC_CRYPT_H