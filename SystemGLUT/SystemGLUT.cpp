#include "SystemGLUT.h"

#include <system.h>
#include <iostream>
#include "stdlib.h"

#include "../ConfigManager.h"

using namespace std;



static GLApp* currentInstance;
static bool fullscreen = false;
static int _width =  DEFAULT_WINDOW_WIDTH;
static int _height = DEFAULT_WINDOW_HEIGHT;


void Reshape( int w, int h ) {
    _width = w;
    _height = h;
}

int GetWindowWidth() {
    return _width;
}

int GetWindowHeight() {
    return _height;
}

static void drawCallback() {
    ConfigManager::I().MutexLock();
    currentInstance->Draw();
    glFlush();
    glFinish();
    glutSwapBuffers();
    ConfigManager::I().MutexUnlock();
}

void toggleFullscreen() {
    if(fullscreen) {
        glutPositionWindow(0,0);
        fullscreen=false;
    } else {
        glutFullScreen();
        fullscreen = true;
    }
}

void keyPressed (unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        exit(0);
        break;
    case 'f':
        toggleFullscreen();
        break;
    default:
        break;
    }
}

void InitGL(){
    cout << "GLUT Init" << endl;
    int iArgc = 0;
    glutInit(&iArgc, NULL);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL );
    glutInitWindowSize(_width, _height);

    glutInitWindowPosition(0, 0);
    glutCreateWindow("PiGLET");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW); // use for the following the much bigger Modelview stack
    glutIdleFunc(drawCallback);
    glutKeyboardFunc(keyPressed);
//    glutReshapeFunc(Reshape);
}

void RunGL(GLApp &app) {
    currentInstance = &app;
    app.Init();
    cout << "Press ESC to quit" << endl;
    glutMainLoop();
}

void ReportGLError() {
    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        cerr << "OpenGL Error (fix that!): " << gluErrorString(err) << endl;        
    }
    
}
