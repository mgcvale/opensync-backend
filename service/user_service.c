#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>

#include "user_service.h"
#include "database.h"

int add_user(User *user) {
    const char *sql = "insert into user(username) values(?)";
    sqlite3_stmt *stmt;
    int rc;
    sqlite3 *db = get_connection();

    if (db == NULL) {
        return ERR_DB_CREATION;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
       fprintf(stderr, "failed to prepare statement: %s\n", sqlite3_errmsg(db));
       return ERR_DB_PREPARED_STMT;
    }

    sqlite3_bind_text(stmt, 1, user->uname, -1, SQLITE_STATIC);

    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "failed to insert user: %s\n", sqlite3_errmsg(db));
        return ERR_DB_INSERTION;
    }

    return OK;
}
