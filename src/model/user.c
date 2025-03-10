#include "user.h"

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <kore/kore.h>

#include "crypt.h"
#include "../lib/yyjson.h"


User *load_user(int id, const char* uname, int s_uname, const char* pwd_hash, const char* token) {
    User* user = malloc(sizeof(User));
    if (user == NULL) {
        fprintf(stderr, "Failed to allocate memory for user\n");
        return NULL;
    }

    if (!pwd_hash || !token || !uname) {
        fprintf(stderr, "Required fields were null in user load_user");
        free(user);
        return NULL;
    }

    user->id = id;
    strncpy(user->uname, uname, USERNAME_LENGTH - 1);
    user->uname[USERNAME_LENGTH - 1] = '\0';

    strncpy(user->hash, pwd_hash, BCRYPT_HASH_LENGTH);
    user->hash[USER_HASH_LENGTH - 1] = '\0';

    strncpy(user->token, token, TOKEN_SIZE - 1);
    user->token[B64_ENCODED_LENGTH(TOKEN_SIZE) - 1] = '\0';

    return user;
}

User *create_new_user(const char *uname, int s_uname, const char *pwd) {
    // hash gen
    char *hash = malloc(USER_HASH_LENGTH);
    int code = hash_password(pwd, hash, USER_HASH_LENGTH);
    if (code != CRYPT_OK) {
        kore_log(LOG_DEBUG, "Error hashing password for new user: %d", code);
        return NULL;
    }

    // generate token
    char *token = malloc(b64_encoded_length(TOKEN_SIZE) * sizeof(char));
    code = gentoken(token, TOKEN_SIZE);
    if (code != CRYPT_OK) {
        kore_log(LOG_DEBUG, "Error generating token for new user: %d", code);
        free(hash);
        free(token);
        return NULL;
    }

    User *u = load_user(-1, uname, s_uname, hash, token);
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
        return NULL;
    }
    return list;
}

int user_list_append(User_list *list, User *user) {
    if (list == NULL) {
        fprintf(stderr, "failed to append user to null list. Call user_list_create() first.");
        return -1;
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
        return 0;
    } else {
        fprintf(stderr, "Error creating new user node in user_list_append().\n");
        return -1;
    }
}

/* caller must free json after use */
static yyjson_doc *jsonify_user(const User *user) {
    const char *json_str = "{\"id\": %d, \"username\": \"%s\", \"token\": \"%s\"}";
    int len = snprintf(NULL, 0, json_str, user->id, user->uname, user->token);

    char *buffer = malloc(len + 1);
    if (!buffer) return NULL;

    snprintf(buffer, len + 1, json_str, user->id, user->uname, user->token);

    yyjson_doc *doc = yyjson_read(buffer, len, 0);
    free(buffer);

    return doc;
}


/* caller must free buffer after use */
char *stringify_user(const User *user) {
    yyjson_doc *doc = jsonify_user(user);

    char *json_str = yyjson_write(doc, 0, NULL);
    yyjson_doc_free(doc);
    return json_str;
}

/* caller must free buffer after use */
char *jsonify_list(const User_list list) {
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *root = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root);

    _user_node *current = list.head;
    while (current) {
        yyjson_mut_arr_add_obj(doc, yyjson_val_mut_copy(doc, yyjson_doc_get_root(jsonify_user(current->user))));
        current = current->next;
    }

    char *json = yyjson_mut_write(doc, 0, NULL);
    yyjson_mut_doc_free(doc);
    return json;
}

// O(n)
User **to_user_array(const User_list list, size_t *size) {
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

static void free_user_node(_user_node *node) {
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
    free_user_node(list->head);
    free(list);
}