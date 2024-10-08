#include "crypt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include "mongoose.h"

// assume caller mallocates output memory
static int encode_b64(const unsigned char *blob, char *output, size_t input_length) {
    if (blob == NULL || output == NULL) {
        return ERR_MALLOC;
    }

    if (input_length == 0) {
        output[0] = '\0';
        return CRYPT_OK;
    }

    mg_base64_encode(blob, input_length, output, b64_encoded_length(input_length));
    output[b64_encoded_length(input_length)-1] = '\0';

    return CRYPT_OK;
}

size_t b64_encoded_length(size_t input_length) {
    return 4 * ((input_length + 2) / 3) + 1; // +1 for null terminator
}

// assume caller mallocates output memory
int gensalt_raw(unsigned char *salt, size_t length) {
    return RAND_bytes(salt, length);
}

// assume caller mallocates output memory
int encode_salt(const unsigned char* salt, size_t length, char* encoded_salt) {
    if (encode_b64(salt, encoded_salt, length) != CRYPT_OK) {
        return ERR_B64_ENCODE;
    }
    return CRYPT_OK;
}

// assume caller mallocates output memory for hash - it should be b64_encoded_length(SHA256_DIGEST_LENGTH)
int hash_password(const char *password, const unsigned char *salt, char *hash, size_t salt_size) {
    if (hash == NULL || password == NULL || salt == NULL) {
        return ERR_MALLOC;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        return ERR_EVP_CTX_CREATION;
    }

    unsigned char temp_hash[SHA256_DIGEST_LENGTH];
    unsigned int hash_len;

    // init digest
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
        EVP_MD_CTX_free(ctx);
        return ERR_EVP_DIGEST_INIT;
    }

    // Add salt and password
    EVP_DigestUpdate(ctx, password, strlen(password));
    EVP_DigestUpdate(ctx, salt, salt_size);


    // first hash calculation
    if (EVP_DigestFinal_ex(ctx, temp_hash, &hash_len) != 1) {
        EVP_MD_CTX_free(ctx);
        return ERR_FINAL_DIGEST;
    }

    // Iterate SHA256_ITER_COUNT times
    for (int i = 1; i < SHA256_ITER_COUNT; i++) {
        // Re-initialize the digest context for each iteration
        if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
            EVP_MD_CTX_free(ctx);
            return ERR_EVP_DIGEST_INIT;
        }

        // re-hash hash
        EVP_DigestUpdate(ctx, temp_hash, SHA256_DIGEST_LENGTH);
        if (EVP_DigestFinal_ex(ctx, temp_hash, &hash_len) != 1) {
            EVP_MD_CTX_free(ctx);
            return ERR_FINAL_DIGEST;
        }
    }
    EVP_MD_CTX_free(ctx);

    // b64-encode final hash
    if (encode_b64(temp_hash, hash, SHA256_DIGEST_LENGTH) != CRYPT_OK) {
        return ERR_B64_ENCODE;
    }

    return CRYPT_OK;
}

// assume caller mallocates output memory
int gentoken(char *token, size_t length) {
    unsigned char *decoded = NULL;
    decoded = malloc(length * sizeof(char));
    if (decoded == NULL) {
        return ERR_MALLOC;
    }

    int code = RAND_bytes(decoded, length);
    if (code != 1) {
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
