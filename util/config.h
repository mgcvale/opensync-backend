#ifndef OPENSYNC_CONFIG_H
#define OPENSYNC_CONFIG_H

#define SUCCESS 0
#define WARNING 1
#define ERROR 2
#define FATAL_ERROR 3

extern char user_data_dir[128];

void load_defaults();
int load_from_file(const char *config_file_path);
int initialize();

#endif //OPENSYNC_CONFIG_H
