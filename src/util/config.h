#ifndef OPENSYNC_CONFIG_H
#define OPENSYNC_CONFIG_H

#define SUCCESS 0
#define ERROR 2
#define FATAL_ERROR 3

extern char user_data_dir[128];

void load_defaults(void);
int load_from_file(const char *config_file_path);
int initialize(void);

#endif //OPENSYNC_CONFIG_H
