#include "LifeSpanHandler.hpp"
#include <iostream>
//------------------------------------------------------------------------------
LifeSpanHandler::LifeSpanHandler()
{
}
//------------------------------------------------------------------------------
LifeSpanHandler::~LifeSpanHandler()
{
}
bool LifeSpanHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
				    bool *no_javascript_access)
{
    if(!target_url.empty())
    {
        switch (target_disposition)
	  {
	  case CEF_WOD_NEW_FOREGROUND_TAB:
	  case CEF_WOD_NEW_BACKGROUND_TAB:
	  case CEF_WOD_NEW_POPUP:
	  case CEF_WOD_NEW_WINDOW:
	      browser->GetMainFrame()->LoadURL(target_url);
	  default:
	      return true;
	  }
    }
    return true;
}
