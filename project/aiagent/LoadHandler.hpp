#ifndef LOADHANDLER_HPP
#define LOADHANDLER_HPP

#include <iostream>

#include "include/cef_load_handler.h"

class LoadHandler: public CefLoadHandler
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

  //! \brief CefBase interface
  IMPLEMENT_REFCOUNTING(LoadHandler);

};

#endif //LOADHANDLER_HPP
