#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <microhttpd.h>
#include "handler/handler.h"
#include "migrate.c"

#define PORT 8081

int main(int argc, char *argv[]) {

    if (argc == 2) {
        if(strcmp("migrate", argv[0])) {
            return migrate();
        }
    }

    printf("starting server on port %d\n", PORT);

    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL, &handler, NULL, MHD_OPTION_END);

    if (NULL == daemon)
        return 1;

    printf("Server is running on port %d\n", PORT);
    getchar(); // Wait for user input to stop the server

    MHD_stop_daemon(daemon);
    return 0;
}
