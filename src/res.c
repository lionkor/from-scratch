#include "res.h"

#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>

#include "io.h"

char res_path[257];
bool is_set = false;

void set_resource_folder(const char* folder_path) {
    assert(folder_path);
    size_t folder_path_len = strlen(folder_path);
    memset(res_path, 0, sizeof(res_path));
    if (folder_path_len > sizeof(res_path)) {
        plog("warning: resource folder path too long, will cause issues");
    }
    struct stat st;
    int res = stat(folder_path, &st);
    if (res != 0) {
        log_perror("stat");
        plog("\"%s\" is not a valid path, resource folder will remain current working directory", folder_path);
        return;
    }
    if (!(S_ISDIR(st.st_mode))) {
        plog("\"%s\" is not a directory, cannot be resource folder, resource folder will remain current working directory", folder_path);
    }
    strcpy(res_path, folder_path);
    if (res_path[folder_path_len - 1] != '/') {
        plog("new resource folder path doesn't end with '/', appending");
        assert(folder_path_len < sizeof(res_path));
        res_path[folder_path_len] = '/';
    }
    is_set = true;
    plog("set resource folder to \"%s\"", res_path);
}

const char* get_resource_folder() {
    if (is_set) {
        return res_path;
    } else {
        return "./";
    }
}

ByteBuffer make_res_path(const char* filename) {
    size_t fn_len = strlen(filename);
    const char* res = get_resource_folder();
    size_t res_len = strlen(res);
    ByteBuffer path = allocate_byte_buffer(fn_len + strlen(res) + 1);
    memcpy(path.bytes + 0, res, res_len);
    memcpy(path.bytes + res_len, filename, fn_len);
    path.bytes[path.size - 1] = 0; // null-terminate
    plog("resolved full res path: \"%s\"", path.bytes);
    return path;
}

static bool res_read_file(const char* filename, ByteBuffer* buf) {
    ByteBuffer path = make_res_path(filename);
    bool ok = read_entire_file((char*)path.bytes, buf);
    deallocate_byte_buffer(&path);
    return ok;
}

typedef struct {
    char name[128];
    ByteBuffer buf;
} NamedBuffer;

NamedBuffer* g_files = NULL;
size_t g_files_count = 0;

void init_resource_manager() {
    if (g_files) {
        plog("resource manager already initialized, deinitializing and reinitializing");
        deinit_resource_manager();
    }
    // list all files in resource folder
    const char* folder = get_resource_folder();
    FileNameList list = get_files_in_directory(folder);
    if (list.size == 0) {
        plog("warning: resource folder \"%s\" is empty", folder);
    }
    g_files = allocate(list.size * sizeof(NamedBuffer));
    g_files_count = list.size;
    for (size_t i = 0; i < list.size; ++i) {
        memcpy(g_files[i].name, list.names[i].bytes, list.names[i].size);
        bool ok = read_entire_file(g_files[i].name, &g_files[i].buf);
        if (!ok) {
            plog("failed to read resource \"%s\"", g_files[i].name);
        }
    }
    deallocate_filenames(&list);
}

void deinit_resource_manager() {
    if (g_files) {
        plog("deallocating resources");
        for (size_t i = 0; i < g_files_count; ++i) {
            deallocate_byte_buffer(&g_files[i].buf);
        }
        deallocate((void**)&g_files);
    } else {
        plog("resource manager deinitializing, even though it is not initialized");
    }
}

ByteBuffer* get_resource(const char* name) {
    assert(g_files);
    for (size_t i = 0; i < g_files_count; ++i) {
        if (strcmp(g_files[i].name, name) == 0) {
            return &g_files[i].buf;
        }
    }
    plog("not found in resources: %s", name);
    return NULL;
}
