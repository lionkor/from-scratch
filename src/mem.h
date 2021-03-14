#pragma once

#include "err.h"
#include <stdlib.h>

void* allocate(size_t n);
void deallocate(void** ptr_ptr);
NODISCARD bool reallocate(void** ptr, size_t new_size);
size_t allocated();
