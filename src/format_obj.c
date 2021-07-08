#include "format_obj.h"
#include "mem.h"
#include "res.h"
#include <assert.h>
#include <ctype.h>

static void skip_line(ByteBuffer* file, size_t* i) {
    //log("skipping a line");
    for (; file->bytes[*i] != '\n'; ++*i)
        ;
}

static char buf[16];
// result invalidated on the next call
static char* read_until_space(ByteBuffer* filebuf, size_t* i, size_t* len) {
    assert(filebuf);
    assert(i);
    assert(len);
    *len = 0;
    // zero allocation
    memset(buf, 0, sizeof(buf));
    // read until space or newline
    ++*i;
    while (!isspace(filebuf->bytes[*i])) {
        buf[*len] = (char)filebuf->bytes[*i];
        ++*len;
        ++*i;
    }
    return buf;
}

static bool handle_vertex(ByteBuffer* filebuf, size_t* i, Mesh* mesh) {
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
            plog("reallocate failed, cannot continue (out of memory?)");
            return false;
        }
    }
    // select last vertex
    MeshVertex* vert = &mesh->vertices[mesh->vertex_count - 1];
    for (size_t k = 0; k < 3; ++k) {
        size_t maybe_number_len;
        char* maybe_number = read_until_space(filebuf, i, &maybe_number_len);
        if (maybe_number_len == 0) {
            plog("expected vertex number %lu, got eof, error, space or similar instead", k);
            return false;
        }
        // FIXME: we're assuming this just works, does it? check the return.
        vert->coords[k] = atof(maybe_number);
    }
    //log("parsed location %lf, %lf, %lf", vert->coords[0], vert->coords[1], vert->coords[2]);
    return true;
}

static bool handle_v(ByteBuffer* filebuf, size_t* i, Mesh* mesh) {
    char c = (char)filebuf->bytes[++(*i)];
    switch (c) {
    case ' ':
        return handle_vertex(filebuf, i, mesh);
    case 'n':
    case 't':
        // not handled
        return true;
    default:
        plog("v%c is unknown, (is this file well-formed?) ignoring for now", c);
        return true;
    }
    return true;
}

static bool handle_f(ByteBuffer* filebuf, size_t* i, Mesh* mesh) {
    char should_be_space = (char)filebuf->bytes[++(*i)];
    if (should_be_space != ' ') {
        plog("expected space, got %c (%d)", should_be_space, should_be_space);
        return false;
    }
    if (mesh->face_elements == NULL) {
        mesh->face_elements = allocate(sizeof(FaceElement));
        mesh->face_element_count += 1;
    } else {
        bool ok = reallocate((void**)&mesh->face_elements, (mesh->face_element_count + 1) * sizeof(FaceElement));
        mesh->face_element_count += 1;
        if (!ok) {
            plog("reallocate failed, cannot continue (out of memory?)");
            return false;
        }
    }
    // select last vertex
    FaceElement* face_elem = &mesh->face_elements[mesh->face_element_count - 1];
    for (size_t k = 0; k < 3; ++k) {
        size_t maybe_number_len;
        char* maybe_number = read_until_space(filebuf, i, &maybe_number_len);
        if (maybe_number_len == 0) {
            plog("expected vertex number %lu, got eof, error, space or similar instead", k);
            return false;
        }
        // FIXME: we're assuming this just works, does it? check the return.
        char* end;
        face_elem->indices[k] = (size_t)strtol(maybe_number, &end, 10);
        //log("%p, %p", maybe_number, end);
    }
    //log("parsed face element %lu, %lu, %lu", face_elem->indices[0], face_elem->indices[1], face_elem->indices[2]);
    return true;
}

// assumes well-formed obj files
bool parse_obj_file(const char* filename, Mesh* mesh) {
    assert(filename);
    assert(mesh);
    memset(mesh, 0, sizeof(Mesh));
    ByteBuffer* filebuf = get_resource(filename);
    if (!filebuf || filebuf->size == 0) {
        plog("file \"%s\" isn't a good obj file.", filename);
        return false;
    }
    for (size_t i = 0; i < filebuf->size; ++i) {
        char c = (char)filebuf->bytes[i];
        //log("c: %c aka %d", c, c);
        switch (c) {
        case ' ':
        case '#':
        case '\t':
            skip_line(filebuf, &i);
            break;
        case '\n':
            // consumed, move on
            break;
        case 'v': {
            // could be 'v' straight, 'vt' or 'vn', gets complicated,
            // handle_v does that
            bool res = handle_v(filebuf, &i, mesh);
            if (!res) {
                return false;
            }
            break;
        }
        case 'f': {
            bool res = handle_f(filebuf, &i, mesh);
            if (!res) {
                return false;
            }
            break;
        }
        default:
            // FIXME
            skip_line(filebuf, &i);
            break;
        }
    }
    return true;
}

void deallocate_mesh(Mesh* mesh) {
    deallocate((void**)&mesh->face_elements);
    deallocate((void**)&mesh->vertices);
}
