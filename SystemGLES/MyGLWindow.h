/*
  Copyright (C) 2012 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received m_a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MYGLWINDOW_H__
#define MYGLWINDOW_H__

#include "EGLWindow.h"

#include <vector>
#include "BlockBuffer.h"
#include "Structs.h"
#include <time.h>

#include "../GLApp.h"

typedef struct timespec timespec;


/// @brief this class create our window by inheriting the features of the EGL Window
class MyGLWindow : public EGLWindow
{
	public :
		/// @brief ctor
		/// @param[in] _config an optional configuration for the buffers etc
		MyGLWindow(EGLconfig *_config=0);
		/// @brief dtor will close down the vc and re-set EGL
		virtual ~MyGLWindow();
		/// @brief the is the main drawing function should only be called once initalizeGL has
		/// been called
		virtual void paintGL();

        GLApp* app;

	protected :
		/// @brief one time OpenGL initialisation
		virtual void initializeGL();

        static const vec2_t square[4];

        PiGLPlot::BlockList graph;
        float phase;
        unsigned int frames;
        timespec start_fps;
};



#endif