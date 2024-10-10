#include "user.h"
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "crypt.h"
#include "mongoose.h"

User *load_user(int id, const char* uname, int s_uname, const char* pwd_hash, const unsigned char* salt, const char* token) {
    User* user = malloc(sizeof(User)); // <- user.c:9
    if (user == NULL) {
        fprintf(stderr, "Failed to allocate memory for user\n");
        return NULL;
    }

    if (!pwd_hash || !token || !salt || !uname) {
        fprintf(stderr, "Required fields were null in user load_user");
        free(user);
        return NULL;
    }

    user->id = id;
    strncpy(user->uname, uname, MAX_USERNAME_LENGTH - 1);
    user->uname[MAX_USERNAME_LENGTH - 1] = '\0';

    strncpy(user->pwd_hash, pwd_hash, B64_ENCODED_LENGTH(SHA256_DIGEST_LENGTH) - 1);
    user->pwd_hash[B64_ENCODED_LENGTH(SHA256_DIGEST_LENGTH) - 1] = '\0';

    strncpy(user->token, token, B64_ENCODED_LENGTH(TOKEN_SIZE) - 1);
    user->token[B64_ENCODED_LENGTH(TOKEN_SIZE) - 1] = '\0';

    memcpy(user->salt, salt, TOKEN_SIZE);
    return user;
}

User *create_new_user(const char *uname, int s_uname, const char *pwd) {

    // salt gen
    unsigned char salt_blob[TOKEN_SIZE];

    int code = gensalt_raw(salt_blob, TOKEN_SIZE);
    if (code != 1) {
        fprintf(stderr, "error generating salt");
        return NULL;
    }

    // hash gen (with generated salt)
    char *hash = malloc(b64_encoded_length(SHA256_DIGEST_LENGTH));
    code = hash_password(pwd, salt_blob, hash, TOKEN_SIZE);
    if (code != CRYPT_OK) {
        fprintf(stderr, "error generating hash: %d\n", code);
        return NULL;
    }

    // generate token
    char *token = malloc(b64_encoded_length(TOKEN_SIZE) * sizeof(char));
    code = gentoken(token, TOKEN_SIZE);
    if (code != 1) {
        free(hash);
        free(token);
        fprintf(stderr, "error generating token");
        return NULL;
    }

    User *u = load_user(-1, uname, s_uname, hash, salt_blob, token);
    free(hash);
    free(token);
    return u;
}

void free_user(User *user) {
    if (user == NULL) {
        return;
    }
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
    cJSON_AddStringToObject(user_json, "token", user->token);
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



