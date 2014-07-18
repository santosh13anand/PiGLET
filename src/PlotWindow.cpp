#include <iostream>
#include <sstream>
#include <string.h> // for strcmp
#include <cmath>
#include "Callback.h"
#include "PlotWindow.h"
#include "ConfigManager.h"

using namespace std;

PlotWindow::PlotWindow( 
        WindowManager* owner, 
        const std::string& pvname,
        const std::string& xlabel,
        const std::string& ylabel,
        const float xscale,
        const float yscale ) :
    Window(owner, pvname,xscale,yscale),
    _pvname(pvname),
    _xlabel(xlabel),
    _ylabel(ylabel),    
    _initialized(false),
    WindowArea( dBackColor, dWindowBorderColor),
    graph(this, DEFAULT_BACKLEN),
    text(this, -0.98, .66, .99, .98),
    frame(0),
    _old_properties(),
    _epics_connected(false),
    discon_lbl(this, -.1, -.1, .8, .9)
{
    //cout << "Plotwindow ctor" << endl;
    text.SetText(pvname);
    discon_lbl.SetColor(kPink);
    discon_lbl.SetText("Disconnected");
    
    // don't forget to call Init()

}

int PlotWindow::Init()
{
    if(_pvname.empty())
        return 1;
    
    ConfigManager::I().addCmd(Name()+"_BackLength", BIND_MEM_CB(&PlotWindow::callbackSetBackLength, this));    
    
    int ret = Window::Init();
    // the provided cb is triggered via processNewDataForPV    
    Epics::I().addPV(_pvname, BIND_MEM_CB(&PlotWindow::ProcessEpicsData, this));     
    // return & save status for dtor    
    _initialized = ret == 0;
    return ret;
}

PlotWindow::~PlotWindow() {
    if(_initialized) {
        Epics::I().removePV(_pvname);      
    }
    ConfigManager::I().removeCmd(_pvname+"_BackLength");
    //cout << "Plotwindow dtor" << endl;
} 

string PlotWindow::callbackSetBackLength(const string& arg){
    graph.SetBackLength(atoi(arg.c_str()));
    return ""; // success
}

void PlotWindow::Draw() {
    
    graph.SetNow(Epics::I().GetCurrentTime());
    Epics::I().processNewDataForPV(_pvname);   
    
   
    // Window border
    WindowArea.Draw();
    graph.Draw();
    text.Draw();

    if( !_epics_connected ) {
        discon_lbl.Draw();
    }
     
    ++frame;
}

void PlotWindow::ProcessEpicsData(const Epics::DataItem* i) {

    // if new, process it!
    switch (i->type) {
    case Epics::Connected:
        _epics_connected = true;        
        graph.enable_lastline = true;
        break;
        
    case Epics::Disconnected:
        // if we were connected before
        // start a new block
        if( _epics_connected ) {
            graph.NewBlock();
        }
        _epics_connected = false;
        graph.enable_lastline = false;
        break;
        
    case Epics::NewValue: {
        vec2_t* d = (vec2_t*)i->data;
        graph.AddToBlockList(*d);     
        break;               
    }
    case Epics::NewProperties: {
        ProcessEpicsProperties((dbr_ctrl_double*)i->data);
        break;
    }
    }        
    
}

void PlotWindow::ProcessEpicsProperties(dbr_ctrl_double* d) {
   
    // we use this macro only in this function 
    // to prevent typos in the field names   
    #define CHANGED(field) ((d->field) != (_old_properties.field))  
    
    // set alarm/warnings levels
    if(CHANGED(lower_alarm_limit) || CHANGED(upper_alarm_limit)) 
        graph.SetMajorAlarms(Interval(d->lower_alarm_limit, d->upper_alarm_limit));
    
    if(CHANGED(lower_warning_limit) || CHANGED(upper_warning_limit))
        graph.SetMinorAlarms(Interval(d->lower_warning_limit, d->upper_warning_limit));
        
    // set alarm state
    if(CHANGED(severity))
        graph.SetAlarm((epicsAlarmSeverity)d->severity);
    
    // display limits a.k.a yrange
    if(CHANGED(lower_disp_limit) || CHANGED(upper_disp_limit)) {
        Interval y(d->lower_disp_limit,d->upper_disp_limit);
        // if the provided interval is empty,
        // try guessing some better one
        if(y.Length()==0) {
            graph.SetAutoRange(true);
        } else {
            graph.SetAutoRange(false);
            graph.SetYRange( y );
        }
    }
    
    // set precision
    // only positive numbers are allowed
    if(CHANGED(precision) && d->precision>0) {
        graph.SetPrecision(d->precision);
    }
    
    // append unit to title
    // d->units is a char array, 
    // so the simple CHANGED can't be used... 
    if(strcmp(d->units,_old_properties.units) != 0) {
        // update title with unit
        stringstream title;
        string u(d->units);
        title << _pvname;
        if(!u.empty()) {
            title << " / " << u;
        }
        text.SetText(title.str());
    }    

    // save a copy of the old state
    _old_properties = *d;
    
    #undef CHANGED
}

    


void PlotWindow::SetYRange(const float min, const float max)
{
    graph.SetYRange( Interval(min, max ));
}

void PlotWindow::Update() 
{ 
    graph.UpdateTicks(); 
}




std::ostream& operator<<( std::ostream& stream, const PlotWindow& win ) {
    stream << "[ " << win.XPixels() << " x " << win.YPixels() << " ]: " << win.Xlabel() << " vs. " << win.Ylabel();
    return stream;
}
