#ifndef RENDERHANDLER_HPP
#define RENDERHANDLER_HPP

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Matrices manipulation for OpenGL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>

#include <include/cef_render_handler.h>

// *************************************************************************
//! \brief Private implementation to handle CEF events to draw the web page.
// *************************************************************************
class RenderHandler: public CefRenderHandler
{
public:

  RenderHandler(glm::vec4 const& viewport);

  //! \brief
  ~RenderHandler();

  //! \brief Compile OpenGL shaders and create OpenGL objects (VAO,
  //! VBO, texture, locations ...)
  bool init();

  //! \brief Render OpenGL VAO (rotating a textured square)
  void draw(glm::vec4 const& viewport);

  //! \brief Resize the view
  void reshape(int w, int h);

  //! \brief Return the OpenGL texture handle
  GLuint texture() const
  {
    return m_tex;
  }

  //! \brief CefRenderHandler interface
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;

  //! \brief CefRenderHandler interface
  //! Update the OpenGL texture.
  virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
		       const RectList &dirtyRects, const void *buffer,
		       int width, int height) override;

  //! \brief CefBase interface
  IMPLEMENT_REFCOUNTING(RenderHandler);

private:

  //! \brief Dimension
  int m_width;
  int m_height;

  //! \brief Where to draw on the OpenGL window
  glm::vec4 const& m_viewport;

  //! \brief OpenGL shader program handle
  GLuint m_prog = 0;
  //! \brief OpenGL texture handle
  GLuint m_tex = 0;
  //! \brief OpenGL vertex array object handle
  GLuint m_vao = 0;
  //! \brief OpenGL vertex buffer obejct handle
  GLuint m_vbo = 0;

  //! \brief OpenGL shader variable locations for vertices of the
  //! rectangle
  GLint m_pos_loc = -1;
  //! \brief OpenGL shader variable locations for the texture
  GLint m_tex_loc = -1;
  //! \brief OpenGL shader variable locations for the Model View
  //! Projection matrix.
  GLint m_mvp_loc = -1;
};
#endif // RENDERHANDLER_HPP
