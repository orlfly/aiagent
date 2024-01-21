#include "JSV8Handler.hpp"
#include <iostream>

bool JSV8Handler::Execute(const CefString& name,
			  CefRefPtr<CefV8Value> object,
			  const CefV8ValueList& arguments,
			  CefRefPtr<CefV8Value>& retval,
			  CefString& exception)
{
    if (name == "aiprint") {
        for (CefV8ValueList::const_iterator it = arguments.begin(); it != arguments.end(); ++it) {
	    CefRefPtr<CefV8Value> v8Value = *it;
	    std::cout << v8Value; 
	}
	return true;
    }
    return false;
}
