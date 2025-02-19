#ifndef OPENSYNCOPENSYNC_USER_SERVICE_H
#define OPENSYNCOPENSYNC_USER_SERVICE_H

#include "../model/user.h"

int add_user(User *user); //implemented
int remove_user_by_token(const char *token); //implemented
int update_user(User *user); // TODO: implement
int get_user_by_id(int id, User **user); // implemented
int get_users_as_list(User_list **list); // implemented
int auth_user_by_pwd(User **out, const char *uname, const char *pwd); // implemented
int auth_user_by_token(User **out, const char *token); // implemented
int get_token_by_pwd(char *token, const char *uname, const char *pwd); // implemented

#endif //OPENSYNCOPENSYNC_USER_SERVICE_H
