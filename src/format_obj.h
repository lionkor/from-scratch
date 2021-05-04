#pragma once

#include "err.h"

typedef struct {
    double coords[3];
} MeshVertex;

typedef struct {
    size_t indices[3];
} FaceElement;

typedef struct {
    MeshVertex* vertices;
    size_t vertex_count;
    FaceElement* face_elements;
    size_t face_element_count;
} Mesh;

bool parse_obj_file(const char* filename, Mesh* mesh);
void deallocate_mesh(Mesh* mesh);
