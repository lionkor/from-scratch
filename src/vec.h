#pragma once

#include <stdbool.h>

typedef struct {
    double x, y, z;
} Vec3;

double vec3_length(const Vec3* vec);
double vec3_length_squared(const Vec3* vec);
void vec3_normalize(Vec3* vec);
void vec3_div_d(Vec3* vec, double d);
void vec3_div_vec3(Vec3* veca, const Vec3* vecb);
void vec3_mul_d(Vec3* vec, double d);
void vec3_mul_vec3(Vec3* veca, const Vec3* vecb);
void vec3_add_d(Vec3* vec, double d);
void vec3_add_vec3(Vec3* veca, const Vec3* vecb);
void vec3_sub_d(Vec3* vec, double d);
void vec3_sub_vec3(Vec3* veca, const Vec3* vecb);
Vec3 vec3_add_vec3_copy(const Vec3* veca, const Vec3* vecb);
Vec3 vec3_sub_vec3_copy(const Vec3* veca, const Vec3* vecb);
bool vec3_is_unit_vector(const Vec3* vec, double epsilon);
bool vec3_equals(const Vec3* veca, const Vec3* vecb, double epsilon);
void vec3_rotate_by(const Vec3* vec, double degrees);

#define vec3_div(vec, x) _Generic((x),             \
                                  double           \
                                  : vec3_div_d,    \
                                  const Vec3*      \
                                  : vec3_div_vec3, \
                                  Vec3*            \
                                  : vec3_div_vec3)(vec, x)

#define vec3_mul(vec, x) _Generic((x),             \
                                  double           \
                                  : vec3_mul_d,    \
                                  const Vec3*      \
                                  : vec3_mul_vec3, \
                                  Vec3*            \
                                  : vec3_mul_vec3)(vec, x)

#define vec3_add(vec, x) _Generic((x),             \
                                  double           \
                                  : vec3_add_d,    \
                                  Vec3*            \
                                  : vec3_add_vec3, \
                                  const Vec3*      \
                                  : vec3_add_vec3)(vec, x)

#define vec3_sub(vec, x) _Generic((x),             \
                                  double           \
                                  : vec3_sub_d,    \
                                  Vec3*            \
                                  : vec3_sub_vec3, \
                                  const Vec3*      \
                                  : vec3_sub_vec3)(vec, x)
