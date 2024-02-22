#include "LoadHandler.hpp"
#include "include/base/cef_logging.h"

//------------------------------------------------------------------------------
LoadHandler::LoadHandler()
  :m_progress(1.0f)
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
    LOG(INFO)<<"----------------------------http:"<<httpStatusCode<<std::endl;

}

//------------------------------------------------------------------------------
void LoadHandler::OnLoadError( CefRefPtr<CefBrowser> browser,
			       CefRefPtr<CefFrame> frame,
			       ErrorCode errorCode,
			       const CefString& errorText,
			       const CefString& failedUrl)
{
    LOG(ERROR)<<"Load error:"<<errorCode <<"  "<<errorText<<std::endl;
}

//------------------------------------------------------------------------------
void LoadHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
			      CefRefPtr<CefFrame> frame,
			      TransitionType transition_type)
{
    LOG(INFO)<<"-----------------------------start type:"<<transition_type<<std::endl;
}

void LoadHandler::OnLoadingProgressChange( CefRefPtr< CefBrowser > browser, double progress )
{

    LOG(INFO)<<"loading m_progress:"<<m_progress<<" progress:"<<progress<<std::endl;
    if( (1.0f-m_progress) > 0.00001f && (1.0f-progress) < 0.00001f)
    {
        LOG(INFO)<<"Get html content..."<<std::endl;
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ContentLoaded");
	browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, msg);
    }
    m_progress = progress;
}

void LoadHandler::OnLoadingStateChange (CefRefPtr< CefBrowser > browser, bool isLoading, bool canGoBack, bool canGoForward)
{
    LOG(INFO)<<"load ----------------------------is loading:"<<isLoading<<std::endl;
}

void LoadHandler::OnStatusMessage (CefRefPtr< CefBrowser > browser, const CefString &value)
{
    LOG(INFO)<<"load ----------------------------status:"<<value<<std::endl;
}
