#pragma once

#include "err.h"
#include "mem.h"

NODISCARD bool read_entire_file(const char* filename, ByteBuffer* buf);

void set_resource_folder(const char* folder_path, size_t folder_path_len);
const char* get_resource_folder();
NODISCARD bool res_read_entire_file(const char* filename, ByteBuffer* buf);
