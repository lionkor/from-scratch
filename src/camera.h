#pragma once

#include "vec.h"

typedef struct {
    Vec3 pos;
    Vec3 up; // not used!
    Vec3 forward;
} Camera;

Camera* new_camera(void);
void free_camera(Camera** cam);
void move_camera(Camera* cam, const Vec3* forward, double speed);
void camera_look_at(Camera* cam, const Vec3* target);
void plog_camera(const Camera* cam);
