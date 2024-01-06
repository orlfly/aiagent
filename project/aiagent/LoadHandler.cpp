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
    std::string cmd = "var iterator = document.createNodeIterator(document,NodeFilter.SHOW_ELEMENT,null,false)\n"
        "var node = iterator.nextNode()\n"
        "while(node !== null) {\n"
        "  console.log(node.tagName)\n"
        "  node = iterator.nextNode()\n"
        "}\n"
        "var testval=window.testfunc()\n"
        "console.log(testval)\n";
    frame->ExecuteJavaScript(cmd, frame->GetURL(), 0);
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
