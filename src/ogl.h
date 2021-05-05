#pragma once

#include <assert.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "err.h"
#include "format_obj.h"

typedef struct {
    GLuint id;
} GLVertexBuffer;

typedef struct {
    GLuint id;
} GLVertexShader;

typedef struct {
    int screen;
    Display* display;
    Window root;
    GLint* att;
    XVisualInfo* vi;
    Colormap cmap;
    XSetWindowAttributes swa;
    Window window;
    XWindowAttributes gwa;
    GLXContext glc;
    int width, height;
} XGLEnvironment;

void gl_draw_mesh(Mesh* mesh, XGLEnvironment* env, GLenum type);
