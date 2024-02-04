#ifndef LOADHANDLER_HPP
#define LOADHANDLER_HPP

#include <iostream>

#include "include/cef_load_handler.h"
#include "include/cef_display_handler.h"

class LoadHandler: public CefLoadHandler,
		   public CefDisplayHandler
{
public:

  LoadHandler();

  //! \brief
  ~LoadHandler();

  virtual void OnLoadEnd( CefRefPtr< CefBrowser > browser,
			  CefRefPtr< CefFrame > frame, int httpStatusCode ) override;

  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
			   CefRefPtr<CefFrame> frame,
			   ErrorCode errorCode,
			   const CefString& errorText,
			   const CefString& failedUrl) override;

  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
			   CefRefPtr<CefFrame> frame,
			   TransitionType transition_type) override;
  virtual void OnLoadingProgressChange( CefRefPtr<CefBrowser> browser, double progress ) override;
  
  virtual void OnLoadingStateChange (CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) override;

  virtual void OnStatusMessage (CefRefPtr<CefBrowser> browser, const CefString &value) override;

  double GetProgress(){return m_progress;}
private:
  double m_progress;
  //! \brief CefBase interface
  IMPLEMENT_REFCOUNTING(LoadHandler);

};

#endif //LOADHANDLER_HPP
