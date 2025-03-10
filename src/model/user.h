#ifndef OPENSYNCOPENSYNC_USER_H
#define OPENSYNCOPENSYNC_USER_H

#include <stdlib.h>
#include "crypt.h"

#define USERNAME_LENGTH 64
#define USER_HASH_LENGTH BCRYPT_HASH_LENGTH
#define TOKEN_SIZE 16

typedef struct {
    int id;
    char uname[USERNAME_LENGTH];
    char hash[USER_HASH_LENGTH];
    char token[B64_ENCODED_LENGTH(TOKEN_SIZE)];
} User;

typedef struct _user_node{
    User *user;
    struct _user_node *next;
} _user_node;

typedef struct {
    _user_node *head;
    _user_node *tail;
    int count;
} User_list;

User* create_new_user(const char *uname, int s_uname, const char *pwd);
User* load_user(int id, const char *uname, int s_uname, const char *hash, const char *token);
void free_user(User *user);

User_list *user_list_create(void);
int user_list_append(User_list *list, User *user);
User **to_user_array(User_list list, size_t *size);
char *stringify_user(const User *user);
char *jsonify_list(User_list list);
char *to_json_string(User *user);
void free_User_list(User_list *list);
#endif
