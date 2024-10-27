#include "file.h"

#include <string.h>
#include <stdio.h>

FileMetadata *create_new_fileMetadata(const char* fname, const char* ftype, const char* fdate, size_t fsize, int fowner_id) {
    FileMetadata *fmdat = malloc(sizeof(FileMetadata));
    if (fmdat == NULL) {
        return NULL;
    }

    if (fname) {
        strncpy(fmdat->fname, fname, FMDAT_FNAME_LEN);
        fmdat->fname[FMDAT_FNAME_LEN - 1] = '\0';
    } else {
        fmdat->fname[0] = '\0';
    }

    if (ftype) {
        strncpy(fmdat->ftype, ftype, FMDAT_FTYPE_LEN);
        fmdat->ftype[FMDAT_FTYPE_LEN - 1] = '\0';
    } else {
        fmdat->ftype[0] = '\0';
    }

    if (fdate) {
        strncpy(fmdat->fdate, fdate, FMDAT_FDATE_LEN);
        fmdat->fdate[FMDAT_FDATE_LEN - 1] = '\0';
    } else {
        fmdat->fdate[0] = '\0';
    }

    fmdat->fsize = fsize;
    fmdat->fowner_id = fowner_id;

    return fmdat;
}

FileMetadata * load_fileMetadata(size_t id, const char* fname, const char* ftype, const char* fdate, size_t fsize, int fowner_id) {
    FileMetadata *fmdat = create_new_fileMetadata(fname, ftype, fdate, fsize, fowner_id);
    if (fmdat == NULL) {
        return NULL;
    }

    fmdat->fid = id;
    return fmdat;
}

void free_fileMetadata(FileMetadata *fmdat) {
    if (fmdat == NULL) {
        return;
    }
    free(fmdat);
}


// file list stuff

static _file_node *_file_node_create(FileMetadata *fmdat) {
    _file_node *node = malloc(sizeof(_file_node));
    if (!node) {
        fprintf(stderr, "failed to allocate memory for new user node\n");
        return NULL;
    }

    node->fmdat = fmdat;
    node->next = NULL;
    return node;
}


FileMetadataList *fileMetadataList_create(void) {
    FileMetadataList *list = malloc(sizeof(FileMetadataList));
    if (list) {
        list->count = 0;
        list->head = NULL;
        list->tail = NULL;
    } else {
        fprintf(stderr, "failed to allocate memory for new File Metadata list\n");
    }
    return list;
}

int fileMetadataList_append(FileMetadataList *list, FileMetadata *fmdat) {
    if (list == NULL) {
        fprintf(stderr, "failed to append FileMetadata to null list. Call fileMetadataList_create() first.\n");
        return -1;
    }

    if (fmdat == NULL) {
        fprintf(stderr, "Failed to append null FileMetadata to list. call create_new_fileMetadata() or load_fileMetadata() first.\n");
    }

    _file_node *new_node = _file_node_create(fmdat);

    if (new_node) {
        if (list->tail) { // list is not new
            list->tail->next = new_node;
            list->tail = list->tail->next;
        } else {
            list->head = new_node;
            list->tail = new_node;
        }

        list->count++;
        return 0;
    } else {
        fprintf(stderr, "Failed to create _file_node in fileMetadataList_append().\n");
        return -1;
    }
}

cJSON * jsonify_fileMetadata(FileMetadata* fmdat) {
    cJSON *fmdat_json = cJSON_CreateObject();
    if (fmdat_json) {
        cJSON_AddNumberToObject(fmdat_json, "fid", fmdat->fid);
        cJSON_AddStringToObject(fmdat_json, "fname", fmdat->fname);
        cJSON_AddStringToObject(fmdat_json, "fdate", fmdat->fdate);
        cJSON_AddStringToObject(fmdat_json, "ftype", fmdat->ftype);
        cJSON_AddNumberToObject(fmdat_json, "fsize", fmdat->fsize);
    }
    return fmdat_json;
}

cJSON * jsonify_fileMetadataList(FileMetadataList list) {
    cJSON *json = cJSON_CreateArray();

    _file_node *current = list.head;
    while (current) {
        cJSON_AddItemToArray(json, jsonify_fileMetadata(current->fmdat));
        current = current->next;
    }

    return json;
}

// O(n)
FileMetadata ** to_fileMetadata_array(FileMetadataList list, size_t* size) {
    if (list.count == 0) {
        return NULL;
    }
    FileMetadata **fmdat_arr = malloc(list.count * sizeof(FileMetadata));

    if (fmdat_arr == NULL) {
        fprintf(stderr, "error allocating memory for user array\n");
        return NULL;
    }

    _file_node *current = list.head;
    size_t i = 0;

    while(current->next) {
        fmdat_arr[i] = current->fmdat;
        current = current->next;
        i++;
    }

    if (size != NULL) {
        *size = i;
    }

    return fmdat_arr;
}

static void _free_file_node(_file_node *node) {
    while (node) {
        _file_node *next = node->next;
        free_fileMetadata(node->fmdat);
        free(node);
        node = next;
    }
}

void free_fileMetadataList(FileMetadataList* list) {
    if (list == NULL) {
        return;
    }

    _free_file_node(list->head);
    free(list);
}
