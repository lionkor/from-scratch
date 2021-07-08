#include "camera.h"
#include "err.h"
#include "mem.h"
#include <stdlib.h>

void move_camera(Camera* cam, const Vec3* forward, double speed) {
    Vec3 copy = *forward;
    vec3_mul(&copy, speed);
    vec3_add(&cam->pos, &copy);
}

void camera_look_at(Camera* cam, const Vec3* target) {
    Vec3 new = vec3_sub_vec3_copy(target, &cam->forward);
    vec3_normalize(&new);
    cam->forward = new;
}

Camera* new_camera() {
    Camera* cam = (Camera*)allocate(sizeof(Camera));
    cam->forward = (Vec3) { 1.0, 0.0, 0.0 };
    cam->pos = (Vec3) { 0.0, 0.0, 0.0 };
    cam->up = (Vec3) { 0.0, 1.0, 0.0 };
    return cam;
}

void free_camera(Camera** cam) {
    deallocate((void**)cam);
}

void plog_camera(const Camera* cam) {
    plog("Camera forward: %0.2f, %0.2f, %0.2f\n", cam->forward.x, cam->forward.y, cam->forward.z);
    plog("Camera pos:     %0.2f, %0.2f, %0.2f\n", cam->pos.x, cam->pos.y, cam->pos.z);
}
