#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "handler/handler.h"
#include "migrate.c"
#include "mongoose.h"
#include "service/database.h"
#include <string.h>
#include <stdio.h>

#define PORT 8082

static int s_exit_flag = 0;

static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);
    s_exit_flag = 1;
}

int main(int argc, char *argv[]) {
    int rc;
    if (argc > 1) {
        if (strcmp("migrate", argv[1]) == 0) {
            return migrate(argc, argv);
        } else {
            printf("starting server...\n");
            rc = db_initialize(argv[1], strlen(argv[1]));
        }
    } else {
        printf("starting server...\n");
        rc = db_initialize("database.db", strlen("database.db"));
    }

    if (rc != 0) {
        fprintf(stderr, "FATAL: Error initializing database! Exiting now...");
        return -1;
    }

    struct mg_mgr mongoose_mgr;
    mg_mgr_init(&mongoose_mgr);
    mg_log_set(MG_LL_DEBUG);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    char *base_url = malloc(64 * sizeof(char));
    snprintf(base_url, 64, "http://localhost:%d", PORT);
    mg_http_listen(&mongoose_mgr, base_url, handler, NULL);
    free(base_url);

    printf("Server is running on port %d!\n", PORT);

    while (!s_exit_flag) {
        mg_mgr_poll(&mongoose_mgr, 1000);
    }

    mg_mgr_free(&mongoose_mgr);
    db_cleanup();

    return 0;
}
