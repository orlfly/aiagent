#include "JSV8Handler.hpp"
#include <iostream>
#include "openai.hpp"
#include "include/base/cef_logging.h"

JSV8Handler::JSV8Handler(CefRefPtr<CefBrowser> browser,
			 CefRefPtr<LoadHandler> load_handler)
  :m_browser(browser), m_load_handler(load_handler){
}
bool JSV8Handler::Execute(const CefString& name,
			  CefRefPtr<CefV8Value> object,
			  const CefV8ValueList& arguments,
			  CefRefPtr<CefV8Value>& retval,
			  CefString& exception)
{
    LOG(INFO)<<"javascript function call:" <<name<<std::endl;
    if (name == "ExecuteTask") {
        CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ExecutFinish");
	CefRefPtr<CefListValue> args = msg->GetArgumentList();
	args->SetString(0, arguments[0]->GetStringValue());
	args->SetString(1, arguments[1]->GetStringValue());
	args->SetString(2, arguments[2]->GetStringValue());

	context->GetFrame()->SendProcessMessage(PID_BROWSER, msg);
	return true;
    }else if (name == "EvaluateResult") {
        CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("EvaluateFinish");
	CefRefPtr<CefListValue> args = msg->GetArgumentList();
	args->SetString(0, arguments[0]->GetStringValue());
	args->SetString(1, arguments[1]->GetStringValue());
	args->SetString(2, arguments[2]->GetStringValue());

	context->GetFrame()->SendProcessMessage(PID_BROWSER, msg);
	return true;
    }else if (name == "TaskTimeout") {
        CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
        int tid = arguments[0]->GetIntValue();
        if(1.0f-m_load_handler->GetProgress() > 0.00001)
	{
	    std::stringstream cmd;
	    cmd<<"aiagent.taskTimer("<<tid<<");";
	    LOG(INFO)<<"continue set timeout...." <<name<<std::endl;
	    context->GetFrame()->ExecuteJavaScript(cmd.str(), context->GetFrame()->GetURL(), 0);
	    return true;
	}else{
	    CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
	    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("Timeout");
	    CefRefPtr<CefListValue> args = msg->GetArgumentList();
	    args->SetInt(0, tid);

	    context->GetFrame()->SendProcessMessage(PID_BROWSER, msg);
	    return true;
	}
    }else if (name == "LLMSummary") {
        CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("Summary");
	CefRefPtr<CefListValue> args = msg->GetArgumentList();
	args->SetString(0, arguments[0]->GetStringValue());
	args->SetString(1, arguments[1]->GetStringValue());

	context->GetFrame()->SendProcessMessage(PID_BROWSER, msg);
    }
    return false;
}
