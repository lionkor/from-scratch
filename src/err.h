#pragma once

#include <errno.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define plog(...)                                        \
    do {                                                 \
        printf("%s:%d: ", basename(__FILE__), __LINE__); \
        printf(__VA_ARGS__);                             \
        putchar('\n');                                   \
    } while (false)
#define log_perror(context) plog("%s: error: %s", context, strerror(errno))
#define NODISCARD __attribute__((warn_unused_result))
