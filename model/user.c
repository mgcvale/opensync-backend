#include "user.h"
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>

User *create_user(int id, const char* uname, int s_uname) {
    User* user = malloc(sizeof(User));
    if (user == NULL) {
        fprintf(stderr, "Failed to allocate memory for user\n");
        return NULL;
    }

    user->id = id;

    user->uname = malloc(s_uname + 1 * sizeof(char)); // has to be +1 because of the null-terminator char
    if (user->uname == NULL) {
        fprintf(stderr, "Failed to allocate memory for username\n");
        free(user);
        return NULL;
    }

    strncpy(user->uname, uname, s_uname+1);
    user->uname[s_uname] = '\0';

    return user;
}

User * create_new_user(const char* uname, int s_uname) {
    return create_user(-1, uname, s_uname);
}

void free_user(User *user) {
    if (user == NULL) {
        return;
    }
    free(user->uname);
    free(user);
}

// user list stuff
static _user_node *user_node_create(User *user) {
    _user_node *node = malloc(sizeof(_user_node));
    if (!node) {
        fprintf(stderr, "failed to allocate memory for new user node\n");
        return NULL;
    }

    node->user = user;
    node->next = NULL;
    return node;
}


User_list *user_list_create() {
    User_list *list = malloc(sizeof(User_list));
    if (list) {
        list->count = 0;
        list->head = NULL;
        list->tail = NULL;
    } else {
        fprintf(stderr, "failed to allocate memory for new user list\n");
    }
    return list;
}

void user_list_append(User_list *list, User *user) {
    if (list == NULL) {
        fprintf(stderr, "failed to append user to null list. Call user_list_create() first.");
        return;
    }
    _user_node *new_node = user_node_create(user);

    if (new_node) {

        if (list->tail) { //list is not new;
            list->tail->next = new_node;
            list->tail = list->tail->next;
        } else {
            list->head = new_node;
            list->tail = new_node;
        }

        list->count++;
    }
}

cJSON *jsonify_user(User *user) {
    cJSON *user_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(user_json, "id", user->id);
    cJSON_AddStringToObject(user_json, "username", user->uname);
    return user_json;
}

cJSON *jsonify_list(User_list list) {
    cJSON *json = cJSON_CreateArray();

    _user_node *current = list.head;
    while (current) {
        cJSON_AddItemToArray(json, jsonify_user(current->user));
        current = current->next;
    }

    return json;
}

// O(n)
User **to_user_array(User_list list, size_t *size) {
    if (list.count == 0) {
        return NULL;
    }
    User** user_arr = malloc(list.count * sizeof(User));

    if (user_arr == NULL) {
        fprintf(stderr, "error allocating memory for user array\n");
        return NULL;
    }

    _user_node *current = list.head;
    size_t i = 0;

    while(current->next) {
        user_arr[i] = current->user;
        current = current->next;
        i++;
    }

    if (size != NULL) {
        *size = i;
    }

    return user_arr;
}

static void _free_user_node(_user_node *node) {
    while (node) {
        _user_node *next = node->next;
        free_user(node->user);
        free(node);
        node = next;
    }
}

void free_User_list(User_list *list) {
    if (list == NULL) {
        return;
    }
    _free_user_node(list->head);
    free(list);
}



