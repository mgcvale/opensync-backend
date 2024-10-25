#include <curl/curl.h>
#include "unity.h"
#include "tests.h"
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

static pid_t server_pid;

void setUp(void) {
}

void tearDown(void) {
}

void beforeTests(void) {
    // build and run server with mockdb
    system("rm opensync-backend");
    system("rm -r ../../build; mkdir ../../build; cd ../../build; cmake ..; make; cd ../test/build");
    system("cp ../../build/opensync-backend .");
    system("rm mockdb.db");
    system("./opensync-backend migrate mockdb.db");

    server_pid = fork();
    if (server_pid == 0) {
        execl("./opensync-backend", "./opensync-backend", "mockdb.db", (char *)NULL);
        perror("Failed to start opensync-backend");
        exit(EXIT_FAILURE);
    } else if (server_pid > 0) {
        printf("Server is running in child process with PID %d\n", server_pid);
        sleep(3);
    } else {
        perror("Failed to fork process");
        exit(EXIT_FAILURE);
    }
}

void cleanup(void) {
    if (server_pid > 0) {
        printf("Terminating server process with PID %d\n", server_pid);
        kill(server_pid, SIGTERM);
        int status;
        waitpid(server_pid, &status, 0);
        printf("Server process terminated.\n");
    }
}

int main(int argc, char *argv[]) {
    atexit(cleanup);
    printf("-----> STARTING TEST SETUP <-----\n");
    beforeTests();
    printf("-----> FINISHED TEST SETUP <-----\n");

    // first, test if server is up
    CURL *curl = NULL;
    CURLcode res;
    char response[RESPONSE_BUFFER_SIZE] = {0};

    curl = curl_easy_init();
    if (curl) {
        printf("url: %s\n", url);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (CURLE_OK != res) {
            fprintf(stderr, "Server is not up; response code on index wasn't 200! Aborting...\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Failed initializing curl; can't test if server is up or not. Aborting...\n");
        exit(EXIT_FAILURE);
    }

    printf("\n\n----->begining tests<------\n\n");

    UNITY_BEGIN();
    RUN_TEST(test_addUserShouldReturnSuccess);
    RUN_TEST(test_addUserConflictsWhenUsernameIsAlreadyTaken);
    RUN_TEST(test_userGetByPasswordFailsWhenPasswordIsIncorrect);
    RUN_TEST(test_userGetByPasswordSucceedsWhenPasswordIsCorrect);
    RUN_TEST(test_userGetByTokenFailsWhenTokenIsIncorrect);
    RUN_TEST(test_userGetByTokenSucceedsWhenTokenIsCorrect);
    RUN_TEST(test_userGetByTokenFailsWhenTokenIsMissing);
    RUN_TEST(test_userAuthenticationSucceedsWhenPasswordIsCorrect);
    RUN_TEST(test_userAuthenticationFailsWhenPasswordIsIncorrect);
    UNITY_END();

    printf("\n\n-----> finished tests <-----\n\n");
    exit(EXIT_FAILURE);
}
