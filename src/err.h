//
// Created by mgcv on 2/18/25.
//

#ifndef ERR_H
#define ERR_H

#define ERR_DB_CREATION -1
#define ERR_DB_CONFLICT -2
#define ERR_DB_PREPARED_STMT -3
#define ERR_DB_INSERTION -4
#define ERR_NULL_POINTER -5
#define ERR_DB_QUERY -6
#define DB_NO_RESULT -7
#define ERR_MALLOC -8
#define ERR_DB_EXECUTION -9
#define NO_AFFECTED_ROWS -10
#define ERR_USERLIST_CREATION -11
#define ERR_INVALID_CREDENTIALS -12
#define ERR_BUFFER_TOO_SMALL -13
#define NO_RESULT -14
#define ERR_NO_DIR_FOUND -15
#define ERR_NO_FILE_FOUND -16
#define ERR_FILE_STORAGE -17
#define ERR_FILE_FSEEK -18
#define ERR_IO -19
#define ERR_FOPEN -20
#define WARNING -21
#define ERR_INVALID_JSON -22
#define ERR_B64_ENCODE -23
#define ERR_TOKEN_GEN -24
#define ERR_HASH_GEN -25
#define ERR_SALT_GEN -26


#define ERR_UNKNOWN -128

#define CRYPT_OK 0
#define OK 0

#endif //ERR_H
