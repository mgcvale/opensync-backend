#include <kore/kore.h>
#include <kore/http.h>
#include "../migrate.c"
#include "util/config.h"
#include "service/database.h"
#include "util/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8082
void kore_parent_configure(int argc, char *argv[]);
void kore_parent_configure(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "migrate") == 0) {
        migrate(argc, argv);
        exit(0);
    }

    load_from_file("./config.json");
}
int kore_worker_initialize(void);
int kore_worker_initialize(void) {
    return db_initialize("database.db", strlen("database.db"));
}

void kore_worker_cleanup(void);
void kore_worker_cleanup(void) {
    db_cleanup();
}

int page_handler(struct http_request *req);
int page_handler(struct http_request *req) {
    http_response(req, 200, "Hello, World!", 13);
    return (KORE_RESULT_OK);
}