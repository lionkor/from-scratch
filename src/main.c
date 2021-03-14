#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "err.h"

enum {
    RECT_X = 20,
    RECT_Y = 20,
    RECT_W = 10,
    RECT_H = 10,

    WIN_X = 0,
    WIN_Y = 0,
    WIN_W = 600,
    WIN_H = 600,
    WIN_BORDER = 0,
};

void DrawAQuad() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1., 1., -1., 1., 1., 20.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

    glBegin(GL_QUADS);
    {
        glColor3f(1., 0., 0.);
        glVertex3f(-.75, -.75, 0.);
        glColor3f(0., 1., 0.);
        glVertex3f(.75, -.75, 0.);
        glColor3f(0., 0., 1.);
        glVertex3f(.75, .75, 0.);
        glColor3f(1., 1., 0.);
        glVertex3f(-.75, .75, 0.);
    }
    glEnd();
}

int main(int argc, char** argv) {
    if (argc != 1) {
        log("%s takes no arguments.\n", argv[0]);
        return 1;
    }
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        log("X: cannot open display");
        return 1;
    }
    int screen = DefaultScreen(display);
    Window root = XDefaultRootWindow(display);
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    XVisualInfo* vi = glXChooseVisual(display, 0, att);
    if (!vi) {
        log("glX: no visual found");
        return 1;
    }
    Colormap cmap = XCreateColormap(display, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;
    Window window = XCreateWindow(display, root,
        WIN_X, WIN_Y, WIN_W, WIN_H, WIN_BORDER,
        vi->depth, InputOutput, vi->visual,
        CWColormap | CWEventMask, &swa);

    log("x properties:");
    log("\tscreen dim (px)   : %dx%d", XDisplayWidth(display, screen), XDisplayHeight(display, screen));
    log("\tscreen dim (mm)   : %dmm x %dmm", XDisplayWidthMM(display, screen), XDisplayHeightMM(display, screen));
    log("\tdepth             : %d", XDefaultDepth(display, screen));
    log("\tdisplay           : %s", XDisplayString(display));
    log("\tprotocol version  : %d.%d", XProtocolVersion(display), XProtocolRevision(display));
    log("\tserver vendor     : %s", XServerVendor(display));
    log("gl properties:");
    log("\tselected visual   : %p", (void*)vi->visualid);

    // process window close event through event handler
    Atom del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &del_window, 1);

    // select which kinds of events we're interested in
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    // display the window
    XMapWindow(display, window);
    XStoreName(display, window, argv[0]);

    GLXContext glc = glXCreateContext(display, vi, NULL, GL_TRUE);
    glXMakeCurrent(display, window, glc);
    glEnable(GL_DEPTH_TEST);

    XEvent event;
    // event loop
    while (true) {
        XNextEvent(display, &event);
        switch (event.type) {
        case KeyPress:
            // fallthrough
        case ClientMessage:
            goto breakout;
        case Expose: {
            XWindowAttributes gwa;
            XGetWindowAttributes(display, window, &gwa);
            glViewport(0, 0, gwa.width, gwa.height);
            DrawAQuad();
            glXSwapBuffers(display, window);
            /*
            // draw the window
            XFillRectangle(display, window, DefaultGC(display, screen),
                RECT_X, RECT_Y, RECT_W, RECT_H);
                */
            break;
        }
        }
    }
breakout:
    log("closing normally...");
    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, glc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
