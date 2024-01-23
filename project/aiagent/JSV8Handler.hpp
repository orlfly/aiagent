#ifndef JSV8HANDLER_HPP
#define JSV8HANDLER_HPP

#include "include/cef_v8.h"
#include <iostream>

class JSV8Handler : public CefV8Handler {
private:
    CefRefPtr<CefBrowser> m_browser;
public:
    JSV8Handler(CefRefPtr<CefBrowser> browser);      
    virtual bool Execute(const CefString& name,
			 CefRefPtr<CefV8Value> object,
			 const CefV8ValueList& arguments,
			 CefRefPtr<CefV8Value>& retval,
			 CefString& exception) override;
    // Provide the reference counting implementation for this class.
    IMPLEMENT_REFCOUNTING(JSV8Handler);
};

#endif // JSV8HANDLER_HPP
