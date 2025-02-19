#include <kore/kore.h>
#include <kore/http.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include "util.h"
#include "../err.h"
#include "config.h"

#define MAX_CONFIG_LEN 1024

char user_data_dir[128];

void load_defaults(void) {
    snprintf(user_data_dir, sizeof(user_data_dir), "./user_data");
}

int load_from_file(const char *config_file_path) {
    load_defaults();

    bool is_warning = false;

    // Read file contents using Kore's utility function
    char json_contents[MAX_CONFIG_LEN];
    int err = util_buffered_get_file_contents(config_file_path, json_contents, MAX_CONFIG_LEN);

    if (err != OK) {
        kore_log(LOG_ERR, "Error reading file: %s. error code: %d", config_file_path, err);
        return err;
    }

    // Parse JSON using Kore's JSON API
    struct kore_json json;
    kore_json_init(&json, json_contents, strlen(json_contents));
    if (kore_json_parse(&json) != KORE_RESULT_OK) {
        kore_log(LOG_ERR, "Error parsing JSON from file: %s", config_file_path);
        kore_free(&json);
        return ERR_INVALID_JSON;
    }

    // Extract `user_data_dir` from the JSON
    struct kore_json_item *user_data_dir_item = kore_json_find_string(json.root, "user_data_dir");
    if (user_data_dir_item == NULL) {
        kore_log(LOG_WARNING, "Error parsing `user_data_dir` from file: %s. Proceeding with default config.", config_file_path);
        is_warning = true;
    } else {
        snprintf(user_data_dir, sizeof(user_data_dir), "%s", user_data_dir_item->data.string);
    }

    // Clean up
    kore_json_cleanup(&json);


    return is_warning ? OK : WARNING;
}

int initialize(void) {
    DIR *user_data_dir_dir = opendir(user_data_dir);
    if (!user_data_dir_dir) {
        if (mkdir(user_data_dir, 0777) != 0) {
            kore_log(LOG_ERR, "FATAL: Error creating user data directory (%s)", user_data_dir);
            return KORE_RESULT_ERROR;
        }
    }
    return KORE_RESULT_OK;
}