#ifndef HYPRSYNC_BACKEND_USER_H
#define HYPRSYNC_BACKEND_USER_H

#include <stdlib.h>

typedef struct {
    int id;
    char* uname;
} User;

User* create_user(char* uname, int s_uname);

#endif
