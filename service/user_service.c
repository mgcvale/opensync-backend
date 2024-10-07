#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user_service.h"
#include "database.h"
#include "crypt.h"

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
    sqlite3_bind_blob(stmt, 2, user->token, sizeof(user->token), SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 3, user->pwd_hash, sizeof(user->pwd_hash), SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 4, user->salt, sizeof(user->salt), SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_CONSTRAINT) {
        fprintf(stderr, "Failed to execute statement; UNIQUE constraint failed");
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

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return OK;
}

/* Caller shouln't malloc User. It should be null) */
int get_user_by_id(int userid, User **user) {
    const char *sql = "select * from user where id=?";
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

    *user = malloc(sizeof(User));
    if (*user == NULL) {
        fprintf(stderr, "Failed to allocate memory for User\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return ERR_MEMORY_ALLOCATION;
    }

    const unsigned char *username = sqlite3_column_text(stmt, 1);

    (*user)->id = userid;
    strncpy((*user)->uname, (const char *)username, MAX_USERNAME_LENGTH);
    (*user)->uname[MAX_USERNAME_LENGTH-1] = '\0';

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return OK;
}


int remove_user(User* user) {
    if (user == NULL) {
        return ERR_NULL_POINTER;
    }
    return remove_user_by_id(user->id);
}

int remove_user_by_id(int userid) {
    const char *sql = "delete from user where id=?";
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

    sqlite3_bind_int(stmt, 1, userid);

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
        const unsigned char *password_hash = sqlite3_column_blob(stmt, 2);
        const unsigned char *token = sqlite3_column_blob(stmt, 3);
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



