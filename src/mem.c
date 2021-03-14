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
