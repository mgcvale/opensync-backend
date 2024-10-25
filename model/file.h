#ifndef OPENSYNC_FILE_H
#define OPENSYNC_FILE_H

#include <stdlib.h>

typedef struct {
    char fname[128];
    char ftype[6];
    size_t fsize;
    char fdate[24]; // YYYY-MM-DDTHH:MM:SS as specified by the ISO 8601
} FileMetadata;

typedef struct _file_node {
    FileMetadata *file;
    struct _file_node *next;
} _file_node;

typedef struct {
    _file_node *head;
    _file_node *tail;
    int count;
} FileMetadataList;


FileMetadata

#endif //OPENSYNC_FILE_H
