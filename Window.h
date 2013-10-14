#ifndef WINDOW_H
#define WINDOW_H

#include <ostream>
#include <string>

#include "Structs.h"
#include "TextRenderer.h"
#include "BlockBuffer.h"
#include "Widget.h"

class Window {
private:
    float _x_scale;
    float _y_scale;
    std::string _title;

public:

    Window( const std::string& title, const float xscale = 1, const float yscale = 1): _title(title) , _x_scale(xscale), _y_scale(yscale) {}

    const float& XScale() const { return _x_scale; }
    const float& YScale() const { return _y_scale; }
    float& XScale() { return _x_scale; }
    float& YScale() { return _y_scale; }

    virtual void Draw() = 0;

};

class PlotWindow: public Window{
private:
    std::string _xlabel;
    std::string _ylabel;

    // Window Frame:
    Rectangle rect;

    SimpleGraph graph;

    TextLabel text;

    int frame;          //for debug
    NumberLabel num;    //for debug

public:
    const std::string& Xlabel() const { return _xlabel; }
    const std::string& Ylabel() const { return _ylabel; }
    std::string& Xlabel() { return _xlabel; }
    std::string& Ylabel() { return _ylabel; }

    PlotWindow( const std::string& title,
                const std::string& xlabel = "Always label your axes",
                const std::string& ylabel = "Alawys label your axes",
                const float xscale = 1,
                const float yscale = 1);


    virtual void Draw();
};

std::ostream& operator<<( std::ostream& stream, const Window& win );
std::ostream& operator<<( std::ostream& stream, const PlotWindow& win );

#endif // INTERVAL_H
