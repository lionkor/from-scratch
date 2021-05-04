#include "format_obj.h"
#include "mem.h"
#include <assert.h>
#include <ctype.h>

static void skip_line(FILE* file) {
    // log("skipping a line");
    while (!feof(file) && !ferror(file)) {
        if (fgetc(file) == '\n') {
            break;
        }
    }
}

static char buf[16];
// result invalidated on the next call
static char* read_until_space(FILE* file, size_t* len) {
    assert(len);
    // zero allocation
    memset(buf, 0, sizeof(buf));
    // read until space, error, eof, or newline
    size_t i = 0;
    while (!feof(file) && !ferror(file)) {
        char c = (char)fgetc(file);
        if (isspace(c)) {
            break;
        } else {
            buf[i] = c;
            ++i;
        }
    }
    *len = i;
    return buf;
}

static bool handle_vertex(FILE* file, Mesh* mesh) {
    // format:
    //  v %X %Y %Z %W
    // where %W is optional.
    // at this point, the 'v ' (v, space) parts have been consumed already.
    // for now, we ignore %W. <- FIXME
    if (mesh->vertices == NULL) {
        mesh->vertices = allocate(sizeof(MeshVertex));
        mesh->vertex_count += 1;
    } else {
        bool ok = reallocate((void**)&mesh->vertices, (mesh->vertex_count + 1) * sizeof(MeshVertex));
        mesh->vertex_count += 1;
        if (!ok) {
            log("reallocate failed, cannot continue (out of memory?)");
            return false;
        }
    }
    // select last vertex
    MeshVertex* vert = &mesh->vertices[mesh->vertex_count - 1];
    for (size_t i = 0; i < 3; ++i) {
        size_t maybe_number_len;
        char* maybe_number = read_until_space(file, &maybe_number_len);
        if (maybe_number_len == 0) {
            log("expected vertex number %lu, got eof, error, space or similar instead", i);
            return false;
        }
        // FIXME: we're assuming this just works, does it? check the return.
        vert->coords[i] = atof(maybe_number);
    }
    log("parsed location %lf, %lf, %lf", vert->coords[0], vert->coords[1], vert->coords[2]);
    return true;
}

static bool handle_v(FILE* file, Mesh* mesh) {
    char c = (char)fgetc(file);
    switch (c) {
    case ' ':
        return handle_vertex(file, mesh);
    case 'n':
    case 't':
        // not handled
        return true;
    default:
        log("v%c is unknown, (is this file well-formed?) ignoring for now", c);
        return true;
    }
    return true;
}

static bool handle_f(FILE* file, Mesh* mesh) {
    char should_be_space = fgetc(file);
    if (should_be_space != ' ') {
        log("expected space, got %c (%d)", should_be_space, should_be_space);
        return false;
    }
    if (mesh->face_elements == NULL) {
        mesh->face_elements = allocate(sizeof(FaceElement));
        mesh->face_element_count += 1;
    } else {
        bool ok = reallocate((void**)&mesh->face_elements, (mesh->face_element_count + 1) * sizeof(FaceElement));
        mesh->face_element_count += 1;
        if (!ok) {
            log("reallocate failed, cannot continue (out of memory?)");
            return false;
        }
    }
    // select last vertex
    FaceElement* face_elem = &mesh->face_elements[mesh->face_element_count - 1];
    for (size_t i = 0; i < 3; ++i) {
        size_t maybe_number_len;
        char* maybe_number = read_until_space(file, &maybe_number_len);
        if (maybe_number_len == 0) {
            log("expected vertex number %lu, got eof, error, space or similar instead", i);
            return false;
        }
        // FIXME: we're assuming this just works, does it? check the return.
        face_elem->indices[i] = (size_t)strtol(maybe_number, NULL, 10);
    }
    log("parsed face element %lu, %lu, %lu", face_elem->indices[0], face_elem->indices[1], face_elem->indices[2]);
    return true;
}

// assumes well-formed obj files
bool parse_obj_file(const char* filename, Mesh* mesh) {
    assert(filename);
    assert(mesh);
    memset(mesh, 0, sizeof(Mesh));
    FILE* file = fopen(filename, "r");
    if (!file) {
        log_perror("fopen");
        return false;
    }
    while (!feof(file) && !ferror(file)) {
        char c = fgetc(file);
        //log("c: %c aka %d", c, c);
        switch (c) {
        case ' ':
        case '#':
        case '\t':
            skip_line(file);
            break;
        case '\n':
            // consumed, move on
            break;
        case 'v': {
            // could be 'v' straight, 'vt' or 'vn', gets complicated,
            // handle_v does that
            bool res = handle_v(file, mesh);
            if (!res) {
                return false;
            }
            break;
        }
        case 'f': {
            bool res = handle_f(file, mesh);
            if (!res) {
                return false;
            }
            break;
        }
        default:
            // FIXME
            skip_line(file);
            break;
        }
    }
    return true;
}

void deallocate_mesh(Mesh* mesh) {
    deallocate((void**)&mesh->face_elements);
    deallocate((void**)&mesh->vertices);
}
