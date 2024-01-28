#include "DisplayHandler.hpp"
#include <iostream>
//------------------------------------------------------------------------------
DisplayHandler::DisplayHandler()
  :m_progress(1.0f)
{
}
//------------------------------------------------------------------------------
DisplayHandler::~DisplayHandler()
{
}

void DisplayHandler::OnLoadingProgressChange( CefRefPtr< CefBrowser > browser, double progress )
{
    if( (1.0f-m_progress) > 0.00001f && (1.0f-progress) < 0.00001f)
    {
        std::string cmd = "aiagent.visualBox();";
	browser->GetMainFrame()->ExecuteJavaScript(cmd, browser->GetMainFrame()->GetURL(), 0);
    }
    m_progress = progress;
}
