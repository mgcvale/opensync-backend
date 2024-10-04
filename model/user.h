#ifndef HYPRSYNC_BACKEND_USER_H
#define HYPRSYNC_BACKEND_USER_H

#include <cjson/cJSON.h>
#include <stdlib.h>

#define MAX_USERNAME_LENGTH 64

typedef struct {
    int id;
    char* uname;
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

User* create_new_user(const char* uname, int s_uname);
User* create_user(int id, const char* uname, int s_uname);
void free_user(User *user);

User_list *user_list_create(void);
void user_list_append(User_list *list, User *user);
User **to_user_array(User_list list, size_t *size);
cJSON *jsonify_user(User *user);
cJSON *jsonify_list(User_list list);


#endif
