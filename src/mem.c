#include "mem.h"
#include <assert.h>
#include <stdatomic.h>

static atomic_size_t s_allocated = 0;

void* allocate(size_t n) {
    assert(n > 0);
    void* ptr = malloc(n);
    if (!ptr) {
        log_perror("malloc");
        return ptr;
    }
    ++s_allocated;
    return ptr;
}

void deallocate(void** ptr_ptr) {
    assert(ptr_ptr);
    free(*ptr_ptr);
    --s_allocated;
    *ptr_ptr = NULL;
}

bool reallocate(void** ptr, size_t new_size) {
    assert(ptr);
    assert(new_size > 0);
    void* new_ptr = realloc(*ptr, new_size);
    if (!new_ptr) {
        log_perror("realloc");
        return false;
    }
    *ptr = new_ptr;
    return true;
}

size_t allocated() {
    return s_allocated;
}

OwnPtr safe_allocate(size_t n) {
    return (OwnPtr) { allocate(n) };
}

void safe_deallocate(OwnPtr* ownptr) {
    assert(ownptr);
    deallocate(&ownptr->ptr);
}

bool safe_reallocate(OwnPtr* ownptr, size_t new_size) {
    assert(ownptr);
    return reallocate(&ownptr->ptr, new_size);
}

ByteBuffer allocate_byte_buffer(size_t n) {
    return (ByteBuffer){ allocate(n), n };
}

void deallocate_byte_buffer(ByteBuffer* buf) {
    assert(buf);
    deallocate((void**)&buf->bytes);
    buf->size = 0;
}

bool resize_byte_buffer(ByteBuffer* buf, size_t new_size) {
    assert(buf);
    bool ok = reallocate((void**)&buf->bytes, new_size);
    if (!ok) {
        return false;
    }
    buf->size = new_size;
    return ok;
}
