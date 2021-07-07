#pragma once

#include "vec.h"

typedef struct {
    Vec3 pos;
    Vec3 up;
    Vec3 forward;
} Camera;

void move_camera(Camera* cam, const Vec3* forward, double speed);
void init_camera(Camera* cam);
void camera_look_at(Camera* cam, const Vec3* target);
