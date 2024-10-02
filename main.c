#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "handler/handler.h"
#include "migrate.c"
#include "mongoose.h"
#include <string.h>
#include <stdio.h>

#define PORT 8082

static int s_exit_flag = 0;

static void signal_handler(int sig_num) {
    signal(sig_num, signal_handler);
    s_exit_flag = 1;
}

int main(int argc, char *argv[]) {

    if (argc == 2 && strcmp("migrate", argv[1]) == 0) {
        return migrate();
    }

    printf("starting server...\n");

    struct mg_mgr mongoose_mgr;
    mg_mgr_init(&mongoose_mgr);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    char *base_url = malloc(64 * sizeof(char));
    snprintf(base_url, 64, "http://localhost:%d", PORT);

    mg_http_listen(&mongoose_mgr, base_url, handler, NULL);

    printf("Server is running on port %d!\n", PORT);

    while (!s_exit_flag) {
        mg_mgr_poll(&mongoose_mgr, 1000);
    }

    return 0;
}
