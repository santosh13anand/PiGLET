#ifndef GLUT_H
#define GLUT_H

#include <GL/glut.h>
#include "GLApp.h"

#define DEFAULT_WINDOW_WIDTH    1024
#define DEFAULT_WINDOW_HEIGHT   1024/1.6

void InitGL();
void RunGL( GLApp& app );

int GetWindowWidth();
int GetWindowHeight();

void ReportGLError();

#endif