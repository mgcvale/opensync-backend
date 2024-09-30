#include <sqlite3.h>
#include <stdio.h>

int migrate() {

    sqlite3 *db;
    char* err_msg = 0;
    int rc;

    printf("[MIGRATION] starting migration...\n");
    printf("---->DB Creation (1/2)<----\n");

    rc = sqlite3_open("database.db", &db);

    if (rc) {
        fprintf(stderr, "Error opening database: %s\nSteps completed successfully: 0/2\n", sqlite3_errmsg(db));
        return rc;
    }

    printf("database created successfully!\n");

    printf("---->Table creation<----\n");

    const char* sql = "create table if not exists user (id smallint primary key, username varchar(64))";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creating user table: %s\nSteps completed successfully: 1/2\n", err_msg);
        return rc;
    }

    printf("user table created successfully.\n");
    printf("[MIGRATION] Migration completed successfully. Cleaning up..");

    sqlite3_close(db);

    return 0;
}
