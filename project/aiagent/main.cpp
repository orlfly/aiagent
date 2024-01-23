#include "CEFGLWindow.hpp"

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>

using std::filesystem::current_path;

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{   
    CefMainArgs args(argc, argv);
    
    CEFGLWindow win(960, 540, 2, "CEF OpenGL");

    std::stringstream fpss;

    fpss << get_current_dir_name() << "/";
    
    win.init(args,fpss.str());

    if (!win.setup()){
        std::cerr << "setup: failed" << std::endl;
        exit(-2);
    }
    int res = win.start();

    return res;
}
