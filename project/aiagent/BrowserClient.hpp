#ifndef BROWSERCLIENT_HPP
#define BROWSERCLIENT_HPP

#include <include/cef_client.h>
#include <string>
#include <iostream>
#include <memory>
#include <queue>
#include "openai.hpp"
#include "json.hpp"
#include "BrowserTask.hpp"
#include "LoadHandler.hpp"
#include <map>

using json = nlohmann::json;
// *************************************************************************
//! \brief Provide access to browser-instance-specific callbacks. A single
//! CefClient instance can be shared among any number of browsers.
// *************************************************************************
class BrowserView;
class BrowserClient: public CefClient
{
public:

    BrowserClient(CefRefPtr<OpenAI> openai,
		  CefRefPtr<BrowserView> browser,
		  CefRefPtr<CefRenderHandler> ptr,
		  CefRefPtr<LoadHandler> loadptr,
		  CefRefPtr<CefLifeSpanHandler> lifespanptr);
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual bool OnProcessMessageReceived( CefRefPtr<CefBrowser> browser,
					   CefRefPtr<CefFrame> frame,
					   CefProcessId source_process,
					   CefRefPtr< CefProcessMessage > message ) override;
    void TaskCompletionCallback(CefRefPtr<CefBrowser> browser, const std::string content, const json& msg);
    void EvaluateCompletionCallback(CefRefPtr<CefBrowser> browser,
				    const std::string site,
				    const std::string problem,
				    const std::string step,
				    const std::string content,
				    const json& msg);
    void ScriptCompletionCallback(CefRefPtr<CefBrowser> browser, const json& msg);
    void SummaryCompletionCallback(CefRefPtr<CefBrowser> browser, const json& msg);
private:
    std::string CalEleDesc(json &data);
    CefRefPtr<OpenAI> m_openai;
    CefRefPtr<BrowserView> m_browser;
    CefRefPtr<CefRenderHandler> m_renderHandler;
    CefRefPtr<LoadHandler> m_loadHandler;
    CefRefPtr<CefLifeSpanHandler> m_lifeSpanHandler;

    std::string m_currentStep;
    std::string m_currentProblem;
    IMPLEMENT_REFCOUNTING(BrowserClient);
};

#endif // BROWSERVIEW_HPP
