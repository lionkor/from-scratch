#include "res.h"

#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>

bool read_entire_file(const char* filename, ByteBuffer* buf) {
    assert(filename);
    assert(buf);
    struct stat st;
    int res = stat(filename, &st);
    if (res != 0) {
        log_perror("stat");
        return false;
    }
    size_t size = st.st_size;
    if (size == 0) {
        log("warning: file \"%s\" is empty, buffer will be NULL", filename);
        buf->bytes = NULL;
        buf->size = 0;
        return true;
    }
    FILE* file = fopen(filename, "rb");
    if (!file) {
        log_perror("fopen");
        return false;
    }
    *buf = allocate_byte_buffer(size);
    size_t n_read = fread(buf->bytes, 1, buf->size, file);
    fclose(file);
    if (n_read != buf->size) {
        log("warning: read unexpected amount of data from \"%s\" - expected to read %lu, instead read %lu bytes", filename, size, n_read);
        // FIXME: what now?
    }
    return true;
}
