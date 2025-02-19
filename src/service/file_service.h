#ifndef OPENSYNC_FILE_SERVICE_H
#define OPENSYNC_FILE_SERVICE_H

#include "../model/user.h"
#include "../model/file.h"

int save_file(const unsigned char *blob, size_t fsize, const char *fname, char *ownerid);
int load_file_metadata_to_db(const char *fname, size_t fsize, size_t ownerid, char *ownername);
int get_user_files(const char *token, FileMetadataList *list);
int get_file_contents(char *fname, char *ownername, unsigned char *blob);
int get_fileMetadata(char *fname, char *ownername, FileMetadata *fmdat);

#endif //OPENSYNC_FILE_SERVICE_H
