#include "camera.h"

void init_camera(Camera* cam) {
    cam->forward = {};
    cam->pos = { 0.0, 0.0, 0.0 };
    cam->up = { 0.0, 1.0, 0.0 };
}

void move_camera(Camera* cam, Vec3* forward, double speed) {
}

void camera_look_at(Camera* cam, const Vec3* target) {
}
