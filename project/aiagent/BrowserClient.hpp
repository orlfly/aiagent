#ifndef BROWSERCLIENT_HPP
#define BROWSERCLIENT_HPP

#include <include/cef_client.h>
#include <string>
#include <iostream>
#include "openai.hpp"
#include "json.hpp"

using json = nlohmann::json;
// *************************************************************************
//! \brief Provide access to browser-instance-specific callbacks. A single
//! CefClient instance can be shared among any number of browsers.
// *************************************************************************
class BrowserClient: public CefClient
{
public:

    BrowserClient(CefRefPtr<OpenAI> openai,
		  CefRefPtr<CefRenderHandler> ptr,
		  CefRefPtr<CefLoadHandler> loadptr,
		  CefRefPtr<CefDisplayHandler> displayptr,
		  CefRefPtr<CefLifeSpanHandler> lifespanptr);
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual bool OnProcessMessageReceived( CefRefPtr<CefBrowser> browser,
					   CefRefPtr<CefFrame> frame,
					   CefProcessId source_process,
					   CefRefPtr< CefProcessMessage > message ) override;
    void ScriptCompletionCallback(CefRefPtr<CefBrowser> browser, const json& msg);
private:
    std::string CalEleDesc(json &data);
    CefRefPtr<OpenAI> m_openai;
    CefRefPtr<CefRenderHandler> m_renderHandler;
    CefRefPtr<CefLoadHandler> m_loadHandler;
    CefRefPtr<CefDisplayHandler> m_displayHandler;
    CefRefPtr<CefLifeSpanHandler> m_lifeSpanHandler;


    IMPLEMENT_REFCOUNTING(BrowserClient);
};

#endif // BROWSERVIEW_HPP
