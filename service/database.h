#ifndef HYPRSYNC_BACKEND_CONNECTION_H
#define HYPRSYNC_BACKEND_CONNECTION_H

#include <sqlite3.h>

#define ERR_DB_CREATION -1
#define ERR_DB_CONFLICT -2
#define ERR_DB_PREPARED_STMT -3
#define ERR_DB_INSERTION -4;
#define OK 0

sqlite3* get_connection();

#endif
