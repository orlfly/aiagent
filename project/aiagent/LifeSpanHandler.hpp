#ifndef LIFESPANHANDLER_HPP
#define LIFESPANHANDLER_HPP

#include <iostream>
#include "include/cef_life_span_handler.h"


class LifeSpanHandler: public CefLifeSpanHandler
{
public:

  LifeSpanHandler();

  //! \brief
  ~LifeSpanHandler();

  virtual bool 	OnBeforePopup (CefRefPtr<CefBrowser> browser,
			       CefRefPtr<CefFrame> frame,
			       const CefString &target_url,
			       const CefString &target_frame_name,
			       WindowOpenDisposition target_disposition,
			       bool user_gesture,
			       const CefPopupFeatures &popupFeatures,
			       CefWindowInfo &windowInfo,
			       CefRefPtr<CefClient> &client,
			       CefBrowserSettings &settings,
			       CefRefPtr<CefDictionaryValue> &extra_info,
			       bool *no_javascript_access) override;
private:
  //! \brief CefBase interface
  IMPLEMENT_REFCOUNTING(LifeSpanHandler);

};

#endif
