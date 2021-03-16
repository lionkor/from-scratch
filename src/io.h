#pragma once

#include "err.h"
#include "mem.h"

NODISCARD bool read_entire_file(const char* filename, ByteBuffer* buf);

typedef struct {
    ByteBuffer* names;
    size_t size;
} FileNameList;

void add_filename(FileNameList* list, const char* directory, const char* filename);
void transfer_filenames(FileNameList* dest, FileNameList* src);
void deallocate_filenames(FileNameList* list);

void change_directory(const char* dir);

FileNameList get_files_in_directory(const char* dir_path);
