#ifndef HYPRSYNC_BACKEND_USER_H
#define HYPRSYNC_BACKEND_USER_H

#include <stdlib.h>

#define MAX_USERNAME_LENGTH 64

typedef struct {
    int id;
    char* uname;
} User;

typedef struct user_listnode{
    User user;
    struct user_listnode *next;

} User_listnode;

typedef struct user_list {
    User_listnode *head;
    User_listnode *tail;
    int count;
} User_list;

User* create_user(char* uname, int s_uname);

User_list *user_list_create();
User_list *user_list_append(User user);
User* to_user_array(User_list *list);

#endif
