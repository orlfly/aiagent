// This code is a modification of the original project that can be found at
// https://github.com/Lecrapouille/OpenGLCppWrapper

#ifndef CEFGLWINDOW_HPP
#  define CEFGLWINDOW_HPP

// Base application class
#  include "GLWindow.hpp"
#  include "BrowserView.hpp"

// ****************************************************************************
//! \brief Extend the OpenGL base window and add Chromium Embedded Framework
//! browser views.
// ****************************************************************************
class CEFGLWindow: public GLWindow
{
public:

    //! \brief Default construction: define the window dimension and title
    CEFGLWindow(uint32_t const width, uint32_t const height, const char *title);

    //! \brief Destructor
    ~CEFGLWindow();

    CefRefPtr<BrowserView> createBrowser(std::string path);

    virtual bool setup() override;

private: // Concrete implementation from GLWindow
    virtual bool update() override;

private:

    //! \brief List of BrowserView managed by createBrowser() and
    CefRefPtr<BrowserView> m_browser;
};

#endif // CEFGLWINDOW_HPP
