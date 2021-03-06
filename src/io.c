#include "io.h"

#include <assert.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

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
        plog("warning: file \"%s\" is empty, buffer will be NULL", filename);
        buf->bytes = NULL;
        buf->size = 0;
        return true;
    }
    if (!(S_ISREG(st.st_mode))) {
        plog("error: \"%s\" is not a regular file", filename);
        return false;
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
        plog("warning: read unexpected amount of data from \"%s\" - expected to read %lu, instead read %lu bytes", filename, size, n_read);
        // FIXME: what now?
    }
    return true;
}

static inline void get_files_in_directory_recursive(FileNameList* files, const char* dir_path) {
    plog("getting files in \"%s\"", dir_path);
    DIR* d;
    struct dirent* dir;
    d = opendir(dir_path);
    size_t dir_path_len = strlen(dir_path);
    char cwd_full[256];
    realpath(".", cwd_full);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            struct stat st;
            change_directory(dir_path);
            int res = stat(dir->d_name, &st);
            change_directory(cwd_full);
            assert(res == 0); // should never fail
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
                // these are special, ignore
            } else if (S_ISDIR(st.st_mode)) {
                char dirname_buf[256];
                assert(dir_path_len < sizeof(dirname_buf));
                memcpy(dirname_buf, dir_path, dir_path_len);
                char* next = dirname_buf + dir_path_len;
                if (dirname_buf[dir_path_len - 1] != '/') {
                    dirname_buf[dir_path_len] = '/';
                    next += 1;
                }
                size_t d_name_len = strlen(dir->d_name);
                memcpy(next, dir->d_name, d_name_len);
                next[d_name_len] = 0;
                get_files_in_directory_recursive(files, dirname_buf);
                // make sure we deallocated properly (sanity check)
            } else if (S_ISREG(st.st_mode)) {
                add_filename(files, dir_path, dir->d_name);
            }
        }
        closedir(d);
    }
}

FileNameList get_files_in_directory(const char* dir_path) {
    //change_directory(dir_path);
    FileNameList list = { NULL, 0 };
    get_files_in_directory_recursive(&list, dir_path);
    //change_directory("..");
    return list;
}

void add_filename(FileNameList* list, const char* directory, const char* filename) {
    assert(list);
    assert(directory);
    assert(filename);

    list->size += 1;
    bool ok = reallocate((void**)&list->names, list->size * (sizeof(ByteBuffer)));
    assert(ok);

    size_t dir_len = strlen(directory);
    size_t file_len = strlen(filename);
    // allocate_byte_buffer(dir_len + file_len + 2); // +2 for / and \0
    size_t size = dir_len + file_len + 2;
    list->names[list->size - 1].size = size;
    list->names[list->size - 1].bytes = allocate(size);
    ByteBuffer* new_buf = &list->names[list->size - 1];
    memset(new_buf->bytes, 0, new_buf->size);
    memcpy(new_buf->bytes, directory, dir_len);
    // ensure '/'
    uint8_t* next_pos = new_buf->bytes + dir_len;
    if (new_buf->bytes[dir_len - 1] != '/') {
        new_buf->bytes[dir_len] = '/';
        next_pos += 1;
    }
    memcpy(next_pos, filename, file_len);
}

/*
*/

void transfer_filenames(FileNameList* dest, FileNameList* src) {
    assert(dest);
    assert(src);
    if (!dest->names) {
        dest->size = src->size;
        dest->names = src->names;
        src->size = 0;
        src->names = NULL;
    } else {
        bool ok = reallocate((void**)&dest->names, (dest->size + src->size) * (sizeof(ByteBuffer)));
        ByteBuffer* iter = &dest->names[dest->size];
        dest->size += src->size;
        assert(ok);
        for (size_t i = 0; i < src->size; ++i) {
            *iter = src->names[i];
            iter += 1;
        }
        deallocate((void**)&src->names);
        src->size = 0;
    }
}

void change_directory(const char* dir) {
    //log("changing working directory to \"%s\"", dir);
    int res = chdir(dir);
    if (res != 0) {
        log_perror("chdir");
    }
}

void deallocate_filenames(FileNameList* list) {
    assert(list);
    for (size_t i = 0; i < list->size; ++i) {
        deallocate((void**)&list->names[i].bytes);
    }
    deallocate((void**)&list->names);
}
