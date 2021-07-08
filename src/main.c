#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

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

#include "camera.h"
#include "err.h"
#include "format_obj.h"
#include "io.h"
#include "mem.h"
#include "ogl.h"
#include "res.h"

enum {
    WIN_X = 0,
    WIN_Y = 0,
    WIN_W = 600,
    WIN_H = 600,
    WIN_BORDER = 0,
};

// returns whether it has handled the event (false = ignored)
static bool handle_key(XEvent* event, bool* shutdown, Camera* camera) {
    assert(event);
    assert(shutdown);
    switch (XLookupKeysym(&event->xkey, 0)) {
    case XK_Escape:
        plog("exiting via escape-key");
        *shutdown = true;
        break;
    case XK_Up:
        plog("UP!");
        move_camera(camera, &camera->forward, 1.0);
        break;
    case XK_Down:
        plog("DOWN!");
        move_camera(camera, &camera->forward, -1.0);
        break;
    default:
        // not handled
        return false;
    }
    plog_camera(camera);
    return true;
}

static void main_loop(XGLEnvironment* env) {
    assert(env);
    XEvent event;
    // event loop
    bool shutdown = false;
    Mesh mesh;
    bool ok = parse_obj_file("data/test2.obj", &mesh);
    if (!ok) {
        plog("parsing failed");
        exit(1); // FIXME
    }
    Camera* cam = new_camera();
    cam->pos = (Vec3) { 0.0, -5.0, 0.0 };
    //cam->forward = (Vec3) { 1.0, 0.0, 0.0 };
    Vec3 origin = { 0.0, 0.0, 0.0 };
    camera_look_at(cam, &origin);
    plog_camera(cam);
    while (!shutdown) {
        if (XPending(env->display)) {
            XNextEvent(env->display, &event);
            switch (event.type) {
            case KeyPress: {
                bool handled = handle_key(&event, &shutdown, cam);
                (void)handled;
                break;
            }
            case ClientMessage:
                shutdown = true;
                break;
            case Expose:
                break;
            default:
                plog("unhandled case: 0x04%x", event.type);
            }
        }
        XGetWindowAttributes(env->display, env->window, &env->gwa);
        glViewport(0, 0, env->gwa.width, env->gwa.height);

        static double f = 0;
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        GLdouble fovy = 50.0;
        const GLdouble clip_near = 0.01;
        const GLdouble clip_far = 100.0;
        gluPerspective(fovy, (double)env->gwa.width / ((double)env->gwa.height), clip_near, clip_far);
        double rotation_speed = 0.4;
        // TODO: rotate camera
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(cam->forward.x, 1, 0, 0);
        glRotatef(cam->forward.z, 0, 1, 0);
        glRotatef(cam->forward.y, 0, 0, 1);
        glTranslatef(cam->pos.x,
            cam->pos.z,
            cam->pos.y);
        // glRotated((((int)round(f)) % 360) * 1.0, 0., 1., 0.);

        gl_draw_mesh(&mesh, env, GL_TRIANGLES);

        glXSwapBuffers(env->display, env->window);
    }
    deallocate_mesh(&mesh);
    free_camera(&cam);
}

static void init(XGLEnvironment* env) {
    assert(env);
    env->display = XOpenDisplay(NULL);
    if (!env->display) {
        plog("X: cannot open display");
        exit(1);
    }
    env->screen = DefaultScreen(env->display);
    env->root = XDefaultRootWindow(env->display);
    env->att = (GLint[]) { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    env->vi = glXChooseVisual(env->display, 0, env->att);
    if (!env->vi) {
        plog("glX: no visual found");
        exit(1);
    }
    env->cmap = XCreateColormap(env->display, env->root, env->vi->visual, AllocNone);
    env->swa.colormap = env->cmap;
    env->swa.event_mask = ExposureMask | KeyPressMask;
    env->window = XCreateWindow(env->display, env->root,
        WIN_X, WIN_Y, WIN_W, WIN_H, WIN_BORDER,
        env->vi->depth, InputOutput, env->vi->visual,
        CWColormap | CWEventMask, &env->swa);

    plog("x properties:");
    plog("\tscreen dim (px)   : %dx%d", XDisplayWidth(env->display, env->screen), XDisplayHeight(env->display, env->screen));
    plog("\tscreen dim (mm)   : %dmm x %dmm", XDisplayWidthMM(env->display, env->screen), XDisplayHeightMM(env->display, env->screen));
    plog("\tdepth             : %d", XDefaultDepth(env->display, env->screen));
    plog("\tdisplay           : %s", XDisplayString(env->display));
    plog("\tprotocol version  : %d.%d", XProtocolVersion(env->display), XProtocolRevision(env->display));
    plog("\tserver vendor     : %s", XServerVendor(env->display));
    plog("gl properties:");
    plog("\tselected visual   : %p", (void*)env->vi->visualid);

    // process window close event through event handler
    Atom del_window = XInternAtom(env->display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(env->display, env->window, &del_window, 1);

    // select which kinds of events we're interested in
    XSelectInput(env->display, env->window, ExposureMask | KeyPressMask);

    // display the window
    XMapWindow(env->display, env->window);
    XStoreName(env->display, env->window, "From Scratch!");

    env->glc = glXCreateContext(env->display, env->vi, NULL, GL_TRUE);
    glXMakeCurrent(env->display, env->window, env->glc);
    glEnable(GL_DEPTH_TEST);

    XGetWindowAttributes(env->display, env->window, &env->gwa);
    env->width = env->gwa.width;
    env->height = env->gwa.height;
}

static void deinit(XGLEnvironment* env) {
    XFree(env->vi);
    glXMakeCurrent(env->display, None, NULL);
    glXDestroyContext(env->display, env->glc);
    XDestroyWindow(env->display, env->window);
    XCloseDisplay(env->display);
}

int main(int argc, char** argv) {
    if (argc != 1) {
        plog("%s takes no arguments.\n", argv[0]);
        return 1;
    }
    set_resource_folder("data");
    init_resource_manager();
    XGLEnvironment* env = allocate(sizeof(XGLEnvironment));
    plog("initializing...");
    init(env);
    plog("main loop...");
    main_loop(env);
    plog("closing normally...");
    deinit(env);
    deallocate((void**)&env);
    deinit_resource_manager();
    return 0;
}
