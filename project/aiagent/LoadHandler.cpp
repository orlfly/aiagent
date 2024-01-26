#include "LoadHandler.hpp"

//------------------------------------------------------------------------------
LoadHandler::LoadHandler()
{
}
//------------------------------------------------------------------------------
LoadHandler::~LoadHandler()
{
}

//------------------------------------------------------------------------------
void LoadHandler::OnLoadEnd( CefRefPtr< CefBrowser > browser,
			     CefRefPtr< CefFrame > frame, int httpStatusCode )
{
    
}

//------------------------------------------------------------------------------
void LoadHandler::OnLoadError( CefRefPtr<CefBrowser> browser,
			       CefRefPtr<CefFrame> frame,
			       ErrorCode errorCode,
			       const CefString& errorText,
			       const CefString& failedUrl)
{
}

//------------------------------------------------------------------------------
void LoadHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
			      CefRefPtr<CefFrame> frame,
			      TransitionType transition_type)
{
}
