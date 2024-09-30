#include "user.h"
#include <stdio.h>
#include <string.h>

User * create_user(char* uname, int s_uname) {
    User* user = malloc(sizeof(User));
    if (user == NULL) {
        fprintf(stderr, "Failed to allocate memory for user\n");
        return NULL;
    }

    user->id = -1;

    user->uname = malloc(s_uname * sizeof(char));
    if (user->uname == NULL) {
        fprintf(stderr, "Failed to allocate memory for username\n");
        free(user);
        return NULL;
    }

    strncpy(user->uname, uname, s_uname);
    user->uname[s_uname - 1] = '\0';

    return user;
}
