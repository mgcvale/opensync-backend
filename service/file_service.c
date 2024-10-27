#include "file_service.h"
#include "database.h"
#include "sqlite3.h"
#include "../util/config.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

static bool _dir_exists(char *dir) {
    DIR *d = opendir(dir);
    return (d != NULL);
}

int save_file(const unsigned char* blob, size_t fsize, const char* fname, char *ownername) {
    char file[256];
    snprintf(file, 256, "%s/%s", user_data_dir, ownername);

    // create user dir if it doesn't exist
    if (!_dir_exists(file)) {
        int rc = mkdir(file, 0777);
        if (rc != 0) {
            return ERR_NO_DIR_FOUND;
        }
    }
    // build filename and open file
    strncat(file, "/", 2);
    strncat(file, fname, strlen(fname));
    FILE *f = fopen(file, "wb");
    if (f == NULL) {
        fprintf(stderr, "Error storing %s file in save_file.", fname);
        return ERR_FILE_STORAGE;
    }

    // write blob directly without seeking
    size_t written = fwrite(blob, sizeof(unsigned char), fsize, f);
    if (written < fsize) {
        fprintf(stderr, "Error writing bytes to %s file. Written: %zu\n", fname, written);
        fclose(f);
        return ERR_FILE_STORAGE;
    }

    fclose(f);
    return OK;
}


// TODO: implement
int load_file_metadata_to_db(const char* fname, size_t fsize, size_t ownerid, char *ownername) {
    char file[256];
    snprintf(file, 256, "%s/%s/%s", user_data_dir, ownername, fname);

    FILE *f = fopen(file, "rb");
    if (f == NULL) {
        fprintf(stderr, "Error opening %s file for metadata collection. Proceeding without file metadata.\n", fname);
    } else {
    }


    return OK;
}

