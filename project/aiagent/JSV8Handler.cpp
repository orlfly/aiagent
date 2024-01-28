#include "JSV8Handler.hpp"
#include <iostream>
#include "openai.hpp"
#include "include/base/cef_logging.h"

JSV8Handler::JSV8Handler(CefRefPtr<CefBrowser> browser) {
    m_browser = browser;
}
bool JSV8Handler::Execute(const CefString& name,
			  CefRefPtr<CefV8Value> object,
			  const CefV8ValueList& arguments,
			  CefRefPtr<CefV8Value>& retval,
			  CefString& exception)
{
    LOG(INFO)<<"javascript function call:" <<name<<std::endl;
    if (name == "HtmlContentHandler") {
        CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("HtmlContent");
	CefRefPtr<CefListValue> args = msg->GetArgumentList();
	args->SetString(0, arguments[0]->GetStringValue());

	context->GetFrame()->SendProcessMessage(PID_BROWSER, msg);
	return true;
    }
    return false;
}
