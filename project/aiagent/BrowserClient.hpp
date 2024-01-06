#ifndef BROWSERCLIENT_HPP
#define BROWSERCLIENT_HPP

#include <include/cef_client.h>

// *************************************************************************
//! \brief Provide access to browser-instance-specific callbacks. A single
//! CefClient instance can be shared among any number of browsers.
// *************************************************************************
class BrowserClient: public CefClient
{
public:

  BrowserClient(CefRefPtr<CefRenderHandler> ptr,
		CefRefPtr<CefLoadHandler> loadptr)
    : m_renderHandler(ptr),
      m_loadHandler(loadptr)
  {}

  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override
  {
    return m_renderHandler;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override
  {
    return m_loadHandler;
  }

  CefRefPtr<CefRenderHandler> m_renderHandler;
  CefRefPtr<CefLoadHandler> m_loadHandler;

  IMPLEMENT_REFCOUNTING(BrowserClient);
};

#endif // BROWSERVIEW_HPP
