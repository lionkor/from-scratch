#define _GNU_SOURCE

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#define deg_to_rad(angledegrees) ((angledegrees)*M_PI / 180.0)
#define rad_to_deg(angleradians) ((angleradians)*180.0 / M_PI)

#include "err.h"
#include "format_obj.h"
#include "mem.h"

#define Array(x, t) \
    struct {        \
        t data[x];  \
    }

enum {
    WIN_X = 0,
    WIN_Y = 0,
    WIN_W = 600,
    WIN_H = 600,
    WIN_BORDER = 0,
};

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
} XGLEnvironment;

typedef struct {
    struct {
        double x;
        double y;
        double z;
    } pos;
    struct {
        double r;
        double g;
        double b;
    } color;
} Vertex;

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

static void main_loop(XGLEnvironment* env) {
    assert(env);
    XEvent event;
    // event loop
    bool shutdown = false;
    struct timeval t;
    Mesh mesh;
    bool ok = parse_obj_file("test.obj", &mesh);
    if (!ok) {
        log("parsing failed");
        exit(1); // FIXME
    }
    gettimeofday(&t, NULL);
    while (!shutdown) {
        struct timeval new_t;
        gettimeofday(&new_t, NULL);
        double secs = (double)(new_t.tv_usec - t.tv_usec) / 1000000 + (double)(new_t.tv_sec - t.tv_sec);
        t = new_t;
        if (secs > 0) {
            double fps = 1.0 / secs;
            //log("fps: %f, frametime: %f", fps, secs);
        } else {
            log("fps: < 1");
        }
        if (XPending(env->display)) {
            XNextEvent(env->display, &event);
            switch (event.type) {
            case KeyPress:
                // fallthrough
            case ClientMessage:
                shutdown = true;
                break;
            case Expose:
                break;
            }
        }
        XGetWindowAttributes(env->display, env->window, &env->gwa);
        glViewport(0, 0, env->gwa.width, env->gwa.height);

        static int f = 0;
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(50.0, (double)WIN_W / ((double)WIN_H), 0.01, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
        ++f;
        glRotated((f % 360) * 1., 0., 1., 0.);

        gl_draw_mesh(&mesh, env, GL_TRIANGLES);

        glXSwapBuffers(env->display, env->window);
    }
}

static void init(XGLEnvironment* env) {
    assert(env);
    env->display = XOpenDisplay(NULL);
    if (!env->display) {
        log("X: cannot open display");
        exit(1);
    }
    env->screen = DefaultScreen(env->display);
    env->root = XDefaultRootWindow(env->display);
    env->att = (GLint[]) { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    env->vi = glXChooseVisual(env->display, 0, env->att);
    if (!env->vi) {
        log("glX: no visual found");
        exit(1);
    }
    env->cmap = XCreateColormap(env->display, env->root, env->vi->visual, AllocNone);
    env->swa.colormap = env->cmap;
    env->swa.event_mask = ExposureMask | KeyPressMask;
    env->window = XCreateWindow(env->display, env->root,
        WIN_X, WIN_Y, WIN_W, WIN_H, WIN_BORDER,
        env->vi->depth, InputOutput, env->vi->visual,
        CWColormap | CWEventMask, &env->swa);

    log("x properties:");
    log("\tscreen dim (px)   : %dx%d", XDisplayWidth(env->display, env->screen), XDisplayHeight(env->display, env->screen));
    log("\tscreen dim (mm)   : %dmm x %dmm", XDisplayWidthMM(env->display, env->screen), XDisplayHeightMM(env->display, env->screen));
    log("\tdepth             : %d", XDefaultDepth(env->display, env->screen));
    log("\tdisplay           : %s", XDisplayString(env->display));
    log("\tprotocol version  : %d.%d", XProtocolVersion(env->display), XProtocolRevision(env->display));
    log("\tserver vendor     : %s", XServerVendor(env->display));
    log("gl properties:");
    log("\tselected visual   : %p", (void*)env->vi->visualid);

    // process window close event through event handler
    Atom del_window = XInternAtom(env->display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(env->display, env->window, &del_window, 1);

    // select which kinds of events we're interested in
    XSelectInput(env->display, env->window, ExposureMask | KeyPressMask);

    // display the window
    XMapWindow(env->display, env->window);
    XStoreName(env->display, env->window, "from_scratch");

    env->glc = glXCreateContext(env->display, env->vi, NULL, GL_TRUE);
    glXMakeCurrent(env->display, env->window, env->glc);
    glEnable(GL_DEPTH_TEST);
}

static void deinit(XGLEnvironment* env) {
    glXMakeCurrent(env->display, None, NULL);
    glXDestroyContext(env->display, env->glc);
    XDestroyWindow(env->display, env->window);
    XCloseDisplay(env->display);
}

char* g_filename = NULL;

int main(int argc, char* argv[argc]) {
    if (argc > 1) {
        g_filename = argv[1];
    }

    XGLEnvironment* env = allocate(sizeof(XGLEnvironment));
    log("initializing...");
    init(env);
    log("main loop...");
    main_loop(env);
    log("closing normally...");
    deinit(env);
    deallocate((void**)&env);
    return 0;
}
