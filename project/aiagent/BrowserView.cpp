// This code is a modification of the original projects that can be found at
// https://github.com/if1live/cef-gl-example
// https://github.com/andmcgregor/cefgui

#include "BrowserView.hpp"
#include "GLCore.hpp"
#include "include/wrapper/cef_helpers.h"
#include <fstream>

std::string FileToString(std::string wp, std::string path) {
    std::ifstream inputFile(wp+path);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
        return ""; // Exit with an error code
    }
    
    std::ostringstream ss;
    std::string fileContent;
    std::string line;

    while (std::getline(inputFile, line)) {
        ss << line << "\n";
    }

    std::string file_contents = ss.str();
    inputFile.close();

    return file_contents;
}


bool BrowserView::viewport(float x, float y, float w, float h)
{
    if (!(x >= 0.0f) && (x < 1.0f))
        return false;

    if (!(x >= 0.0f) && (y < 1.0f))
        return false;

    if (!(w > 0.0f) && (w <= 1.0f))
        return false;

    if (!(h > 0.0f) && (h <= 1.0f))
        return false;

    if (x + w > 1.0f)
        return false;

    if (y + h > 1.0f)
        return false;

    m_viewport[0] = x;
    m_viewport[1] = y;
    m_viewport[2] = w;
    m_viewport[3] = h;

    return true;
}


//------------------------------------------------------------------------------
BrowserView::BrowserView(std::string path)
  : m_viewport(0.0f, 0.0f, 1.0f, 1.0f), m_extensionCode("")
{
    m_jsHandler = new JSV8Handler();
    m_extensionCode = FileToString(path, "script/aiagent.js");
}

//------------------------------------------------------------------------------
BrowserView::~BrowserView()
{
    CefDoMessageLoopWork();
    m_browser->GetHost()->CloseBrowser(true);

    m_browser = nullptr;
    m_client = nullptr;
}

//------------------------------------------------------------------------------
void BrowserView::load(const std::string &url)
{
    assert(m_initialized);
    m_browser->GetMainFrame()->LoadURL(url);
}

//------------------------------------------------------------------------------
void BrowserView::draw()
{
    CefDoMessageLoopWork();
    m_render_handler->draw(m_viewport);
}

//------------------------------------------------------------------------------
void BrowserView::reshape(int w, int h)
{
    m_render_handler->reshape(w, h);
    GLCHECK(glViewport(m_viewport[0],
                       m_viewport[1],
                       GLsizei(m_viewport[2] * w),
                       GLsizei(m_viewport[3] * h)));
    m_browser->GetHost()->WasResized();
}

void BrowserView::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();
    CefWindowInfo window_info;
    window_info.SetAsWindowless(0);

    m_render_handler = new RenderHandler(m_viewport);
    m_initialized = m_render_handler->init();
    m_render_handler->reshape(128, 128); // initial size

    m_load_handler = new LoadHandler();
    
    CefBrowserSettings browserSettings;
    browserSettings.windowless_frame_rate = 60; // 30 is default

    m_client = new BrowserClient(m_render_handler, m_load_handler);

    CefRefPtr<CefCommandLine> command_line =
        CefCommandLine::GetGlobalCommandLine();
    
    std::string url = command_line->GetSwitchValue("url");
    if (url.empty()) {
        url = "http://www.baidu.com";
    }
    m_browser = CefBrowserHost::CreateBrowserSync(window_info, m_client.get(),
                                                  url, browserSettings,
                                                  nullptr, nullptr);

}

void BrowserView::OnContextCreated(CefRefPtr<CefBrowser> browser,
				   CefRefPtr<CefFrame> frame,
				   CefRefPtr<CefV8Context> context) {

  
    CefRefPtr<CefV8Value> object = context->GetGlobal();
    
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("testfunc", m_jsHandler);

    object->SetValue("testfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);
}

void BrowserView::OnWebKitInitialized() {
    // Register the extension.
    if(m_extensionCode.length()>0){
        CefRegisterExtension("v8/ai", m_extensionCode, nullptr);
    }
}
