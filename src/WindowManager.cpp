#include <iostream>
#include <stdlib.h>

#include "ConfigManager.h"
#include "WindowManager.h"
#include "PlotWindow.h"
#include "ImageWindow.h"

using namespace std;

WindowManager::WindowManager(const int dx, const int dy): 
    _size_x(dx), _size_y(dy), _color(1.0,1.0,1.0)
{
    // register the callbacks in the ConfigManager
    ConfigManager::I().addCmd("RemoveAllWindows",BIND_MEM_CB(&WindowManager::callbackRemoveAllWindows,this));
    ConfigManager::I().addCmd("AddPlotWindow",BIND_MEM_CB(&WindowManager::callbackAddPlotWindow,this));
    ConfigManager::I().addCmd("AddImageWindow",BIND_MEM_CB(&WindowManager::callbackAddImageWindow,this));    
    // prepare "no windows" texture
    _render.Text2Texture( _tex, "No Windows. Telnet to port 1337.");
}

string WindowManager::AddWindow(Window *win)
{    
    // check if name is unique
    for(size_t i=0; i<NumWindows(); i++) {
        if(_window_list[i]->Name() == win->Name()) {
            delete win;
            return "Window already exists.";
        }        
    }
    
    // init and add on success
    int ret = win->Init();
    if(ret==0) {
        _window_list.push_back(win);
        alignWindows();
        return ""; // success
    }
    else {
        // delete the window again if 
        // init was unsuccessful
        delete win;
        return "Window could not be initialized.";
    }
}

int WindowManager::RemoveWindow(const size_t n){
    if ( n >= NumWindows() ) return 1;
    delete _window_list.at(n);
    _window_list.erase(_window_list.begin() + n);
    alignWindows();
    return 0;
}

int WindowManager::RemoveWindow(const string &name)
{
    for(size_t i=0; i<NumWindows(); i++) {
        if(_window_list[i]->Name() == name) {
            RemoveWindow(i);
            return 0;
        }        
    }
    return 1;
}

void WindowManager::Draw(){
    
    float dy = 2. / _rows.size();
    float dx = 0;
    
    float wscaley = 1. / _rows.size();
    float wscalex = 1.;
    
    int i_window = 0;
    for ( size_t row = 0; row < _rows.size() ; ++row){
        dx = 2. / _rows.at(row);
        for ( int in_row = 0 ; in_row < _rows.at(row) ; ++in_row ){
            wscalex = 1. / _rows.at(row);
            glPushMatrix();
            glTranslatef(-1 + (dx / 2) + (in_row * dx ),1 - (dy / 2. ) - (row * dy ),0.);
            glScalef( wscalex , wscaley ,1);
            _window_list.at(i_window)->Draw();
            i_window++;
            glPopMatrix();
        }
    }
    
    if(_window_list.empty()) {
        _color.Activate();
        _tex.Activate();
        glPushMatrix();
        const float winratio = GetWindowWidth() / GetWindowHeight();
        const float totalratio = _tex.GetAspectRatio() / winratio;
        
        if( totalratio >= 1.0f )
            glScalef(1.0f,1.0f/totalratio,1.0f);
        else
            glScalef(1.0f*totalratio,1.0f,1.0f);
        
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
        Rectangle::unit.Draw( GL_TRIANGLE_FAN );
    
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glPopMatrix();
        
    }
    
}



string WindowManager::callbackRemoveAllWindows(const string &arg){
    int i;
    for ( i = NumWindows() ; i > 0 ; --i){
        RemoveWindow(0);
    }
    return ""; // removing is always successful :)
}

string WindowManager::callbackAddPlotWindow(const string &arg)
{
    return AddWindow(new PlotWindow(this, arg));
}

string WindowManager::callbackAddImageWindow(const string &arg)
{
    return AddWindow(new ImageWindow(this, arg));
}

void WindowManager::alignWindows(){
    _rows.clear();
    int row = -1;
    size_t i = 0;
    while ( i < NumWindows() ){
        if ( i < 2 ){
            _rows.push_back(1);
            i++;
        } else{
            row++;
            if ( row >= (int)_rows.size() && i < NumWindows() ){
                _rows.push_back(1);
                i++;
                while( _rows.at(row) < (int)(_rows.size() - 1) && ( i < NumWindows() ) ){
                    _rows.at(row)++;
                    i++;
                    row = -1;
                    break;
                }
            } else{
                while ( (_rows.at(row) < (int)_rows.size()) && (i < NumWindows()) ){
                    _rows.at(row)++;
                    i++;
                }
            }
        }
    }
    //cout << endl << "New tiling:" << endl;
    //for ( int r = 0 ; r < _rows.size() ; ++r ){
    //   cout << "  " <<_rows.at(r) << endl;
    //}
    //cout << endl;
    
    float wscaley = 1. / _rows.size();
    float wscalex = 1.;
    int i_window = 0;
    
    for ( size_t row = 0; row < _rows.size() ; ++row){
        for ( int in_row = 0 ; in_row < _rows.at(row) ; ++in_row ){
            wscalex = 1. / _rows.at(row);
            _window_list.at(i_window)->XPixels() = wscalex * GetWindowWidth();
            _window_list.at(i_window)->YPixels() = wscaley * GetWindowHeight();
            _window_list.at(i_window)->Update();
            i_window++;
        }
    }
}

std::ostream& operator<<( std::ostream& stream, const WindowManager& wman ) {
    stream << "Main Window Size: " << wman.SizeX() << " x " << wman.SizeY() << " ]";
    return stream;
}
