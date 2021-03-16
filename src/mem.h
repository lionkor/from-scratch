#pragma once

#include "err.h"
#include <stdlib.h>
#include <stdint.h>

// a pointer type which owns the allocated pointer, has to be free'd
typedef struct {
    void* ptr;
} OwnPtr;

NODISCARD void* allocate(size_t n);
void deallocate(void** ptr_ptr);
NODISCARD bool reallocate(void** ptr, size_t new_size);
size_t allocated();

NODISCARD OwnPtr safe_allocate(size_t n);
void safe_deallocate(OwnPtr* ownptr);
NODISCARD bool safe_reallocate(OwnPtr* ownptr, size_t new_size);

typedef struct {
    uint8_t* bytes;
    size_t size;
} ByteBuffer;

NODISCARD ByteBuffer allocate_byte_buffer(size_t n);
void deallocate_byte_buffer(ByteBuffer* buf);
NODISCARD bool resize_byte_buffer(ByteBuffer* buf, size_t new_size);
