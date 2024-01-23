// This code is a modification of the original project that can be found at
// https://github.com/Lecrapouille/OpenGLCppWrapper

#include "GLWindow.hpp"
#include "WebStream.hpp"
#include <iostream>
#include <cassert>
#include "include/cef_command_line.h"

static void error_callback(int error, const char* description)
{
    std::cerr << error << ": " << description << std::endl;
}

GLWindow::GLWindow(uint32_t const width, uint32_t const height, const char *title)
  : m_width(width), m_height(height), m_title(title)
{}

void GLWindow::init(CefMainArgs args,std::string path)
{
    // Initialize glfw3
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        std::cerr << "glfwInit: failed" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    CefRefPtr<BrowserView> browser = createBrowser(path);
    
    CefExecuteProcess(args, browser , nullptr);
    
    m_window = glfwCreateWindow(static_cast<int>(m_width),
                                static_cast<int>(m_height),
                                m_title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "glfwCreateWindow: failed" << std::endl;
        glfwTerminate();
        exit(1);
    }
    
    glfwMakeContextCurrent(m_window);

    
    // Initialize GLEW
    glewExperimental = GL_TRUE; // stops glew crashing on OSX :-/
    if (GLEW_OK != glewInit())
    {
        std::cerr << "glewInit: failed" << std::endl;
        exit(1);
    }
    // create framebuffer object
    
    // Configurate Chromium
    CefSettings settings;
    settings.windowless_rendering_enabled = true;
    
    settings.no_sandbox = false;

    
    bool result = CefInitialize(args, settings, browser, nullptr);
    if (!result)
    {
        std::cerr << "CefInitialize: failed" << std::endl;
        exit(-2);
    }
    

    
    glGenFramebuffersEXT(1, &m_fboId);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);

    // create renderbuffer object
    glGenRenderbuffersEXT(1, &m_renderbufferId);

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_renderbufferId);

    glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_RGBA, m_width, m_height );


    // attach renderbuffer to framebuffer at 'color attachment point'
    glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_renderbufferId );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, m_width, m_height);

    WebStream::GetInstance()->Initialize("rtsp://localhost:8554/aiagent", 25, m_width, m_height);
    
    // Print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // Make sure OpenGL version 3.2 API is available
    if (!GLEW_VERSION_3_2)
    {
        std::cerr << "OpenGL 3.2 API is not available!" << std::endl;
    }
}

GLWindow::~GLWindow()
{
    if (nullptr != m_window)
        glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool GLWindow::start()
{
    while (!glfwWindowShouldClose(m_window))
    {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);

        if (!update())
            return false;

        GLubyte pixels[m_width*m_height*4];
        uint8_t rgb[m_width*m_height*4];

        glReadPixels(0, 0, m_width, m_height, GL_BGRA, GL_UNSIGNED_BYTE, pixels);


        unsigned int cur_gl, cur_rgb;
        for( unsigned int i = 0; i < m_height; i++) {
            for (unsigned int j = 0; j < m_width; j++) {
                cur_gl  = 4 * (m_width * (m_height - i - 1) + j);
                cur_rgb = 4 * (m_width * i + j);
                for (unsigned int k = 0; k < 4; k++)
                    rgb[cur_rgb + k] = pixels[cur_gl + k];
            }
        }
        WebStream::GetInstance()->Write(rgb);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
    }

    return true;
}
