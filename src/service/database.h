#ifndef OPENSYNCOPENSYNC_CONNECTION_H
#define OPENSYNCOPENSYNC_CONNECTION_H

#include <sqlite3.h>

sqlite3* get_connection(void);
int db_initialize(const char *name, int len);
void db_cleanup(void);

#endif
