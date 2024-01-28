// This code is a modification of the original project that can be found at
// https://github.com/if1live/cef-gl-example

#include "CEFGLWindow.hpp"
#include "GLCore.hpp"
#include <string>

//------------------------------------------------------------------------------
CEFGLWindow::CEFGLWindow(uint32_t const width, uint32_t const height, float const scale, const char *title)
    : GLWindow(width, height, title), m_scale(scale)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

//------------------------------------------------------------------------------
CEFGLWindow::~CEFGLWindow()
{
    m_server->StopServer(ServerHandler::CompleteCallback());
    CefShutdown();
}

//------------------------------------------------------------------------------
CefRefPtr<BrowserView> CEFGLWindow::createBrowser()
{
  CefRefPtr<BrowserView> app = new BrowserView(m_scale);
    m_browser=app;
    return app;
}


//------------------------------------------------------------------------------
bool CEFGLWindow::setup()
{
    m_browser->reshape(static_cast<uint32_t>(m_width*m_scale), static_cast<uint32_t>(m_height*m_scale));

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

    m_server = new ServerHandler(m_browser);

    // Start the server. OnComplete will be executed upon completion.
    m_server->StartServer(9191,
                          ServerHandler::CompleteCallback());
    
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
