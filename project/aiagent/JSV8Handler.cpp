#include "JSV8Handler.hpp"

bool JSV8Handler::Execute(const CefString& name,
			  CefRefPtr<CefV8Value> object,
			  const CefV8ValueList& arguments,
			  CefRefPtr<CefV8Value>& retval,
			  CefString& exception)
{
    if (name == "testfunc") {
        retval = CefV8Value::CreateString("test!!!!");
	return true;
    }
    return false;
}
