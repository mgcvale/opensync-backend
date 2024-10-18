#include "database.h"
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *dbname;

int db_initialize(const char *name, int len) {
    dbname = malloc(len * sizeof(char));
    if (dbname == NULL) {
        return -1;
    }

    strncpy(dbname, name, len);
    return 0;
}

sqlite3* get_connection() {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(dbname, &db);

    if (rc) {
       fprintf(stderr, "Error opening database file. Check if you have one in your root directory. If not, create it with the `migrate` CLI arg. Error: %s\n", sqlite3_errmsg(db));
       return NULL;
    }

    return db;
}
