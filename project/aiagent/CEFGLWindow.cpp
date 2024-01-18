// This code is a modification of the original project that can be found at
// https://github.com/if1live/cef-gl-example

#include "CEFGLWindow.hpp"
#include "GLCore.hpp"
#include <string>

//------------------------------------------------------------------------------
CEFGLWindow::CEFGLWindow(uint32_t const width, uint32_t const height, const char *title)
    : GLWindow(width, height, title)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

//------------------------------------------------------------------------------
CEFGLWindow::~CEFGLWindow()
{
    CefShutdown();
}

//------------------------------------------------------------------------------
CefRefPtr<BrowserView> CEFGLWindow::createBrowser(std::string path)
{
    CefRefPtr<BrowserView> app = new BrowserView(path);
    m_browser=app;
    return app;
}


//------------------------------------------------------------------------------
bool CEFGLWindow::setup()
{
    
    m_browser->reshape(m_width, m_height);

    // Change viewports (vertical split)
    m_browser->viewport(0.0f, 0.0f, 1.0f, 1.0f);

    // Set OpenGL states
    //GLCHECK(glViewport(0, 0, m_width, m_height));
    GLCHECK(glClearColor(0.0, 0.0, 0.0, 0.0));
    //GLCHECK(glEnable(GL_CULL_FACE));
    GLCHECK(glEnable(GL_DEPTH_TEST));
    GLCHECK(glDepthFunc(GL_LESS));
    GLCHECK(glDisable(GL_BLEND));
    GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    return true;
}

//------------------------------------------------------------------------------
bool CEFGLWindow::update()
{
    GLCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    m_browser->draw();

    CefDoMessageLoopWork();
    return true;
}
