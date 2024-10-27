#ifndef OPENSYNC_FILE_H
#define OPENSYNC_FILE_H

#include <stdlib.h>
#include <cjson/cJSON.h>
#include "user.h"

#define FMDAT_FNAME_LEN 128
#define FMDAT_FTYPE_LEN 8
#define FMDAT_FDATE_LEN 24

typedef struct {
    size_t fid;
    char fname[128];
    char ftype[8];
    size_t fsize;
    char fdate[24]; // YYYY-MM-DDTHH:MM:SS as specified by the ISO 8601

    // FK
    int fowner_id;
} FileMetadata;

typedef struct _file_node {
    FileMetadata *fmdat;
    struct _file_node *next;
} _file_node;

typedef struct {
    _file_node *head;
    _file_node *tail;
    int count;
} FileMetadataList;

FileMetadata* create_new_fileMetadata(const char *fname, const char *ftype, const char *fdate, size_t fsize, int fowner_id);
FileMetadata* load_fileMetadata(size_t id, const char *fname, const char *ftype, const char *fdate, size_t fsize, int fowner_id);
void free_fileMetadata(FileMetadata *fmdat);

FileMetadataList *fileMetadataList_create();
int fileMetadataList_append(FileMetadataList *list, FileMetadata *user);
FileMetadata **to_fileMetadata_array(FileMetadataList list, size_t *size);
cJSON *jsonify_fileMetadata(FileMetadata *user);
cJSON *jsonify_fileMetadataList(FileMetadataList list);
void free_fileMetadataList(FileMetadataList *list);

#endif //OPENSYNC_FILE_H
