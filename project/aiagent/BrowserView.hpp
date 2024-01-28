// This code is a modification of the original "cefsimple" example that can be found at
// https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage

#ifndef BROWSERVIEW_HPP
#  define BROWSERVIEW_HPP

#  include <iostream>

// Chromium Embedded Framework
#include "include/cef_browser.h"
#include "include/cef_app.h"

#include "BrowserClient.hpp"
#include "LoadHandler.hpp"
#include "RenderHandler.hpp"
#include "JSV8Handler.hpp"
#include "DisplayHandler.hpp"
#include "LifeSpanHandler.hpp"
#include "json.hpp"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

using json = nlohmann::json;
// ****************************************************************************
//! \brief Interface class rendering a single web page.
// ****************************************************************************
class BrowserView:public CefApp,
		  public CefBrowserProcessHandler,
		  public CefRenderProcessHandler
{
public:

    //! \brief Default Constructor using a given URL.
    BrowserView(float scale);

    //! \brief
    ~BrowserView();

    //! \brief Load the given web page.
    void load(const std::string &url);

    //! \brief Render the web page.
    void draw();

    //! \brief Set the windows size.
    void reshape(int w, int h);

    //! \brief Set the viewport: the rectangle on the window where to display
    //! the web document.
    //! \return false if arguments are incorrect.
    bool viewport(float x, float y, float w, float h);

    //! \brief Get the viewport.
    inline glm::vec4 const& viewport() const
    {
        return m_viewport;
    }

    // CefApp methods:
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
	return this;
    };
     
    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() override;

    CefRefPtr<CefClient> GetDefaultClient() override {
        return m_client;
    }
    // CefRenderProcessHandler methods:
    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
			  CefRefPtr<CefFrame> frame,
			  CefRefPtr<CefV8Context> context) override;

    virtual void OnWebKitInitialized();

    virtual bool OnProcessMessageReceived( CefRefPtr< CefBrowser > browser,
					   CefRefPtr< CefFrame > frame,
					   CefProcessId source_process,
					   CefRefPtr< CefProcessMessage > message ) override;
    void SengUserMessage(json& msg);
    void UserCompletionCallback(CefRefPtr<CefBrowser> browser, const json& msg);
private:

    //! \brief Where to draw on the OpenGL window
    glm::vec4 m_viewport;
    float m_scale;

    //! \brief Chromium Embedded framework elements
    CefRefPtr<CefBrowser> m_browser;
    CefRefPtr<BrowserClient> m_client;
    CefRefPtr<RenderHandler> m_render_handler = nullptr;
    CefRefPtr<LoadHandler> m_load_handler = nullptr;
    CefRefPtr<DisplayHandler> m_display_handler = nullptr;
    CefRefPtr<LifeSpanHandler> m_lifespan_handler = nullptr;
    CefRefPtr<JSV8Handler> m_jsHandler = nullptr;

    //! \brief OpenGL has created GPU elements with success
    bool m_initialized = false;

    std::string m_extensionCode;
    CefRefPtr<OpenAI> m_openai=nullptr;

public:

    //! \brief If set to false then the web page is turning.
   IMPLEMENT_REFCOUNTING(BrowserView);
};

#endif // BROWSERVIEW_HPP
