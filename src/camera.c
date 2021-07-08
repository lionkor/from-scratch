#include "camera.h"
#include "err.h"
#include "mem.h"
#include <stdlib.h>

void move_camera(Camera* cam, const Vec3* forward, double speed) {
}

void camera_look_at(Camera* cam, const Vec3* target) {
}

Camera* new_camera() {
    Camera* cam = (Camera*)malloc(sizeof(Camera));
    cam->forward = (Vec3) { 0.0, 0.0, 0.0 };
    cam->pos = (Vec3) { 0.0, 0.0, 0.0 };
    cam->up = (Vec3) { 0.0, 1.0, 0.0 };
    return cam;
}

void free_camera(Camera* cam) {
}
