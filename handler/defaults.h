#ifndef HYPRSYNC_BACKEND_DEFAULTS_H
#define HYPRSYNC_BACKEND_DEFAULTS_H

#include <microhttpd.h>

enum MHD_Result default_404(struct MHD_Connection *connection);

enum MHD_Result default_400(struct MHD_Connection *connection);

enum MHD_Result default_405(struct MHD_Connection *connection);

#endif //HYPRSYNC_BACKEND_DEFAULTS_H
