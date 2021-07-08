#include "vec.h"
#include "err.h"

#include <math.h>

double vec3_length(const Vec3* vec) {
    return sqrt(vec->x * vec->x
        + vec->y * vec->y
        + vec->z * vec->z);
}

void vec3_normalize(Vec3* vec) {
    vec3_div(vec, vec3_length(vec));
}

void vec3_div_d(Vec3* vec, double d) {
    vec->x /= d;
    vec->y /= d;
    vec->z /= d;
}

void vec3_div_vec3(Vec3* veca, const Vec3* vecb) {
#ifndef VEC3_DIV_VEC3_OK
    plog("warning: vec3/vec3 was invoked - this is likely not what you want. define 'VEC3_DIV_VEC3_OK' to disable this warning.");
#endif
    veca->x /= vecb->x;
    veca->y /= vecb->y;
    veca->z /= vecb->z;
}

void vec3_mul_d(Vec3* vec, double d) {
    vec->x *= d;
    vec->y *= d;
    vec->z *= d;
}

void vec3_mul_vec3(Vec3* veca, const Vec3* vecb) {
#ifndef VEC3_MUL_VEC3_OK
    plog("warning: vec3*vec3 was invoked - this is likely not what you want. define 'VEC3_MUL_VEC3_OK' to disable this warning.");
#endif
    veca->x *= vecb->x;
    veca->y *= vecb->y;
    veca->z *= vecb->z;
}

void vec3_add_d(Vec3* vec, double d) {
#ifndef VEC3_ADD_D_OK
    plog("warning: vec3+d was invoked - this is likely not what you want. define 'VEC3_ADD_D_OK' to disable this warning.");
#endif
    vec->x += d;
    vec->y += d;
    vec->z += d;
}

void vec3_add_vec3(Vec3* veca, const Vec3* vecb) {
    veca->x += vecb->x;
    veca->y += vecb->y;
    veca->z += vecb->z;
}

double vec3_length_squared(const Vec3* vec) {
    return vec->x * vec->x
        + vec->y * vec->y
        + vec->z * vec->z;
}

void vec3_sub_d(Vec3* vec, double d) {
#ifndef VEC3_SUB_D_OK
    plog("warning: vec3-d was invoked - this is likely not what you want. define 'VEC3_SUB_D_OK' to disable this warning.");
#endif
    vec->x -= d;
    vec->y -= d;
    vec->z -= d;
}

void vec3_sub_vec3(Vec3* veca, const Vec3* vecb) {
    veca->x -= vecb->x;
    veca->y -= vecb->y;
    veca->z -= vecb->z;
}

bool vec3_is_unit_vector(const Vec3* vec, double epsilon) {
    return fabs(vec3_length(vec) - 1.0) < epsilon;
}

bool vec3_equals(const Vec3* veca, const Vec3* vecb, double epsilon) {
    bool x_eq = fabs(veca->x - vecb->x) < epsilon;
    bool y_eq = fabs(veca->y - vecb->y) < epsilon;
    bool z_eq = fabs(veca->z - vecb->z) < epsilon;
    return x_eq && y_eq && z_eq;
}

Vec3 vec3_add_vec3_copy(const Vec3* veca, const Vec3* vecb) {
    return (Vec3) {
        .x = veca->x + vecb->x,
        .y = veca->y + vecb->y,
        .z = veca->z + vecb->z
    };
}

Vec3 vec3_sub_vec3_copy(const Vec3* veca, const Vec3* vecb) {
    return (Vec3) {
        .x = veca->x - vecb->x,
        .y = veca->y - vecb->y,
        .z = veca->z - vecb->z
    };
}

void vec3_rotate_by(const Vec3* vec, double degrees) {
}
