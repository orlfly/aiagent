#include "CEFGLWindow.hpp"

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>


//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{   
    CefMainArgs args(argc, argv);
    
    CEFGLWindow win(960, 540, 2, "CEF OpenGL");

    win.init(args);

    if (!win.setup()){
        std::cerr << "setup: failed" << std::endl;
        exit(-2);
    }
    int res = win.start();

    return res;
}
