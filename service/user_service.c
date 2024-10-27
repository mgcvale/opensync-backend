#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "user_service.h"
#include "database.h"
#include "crypt.h"
#include "../util/config.h"

int add_user(User *user) {
    const char *sql = "insert into user(username, token, password_hash, salt) values(?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    int rc;
    sqlite3 *db = get_connection();

    if (db == NULL) {
        return ERR_DB_CREATION;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
       fprintf(stderr, "failed to prepare statement: %s\n", sqlite3_errmsg(db));
       sqlite3_close(db);
       return ERR_DB_PREPARED_STMT;
    }

    sqlite3_bind_text(stmt, 1, user->uname, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user->token, strlen(user->token), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user->pwd_hash, strlen(user->pwd_hash), SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 4, user->salt, sizeof(user->salt), SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_CONSTRAINT) {
        fprintf(stderr, "Failed to execute statement; UNIQUE constraint failed\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_DB_CONFLICT;
    }
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_DB_INSERTION;
    }

    char userdir[128];
    snprintf(userdir, 128, "%s/%s", user_data_dir, user->uname);
    if (mkdir(userdir, 0777) != 0) {
        fprintf(stderr, "non-critical: failed to create user directory. It will be created when the first file is uploaded.\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return OK;
}

/* Caller shouln't malloc User. It should be null) */
int get_user_by_id(int userid, User **user) {
    const char sql[] = "select * from user where id=?";
    sqlite3_stmt *stmt;
    int rc;
    sqlite3 *db = get_connection();

    if (db == NULL) {
        fprintf(stderr, "Failed to open DB connection; db is NULL");
        return ERR_DB_CREATION;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ERR_DB_PREPARED_STMT;
    }

    sqlite3_bind_int(stmt, 1, userid);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        return DB_NO_RESULT;
    } else if (rc != SQLITE_ROW) {
        fprintf(stderr, "failed to get result from User query from ID: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_DB_QUERY;
    }

    const char *username = (const char*) sqlite3_column_text(stmt, 1);
    const char *pwd_hash = (const char*) sqlite3_column_text(stmt, 2);
    const char *token = (const char*) sqlite3_column_text(stmt, 3);
    const unsigned char *salt = sqlite3_column_blob(stmt, 4);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    (*user) = load_user(userid, username, strlen(username), pwd_hash, salt, token);
    if (*user == NULL) {
        fprintf(stderr, "Error constructing user from database query");
        return ERR_NULL_POINTER;
    }

    return OK;
}

int auth_user_by_pwd(User **out, const char *uname, const char *pwd) {
    sqlite3 *db = get_connection();

    if (db == NULL) {
        return ERR_DB_CREATION;
    }

    const char sql[] = "select * from user where username=?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ERR_DB_PREPARED_STMT;
    }

    sqlite3_bind_text(stmt, 1, uname, strlen(uname), SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return DB_NO_RESULT;
    } else if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_DB_QUERY;
    }

    int id = sqlite3_column_int(stmt, 0);
    const char *username = (char*) sqlite3_column_text(stmt, 1);
    const char *pwd_hash = (char*) sqlite3_column_text(stmt, 2);
    const char *token = (char*) sqlite3_column_text(stmt, 3);
    const unsigned char *salt = (unsigned char*) sqlite3_column_blob(stmt, 4);

    // before returning the found user, we need to check if hashing the password with the salt results in the same hash
    char hash[B64_ENCODED_LENGTH(SHA256_DIGEST_LENGTH)];
    hash_password(pwd, salt, hash, TOKEN_SIZE);

    if (strncmp(hash, pwd_hash, b64_encoded_length(SHA256_DIGEST_LENGTH)) != 0) {
        fprintf(stderr, "Error authenticating user in auth_user_by_pwd: hashes don't match");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_INVALID_CREDENTIALS;
    }

    (*out) = load_user(id, username, strlen(username), pwd_hash, salt, token);
    if (*out == NULL) {
        fprintf(stderr, "Error constructing user from database query");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_NULL_POINTER;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return OK;
}

int auth_user_by_token(User **user, const char *token) {
    sqlite3 *db = get_connection();

    if (db == NULL) {
        return ERR_DB_CREATION;
    }

    const char sql[] = "select id, username, password_hash, salt from user where token=?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ERR_DB_PREPARED_STMT;
    }

    sqlite3_bind_text(stmt, 1, token, strlen(token), SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return DB_NO_RESULT;
    } else if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_DB_QUERY;
    }

    int id = sqlite3_column_int(stmt, 0);
    const char *username = (char*) sqlite3_column_text(stmt, 1);
    const char *pwd_hash = (char*) sqlite3_column_text(stmt, 2);
    const unsigned char *salt = (unsigned char*) sqlite3_column_blob(stmt, 3);

    (*user) = load_user(id, username, strlen(username), pwd_hash, salt, token);
    if (*user == NULL) {
        fprintf(stderr, "Error constructing user from database query");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_NULL_POINTER;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return OK;
}


int get_token_by_pwd(char* token, const char* uname, const char* pwd) { // assume caller mallocated token with B64_ENCODED_LENGTH(TOKEN_SIZE) size
    sqlite3 *db = get_connection();
    if (db == NULL) {
        return ERR_DB_CREATION;
    }

    const char sql[] = "select password_hash, token, salt from user where username=?";
    sqlite3_stmt *stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ERR_DB_PREPARED_STMT;
    }

    sqlite3_bind_text(stmt, 1, uname, strlen(uname), SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return DB_NO_RESULT;
    } else if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_DB_QUERY;
    }

    const char *pwd_hash = (char*) sqlite3_column_text(stmt, 0);
    const char *temp_token = (char*) sqlite3_column_text(stmt, 1);
    const unsigned char *salt = (unsigned char*) sqlite3_column_blob(stmt, 2);

    char hash[B64_ENCODED_LENGTH(SHA256_DIGEST_LENGTH)];
    hash_password(pwd, salt, hash, TOKEN_SIZE);
    int b64_tokensize = b64_encoded_length(TOKEN_SIZE);

    if (strncmp(hash, pwd_hash, b64_encoded_length(SHA256_DIGEST_LENGTH)) != 0) {
        fprintf(stderr, "Error authenticating user in auth_user_by_pwd: hashes don't match");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_INVALID_CREDENTIALS;
    }

    if (strlen(temp_token) >= b64_tokensize) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_BUFFER_TOO_SMALL;
    }

    strncpy(token, temp_token, b64_tokensize - 1);
    token[b64_tokensize - 1] = '\0';
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return OK;
}

int remove_user_by_token(const char *token) {
    const char *sql = "delete from user where token=?";
    sqlite3_stmt *stmt;
    int rc;
    sqlite3 *db = get_connection();

    if (db == NULL) {
        fprintf(stderr, "failed to open DB connection; DB is NULL\n");
        return ERR_DB_CREATION;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ERR_DB_PREPARED_STMT;
    }

    sqlite3_bind_text(stmt, 1, token, strlen(token), SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (sqlite3_changes(db) == 0) {
        fprintf(stderr, "no entry was deleted!\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return NO_AFFECTED_ROWS;
    }
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ERR_DB_EXECUTION;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return OK;
}

/* assume caller passes null userlist*/
int get_users_as_list(User_list **userlist) {
    const char* sql = "select * from user";
    sqlite3_stmt *stmt;
    int rc;
    sqlite3 *db = get_connection();

    if (db == NULL) {
        fprintf(stderr, "failed to open DB connection; DB is NULL");
        return ERR_DB_CREATION;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "failed to prepare sql select statement (?): %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ERR_DB_PREPARED_STMT;
    }

    *userlist = user_list_create();
    if (userlist == NULL) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_USERLIST_CREATION;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *username = (const char *)sqlite3_column_text(stmt, 1);
        const char *password_hash = (const char *) sqlite3_column_text(stmt, 2);
        const char *token = (const char *) sqlite3_column_text(stmt, 3);
        const unsigned char *salt = sqlite3_column_blob(stmt, 4);

        if (!username || !password_hash || !token || !salt) {
            fprintf(stderr, "Null value encountered in database result\n");
            continue;
        }

        User *u = load_user(id, username, strlen(username), password_hash, salt, token);
        if (u) {
            user_list_append(*userlist, u);
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return OK;
}

