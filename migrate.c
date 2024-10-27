#include <sqlite3.h>
#include <stdio.h>

int migrate(int argc, char *argv[]) {
    sqlite3 *db;
    char* err_msg = 0;
    int rc;
    char sql[512] = "create table if not exists user (id integer primary key, username varchar(64) unique, password_hash varchar(48), token varchar(24) unique, salt blob)";

    printf("[MIGRATION] starting migration...\n");
    printf("---->DB Creation<----\n");

    if (argc > 2) {
        rc = sqlite3_open(argv[2], &db);
    } else {
        rc = sqlite3_open("database.db", &db);
    }

    if (rc) {
        fprintf(stderr, "Error opening database: %s\nSteps completed successfully: 0/3\n", sqlite3_errmsg(db));
        return rc;
    }

    printf("database created successfully! (1/3)\n");

    printf("---->User Table creation<----\n");

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creating user table: %s\nSteps completed successfully: 1/3\n", err_msg);
        sqlite3_close(db);
        return rc;
    }

    printf("user table created successfully. (2/3)\n");

    sql[0] = '\0';
    strncpy(sql, "create table if not exists file_metadata ("
    "id integer primary key, "
    "fname varchar(128) not null, "
    "ftype varchar(8) not null, "
    "fdate varchar(24) not null, "
    "fsize long integer, "
    "fowner_id integer, "
    "foreign key (fowner_id) references user(id), "
    "unique(fowner_id, fname))", 512);

    printf("---->File Metadata Table creation<----\n");

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creating File Metadata table: %s\nSteps completed successfully: 2/3\n", err_msg);
        sqlite3_close(db);
        return rc;
    }

    printf("user table created successfully. (3/3)\n");

    printf("[MIGRATION] Migration completed successfully. Cleaning up..");

    sqlite3_close(db);
    return 0;
}
