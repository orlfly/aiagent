#ifndef BROWSERCLIENT_HPP
#define BROWSERCLIENT_HPP

#include <include/cef_client.h>
#include <string>
#include <iostream>
#include "openai.hpp"

// *************************************************************************
//! \brief Provide access to browser-instance-specific callbacks. A single
//! CefClient instance can be shared among any number of browsers.
// *************************************************************************
class BrowserClient: public CefClient
{
public:

    BrowserClient(CefRefPtr<CefRenderHandler> ptr,
		  CefRefPtr<CefLoadHandler> loadptr);
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual bool OnProcessMessageReceived( CefRefPtr< CefBrowser > browser,
					   CefRefPtr< CefFrame > frame,
					   CefProcessId source_process,
					   CefRefPtr< CefProcessMessage > message ) override;

    CefRefPtr<CefRenderHandler> m_renderHandler;
    CefRefPtr<CefLoadHandler> m_loadHandler;
    CefRefPtr<OpenAI> m_openai;

    IMPLEMENT_REFCOUNTING(BrowserClient);
};

#endif // BROWSERVIEW_HPP
