#ifndef OPENSYNCOPENSYNC_CONNECTION_H
#define OPENSYNCOPENSYNC_CONNECTION_H

#include <sqlite3.h>

#define ERR_DB_CREATION -1
#define ERR_DB_CONFLICT -2
#define ERR_DB_PREPARED_STMT -3
#define ERR_DB_INSERTION -4
#define ERR_NULL_POINTER -5
#define ERR_DB_QUERY -6
#define DB_NO_RESULT -7
#define ERR_MEMORY_ALLOCATION -8
#define ERR_DB_EXECUTION -9
#define NO_AFFECTED_ROWS -10
#define ERR_USERLIST_CREATION -11
#define ERR_INVALID_CREDENTIALS -12
#define ERR_BUFFER_TOO_SMALL -13
#define OK 0

sqlite3* get_connection();
int db_initialize(const char *name, int len);
void db_cleanup();

#endif
