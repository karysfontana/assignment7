#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "View.h"
#include "Model.h"

class Controller
{
public:
    Controller(string file, Model& m,View& v);
    ~Controller();
    void run();
private:
    void initScenegraph();

    Model& model;
    View& view;
    string file;
};

#endif