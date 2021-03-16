#include "ogl.h"

void gl_draw_mesh(Mesh* mesh, XGLEnvironment* env, GLenum type) {
    assert(mesh);
    assert(env);
    glBegin(type);
    {
        for (size_t i = 0; i < mesh->face_element_count; ++i) {
            FaceElement* cur = &mesh->face_elements[i];
            for (size_t k = 0; k < 3; ++k) {
                MeshVertex* vert = &mesh->vertices[cur->indices[k] - 1];
                // log("%lu,%lu: drawing %lf, %lf, %lf", i, k, vert->coords[0], vert->coords[1], vert->coords[2]);
                glColor3f(vert->coords[0], vert->coords[1], vert->coords[2]);
                glVertex3f(vert->coords[0], vert->coords[1], vert->coords[2]);
            }
        }
    }
    glEnd();
}
