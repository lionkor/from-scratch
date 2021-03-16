#pragma once

#include "err.h"
#include "mem.h"

void set_resource_folder(const char* folder_path);
const char* get_resource_folder();
ByteBuffer make_res_path(const char* filename);

void init_resource_manager();
void deinit_resource_manager();
ByteBuffer* get_resource(const char* name);
