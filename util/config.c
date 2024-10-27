#include "config.h"
#include "util.h"
#include <cjson/cJSON.h>
#include <stdio.h>

char user_data_dir[128];

void load_defaults() {
    snprintf(user_data_dir, sizeof(user_data_dir), "./user_data");
}

int load_from_file(const char* config_file_path) {
    load_defaults();
    char *contents;
    bool is_warning = false;
    contents = util_get_file_contents(config_file_path, 256);

    cJSON *json = cJSON_ParseWithLength(contents, 256);
    if (json == NULL) {
        fprintf(stderr, "Error parsing JSON from file %s\n", config_file_path);
        return ERROR;
    }

    cJSON *user_data_dir_json = cJSON_GetObjectItemCaseSensitive(json, "user_data_dir");
    if (!cJSON_IsString(user_data_dir_json) || user_data_dir_json->valuestring == NULL) {
        fprintf(stderr, "Error parsing `user_data_dir config from file %s. Proceeding with default config.\n", config_file_path);
        is_warning = true;
    } else {
        snprintf(user_data_dir, sizeof(user_data_dir), "%s", user_data_dir_json->valuestring);
    }

    return is_warning ? WARNING : SUCCESS;
}

int initialize() {
    DIR *user_data_dir_dir = opendir(user_data_dir);
    if (!user_data_dir_dir) {
        if (mkdir(user_data_dir, 0777) != 0) {
            fprintf(stderr, "FATAL: Error creating user data direcory (%s)\n", user_data_dir);
            return FATAL_ERROR;
        }
    }
    return SUCCESS;
}
