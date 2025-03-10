#include <kore/kore.h>
#include <kore/http.h>
#include "../migrate.c"
#include "util/config.h"
#include "service/database.h"
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8082

void kore_parent_configure(int argc, char *argv[]);
void kore_parent_configure(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "migrate") == 0) {
        migrate(argc, argv);
        exit(0);
    }

    load_from_file("./config.json");
}
int kore_worker_initialize(void);
int kore_worker_initialize(void) {
    return db_initialize("database.db", strlen("database.db"));
}

void kore_worker_cleanup(void);
void kore_worker_cleanup(void) {
    db_cleanup();
}

/*
 * This is just temporary code. I was tinkering with kore's new body_chunk feature.
 * This will be properly implemented in the near future.
 */

// state of the current upload
struct upload_state {
    int fd;
    char filename[PATH_MAX];
    size_t bytes_written;
    int initialized;
    int in_file_data;
    char boundary[128];
    size_t boundary_len;
    char buffer[4096];
    size_t buffer_len;
};

int upload_handler(struct http_request *);
int body_chunk(struct http_request *, const void *, size_t);
static void extract_boundary(struct upload_state *, const char *);
static int process_chunk(struct upload_state *, const char *, size_t);

int
upload_handler(struct http_request *req)
{
    if (req->method != HTTP_METHOD_POST) {
        http_response(req, 405, NULL, 0);
        return (KORE_RESULT_OK);
    }

    if (req->flags & HTTP_REQUEST_COMPLETE) {
        struct upload_state *state = req->hdlr_extra;

        if (state != NULL) {
            char response[5000];

            if (state->fd != -1) {
                close(state->fd);
            }

            snprintf(response, sizeof(response),
                "Upload complete: %s (%zu bytes)",
                state->filename, state->bytes_written);

            http_response_header(req, "content-type", "text/plain");
            http_response(req, 200, response, strlen(response));

            kore_free(state);
            req->hdlr_extra = NULL;
        } else {
            http_response(req, 200, "Upload complete", 15);
        }

        return (KORE_RESULT_OK);
    }

    return (KORE_RESULT_OK);
}

// boundary extractor from Content-Type header
static void
extract_boundary(struct upload_state *state, const char *content_type)
{
    char *boundary_start;

    // look for boundary in Content-Type
    boundary_start = strstr(content_type, "boundary=");
    if (boundary_start != NULL) {
        boundary_start += 9; // skip "boundary="

        // copy the boundary
        snprintf(state->boundary, sizeof(state->boundary), "--%s", boundary_start);
        state->boundary_len = strlen(state->boundary);

        kore_log(LOG_INFO, "found boundary: %s", state->boundary);
    }
}

// process a chunk of multipart data from body_chunk
static int
process_chunk(struct upload_state *state, const char *data, size_t len)
{
    char *line_start, *filename_start, *filename_end;
    size_t to_write;
    ssize_t written;

    // if we're already in file data mode (no more headers), write directly
    if (state->in_file_data) {
        // check if this chunk contains the boundary
        if (len >= state->boundary_len &&
            memcmp(data, state->boundary, state->boundary_len) == 0) {
            // found the boundary, exit file data mode
            state->in_file_data = 0;
            return (KORE_RESULT_OK);
        }

        if (state->fd != -1) {
            written = write(state->fd, data, len);
            if (written == -1 || (size_t)written != len) {
                kore_log(LOG_ERR, "write error: %s", strerror(errno));
                close(state->fd);
                state->fd = -1;
                return (KORE_RESULT_ERROR);
            }

            state->bytes_written += written;
        }
        return (KORE_RESULT_OK);
    }

    // otherwise, process headers to find file data beginning and skip headers. Ideally, we would read and store those headers somewhere too, but for now, this should work.

    // look for Content-Disposition line
    line_start = strstr(data, "Content-Disposition:");
    if (line_start != NULL) {
        // look for filename
        filename_start = strstr(line_start, "filename=\"");
        if (filename_start != NULL) {
            filename_start += 10; // skip 'filename="'

            // find the closing quote
            filename_end = strchr(filename_start, '"');
            if (filename_end != NULL) {
                *filename_end = '\0';

                // save the filename and add timestamp for uniqueness (should be replaced by more robust system in the future)
                snprintf(state->filename, sizeof(state->filename),
                    "./uploads/upload_%lu_%s", (unsigned long)time(NULL), filename_start);

                *filename_end = '"'; // restore the string

                // open the output file
                state->fd = open(state->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (state->fd == -1) {
                    kore_log(LOG_ERR, "failed to open %s: %s",
                        state->filename, strerror(errno));
                    return (KORE_RESULT_ERROR);
                }

                kore_log(LOG_INFO, "uploading to %s", state->filename);
            }
        }
    }

    // the data only starts after the end of the last few http headers (that kore didn't read), so we need to skip until we find them.
    line_start = strstr(data, "\r\n\r\n");
    if (line_start != NULL) {
        // skip the empty line from multipart request
        line_start += 4;
        to_write = len - (line_start - data);

        // and we can mark that we're now in file data, and not in headers anymore
        state->in_file_data = 1;

        // write the data chunk we have
        if (to_write > 0 && state->fd != -1) {
            written = write(state->fd, line_start, to_write);
            if (written == -1 || (size_t)written != to_write) {
                kore_log(LOG_ERR, "write error: %s", strerror(errno));
                close(state->fd);
                state->fd = -1;
                return (KORE_RESULT_ERROR);
            }

            state->bytes_written += written;
        }
    }

    return (KORE_RESULT_OK);
}

// handler for each chunk of the uploaded file (set in opensync.conf)
int
body_chunk(struct http_request *req, const void *data, size_t len)
{
    struct upload_state *state;
    const char *content_type;

    kore_log(LOG_INFO, "received %zu bytes", len);

    // init state if this is the first chunk
    if (req->hdlr_extra == NULL) {
        state = kore_calloc(1, sizeof(struct upload_state));
        state->fd = -1;
        state->bytes_written = 0;
        state->initialized = 0;
        state->in_file_data = 0;
        state->boundary_len = 0;
        state->buffer_len = 0;

        req->hdlr_extra = state;

        // extract boundary from Content-Type
        if (http_request_header(req, "content-type", &content_type)) {
            extract_boundary(state, content_type);
        }
    } else {
        state = req->hdlr_extra;
    }

    // if there is a boundary, just process the chunk
    if (state->boundary_len > 0) {
        return process_chunk(state, data, len);
    } else {
        // otherwise, we need to buffer the data until we can find headers
        if (state->buffer_len + len <= sizeof(state->buffer)) {
            // add it to the buffer
            memcpy(state->buffer + state->buffer_len, data, len);
            state->buffer_len += len;

            // try to process the chunk
            return process_chunk(state, state->buffer, state->buffer_len);
        } else {
            kore_log(LOG_ERR, "buffer overflow, data too large");
            return (KORE_RESULT_ERROR);
        }
    }
}