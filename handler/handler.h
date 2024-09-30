#ifndef HYPRSYNC_BACKEND_HANDLER_H
#define HYPRSYNC_BACKEND_HANDLER_H

#include <setjmp.h>
#include <microhttpd.h>

extern jmp_buf exceptionBuffer;

#define try if (setjmp(exceptionBuffer) == 0)
#define except else

void log_api(const char *url, const char *method);

enum MHD_Result handler(void *cls, struct MHD_Connection *connection,
                   const char *url, const char *method,
                   const char *version, const char *upload_data,
                   size_t *upload_data_size, void **con_cls);

#endif
