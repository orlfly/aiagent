#include "RenderHandler.hpp"
#include "GLCore.hpp"

//------------------------------------------------------------------------------
RenderHandler::RenderHandler(glm::vec4 const& viewport, float scale)
  : m_viewport(viewport), m_scale(scale)
{}

//------------------------------------------------------------------------------
RenderHandler::~RenderHandler()
{
    // Free GPU memory
    GLCore::deleteProgram(m_prog);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

//------------------------------------------------------------------------------
bool RenderHandler::init()
{
    // Dummy texture data
    const unsigned char data[] = {
        255, 0, 0, 255,
        0, 255, 0, 255,
        0, 0, 255, 255,
        255, 255, 255, 255,
    };

    // Compile vertex and fragment shaders
    m_prog = GLCore::createShaderProgram("shaders/tex.vert", "shaders/tex.frag");
    if (m_prog == 0)
    {
        std::cerr << "shader compile failed" << std::endl;
        return false;
    }

    // Get locations of shader variables (attributes and uniforms)
    m_pos_loc = GLCHECK(glGetAttribLocation(m_prog, "position"));
    m_tex_loc = GLCHECK(glGetUniformLocation(m_prog, "tex"));
    m_mvp_loc = GLCHECK(glGetUniformLocation(m_prog, "mvp"))

    // Square vertices (texture positions are computed directly inside the shader)
    float coords[] = {-1.0,-1.0,-1.0,1.0,1.0,-1.0,1.0,-1.0,-1.0,1.0,1.0,1.0};

    // See https://learnopengl.com/Getting-started/Textures
    GLCHECK(glGenVertexArrays(1, &m_vao));
    GLCHECK(glBindVertexArray(m_vao));
    GLCHECK(glGenBuffers(1, &m_vbo));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW));
    GLCHECK(glEnableVertexAttribArray(m_pos_loc));
    GLCHECK(glVertexAttribPointer(m_pos_loc, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GLCHECK(glGenTextures(1, &m_tex));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, m_tex));
    GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));

    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCHECK(glBindVertexArray(0));

    return true;
}

//------------------------------------------------------------------------------
void RenderHandler::draw(glm::vec4 const& viewport)
{
    // Where to paint on the OpenGL window
    GLCHECK(glViewport(viewport[0],
                       viewport[1],
                       GLsizei(viewport[2] * m_width),
                       GLsizei(viewport[3] * m_height)));

    // Apply a rotation
    glm::mat4 trans = glm::mat4(1.0f); // Identity matrix
    trans = glm::translate(trans, glm::vec3(-1.0f+1.0f/m_scale, -1.0f+1.0f/m_scale, 0.0f));
    trans = glm::scale(trans, glm::vec3(1.0f/m_scale, 1.0f/m_scale, 0.0f));
    // See https://learnopengl.com/Getting-started/Textures
    GLCHECK(glUseProgram(m_prog));
    GLCHECK(glBindVertexArray(m_vao));

    GLCHECK(glUniformMatrix4fv(m_mvp_loc, 1, GL_FALSE, glm::value_ptr(trans)));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, m_tex));
    GLCHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    GLCHECK(glBindVertexArray(0));
    GLCHECK(glUseProgram(0));
}

//------------------------------------------------------------------------------
void RenderHandler::reshape(int w, int h)
{
    m_width = w;
    m_height = h;
}

//------------------------------------------------------------------------------
void RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    rect = CefRect(m_viewport[0], m_viewport[1], m_viewport[2] * m_width, m_viewport[3] * m_height);
}

//------------------------------------------------------------------------------
void RenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                                         const RectList &dirtyRects, const void *buffer,
                                         int width, int height)
{
    //std::cout << "RenderHandler::OnPaint" << std::endl;
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, m_tex));
    GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT,
                         GL_UNSIGNED_BYTE, (unsigned char*)buffer));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
}
