#include "database.h"
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>

sqlite3* get_connection() {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open("database.db", &db);

    if (rc) {
       fprintf(stderr, "Error opening database file. Check if you have one in your root directory. If not, create it with the `migrate` CLI arg. Error: %s\n", sqlite3_errmsg(db));
       return NULL;
    }

    return db;
}
