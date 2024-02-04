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
class BrowserClient: public CefClient
{
public:

    BrowserClient(CefRefPtr<OpenAI> openai,
		  std::shared_ptr<std::queue<std::shared_ptr<BrowserTask>>> taskQue,
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
    void ScriptCompletionCallback(CefRefPtr<CefBrowser> browser, int nexttask, const json& msg);
    void ResultCompletionCallback(CefRefPtr<CefBrowser> browser, std::string content, const json& msg);
private:
  void DoTask(CefRefPtr<CefBrowser> browser, std::string content);
    std::string CalEleDesc(json &data);
    CefRefPtr<OpenAI> m_openai;
    CefRefPtr<CefRenderHandler> m_renderHandler;
    CefRefPtr<LoadHandler> m_loadHandler;
    CefRefPtr<CefLifeSpanHandler> m_lifeSpanHandler;

    std::shared_ptr<std::queue<std::shared_ptr<BrowserTask>>> m_taskQue = nullptr;

    std::map<int,std::shared_ptr<BrowserTask>> m_taskMap;
    std::string m_currentOp;

    IMPLEMENT_REFCOUNTING(BrowserClient);
};

#endif // BROWSERVIEW_HPP
